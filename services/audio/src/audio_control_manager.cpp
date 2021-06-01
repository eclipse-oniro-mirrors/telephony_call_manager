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

#include "call_manager_log.h"

namespace OHOS {
namespace TelephonyCallManager {
AudioInterruptState AudioControlManager::currentAudioInterruptState_ = AudioInterruptState::UN_INTERRUPT;
bool AudioControlManager::isRinging_ = false;
std::shared_ptr<AudioEventHandler> AudioControlManager::audioEventHandler_ = nullptr;

AudioControlManager::AudioControlManager()
    : isTonePlaying_(false), foregroundCallNum_(""), ring_(nullptr), tone_(nullptr), audioEventLoop_(nullptr)
{}

AudioControlManager::~AudioControlManager()
{
    if (ring_ != nullptr) {
        ring_ = nullptr;
    }
    if (tone_ != nullptr) {
        tone_ = nullptr;
    }
    alertingCalls_.clear();
    activeCalls_.clear();
    ringingCalls_.clear();
    holdingCalls_.clear();
}

void AudioControlManager::Init()
{
    CALLMANAGER_DEBUG_LOG("audio control manager init");
    audioEventLoop_ = AppExecFwk::EventRunner::Create("AudioControlManager");
    if (audioEventLoop_.get() == nullptr) {
        CALLMANAGER_ERR_LOG("audio event loop nullptr");
        return;
    } else {
        audioEventLoop_->Run();
    }
    audioEvent_ = std::make_shared<AudioEvent>();
    if (audioEvent_ == nullptr) {
        CALLMANAGER_ERR_LOG("audio event nullptr");
        return;
    }
    audioEventHandler_ = std::make_shared<AudioEventHandler>(audioEventLoop_, audioEvent_);
    if (audioEventHandler_ == nullptr) {
        CALLMANAGER_ERR_LOG("audio event handler nullptr");
        return;
    }
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return;
    }
    audioProxy->Init();
    audioEvent_->Init();
}

void AudioControlManager::Reset()
{
    if (audioEventLoop_ != nullptr) {
        audioEventLoop_.reset();
    }
}

void AudioControlManager::NewCallCreated(sptr<CallBase> &callObjectPtr)
{
    CALLMANAGER_DEBUG_LOG("audio control manager new call created");
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("call object ptr nullptr");
        return;
    }
    if (totalCalls_.count(callObjectPtr) > 0) {
        CALLMANAGER_ERR_LOG("call already added");
        return;
    }
    totalCalls_.insert(callObjectPtr);
    CallInfo info;
    callObjectPtr->GetBaseCallInfo(info);
    AddCallByState(callObjectPtr->GetPhoneNumber(), info.state);
    UpdateCurrentCall();
}

void AudioControlManager::CallDestroyed(sptr<CallBase> &callObjectPtr)
{
    CALLMANAGER_DEBUG_LOG("audio control manager call destroyed");
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("call object ptr nullptr");
        return;
    }
    if (totalCalls_.count(callObjectPtr) == 0) {
        CALLMANAGER_ERR_LOG("call not included");
        return;
    }
    totalCalls_.erase(callObjectPtr);
    CallInfo info;
    callObjectPtr->GetBaseCallInfo(info);
    DeleteCallByState(callObjectPtr->GetPhoneNumber(), info.state);
    UpdateCurrentCall();
}

void AudioControlManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallStates priorState, TelCallStates nextState)
{
    CALLMANAGER_DEBUG_LOG("audio control manager call state updated");
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("call object ptr nullptr");
        return;
    }
    if (totalCalls_.count(callObjectPtr) == 0) {
        return;
    }
    DeleteCall(callObjectPtr->GetPhoneNumber(), priorState);
    AddCallByState(callObjectPtr->GetPhoneNumber(), nextState);
    UpdateCurrentCall();
    if (NeedToPlayCallEndedTone(priorState, nextState)) {
        PlayCallEndedTone(callObjectPtr->GetCallEndedType());
    }
}

