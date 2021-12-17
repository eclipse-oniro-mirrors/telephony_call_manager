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
using namespace AudioStandard;

AudioProxy::AudioProxy() {}

AudioProxy::~AudioProxy() {}

void AudioProxy::Init() {}

bool AudioProxy::SetAudioMode(int32_t audioMode)
{
#ifdef ABILITY_AUDIO_SUPPORT
    return AudioSystemManager::GetInstance()->SetMode(audioMode);
#endif
    return true;
}

int32_t AudioProxy::ActivateAudioInterrupt()
{
    return TELEPHONY_SUCCESS;
}

int32_t AudioProxy::DeactivateAudioInterrupt()
{
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

int32_t AudioProxy::StartVibrate()
{
    return TELEPHONY_SUCCESS; // return default value
}

int32_t AudioProxy::CancelVibrate()
{
    return TELEPHONY_SUCCESS; // return default value
}

void AudioProxy::SetAudioDeviceChangeObserver() {}

void AudioProxy::SetOnCreateCompleteListener() {}

bool AudioProxy::SetBluetoothDevActive()
{
    if (!DelayedSingleton<AudioControlManager>::GetInstance()->IsAudioActivated()) {
        TELEPHONY_LOGE("audio is not activated");
        return false;
    }
    if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(
        AudioStandard::ActiveDeviceType::BLUETOOTH_SCO)) {
        TELEPHONY_LOGI("bluetooth device is already active");
        return true;
    }
#ifdef ABILITY_BLUETOOTH_SUPPORT
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
    if (!DelayedSingleton<AudioControlManager>::GetInstance()->IsAudioActivated()) {
        TELEPHONY_LOGE("audio is not activated");
        return false;
    }
    if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(AudioStandard::ActiveDeviceType::SPEAKER)) {
        TELEPHONY_LOGI("speaker device is already active");
        return true;
    }
#ifdef ABILITY_BLUETOOTH_SUPPORT
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

int32_t AudioProxy::GetVolume(AudioSystemManager::AudioVolumeType audioVolumeType)
{
    return AudioSystemManager::GetInstance()->GetVolume(audioVolumeType);
}

int32_t AudioProxy::SetVolume(AudioSystemManager::AudioVolumeType audioVolumeType, int32_t volume)
{
    return AudioSystemManager::GetInstance()->SetVolume(audioVolumeType, volume);
}

int32_t AudioProxy::SetMaxVolume(AudioSystemManager::AudioVolumeType audioVolumeType)
{
    int32_t maxVolume = GetMaxVolume(audioVolumeType);
    return AudioSystemManager::GetInstance()->SetVolume(audioVolumeType, maxVolume);
}

void AudioProxy::SetVolumeAudible()
{
    if (GetVolume(AudioSystemManager::AudioVolumeType::STREAM_VOICE_CALL) != TELEPHONY_SUCCESS) {
        return;
    }
    int32_t volume = GetMaxVolume(AudioSystemManager::AudioVolumeType::STREAM_VOICE_CALL);
    SetVolume(AudioSystemManager::AudioVolumeType::STREAM_VOICE_CALL, (int32_t)(volume / VOLUME_AUDIBLE_DIVISOR));
}

bool AudioProxy::IsStreamActive(AudioSystemManager::AudioVolumeType audioVolumeType)
{
    return AudioSystemManager::GetInstance()->IsStreamActive(audioVolumeType);
}

bool AudioProxy::IsStreamMute(AudioSystemManager::AudioVolumeType audioVolumeType)
{
    return AudioSystemManager::GetInstance()->IsStreamMute(audioVolumeType);
}

int32_t AudioProxy::GetMaxVolume(AudioSystemManager::AudioVolumeType audioVolumeType)
{
    return AudioSystemManager::GetInstance()->GetMaxVolume(audioVolumeType);
}

int32_t AudioProxy::GetMinVolume(AudioSystemManager::AudioVolumeType audioVolumeType)
{
    return AudioSystemManager::GetInstance()->GetMinVolume(audioVolumeType);
}

bool AudioProxy::IsMicrophoneMute()
{
    return AudioSystemManager::GetInstance()->IsMicrophoneMute();
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

AudioRingerMode AudioProxy::GetRingerMode() const
{
    return AudioSystemManager::GetInstance()->GetRingerMode();
}

bool AudioProxy::IsVibrateMode() const
{
    return AudioRingerMode::RINGER_MODE_VIBRATE == GetRingerMode();
}
} // namespace Telephony
} // namespace OHOS