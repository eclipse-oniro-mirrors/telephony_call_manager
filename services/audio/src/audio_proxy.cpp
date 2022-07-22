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
AudioProxy::AudioProxy()
    : context_(nullptr), audioSoundManager_(std::make_unique<AudioStandard::RingtoneSoundManager>()),
      deviceCallback_(std::make_shared<AudioDeviceChangeCallback>())
{}

AudioProxy::~AudioProxy() {}

bool AudioProxy::SetAudioScene(AudioStandard::AudioScene audioScene)
{
    return (AudioStandard::AudioSystemManager::GetInstance()->SetAudioScene(audioScene) == TELEPHONY_SUCCESS);
}

int32_t AudioProxy::SetAudioDeviceChangeCallback()
{
    if (deviceCallback_ == nullptr) {
        TELEPHONY_LOGE("device callback nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return AudioStandard::AudioSystemManager::GetInstance()->SetDeviceChangeCallback(
        AudioStandard::DeviceFlag::ALL_DEVICES_FLAG, deviceCallback_);
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

bool AudioProxy::SetWiredHeadsetDevActive()
{
    return false;
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
    if (mute == IsMicrophoneMute()) {
        return true;
    }
    int32_t muteResult = AudioStandard::AudioSystemManager::GetInstance()->SetMicrophoneMute(mute);
    TELEPHONY_LOGI("set microphone mute result : %{public}d", muteResult);
    return (muteResult == TELEPHONY_SUCCESS);
}

AudioStandard::AudioRingerMode AudioProxy::GetRingerMode() const
{
    return AudioStandard::AudioSystemManager::GetInstance()->GetRingerMode();
}

bool AudioProxy::IsVibrateMode() const
{
    return (AudioStandard::AudioRingerMode::RINGER_MODE_VIBRATE == GetRingerMode());
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
