/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Telephony {
CallManagerProxy::CallManagerProxy()
    : systemAbilityId_(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID), registerStatus_(false), initStatus_(false)
{}

CallManagerProxy::~CallManagerProxy()
{
    UnInit();
}

void CallManagerProxy::Init(int32_t systemAbilityId, std::u16string &bundleName)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (initStatus_) {
        TELEPHONY_LOGW("you have already initialized");
        return;
    }
    bundleName_ = bundleName;
    systemAbilityId_ = systemAbilityId;
    int32_t result = ConnectService();
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("connect service failed,errCode: %{public}d", result);
        Timer::start(CONNECT_SERVICE_WAIT_TIME, CallManagerProxy::task);
        return;
    }
    initStatus_ = true;
    TELEPHONY_LOGI("connected to call manager service successfully!");
}

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
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callAbilityCallbackPtr_ = (std::make_unique<CallAbilityCallback>()).release();
    if (callAbilityCallbackPtr_ == nullptr) {
        TELEPHONY_LOGE("create CallAbilityCallback object failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = callManagerServicePtr_->RegisterCallBack(callAbilityCallbackPtr_, bundleName_);
    if (ret != TELEPHONY_SUCCESS && ret != TELEPHONY_ERR_PERMISSION_ERR) {
        callAbilityCallbackPtr_.clear();
        callAbilityCallbackPtr_ = nullptr;
        TELEPHONY_LOGE("register callback to call manager service failed,result: %{public}d", ret);
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
    int32_t ret = callManagerServicePtr_->UnRegisterCallBack(bundleName_);
    if (ret != TELEPHONY_SUCCESS && ret != TELEPHONY_ERR_PERMISSION_ERR) {
        TELEPHONY_LOGE("UnRegisterCallBack failed,result: %{public}d", ret);
        return TELEPHONY_ERR_UNREGISTER_CALLBACK_FAIL;
    }
    if (callAbilityCallbackPtr_ == nullptr) {
        TELEPHONY_LOGE("callAbilityCallbackPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callAbilityCallbackPtr_->SetProcessCallback(nullptr);
    callAbilityCallbackPtr_.clear();
    callAbilityCallbackPtr_ = nullptr;
    registerStatus_ = false;
    TELEPHONY_LOGI("UnRegisterCallBack success!");
    return TELEPHONY_SUCCESS;
}

void CallManagerProxy::task()
{
    int32_t ret = DelayedSingleton<CallManagerProxy>::GetInstance()->ReConnectService();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("call manager service reconnection failed! errCode:%{public}d", ret);
        return;
    }
    DelayedSingleton<CallManagerProxy>::GetInstance()->ThreadExit();
    TELEPHONY_LOGI("call manager service reconnection successfully!");
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
    callManagerServicePtr = iface_cast<ICallManagerService>(iRemoteObjectPtr);
    if (!callManagerServicePtr) {
        TELEPHONY_LOGE("iface_cast<ICallManagerService> failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callManagerServicePtr_ = callManagerServicePtr;
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
    }
    if (registerStatus_) {
        ReRegisterCallBack();
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
    int32_t ret = callManagerServicePtr_->RegisterCallBack(callAbilityCallbackPtr_, bundleName_);
    if (ret != TELEPHONY_SUCCESS && ret != TELEPHONY_ERR_PERMISSION_ERR) {
        callAbilityCallbackPtr_.clear();
        callAbilityCallbackPtr_ = nullptr;
        TELEPHONY_LOGE("register callback to call manager service failed,result: %{public}d", ret);
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    }
    TELEPHONY_LOGI("register call ability callback again success!");
    return TELEPHONY_SUCCESS;
}

void CallManagerProxy::OnDeath()
{
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
    if (callManagerServicePtr_ != nullptr) {
        callManagerServicePtr_.clear();
        callManagerServicePtr_ = nullptr;
    }
    NotifyDeath();
}

void CallManagerProxy::NotifyDeath()
{
    Timer::start(CONNECT_SERVICE_WAIT_TIME, CallManagerProxy::task);
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

int32_t CallManagerProxy::GetMainCallId(int32_t &callId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetMainCallId(callId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetMainCallId failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

std::vector<std::u16string> CallManagerProxy::GetSubCallIdList(int32_t callId)
{
    std::vector<std::u16string> list;
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return list;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    list = callManagerServicePtr_->GetSubCallIdList(callId);
    return list;
}

std::vector<std::u16string> CallManagerProxy::GetCallIdListForConference(int32_t callId)
{
    std::vector<std::u16string> list;
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return list;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    list = callManagerServicePtr_->GetCallIdListForConference(callId);
    return list;
}

int32_t CallManagerProxy::GetCallWaiting(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetCallWaiting(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCallWaiting failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetCallWaiting(int32_t slotId, bool activate)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetCallWaiting(slotId, activate);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallWaiting failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetCallRestriction(slotId, type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCallRestriction failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetCallRestriction(slotId, info);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallRestriction failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetCallTransferInfo(slotId, type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCallTransferInfo failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetCallTransferInfo(slotId, info);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallTransferInfo failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetCallPreferenceMode(slotId, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallPreferenceMode failed, errcode:%{public}d", errCode);
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

bool CallManagerProxy::IsRinging()
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->IsRinging();
}

bool CallManagerProxy::HasCall()
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->HasCall();
}

bool CallManagerProxy::IsNewCallAllowed()
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->IsNewCallAllowed();
}

bool CallManagerProxy::IsInEmergencyCall()
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->IsInEmergencyCall();
}

bool CallManagerProxy::IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, int32_t &errorCode)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return callManagerServicePtr_->IsEmergencyPhoneNumber(number, slotId, errorCode);
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

int32_t CallManagerProxy::SetAudioDevice(AudioDevice deviceType)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetAudioDevice(deviceType);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetAudioDevice failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::ControlCamera(std::u16string cameraId, std::u16string callingPackage)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->ControlCamera(cameraId, callingPackage);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ControlCamera failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetPreviewWindow(VideoWindow &window)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetPreviewWindow(window);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreviewWindow failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetDisplayWindow(VideoWindow &window)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetDisplayWindow(window);
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

int32_t CallManagerProxy::SetPausePicture(std::u16string path)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetPausePicture(path);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPausePicture failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetDeviceDirection(int32_t rotation)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetDeviceDirection(rotation);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetDeviceDirection failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetImsConfig(int32_t slotId, ImsConfigItem item)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetImsConfig(slotId, item);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetImsConfig failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetImsConfig(slotId, item, value);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsConfig failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::GetImsFeatureValue(int32_t slotId, FeatureType type)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->GetImsFeatureValue(slotId, type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetImsFeatureValue failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->SetImsFeatureValue(slotId, type, value);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsFeatureValue failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::UpdateCallMediaMode(int32_t callId, CallMediaMode mode)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->UpdateCallMediaMode(callId, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallMediaMode failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::EnableVoLte(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->EnableVoLte(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("EnableVoLte failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::DisableVoLte(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->DisableVoLte(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("DisableVoLte failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::IsVoLteEnabled(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->IsVoLteEnabled(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("IsVoLteEnabled failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::EnableLteEnhanceMode(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->EnableLteEnhanceMode(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("EnableLteEnhanceMode failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::DisableLteEnhanceMode(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->DisableLteEnhanceMode(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("DisableLteEnhanceMode failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerProxy::IsLteEnhanceModeEnabled(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t errCode = callManagerServicePtr_->IsLteEnhanceModeEnabled(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetLteEnhanceMode failed, errcode:%{public}d", errCode);
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
} // namespace Telephony
} // namespace OHOS
