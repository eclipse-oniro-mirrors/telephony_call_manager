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

#ifndef TELPHONY_SUPER_PRIVACY_INFO
#define TELPHONY_SUPER_PRIVACY_INFO

#include <vector>
#include "parcel.h"

namespace OHOS::Telephony {

enum class SuperPrivacyMode {
    OFF = 0,
    ON_WHEN_FOLDED = 1,
    ALWAYS_ON = 2,
};

enum class PrivacySensorType {
    CAMERA = 0,
    MICROPHONE = 1,
    LOCATION = 2,
};

enum class PrivacySensorState {
    DEFAULT = 0,
    ENABLED = 1,
    DISABLED = 2,
};

struct SuperPrivacyPolicy : public Parcelable {
public:
    PrivacySensorType sensorType = PrivacySensorType::CAMERA;
    PrivacySensorState sensorState = PrivacySensorState::DEFAULT;
    SuperPrivacyPolicy() : sensorType(PrivacySensorType::CAMERA), sensorState(PrivacySensorState::DEFAULT) {}
    SuperPrivacyPolicy(PrivacySensorType type, PrivacySensorState state) : sensorType(type), sensorState(state) {}
    bool Marshalling(Parcel& out) const override;
};

struct SuperPrivacyPolicyInfo : public Parcelable {
public:
    SuperPrivacyMode superPrivacyMode = SuperPrivacyMode::OFF;
    std::vector<SuperPrivacyPolicy> superPrivacyPolicies;
    SuperPrivacyPolicyInfo() : superPrivacyMode(SuperPrivacyMode::OFF), superPrivacyPolicies({
        {PrivacySensorType::CAMERA, PrivacySensorState::DEFAULT},
        {PrivacySensorType::MICROPHONE, PrivacySensorState::DEFAULT},
        {PrivacySensorType::LOCATION, PrivacySensorState::DEFAULT}
    }) {}
    SuperPrivacyPolicyInfo(SuperPrivacyMode mode, std::vector<SuperPrivacyPolicy> policies)
        : superPrivacyMode(mode), superPrivacyPolicies(policies) {}
    bool Marshalling(Parcel& out) const override;
};
}
#endif