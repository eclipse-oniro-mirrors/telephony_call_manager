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

#include "audio_proxy.h"

#include "telephony_log_wrapper.h"
#include "bluetooth_call_manager.h"
#include "audio_control_manager.h"

namespace OHOS {
namespace Telephony {
int32_t AudioProxy::currentSessionId_ = DEFAULT_SESSION_ID;
bool AudioProxy::isRingtoneActivated_ = false;
bool AudioProxy::isVoiceCallActivated_ = false;
std::shared_ptr<AudioStandard::AudioManagerCallback> AudioProxy::audioManagerCallback_ = nullptr;

AudioProxy::AudioProxy()
    : context_(nullptr), audioSoundManager_(std::make_unique<AudioStandard::RingtoneSoundManager>()),
      deviceCallback_(std::make_shared<AudioDeviceChangeCallback>())
{
    ringtoneAudioInterrupt_.streamUsage = AudioStandard::StreamUsage::STREAM_USAGE_NOTIFICATION_RINGTONE;
    ringtoneAudioInterrupt_.contentType = AudioStandard::ContentType::CONTENT_TYPE_MUSIC;
    ringtoneAudioInterrupt_.streamType = AudioStandard::AudioStreamType::STREAM_RING;
    ringtoneAudioInterrupt_.sessionID = RINGTONE_SESSION_ID;
    voiceCallAudioInterrupt_.streamUsage = AudioStandard::StreamUsage::STREAM_USAGE_VOICE_COMMUNICATION;
    voiceCallAudioInterrupt_.contentType = AudioStandard::ContentType::CONTENT_TYPE_SPEECH;
    voiceCallAudioInterrupt_.streamType = AudioStandard::AudioStreamType::STREAM_VOICE_CALL;
    voiceCallAudioInterrupt_.sessionID = VOICE_CALL_SESSION_ID;
}

AudioProxy::~AudioProxy()
{
    UnsetAudioManagerCallback();
}

int32_t AudioProxy::ActivateAudioInterrupt(const AudioStandard::AudioInterrupt &audioInterrupt)
{
    return AudioStandard::AudioSystemManager::GetInstance()->ActivateAudioInterrupt(audioInterrupt);
}

int32_t AudioProxy::DeactivateAudioInterrupt(const AudioStandard::AudioInterrupt &audioInterrupt)
{
    return AudioStandard::AudioSystemManager::GetInstance()->DeactivateAudioInterrupt(audioInterrupt);
}

bool AudioProxy::SetAudioScene(AudioStandard::AudioScene audioScene)
{
    return AudioStandard::AudioSystemManager::GetInstance()->SetAudioScene(audioScene) == TELEPHONY_SUCCESS;
}

int32_t AudioProxy::SetAudioDeviceChangeCallback()
{
    if (deviceCallback_ == nullptr) {
        TELEPHONY_LOGE("device callback nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return AudioStandard::AudioSystemManager::GetInstance()->SetDeviceChangeCallback(deviceCallback_);
}

bool AudioProxy::SetBluetoothDevActive()
{
    if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(
        AudioStandard::ActiveDeviceType::BLUETOOTH_SCO)) {
        TELEPHONY_LOGI("bluetooth device is already active");
        return true;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
        AudioStandard::ActiveDeviceType::BLUETOOTH_SCO, true) &&
        AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
            AudioStandard::ActiveDeviceType::SPEAKER, false);
#endif
    return true;
}

bool AudioProxy::SetWiredHeadsetDevActive()
{
    return false;
}

bool AudioProxy::SetSpeakerDevActive()
{
    if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(AudioStandard::ActiveDeviceType::SPEAKER)) {
        TELEPHONY_LOGI("speaker device is already active");
        return true;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
        AudioStandard::ActiveDeviceType::BLUETOOTH_SCO, false) &&
        AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
            AudioStandard::ActiveDeviceType::SPEAKER, true);
#endif
    return true;
}

bool AudioProxy::SetEarpieceDevActive()
{
    return false;
}

