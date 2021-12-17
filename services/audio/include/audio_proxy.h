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

#ifndef AUDIO_PROXY_H
#define AUDIO_PROXY_H

#include <cstdint>
#include <memory>
#include <mutex>

#include "audio_manager_proxy.h"
#include "singleton.h"

#include "call_manager_errors.h"

namespace OHOS {
namespace Telephony {
using namespace AudioStandard;
constexpr uint32_t VOLUME_AUDIBLE_DIVISOR = 2;

enum AudioInterruptState {
    INTERRUPT_STATE_UNKNOWN = 0,
    INTERRUPT_STATE_DEACTIVATED,
    INTERRUPT_STATE_ACTIVATED,
    INTERRUPT_STATE_RINGING,
};

class AudioProxy : public std::enable_shared_from_this<AudioProxy> {
    DECLARE_DELAYED_SINGLETON(AudioProxy)
public:
    void Init();
    int32_t ActivateAudioInterrupt();
    int32_t DeactivateAudioInterrupt();
    /**
     * Set audio mode
     * @param state : IN_CALL , IN_VOIP , RINGTONE , IN_IDLE
     */
    bool SetAudioMode(int32_t audioMode);
    std::string GetDefaultRingPath() const;
    std::string GetDefaultTonePath() const;
    std::string GetDefaultDtmfPath() const;
    void SetVolumeAudible();
    bool IsMicrophoneMute();
    bool SetMicrophoneMute(bool mute);
    bool SetEarpieceDevActive();
    bool SetSpeakerDevActive();
    bool SetBluetoothDevActive();
    bool SetWiredHeadsetDevActive();
    AudioRingerMode GetRingerMode() const;
    int32_t GetVolume(AudioSystemManager::AudioVolumeType audioVolumeType);
    int32_t SetVolume(AudioSystemManager::AudioVolumeType audioVolumeType, int32_t volume);
    int32_t SetMaxVolume(AudioSystemManager::AudioVolumeType audioVolumeType);
    bool IsStreamActive(AudioSystemManager::AudioVolumeType audioVolumeType);
    bool IsStreamMute(AudioSystemManager::AudioVolumeType audioVolumeType);
    int32_t GetMaxVolume(AudioSystemManager::AudioVolumeType audioVolumeType);
    int32_t GetMinVolume(AudioSystemManager::AudioVolumeType audioVolumeType);
    void SetAudioDeviceChangeObserver();
    void SetOnCreateCompleteListener();
    bool IsVibrateMode() const;
    int32_t StartVibrate();
    int32_t CancelVibrate();

private:
    bool isVoiceEnabled = false;
    const std::string defaultRingPath_ = "/system/data/telephony/rings/ring.wav";
    const std::string defaultTonePath_ = "/system/data/telephony/tones/tone.wav";
    const std::string defaultDtmfPath_ = "/system/data/telephony/dtmfs/dtmf.wav";
};
} // namespace Telephony
} // namespace OHOS
#endif // AUDIO_PROXY_H