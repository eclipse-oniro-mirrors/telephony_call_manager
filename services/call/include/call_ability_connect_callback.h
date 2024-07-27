/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef CALL_ABILITY_CONNECT_CALLBACK_H
#define CALL_ABILITY_CONNECT_CALLBACK_H

#include "ability_connect_callback_interface.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Telephony {
class CallAbilityConnectCallback : public IRemoteStub<AAFwk::IAbilityConnection> {
public:
    CallAbilityConnectCallback() = default;
    ~CallAbilityConnectCallback() = default;

    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode);
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode);
    void ReConnectAbility();
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_ABILITY_CONNECT_CALLBACK_H