int32_t AudioProxy::GetVolume(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->GetVolume(audioVolumeType);
}

int32_t AudioProxy::SetVolume(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType, int32_t volume)
{
    return AudioStandard::AudioSystemManager::GetInstance()->SetVolume(audioVolumeType, volume);
}

int32_t AudioProxy::SetMaxVolume(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType)
{
    int32_t maxVolume = GetMaxVolume(audioVolumeType);
    return AudioStandard::AudioSystemManager::GetInstance()->SetVolume(audioVolumeType, maxVolume);
}

void AudioProxy::SetVolumeAudible()
{
    int32_t volume = GetMaxVolume(AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_VOICE_CALL);
    SetVolume(AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_VOICE_CALL,
        (int32_t)(volume / VOLUME_AUDIBLE_DIVISOR));
}

bool AudioProxy::IsStreamActive(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->IsStreamActive(audioVolumeType);
}

bool AudioProxy::IsStreamMute(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->IsStreamMute(audioVolumeType);
}

int32_t AudioProxy::GetMaxVolume(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->GetMaxVolume(audioVolumeType);
}

int32_t AudioProxy::GetMinVolume(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->GetMinVolume(audioVolumeType);
}

bool AudioProxy::IsMicrophoneMute()
{
    return AudioStandard::AudioSystemManager::GetInstance()->IsMicrophoneMute();
}

bool AudioProxy::SetMicrophoneMute(bool mute)
{
    if (!DelayedSingleton<AudioControlManager>::GetInstance()->IsAudioActivated()) {
        TELEPHONY_LOGE("audio is not activated");
        return false;
    }
    if (mute == IsMicrophoneMute()) {
        return true;
    }
    int32_t muteResult = AudioStandard::AudioSystemManager::GetInstance()->SetMicrophoneMute(mute);
    TELEPHONY_LOGI("set microphone mute result : %{public}d", muteResult);
    return muteResult == TELEPHONY_SUCCESS;
}

AudioStandard::AudioRingerMode AudioProxy::GetRingerMode() const
{
    return AudioStandard::AudioSystemManager::GetInstance()->GetRingerMode();
}

bool AudioProxy::IsVibrateMode() const
{
    return AudioStandard::AudioRingerMode::RINGER_MODE_VIBRATE == GetRingerMode();
}

int32_t AudioProxy::ActivateVoiceCallStream()
{
    if (isVoiceCallActivated_) {
        TELEPHONY_LOGI("voice call stream is already activate");
        return TELEPHONY_SUCCESS;
    }
    int32_t ret = SetAudioManagerCallback(AudioStandard::AudioStreamType::STREAM_VOICE_CALL);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("set audio manager callback failed");
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    } else {
        TELEPHONY_LOGI("set voice call stream callback success");
    }
    return DelayedSingleton<AudioProxy>::GetInstance()->ActivateAudioInterrupt(voiceCallAudioInterrupt_);
}

int32_t AudioProxy::DeactivateVoiceCallStream()
{
    if (!isVoiceCallActivated_) {
        TELEPHONY_LOGI("voice call stream is already deactivate");
        return TELEPHONY_SUCCESS;
    }
    return AudioStandard::AudioSystemManager::GetInstance()->DeactivateAudioInterrupt(voiceCallAudioInterrupt_);
}

int32_t AudioProxy::ActivateRingtoneStream()
{
    if (isRingtoneActivated_) {
        TELEPHONY_LOGI("ringtone stream is already activate");
        return TELEPHONY_SUCCESS;
    }
    int32_t ret = SetAudioManagerCallback(AudioStandard::AudioStreamType::STREAM_RING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("set audio manager callback failed");
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    } else {
        TELEPHONY_LOGI("set ringtone stream callback success");
    }
    return DelayedSingleton<AudioProxy>::GetInstance()->ActivateAudioInterrupt(ringtoneAudioInterrupt_);
}

int32_t AudioProxy::DeactivateRingtoneStream()
{
    if (!isRingtoneActivated_) {
        TELEPHONY_LOGI("ringtone stream is already deactivate");
        return TELEPHONY_SUCCESS;
    }
    return AudioStandard::AudioSystemManager::GetInstance()->DeactivateAudioInterrupt(ringtoneAudioInterrupt_);
}