void AudioControlManager::IncomingCallActivated(sptr<CallBase> &callObjectPtr)
{
    CALLMANAGER_DEBUG_LOG("audio control manager incoming call activated");
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("call object ptr nullptr");
        return;
    }
    if (totalCalls_.count(callObjectPtr) == 0) {
        return;
    }
    StopRingtone(); // stop ringtone
    DeleteCallByState(callObjectPtr->GetPhoneNumber(), TelCallStates::CALL_STATUS_INCOMING);
    AddCallByState(callObjectPtr->GetPhoneNumber(), TelCallStates::CALL_STATUS_ACTIVE);
    UpdateMuteState(false); // unmute microphone
}

void AudioControlManager::IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{
    CALLMANAGER_DEBUG_LOG("audio control manager incoming call hung up");
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("call object ptr nullptr");
        return;
    }
    StopRingtone(); // stop ringtone
    DeleteCallByState(callObjectPtr->GetPhoneNumber(), TelCallStates::CALL_STATUS_INCOMING);
}

void AudioControlManager::HandleCallAdded(TelCallStates state)
{
    switch (state) {
        case TelCallStates::CALL_STATUS_ALERTING:
            if (alertingCalls_.size() == existOneCall_) {
                PlayRingback(); // play ringback while only one alerting call exists
            }
            break;
        case TelCallStates::CALL_STATUS_ACTIVE:
            if (activeCalls_.size() == existOneCall_) {
                if (IsCurrentCallIms()) {
                    HandleEvent(CallStateManager::COMING_IMS_CALL);
                } else {
                    HandleEvent(CallStateManager::COMING_CS_CALL);
                }
            }
            break;
        case TelCallStates::CALL_STATUS_INCOMING:
            if (ringingCalls_.size() == existOneCall_ && PlayRingtone()) {
                HandleEvent(CallStateManager::COMING_RINGING_CALL);
            }
            break;
        default:
            break;
    }
}

void AudioControlManager::HandleCallDeleted(TelCallStates state)
{
    switch (state) {
        case TelCallStates::CALL_STATUS_ALERTING:
            if (alertingCalls_.empty()) {
                StopRingback(); // stop ringback tone while no more alerting call
                if (activeCalls_.empty()) {
                    HandleEvent(CallStateManager::NONE_ALERTING_OR_ACTIVE_CALLS);
                }
            }
            break;
        case TelCallStates::CALL_STATUS_ACTIVE:
            if (activeCalls_.empty() && alertingCalls_.empty()) {
                HandleEvent(CallStateManager::NONE_ALERTING_OR_ACTIVE_CALLS);
            }
            break;
        case TelCallStates::CALL_STATUS_INCOMING:
            if (ringingCalls_.empty()) {
                HandleEvent(CallStateManager::NONE_RINGING_CALLS);
            }
            break;
        default:
            break;
    }
}

void AudioControlManager::AddCallByState(const std::string &phoneNum, TelCallStates stateType)
{
    if (phoneNum.empty()) {
        return;
    }
    bool isCallAdded = true;
    switch (stateType) {
        case TelCallStates::CALL_STATUS_ACTIVE:
            CALLMANAGER_DEBUG_LOG("add call , state : active");
            activeCalls_.insert(phoneNum);
            break;
        case TelCallStates::CALL_STATUS_ALERTING:
            CALLMANAGER_DEBUG_LOG("add call , state : alerting");
            alertingCalls_.insert(phoneNum);
            break;
        case TelCallStates::CALL_STATUS_INCOMING:
            CALLMANAGER_DEBUG_LOG("add call , state : incoming");
            ringingCalls_.insert(phoneNum);
            break;
        case TelCallStates::CALL_STATUS_HOLDING:
            CALLMANAGER_DEBUG_LOG("add call , state : holding");
            holdingCalls_.insert(phoneNum);
            break;
        default:
            isCallAdded = false;
            break;
    }
    if (isCallAdded) {
        HandleCallAdded(stateType);
    }
}

