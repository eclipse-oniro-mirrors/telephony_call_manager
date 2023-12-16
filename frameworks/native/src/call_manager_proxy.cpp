/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "call_manager_proxy.h"

#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
#include "core_manager_inner.h"
#endif

namespace OHOS {
namespace Telephony {
CallManagerProxy::CallManagerProxy()
    : systemAbilityId_(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID), registerStatus_(false), initStatus_(false)
{}

CallManagerProxy::~CallManagerProxy() = default;

#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
bool CallManagerProxy::IsServiceStart()
{
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("GetSystemAbilityManager failed!");
        return false;
    }
    if (managerPtr->CheckSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID) == nullptr) {
        return false;
    }
    return true;
}

void CallManagerProxy::SetInitState(bool status)
{
    initStatus_ = status;
}

std::unique_ptr<CallManagerCallback> CallManagerProxy::GetCallBack()
{
    return std::move(callBack_);
}
#endif

void CallManagerProxy::Init(int32_t systemAbilityId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (initStatus_) {
        TELEPHONY_LOGW("you have already initialized");
        return;
    }
    systemAbilityId_ = systemAbilityId;
#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
    if (!IsServiceStart()) {
        EventFwk::MatchingSkills matchingSkills;
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_RADIO_STATE_CHANGE);
        EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
        subscriberInfo.SetThreadMode(EventFwk::CommonEventSubscribeInfo::COMMON);
        std::shared_ptr<CallManagerProxySubcribed> subscriberPtr =
            std::make_shared<CallManagerProxySubcribed>(subscriberInfo);

        if (subscriberPtr == nullptr) {
            TELEPHONY_LOGE("CallManagerProxy::Init subscriberPtr is nullptr");
            return;
        }

        bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberPtr);
        TELEPHONY_LOGI("CallManagerProxy::Init subscribeResult = %{public}d", subscribeResult);
        return;
    }
#endif
    int32_t result = ConnectService();
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("connect service failed,errCode: %{public}d", result);
        return;
    }
    initStatus_ = true;
    TELEPHONY_LOGI("connected to call manager service successfully!");
}

#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
CallManagerProxy::CallManagerProxySubcribed::CallManagerProxySubcribed(
    const EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{}

void CallManagerProxy::CallManagerProxySubcribed::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    std::shared_ptr<CallManagerProxy> proxy = DelayedSingleton<CallManagerProxy>::GetInstance();
    if (proxy == nullptr) {
        TELEPHONY_LOGE("proxy is nullptr");
        return;
    }
    int32_t result = proxy->ConnectService();
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("OnReceiveEvent connect service failed,errCode: %{public}d", result);
        return;
    }
    result = proxy->RegisterCallBack(proxy->GetCallBack());
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("OnReceiveEvent register callback failed,errCode: %{public}d", result);
        return;
    }
    proxy->SetInitState(true);
    TELEPHONY_LOGI("OnReceiveEvent connected to call manager service successfully!");
    bool unsubscribeResult = EventFwk::CommonEventManager::UnSubscribeCommonEvent(shared_from_this());
    TELEPHONY_LOGI("OnReceiveEvent UnSubscribeCommonEvent unsubscribeResult: %{public}d", unsubscribeResult);
}
#endif

void CallManagerProxy::UnInit()
{
    DisconnectService();
    std::lock_guard<std::mutex> lock(mutex_);
    initStatus_ = false;
    TELEPHONY_LOGI("disconnect service\n");
}

/**
 * Register a callback with the CallManager, through which the CallManager's data is notified to the NAPI.
 */