int32_t AudioProxy::DeactivateAudioInterrupt()
{
    int32_t ret = TELEPHONY_SUCCESS;
    switch (currentSessionId_) {
        case VOICE_CALL_SESSION_ID:
            ret = DeactivateVoiceCallStream();
            break;
        case RINGTONE_SESSION_ID:
            ret = DeactivateRingtoneStream();
            break;
        default:
            break;
    }
    return ret;
}

int32_t AudioProxy::SetAudioManagerCallback(AudioStandard::AudioStreamType streamType)
{
    int32_t ret = TELEPHONY_SUCCESS;
    switch (streamType) {
        case AudioStandard::AudioStreamType::STREAM_VOICE_CALL:
            if (currentSessionId_ == VOICE_CALL_SESSION_ID) {
                return TELEPHONY_SUCCESS;
            }
            ret = UnsetAudioManagerCallback();
            if (ret == TELEPHONY_SUCCESS) {
                audioManagerCallback_ = std::make_shared<VoiceCallCallback>();
            } else {
                TELEPHONY_LOGE("unset audio manager callback failed");
            }
            break;
        case AudioStandard::AudioStreamType::STREAM_RING:
            if (currentSessionId_ == RINGTONE_SESSION_ID) {
                return TELEPHONY_SUCCESS;
            }
            ret = UnsetAudioManagerCallback();
            if (ret == TELEPHONY_SUCCESS) {
                audioManagerCallback_ = std::make_shared<RingtoneCallback>();
            } else {
                TELEPHONY_LOGE("unset audio manager callback failed");
            }
            break;
        default:
            break;
    }
    if (audioManagerCallback_ == nullptr) {
        TELEPHONY_LOGE("audio manager callback nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return AudioStandard::AudioSystemManager::GetInstance()->SetAudioManagerCallback(
        static_cast<AudioStandard::AudioSystemManager::AudioVolumeType>(streamType), audioManagerCallback_);
}

int32_t AudioProxy::UnsetAudioManagerCallback()
{
    int32_t ret = TELEPHONY_SUCCESS;
    switch (currentSessionId_) {
        case VOICE_CALL_SESSION_ID:
            ret = UnsetAudioManagerCallback(AudioStandard::AudioStreamType::STREAM_VOICE_CALL);
            break;
        case RINGTONE_SESSION_ID:
            ret = UnsetAudioManagerCallback(AudioStandard::AudioStreamType::STREAM_RING);
            break;
        default:
            break;
    }
    return ret;
}

int32_t AudioProxy::UnsetAudioManagerCallback(AudioStandard::AudioStreamType streamType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->UnsetAudioManagerCallback(
        static_cast<AudioStandard::AudioSystemManager::AudioVolumeType>(streamType));
}

void VoiceCallCallback::OnInterrupt(const AudioStandard::InterruptAction &interruptAction)
{
    AudioStandard::InterruptActionType type = interruptAction.actionType;
    AudioProxy::isRingtoneActivated_ = false;
    AudioInterruptState state = AudioInterruptState::INTERRUPT_STATE_UNKNOWN;
    switch (type) {
        case AudioStandard::InterruptActionType::TYPE_ACTIVATED:
            AudioProxy::isVoiceCallActivated_ = true;
            AudioProxy::currentSessionId_ = VOICE_CALL_SESSION_ID;
            state = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
            break;
        case AudioStandard::InterruptActionType::TYPE_DEACTIVATED:
            AudioProxy::isVoiceCallActivated_ = false;
            AudioProxy::currentSessionId_ = DEFAULT_SESSION_ID;
            state = AudioInterruptState::INTERRUPT_STATE_DEACTIVATED;
            break;
        default:
            AudioProxy::isVoiceCallActivated_ = false;
            break;
    }
    DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioInterruptState(state);
}

void RingtoneCallback::OnInterrupt(const AudioStandard::InterruptAction &interruptAction)
{
    AudioStandard::InterruptActionType type = interruptAction.actionType;
    AudioProxy::isVoiceCallActivated_ = false;
    AudioInterruptState state = AudioInterruptState::INTERRUPT_STATE_UNKNOWN;
    switch (type) {
        case AudioStandard::InterruptActionType::TYPE_ACTIVATED:
            AudioProxy::isRingtoneActivated_ = true;
            AudioProxy::currentSessionId_ = RINGTONE_SESSION_ID;
            state = AudioInterruptState::INTERRUPT_STATE_RINGING;
            break;
        case AudioStandard::InterruptActionType::TYPE_DEACTIVATED:
            AudioProxy::isRingtoneActivated_ = false;
            AudioProxy::currentSessionId_ = DEFAULT_SESSION_ID;
            state = AudioInterruptState::INTERRUPT_STATE_DEACTIVATED;
            break;
        default:
            AudioProxy::isRingtoneActivated_ = false;
            break;
    }
    DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioInterruptState(state);
}

void AudioDeviceChangeCallback::OnDeviceChange(const AudioStandard::DeviceChangeAction &deviceChangeAction)
{
    AudioStandard::DeviceChangeType changeType = deviceChangeAction.type;
    switch (changeType) {
        case AudioStandard::DeviceChangeType::CONNECT:
#ifdef ABILITY_AUDIO_SUPPORT
            auto devices = deviceChangeAction.deviceDescriptors;
            for (auto device : devices) {
                if (device->getType() == AudioStandard::DeviceType::DEVICE_TYPE_BLUETOOTH_SCO) {
                    DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                        AudioEvent::BLUETOOTH_SCO_CONNECTED);
                } else if (device->getType() == AudioStandard::DeviceType::DEVICE_TYPE_WIRED_HEADSET) {
                    DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                        AudioEvent::WIRED_HEADSET_CONNECTED);
                }
            }
#endif
            break;
        case AudioStandard::DeviceChangeType::DISCONNECT:
#ifdef ABILITY_AUDIO_SUPPORT
            auto devices = deviceChangeAction.deviceDescriptors;
            for (auto device : devices) {
                if (device->getType() == AudioStandard::DeviceType::DEVICE_TYPE_BLUETOOTH_SCO) {
                    DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                        AudioEvent::BLUETOOTH_SCO_DISCONNECTED);
                } else if (device->getType() == AudioStandard::DeviceType::DEVICE_TYPE_WIRED_HEADSET) {
                    DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                        AudioEvent::WIRED_HEADSET_DISCONNECTED);
                }
            }