void AudioControlManager::DeleteCallByState(const std::string &phoneNum, TelCallStates stateType)
{
    if (phoneNum.empty()) {
        return;
    }
    bool isCallDeleted = true;
    switch (stateType) {
        case TelCallStates::CALL_STATUS_ACTIVE:
            CALLMANAGER_DEBUG_LOG("erase call , state : active");
            activeCalls_.erase(phoneNum);
            break;
        case TelCallStates::CALL_STATUS_ALERTING:
            CALLMANAGER_DEBUG_LOG("erase call , state : alerting");
            alertingCalls_.erase(phoneNum);
            break;
        case TelCallStates::CALL_STATUS_INCOMING:
            CALLMANAGER_DEBUG_LOG("erase call , state : incoming");
            ringingCalls_.erase(phoneNum);
            break;
        case TelCallStates::CALL_STATUS_HOLDING:
            CALLMANAGER_DEBUG_LOG("erase call , state : holding");
            holdingCalls_.erase(phoneNum);
            break;
        default:
            isCallDeleted = false;
            break;
    }
    if (isCallDeleted) {
        HandleCallDeleted(stateType);
    }
}

void AudioControlManager::DeleteCall(const std::string &phoneNum, TelCallStates stateType)
{
    if (phoneNum.empty()) {
        return;
    }
    // call is in one of alertingCalls , activeCalls , holdingCalls or ringingCalls
    if (alertingCalls_.size() > 0) {
        for (auto callNum : alertingCalls_) {
            if (callNum == phoneNum) {
                CALLMANAGER_DEBUG_LOG("erase call , state : alerting");
                alertingCalls_.erase(callNum);
                HandleCallDeleted(stateType);
                return;
            }
        }
    }
    if (activeCalls_.size() > 0) {
        for (auto callNum : activeCalls_) {
            if (callNum == phoneNum) {
                CALLMANAGER_DEBUG_LOG("erase call , state : active");
                activeCalls_.erase(callNum);
                HandleCallDeleted(stateType);
                return;
            }
        }
    }
    if (holdingCalls_.size() > 0) {
        for (auto callNum : holdingCalls_) {
            if (callNum == phoneNum) {
                CALLMANAGER_DEBUG_LOG("erase call , state : holding");
                holdingCalls_.erase(callNum);
                HandleCallDeleted(stateType);
                return;
            }
        }
    }
    if (ringingCalls_.size() > 0) {
        for (auto callNum : ringingCalls_) {
            if (callNum == phoneNum) {
                CALLMANAGER_DEBUG_LOG("erase call , state : ringing");
                ringingCalls_.erase(callNum);
                HandleCallDeleted(stateType);
                return;
            }
        }
    }
}

/**
 * @param route , audio device
 * usually called by the ui interaction , in purpose of switching to another audio device
 */
void AudioControlManager::SetAudioDevice(AudioDevice device)
{
    int32_t event = -1;
    switch (device) {
        case AudioDevice::DEVICE_BLUETOOTH_SCO:
            event = AudioDeviceManager::SWITCH_DEVICE_BLUETOOTH;
            break;
        case AudioDevice::DEVICE_WIRED_HEADSET:
            event = AudioDeviceManager::SWITCH_DEVICE_WIRED_HEADSET;
            break;
        case AudioDevice::DEVICE_SPEAKER:
            event = AudioDeviceManager::SWITCH_DEVICE_SPEAKER;
            break;
        case AudioDevice::DEVICE_EARPIECE:
            event = AudioDeviceManager::SWITCH_DEVICE_EARPIECE;
            break;
        default:
            CALLMANAGER_ERR_LOG("invalid device type");
            break;
    }
    if (event == -1) {
        CALLMANAGER_ERR_LOG("set audio device failed");
        return;
    }
    HandleEvent(event);
}