int32_t CallManagerProxy::RegisterCallBack(std::unique_ptr<CallManagerCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (registerStatus_) {
        TELEPHONY_LOGE("you have already register callback yet!");
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    }
    if (callManagerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callManagerServicePtr_ is null");
#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
        callBack_ = std::move(callback);
#endif
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callAbilityCallbackPtr_ = new (std::nothrow) CallAbilityCallback();
    if (callAbilityCallbackPtr_ == nullptr) {
        TELEPHONY_LOGE("create CallAbilityCallback object failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = callManagerServicePtr_->RegisterCallBack(callAbilityCallbackPtr_);
    if (ret != TELEPHONY_SUCCESS) {
        callAbilityCallbackPtr_.clear();
        callAbilityCallbackPtr_ = nullptr;
        TELEPHONY_LOGE("register callback to call manager service failed,result: %{public}d", ret);
        if (ret == TELEPHONY_ERR_PERMISSION_ERR) {
            return TELEPHONY_ERR_PERMISSION_ERR;
        } else if (ret == TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API) {
            return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
        }
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    }
    TELEPHONY_LOGI("register call ability callback success!");
    callAbilityCallbackPtr_->SetProcessCallback(std::move(callback));
    registerStatus_ = true;
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::UnRegisterCallBack()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!registerStatus_) {
        TELEPHONY_LOGE("you haven't register callback yet, please RegisterCallBack first!");
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    }
    if (callManagerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callManagerServicePtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = callManagerServicePtr_->UnRegisterCallBack();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UnRegisterCallBack failed,result: %{public}d", ret);
        if (ret == TELEPHONY_ERR_PERMISSION_ERR) {
            return TELEPHONY_ERR_PERMISSION_ERR;
        } else if (ret == TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API) {
            return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
        }
        return TELEPHONY_ERR_UNREGISTER_CALLBACK_FAIL;
    }
    if (callAbilityCallbackPtr_ == nullptr) {
        TELEPHONY_LOGE("callAbilityCallbackPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callAbilityCallbackPtr_.clear();
    callAbilityCallbackPtr_ = nullptr;
    registerStatus_ = false;
    TELEPHONY_LOGI("UnRegisterCallBack success!");
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::ConnectService()
{
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
    if (callManagerServicePtr_ != nullptr) {
        return TELEPHONY_SUCCESS;
    }
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("GetSystemAbilityManager failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    sptr<ICallManagerService> callManagerServicePtr = nullptr;
    sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(systemAbilityId_);
    if (iRemoteObjectPtr == nullptr) {
        TELEPHONY_LOGE("GetSystemAbility failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }

    std::unique_ptr<CallManagerServiceDeathRecipient> recipient =
        std::make_unique<CallManagerServiceDeathRecipient>(*this);
    if (recipient == nullptr) {
        TELEPHONY_LOGE("recipient is null");
        return TELEPHONY_ERROR;
    }
    sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
    if ((iRemoteObjectPtr->IsProxyObject()) && (!iRemoteObjectPtr->AddDeathRecipient(dr))) {
        TELEPHONY_LOGE("Failed to add death recipient");
        return TELEPHONY_ERROR;
    }
    callManagerServicePtr = iface_cast<ICallManagerService>(iRemoteObjectPtr);
    if (!callManagerServicePtr) {
        TELEPHONY_LOGE("iface_cast<ICallManagerService> failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callManagerServicePtr_ = callManagerServicePtr;
    deathRecipient_ = dr;
    return TELEPHONY_SUCCESS;
}

void CallManagerProxy::DisconnectService()
{
    UnRegisterCallBack();
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
    if (callManagerServicePtr_ != nullptr) {
        callManagerServicePtr_.clear();
        callManagerServicePtr_ = nullptr;
    }
}

int32_t CallManagerProxy::ReConnectService()
{
    if (callManagerServicePtr_ == nullptr) {
        TELEPHONY_LOGI("try to reconnect call manager service now...");
        int32_t result = ConnectService();
        if (result != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("Connect service: %{public}d", result);
            return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
        }
        if (registerStatus_) {
            ReRegisterCallBack();
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::ReRegisterCallBack()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callManagerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callManagerServicePtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (callAbilityCallbackPtr_ == nullptr) {
        TELEPHONY_LOGE("create CallAbilityCallback object failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = callManagerServicePtr_->RegisterCallBack(callAbilityCallbackPtr_);
    if (ret != TELEPHONY_SUCCESS) {
        callAbilityCallbackPtr_.clear();
        callAbilityCallbackPtr_ = nullptr;
        TELEPHONY_LOGE("register callback to call manager service failed,result: %{public}d", ret);
        if (ret == TELEPHONY_ERR_PERMISSION_ERR) {
            return TELEPHONY_ERR_PERMISSION_ERR;
        }
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    }
    TELEPHONY_LOGI("register call ability callback again success!");
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::RegisterVoipCallManagerCallback()
{
    if (callManagerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callManagerServicePtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = callManagerServicePtr_->RegisterVoipCallManagerCallback();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("register callback to call manager service failed,result: %{public}d", ret);
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::UnRegisterVoipCallManagerCallback()
{
    if (callManagerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callManagerServicePtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = callManagerServicePtr_->UnRegisterVoipCallManagerCallback();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("unregister callback to call manager service failed,result: %{public}d", ret);
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::DialCall(std::u16string number, AppExecFwk::PacMap &extras)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->DialCall(number, extras);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("DialCall failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::AnswerCall(int32_t callId, int32_t videoState)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->AnswerCall(callId, videoState);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("AnswerCall failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::RejectCall(int32_t callId, bool isSendSms, std::u16string content)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->RejectCall(callId, isSendSms, content);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RejectCall failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::HangUpCall(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->HangUpCall(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HangUpCall failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetCallState()
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetCallState();
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCallState failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::HoldCall(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->HoldCall(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HoldCall failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::UnHoldCall(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->UnHoldCall(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UnHoldCall failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SwitchCall(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SwitchCall(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SwitchCall failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::CombineConference(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->CombineConference(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CombineConference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SeparateConference(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SeparateConference(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SeparateConference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::KickOutFromConference(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->KickOutFromConference(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("KickOutFromConference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetMainCallId(int32_t &callId, int32_t &mainCallId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetMainCallId(callId, mainCallId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetMainCallId failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetSubCallIdList(callId, callIdList);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetMainCallId failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetCallIdListForConference(callId, callIdList);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetMainCallId failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetCallWaiting(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetCallWaiting(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetCallWaiting(int32_t slotId, bool activate)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetCallWaiting(slotId, activate);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetCallRestriction(slotId, type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetCallRestriction(slotId, info);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetCallRestrictionPassword(
    int32_t slotId, CallRestrictionType fac, const char *oldPassword, const char *newPassword)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetCallRestrictionPassword(slotId, fac, oldPassword, newPassword);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetCallTransferInfo(slotId, type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetCallTransferInfo(slotId, info);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::CanSetCallTransferTime(int32_t slotId, bool &result)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->CanSetCallTransferTime(slotId, result);
}

int32_t CallManagerProxy::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetCallPreferenceMode(slotId, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::StartDtmf(int32_t callId, char str)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->StartDtmf(callId, str);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StartDtmf failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::StopDtmf(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->StopDtmf(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StopDtmf failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::PostDialProceed(int32_t callId, bool proceed)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->PostDialProceed(callId, proceed);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PostDialProceed failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::IsRinging(bool &enabled)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->IsRinging(enabled);
}

bool CallManagerProxy::HasCall()
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->HasCall();
}

int32_t CallManagerProxy::IsNewCallAllowed(bool &enabled)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->IsNewCallAllowed(enabled);
}

int32_t CallManagerProxy::IsInEmergencyCall(bool &enabled)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->IsInEmergencyCall(enabled);
}

int32_t CallManagerProxy::IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, bool &enabled)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->IsEmergencyPhoneNumber(number, slotId, enabled);
}

int32_t CallManagerProxy::FormatPhoneNumber(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->FormatPhoneNumber(number, countryCode, formatNumber);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("FormatPhoneNumber failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::FormatPhoneNumberToE164(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->FormatPhoneNumberToE164(number, countryCode, formatNumber);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("FormatPhoneNumberToE164 failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetMuted(bool isMute)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetMuted(isMute);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetMute failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::MuteRinger()
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->MuteRinger();
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("MuteRinger failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetAudioDevice(const AudioDevice &audioDevice)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetAudioDevice(audioDevice);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetAudioDevice failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::ControlCamera(int32_t callId, std::u16string &cameraId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->ControlCamera(callId, cameraId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ControlCamera failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetPreviewWindow(int32_t callId, std::string &surfaceId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = TELEPHONY_SUCCESS;
    if (surfaceId.empty() || surfaceId[0] < '0' || surfaceId[0] > '9') {
        TELEPHONY_LOGI("surfaceId is invalid, set surface nullptr");
        surfaceId = "";
        errCode = callManagerServicePtr_->SetPreviewWindow(callId, surfaceId, nullptr);
    } else {
        uint64_t previewSurfaceId = std::stoull(surfaceId);
        auto surface = SurfaceUtils::GetInstance()->GetSurface(previewSurfaceId);
        if (surface == nullptr) {
            TELEPHONY_LOGI("surface is null");
            surfaceId = "";
        }
        errCode = callManagerServicePtr_->SetPreviewWindow(callId, surfaceId, surface);
    }
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreviewWindow failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetDisplayWindow(int32_t callId, std::string &surfaceId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = TELEPHONY_SUCCESS;
    if (surfaceId.empty() || surfaceId[0] < '0' || surfaceId[0] > '9') {
        TELEPHONY_LOGI("surfaceId is invalid, set surface nullptr");
        surfaceId = "";
        errCode = callManagerServicePtr_->SetDisplayWindow(callId, surfaceId, nullptr);
    } else {
        uint64_t displaySurfaceId = std::stoull(surfaceId);
        auto surface = SurfaceUtils::GetInstance()->GetSurface(displaySurfaceId);
        if (surface == nullptr) {
            TELEPHONY_LOGI("surface is null");
            surfaceId = "";
        }
        errCode = callManagerServicePtr_->SetDisplayWindow(callId, surfaceId, surface);
    }
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetDisplayWindow failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetCameraZoom(float zoomRatio)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetCameraZoom(zoomRatio);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCameraZoom failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetPausePicture(int32_t callId, std::u16string &path)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetPausePicture(callId, path);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPausePicture failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetDeviceDirection(int32_t callId, int32_t rotation)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetDeviceDirection(callId, rotation);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetDeviceDirection failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetImsConfig(int32_t slotId, ImsConfigItem item)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetImsConfig(slotId, item);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetImsConfig(slotId, item, value);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetImsFeatureValue(int32_t slotId, FeatureType type)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetImsFeatureValue(slotId, type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetImsFeatureValue(slotId, type, value);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::UpdateImsCallMode(int32_t callId, ImsCallMode mode)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->UpdateImsCallMode(callId, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateImsCallMode failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::EnableImsSwitch(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->EnableImsSwitch(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::DisableImsSwitch(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->DisableImsSwitch(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::IsImsSwitchEnabled(int32_t slotId, bool &enabled)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->IsImsSwitchEnabled(slotId, enabled);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetVoNRState(int32_t slotId, int32_t state)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetVoNRState(slotId, state);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetVoNRState(int32_t slotId, int32_t &state)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetVoNRState(slotId, state);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::StartRtt(int32_t callId, std::u16string &msg)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->StartRtt(callId, msg);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StartRtt failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::StopRtt(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->StopRtt(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StopRtt failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::JoinConference(int32_t callId, std::vector<std::u16string> &numberList)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->JoinConference(callId, numberList);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("JoinConference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::ReportOttCallDetailsInfo(std::vector<OttCallDetailsInfo> &ottVec)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->ReportOttCallDetailsInfo(ottVec);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ReportOttCallInfo failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::ReportOttCallEventInfo(OttCallEventInfo &eventInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->ReportOttCallEventInfo(eventInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ReportOttCallEventInfo failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::CloseUnFinishedUssd(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->CloseUnFinishedUssd(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed, errcode:%{public}d", slotId, errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::InputDialerSpecialCode(const std::string &specialCode)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->InputDialerSpecialCode(specialCode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("InputDialerSpecialCode failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::RemoveMissedIncomingCallNotification()
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->RemoveMissedIncomingCallNotification();
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RemoveMissedIncomingCallNotification failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetVoIPCallState(int32_t state)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetVoIPCallState(state);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetVoIPCallState(int32_t &state)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetVoIPCallState(state);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

sptr<IRemoteObject> CallManagerProxy::GetProxyObjectPtr(CallManagerProxyType proxyType)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    sptr<IRemoteObject> ptr = callManagerServicePtr_->GetProxyObjectPtr(proxyType);
    if (ptr == nullptr) {
        TELEPHONY_LOGE("GetProxyObjectPtr failed");
        return nullptr;
    }
    return ptr;
}

void CallManagerProxy::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    if (remote == nullptr) {
        TELEPHONY_LOGE("OnRemoteDied failed, remote is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (callManagerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("OnRemoteDied failed, callManagerServicePtr_ is nullptr");
        return;
    }
    auto serviceRemote = callManagerServicePtr_->AsObject();
    if (serviceRemote != nullptr && serviceRemote == remote.promote()) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        callManagerServicePtr_ = nullptr;
        initStatus_ = false;
        TELEPHONY_LOGE("on remote died");
    }
}

int32_t CallManagerProxy::ReportAudioDeviceInfo()
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->ReportAudioDeviceInfo();
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ReportAudioDeviceInfo failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::CancelCallUpgrade(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->CancelCallUpgrade(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ReportAudioDeviceInfo failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::RequestCameraCapabilities(int32_t callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->RequestCameraCapabilities(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ReportAudioDeviceInfo failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