#endif
            break;
        default:
            break;
    }
}

std::string AudioProxy::GetSystemRingtoneUri() const
{
    if (audioSoundManager_ == nullptr) {
        TELEPHONY_LOGE("audio sound manager nullptr");
        return "";
    }
    if (context_ == nullptr) {
        TELEPHONY_LOGE("context nullptr");
        return "";
    }
    AudioStandard::RingtoneType rinigtoneType = AudioStandard::RingtoneType::RINGTONE_TYPE_DEFAULT;
    return audioSoundManager_->GetSystemRingtoneUri(context_, rinigtoneType);
}

int32_t AudioProxy::StartVibrate()
{
#ifdef ABILITY_SENSOR_SUPPORT
    return VibratorManager::GetInstance()->StartVibrate();
#endif
    return TELEPHONY_SUCCESS;
}

int32_t AudioProxy::CancelVibrate()
{
#ifdef ABILITY_SENSOR_SUPPORT
    return VibratorManager::GetInstance()->CancelVibrate();
#endif
    return TELEPHONY_SUCCESS;
}

std::string AudioProxy::GetDefaultRingPath() const
{
    return defaultRingPath_;
}

std::string AudioProxy::GetDefaultTonePath() const
{
    return defaultTonePath_;
}

std::string AudioProxy::GetDefaultDtmfPath() const
{
    return defaultDtmfPath_;
}
} // namespace Telephony
} // namespace OHOS