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

#include "session_adapter.h"
#include "telephony_log_wrapper.h"
#include "transmission_manager.h"

namespace OHOS {
namespace Telephony {
SessionAdapter::SessionAdapter(const std::shared_ptr<ISessionCallback> &callback) : callback_(callback)
{
    listener_.OnBind = SessionAdapter::OnBind;
    listener_.OnShutdown = SessionAdapter::OnShutdown;
    listener_.OnBytes = SessionAdapter::OnBytes;
    listener_.OnMessage = nullptr;
    listener_.OnStream = nullptr;
    listener_.OnFile = nullptr;
    listener_.OnQos = nullptr;
    listener_.OnError = SessionAdapter::OnError;
    listener_.OnNegotiate = nullptr;
}

bool SessionAdapter::IsReady()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return socket_ > INVALID_SOCKET_ID;
}

void SessionAdapter::SendMsg(const void *data, uint32_t len)
{
    int32_t socket = INVALID_SOCKET_ID;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        if (socket_ <= INVALID_SOCKET_ID) {
            TELEPHONY_LOGE("send msg fail, invalid socket %{public}d", socket_);
            return;
        }
        socket = socket_;
    }
    int32_t ret = SendBytes(socket, data, len);
    TELEPHONY_LOGI("send socket %{public}d msg len %{public}u, ret %{public}d", socket, len, ret);
}

void SessionAdapter::OnReceiveMsg(int32_t socket, const char* data, uint32_t dataLen)
{
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        if (socket != socket_) {
            TELEPHONY_LOGW("socket %{public}d not match %{public}d", socket, socket_);
            return;
        }
    }
    if (callback_ != nullptr) {
        callback_->OnReceiveMsg(data, dataLen);
    }
}

void SessionAdapter::OnBind(int32_t socket, PeerSocketInfo info)
{
    TELEPHONY_LOGI("session %{public}d bind, data type %{public}d", socket, info.dataType);
    auto transMgr = DelayedSingleton<TransmissionManager>::GetInstance();
    if (transMgr != nullptr) {
        transMgr->OnBind(socket);
    }
}

void SessionAdapter::OnShutdown(int32_t socket, ShutdownReason reason)
{
    TELEPHONY_LOGI("session %{public}d closed, reason %{public}d", socket, reason);
    auto transMgr = DelayedSingleton<TransmissionManager>::GetInstance();
    if (transMgr != nullptr) {
        transMgr->OnShutdown(socket);
    }
}

void SessionAdapter::OnBytes(int32_t socket, const void *data, uint32_t dataLen)
{
    auto transMgr = DelayedSingleton<TransmissionManager>::GetInstance();
    if (transMgr != nullptr) {
        transMgr->OnReceiveMsg(socket, static_cast<const char*>(data), dataLen);
    }
}

void SessionAdapter::OnError(int32_t socket, int32_t errCode)
{
    TELEPHONY_LOGE("async bind session %{public}d fail, reason %{public}d", socket, errCode);
}

} // namespace Telephony
} // namespace OHOS
