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

#include "call_control_manager.h"
#include "call_manager_errors.h"
#include "call_manager_inner_type.h"
#include "call_object_manager.h"
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
    UpdateCallState(callObjectPtr, nextState);
    UpdateCallStateForSlotId(callObjectPtr, nextState);
}

void CallStateReportProxy::UpdateCallState(sptr<CallBase> &callObjectPtr, TelCallState nextState)
{
    sptr<CallBase> foregroundCall;
    if (nextState == TelCallState::CALL_STATUS_ANSWERED) {
        foregroundCall = callObjectPtr;
    } else {
        foregroundCall = CallObjectManager::GetForegroundCall(false);
    }
    if (foregroundCall == nullptr && callObjectPtr != nullptr) {
        foregroundCall = callObjectPtr;
    }
    CallAttributeInfo info;
    TelCallState currentVoipCallState = GetVoipCallState();
    if ((foregroundCall != nullptr) && (nextState == TelCallState::CALL_STATUS_ANSWERED ||
        foregroundCall->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING ||
        foregroundCall->GetTelCallState() == TelCallState::CALL_STATUS_WAITING)) {
        foregroundCall->GetCallAttributeInfo(info);
        if (nextState == TelCallState::CALL_STATUS_ANSWERED) {
            info.callState = TelCallState::CALL_STATUS_ANSWERED;
        }
    } else if (currentVoipCallState == TelCallState::CALL_STATUS_INCOMING ||
        currentVoipCallState == TelCallState::CALL_STATUS_ANSWERED) {
        info.callState = currentVoipCallState;
    } else if (foregroundCall != nullptr) {
        foregroundCall->GetCallAttributeInfo(info);
    } else {
        info.callState = currentVoipCallState;
    }
    if (info.callState == currentCallState_) {
        TELEPHONY_LOGI("foreground call state is not changed, currentCallState_:%{public}d!", currentCallState_);
        return;
    }
    if (info.callState == TelCallState::CALL_STATUS_DISCONNECTING) {
        TELEPHONY_LOGI("disconnecting call no need to report call state");
        return;
    }
    currentCallState_ = info.callState;
    std::string str(info.accountNumber);
    std::u16string accountNumber = Str8ToStr16(str);
    ReportCallState(static_cast<int32_t>(info.callState), accountNumber);
}

void CallStateReportProxy::UpdateCallStateForVoIPOrRestart()
{
    sptr<CallBase> callObjectPtr = nullptr;
    UpdateCallState(callObjectPtr, TelCallState::CALL_STATUS_IDLE);
}

TelCallState CallStateReportProxy::GetVoipCallState()
{
    int32_t state;
    DelayedSingleton<CallControlManager>::GetInstance()->GetVoIPCallState(state);
    TELEPHONY_LOGI("report voip call state = %{public}d.", state);
    TelCallState nextState = TelCallState::CALL_STATUS_IDLE;
    switch ((CallStateToApp)state) {
        case CallStateToApp::CALL_STATE_IDLE:
            nextState = TelCallState::CALL_STATUS_IDLE;
            break;
        case CallStateToApp::CALL_STATE_OFFHOOK:
            nextState = TelCallState::CALL_STATUS_ACTIVE;
            break;
        case CallStateToApp::CALL_STATE_RINGING:
            nextState = TelCallState::CALL_STATUS_INCOMING;
            break;
        case CallStateToApp::CALL_STATE_ANSWERED:
            nextState = TelCallState::CALL_STATUS_ANSWERED;
            break;
        default:
            break;
    }
    return nextState;
}

void CallStateReportProxy::UpdateCallStateForSlotId(sptr<CallBase> &callObjectPtr, TelCallState nextState)
{
    CallAttributeInfo info;
    callObjectPtr->GetCallAttributeInfo(info);
    std::string str(info.accountNumber);
    std::u16string accountNumber = Str8ToStr16(str);
    if (nextState == TelCallState::CALL_STATUS_ANSWERED) {
        info.callState = TelCallState::CALL_STATUS_ANSWERED;
    }
    if (info.callState == TelCallState::CALL_STATUS_DISCONNECTING) {
        TELEPHONY_LOGI("disconnecting call no need to report call state");
        return;
    }
    ReportCallStateForCallId(info.accountId, static_cast<int32_t>(info.callState), accountNumber);
}

int32_t CallStateReportProxy::ReportCallState(int32_t callState, std::u16string phoneNumber)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = DelayedRefSingleton<TelephonyStateRegistryClient>::GetInstance().UpdateCallState(
        callState, phoneNumber);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("notifyCallStateUpdated failed, errcode:%{public}d", ret);
        return ret;
    }
    TELEPHONY_LOGI("report call state:%{public}d", callState);
    return ret;
}

int32_t CallStateReportProxy::ReportCallStateForCallId(
    int32_t slotId, int32_t callState, std::u16string incomingNumber)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = DelayedRefSingleton<TelephonyStateRegistryClient>::GetInstance().UpdateCallStateForSlotId(
        slotId, callState, incomingNumber);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("NotifyCallStateUpdated failed, errcode:%{public}d", ret);
        return ret;
    }
    TELEPHONY_LOGI("report call state:%{public}d, slotId:%{public}d", callState, slotId);
    return ret;
}
} // namespace Telephony
} // namespace OHOS
