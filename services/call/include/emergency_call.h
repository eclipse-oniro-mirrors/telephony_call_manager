/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef EMERGENCY_CALL_H
#define EMERGENCY_CALL_H
#include <cstdio>
#include <cstdlib>
#include <list>
#include <string>

#include "common_type.h"

namespace OHOS {
namespace TelephonyCallManager {
class EmergencyCall {
public:
    EmergencyCall();
    ~EmergencyCall();

    bool CheckNumberIsEmergency(std::string phoneNum);
};
} // namespace TelephonyCallManager
} // namespace OHOS

#endif // EMERGENCY_CALL_H