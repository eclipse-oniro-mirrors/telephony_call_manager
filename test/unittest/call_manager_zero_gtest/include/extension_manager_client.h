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

#ifndef CALL_MANAGER_ZERO_EXTENSION_MANAGER_CLIENT_H
#define CALL_MANAGER_ZERO_EXTENSION_MANAGER_CLIENT_H

#include <cstdint>

#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace AAFwk {
class ExtensionManagerClient {
public:
    ExtensionManagerClient() = default;
    virtual ~ExtensionManagerClient() = default;

    static ExtensionManagerClient &GetInstance();
    int32_t ConnectServiceExtensionAbility(const Want &want, const sptr<IRemoteObject> &connect, int32_t userId);
    int32_t ConnectServiceExtensionAbility(const Want &want, const sptr<IRemoteObject> &connect,
        const sptr<IRemoteObject> &callerToken, int32_t userId);
    int32_t DisconnectAbility(const sptr<IRemoteObject> &connect);
    void SetConnectResult(int32_t result);
    sptr<IRemoteObject> GetConnectObject();
    void Reset();

private:
    int32_t connectResult_ = -1;
    int32_t disconnectResult_ = -1;
    sptr<IRemoteObject> connect_ = nullptr;
};
} // namespace AAFwk
} // namespace OHOS

#endif // CALL_MANAGER_ZERO_EXTENSION_MANAGER_CLIENT_H
