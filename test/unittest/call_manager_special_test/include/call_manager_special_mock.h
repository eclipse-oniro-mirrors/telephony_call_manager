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

#ifndef CALL_MANAGER_SPECIAL_MOCK_H
#define CALL_MANAGER_SPECIAL_MOCK_H

#define private public
#define protected public

#include <gmock/gmock.h>
#include <cstdint>

#include "call_manager_service_proxy.h"
#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
class MockRemoteObject : public IRemoteObject {
public:
    explicit MockRemoteObject(int command) : IRemoteObject(u"default")
    {
        command_ = command;
    }

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        if (command_ == -1) {
            return -1;
        }
        reply.WriteInt32(0);
        if (command_ == 0) {
            std::vector<std::u16string> responseBuffer{u"0"};
            reply.WriteString16Vector(responseBuffer);
        }
        return 0;
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    bool IsObjectDead() const
    {
        return command_ != 0 && command_ != 1;
    }

    int Dump(int fd, const std::vector<std::u16string> &args)
    {
        return 0;
    }
private:
    int command_;
}; // class MockRemoteObject
} // namespace OHOS
} // namespace Telephony

namespace OHOS {
namespace AAFwk {
class ExtensionManagerClient {
public:
    static ExtensionManagerClient& GetInstance()
    {
        static ExtensionManagerClient extensionManagerClient;
        return extensionManagerClient;
    }

    int32_t ConnectServiceExtensionAbility(const Want &want, const sptr<IRemoteObject> &connect, int32_t userId)
    {
        return -1;
    }

    int32_t DisconnectAbility(const sptr<IRemoteObject> &connect)
    {
        return -1;
    }
}; // class ExtensionManagerClient
} // namespace AAFwk
} // namespace OHOS

#endif // CALL_MANAGER_SPECIAL_MOCK_H
