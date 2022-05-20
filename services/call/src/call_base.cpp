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
#include "telephony_log_wrapper.h"

#include "common_type.h"
#include "cellular_call_connection.h"
#include "audio_control_manager.h"

namespace OHOS {
namespace Telephony {
CallBase::CallBase(DialParaInfo &info)
    : callId_(info.callId), callType_(info.callType), videoState_(info.videoState), accountNumber_(info.number),
    bundleName_(info.bundleName), callRunningState_(CallRunningState::CALL_RUNNING_STATE_CREATE),
    conferenceState_(TelConferenceState::TEL_CONFERENCE_IDLE), startTime_(0),
    direction_(CallDirection::CALL_DIRECTION_IN), policyFlag_(0), callState_(info.callState),
    isSpeakerphoneOn_(false), callEndedType_(CallEndedType::UNKNOWN), callBeginTime_(0), callEndTime_(0),
    ringBeginTime_(0), ringEndTime_(0), answerType_(CallAnswerType::CALL_ANSWER_MISSED)
{
    (void)memset_s(&contactInfo_, sizeof(ContactInfo), 0, sizeof(ContactInfo));
}

CallBase::CallBase(DialParaInfo &info, AppExecFwk::PacMap &extras)
    : callId_(info.callId), callType_(info.callType), videoState_(info.videoState), accountNumber_(info.number),
    bundleName_(info.bundleName), callRunningState_(CallRunningState::CALL_RUNNING_STATE_CREATE),
    conferenceState_(TelConferenceState::TEL_CONFERENCE_IDLE), startTime_(0),
    direction_(CallDirection::CALL_DIRECTION_OUT), policyFlag_(0), callState_(info.callState),
    isSpeakerphoneOn_(false), callEndedType_(CallEndedType::UNKNOWN), callBeginTime_(0), callEndTime_(0),
    ringBeginTime_(0), ringEndTime_(0), answerType_(CallAnswerType::CALL_ANSWER_MISSED)
{
    (void)memset_s(&contactInfo_, sizeof(ContactInfo), 0, sizeof(ContactInfo));
}

CallBase::~CallBase() {}

int32_t CallBase::DialCallBase()
{
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_CONNECTING;
    TELEPHONY_LOGI("start to set audio");
    // Set audio, set hands-free
    SetAudio();
    return TELEPHONY_SUCCESS;
}

int32_t CallBase::IncomingCallBase()
{
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_RINGING;
    return TELEPHONY_SUCCESS;
}

int32_t CallBase::AnswerCallBase()
{
    if (!IsCurrentRinging()) {
        TELEPHONY_LOGW("the device is currently not ringing");
        return CALL_ERR_PHONE_ANSWER_IS_BUSY;
    }
    DelayedSingleton<AudioControlManager>::GetInstance()->SetVolumeAudible();
    return TELEPHONY_SUCCESS;
}

int32_t CallBase::RejectCallBase()
{
    answerType_ = CallAnswerType::CALL_ANSWER_REJECT;
    return TELEPHONY_SUCCESS;
}

void CallBase::GetCallAttributeBaseInfo(CallAttributeInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    (void)memset_s(info.accountNumber, kMaxNumberLen, 0, kMaxNumberLen);
    if (memcpy_s(info.accountNumber, kMaxNumberLen, accountNumber_.c_str(), accountNumber_.length()) == 0) {
        info.speakerphoneOn = isSpeakerphoneOn_;
        info.videoState = videoState_;
        info.startTime = startTime_;
        info.callType = callType_;
        info.callId = callId_;
        info.callState = callState_;
        info.conferenceState = conferenceState_;
        info.callBeginTime = callBeginTime_;
        info.callEndTime = callEndTime_;
        info.ringBeginTime = ringBeginTime_;
        info.ringEndTime = ringEndTime_;
        info.callDirection = direction_;
        info.answerType = answerType_;
        errno_t result = memcpy_s(info.bundleName, kMaxBundleNameLen, bundleName_.c_str(),
                                  bundleName_.length());
        if (result != EOK) {
            TELEPHONY_LOGE("memcpy_s failed!");
        }
    }
}

int32_t CallBase::GetCallID()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return callId_;
}

CallType CallBase::GetCallType()
{
    return callType_;
}

CallRunningState CallBase::GetCallRunningState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return callRunningState_;
}

// transfer from external call state to callmanager local state
int32_t CallBase::SetTelCallState(TelCallState nextState)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callState_ == nextState) {
        TELEPHONY_LOGI("Call state duplication %{public}d", nextState);
        return CALL_ERR_NOT_NEW_STATE;
    }
    callState_ = nextState;
    switch (nextState) {
        case TelCallState::CALL_STATUS_DIALING:
            StateChangesToDialing();
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            StateChangesToIncoming();
            break;
        case TelCallState::CALL_STATUS_WAITING:
            StateChangesToWaiting();
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            StateChangesToActive();
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            StateChangesToHolding();
            break;
        case TelCallState::CALL_STATUS_DISCONNECTED:
            StateChangesToDisconnected();
            break;
        case TelCallState::CALL_STATUS_DISCONNECTING:
            StateChangesToDisconnecting();
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            StateChangesToAlerting();
            break;
        default:
            break;
    }
    return TELEPHONY_SUCCESS;
}

