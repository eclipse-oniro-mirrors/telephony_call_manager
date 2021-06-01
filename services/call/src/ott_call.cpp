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

#include "ott_call.h"

#include "call_manager_errors.h"
#include "call_manager_log.h"

namespace OHOS {
namespace TelephonyCallManager {
OTTCall::OTTCall(const CallInfo &info) : CallBase(info) {}

OTTCall::~OTTCall() {}

int32_t OTTCall::DialCall()
{
    int32_t ret = TELEPHONY_FAIL;
    ret = DialCallBase();
    if (ret == TELEPHONY_NO_ERROR) {
        callState_ = CallStateType::CALL_STATE_ACTIVE_TYPE;
    }
    return ret;
}
} // namespace TelephonyCallManager
} // namespace OHOS
