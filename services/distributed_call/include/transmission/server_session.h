/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_SERVER_SESSION_H
#define TELEPHONY_SERVER_SESSION_H

#include "session_adapter.h"

namespace OHOS {
namespace Telephony {
class ServerSession : public SessionAdapter {
public:
    explicit ServerSession(const std::shared_ptr<ISessionCallback> &callback) : SessionAdapter(callback) {}
    ~ServerSession() override;
    void Create(const std::string &localName) override;
    void Destroy() override;
    void Connect(const std::string &peerDevId, const std::string &localName, const std::string &peerName) override {}
    void Disconnect() override {}
    void OnSessionBind(int32_t socket) override;
    void OnSessionShutdown(int32_t socket) override;

private:
    int32_t serverSocket_{INVALID_SOCKET_ID};
};

} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_SERVER_SESSION_H
