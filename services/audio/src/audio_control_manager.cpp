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

#include "audio_control_manager.h"

#include "telephony_log_wrapper.h"

#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
AudioControlManager::AudioControlManager() : isTonePlaying_(false), ring_(nullptr), tone_(nullptr) {}

AudioControlManager::~AudioControlManager()
{
    activeCalls_.clear();
    alertingCalls_.clear();
    incomingCalls_.clear();
    holdingCalls_.clear();
}

void AudioControlManager::Init()
{
    DelayedSingleton<AudioProxy>::GetInstance()->Init();
    DelayedSingleton<CallStateProcessor>::GetInstance()->Init();
    DelayedSingleton<AudioDeviceManager>::GetInstance()->Init();
}

void AudioControlManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object nullptr");
        return;
    }
    if (totalCalls_.count(callObjectPtr) == 0) {
        int32_t callId = callObjectPtr->GetCallID();
        TelCallState callState = callObjectPtr->GetTelCallState();
        TELEPHONY_LOGI("add new call , call id : %{public}d , call state : %{public}d", callId, callState);
        totalCalls_.insert(callObjectPtr);
    }
    HandleCallStateUpdated(callObjectPtr, priorState, nextState);
    if (nextState == TelCallState::CALL_STATUS_DISCONNECTED && totalCalls_.count(callObjectPtr) > 0) {
        totalCalls_.erase(callObjectPtr);
    }
}

void AudioControlManager::IncomingCallActivated(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object ptr nullptr");
        return;
    }
    StopRingtone();
    SetMute(false); // unmute microphone
}

void AudioControlManager::IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object ptr nullptr");
        return;
    }
    StopRingtone();
    SetMute(true); // mute microphone
}

void AudioControlManager::NewCallCreated(sptr<CallBase> &callObjectPtr) {}

void AudioControlManager::CallDestroyed(sptr<CallBase> &callObjectPtr) {}

void AudioControlManager::HandleCallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    AddCall(callObjectPtr->GetAccountNumber(), nextState);
    switch (nextState) {
        case TelCallState::CALL_STATUS_ALERTING:
            DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(AudioEvent::NEW_ALERTING_CALL);
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            AddNewActiveCall(callObjectPtr->GetCallType());
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(AudioEvent::NEW_INCOMING_CALL);
            break;
        default:
            break;
    }
    if (priorState == nextState) {
        TELEPHONY_LOGI("prior state equals next state");
        return;
    }
    DeleteCall(callObjectPtr->GetAccountNumber(), priorState);
    switch (priorState) {
        case TelCallState::CALL_STATUS_ALERTING:
            if (alertingCalls_.empty()) {
                StopRingback(); // should stop ringback tone while no more alerting calls
                DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::NO_MORE_ALERTING_CALL);
            }
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            if (activeCalls_.empty()) {
                DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(AudioEvent::NO_MORE_ACTIVE_CALL);
            }
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            if (incomingCalls_.empty()) {
                StopRingtone(); // should stop ringtone while no more incoming calls
                DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::NO_MORE_INCOMING_CALL);
            }
            break;
        default:
            break;
    }
}

void AudioControlManager::AddNewActiveCall(CallType callType)
{
    switch (callType) {
        case CallType::TYPE_CS:
            DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(AudioEvent::NEW_ACTIVE_CS_CALL);
            break;
        case CallType::TYPE_IMS:
            DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(AudioEvent::NEW_ACTIVE_IMS_CALL);
            break;
        default:
            break;
    }
}

