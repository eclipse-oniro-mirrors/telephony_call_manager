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

#include "call_status_policy.h"

#include <securec.h>

#include "call_manager_errors.h"
#include "call_manager_log.h"

#include "call_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
CallStatusPolicy::CallStatusPolicy() {}

CallStatusPolicy::~CallStatusPolicy() {}

int32_t CallStatusPolicy::IncomingHandlePolicy(ContactInfo info)
{
    int32_t ret = TELEPHONY_FAIL;
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("VoicemailInterceptor failed!");
    } else {
        CALLMANAGER_INFO_LOG("VoicemailInterceptor success!");
    }
    return ret;
}

int32_t CallStatusPolicy::DialingHandlePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusPolicy::ActiveHandlePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusPolicy::HoldingHandlePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusPolicy::WaitingHandlePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusPolicy::AlertHandlePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusPolicy::DisconnectingHandlePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusPolicy::DisconnectedHandlePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusPolicy::IdleHandlePolicy()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusPolicy::FilterResultsDispose(sptr<CallBase> call)
{
    int32_t ret = TELEPHONY_FAIL;
    ContactInfo info;
    if (call == nullptr) {
        return CALL_MANAGER_CALL_NULL;
    }
    call->GetCallerInfo(info);
    if (HasRingingMaximum() || HasDialingMaximum()) {
        ret = call->HangUpBase();
        CALLMANAGER_INFO_LOG("HasDialingMaximum and HasDialingMaximum");
        // Print the log and log the information to the database
    } else {
        ret = call->IncomingCallBase();
        AddOneCallObject(call);
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyNewCallCreated(call);
    }
    // Print the log and log the information to the database
    CALLMANAGER_INFO_LOG("IncomingCall");
    // Call notification module
    return ret;
}
} // namespace TelephonyCallManager
} // namespace OHOS