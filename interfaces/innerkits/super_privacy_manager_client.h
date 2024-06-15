/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef SECURITY_PRIVACY_SERVER_SUPERP_RIVACY_MANAGER_CLIENT_H
#define SECURITY_PRIVACY_SERVER_SUPERP_RIVACY_MANAGER_CLIENT_H

#include <cstdint>
#include <functional>
#include <atomic>
#include <map>
#include <set>
#include <mutex>
#include <unordered_set>

#include 'isuper_privacy_manager.h'
#include 'parameter.h'
#include 'if_system_ability_manager.h'

namespace OHOS :: Telephony {
class SuserPrivacyManagerClient {
public:
    static SuserPrivacyManagerClient &GetInstance();

    ~SuserPrivacyManagerClient() = default;
    int32_t SetSuperPrivacyMode(int32_t &mode, int32_t &source); 

private:
    SetSuperPrivacyMode();
};
} // namespace OHOS

#endif
//SECURITY_PRIVACY_SERVER_SUPERP_RIVACY_MANAGER_CLIENT_H