void AudioControlManager::AddCall(const std::string &phoneNum, TelCallState state)
{
    switch (state) {
        case TelCallState::CALL_STATUS_ALERTING:
            if (alertingCalls_.count(phoneNum) == 0) {
                TELEPHONY_LOGI("add call , state : alerting");
                alertingCalls_.insert(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            if (incomingCalls_.count(phoneNum) == 0) {
                TELEPHONY_LOGI("add call , state : incoming");
                incomingCalls_.insert(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            if (activeCalls_.count(phoneNum) == 0) {
                TELEPHONY_LOGI("add call , state : active");
                activeCalls_.insert(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            if (holdingCalls_.count(phoneNum) == 0) {
                TELEPHONY_LOGI("add call , state : holding");
                holdingCalls_.insert(phoneNum);
            }
            break;
        default:
            break;
    }
}

void AudioControlManager::DeleteCall(const std::string &phoneNum, TelCallState state)
{
    switch (state) {
        case TelCallState::CALL_STATUS_ALERTING:
            if (alertingCalls_.count(phoneNum) > 0) {
                TELEPHONY_LOGI("erase call , state : alerting");
                alertingCalls_.erase(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            if (incomingCalls_.count(phoneNum) > 0) {
                TELEPHONY_LOGI("erase call , state : incoming");
                incomingCalls_.erase(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            if (activeCalls_.count(phoneNum) > 0) {
                TELEPHONY_LOGI("erase call , state : active");
                activeCalls_.erase(phoneNum);
            }
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            if (holdingCalls_.count(phoneNum) > 0) {
                TELEPHONY_LOGI("erase call , state : holding");
                holdingCalls_.erase(phoneNum);
            }
            break;
        default:
            break;
    }
}

bool AudioControlManager::SetAudioDevice(int32_t device)
{
    AudioDevice audioDevice = AudioDevice::DEVICE_UNKNOWN;
    switch (device) {
        case (int32_t)AudioDevice::DEVICE_BLUETOOTH_SCO:
            audioDevice = AudioDevice::DEVICE_BLUETOOTH_SCO;
            break;
        case (int32_t)AudioDevice::DEVICE_WIRED_HEADSET:
            audioDevice = AudioDevice::DEVICE_WIRED_HEADSET;
            break;
        case (int32_t)AudioDevice::DEVICE_SPEAKER:
            audioDevice = AudioDevice::DEVICE_SPEAKER;
            break;
        case (int32_t)AudioDevice::DEVICE_EARPIECE:
            audioDevice = AudioDevice::DEVICE_EARPIECE;
            break;
        default:
            break;
    }
    return SetAudioDevice(audioDevice);
}

/**
 * @param device , audio device
 * usually called by the ui interaction , in purpose of switching to another audio device
 */
bool AudioControlManager::SetAudioDevice(AudioDevice device)
{
    return DelayedSingleton<AudioDeviceManager>::GetInstance()->SwitchDevice(device);
}

bool AudioControlManager::PlayRingtone()
{
    // should play ringtone while there exists only one incoming call
    if (!IsOnlyOneIncomingCall() || IsAlertingCallExists() || ringState_ == RingState::RINGING) {
        TELEPHONY_LOGE("play ringtone failed");
        return false;
    }
    ring_ = std::make_unique<AudioRing>();
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("create ring object failed");
        return false;
    }
    if (ring_->Play() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ringtone play failed");
        return false;
    }
    return true;
}

// play default ring
bool AudioControlManager::PlayRingtone(const std::string &phoneNum)
{
    // should play ringtone while there exists only one incoming call
    if (phoneNum.empty() || !IsAllowNumber(phoneNum) || !IsOnlyOneIncomingCall() || IsAlertingCallExists() ||
        ringState_ == RingState::RINGING) {
        TELEPHONY_LOGE("play ringtone failed");
        return false;
    }
    ring_ = std::make_unique<AudioRing>();
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("create ring failed");
        return false;
    }
    if (ring_->Play() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ringtone play failed");
        return false;
    }
    TELEPHONY_LOGI("play ringtone success");
    return true;
}

// play specific ring
bool AudioControlManager::PlayRingtone(const std::string &phoneNum, const std::string &ringtonePath)
{
    // should play ringtone while there exists only one incoming call
    if (phoneNum.empty() || !IsAllowNumber(phoneNum) || !IsOnlyOneIncomingCall() || IsAlertingCallExists() ||
        ringState_ == RingState::RINGING) {
        TELEPHONY_LOGE("play ringtone failed");
        return false;
    }
    ring_ = std::make_unique<AudioRing>(ringtonePath);
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("create ring failed");
        return false;
    }
    if (ring_->Play() == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ringtone play failed");
        return false;
    }
    TELEPHONY_LOGI("play ringtone success");
    return true;
}

bool AudioControlManager::StopRingtone()
{
    if (ringState_ == RingState::STOPPED) {
        TELEPHONY_LOGI("ringtone already stopped");
        return true;
    }
    if (ring_ != nullptr && ring_->Stop() == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("stop ringtone success");
        ring_ = nullptr;
        return true;
    }
    TELEPHONY_LOGE("stop ringtone failed");
    return false;
}

void AudioControlManager::SetAudioInterruptState(AudioInterruptState state)
{
    TELEPHONY_LOGI("set audio interrupt state : %{public}d", state);
    audioInterruptState_ = state;
    /**
     * send audio interrupt event to audio device manager , maybe need to reinitialize the audio device
     */
    AudioEvent event = AudioEvent::UNKNOWN_EVENT;
    switch (audioInterruptState_) {
        case AudioInterruptState::INTERRUPT_STATE_ACTIVATED:
            event = AudioEvent::AUDIO_ACTIVATED;
            break;
        case AudioInterruptState::INTERRUPT_STATE_DEACTIVATED:
            event = AudioEvent::AUDIO_DEACTIVATED;
            break;
        case AudioInterruptState::INTERRUPT_STATE_RINGING:
            event = AudioEvent::AUDIO_RINGING;
            break;
        default:
            break;
    }
    DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(event);
}

/**
 * while audio state changed , maybe need to reinitialize the audio device
 * in order to get the initialization status of audio device , need to consider varieties of  audio conditions
 */
AudioDevice AudioControlManager::GetInitAudioDevice() const
{
    if (audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_DEACTIVATED) {
        // disable device
        return AudioDevice::DEVICE_DISABLE;
    } else {
        // interrupted or ringing , priority : bt sco , wired headset , speaker
        if (AudioDeviceManager::IsBtScoAvailable()) {
            return AudioDevice::DEVICE_BLUETOOTH_SCO;
        }
        if (AudioDeviceManager::IsWiredHeadsetAvailable()) {
            return AudioDevice::DEVICE_WIRED_HEADSET;
        }
        if (audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_RINGING) {
            return AudioDevice::DEVICE_SPEAKER;
        }
        if (GetCurrentCall() != nullptr && GetCurrentCall()->IsSpeakerphoneOn()) {
            TELEPHONY_LOGI("current call speaker is active");
            return AudioDevice::DEVICE_SPEAKER;
        }
        if (AudioDeviceManager::IsEarpieceAvailable()) {
            return AudioDevice::DEVICE_EARPIECE;
        }
        return AudioDevice::DEVICE_SPEAKER;
    }
}

bool AudioControlManager::UpdateCurrentCallState()
{
    if (IsActiveCallExists()) {
        // no need to update call state while active calls exists
        return false;
    }
    AudioEvent event = AudioEvent::UNKNOWN_EVENT;
    if (IsHoldingCallExists()) {
        event = AudioEvent::SWITCH_HOLDING_STATE;
    } else if (IsIncomingCallExists()) {
        event = AudioEvent::SWITCH_INCOMING_STATE;
    } else {
        event = AudioEvent::SWITCH_AUDIO_INACTIVE_STATE;
    }
    return DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(event);
}

// mute or unmute microphone
int32_t AudioControlManager::SetMute(bool isMute)
{
    if (DelayedSingleton<CallControlManager>::GetInstance()->HasEmergency()) {
        isMute = false;
    }
    if (DelayedSingleton<AudioProxy>::GetInstance()->SetMicrophoneMute(isMute)) {
        TELEPHONY_LOGI("set mute success , current mute state : %{public}d", isMute);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("set mute failed");
    return TELEPHONY_ERR_FAIL;
}

int32_t AudioControlManager::MuteRinger()
{
    if (ringState_ == RingState::STOPPED) {
        TELEPHONY_LOGI("ring alreay stopped");
        return TELEPHONY_SUCCESS;
    }
    if (ring_ != nullptr && ring_->Stop() == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("mute ring success");
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("mute ring failed");
    return TELEPHONY_ERR_FAIL;
}

void AudioControlManager::PlayCallEndedTone(TelCallState priorState, TelCallState nextState, CallEndedType type)
{
    if (nextState != TelCallState::CALL_STATUS_DISCONNECTED) {
        return;
    }
    if (priorState == TelCallState::CALL_STATUS_ACTIVE || priorState == TelCallState::CALL_STATUS_DIALING ||
        priorState == TelCallState::CALL_STATUS_HOLDING) {
        switch (type) {
            case CallEndedType::PHONE_IS_BUSY:
                PlayCallTone(ToneDescriptor::TONE_ENGAGED);
                break;
            case CallEndedType::CALL_ENDED_NORMALLY:
                PlayCallTone(ToneDescriptor::TONE_FINISHED);
                break;
            case CallEndedType::UNKNOWN:
                PlayCallTone(ToneDescriptor::TONE_UNKNOWN);
                break;
            case CallEndedType::INVALID_NUMBER:
                PlayCallTone(ToneDescriptor::TONE_INVALID_NUMBER);
                break;
            default:
                break;
        }
    }
}

sptr<CallBase> AudioControlManager::GetCallBase(const std::string &phoneNum) const
{
    sptr<CallBase> retPtr;
    if (phoneNum.empty()) {
        return retPtr;
    }
    for (auto &call : totalCalls_) {
        if (call->GetAccountNumber() == phoneNum) {
            retPtr = call;
            break;
        }
    }
    return retPtr;
}

std::string AudioControlManager::GetIncomingCallRingtonePath()
{
    if (incomingCalls_.size() == EXIST_ONLY_ONE_CALL) {
        ContactInfo info;
        sptr<CallBase> incomingCall = GetCallBase(*incomingCalls_.begin());
        if (incomingCall != nullptr) {
            incomingCall->GetCallerInfo(info);
            std::string path(info.ringtonePath);
            return path;
        }
    }
    return "";
}

sptr<CallBase> AudioControlManager::GetCurrentCall() const
{
    if (IsOnlyOneActiveCall()) {
        return GetCallBase(*activeCalls_.begin());
    }
    return nullptr;
}

std::set<sptr<CallBase>> AudioControlManager::GetCallList()
{
    return totalCalls_;
}

AudioInterruptState AudioControlManager::GetAudioInterruptState()
{
    return audioInterruptState_;
}

void AudioControlManager::SetVolumeAudible()
{
    DelayedSingleton<AudioProxy>::GetInstance()->SetVolumeAudible();
}

bool AudioControlManager::IsAllowNumber(const std::string &phoneNum)
{
    // check whether the phone number is allow or not , should not ring if number is not allow
    return false;
}

bool AudioControlManager::IsCurrentVideoCall() const
{
    return false;
}

void AudioControlManager::SetRingState(RingState state)
{
    ringState_ = state;
}

bool AudioControlManager::IsActiveCallExists() const
{
    return !activeCalls_.empty();
}

bool AudioControlManager::IsHoldingCallExists() const
{
    return !holdingCalls_.empty();
}

bool AudioControlManager::IsIncomingCallExists() const
{
    return !incomingCalls_.empty();
}

bool AudioControlManager::IsAlertingCallExists() const
{
    return !alertingCalls_.empty();
}

bool AudioControlManager::IsEmergencyCallExists() const
{
    for (auto call : totalCalls_) {
        if (call->GetEmergencyState()) {
            return true;
        }
    }
    return false;
}

bool AudioControlManager::IsTonePlaying() const
{
    return isTonePlaying_;
}

bool AudioControlManager::IsCurrentRinging() const
{
    return ringState_ == RingState::RINGING;
}

bool AudioControlManager::IsAudioActivated() const
{
    return audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_ACTIVATED ||
        audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_RINGING;
}

bool AudioControlManager::IsOnlyOneActiveCall() const
{
    return activeCalls_.size() == EXIST_ONLY_ONE_CALL;
}

bool AudioControlManager::IsOnlyOneAlertingCall() const
{
    return alertingCalls_.size() == EXIST_ONLY_ONE_CALL;
}

bool AudioControlManager::IsOnlyOneIncomingCall() const
{
    return incomingCalls_.size() == EXIST_ONLY_ONE_CALL;
}

bool AudioControlManager::ShouldSwitchAlertingState() const
{
    return alertingCalls_.size() == EXIST_ONLY_ONE_CALL && incomingCalls_.empty() && activeCalls_.empty();
}

bool AudioControlManager::ShouldSwitchIncomingState() const
{
    return incomingCalls_.size() == EXIST_ONLY_ONE_CALL && alertingCalls_.empty() && activeCalls_.empty();
}

int32_t AudioControlManager::PlayCallTone(ToneDescriptor type)
{
    tone_ = std::make_unique<AudioTone>(type);
    if (tone_ == nullptr) {
        TELEPHONY_LOGE("create tone failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (tone_->Play() == TELEPHONY_SUCCESS) {
        isTonePlaying_ = true;
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

int32_t AudioControlManager::StopCallTone()
{
    if (tone_ != nullptr && tone_->Stop() == TELEPHONY_SUCCESS) {
        isTonePlaying_ = false;
        tone_ = nullptr;
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

void AudioControlManager::TurnOffVoice()
{
    if (ring_ != nullptr) {
        ring_->Stop();
    }
    if (tone_ != nullptr) {
        tone_->Stop();
    }
}

int32_t AudioControlManager::PlayRingback()
{
    // should play ringback tone while there exists only one alerting call
    if (alertingCalls_.size() != EXIST_ONLY_ONE_CALL || IsIncomingCallExists()) {
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t AudioControlManager::StopRingback()
{
    return TELEPHONY_SUCCESS;
}

int32_t AudioControlManager::PlayWaitingTone()
{
    return PlayCallTone(ToneDescriptor::TONE_WAITING);
}

int32_t AudioControlManager::StopWaitingTone()
{
    return StopCallTone();
}

int32_t AudioControlManager::PlayHoldingTone()
{
    return PlayCallTone(ToneDescriptor::TONE_WAITING);
}

int32_t AudioControlManager::StopHoldingTone()
{
    return StopCallTone();
}
} // namespace Telephony
} // namespace OHOS