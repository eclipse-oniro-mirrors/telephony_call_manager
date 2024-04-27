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

#include "audio_control_manager.h"
#include "bluetooth_call_manager.h"
#include "call_manager_errors.h"
#include "cellular_call_connection.h"
#include "common_type.h"
#include "ffrt.h"
#include "telephony_log_wrapper.h"
#include "voip_call.h"
#include "voip_call_connection.h"
#include "call_manager_info.h"

namespace OHOS {
namespace Telephony {
CallBase::CallBase(DialParaInfo &info)
    : callId_(info.callId), callType_(info.callType), videoState_(info.videoState), accountNumber_(info.number),
      bundleName_(info.bundleName), callRunningState_(CallRunningState::CALL_RUNNING_STATE_CREATE),
      conferenceState_(TelConferenceState::TEL_CONFERENCE_IDLE), startTime_(0),
      direction_(CallDirection::CALL_DIRECTION_IN), policyFlag_(0), callState_(info.callState), autoAnswerState_(false),
      canUnHoldState_(true), canSwitchCallState_(true), answerVideoState_(0), isSpeakerphoneOn_(false),
      callEndedType_(CallEndedType::UNKNOWN), callBeginTime_(0), callEndTime_(0), ringBeginTime_(0), ringEndTime_(0),
      answerType_(CallAnswerType::CALL_ANSWER_MISSED), accountId_(info.accountId), crsType_(info.crsType),
      originalCallType_(info.originalCallType), isMuted_(false), numberLocation_("default"), blockReason_(0)
{
    (void)memset_s(&contactInfo_, sizeof(ContactInfo), 0, sizeof(ContactInfo));
    (void)memset_s(&numberMarkInfo_, sizeof(NumberMarkInfo), 0, sizeof(NumberMarkInfo));
}

CallBase::CallBase(DialParaInfo &info, AppExecFwk::PacMap &extras)
    : callId_(info.callId), callType_(info.callType), videoState_(info.videoState), accountNumber_(info.number),
      bundleName_(info.bundleName), callRunningState_(CallRunningState::CALL_RUNNING_STATE_CREATE),
      conferenceState_(TelConferenceState::TEL_CONFERENCE_IDLE), startTime_(0),
      direction_(CallDirection::CALL_DIRECTION_OUT), policyFlag_(0), callState_(info.callState),
      autoAnswerState_(false), canUnHoldState_(true), canSwitchCallState_(true), answerVideoState_(0),
      isSpeakerphoneOn_(false), callEndedType_(CallEndedType::UNKNOWN), callBeginTime_(0), callEndTime_(0),
      ringBeginTime_(0), ringEndTime_(0), answerType_(CallAnswerType::CALL_ANSWER_MISSED), accountId_(info.accountId),
      crsType_(info.crsType), originalCallType_(info.originalCallType), isMuted_(false), numberLocation_("default"),
      blockReason_(0)
{
    (void)memset_s(&contactInfo_, sizeof(ContactInfo), 0, sizeof(ContactInfo));
    (void)memset_s(&numberMarkInfo_, sizeof(NumberMarkInfo), 0, sizeof(NumberMarkInfo));
}

CallBase::~CallBase() {}

int32_t CallBase::DialCallBase()
{
    std::lock_guard<std::mutex> lock(mutex_);
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_CONNECTING;
    TELEPHONY_LOGI("start to set audio");
    // Set audio, set hands-free
    ffrt::submit([=]() { HangUpVoipCall(); });
    return TELEPHONY_SUCCESS;
}

void CallBase::HangUpVoipCall()
{
    std::vector<CallAttributeInfo> callAttributeInfo = CallObjectManager::GetAllCallInfoList();
    std::vector<CallAttributeInfo>::iterator it = callAttributeInfo.begin();
    while (it != callAttributeInfo.end()) {
        CallAttributeInfo callinfo = (*it);
        TelCallState callState = callinfo.callState;
        ++it;
        if (callinfo.callType == CallType::TYPE_VOIP &&
            (callState == TelCallState::CALL_STATUS_ACTIVE || callState == TelCallState::CALL_STATUS_INCOMING)) {
            sptr<CallBase> tempCall = CallObjectManager::GetOneCallObject(callinfo.callId);
            sptr<VoIPCall> call = static_cast<VoIPCall *>(static_cast<void *>(tempCall.GetRefPtr()));
            if (call == nullptr) {
                TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callinfo.callId);
                break;
            }
            call->HangUpCall(ErrorReason::CELLULAR_CALL_EXISTS);
        }
    }
}

int32_t CallBase::IncomingCallBase()
{
    std::lock_guard<std::mutex> lock(mutex_);
    callRunningState_ = CallRunningState::CALL_RUNNING_STATE_RINGING;
    return TELEPHONY_SUCCESS;
}

int32_t CallBase::AnswerCallBase()
{
    if (!IsCurrentRinging()) {
        TELEPHONY_LOGW("the device is currently not ringing");
        return CALL_ERR_PHONE_ANSWER_IS_BUSY;
    }
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
    if (accountNumber_.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return;
    }
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
        info.accountId = accountId_;
        info.crsType = crsType_;
        info.originalCallType = originalCallType_;
        (void)memset_s(info.numberLocation, kMaxNumberLen, 0, kMaxNumberLen);
        (void)memcpy_s(info.numberLocation, kMaxNumberLen, numberLocation_.c_str(), numberLocation_.length());
        info.numberMarkInfo = numberMarkInfo_;
        info.blockReason = blockReason_;
        if (bundleName_.length() > static_cast<size_t>(kMaxBundleNameLen)) {
            TELEPHONY_LOGE("Number out of limit!");
            return;
        }
        errno_t result = memcpy_s(info.bundleName, kMaxBundleNameLen, bundleName_.c_str(), bundleName_.length());
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
    if (callRunningState_ != CallRunningState::CALL_RUNNING_STATE_CREATE && callState_ == nextState &&
        nextState != TelCallState::CALL_STATUS_DIALING) {
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

void CallBase::SetAutoAnswerState(bool flag)
{
    std::lock_guard<std::mutex> lock(mutex_);
    autoAnswerState_ = flag;
    TELEPHONY_LOGI("NeedAutoAnswer:%{public}d", autoAnswerState_);
}

bool CallBase::GetAutoAnswerState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return autoAnswerState_;
}

void CallBase::SetAnswerVideoState(int32_t videoState)
{
    std::lock_guard<std::mutex> lock(mutex_);
    answerVideoState_ = videoState;
    TELEPHONY_LOGI("set answer video state :%{public}d", answerVideoState_);
}

int32_t CallBase::GetAnswerVideoState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return answerVideoState_;
}

void CallBase::SetCanUnHoldState(bool flag)
{
    std::lock_guard<std::mutex> lock(mutex_);
    canUnHoldState_ = flag;
    TELEPHONY_LOGI("CanUnHoldState:%{public}d", canUnHoldState_);
}

bool CallBase::GetCanUnHoldState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    TELEPHONY_LOGI("CanUnHoldState:%{public}d", canUnHoldState_);
    return canUnHoldState_;
}

