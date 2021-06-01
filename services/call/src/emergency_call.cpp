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

#include "emergency_call.h"

#include "call_manager_log.h"
#include "cellular_call_ipc_interface_proxy.h"

namespace OHOS {
namespace TelephonyCallManager {
EmergencyCall::EmergencyCall() {}

EmergencyCall::~EmergencyCall() {}

bool EmergencyCall::CheckNumberIsEmergency(std::string phoneNum)
{
    int isEcc = 0;
    CALLMANAGER_DEBUG_LOG("CheckNumberIsEmergency");
    isEcc = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->IsUrgentCall(phoneNum, 1);
    return (isEcc != 0);
}
} // namespace TelephonyCallManager
} // namespace OHOS