bool AudioControlManager::PlayRingtone()
{
    if (ring_ != nullptr) {
        ring_ = nullptr;
    }
    ring_ = std::make_unique<Ring>();
    if (ring_ == nullptr) {
        CALLMANAGER_ERR_LOG("create ring object failed");
        return false;
    }
    if (ring_->Start() == TELEPHONY_NO_ERROR) {
        CALLMANAGER_DEBUG_LOG("play ringtone succeed");
        isRinging_ = true;
        return true;
    } else {
        CALLMANAGER_ERR_LOG("play ringtone failed");
    }
    return false;
}

// play default ring
bool AudioControlManager::PlayRingtone(const std::string &phoneNum)
{
    if (IsBlackNumber(phoneNum)) {
        return false;
    }
    if (ring_ != nullptr) {
        ring_ = nullptr;
    }
    ring_ = std::make_unique<Ring>();
    if (ring_ == nullptr) {
        CALLMANAGER_ERR_LOG("create ring object failed");
        return false;
    }
    if (ring_->Start() == TELEPHONY_NO_ERROR) {
        CALLMANAGER_DEBUG_LOG("play ringtone succeed");
        isRinging_ = true;
        return true;
    } else {
        CALLMANAGER_ERR_LOG("play ringtone failed");
    }
    return false;
}

// play specific ring
bool AudioControlManager::PlayRingtone(const std::string &phoneNum, const std::string &ringtonePath)
{
    if (IsBlackNumber(phoneNum)) {
        return false;
    }
    if (ring_ != nullptr) {
        ring_ = nullptr;
    }
    ring_ = std::make_unique<Ring>(ringtonePath);
    if (ring_ == nullptr) {
        CALLMANAGER_ERR_LOG("create ring object failed");
        return false;
    }
    if (ring_->Start() == TELEPHONY_NO_ERROR) {
        CALLMANAGER_DEBUG_LOG("play ringtone succeed");
        isRinging_ = true;
        return true;
    } else {
        CALLMANAGER_ERR_LOG("play ringtone failed");
    }
    return false;
}

bool AudioControlManager::StopRingtone()
{
    CALLMANAGER_DEBUG_LOG("try to stop ringtone");
    if (isRinging_ == false) {
        return true;
    }
    if (ring_ != nullptr && ring_->Stop() == TELEPHONY_NO_ERROR) {
        CALLMANAGER_DEBUG_LOG("stop ringtone succeed");
        isRinging_ = false;
        return true;
    } else {
        CALLMANAGER_ERR_LOG("stop ringtone failed");
    }
    return false;
}

int32_t AudioControlManager::PlayRingback()
{
    CALLMANAGER_DEBUG_LOG("try to play ringback tone");
    return PlayCallTone(ToneDescriptor::TONE_RING_BACK);
}

int32_t AudioControlManager::StopRingback()
{
    CALLMANAGER_DEBUG_LOG("try to stop ringback tone");
    return StopCallTone();
}

int32_t AudioControlManager::PlayWaitingTone()
{
    CALLMANAGER_DEBUG_LOG("try to play waiting tone");
    return PlayCallTone(ToneDescriptor::TONE_WAITING);
}

int32_t AudioControlManager::StopWaitingTone()
{
    CALLMANAGER_DEBUG_LOG("try to stop waiting tone");
    return StopCallTone();
}

int32_t AudioControlManager::PlayHoldingTone()
{
    CALLMANAGER_DEBUG_LOG("try to play holding tone");
    return PlayCallTone(ToneDescriptor::TONE_WAITING);
}

int32_t AudioControlManager::StopHoldingTone()
{
    CALLMANAGER_DEBUG_LOG("try to stop holding tone");
    return StopCallTone();
}

