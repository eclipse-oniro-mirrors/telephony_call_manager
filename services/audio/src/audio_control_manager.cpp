/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#include "audio_player.h"
#include "call_state_processor.h"

#include "telephony_log_wrapper.h"

#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
AudioControlManager::AudioControlManager()
    : isTonePlaying_(false), isLocalRingbackNeeded_(false), ring_(nullptr), tone_(nullptr)
{}

AudioControlManager::~AudioControlManager() {}

void AudioControlManager::Init()
{
    DelayedSingleton<AudioDeviceManager>::GetInstance()->Init();
    DelayedSingleton<AudioSceneProcessor>::GetInstance()->Init();
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
    DelayedSingleton<AudioProxy>::GetInstance()->SetMicrophoneMute(false); // unmute microphone
}

void AudioControlManager::IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object ptr nullptr");
        return;
    }
    StopRingtone();
    DelayedSingleton<AudioProxy>::GetInstance()->SetMicrophoneMute(false); // unmute microphone
}

void AudioControlManager::HandleCallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    HandleNextState(callObjectPtr, nextState);
    if (priorState == nextState) {
        TELEPHONY_LOGI("prior state equals next state");
        return;
    }
    HandlePriorState(callObjectPtr, priorState);
}

void AudioControlManager::HandleNextState(sptr<CallBase> &callObjectPtr, TelCallState nextState)
{
    AudioEvent event = AudioEvent::UNKNOWN_EVENT;
    std::string number = callObjectPtr->GetAccountNumber();
    DelayedSingleton<CallStateProcessor>::GetInstance()->AddCall(number, nextState);
    switch (nextState) {
        case TelCallState::CALL_STATUS_ALERTING:
            event = AudioEvent::NEW_ALERTING_CALL;
            SetAudioInterruptState(AudioInterruptState::INTERRUPT_STATE_RINGING);
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            HandleNewActiveCall(callObjectPtr);
            SetAudioInterruptState(AudioInterruptState::INTERRUPT_STATE_ACTIVATED);
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            event = AudioEvent::NEW_INCOMING_CALL;
            SetAudioInterruptState(AudioInterruptState::INTERRUPT_STATE_RINGING);
            break;
        case TelCallState::CALL_STATUS_DISCONNECTED:
            SetAudioInterruptState(AudioInterruptState::INTERRUPT_STATE_DEACTIVATED);
            break;
        default:
            break;
    }
    if (event == AudioEvent::UNKNOWN_EVENT) {
        return;
    }
    DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(event);
}

void AudioControlManager::HandlePriorState(sptr<CallBase> &callObjectPtr, TelCallState priorState)
{
    AudioEvent event = AudioEvent::UNKNOWN_EVENT;
    std::string number = callObjectPtr->GetAccountNumber();
    DelayedSingleton<CallStateProcessor>::GetInstance()->DeleteCall(number, priorState);
    int32_t stateNumber = DelayedSingleton<CallStateProcessor>::GetInstance()->GetCallNumber(priorState);
    switch (priorState) {
        case TelCallState::CALL_STATUS_ALERTING:
            if (stateNumber == EMPTY_VALUE) {
                StopRingback(); // should stop ringtone while no more alerting calls
                event = AudioEvent::NO_MORE_ALERTING_CALL;
            }
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            if (stateNumber == EMPTY_VALUE) {
                bool result = StopRingtone(); // should stop ringtone while no more incoming calls
                if (result && callObjectPtr->GetCallRunningState() != CallRunningState::CALL_RUNNING_STATE_ACTIVE) {
                    AudioPlayer::ReleaseRenderer();
                }
                event = AudioEvent::NO_MORE_INCOMING_CALL;
            }
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            if (stateNumber == EMPTY_VALUE) {
                // unmute microphone while no more active call
                DelayedSingleton<AudioProxy>::GetInstance()->SetMicrophoneMute(false);
                event = AudioEvent::NO_MORE_ACTIVE_CALL;
            }
            break;
        default:
            break;
    }
    if (event == AudioEvent::UNKNOWN_EVENT) {
        return;
    }
    DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(event);
}

void AudioControlManager::HandleNewActiveCall(sptr<CallBase> &callObjectPtr)
{
    std::string number = callObjectPtr->GetAccountNumber();
    if (number.empty()) {
        TELEPHONY_LOGE("call object account number empty");
        return;
    }
    CallType callType = callObjectPtr->GetCallType();
    AudioEvent event = AudioEvent::UNKNOWN_EVENT;
    switch (callType) {
        case CallType::TYPE_CS:
            event = AudioEvent::NEW_ACTIVE_CS_CALL;
            break;
        case CallType::TYPE_IMS:
            event = AudioEvent::NEW_ACTIVE_IMS_CALL;
            break;
        case CallType::TYPE_OTT:
            event = AudioEvent::NEW_ACTIVE_OTT_CALL;
            break;
        default:
            break;
    }
    if (event == AudioEvent::UNKNOWN_EVENT) {
        return;
    }
    DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(event);
}

