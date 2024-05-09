/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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
#include "call_dialog.h"
#include "call_state_processor.h"
#include "common_type.h"
#include "distributed_call_manager.h"
#include "telephony_log_wrapper.h"
#include "audio_system_manager.h"
#include "audio_routing_manager.h"
#include "audio_device_info.h"
#include "audio_info.h"
#include "voip_call_connection.h"

namespace OHOS {
namespace Telephony {
using namespace AudioStandard;
constexpr int32_t DTMF_PLAY_TIME = 30;
constexpr int32_t VOICE_TYPE = 0;
constexpr int32_t CRS_TYPE = 2;

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

void AudioControlManager::UpdateForegroundLiveCall()
{
    int32_t callId = DelayedSingleton<CallStateProcessor>::GetInstance()->GetAudioForegroundLiveCall();
    if (callId == INVALID_CALLID) {
        frontCall_ = nullptr;
        DelayedSingleton<AudioProxy>::GetInstance()->SetMicrophoneMute(false);
        TELEPHONY_LOGE("callId is invalid");
        return;
    }

    sptr<CallBase> liveCall = CallObjectManager::GetOneCallObject(callId);
    if (liveCall == nullptr) {
        TELEPHONY_LOGE("liveCall is nullptr");
        return;
    }
    if (liveCall->GetTelCallState() == TelCallState::CALL_STATUS_ACTIVE) {
        if (frontCall_ == nullptr) {
            frontCall_ = liveCall;
        } else {
            int32_t frontCallId = frontCall_->GetCallID();
            int32_t liveCallId = liveCall->GetCallID();
            if (frontCallId != liveCallId) {
                frontCall_ = liveCall;
            }
        }
        bool frontCallMute = frontCall_->IsMuted();
        bool currentMute = DelayedSingleton<AudioProxy>::GetInstance()->IsMicrophoneMute();
        if (frontCallMute != currentMute) {
            SetMute(frontCallMute);
        }
    }
}

void AudioControlManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object nullptr");
        return;
    }
    if (callObjectPtr->GetCallType() == CallType::TYPE_VOIP) {
        TELEPHONY_LOGI("voip call not need control audio");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
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
    UpdateForegroundLiveCall();
}

void AudioControlManager::VideoStateUpdated(
    sptr<CallBase> &callObjectPtr, VideoStateType priorVideoState, VideoStateType nextVideoState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object nullptr");
        return;
    }
    if (callObjectPtr->GetCallType() != CallType::TYPE_IMS) {
        TELEPHONY_LOGE("other call not need control audio");
        return;
    }
    AudioDevice device = {
        .deviceType = AudioDeviceType::DEVICE_SPEAKER,
        .address = { 0 },
    };
    AudioDeviceType initDeviceType = GetInitAudioDeviceType();
    if (callObjectPtr->GetCrsType() == CRS_TYPE) {
        AudioStandard::AudioRingerMode ringMode = DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode();
        if (ringMode != AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL) {
            if (initDeviceType == AudioDeviceType::DEVICE_WIRED_HEADSET ||
                initDeviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
                device.deviceType = initDeviceType;
            }
        }
        TELEPHONY_LOGI("crs ring tone should be speaker");
        SetAudioDevice(device);
        return;
    }
    CheckTypeAndSetAudioDevice(callObjectPtr, priorVideoState, nextVideoState, initDeviceType, device);
}

void AudioControlManager::CheckTypeAndSetAudioDevice(sptr<CallBase> &callObjectPtr, VideoStateType priorVideoState,
    VideoStateType nextVideoState, AudioDeviceType &initDeviceType, AudioDevice &device)
{
    TelCallState telCallState = callObjectPtr->GetTelCallState();
    if (!IsVideoCall(priorVideoState) && IsVideoCall(nextVideoState) &&
        (telCallState != TelCallState::CALL_STATUS_INCOMING && telCallState != TelCallState::CALL_STATUS_WAITING)) {
        if (callObjectPtr->GetOriginalCallType() == VOICE_TYPE &&
            (telCallState == TelCallState::CALL_STATUS_DIALING || telCallState == TelCallState::CALL_STATUS_ALERTING)) {
            TELEPHONY_LOGI("before modify set device to EARPIECE, now not set");
            return;
        }
        if (initDeviceType == AudioDeviceType::DEVICE_WIRED_HEADSET ||
            initDeviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO ||
            initDeviceType == AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE) {
            device.deviceType = initDeviceType;
        }
        TELEPHONY_LOGI("set device type, type: %{public}d", static_cast<int32_t>(device.deviceType));
        SetAudioDevice(device);
    } else if (!isSetAudioDeviceByUser_ && IsVideoCall(priorVideoState) && !IsVideoCall(nextVideoState)) {
        device.deviceType = AudioDeviceType::DEVICE_EARPIECE;
        if (initDeviceType == AudioDeviceType::DEVICE_WIRED_HEADSET ||
            initDeviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO ||
            initDeviceType == AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE) {
            device.deviceType = initDeviceType;
        }
        TELEPHONY_LOGI("set device type, type: %{public}d", static_cast<int32_t>(device.deviceType));
        SetAudioDevice(device);
    }
}

