/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef AFS_DETECT_TYPE_H
#define AFS_DETECT_TYPE_H

#include <cstdint>
#include <string>
#include <vector>
 
#include "parcel.h"

namespace OHOS::AntiFraudService {
 
struct AfsDetectType final : public Parcelable {
public:
    AfsDetectType() = default;
    AfsDetectType(uint32_t type, bool isFirstTime, std::string callNum, uint32_t voiceType);

    bool Marshalling(Parcel& out) const override;
    static AfsDetectType* Unmarshalling(Parcel& in);

    uint32_t type_ = 0;
    bool isFirstTime_ = 0;
    std::string callNum_;
    uint32_t voiceType_ = 0;
};

}  // namespace OHOS::AntiFraudService

#endif  // AFS_DETECT_TYPE_H