/**
 * @param device , audio device
 * usually called by the ui interaction , in purpose of switching to another audio device
 */
int32_t AudioControlManager::SetAudioDevice(AudioDevice device)
{
    AudioDevice audioDevice = AudioDevice::DEVICE_UNKNOWN;
    switch (device) {
        case AudioDevice::DEVICE_SPEAKER:
        case AudioDevice::DEVICE_EARPIECE:
        case AudioDevice::DEVICE_WIRED_HEADSET:
        case AudioDevice::DEVICE_BLUETOOTH_SCO:
            audioDevice = device;
            break;
        default:
            break;
    }
    if (audioDevice != AudioDevice::DEVICE_UNKNOWN &&
        DelayedSingleton<AudioDeviceManager>::GetInstance()->SwitchDevice(audioDevice)) {
        return TELEPHONY_SUCCESS;
    }
    return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
}

bool AudioControlManager::PlayRingtone()
{
    if (!ShouldPlayRingtone()) {
        TELEPHONY_LOGE("should not play ringtone");
        return false;
    }
    ring_ = std::make_unique<Ring>();
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("create ring object failed");
        return false;
    }
    if (ring_->Play() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play ringtone failed");
        return false;
    }
    TELEPHONY_LOGI("play ringtone success");
    return true;
}

bool AudioControlManager::PlayRingtone(const std::string &phoneNum)
{
    if (!IsNumberAllowed(phoneNum)) {
        TELEPHONY_LOGE("number is not allowed");
        return false;
    }
    if (!ShouldPlayRingtone()) {
        TELEPHONY_LOGE("should not play ringtone");
        return false;
    }
    ring_ = std::make_unique<Ring>();
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("create ring failed");
        return false;
    }
    if (ring_->Play() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play ringtone failed");
        return false;
    }
    TELEPHONY_LOGI("play ringtone success");
    return true;
}

bool AudioControlManager::PlayRingtone(const std::string &phoneNum, const std::string &ringtonePath)
{
    if (!IsNumberAllowed(phoneNum)) {
        TELEPHONY_LOGE("number is not allowed");
        return false;
    }
    if (!ShouldPlayRingtone()) {
        TELEPHONY_LOGE("should not play ringtone");
        return false;
    }
    ring_ = std::make_unique<Ring>(ringtonePath);
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("create ring failed");
        return false;
    }
    if (ring_->Play() == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play ringtone failed");
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
    if (event == AudioEvent::UNKNOWN_EVENT) {
        return;
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
        return AudioDevice::DEVICE_DISABLE;
    } else {
        // interrupted or ringing , priority : bt sco , wired headset , speaker
        if (AudioDeviceManager::IsBtScoConnected()) {
            return AudioDevice::DEVICE_BLUETOOTH_SCO;
        }
        if (AudioDeviceManager::IsWiredHeadsetConnected()) {
            return AudioDevice::DEVICE_WIRED_HEADSET;
        }
        if (audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_RINGING) {
            return AudioDevice::DEVICE_SPEAKER;
        }
        if (GetCurrentActiveCall() != nullptr && GetCurrentActiveCall()->IsSpeakerphoneOn()) {
            TELEPHONY_LOGI("current call speaker is active");
            return AudioDevice::DEVICE_SPEAKER;
        }
        if (AudioDeviceManager::IsEarpieceAvailable()) {
            return AudioDevice::DEVICE_EARPIECE;
        }
        return AudioDevice::DEVICE_SPEAKER;
    }
}

/**
 * @param isMute , mute state
 * usually called by the ui interaction , mute or unmute microphone
 */
