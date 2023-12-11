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

#include "call_control_manager.h"
#include "call_state_processor.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t DTMF_PLAY_TIME = 30;

AudioControlManager::AudioControlManager()
    : isLocalRingbackNeeded_(false), ring_(nullptr), tone_(nullptr), sound_(nullptr)
{}

AudioControlManager::~AudioControlManager()
{
    DelayedSingleton<AudioProxy>::GetInstance()->UnsetDeviceChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->UnsetAudioPreferDeviceChangeCallback();
}

void AudioControlManager::Init()
{
    DelayedSingleton<AudioDeviceManager>::GetInstance()->Init();
    DelayedSingleton<AudioSceneProcessor>::GetInstance()->Init();
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioDeviceChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioPreferDeviceChangeCallback();
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
    if(nextState == TelCallState::CALL_STATUS_ANSWERED) {
        TELEPHONY_LOGI("NO NEED TO UPDATE");
        return;
    }
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
    DelayedSingleton<CallStateProcessor>::GetInstance()->AddCall(callObjectPtr->GetCallID(), nextState);
    switch (nextState) {
        case TelCallState::CALL_STATUS_DIALING:
            event = AudioEvent::NEW_DIALING_CALL;
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_RINGING;
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            event = AudioEvent::NEW_ALERTING_CALL;
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_RINGING;
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            HandleNewActiveCall(callObjectPtr);
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            event = AudioEvent::NEW_INCOMING_CALL;
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_RINGING;
            break;
        case TelCallState::CALL_STATUS_DISCONNECTING:
        case TelCallState::CALL_STATUS_DISCONNECTED:
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_DEACTIVATED;
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
    DelayedSingleton<CallStateProcessor>::GetInstance()->DeleteCall(callObjectPtr->GetCallID(), priorState);
    int32_t stateNumber = DelayedSingleton<CallStateProcessor>::GetInstance()->GetCallNumber(priorState);
    switch (priorState) {
        case TelCallState::CALL_STATUS_DIALING:
            if (stateNumber == EMPTY_VALUE) {
                StopRingback(); // should stop ringtone while no more alerting calls
                event = AudioEvent::NO_MORE_DIALING_CALL;
            }
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            if (stateNumber == EMPTY_VALUE) {
                StopRingback(); // should stop ringtone while no more alerting calls
                event = AudioEvent::NO_MORE_ALERTING_CALL;
            }
            break;
        case TelCallState::CALL_STATUS_INCOMING:
        case TelCallState::CALL_STATUS_WAITING:
            if (stateNumber == EMPTY_VALUE) {
                if (callObjectPtr->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_ACTIVE) {
                    PlaySoundtone();
                } else {
                    StopRingtone();
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
            StopRingback();
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            if (stateNumber == EMPTY_VALUE) {
                event = AudioEvent::NO_MORE_HOLDING_CALL;
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
int32_t AudioControlManager::SetAudioDevice(const AudioDevice &device)
{
    AudioDeviceType audioDeviceType = AudioDeviceType::DEVICE_UNKNOWN;
    switch (device.deviceType) {
        case AudioDeviceType::DEVICE_SPEAKER:
        case AudioDeviceType::DEVICE_EARPIECE:
        case AudioDeviceType::DEVICE_WIRED_HEADSET:
            audioDeviceType = device.deviceType;
            break;
        default:
            break;
    }
    if (audioDeviceType != AudioDeviceType::DEVICE_UNKNOWN &&
        DelayedSingleton<AudioDeviceManager>::GetInstance()->SwitchDevice(audioDeviceType)) {
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
    if (ring_ == nullptr) {
        ring_ = std::make_unique<Ring>();
        if (ring_ == nullptr) {
            TELEPHONY_LOGE("create ring object failed");
            return false;
        }
    }
    sptr<CallBase> incomingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (incomingCall == nullptr) {
        TELEPHONY_LOGE("incomingCall is nullptr");
        return false;
    }
    CallAttributeInfo info;
    incomingCall->GetCallAttributeBaseInfo(info);
    if (ring_->Play(info.accountId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play ringtone failed");
        return false;
    }
    TELEPHONY_LOGI("play ringtone success");
    return true;
}

bool AudioControlManager::PlaySoundtone()
{
    if (soundState_ == SoundState::SOUNDING) {
        TELEPHONY_LOGE("should not play soundTone");
        return false;
    }
    if (sound_ == nullptr) {
        sound_ = std::make_unique<Sound>();
        if (sound_ == nullptr) {
            TELEPHONY_LOGE("create sound object failed");
            return false;
        }
    }
    if (sound_->Play() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play soundtone failed");
        return false;
    }
    TELEPHONY_LOGI("play soundtone success");
    return true;
}

bool AudioControlManager::StopSoundtone()
{
    if (soundState_ == SoundState::STOPPED) {
        TELEPHONY_LOGI("soundtone already stopped");
        return true;
    }
    if (sound_ == nullptr) {
        TELEPHONY_LOGE("sound_ is nullptr");
        return false;
    }
    if (sound_->Stop() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop soundtone failed");
        return false;
    }
    sound_->ReleaseRenderer();
    TELEPHONY_LOGI("stop soundtone success");
    return true;
}

bool AudioControlManager::StopRingtone()
{
    if (ringState_ == RingState::STOPPED) {
        TELEPHONY_LOGI("ringtone already stopped");
        return true;
    }
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("ring_ is nullptr");
        return false;
    }
    if (ring_->Stop() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop ringtone failed");
        return false;
    }
    ring_->ReleaseRenderer();
    TELEPHONY_LOGI("stop ringtone success");
    return true;
}

/**
 * while audio state changed , maybe need to reinitialize the audio device
 * in order to get the initialization status of audio device , need to consider varieties of  audio conditions
 */
AudioDeviceType AudioControlManager::GetInitAudioDeviceType() const
{
    if (audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_DEACTIVATED) {
        return AudioDeviceType::DEVICE_DISABLE;
    } else {
        /**
         * Init audio device type according to the priority in different call state:
         * In voice call state, bluetooth sco > wired headset > earpiece > speaker
         * In video call state, bluetooth sco > wired headset > speaker > earpiece
         */
        if (AudioDeviceManager::IsBtScoConnected()) {
            return AudioDeviceType::DEVICE_BLUETOOTH_SCO;
        }
        if (AudioDeviceManager::IsWiredHeadsetConnected()) {
            return AudioDeviceType::DEVICE_WIRED_HEADSET;
        }
        sptr<CallBase> liveCall = CallObjectManager::GetForegroundLiveCall();
        if (liveCall != nullptr && liveCall->GetVideoStateType() == VideoStateType::TYPE_VIDEO) {
            TELEPHONY_LOGI("current video call speaker is active");
            return AudioDeviceType::DEVICE_SPEAKER;
        }
        if (AudioDeviceManager::IsEarpieceAvailable()) {
            return AudioDeviceType::DEVICE_EARPIECE;
        }
        return AudioDeviceType::DEVICE_SPEAKER;
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
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("ring is nullptr");
        return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
    }
    if (ring_->SetMute() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetMute fail");
        return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
    }
    TELEPHONY_LOGI("mute ring success");
    return TELEPHONY_SUCCESS;
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
    std::lock_guard<std::mutex> lock(mutex_);
    return totalCalls_;
}

sptr<CallBase> AudioControlManager::GetCurrentActiveCall()
{
    int32_t callId = DelayedSingleton<CallStateProcessor>::GetInstance()->GetCurrentActiveCall();
    if (callId != INVALID_CALLID) {
        return GetCallBase(callId);
    }
    return nullptr;
}

sptr<CallBase> AudioControlManager::GetCallBase(int32_t callId)
{
    sptr<CallBase> callBase = nullptr;
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &call : totalCalls_) {
        if (call->GetCallID() == callId) {
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

void AudioControlManager::SetSoundState(SoundState state)
{
    soundState_ = state;
}

void AudioControlManager::SetToneState(ToneState state)
{
    toneState_ = state;
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
    if (incomingCallNum != EXIST_ONLY_ONE_CALL || alertingCallNum > EMPTY_VALUE || ringState_ == RingState::RINGING) {
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
    if (toneState_ == ToneState::TONEING) {
        TELEPHONY_LOGE("should not play callTone");
        return CALL_ERR_AUDIO_TONE_PLAY_FAILED;
    }
    toneState_ = ToneState::TONEING;
    if (tone_ == nullptr) {
        tone_ = std::make_unique<Tone>(type);
        if (tone_ == nullptr) {
            TELEPHONY_LOGE("create tone failed");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
    }
    if (tone_->Play() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play calltone failed");
        return CALL_ERR_AUDIO_TONE_PLAY_FAILED;
    }
    TELEPHONY_LOGI("play calltone success");
    return TELEPHONY_SUCCESS;
}

int32_t AudioControlManager::StopCallTone()
{
    if (toneState_ == ToneState::STOPPED) {
        TELEPHONY_LOGI("tone is already stopped");
        return TELEPHONY_SUCCESS;
    }
    if (tone_ == nullptr) {
        TELEPHONY_LOGE("tone_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (tone_->Stop() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop calltone failed");
        return CALL_ERR_AUDIO_TONE_STOP_FAILED;
    }
    tone_->ReleaseRenderer();
    tone_ = nullptr;
    toneState_ = ToneState::STOPPED;
    TELEPHONY_LOGI("stop call tone success");
    return TELEPHONY_SUCCESS;
}

bool AudioControlManager::IsTonePlaying() const
{
    return toneState_ == ToneState::TONEING;
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

int32_t AudioControlManager::PlayDtmfTone(char str)
{
    ToneDescriptor dtmfTone = Tone::ConvertDigitToTone(str);
    return PlayCallTone(dtmfTone);
}

int32_t AudioControlManager::StopDtmfTone()
{
    return StopCallTone();
}

int32_t AudioControlManager::OnPostDialNextChar(char str)
{
    int32_t result = PlayDtmfTone(str);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(DTMF_PLAY_TIME));
    result = StopDtmfTone();
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    return TELEPHONY_SUCCESS;
}

void AudioControlManager::NewCallCreated(sptr<CallBase> &callObjectPtr) {}

void AudioControlManager::CallDestroyed(const DisconnectedDetails &details) {}

bool AudioControlManager::IsSoundPlaying()
{
    return soundState_ == SoundState::SOUNDING;
}
} // namespace Telephony
} // namespace OHOS