void AudioControlManager::UpdateCurrentCall()
{
    std::string preNumber = foregroundCallNum_;
    // foreground priority : active > alerting > ringing > holding
    if (activeCalls_.size() > 0) {
        foregroundCallNum_ = *activeCalls_.begin(); // first number
    } else if (alertingCalls_.size() > 0) {
        foregroundCallNum_ = *alertingCalls_.begin(); // first number
    } else if (ringingCalls_.size() > 0) {
        foregroundCallNum_ = *ringingCalls_.begin(); // first number
    } else if (holdingCalls_.size() > 0) {
        foregroundCallNum_ = *holdingCalls_.begin(); // first number
    } else {
        foregroundCallNum_ = "";
    }
    if (preNumber != foregroundCallNum_) {
        CALLMANAGER_DEBUG_LOG("pre foreground call number != current foreground call number");
    }
}

// mute or unmute microphone
void AudioControlManager::UpdateMuteState(bool on)
{
    if (audioEventHandler_ == nullptr) {
        return;
    }
    if (IsEmergencyCallExists()) {
        on = false;
    }
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr, update mute state failed");
        return;
    }
    audioProxy->SetMicrophoneMute(on);
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

void AudioControlManager::SetVolumeAudible()
{
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return;
    }
    if (audioProxy->GetVolume(AudioVolumeType::STREAM_VOICE_CALL) == 0) {
        int32_t volume = audioProxy->GetMaxVolume(AudioVolumeType::STREAM_VOICE_CALL);
        audioProxy->SetVolume(AudioVolumeType::STREAM_VOICE_CALL, volume); // set max volume
    }
}

bool AudioControlManager::IsBlackNumber(const std::string &phoneNum)
{
    // check whether the phone number is black or not , black number should not ring
    return false;
}

bool AudioControlManager::NeedToPlayCallEndedTone(TelCallStates priorState, TelCallStates nextState) const
{
    if (nextState != TelCallStates::CALL_STATUS_DISCONNECTED) {
        return false;
    }
    return priorState == TelCallStates::CALL_STATUS_ACTIVE || priorState == TelCallStates::CALL_STATUS_DIALING ||
        priorState == TelCallStates::CALL_STATUS_HOLDING;
}

