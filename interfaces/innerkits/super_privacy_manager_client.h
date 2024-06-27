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

#ifndef SECURITY_PRIVACY_SERVER_SUPER_PRIVACY_MANAGER_CLIENT_H
#define SECURITY_PRIVACY_SERVER_SUPER_PRIVACY_MANAGER_CLIENT_H

#include <cstdint>
#include <functional>
#include <atomic>
#include <map>
#include <set>
#include <mutex>
#include <unordered_set>

#include "parameter.h"

#include "if_system_ability_manager.h"

namespace OHOS {
namespace Telephony {
class SuperPrivacyManagerClient {
public:
    static SuperPrivacyManagerClient &GetInstance();
    ~SuperPrivacyManagerClient() = default;
    int32_t SetSuperPrivacyMode(const int32_t &mode, const int32_t &source);

private:
    SuperPrivacyManagerClient();
    static constexpr int32_t COMMAND_SET_SUPER_PRIVACY_MODE = MIN_TRANSACTION_ID + 0;
};
} // namespace Telephony
} // namespace OHOS

#endif //SECURITY_PRIVACY_SERVER_SUPER_PRIVACY_MANAGER_CLIENT_H