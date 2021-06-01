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

#ifndef CALL_MANAGER_IPC_CLIENT_H
#define CALL_MANAGER_IPC_CLIENT_H
#include <string>

#include "if_system_ability_manager.h"
#include "pac_map.h"
#include "refbase.h"

#include "call_manager_service_proxy.h"

namespace OHOS {
namespace TelephonyCallManager {
class CallManagerIpcClient {
public:
    CallManagerIpcClient();
    virtual ~CallManagerIpcClient();

    int32_t Init(int32_t systemAbilityId);
    void UnInit();
    int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras, int32_t &callId) const;
    int32_t AcceptCall(int32_t callId, int32_t videoState) const;
    int32_t RejectCall(int32_t callId, bool isSendSms, std::u16string content) const;
    int32_t HangUpCall(int32_t callId) const;
    int32_t GetCallState() const;

private:
    int32_t ConnectService();
    void DisconnectService();

private:
    int32_t systemAbilityId_;
    sptr<ICallManagerService> callManagerServicePtr_;
};
} // namespace TelephonyCallManager
} // namespace OHOS

#endif // CALL_MANAGER_IPC_CLIENT_H