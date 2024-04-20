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

#include "call_state_report_proxy.h"

#include <string_ex.h>

#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "call_manager_errors.h"
#include "call_manager_inner_type.h"
#include "telephony_log_wrapper.h"
#include "telephony_state_registry_client.h"

namespace OHOS {
namespace Telephony {
CallStateReportProxy::CallStateReportProxy() {}

CallStateReportProxy::~CallStateReportProxy() {}

void CallStateReportProxy::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("callObjectPtr is nullptr!");
        return;
    }
    if (callObjectPtr->GetCallType() == CallType::TYPE_VOIP) {
        TELEPHONY_LOGI("voip call no need to report call state");
        return;
    }
    CallAttributeInfo info;
    callObjectPtr->GetCallAttributeInfo(info);
    std::string str(info.accountNumber);
    std::u16string accountNumber = Str8ToStr16(str);
    if (info.callState == TelCallState::CALL_STATUS_INCOMING) {
        ReportCallState(info.accountId, static_cast<int32_t>(info.callState), accountNumber);
    } else {
        ReportCallStateForCallId(info.accountId, info.callId, static_cast<int32_t>(info.callState), accountNumber);
    }
}

int32_t CallStateReportProxy::ReportCallState(int32_t slotId, int32_t callState, std::u16string phoneNumber)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = DelayedRefSingleton<TelephonyStateRegistryClient>::GetInstance().UpdateCallState(
        slotId, callState, phoneNumber);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("notifyCallStateUpdated failed, errcode:%{public}d", ret);
        return ret;
    }
    TELEPHONY_LOGI("report call state:%{public}d", callState);
    return ret;
}

int32_t CallStateReportProxy::ReportCallStateForCallId(
    int32_t slotId, int32_t callId, int32_t callState, std::u16string incomingNumber)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = DelayedRefSingleton<TelephonyStateRegistryClient>::GetInstance().UpdateCallStateForSlotId(
        slotId, callId, callState, incomingNumber);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("NotifyCallStateUpdated failed, errcode:%{public}d", ret);
        return ret;
    }
    TELEPHONY_LOGI("report call state:%{public}d, callId:%{public}d", callState, callId);
    return ret;
}
} // namespace Telephony
} // namespace OHOS