void AudioControlManager::UpdateDeviceTypeForVideoOrSatelliteCall()
{
    sptr<CallBase> foregroundCall = CallObjectManager::GetForegroundLiveCall();
    if (foregroundCall == nullptr) {
        TELEPHONY_LOGE("call object nullptr");
        return;
    }
    if (foregroundCall->GetCallType() != CallType::TYPE_IMS ||
        foregroundCall->GetCallType() != CallType::TYPE_SATELLITE) {
        TELEPHONY_LOGE("other call not need control audio");
        return;
    }
    AudioDevice device = {
        .deviceType = AudioDeviceType::DEVICE_SPEAKER,
        .address = { 0 },
    };
    AudioDeviceType initDeviceType = GetInitAudioDeviceType();
    if (IsVideoCall(foregroundCall->GetVideoStateType()) ||
        foregroundCall->GetCallType() == CallType::TYPE_SATELLITE) {
        if (initDeviceType == AudioDeviceType::DEVICE_WIRED_HEADSET ||
            initDeviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
            device.deviceType = initDeviceType;
        }
        TELEPHONY_LOGI("set device type, type: %{public}d", static_cast<int32_t>(device.deviceType));
        SetAudioDevice(device);
    }
}

void AudioControlManager::UpdateDeviceTypeForCrs()
{
    sptr<CallBase> incomingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (incomingCall == nullptr) {
        return;
    }
    if (incomingCall->GetCrsType() == CRS_TYPE) {
        AudioDevice device = {
            .deviceType = AudioDeviceType::DEVICE_SPEAKER,
            .address = { 0 },
        };
        AudioStandard::AudioRingerMode ringMode = DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode();
        if (ringMode != AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL) {
            AudioDeviceType initDeviceType = GetInitAudioDeviceType();
            if (initDeviceType == AudioDeviceType::DEVICE_WIRED_HEADSET ||
                initDeviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
                device.deviceType = initDeviceType;
            }
        }
        TELEPHONY_LOGI("crs ring tone should be speaker");
        SetAudioDevice(device);
    }
}

void AudioControlManager::IncomingCallActivated(sptr<CallBase> &callObjectPtr) {}

void AudioControlManager::IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object ptr nullptr");
        return;
    }
    StopCallTone();
}