void AudioControlManager::PlayCallEndedTone(CallEndedType type)
{
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

sptr<CallBase> AudioControlManager::GetCallBase(const std::string &phoneNum) const
{
    sptr<CallBase> retPtr;
    if (phoneNum.empty()) {
        return retPtr;
    }
    for (auto &call : totalCalls_) {
        if (call->GetPhoneNumber() == phoneNum) {
            retPtr = call;
            break;
        }
    }
    return retPtr;
}

AudioInterruptState AudioControlManager::GetAudioInterruptState()
{
    return currentAudioInterruptState_;
}

bool AudioControlManager::IsCurrentCallIms() const
{
    if (foregroundCallNum_.empty()) {
        return false;
    }
    CallInfo info;
    auto foregroundCall = GetCallBase(foregroundCallNum_);
    if (foregroundCall != nullptr) {
        foregroundCall->GetBaseCallInfo(info);
        return info.videoState == 1;
    }
    return false;
}

bool AudioControlManager::IsActiveCallExists() const
{
    return activeCalls_.size() > 0;
}

bool AudioControlManager::IsHoldingCallExists() const
{
    return holdingCalls_.size() > 0;
}

bool AudioControlManager::IsRingingCallExists() const
{
    return ringingCalls_.size() > 0;
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

bool AudioControlManager::IsCurrentRinging()
{
    return isRinging_;
}

bool AudioControlManager::IsAudioActive()
{
    return currentAudioInterruptState_ != AudioInterruptState::UN_INTERRUPT;
}

int32_t AudioControlManager::PlayCallTone(ToneDescriptor type)
{
    if (tone_ != nullptr) {
        tone_ = nullptr;
    }
    tone_ = std::make_unique<Tone>(type);
    if (tone_ == nullptr) {
        CALLMANAGER_ERR_LOG("create tone object failed");
        return TELEPHONY_LOCAL_PTR_NULL;
    }
    if (tone_->Start() == TELEPHONY_NO_ERROR) {
        isTonePlaying_ = true;
        return TELEPHONY_NO_ERROR;
    }
    return TELEPHONY_FAIL;
}

int32_t AudioControlManager::StopCallTone()
{
    if (tone_ != nullptr && tone_->Stop() == TELEPHONY_NO_ERROR) {
        isTonePlaying_ = false;
        return TELEPHONY_NO_ERROR;
    }
    return TELEPHONY_FAIL;
}

sptr<CallBase> AudioControlManager::GetCurrentCall() const
{
    auto call = GetCallBase(foregroundCallNum_);
    if (call != nullptr && call->GetState() != CallStateType::CALL_STATE_HOLD_TYPE) {
        return call;
    }
    return nullptr;
}

void AudioControlManager::HandleEvent(int32_t event)
{
    if (audioEventHandler_ == nullptr) {
        CALLMANAGER_ERR_LOG("audio event handler nullptr");
        return;
    }
    audioEventHandler_->SendEmptyEvent(event);
}

void AudioControlManager::SetAudioInterruptState(AudioInterruptState state)
{
    currentAudioInterruptState_ = state;
    if (audioEventHandler_ == nullptr) {
        CALLMANAGER_ERR_LOG("audio event handler nullptr");
        return;
    }
    /**
     * send audio interrupt event to audio device manager , maybe need to reinit the audio device
     */
    switch (currentAudioInterruptState_) {
        case AudioInterruptState::UN_INTERRUPT:
            CALLMANAGER_DEBUG_LOG("current audio interrupt state : un_interrupt");
            HandleEvent(AudioDeviceManager::AUDIO_UN_INTERRUPT);
            break;
        case AudioInterruptState::IN_RINGING:
            CALLMANAGER_DEBUG_LOG("current audio interrupt state : in_ringing");
            HandleEvent(AudioDeviceManager::AUDIO_RINGING);
            break;
        case AudioInterruptState::IN_INTERRUPT:
            CALLMANAGER_DEBUG_LOG("current audio interrupt state : in_interrupt");
            HandleEvent(AudioDeviceManager::AUDIO_INTERRUPTED);
            break;
        default:
            break;
    }
}

/**
 * update call state according to current conditions
 * priority : active > holding > ringing
 */
void AudioControlManager::UpdateCallState()
{
    if (IsActiveCallExists()) {
        if (IsCurrentCallIms()) {
            HandleEvent(CallStateManager::SWITCH_IMS_CALL);
        } else {
            HandleEvent(CallStateManager::SWITCH_CS_CALL);
        }
        return;
    }
    if (IsHoldingCallExists()) {
        HandleEvent(CallStateManager::SWITCH_HOLDING);
        return;
    }
    if (IsRingingCallExists()) {
        HandleEvent(CallStateManager::SWITCH_RINGING);
        return;
    }
    HandleEvent(CallStateManager::ABANDON_AUDIO); // finally switch to inactive state
}

/**
 * while audio state changed , maybe need to reinit the audio device
 * in order to get the initialization status of audio device , need to consider varieties of  audio conditions
 */
AudioDevice AudioControlManager::GetInitAudioDevice() const
{
    if (currentAudioInterruptState_ == AudioInterruptState::UN_INTERRUPT) {
        // disable device
        return AudioDevice::DEVICE_DISABLE;
    } else {
        // interrupted or ringing , priority : bt sco , wired headset , speaker , earpiece
        if (AudioDeviceManager::IsBtScoAvailable()) {
            return AudioDevice::DEVICE_BLUETOOTH_SCO;
        }
        if (AudioDeviceManager::IsWiredHeadsetAvailable()) {
            return AudioDevice::DEVICE_WIRED_HEADSET;
        }
        if (currentAudioInterruptState_ == AudioInterruptState::IN_RINGING) {
            return AudioDevice::DEVICE_SPEAKER;
        }
        if (GetCurrentCall() != nullptr && GetCurrentCall()->IsSpeakerphoneOn()) {
            return AudioDevice::DEVICE_SPEAKER;
        }
        return AudioDevice::DEVICE_EARPIECE;
    }
}
} // namespace TelephonyCallManager
} // namespace OHOS