void CallBase::SetCanSwitchCallState(bool flag)
{
    std::lock_guard<std::mutex> lock(mutex_);
    canSwitchCallState_ = flag;
    TELEPHONY_LOGI("CanSwitchCallState:%{public}d", canSwitchCallState_);
}

bool CallBase::GetCanSwitchCallState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    TELEPHONY_LOGI("CanSwitchCallState:%{public}d", canSwitchCallState_);
    return canSwitchCallState_;
}

void CallBase::SetTelConferenceState(TelConferenceState state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    conferenceState_ = state;
    TELEPHONY_LOGI("SetTelConferenceState, callId:%{public}d, state:%{public}d", callId_, state);
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

int32_t CallBase::GetCrsType()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return crsType_;
}

void CallBase::SetCrsType(int32_t crsType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    crsType_ = crsType;
}

int32_t CallBase::GetOriginalCallType()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return originalCallType_;
}

void CallBase::SetOriginalCallType(int32_t originalCallType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    originalCallType_ = originalCallType;
}

void CallBase::SetNumberLocation(std::string numberLocation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    numberLocation_ = numberLocation;
}

std::string CallBase::GetNumberLocation()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return numberLocation_;
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

ContactInfo CallBase::GetCallerInfo()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return contactInfo_;
}

void CallBase::SetCallerInfo(const ContactInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    contactInfo_ = info;
}

NumberMarkInfo CallBase::GetNumberMarkInfo()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return numberMarkInfo_;
}

void CallBase::SetNumberMarkInfo(const NumberMarkInfo &numberMarkInfo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    numberMarkInfo_ = numberMarkInfo;
}

void CallBase::SetBlockReason(const int32_t &blockReason)
{
    std::lock_guard<std::mutex> lock(mutex_);
    blockReason_ = blockReason;
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

void CallBase::SetCallId(int32_t callId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    callId_ = callId;
}

bool CallBase::CheckVoicemailNumber(std::string phoneNumber)
{
    return false;
}

bool CallBase::IsSpeakerphoneEnabled()
{
    std::shared_ptr<BluetoothCallManager> bluetoothCallManager = std::make_shared<BluetoothCallManager>();
    // Gets whether the device can be started from the configuration
    if (bluetoothCallManager->IsBtAvailble()) {
        return false;
    }
    return true;
}

bool CallBase::IsCurrentRinging()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return (callRunningState_ == CallRunningState::CALL_RUNNING_STATE_RINGING) ? true : false;
}

std::string CallBase::GetAccountNumber()
{
    return accountNumber_;
}

void CallBase::SetAccountNumber(const std::string accountNumber)
{
    accountNumber_ = accountNumber;
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

void CallBase::SetBundleName(const char *bundleName)
{
    bundleName_ = bundleName;
}

void CallBase::SetCallType(CallType callType)
{
    callType_ = callType;
}

int32_t CallBase::SetMicPhoneState(bool isMuted)
{
    isMuted_ = isMuted;
    return TELEPHONY_SUCCESS;
}

bool CallBase::IsMuted()
{
    return isMuted_;
}
} // namespace Telephony
} // namespace OHOS
