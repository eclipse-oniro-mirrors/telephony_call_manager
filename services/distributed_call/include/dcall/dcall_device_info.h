/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_IDCALL_DEVICE_INFO_H
#define OHOS_IDCALL_DEVICE_INFO_H

#include <cstdint>
#include <string>
#include <iostream>

#include "securec.h"

namespace OHOS {
namespace DistributedHardware {
constexpr int16_t kMaxDCallDevIdLen = 96;
constexpr int16_t kMaxDCallDevNameLen = 128;

enum class DCallDeviceType : int32_t {
    DISTRIBUTED_DEVICE_UNKNOWN = 0,
    DISTRIBUTED_DEVICE_CAR,
    DISTRIBUTED_DEVICE_PHONE,
    DISTRIBUTED_DEVICE_PAD,
};

struct DCallDeviceInfo {
    char devId[kMaxDCallDevIdLen] = { 0 };
    char devName[kMaxDCallDevNameLen] = { 0 };
    DCallDeviceType devType = DCallDeviceType::DISTRIBUTED_DEVICE_UNKNOWN;
};
} // DistributedHardware
} // OHOS

#endif // OHOS_IDCALL_DEVICE_INFO_H
