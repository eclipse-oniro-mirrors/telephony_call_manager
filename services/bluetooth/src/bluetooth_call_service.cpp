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

#include "bluetooth_call_service.h"

#include "bluetooth_call_manager.h"
#include "call_manager_errors.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "audio_control_manager.h"
#include "bluetooth_call_manager.h"
#include "ffrt.h"


namespace OHOS {
namespace Telephony {
const uint64_t DISCONNECT_DELAY_PLAY_TIME = 3000000;
BluetoothCallService::BluetoothCallService()
    : callControlManagerPtr_(DelayedSingleton<CallControlManager>::GetInstance()),
    sendDtmfState_(false), sendDtmfCallId_(ERR_ID)
{}

BluetoothCallService::~BluetoothCallService()
{}

int32_t BluetoothCallService::AnswerCall()
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    int32_t callId = ERR_ID;
    int32_t ret = AnswerCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("AnswerCallPolicy failed!");
        return ret;
    }
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    VideoStateType videoState = call->GetVideoStateType();
    if (callControlManagerPtr_ != nullptr) {
        DelayedSingleton<AudioControlManager>::GetInstance()->PlayWaitingTone();
        ffrt::submit_h([&])(){DelayedSingleton<AudioControlManager>::GetInstance()->StopWaitingTone();
        }, {}, {}, ffrt::task_attr().delay(DISCONNECT_DELAY_PLAY_TIME);
        }
        return callControlManagerPtr_->AnswerCall(callId, static_cast<int32_t>(videoState));
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t BluetoothCallService::RejectCall()
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    int32_t callId = ERR_ID;
    bool rejectWithMessage = false;
    std::u16string textMessage = Str8ToStr16("");
    int32_t ret = RejectCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RejectCallPolicy failed!");
        return ret;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->RejectCall(callId, rejectWithMessage, textMessage);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t BluetoothCallService::HangUpCall()
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    int32_t callId = ERR_ID;
    int32_t ret = HangUpPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HangUpPolicy failed!");
        return ret;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HangUpCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t BluetoothCallService::GetCallState()
{
    TELEPHONY_LOGI("Entry BluetoothCallService GetCallState");
    int32_t numActive = GetCallNum(TelCallState::CALL_STATUS_ACTIVE);
    int32_t numHeld = GetCallNum(TelCallState::CALL_STATUS_HOLDING);
    int32_t numDial = GetCallNum(TelCallState::CALL_STATUS_DIALING);
    int32_t numAlert = GetCallNum(TelCallState::CALL_STATUS_ALERTING);
    int32_t numIncoming = GetCallNum(TelCallState::CALL_STATUS_INCOMING);
    int32_t numWait = GetCallNum(TelCallState::CALL_STATUS_WAITING);
    int32_t numDisconnected = GetCallNum(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t numDisconnecting = GetCallNum(TelCallState::CALL_STATUS_DISCONNECTING);
    int32_t callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
    std::string number = "";
    if (numHeld > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
        number = GetCallNumber(TelCallState::CALL_STATUS_HOLDING);
    }
    if (numActive > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
        number = GetCallNumber(TelCallState::CALL_STATUS_ACTIVE);
    }
    if (numDial > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_DIALING);
        number = GetCallNumber(TelCallState::CALL_STATUS_DIALING);
    }
    if (numAlert > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_ALERTING);
        number = GetCallNumber(TelCallState::CALL_STATUS_ALERTING);
    }
    if (numIncoming > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING);
        number = GetCallNumber(TelCallState::CALL_STATUS_INCOMING);
    }
    if (numWait > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING);
        number = GetCallNumber(TelCallState::CALL_STATUS_WAITING);
    }
    if (numDisconnected > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
        number = GetCallNumber(TelCallState::CALL_STATUS_DISCONNECTED);
    }
    if (numDisconnecting > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
        number = GetCallNumber(TelCallState::CALL_STATUS_DISCONNECTING);
    }
    return DelayedSingleton<BluetoothCallManager>::GetInstance()->
        SendBtCallState(numActive, numHeld, callState, number);
}

int32_t BluetoothCallService::HoldCall()
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    int32_t callId = ERR_ID;
    int32_t ret = HoldCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HoldCallPolicy failed!");
        return ret;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HoldCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t BluetoothCallService::UnHoldCall()
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    int32_t callId = ERR_ID;
    int32_t ret = UnHoldCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UnHoldCallPolicy failed!");
        return ret;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->UnHoldCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t BluetoothCallService::SwitchCall()
{
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    int32_t callId = ERR_ID;
    int32_t ret = SwitchCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SwitchCallPolicy failed!");
        return ret;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SwitchCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t BluetoothCallService::StartDtmf(char str)
{
    int32_t callId = ERR_ID;
    int32_t ret = StartDtmfPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StartDtmfPolicy failed!");
        return ret;
    }
    if (callControlManagerPtr_ != nullptr) {
        {
            std::lock_guard<std::mutex> guard(lock_);
            sendDtmfState_ = true;
            sendDtmfCallId_ = callId;
        }
        return callControlManagerPtr_->StartDtmf(callId, str);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t BluetoothCallService::StopDtmf()
{
    int32_t callId = ERR_ID;
    if (callControlManagerPtr_ != nullptr) {
        {
            std::lock_guard<std::mutex> guard(lock_);
            callId = sendDtmfCallId_;
            sendDtmfState_ = false;
            sendDtmfCallId_ = ERR_ID;
        }
        return callControlManagerPtr_->StopDtmf(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t BluetoothCallService::CombineConference()
{
    int32_t callId = ERR_ID;
    int32_t ret = CombineConferencePolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CombineConferencePolicy failed!");
        return ret;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->CombineConference(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t BluetoothCallService::SeparateConference()
{
    int32_t callId = ERR_ID;
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SeparateConference(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t BluetoothCallService::KickOutFromConference()
{
    int32_t callId = ERR_ID;
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->KickOutFromConference(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

std::vector<CallAttributeInfo> BluetoothCallService::GetCurrentCallList(int32_t slotId)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("BluetoothCallService::GetCurrentCallList, Permission denied!");
        std::vector<CallAttributeInfo> vec;
        vec.clear();
        return vec;
    }
    if (slotId < 0) {
        return GetAllCallInfoList();
    }
    return GetCallInfoList(slotId);
}
} // namespace Telephony
} // namespace OHOS
