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

#include "server_session.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
ServerSession::~ServerSession()
{
    Destroy();
}

void ServerSession::Create(const std::string &localName)
{
    if (localName.empty()) {
        TELEPHONY_LOGE("create server socket fail, empty local name");
        return;
    }
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        if (serverSocket_ > INVALID_SOCKET_ID) {
            TELEPHONY_LOGI("server socket %{public}d already exist", serverSocket_);
            return;
        }
    }

    SocketInfo socketInfo = {
        .name = const_cast<char*>((localName.c_str())),
        .peerName = nullptr,
        .peerNetworkId = nullptr,
        .pkgName = const_cast<char*>(PACKET_NAME),
        .dataType = DATA_TYPE_BYTES
    };
    int32_t socket = Socket(socketInfo);
    if (socket <= INVALID_SOCKET_ID) {
        TELEPHONY_LOGE("create server socket fail %{public}d", socket);
        return;
    }

    QosTV qos[] = {
        { .qos = QOS_TYPE_MIN_BW, .value = QOS_MIN_BW },
        { .qos = QOS_TYPE_MAX_LATENCY, .value = QOS_MAX_LATENCY }
    };
    int32_t ret = Listen(socket, qos, sizeof(qos) / sizeof(qos[0]), &listener_);
    if (ret != 0) {
        TELEPHONY_LOGE("start listen socket %{public}d fail, result %{public}d", socket, ret);
        return;
    }
    TELEPHONY_LOGI("create server socket %{public}d success", socket);
    std::lock_guard<ffrt::mutex> lockAdd(mutex_);
    serverSocket_ = socket;
}

void ServerSession::Destroy()
{
    int32_t socket = INVALID_SOCKET_ID;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        if (serverSocket_ > INVALID_SOCKET_ID) {
            socket = serverSocket_;
        }
        serverSocket_ = INVALID_SOCKET_ID;
        socket_ = INVALID_SOCKET_ID;
        TELEPHONY_LOGI("disconnect server session");
    }
    if (socket > INVALID_SOCKET_ID) {
        Shutdown(socket);
        TELEPHONY_LOGI("close server socket %{public}d success", socket);
    }
}

void ServerSession::OnSessionBind(int32_t socket)
{
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        if (socket_ > INVALID_SOCKET_ID) {
            TELEPHONY_LOGW("replace old socket %{public}d", socket_);
        }
        socket_ = socket;
    }
    TELEPHONY_LOGI("session %{public}d bind success", socket);
    if (callback_ != nullptr) {
        callback_->OnConnected();
    }
}

void ServerSession::OnSessionShutdown(int32_t socket)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (socket == serverSocket_) {
        serverSocket_ = INVALID_SOCKET_ID;
        socket_ = INVALID_SOCKET_ID;
    }
    if (socket == socket_) {
        socket_ = INVALID_SOCKET_ID;
    }
}

} // namespace Telephony
} // namespace OHOS
