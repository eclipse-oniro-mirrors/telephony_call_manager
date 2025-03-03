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

#include "native_call_manager_hisysevent.h"

namespace OHOS {
namespace Telephony {
// EVENT
static const std::string VOIP_CALL_MANAGER = "CALL_INCOMING_NUM_IDENTITY";

// KEY
static const std::string CALL_ID_KEY = "CALL_ID";
static const std::string BUNDL_NAME_KEY = "BUNDL_NAME";
static const std::string SCENARIOS_KEY = "SCENARIOS";
static const std::string ERR_CODE_KEY = "ERR_CODE";
static const std::string ERROR_MSG_KEY = "ERROR_MSG";
static const std::string CALL_TYPE_KEY = "CALL_TYPE";
static const std::string CALL_STATE_KEY = "CALL_STATE";
static const std::string CALL_DIRECTION_KEY = "CALL_DIRECTION";
static const std::string TIMESTAMP_KEY = "TIMESTAMP";

//VALUE

void NativeCallManagerHisysevent::WriteVoipCallEvent(const std::string &callId, const std::string &bundleName,
    const int32_t scenario, const int32_t errCode, const std::string &errorMsg, const int32_t callType,
    const int32_t callState, const int32_t callDirection)
{
    time_t currentTime = time(0);
    HiSysEventWrite(DOMAIN_NAME, VOIP_CALL_MANAGER, EventType::FAULT, CALL_ID_KEY, callId, BUNDL_NAME_KEY, bundleName,
        SCENARIOS_KEY, scenario, ERR_CODE_KEY, errCode, ERROR_MSG_KEY, errorMsg, CALL_TYPE_KEY, callType,
        CALL_STATE_KEY, callState, CALL_DIRECTION_KEY, callDirection, TIMESTAMP_KEY, static_cast<int64_t>(currentTime));
}

} // namespace Telephony
} // namespace OHOS
