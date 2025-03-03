/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef NATIVE_CALL_MANAGER_HISYSEVENT_H
#define NATIVE_CALL_MANAGER_HISYSEVENT_H

#include <string>

#include "telephony_hisysevent.h"

namespace OHOS {
namespace Telephony {

enum class Scenario {
    /**
     * Indicates that the sendrequest request is sent through the remote
     */
    VOPI_PROXY_IPC_CONNECTTING = 0,   
};
class NativeCallManagerHisysevent : public TelephonyHiSysEvent {
public:
    static void WriteVoipCallEvent(const std::string &callId, const std::string &bundleName,
        const int32_t scenario, const int32_t errCode, const std::string &errorMsg, const int32_t callType,
        const int32_t callState, const int32_t callDirection);
};
} // namespace Telephony
} // namespace OHOS

#endif // NATIVE_CALL_MANAGER_HISYSEVENT_H
