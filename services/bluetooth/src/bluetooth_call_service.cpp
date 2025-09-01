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

#include "audio_device_manager.h"
#include "bluetooth_call_manager.h"
#include "call_manager_errors.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "audio_control_manager.h"
#include "ffrt.h"
#include "call_superprivacy_control_manager.h"


namespace OHOS {
namespace Telephony {
const uint64_t DELAY_STOP_PLAY_TIME = 3000000;
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
        if (IsVoipCallExist()) {
            sendEventToVoip(CallAbilityEventId::EVENT_ANSWER_VOIP_CALL);
            return TELEPHONY_SUCCESS;
        }
        return ret;
    }
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    VideoStateType videoState = call->GetVideoStateType();
    if (videoState != VideoStateType::TYPE_VOICE && videoState != VideoStateType::TYPE_VIDEO) {
        TELEPHONY_LOGE("get original call type!");
        videoState = static_cast<VideoStateType>(call->GetOriginalCallType());
    }
    if (callControlManagerPtr_ != nullptr) {
        if (DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->
            GetCurrentIsSuperPrivacyMode()) {
            DelayedSingleton<AudioControlManager>::GetInstance()->PlayWaitingTone();
            ffrt::submit_h([]() {
                DelayedSingleton<AudioControlManager>::GetInstance()->StopWaitingTone();
                }, {}, {}, ffrt::task_attr().delay(DELAY_STOP_PLAY_TIME));
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
        if (IsVoipCallExist()) {
            sendEventToVoip(CallAbilityEventId::EVENT_REJECT_VOIP_CALL);
            return TELEPHONY_SUCCESS;
        }
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
        TELEPHONY_LOGE("HangUpPolicy Voip Call!");
        if (IsVoipCallExist()) {
            sendEventToVoip(CallAbilityEventId::EVENT_HANGUP_VOIP_CALL);
            return TELEPHONY_SUCCESS;
        }
        DeleteOneVoipCallObject(callId);
        return ret;
    }
    if (callId >= VOIP_CALL_MINIMUM  && IsVoipCallExist()) {
        sendEventToVoip(CallAbilityEventId::EVENT_HANGUP_VOIP_CALL);
        DeleteOneVoipCallObject(callId);
        return TELEPHONY_SUCCESS;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HangUpCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

void BluetoothCallService::sendEventToVoip(CallAbilityEventId eventId)
{
    CallEventInfo eventInfo;
    (void)memset_s(&eventInfo, sizeof(CallEventInfo), 0, sizeof(CallEventInfo));
    eventInfo.eventId = eventId;
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallEventUpdated(eventInfo);
}

int32_t BluetoothCallService::GetCallState()
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    TELEPHONY_LOGI("Entry BluetoothCallService GetCallState");
    int32_t numActive = 0;
    int32_t numHeld = 0;
    int32_t callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
    std::string number = "";
    std::vector<int32_t> carrierCallInfo = getCarrierCallInfoNum(callState, number);
    if (!carrierCallInfo.empty()) {
        numActive = carrierCallInfo[0];
        numHeld = carrierCallInfo[1];
    }
    if (IsVoipCallExist() && numActive == 0) {
        GetVoipCallState(numActive, callState, number);
    }
    return DelayedSingleton<BluetoothCallManager>::GetInstance()->
        SendBtCallState(numActive, numHeld, callState, number);
}

std::vector<int32_t> BluetoothCallService::getCarrierCallInfoNum(int32_t &callState, std::string &number)
{
    int32_t numActive = GetCallNum(TelCallState::CALL_STATUS_ACTIVE, false);
    int32_t numHeld = GetCallNum(TelCallState::CALL_STATUS_HOLDING, false);
    int32_t numDial = GetCallNum(TelCallState::CALL_STATUS_DIALING, false);
    int32_t numAlert = GetCallNum(TelCallState::CALL_STATUS_ALERTING, false);
    int32_t numIncoming = GetCallNum(TelCallState::CALL_STATUS_INCOMING, false);
    int32_t numWait = GetCallNum(TelCallState::CALL_STATUS_WAITING, false);
    int32_t numDisconnected = GetCallNum(TelCallState::CALL_STATUS_DISCONNECTED, false);
    int32_t numDisconnecting = GetCallNum(TelCallState::CALL_STATUS_DISCONNECTING, false);
    if (numHeld > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
        number = GetCallNumber(TelCallState::CALL_STATUS_HOLDING, false);
    }
    if (numActive > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
        number = GetCallNumber(TelCallState::CALL_STATUS_ACTIVE, false);
    }
    if (numDial > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_DIALING);
        number = GetCallNumber(TelCallState::CALL_STATUS_DIALING, false);
    }
    if (numAlert > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_ALERTING);
        number = GetCallNumber(TelCallState::CALL_STATUS_ALERTING, false);
    }
    if (numIncoming > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING);
        number = GetCallNumber(TelCallState::CALL_STATUS_INCOMING, false);
    }
    if (numWait > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING);
        number = GetCallNumber(TelCallState::CALL_STATUS_WAITING, false);
    }
    if (numDisconnected > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
        number = GetCallNumber(TelCallState::CALL_STATUS_DISCONNECTED, false);
    }
    if (numDisconnecting > 0) {
        callState = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
        number = GetCallNumber(TelCallState::CALL_STATUS_DISCONNECTING, false);
    }
    return {numActive, numHeld};
}

void BluetoothCallService::GetVoipCallState(int32_t &numActive, int32_t &callState, std::string &number)
{
    TELEPHONY_LOGI("GetVoipCallState start,callState:%{public}d", callState);
    CallAttributeInfo callAttributeInfo = GetVoipCallInfo();
    if (callState == (int32_t)TelCallState::CALL_STATUS_IDLE && number == "") {
        switch (callAttributeInfo.callState) {
            case TelCallState::CALL_STATUS_IDLE:
                numActive = 1;
                break;
            case TelCallState::CALL_STATUS_WAITING:
                callState = (int32_t)TelCallState::CALL_STATUS_INCOMING;
                break;
            case TelCallState::CALL_STATUS_INCOMING:
                callState = (int32_t)TelCallState::CALL_STATUS_INCOMING;
                break;
            case TelCallState::CALL_STATUS_ALERTING:
                callState = (int32_t)TelCallState::CALL_STATUS_ALERTING;
                break;
            default:
                TELEPHONY_LOGI("voip call state need not handle");
                break;
        }
        number = callAttributeInfo.accountNumber;
    }

    if (numActive == 0 && callAttributeInfo.callState == TelCallState::CALL_STATUS_ACTIVE) {
        numActive = 1;
    }
    TELEPHONY_LOGI("GetVoipCallState finish,callState:%{public}d, numActive:%{public}d", callState, numActive);
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
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
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
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
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
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
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
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
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
    if (!TelephonyPermission::CheckPermission(Permission::ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
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
        return GetAllCallInfoList(false);
    }
    return GetCallInfoList(slotId, false);
}

int32_t BluetoothCallService::AddAudioDeviceList(const std::string &address, int32_t deviceType,
    const std::string &name)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("AddAudioDeviceList, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (deviceType != static_cast<int32_t>(AudioDeviceType::DEVICE_NEARLINK) &&
        deviceType != static_cast<int32_t>(AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID)) {
        TELEPHONY_LOGE("AddAudioDeviceList, invalid device type!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    DelayedSingleton<AudioDeviceManager>::GetInstance()->AddAudioDeviceList(address,
        static_cast<AudioDeviceType>(deviceType), name);
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallService::RemoveAudioDeviceList(const std::string &address, int32_t deviceType)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("RemoveAudioDeviceList, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (deviceType != static_cast<int32_t>(AudioDeviceType::DEVICE_NEARLINK) &&
        deviceType != static_cast<int32_t>(AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID)) {
        TELEPHONY_LOGE("RemoveAudioDeviceList, invalid device type!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    DelayedSingleton<AudioDeviceManager>::GetInstance()->RemoveAudioDeviceList(address,
        static_cast<AudioDeviceType>(deviceType));
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallService::ResetNearlinkDeviceList()
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("ResetNearlinkDeviceList, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    DelayedSingleton<AudioDeviceManager>::GetInstance()->ResetNearlinkAudioDevicesList();
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallService::ResetBtHearingAidDeviceList()
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("ResetBtHearingAidDeviceList, Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    DelayedSingleton<AudioDeviceManager>::GetInstance()->ResetBtHearingAidDeviceList();
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
