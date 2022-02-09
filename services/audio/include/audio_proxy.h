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

#ifndef TELEPHONY_AUDIO_PROXY_H
#define TELEPHONY_AUDIO_PROXY_H

#include <cstdint>
#include <memory>
#include <mutex>

#include "audio_manager_proxy.h"
#include "singleton.h"
#include "audio_ringtone_manager.h"

#include "call_manager_errors.h"

namespace OHOS {
namespace Telephony {
constexpr uint16_t VOLUME_AUDIBLE_DIVISOR = 2;

enum AudioInterruptState {
    INTERRUPT_STATE_UNKNOWN = 0,
    INTERRUPT_STATE_DEACTIVATED,
    INTERRUPT_STATE_ACTIVATED,
    INTERRUPT_STATE_RINGING,
};

class AudioDeviceChangeCallback : public AudioStandard::AudioManagerDeviceChangeCallback {
    void OnDeviceChange(const AudioStandard::DeviceChangeAction &deviceChangeAction) override;
};

class AudioProxy : public std::enable_shared_from_this<AudioProxy> {
    DECLARE_DELAYED_SINGLETON(AudioProxy)
public:
    bool SetAudioScene(AudioStandard::AudioScene audioScene);
    int32_t ActivateAudioInterrupt(const AudioStandard::AudioInterrupt &audioInterrupt);
    int32_t DeactivateAudioInterrupt(const AudioStandard::AudioInterrupt &audioInterrupt);
    int32_t DeactivateAudioInterrupt();
    void SetVolumeAudible();
    bool IsMicrophoneMute();
    bool SetMicrophoneMute(bool mute);
    bool SetEarpieceDevActive();
    bool SetSpeakerDevActive();
    bool SetBluetoothDevActive();
    bool SetWiredHeadsetDevActive();
    AudioStandard::AudioRingerMode GetRingerMode() const;
    int32_t GetVolume(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType);
    int32_t SetVolume(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType, int32_t volume);
    int32_t SetMaxVolume(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType);
    bool IsStreamActive(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType);
    bool IsStreamMute(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType);
    int32_t GetMaxVolume(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType);
    int32_t GetMinVolume(AudioStandard::AudioSystemManager::AudioVolumeType audioVolumeType);
    int32_t SetAudioDeviceChangeCallback();
    bool IsVibrateMode() const;
    int32_t StartVibrate();
    int32_t CancelVibrate();
    std::string GetSystemRingtoneUri() const;
    std::string GetDefaultRingPath() const;
    std::string GetDefaultTonePath() const;
    std::string GetDefaultDtmfPath() const;

private:
    const std::string defaultRingPath_ = "/system/data/telephony/rings/ring.wav";
    const std::string defaultTonePath_ = "/system/data/telephony/tones/tone.wav";
    const std::string defaultDtmfPath_ = "/system/data/telephony/dtmfs/dtmf.wav";
    std::shared_ptr<AbilityRuntime::Context> context_;
    std::unique_ptr<AudioStandard::RingtoneSoundManager> audioSoundManager_;
    std::shared_ptr<AudioStandard::AudioManagerDeviceChangeCallback> deviceCallback_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_PROXY_H
