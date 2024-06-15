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

#include "super_privacy_manager_client.h"

#include <thread>

#include "parameter.h"
#include "iservice_register.h"

namespace OHOS {
namespace Telephony {
static const int SUPER_PRIVACY_MANAGER_SA_ID = 66102;
static constexpr int32_t LOAD_SA_TIMEOUT_SECONDS = 4;

SuperPrivacyManagerClient::SuperPrivacyManagerClient() {}

SuperPrivacyManagerClient &SuperPrivacyManagerClient::GetInstance()
{
    static SuperPrivacyManagerClient instance;
    return instance;
}

int32_t SuperPrivacyManagerClient::SetSuperPrivacyMode(const int32_t &mode, const int32_t &source)
{
    int32_t result;
    WithSystemAbilityProxy([mode, source, &result](const sptr<ISuperPrivacyManager> &proxy) {
        if (proxy == nullptr) {
            return;
        }
        result = proxy->SetSuperPrivacyMode(mode, source);
    });
    return result;
}

void SuperPrivacyManagerClient::WithSystemAbilityProxy(
    const std::function<void(const sptr<ISuperPrivacyManager> &)> &consumer)
{
    lock_guard<recursive_mutex> lockGurad(withSystemAbilityProxyRecursiveMutex_);
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        consumer(nullptr);
        return;
    }    
    auto sa = sam->LoadSystemAbility(SUPER_PRIVACY_MANAGER_SA_ID, LOAD_SA_TIMEOUT_SECONDS);
    ConsumeWithSystemAbility(consumer, sa);
}

void SuperPrivacyManagerClient::ConsumeWithSystemAbility(
    const std::function<void(const sptr<ISuperPrivacyManager> &)> &consumer, const sptr<IRemoteObject> &sa)
{
    if (sa == nullptr) {
        consumer(nullptr);
        return;
    }
    if (sa->IsObjectDead()) {
        consumer(nullptr);
        return;
    }
    sptr<ISuperPrivacyManager> proxy = iface_cast<ISuperPrivacyManager>(sa);
    if (proxy == nullptr) {
        consumer(nullptr);
        return;
    }
    consumer(proxy);
}

} // namespace Telephony
} // namespace OHOS