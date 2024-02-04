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
#include "audio_system_manager.h"
#include "call_manager_errors.h"
#include "call_manager_inner_type.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
constexpr uint16_t VOLUME_AUDIBLE_DIVISOR = 2;

enum AudioInterruptState {
    INTERRUPT_STATE_UNKNOWN = 0,
    INTERRUPT_STATE_DEACTIVATED,
    INTERRUPT_STATE_ACTIVATED,
    INTERRUPT_STATE_RINGING,
};

enum class VibrationType {
    VIBRATION_RINGTONE = 0,
};

class AudioDeviceChangeCallback : public AudioStandard::AudioManagerDeviceChangeCallback {
    void OnDeviceChange(const AudioStandard::DeviceChangeAction &deviceChangeAction) override;
};

class AudioPreferDeviceChangeCallback : public AudioStandard::AudioPreferredOutputDeviceChangeCallback {
public:
    void OnPreferredOutputDeviceUpdated(const std::vector<sptr<AudioStandard::AudioDeviceDescriptor>> &desc) override;
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
    int32_t StartVibrator();
    int32_t StopVibrator();
    bool SetMicrophoneMute(bool mute);
    bool SetEarpieceDevActive();
    bool SetSpeakerDevActive();
    bool SetBluetoothDevActive();
    bool SetWiredHeadsetDevActive();
    AudioStandard::AudioRingerMode GetRingerMode() const;
    int32_t GetVolume(AudioStandard::AudioVolumeType audioVolumeType);
    int32_t SetVolume(AudioStandard::AudioVolumeType audioVolumeType, int32_t volume);
    int32_t SetMaxVolume(AudioStandard::AudioVolumeType audioVolumeType);
    bool IsStreamActive(AudioStandard::AudioVolumeType audioVolumeType);
    bool IsStreamMute(AudioStandard::AudioVolumeType audioVolumeType);
    int32_t GetMaxVolume(AudioStandard::AudioVolumeType audioVolumeType);
    int32_t GetMinVolume(AudioStandard::AudioVolumeType audioVolumeType);
    int32_t SetAudioDeviceChangeCallback();
    bool IsVibrateMode() const;
    int32_t StartVibrate();
    int32_t CancelVibrate();
    std::string GetDefaultRingPath() const;
    std::string GetDefaultTonePath() const;
    std::string GetDefaultDtmfPath() const;
    int32_t UnsetDeviceChangeCallback();
    void SetWiredHeadsetState(bool isConnected);
    int32_t GetPreferredOutputAudioDevice(AudioDevice &device);
    int32_t SetAudioPreferDeviceChangeCallback();
    int32_t UnsetAudioPreferDeviceChangeCallback();

private:
    const std::string defaultRingPath_ = "/system/etc/telephony/rings/ring.wav";
    const std::string defaultTonePath_ = "/system/etc/telephony/tones/tone.wav";
    const std::string defaultDtmfPath_ = "/system/etc/telephony/dtmfs/dtmf.wav";
    std::shared_ptr<AudioStandard::AudioManagerDeviceChangeCallback> deviceCallback_;
    std::shared_ptr<AudioStandard::AudioPreferredOutputDeviceChangeCallback> preferredDeviceCallback_;
    bool isWiredHeadsetConnected_ = false;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_PROXY_H
