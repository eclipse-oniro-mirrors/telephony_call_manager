/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "super_privacy_policy_info.h"
#include "telephony_log_wrapper.h"

namespace OHOS::Telephony {
bool SuperPrivacyPolicy::Marshalling(Parcel& out) const
{
    if (!out.WriteInt32(static_cast<int32_t>(sensorType))) {
        return false;
    }
    if (!out.WriteInt32(static_cast<int32_t>(sensorState))) {
        return false;
    }
    return true;
}

bool SuperPrivacyPolicyInfo::Marshalling(Parcel& out) const
{
    if (!out.WriteInt32(static_cast<int32_t>(superPrivacyMode))) {
        return false;
    }
    if (!out.WriteInt32(static_cast<int32_t>(superPrivacyPolicies.size()))) {
        return false;
    }
    
    for (const auto& policy : superPrivacyPolicies) {
        if (!policy.Marshalling(out)) {
            return false;
        }
    }
    return true;
}
}