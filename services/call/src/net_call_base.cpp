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

#include "net_call_base.h"

#include "call_manager_errors.h"

namespace OHOS {
namespace TelephonyCallManager {
NetCallBase::NetCallBase() {}

int NetCallBase::StartConference()
{
    return TELEPHONY_NO_ERROR;
}

int NetCallBase::InviteToConference()
{
    return TELEPHONY_NO_ERROR;
}

int NetCallBase::KickOutConference()
{
    return TELEPHONY_NO_ERROR;
}

int NetCallBase::LeaveConference()
{
    return TELEPHONY_NO_ERROR;
}

int NetCallBase::GetNetCallType()
{
    return TELEPHONY_NO_ERROR;
}

int NetCallBase::ChangeNetCallType()
{
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS
