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

#include "call_base.h"

#include <securec.h>

#include "call_manager_errors.h"
#include "call_manager_log.h"

#include "cellular_call_ipc_interface_proxy.h"
#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
CallBase::CallBase(const CallInfo &info)
    : callState_(CallStateType::CALL_STATE_CREATE_TYPE), callInfo_(info), isSpeakerphoneOn_(false),
      callEndedType_(CallEndedType::UNKNOWN)
{
    (void)memset_s(&contactInfo_, sizeof(ContactInfo), 0, sizeof(ContactInfo));
}

CallBase::~CallBase() {}

int32_t CallBase::Init()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallBase::DialCallBase()
{
    int32_t ret = CALL_MANAGER_DIAL_FAILED;
    CellularCall::CellularCallInfo callInfo;
    bool isVoicemailNumber = false;
    std::string phoneNumber;

    if (strlen(callInfo_.phoneNum) == 0) {
        CALLMANAGER_WARNING_LOG("phone number is null!");
        return CALL_MANAGER_PHONENUM_NULL;
    }
    callState_ = CallStateType::CALL_STATE_CONNECTING_TYPE;
    callInfo.callId = callInfo_.callId;
    callInfo.accountId = callInfo_.accountId;
    callInfo.phoneNetType = PhoneNetType::PHONE_TYPE_GSM;
    callInfo.callType = callInfo_.callType;
    callInfo.videoState = callInfo_.videoState;
    (void)memset_s(callInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen);
    if (memcpy_s(callInfo.phoneNum, kMaxNumberLen, callInfo_.phoneNum, strlen(callInfo_.phoneNum)) != 0) {
        return TELEPHONY_MEMCPY_FAIL;
    }
    phoneNumber = callInfo_.phoneNum;
    // Determine voice mail if it is dialed directly by omitting the following steps.
    isVoicemailNumber = CheckVoicemailNumber(phoneNumber);
    if (isVoicemailNumber) {
        if (callInfo_.dialScene == CALL_NORMAL || callInfo_.dialScene == CALL_PRIVILEGED) {
            ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->Dial(callInfo);
        }
        return ret;
    }
    if ((callInfo_.dialScene == CALL_NORMAL && callInfo_.isEcc && !callInfo_.isDefault) ||
        (callInfo_.dialScene == CALL_EMERGENCY && !callInfo_.isEcc)) {
        return CALL_MANAGER_DIAL_SCENE_INCORRECT;
    }
    // Obtain gateway information
    CALLMANAGER_ERR_LOG("CALL TYPE: %{public}d", callInfo.callType);
    ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->Dial(callInfo);
    CALLMANAGER_DEBUG_LOG("start to set audio");
    // Set audio, set hands-free
    SetAudio();
    return ret;
}

int32_t CallBase::IncomingCallBase()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallBase::AccpetCallBase()
{
    int32_t ret = CALL_MANAGER_ACCPET_FAILED;
    DelayedSingleton<AudioControlManager>::GetInstance()->SetVolumeAudible();
    CellularCall::CellularCallInfo callInfo;
    callInfo.callId = callInfo_.callId;
    callInfo.accountId = callInfo_.accountId;
    callInfo.phoneNetType = PhoneNetType::PHONE_TYPE_GSM;
    callInfo.callType = callInfo_.callType;
    callInfo.videoState = callInfo_.videoState;
    (void)memset_s(callInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen);
    if (memcpy_s(callInfo.phoneNum, kMaxNumberLen, callInfo_.phoneNum, strlen(callInfo_.phoneNum)) != 0) {
        return TELEPHONY_MEMCPY_FAIL;
    }
    ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->Answer(callInfo);
    return ret;
}

int32_t CallBase::RejectCallBase()
{
    int32_t ret = CALL_MANAGER_REJECT_FAILED;
    if (IsCurrentRinging()) {
        CellularCall::CellularCallInfo callInfo;
        callInfo.callId = callInfo_.callId;
        callInfo.accountId = callInfo_.accountId;
        callInfo.phoneNetType = PhoneNetType::PHONE_TYPE_GSM;
        callInfo.callType = callInfo_.callType;
        callInfo.videoState = callInfo_.videoState;
        (void)memset_s(callInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen);
        if (memcpy_s(callInfo.phoneNum, kMaxNumberLen, callInfo_.phoneNum, strlen(callInfo_.phoneNum)) != 0) {
            return TELEPHONY_MEMCPY_FAIL;
        }
        ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->Reject(callInfo);
    }
    return ret;
}

int32_t CallBase::HoldCallBase()
{
    int32_t ret = CALL_MANAGER_HOLD_FAILED;
    return ret;
}

int32_t CallBase::UnHoldCallBase()
{
    int32_t ret = CALL_MANAGER_UNHOLD_FAILED;
    return ret;
}

int32_t CallBase::HangUpBase()
{
    CellularCall::CellularCallInfo callInfo;
    callInfo.callId = callInfo_.callId;
    callInfo.accountId = callInfo_.accountId;
    callInfo.phoneNetType = PhoneNetType::PHONE_TYPE_GSM;
    callInfo.callType = callInfo_.callType;
    callInfo.videoState = callInfo_.videoState;
    (void)memset_s(callInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen);
    if (memcpy_s(callInfo.phoneNum, kMaxNumberLen, callInfo_.phoneNum, strlen(callInfo_.phoneNum)) != 0) {
        return TELEPHONY_MEMCPY_FAIL;
    }

    int32_t ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->End(callInfo);
    return ret;
}

int32_t CallBase::TurnOffVoice(bool silence)
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallBase::GetCallID()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return callInfo_.callId;
}

int32_t CallBase::SetCallID(int32_t callId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    callInfo_.callId = callId;
    return TELEPHONY_NO_ERROR;
}

CallStateType CallBase::GetState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return callState_;
}

void CallBase::SetState(TelCallStates nextState)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callInfo_.state == nextState) {
        CALLMANAGER_INFO_LOG("Call state duplication %d", nextState);
        return;
    }
    callInfo_.state = nextState;
    switch (nextState) {
        case TelCallStates::CALL_STATUS_DIALING:
            callState_ = CallStateType::CALL_STATE_DIALING_TYPE;
            break;
        case TelCallStates::CALL_STATUS_INCOMING:
            callState_ = CallStateType::CALL_STATE_RINGING_TYPE;
            break;
        case TelCallStates::CALL_STATUS_WAITING:
            callState_ = CallStateType::CALL_STATE_RINGING_TYPE;
            break;
        case TelCallStates::CALL_STATUS_ACTIVE:
            callState_ = CallStateType::CALL_STATE_ACTIVE_TYPE;
            break;
        case TelCallStates::CALL_STATUS_HOLDING:
            callState_ = CallStateType::CALL_STATE_HOLD_TYPE;
            break;
        case TelCallStates::CALL_STATUS_DISCONNECTED:
            callState_ = CallStateType::CALL_STATE_ENDED_TYPE;
            break;
        case TelCallStates::CALL_STATUS_DISCONNECTING:
            callState_ = CallStateType::CALL_STATE_ENDING_TYPE;
            break;
        case TelCallStates::CALL_STATUS_ALERTING:
            callState_ = CallStateType::CALL_STATE_DIALING_TYPE;
            break;
        default:
            break;
    }
    CALLMANAGER_INFO_LOG("call new state is %{public}d.", callState_);
}

bool CallBase::GetBaseCallInfo(CallInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    info = callInfo_;
    return true;
}

int32_t CallBase::SetCallInfo(const CallInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    callInfo_ = info;
    return TELEPHONY_NO_ERROR;
}

bool CallBase::GetCallerInfo(ContactInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    info = contactInfo_;
    return true;
}

void CallBase::SetCallerInfo(const ContactInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    contactInfo_ = info;
}

CallEndedType CallBase::GetCallEndedType()
{
    return callEndedType_;
}

int32_t CallBase::SetCallEndedType(CallEndedType callEndedType)
{
    callEndedType_ = callEndedType;
    return TELEPHONY_NO_ERROR;
}

bool CallBase::CheckVoicemailNumber(std::string phoneNumber)
{
    return false;
}

void CallBase::SetAudio()
{
    // Get from the configuration file
    bool useSpeakerWhenDocked = true;
    /**
     * Determine if hands-free should be enabled during docking.
     * When the device is docked and the Bluetooth or wired headset is not in use,
     * please turn on the hands-free
     */
    bool useSpeakerForDock = IsSpeakerphoneEnabled();

    SetSpeakerphoneOn(useSpeakerWhenDocked || useSpeakerForDock);
    // Confirm whether the speaker is turned on
    if (isSpeakerphoneOn_) {
        DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioDevice(DEVICE_SPEAKER);
    }
}

bool CallBase::IsSpeakerphoneEnabled()
{
    // Gets whether the device can be started from the configuration
    return true;
}

bool CallBase::GetEmergencyState()
{
    return callInfo_.isEcc;
}

bool CallBase::IsCurrentRinging()
{
    return (callState_ == CallStateType::CALL_STATE_RINGING_TYPE) ? true : false;
}

std::string CallBase::GetPhoneNumber()
{
    return callInfo_.phoneNum;
}

int32_t CallBase::SetSpeakerphoneOn(bool speakerphoneOn)
{
    isSpeakerphoneOn_ = speakerphoneOn;
    return TELEPHONY_NO_ERROR;
}

bool CallBase::IsSpeakerphoneOn()
{
    return isSpeakerphoneOn_;
}
} // namespace TelephonyCallManager
} // namespace OHOS
