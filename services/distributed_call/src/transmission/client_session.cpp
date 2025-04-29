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

#include "client_session.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
ClientSession::~ClientSession()
{
    Disconnect();
}

void ClientSession::Connect(const std::string &peerDevId, const std::string &localName, const std::string &peerName,
    const int32_t &qosMinBw)
{
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        if (clientSocket_ > INVALID_SOCKET_ID) {
            TELEPHONY_LOGI("client socket %{public}d already connect", clientSocket_);
            return;
        }
    }
    int32_t socket = CreateSocket(peerDevId, localName, peerName);
    if (socket <= INVALID_SOCKET_ID) {
        return;
    }
    QosTV qos[] = {
        { .qos = QOS_TYPE_MIN_BW, .value = qosMinBw },
        { .qos = QOS_TYPE_MAX_LATENCY, .value = QOS_MAX_LATENCY }
    };
    int32_t ret = BindAsync(socket, qos, sizeof(qos) / sizeof(qos[0]), &listener_);
    TELEPHONY_LOGI("async bind socket %{public}d result %{public}d", socket, ret);

    std::lock_guard<ffrt::mutex> lock(mutex_);
    clientSocket_ = socket;
}

void ClientSession::Disconnect()
{
    int32_t socket = INVALID_SOCKET_ID;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        if (clientSocket_ > INVALID_SOCKET_ID) {
            socket = clientSocket_;
        }
        clientSocket_ = INVALID_SOCKET_ID;
        socket_ = INVALID_SOCKET_ID;
        TELEPHONY_LOGI("disconnect client session");
    }
    if (socket > INVALID_SOCKET_ID) {
        Shutdown(socket);
        TELEPHONY_LOGI("close client socket %{public}d success", socket);
    }
}

void ClientSession::OnSessionBind(int32_t socket)
{
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        socket_ = socket;
    }
    TELEPHONY_LOGI("session %{public}d bind success", socket);
    if (callback_ != nullptr) {
        callback_->OnConnected();
    }
}

void ClientSession::OnSessionShutdown(int32_t socket)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (socket == socket_) {
        socket_ = INVALID_SOCKET_ID;
    }
    if (socket == clientSocket_) {
        clientSocket_ = INVALID_SOCKET_ID;
        socket_ = INVALID_SOCKET_ID;
    }
}

int32_t ClientSession::CreateSocket(const std::string &peerDevId, const std::string &localName,
                                    const std::string &peerName)
{
    if (peerDevId.empty()) {
        TELEPHONY_LOGE("create socket fail, empty peer dev");
        return INVALID_SOCKET_ID;
    }
    SocketInfo socketInfo = {
        .name = const_cast<char*>((localName.c_str())),
        .peerName = const_cast<char*>(peerName.c_str()),
        .peerNetworkId = const_cast<char*>(peerDevId.c_str()),
        .pkgName = const_cast<char*>(PACKET_NAME),
        .dataType = DATA_TYPE_BYTES
    };
    int32_t socket = Socket(socketInfo);
    if (socket <= INVALID_SOCKET_ID) {
        TELEPHONY_LOGE("create client socket fail %{public}d", socket);
        return socket;
    }
    return socket;
}

} // namespace Telephony
} // namespace OHOS