void AudioControlManager::HandleCallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (nextState == TelCallState::CALL_STATUS_ANSWERED) {
        TELEPHONY_LOGI("user answered, mute ringer instead of release renderer");
        if (priorState == TelCallState::CALL_STATUS_INCOMING) {
            DelayedSingleton<CallStateProcessor>::GetInstance()->DeleteCall(callObjectPtr->GetCallID(), priorState);
        }
        MuteRinger();
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
        case TelCallState::CALL_STATUS_WAITING:
        case TelCallState::CALL_STATUS_INCOMING:
            event = AudioEvent::NEW_INCOMING_CALL;
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_RINGING;
            break;
        case TelCallState::CALL_STATUS_DISCONNECTING:
        case TelCallState::CALL_STATUS_DISCONNECTED:
            if (isCrsVibrating_) {
                DelayedSingleton<AudioProxy>::GetInstance()->StopVibrator();
                isCrsVibrating_ = false;
            }
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
            ProcessAudioWhenCallActive(callObjectPtr);
            event = AudioEvent::NO_MORE_INCOMING_CALL;
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            if (stateNumber == EMPTY_VALUE) {
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

void AudioControlManager::ProcessAudioWhenCallActive(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_ACTIVE) {
        if (isCrsVibrating_) {
            DelayedSingleton<AudioProxy>::GetInstance()->StopVibrator();
            isCrsVibrating_ = false;
        }
        if (CallObjectManager::GetCurrentCallNum() < MIN_MULITY_CALL_COUNT) {
            StopSoundtone();
            PlaySoundtone();
        }
        UpdateDeviceTypeForVideoOrSatelliteCall();
    }
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
        case CallType::TYPE_SATELLITE:
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
    return SetAudioDevice(device, false);
}

/**
 * @param device , audio device
 * @param isByUser , call from callui or not
 * usually called by the ui interaction , in purpose of switching to another audio device
 */
int32_t AudioControlManager::SetAudioDevice(const AudioDevice &device, bool isByUser)
{
    TELEPHONY_LOGI("set audio device, type: %{public}d", static_cast<int32_t>(device.deviceType));
    AudioDeviceType audioDeviceType = AudioDeviceType::DEVICE_UNKNOWN;
    if (CallObjectManager::HasSatelliteCallExist() && device.deviceType == AudioDeviceType::DEVICE_EARPIECE) {
        DelayedSingleton<CallDialog>::GetInstance()->DialogConnectExtension("SATELLITE_CALL_NOT_SUPPORT_EARPIECE");
        return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
    }
    isSetAudioDeviceByUser_ = isByUser;
    switch (device.deviceType) {
        case AudioDeviceType::DEVICE_SPEAKER:
        case AudioDeviceType::DEVICE_EARPIECE:
        case AudioDeviceType::DEVICE_WIRED_HEADSET:
            audioDeviceType = device.deviceType;
            break;
        case AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE:
        case AudioDeviceType::DEVICE_DISTRIBUTED_PHONE:
        case AudioDeviceType::DEVICE_DISTRIBUTED_PAD:
            return HandleDistributeAudioDevice(device);
        case AudioDeviceType::DEVICE_BLUETOOTH_SCO: {
            AudioSystemManager* audioSystemManager = AudioSystemManager::GetInstance();
            int32_t ret = audioSystemManager->SetCallDeviceActive(ActiveDeviceType::BLUETOOTH_SCO,
                true, device.address);
            if (ret != 0) {
                TELEPHONY_LOGE("SetCallDeviceActive failed");
                return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
            }
            audioDeviceType = device.deviceType;
            break;
        }
        default:
            break;
    }
    if (audioDeviceType != AudioDeviceType::DEVICE_UNKNOWN) {
        if (DelayedSingleton<DistributedCallManager>::GetInstance()->IsDCallDeviceSwitchedOn()) {
            DelayedSingleton<DistributedCallManager>::GetInstance()->SwitchOffDCallDeviceSync();
        }
        if (DelayedSingleton<AudioDeviceManager>::GetInstance()->SwitchDevice(audioDeviceType)) {
            return TELEPHONY_SUCCESS;
        }
    }
    return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
}

int32_t AudioControlManager::HandleDistributeAudioDevice(const AudioDevice &device)
{
    if (!DelayedSingleton<DistributedCallManager>::GetInstance()->IsDCallDeviceSwitchedOn()) {
        TELEPHONY_LOGI("set audio device, address: %{public}s", device.address);
        if (DelayedSingleton<DistributedCallManager>::GetInstance()->SwitchOnDCallDeviceSync(device)) {
            DelayedSingleton<AudioDeviceManager>::GetInstance()->SetCurrentAudioDevice(device.deviceType);
            return TELEPHONY_SUCCESS;
        }
        return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
    }
    return TELEPHONY_SUCCESS;
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
    sptr<CallBase> incomingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (incomingCall == nullptr) {
        TELEPHONY_LOGE("incomingCall is nullptr");
        return false;
    }
    CallAttributeInfo info;
    incomingCall->GetCallAttributeBaseInfo(info);
    AudioStandard::AudioRingerMode ringMode = DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode();
    if (incomingCall->GetCrsType() == CRS_TYPE) {
        if (!isCrsVibrating_ && (ringMode != AudioStandard::AudioRingerMode::RINGER_MODE_SILENT)) {
            isCrsVibrating_ = (DelayedSingleton<AudioProxy>::GetInstance()->StartVibrator() == TELEPHONY_SUCCESS);
        }
        if ((ringMode == AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL) || IsBtOrWireHeadPlugin()) {
            if (PlaySoundtone()) {
                TELEPHONY_LOGI("play soundtone success");
                return true;
            }
            return false;
        }
    }
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
        if (AudioDeviceManager::IsDistributedCallConnected()) {
            return AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE;
        }
        if (AudioDeviceManager::IsBtScoConnected()) {
            return AudioDeviceType::DEVICE_BLUETOOTH_SCO;
        }
        if (AudioDeviceManager::IsWiredHeadsetConnected()) {
            return AudioDeviceType::DEVICE_WIRED_HEADSET;
        }
        sptr<CallBase> liveCall = CallObjectManager::GetForegroundLiveCall();
        if (liveCall != nullptr && (liveCall->GetVideoStateType() == VideoStateType::TYPE_VIDEO ||
            liveCall->GetCallType() == CallType::TYPE_SATELLITE)) {
            TELEPHONY_LOGI("current video or satellite call speaker is active");
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
    if (!DelayedSingleton<AudioProxy>::GetInstance()->SetMicrophoneMute(isMute)) {
        TELEPHONY_LOGE("set mute failed");
        return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
    }
    DelayedSingleton<AudioDeviceManager>::GetInstance()->ReportAudioDeviceInfo();
    if (frontCall_ == nullptr) {
        TELEPHONY_LOGE("frontCall_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    frontCall_->SetMicPhoneState(isMute);
    TELEPHONY_LOGI("SetMute success callId:%{public}d, mute:%{public}d", frontCall_->GetCallID(), isMute);
    return TELEPHONY_SUCCESS;
}

int32_t AudioControlManager::MuteRinger()
{
    sptr<CallBase> incomingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (incomingCall != nullptr) {
        if (incomingCall->GetCrsType() == CRS_TYPE) {
            TELEPHONY_LOGI("Mute network ring tone.");
            MuteNetWorkRingTone();
        }
    }
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

bool AudioControlManager::IsEmergencyCallExists()
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto call : totalCalls_) {
        if (call->GetEmergencyState()) {
            return true;
        }
    }
    return false;
}

bool AudioControlManager::IsSatelliteExists()
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto call : totalCalls_) {
        if (call->GetCallType() == CallType::TYPE_SATELLITE) {
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
    std::lock_guard<std::recursive_mutex> lock(toneStateLock_);
    toneState_ = state;
}

void AudioControlManager::SetLocalRingbackNeeded(bool isNeeded)
{
    if (isLocalRingbackNeeded_ && !isNeeded) {
        StopRingback();
    }
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
    if (incomingCallNum == EMPTY_VALUE || alertingCallNum > EMPTY_VALUE || ringState_ == RingState::RINGING
        || soundState_ == SoundState::SOUNDING) {
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
    std::lock_guard<std::recursive_mutex> lock(toneStateLock_);
    if (toneState_ == ToneState::TONEING) {
        TELEPHONY_LOGE("should not play callTone");
        return CALL_ERR_AUDIO_TONE_PLAY_FAILED;
    }
    toneState_ = ToneState::TONEING;
    tone_ = std::make_unique<Tone>(type);
    if (tone_ == nullptr) {
        TELEPHONY_LOGE("create tone failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
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
    std::lock_guard<std::recursive_mutex> lock(toneStateLock_);
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
    toneState_ = ToneState::STOPPED;
    TELEPHONY_LOGI("stop call tone success");
    return TELEPHONY_SUCCESS;
}

bool AudioControlManager::IsTonePlaying()
{
    std::lock_guard<std::recursive_mutex> lock(toneStateLock_);
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
    std::unique_ptr<Tone> tone = std::make_unique<Tone>(dtmfTone);
    if (tone == nullptr) {
        TELEPHONY_LOGE("create dtmf tone failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (tone->Play() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play dtmftone failed");
        return CALL_ERR_AUDIO_TONE_PLAY_FAILED;
    }
    TELEPHONY_LOGI("play dtmftone success");
    std::this_thread::sleep_for(std::chrono::milliseconds(DTMF_PLAY_TIME));
    if (tone->Stop() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop dtmftone failed");
        return CALL_ERR_AUDIO_TONE_STOP_FAILED;
    }
    tone->ReleaseRenderer();
    TELEPHONY_LOGI("stop dtmf tone success");
    return TELEPHONY_SUCCESS;
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
    return TELEPHONY_SUCCESS;
}

void AudioControlManager::NewCallCreated(sptr<CallBase> &callObjectPtr) {}

void AudioControlManager::CallDestroyed(const DisconnectedDetails &details) {}

bool AudioControlManager::IsSoundPlaying()
{
    return soundState_ == SoundState::SOUNDING;
}

void AudioControlManager::MuteNetWorkRingTone()
{
    bool result =
        DelayedSingleton<AudioProxy>::GetInstance()->SetAudioScene(AudioStandard::AudioScene::AUDIO_SCENE_DEFAULT);
    TELEPHONY_LOGI("Set volume mute, result: %{public}d", result);
    if (isCrsVibrating_) {
        DelayedSingleton<AudioProxy>::GetInstance()->StopVibrator();
        isCrsVibrating_ = false;
    }
}

bool AudioControlManager::IsVideoCall(VideoStateType videoState)
{
    return videoState == VideoStateType::TYPE_SEND_ONLY || videoState == VideoStateType::TYPE_RECEIVE_ONLY ||
           videoState == VideoStateType::TYPE_VIDEO;
}

bool AudioControlManager::IsBtOrWireHeadPlugin()
{
    return AudioDeviceManager::IsBtScoConnected() || AudioDeviceManager::IsWiredHeadsetConnected();
}
} // namespace Telephony
} // namespace OHOS