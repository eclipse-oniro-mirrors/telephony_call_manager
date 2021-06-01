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

#include "call_policy.h"

#include "call_manager_errors.h"
#include "call_manager_log.h"

namespace OHOS {
namespace TelephonyCallManager {
CallPolicy::CallPolicy() {}

CallPolicy::~CallPolicy() {}

int32_t CallPolicy::DialPolicy()
{
    return HasNewCall();
}

int32_t CallPolicy::AccpetCallPolicy(int32_t callid)
{
    std::list<sptr<CallBase>> activeCallList;
    GetActiveCallList(activeCallList);
    if (activeCallList.size() <= onlyOneCall_) {
        activeCallList.clear();
        return TELEPHONY_NO_ERROR;
    }
    std::list<sptr<CallBase>>::iterator callIterator;
    for (callIterator = activeCallList.begin(); callIterator != activeCallList.end(); callIterator++) {
        if ((*callIterator)->GetCallID() != callid) {
            (*callIterator)->HoldCallBase();
        }
    }
    activeCallList.clear();
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::RejectCallPolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::HoldCallPolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::UnHoldCallPolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::HangUpPolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::SwapCallPolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::JoinCallPolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::GetTransferNumberPolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::SetTransferNumberPolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::StartConferencePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::InviteToConferencePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::KickOutConferencePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallPolicy::LeaveConferencePolicy()
{
    return TELEPHONY_NO_ERROR;
}

bool CallPolicy::IsPhoneNumberLegal(std::string number)
{
    if (number.find("!") != number.npos || number.find("@") != number.npos || number.find("$") != number.npos ||
        number.find("%") != number.npos || number.find("^") != number.npos || number.find("&") != number.npos ||
        number.find("-") != number.npos || number.find("+") != number.npos || number.find("_") != number.npos ||
        number.find("=") != number.npos || number.find("(") != number.npos || number.find(")") != number.npos ||
        number.find(",") != number.npos || number.find(";") != number.npos || number.find("|") != number.npos) {
        CALLMANAGER_DEBUG_LOG("invalid phone number!");
        return false;
    }
    return true;
}
} // namespace TelephonyCallManager
} // namespace OHOS