int32_t AudioControlManager::SetMute(bool isMute)
{
    bool hasCall = DelayedSingleton<CallControlManager>::GetInstance()->HasCall();
    if (!hasCall) {
        TELEPHONY_LOGE("no call exists, set mute failed");
        return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
    }
    bool enabled = false;
    if ((DelayedSingleton<CallControlManager>::GetInstance()->HasEmergency(enabled) == TELEPHONY_SUCCESS) && enabled) {
        isMute = false;
    }
    if (DelayedSingleton<AudioProxy>::GetInstance()->SetMicrophoneMute(isMute)) {
        TELEPHONY_LOGI("set mute success , current mute state : %{public}d", isMute);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("set mute failed");
    return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
}

int32_t AudioControlManager::MuteRinger()
{
    if (ringState_ == RingState::STOPPED) {
        TELEPHONY_LOGI("ring already stopped");
        return TELEPHONY_SUCCESS;
    }
    if (ring_ != nullptr && ring_->Stop() == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("mute ring success");
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("mute ring failed");
    return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
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

std::set<sptr<CallBase>> AudioControlManager::GetCallList()
{
    return totalCalls_;
}

sptr<CallBase> AudioControlManager::GetCurrentActiveCall() const
{
    std::string number = DelayedSingleton<CallStateProcessor>::GetInstance()->GetCurrentActiveCall();
    if (!number.empty()) {
        return GetCallBase(number);
    }
    return nullptr;
}

sptr<CallBase> AudioControlManager::GetCallBase(const std::string &phoneNum) const
{
    sptr<CallBase> callBase = nullptr;
    if (phoneNum.empty()) {
        return callBase;
    }
    for (auto &call : totalCalls_) {
        if (call->GetAccountNumber() == phoneNum) {
            callBase = call;
            break;
        }
    }
    return callBase;
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

AudioInterruptState AudioControlManager::GetAudioInterruptState()
{
    return audioInterruptState_;
}

void AudioControlManager::SetVolumeAudible()
{
    DelayedSingleton<AudioProxy>::GetInstance()->SetVolumeAudible();
}

void AudioControlManager::SetRingState(RingState state)
{
    ringState_ = state;
}

void AudioControlManager::SetLocalRingbackNeeded(bool isNeeded)
{
    isLocalRingbackNeeded_ = isNeeded;
}

bool AudioControlManager::IsNumberAllowed(const std::string &phoneNum)
{
    // check whether the phone number is allowed or not , should not ring if number is not allowed
    return true;
}

bool AudioControlManager::ShouldPlayRingtone() const
{
    auto processor = DelayedSingleton<CallStateProcessor>::GetInstance();
    int32_t alertingCallNum = processor->GetCallNumber(TelCallState::CALL_STATUS_ALERTING);
    int32_t incomingCallNum = processor->GetCallNumber(TelCallState::CALL_STATUS_INCOMING);
    if (incomingCallNum != EXIST_ONLY_ONE_CALL || alertingCallNum > EMPTY_VALUE ||
        ringState_ == RingState::RINGING) {
        return false;
    }
    return true;
}

bool AudioControlManager::IsAudioActivated() const
{
    return audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_ACTIVATED ||
        audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_RINGING;
}

int32_t AudioControlManager::PlayCallTone(ToneDescriptor type)
{
    tone_ = std::make_unique<Tone>(type);
    if (tone_ == nullptr) {
        TELEPHONY_LOGE("create tone failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (tone_->Play() == TELEPHONY_SUCCESS) {
        isTonePlaying_ = true;
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("play call tone failed");
    return CALL_ERR_AUDIO_TONE_PLAY_FAILED;
}

int32_t AudioControlManager::StopCallTone()
{
    if (!isTonePlaying_) {
        TELEPHONY_LOGI("tone is already stopped");
        return TELEPHONY_SUCCESS;
    }
    if (tone_ != nullptr && tone_->Stop() == TELEPHONY_SUCCESS) {
        isTonePlaying_ = false;
        tone_ = nullptr;
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("stop call tone failed");
    return CALL_ERR_AUDIO_TONE_STOP_FAILED;
}

bool AudioControlManager::IsTonePlaying() const
{
    return isTonePlaying_;
}

bool AudioControlManager::IsCurrentRinging() const
{
    return ringState_ == RingState::RINGING;
}

int32_t AudioControlManager::PlayRingback()
{
    if (!isLocalRingbackNeeded_) {
        return CALL_ERR_AUDIO_TONE_PLAY_FAILED;
    }
    return PlayCallTone(ToneDescriptor::TONE_RINGBACK);
}

int32_t AudioControlManager::StopRingback()
{
    return StopCallTone();
}

int32_t AudioControlManager::PlayWaitingTone()
{
    return PlayCallTone(ToneDescriptor::TONE_WAITING);
}

int32_t AudioControlManager::StopWaitingTone()
{
    return StopCallTone();
}

void AudioControlManager::NewCallCreated(sptr<CallBase> &callObjectPtr) {}

void AudioControlManager::CallDestroyed(const DisconnectedDetails &details) {}
} // namespace Telephony
} // namespace OHOS