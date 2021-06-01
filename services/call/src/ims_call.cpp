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

#include "ims_call.h"

#include "call_manager_errors.h"
#include "call_manager_log.h"

namespace OHOS {
namespace TelephonyCallManager {
IMSCall::IMSCall(const CallInfo &info) : CallBase(info) {}

IMSCall::~IMSCall() {}

int32_t IMSCall::DialCall()
{
    int32_t ret = TELEPHONY_FAIL;
    ret = DialCallBase();
    if (ret == TELEPHONY_NO_ERROR) {
        callState_ = CallStateType::CALL_STATE_ACTIVE_TYPE;
    }
    return ret;
}

int32_t IMSCall::UpgradeCall()
{
    return TELEPHONY_NO_ERROR;
}

int32_t IMSCall::DownGradeCall()
{
    return TELEPHONY_NO_ERROR;
}

int32_t IMSCall::SetVoLTE()
{
    return TELEPHONY_NO_ERROR;
}

int32_t IMSCall::SetWifiCalling()
{
    return TELEPHONY_NO_ERROR;
}

int32_t IMSCall::SetWifiCallingMode()
{
    return TELEPHONY_NO_ERROR;
}

int32_t IMSCall::SetVoLTEStrongMode()
{
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS
