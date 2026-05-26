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

#include "afs_detect_type.h"

namespace OHOS::AntiFraudService {

template <typename T>
inline bool releaseIfFalse(bool expr, T*& obj)
{
    if (!expr) {
        delete obj;
        obj = nullptr;
        return true;
    }
    return false;
}

AfsDetectType::AfsDetectType(uint32_t type, bool isFirstTime, std::string callNum, uint32_t voiceType):
    type_(type), isFirstTime_(isFirstTime), callNum_(callNum), voiceType_(voiceType) {}

bool AfsDetectType::Marshalling(Parcel& out) const
{
    if (!out.WriteUint32(type_)) {
        return false;
    }
    if (!out.WriteBool(isFirstTime_)) {
        return false;
    }
    if (!out.WriteString(callNum_)) {
        return false;
    }
    if (!out.WriteUint32(voiceType_)) {
        return false;
    }
    return true;
}

AfsDetectType* AfsDetectType::Unmarshalling(Parcel& in)
{
    auto* afsDetectType = new (std::nothrow) AfsDetectType();
    if (afsDetectType == nullptr) {
        return nullptr;
    }
    if (releaseIfFalse(in.ReadUint32(afsDetectType->type_), afsDetectType)) {
        return nullptr;
    }
    if (releaseIfFalse(in.ReadBool(afsDetectType->isFirstTime_), afsDetectType)) {
        return nullptr;
    }
    if (releaseIfFalse(in.ReadString(afsDetectType->callNum_), afsDetectType)) {
        return nullptr;
    }
    if (releaseIfFalse(in.ReadUint32(afsDetectType->voiceType_), afsDetectType)) {
        return nullptr;
    }
    return afsDetectType;
}
}