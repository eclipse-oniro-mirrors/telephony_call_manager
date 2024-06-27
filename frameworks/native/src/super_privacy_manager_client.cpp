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
#include <iremote_proxy.h>

#include "parameter.h"
#include "iservice_registry.h"

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
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto sa = sam->LoadSystemAbility(SUPER_PRIVACY_MANAGER_SA_ID, LOAD_SA_TIMEOUT_SECONDS);
    if (sa == nullptr) {
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(
        u"OHOS.AppSecurityPrivacy.SecurityPrivacyServer.SuperPrivacy.ISuperPrivacyManager")) {
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(mode)) {
        return ERR_INVALID_DATA;
    }
    if (!data.WriteInt32(source)) {
        return ERR_INVALID_DATA;
    }
    int32_t result = sa->SendRequest(COMMAND_SET_SUPER_PRIVACY_MODE, data, reply, option);
    return result;
}
} // namespace Telephony
} // namespace OHOS