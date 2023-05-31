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

#ifndef CALL_CONNECT_ABILITY_H
#define CALL_CONNECT_ABILITY_H

#include <condition_variable>
#include <mutex>

#include "ability_connect_callback_interface.h"
#include "call_manager_inner_type.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
class CallConnectAbility {
DECLARE_DELAYED_SINGLETON(CallConnectAbility)
public:
    void ConnectAbility(const CallAttributeInfo &info);
    void DisconnectAbility();
    void SetConnectFlag(bool isConnected);
    void NotifyAll();
    bool WaitForConnectResult();

private:
    sptr<AAFwk::IAbilityConnection> connectCallback_ = nullptr;
    bool isConnected_ = false;
    static std::condition_variable cv_;
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_CONNECT_ABILITY_H
