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
#include "call_manager.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
namespace OHOS {
namespace TelephonyApi {
CallManager::CallManager()
{
    ConnectService();
}

CallManager::~CallManager() {}

int32_t CallManager::DialCall(std::u16string number, AppExecFwk::PacMap &extras, int32_t &callId)
{
    if (callManagerService_ != nullptr) {
        return callManagerService_->DialCall(number, extras, callId);
    }
    return DEFAULT_ERROR;
}

int32_t CallManager::AcceptCall(int32_t callId, int32_t videoState)
{
    if (callManagerService_ != nullptr) {
        return callManagerService_->AcceptCall(callId, videoState);
    }
    return DEFAULT_ERROR;
}

int32_t CallManager::RejectCall(int32_t callId, bool isSendSms, std::u16string content)
{
    if (callManagerService_ != nullptr) {
        return callManagerService_->RejectCall(callId, isSendSms, content);
    }
    return DEFAULT_ERROR;
}

int32_t CallManager::HangUpCall(int32_t callId)
{
    if (callManagerService_ != nullptr) {
        return callManagerService_->HangUpCall(callId);
    }
    return DEFAULT_ERROR;
}

int32_t CallManager::GetCallState()
{
    if (callManagerService_ != nullptr) {
        return callManagerService_->GetCallState();
    }
    return DEFAULT_ERROR;
}

int32_t CallManager::ConnectService()
{
    sptr<OHOS::ISystemAbilityManager> systemAblilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAblilityManager == nullptr) {
        return DEFAULT_ERROR;
    }
    sptr<IRemoteObject> object = systemAblilityManager->GetSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (object != nullptr) {
        callManagerService_ = iface_cast<OHOS::TelephonyCallManager::ICallManagerService>(object);
    }
    return 0;
}
} // namespace TelephonyApi
} // namespace OHOS