void CallBase::StateChangesToDialing()
{
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_DIALING;
}

void CallBase::StateChangesToIncoming()
{
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_RINGING;
    ringBeginTime_ = time(nullptr);
}

void CallBase::StateChangesToWaiting()
{
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_RINGING;
    ringBeginTime_ = time(nullptr);
}

void CallBase::StateChangesToActive()
{
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_ACTIVE;
    if (callBeginTime_ == 0) {
        callBeginTime_ = ringEndTime_ = time(nullptr);
        startTime_ = callBeginTime_;
        answerType_ = CallAnswerType::CALL_ANSWER_ACTIVED;
    }
}

void CallBase::StateChangesToHolding()
{
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_HOLD;
    if (conferenceState_ == TelConferenceState::TEL_CONFERENCE_ACTIVE) {
        conferenceState_ = TelConferenceState::TEL_CONFERENCE_DISCONNECTED;
    }
}

void CallBase::StateChangesToDisconnected()
{
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_ENDED;
    if (conferenceState_ == TelConferenceState::TEL_CONFERENCE_DISCONNECTING ||
        conferenceState_ == TelConferenceState::TEL_CONFERENCE_ACTIVE) {
        conferenceState_ = TelConferenceState::TEL_CONFERENCE_DISCONNECTED;
    }
    callEndTime_ = time(nullptr);
    if (ringEndTime_ == 0) {
        ringEndTime_ = time(nullptr);
    }
}

void CallBase::StateChangesToDisconnecting()
{
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_ENDING;
    if (conferenceState_ == TelConferenceState::TEL_CONFERENCE_ACTIVE) {
        conferenceState_ = TelConferenceState::TEL_CONFERENCE_DISCONNECTING;
    }
    if (ringEndTime_ == 0) {
        ringEndTime_ = time(nullptr);
    }
}

void CallBase::StateChangesToAlerting()
{
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_DIALING;
    ringBeginTime_ = time(nullptr);
}

TelCallState CallBase::GetTelCallState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return callState_;
}

void CallBase::SetTelConferenceState(TelConferenceState state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    conferenceState_ = state;
    TELEPHONY_LOGI("SetTelConferenceState, state:%{public}d", state);
}

TelConferenceState CallBase::GetTelConferenceState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return conferenceState_;
}

VideoStateType CallBase::GetVideoStateType()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return videoState_;
}

void CallBase::SetVideoStateType(VideoStateType mediaType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    videoState_ = mediaType;
}

void CallBase::SetPolicyFlag(PolicyFlag flag)
{
    std::lock_guard<std::mutex> lock(mutex_);
    policyFlag_ |= flag;
}

uint64_t CallBase::GetPolicyFlag()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return policyFlag_;
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

void CallBase::SetCallRunningState(CallRunningState callRunningState)
{
    std::lock_guard<std::mutex> lock(mutex_);
    callRunningState_ = callRunningState;
}

void CallBase::SetStartTime(int64_t startTime)
{
    std::lock_guard<std::mutex> lock(mutex_);
    startTime_ = startTime;
}

void CallBase::SetCallBeginTime(time_t callBeginTime)
{
    std::lock_guard<std::mutex> lock(mutex_);
    callBeginTime_ = callBeginTime;
}

void CallBase::SetCallEndTime(time_t callEndTime)
{
    std::lock_guard<std::mutex> lock(mutex_);
    callEndTime_ = callEndTime;
}

void CallBase::SetRingBeginTime(time_t ringBeginTime)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ringBeginTime_ = ringBeginTime;
}

void CallBase::SetRingEndTime(time_t ringEndTime)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ringEndTime_ = ringEndTime;
}

void CallBase::SetAnswerType(CallAnswerType answerType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    answerType_ = answerType;
}

CallEndedType CallBase::GetCallEndedType()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return callEndedType_;
}

int32_t CallBase::SetCallEndedType(CallEndedType callEndedType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    callEndedType_ = callEndedType;
    return TELEPHONY_SUCCESS;
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
        DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioDevice(AudioDevice::DEVICE_SPEAKER);
    }
}

bool CallBase::IsSpeakerphoneEnabled()
{
    // Gets whether the device can be started from the configuration
    return true;
}

bool CallBase::IsCurrentRinging()
{
    return (callRunningState_ == CallRunningState::CALL_RUNNING_STATE_RINGING) ? true : false;
}

std::string CallBase::GetAccountNumber()
{
    return accountNumber_;
}

int32_t CallBase::SetSpeakerphoneOn(bool speakerphoneOn)
{
    isSpeakerphoneOn_ = speakerphoneOn;
    return TELEPHONY_SUCCESS;
}

bool CallBase::IsSpeakerphoneOn()
{
    return isSpeakerphoneOn_;
}

bool CallBase::IsAliveState()
{
    return !(callState_ == TelCallState::CALL_STATUS_IDLE || callState_ == TelCallState::CALL_STATUS_DISCONNECTED ||
        callState_ == TelCallState::CALL_STATUS_DISCONNECTING);
}
} // namespace Telephony
} // namespace OHOS
