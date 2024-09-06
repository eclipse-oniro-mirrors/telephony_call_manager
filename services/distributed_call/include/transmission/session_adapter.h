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

#ifndef TELEPHONY_SESSION_ADAPTER_H
#define TELEPHONY_SESSION_ADAPTER_H

#include <memory>
#include "ffrt.h"
#include "transport/socket.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t INVALID_SOCKET_ID = -1;
constexpr int32_t QOS_MIN_BW = 4 * 1024 * 1024;
constexpr int32_t QOS_MAX_LATENCY = 10000;
constexpr const char* PACKET_NAME = "ohos.telephony.callmanager";
constexpr const char* SESSION_NAME = "ohos.telephony.callmanager.distributed_communication";

class ISessionCallback {
public:
    ISessionCallback() = default;
    virtual ~ISessionCallback() = default;
    virtual void OnConnected() = 0;
    virtual void OnReceiveMsg(const char* data, uint32_t dataLen) = 0;
};

class SessionAdapter {
public:
    explicit SessionAdapter(const std::shared_ptr<ISessionCallback> &callback);
    virtual ~SessionAdapter() = default;

    virtual void Create(const std::string &localName) = 0;
    virtual void Destroy() = 0;
    virtual void Connect(const std::string &peerDevId, const std::string &localName, const std::string &peerName) = 0;
    virtual void Disconnect() = 0;
    virtual void OnSessionBind(int32_t socket) = 0;
    virtual void OnSessionShutdown(int32_t socket) = 0;

    bool IsReady();
    void SendMsg(const void *data, uint32_t len);
    void OnReceiveMsg(int32_t socket, const char* data, uint32_t dataLen);

    static void OnBind(int32_t socket, PeerSocketInfo info);
    static void OnShutdown(int32_t socket, ShutdownReason reason);
    static void OnBytes(int32_t socket, const void *data, uint32_t dataLen);
    static void OnError(int32_t socket, int32_t errCode);

protected:
    ISocketListener listener_{};
    std::shared_ptr<ISessionCallback> callback_{};
    ffrt::mutex mutex_{};
    int32_t socket_{INVALID_SOCKET_ID};
};

} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_SESSION_ADAPTER_H
