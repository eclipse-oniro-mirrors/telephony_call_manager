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

#include "transmission_manager.h"
#include "telephony_log_wrapper.h"
#include "client_session.h"
#include "server_session.h"

namespace OHOS {
namespace Telephony {
TransmissionManager::TransmissionManager()
{
}

TransmissionManager::~TransmissionManager()
{
}

std::shared_ptr<SessionAdapter> TransmissionManager::CreateServerSession(
    const std::shared_ptr<ISessionCallback> &callback)
{
    auto serverSession = std::make_shared<ServerSession>(callback);
    if (serverSession == nullptr) {
        TELEPHONY_LOGE("create server session fail");
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    session_ = serverSession;
    return serverSession;
}

std::shared_ptr<SessionAdapter> TransmissionManager::CreateClientSession(
    const std::shared_ptr<ISessionCallback> &callback)
{
    auto clientSession = std::make_shared<ClientSession>(callback);
    if (clientSession == nullptr) {
        TELEPHONY_LOGE("create client session fail");
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    session_ = clientSession;
    return clientSession;
}

void TransmissionManager::OnBind(int32_t socket)
{
    std::shared_ptr<SessionAdapter> session = nullptr;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        session = session_.lock();
    }
    if (session != nullptr) {
        session->OnSessionBind(socket);
    }
}

void TransmissionManager::OnShutdown(int32_t socket)
{
    std::shared_ptr<SessionAdapter> session = nullptr;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        session = session_.lock();
    }
    if (session != nullptr) {
        session->OnSessionShutdown(socket);
    }
}

void TransmissionManager::OnReceiveMsg(int32_t socket, const char* data, uint32_t dataLen)
{
    std::shared_ptr<SessionAdapter> session = nullptr;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        session = session_.lock();
    }
    if (session != nullptr) {
        session->OnReceiveMsg(socket, data, dataLen);
    }
}

} // namespace Telephony
} // namespace OHOS
