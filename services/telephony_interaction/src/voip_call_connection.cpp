/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "voip_call_connection.h"

#include "i_voip_call_manager_callback.h"
#include "i_voip_call_manager_service.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"
#include "voip_call_manager_proxy.h"

namespace OHOS {
namespace Telephony {
VoipCallConnection::VoipCallConnection()
    : systemAbilityId_(TELEPHONY_VOIP_CALL_MANAGER_SYS_ABILITY_ID), connectCallManagerState_(false)
{}

VoipCallConnection::~VoipCallConnection()
{
    UnInit();
}

void VoipCallConnection::Init(int32_t systemAbilityId)
{
    if (connectCallManagerState_) {
        TELEPHONY_LOGE("Init, connectState is true");
        return;
    }
    systemAbilityId_ = systemAbilityId;
    TELEPHONY_LOGI("systemAbilityId_ = %{public}d", systemAbilityId);
    GetCallManagerProxy();
    statusChangeListener_ = new (std::nothrow) SystemAbilityListener();
    if (statusChangeListener_ == nullptr) {
        TELEPHONY_LOGE("Init, failed to create statusChangeListener.");
        return;
    }
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("voipconnect managerPtr is null");
        return;
    }
    int32_t ret = managerPtr->SubscribeSystemAbility(systemAbilityId_, statusChangeListener_);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Init, failed to subscribe sa:%{public}d", systemAbilityId_);
        return;
    }
    TELEPHONY_LOGI("subscribe voip call manager successfully!");
}

void VoipCallConnection::UnInit()
{
    voipCallManagerInterfacePtr_ = nullptr;
    connectCallManagerState_ = false;
    if (statusChangeListener_ != nullptr) {
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy != nullptr) {
            samgrProxy->UnSubscribeSystemAbility(systemAbilityId_, statusChangeListener_);
            statusChangeListener_ = nullptr;
        }
    }
    TELEPHONY_LOGI("voip call connection uninit");
}

int32_t VoipCallConnection::GetCallManagerProxy()
{
    TELEPHONY_LOGI("Voipconnect GetCallManagerProxy start");
    if (voipCallManagerInterfacePtr_ != nullptr) {
        return TELEPHONY_SUCCESS;
    }
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGI("Voipconnect managerPtr is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    sptr<IVoipCallManagerService> voipCallManagerInterfacePtr = nullptr;
    sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(systemAbilityId_);
    if (iRemoteObjectPtr == nullptr) {
        TELEPHONY_LOGI("Voipconnect iRemoteObjectPtr is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    voipCallManagerInterfacePtr = iface_cast<IVoipCallManagerService>(iRemoteObjectPtr);
    if (!voipCallManagerInterfacePtr) {
        TELEPHONY_LOGI("Voipconnect GetCallManagerProxy voipCallManagerInterfacePtr is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    voipCallManagerInterfacePtr_ = voipCallManagerInterfacePtr;
    connectCallManagerState_ = true;
    return TELEPHONY_SUCCESS;
}

int32_t VoipCallConnection::AnswerCall(const VoipCallEventInfo &events, int32_t videoState)
{
    GetCallManagerProxy();
    if (voipCallManagerInterfacePtr_ == nullptr) {
        TELEPHONY_LOGI("Voipconnect AnswerCall voipCallManagerInterfacePtr_ is null");
        return TELEPHONY_ERROR;
    }
    return voipCallManagerInterfacePtr_->Answer(events, videoState);
}

int32_t VoipCallConnection::RejectCall(const VoipCallEventInfo &events)
{
    GetCallManagerProxy();
    if (voipCallManagerInterfacePtr_ == nullptr) {
        TELEPHONY_LOGI("Voipconnect RejectCall voipCallManagerInterfacePtr_ is null");
        return TELEPHONY_ERROR;
    }
    return voipCallManagerInterfacePtr_->Reject(events);
}

int32_t VoipCallConnection::HangUpCall(const VoipCallEventInfo &events)
{
    GetCallManagerProxy();
    if (voipCallManagerInterfacePtr_ == nullptr) {
        TELEPHONY_LOGI("Voipconnect HangUpCall voipCallManagerInterfacePtr_ is null");
        return TELEPHONY_ERROR;
    }
    return voipCallManagerInterfacePtr_->HangUp(events);
}

int32_t VoipCallConnection::RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback)
{
    GetCallManagerProxy();
    if (voipCallManagerInterfacePtr_ == nullptr) {
        TELEPHONY_LOGI("Voipconnect RegisterCallManagerCallBack voipCallManagerInterfacePtr_ is null");
        return TELEPHONY_ERROR;
    }
    return voipCallManagerInterfacePtr_->RegisterCallManagerCallBack(callback);
}

int32_t VoipCallConnection::UnRegisterCallManagerCallBack()
{
    GetCallManagerProxy();
    if (voipCallManagerInterfacePtr_ == nullptr) {
        TELEPHONY_LOGI("Voipconnect UnRegisterCallManagerCallBack voipCallManagerInterfacePtr_ is null");
        return TELEPHONY_ERROR;
    }
    int32_t ret = voipCallManagerInterfacePtr_->UnRegisterCallManagerCallBack();
    UnInit();
    return ret;
}

void VoipCallConnection::SystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    auto voipCallConnection = DelayedSingleton<VoipCallConnection>::GetInstance();
    if (voipCallConnection == nullptr) {
        TELEPHONY_LOGE("voipCallConnection is nullptr");
        return;
    }
    voipCallConnection->Init(systemAbilityId);
}

void VoipCallConnection::SystemAbilityListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    auto voipCallConnection = DelayedSingleton<VoipCallConnection>::GetInstance();
    if (voipCallConnection == nullptr) {
        TELEPHONY_LOGE("voipCallConnection is nullptr");
        return;
    }
    voipCallConnection->ClearVoipCall();
    voipCallConnection->UnInit();
}

void VoipCallConnection::ClearVoipCall()
{
    if (!CallObjectManager::HasVoipCallExist()) {
        TELEPHONY_LOGI("no voip call exist, no need to clear");
        return;
    }
    std::list<sptr<CallBase>> allCallList = CallObjectManager::GetAllCallList();
    for (auto call : allCallList) {
        if (call != nullptr && call->GetCallType() == CallType::TYPE_VOIP) {
            TELEPHONY_LOGI("clearVoipCall callId %{public}d", call->GetCallID());
            CallObjectManager::DeleteOneCallObject(call);
        }
    }
}

int32_t VoipCallConnection::SendCallUiEvent(std::string &voipCallId, const CallAudioEvent &callAudioEvent)
{
    GetCallManagerProxy();
    if (voipCallManagerInterfacePtr_ == nullptr) {
        return TELEPHONY_ERROR;
    }
    return voipCallManagerInterfacePtr_->SendCallUiEvent(voipCallId, callAudioEvent);
}
} // namespace Telephony
} // namespace OHOS
