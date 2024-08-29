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

#ifndef TELEPHONY_TRANSMISSION_MANAGER_H
#define TELEPHONY_TRANSMISSION_MANAGER_H

#include "ffrt.h"
#include "singleton.h"
#include "session_adapter.h"

namespace OHOS {
namespace Telephony {
class TransmissionManager {
    DECLARE_DELAYED_SINGLETON(TransmissionManager);

public:
    std::shared_ptr<SessionAdapter> CreateServerSession(const std::shared_ptr<ISessionCallback> &callback);
    std::shared_ptr<SessionAdapter> CreateClientSession(const std::shared_ptr<ISessionCallback> &callback);
    void OnBind(int32_t socket);
    void OnShutdown(int32_t socket);
    void OnReceiveMsg(int32_t socket, const char* data, uint32_t dataLen);

private:
    ffrt::mutex mutex_{};
    std::weak_ptr<SessionAdapter> session_{};
};

} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_TRANSMISSION_MANAGER_H
