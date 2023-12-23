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
    TELEPHONY_LOGI("VoipCallConnection Init start");
    if (connectCallManagerState_) {
        TELEPHONY_LOGE("Init, connectState is true");
        return;
    }
    systemAbilityId_ = systemAbilityId;
    TELEPHONY_LOGI("systemAbilityId_ = %{public}d", systemAbilityId);
    GetCallManagerProxy();
    TELEPHONY_LOGI("connected to voip call proxy successfully!");
}

void VoipCallConnection::UnInit()
{
    voipCallManagerInterfacePtr_ = nullptr;
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
    TELEPHONY_LOGI("Voipconnect UnRegisterCallManagerCallBack voipCallManagerInterfacePtr_ is null");
    if (voipCallManagerInterfacePtr_ == nullptr) {
        return TELEPHONY_ERROR;
    }
    return voipCallManagerInterfacePtr_->UnRegisterCallManagerCallBack();
}

} // namespace Telephony
} // namespace OHOS
