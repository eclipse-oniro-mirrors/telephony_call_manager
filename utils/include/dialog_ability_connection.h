/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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
 
#ifndef DIALOG_ABILITY_CONNECTION_H
#define DIALOG_ABILITY_CONNECTION_H
 
#include <string>
 
#include "ability_manager_client.h"
#include "ability_connect_callback_stub.h"
#include "ffrt.h"
 
namespace OHOS {
namespace Telephony {
class DialogAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    DialogAbilityConnection();
    ~DialogAbilityConnection();
    DialogAbilityConnection(const DialogAbilityConnection &other) = delete;
    DialogAbilityConnection(DialogAbilityConnection &&other) = delete;
    DialogAbilityConnection &operator=(const DialogAbilityConnection &other) = delete;
    DialogAbilityConnection &operator=(DialogAbilityConnection &&other) = delete;
 
    int32_t OpenSystemDialog(const std::string &bundleName, const std::string &abilityName,
        const std::string &phoneNumber, const std::string &callingBundleName);
    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;
 
private:
    int32_t ConnectExtension();
    int32_t Destroy();
 
    sptr<IRemoteObject> dialogContext_;
    std::string bundleName_;
    std::string abilityName_;
    std::string phoneNumber_;
    std::string callingBundleName_;
    ffrt::mutex mutex_;
};
 
} // namespace Telephony
} // namespace OHOS
#endif // DIALOG_ABILITY_CONNECTION_H