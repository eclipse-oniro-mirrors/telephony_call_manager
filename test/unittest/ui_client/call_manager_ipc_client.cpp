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

#include "call_manager_ipc_client.h"

#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "call_manager_errors.h"
#include "call_manager_log.h"

namespace OHOS {
namespace TelephonyCallManager {
CallManagerIpcClient::CallManagerIpcClient()
    : systemAbilityId_(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID), callManagerServicePtr_(nullptr)
{}

CallManagerIpcClient::~CallManagerIpcClient()
{
    if (callManagerServicePtr_) {
        callManagerServicePtr_.clear();
        callManagerServicePtr_ = nullptr;
    }
}

int32_t CallManagerIpcClient::Init(int32_t systemAbilityId)
{
    printf("Enter CallManagerIpcClient::Init,systemAbilityId:%d\n", systemAbilityId);
    systemAbilityId_ = systemAbilityId;
    int32_t result = ConnectService();
    printf("Connect service: %X\n", result);
    return result;
}

void CallManagerIpcClient::UnInit()
{
    DisconnectService();
    printf("Disconnect service\n");
}

int32_t CallManagerIpcClient::ConnectService()
{
    int32_t ret = TELEPHONY_FAIL;
    if (callManagerServicePtr_ != nullptr) {
        printf("Already get telephony base service\n");
        return ret;
    }

    printf("Get telephony base service %d start\n", systemAbilityId_);
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        printf("GetSystemAbilityManager null\n");
        return ret;
    }

    sptr<ICallManagerService> callManagerServicePtr =
        iface_cast<ICallManagerService>(managerPtr->GetSystemAbility(systemAbilityId_));
    if (callManagerServicePtr == nullptr) {
        printf("GetSystemAbility null\n");
        return ret;
    }

    printf("Get callManagerService finish\n");
    callManagerServicePtr_ = callManagerServicePtr;
    return TELEPHONY_NO_ERROR;
}

void CallManagerIpcClient::DisconnectService()
{
    printf("Enter CallManagerIpcClient::DisconnectService\n");
    if (callManagerServicePtr_ == nullptr) {
        callManagerServicePtr_ = nullptr;
    }
    printf("Leave CallManagerIpcClient::DisconnectService\n");
}

int32_t CallManagerIpcClient::DialCall(std::u16string number, AppExecFwk::PacMap &extras, int32_t &callId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->DialCall(number, extras, callId);
    }
    return TELEPHONY_FAIL;
}

int32_t CallManagerIpcClient::AcceptCall(int32_t callId, int32_t videoState) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->AcceptCall(callId, videoState);
    }
    return TELEPHONY_FAIL;
}

int32_t CallManagerIpcClient::RejectCall(int32_t callId, bool isSendSms, std::u16string content) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->RejectCall(callId, isSendSms, content);
    }
    return TELEPHONY_FAIL;
}

int32_t CallManagerIpcClient::HangUpCall(int32_t callId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->HangUpCall(callId);
    }
    return TELEPHONY_FAIL;
}

int32_t CallManagerIpcClient::GetCallState() const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->GetCallState();
    }
    return TELEPHONY_FAIL;
}
} // namespace TelephonyCallManager
} // namespace OHOS