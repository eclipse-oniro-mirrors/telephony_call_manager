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

#ifndef AUDIO_DEVICE_MANAGER_H
#define AUDIO_DEVICE_MANAGER_H
#include "audio_state.h"

#include "call_manager_type.h"

namespace OHOS {
namespace TelephonyCallManager {
enum AudioDevice {
    DEVICE_EARPIECE = 0,
    DEVICE_SPEAKER,
    DEVICE_WIRED_HEADSET,
    DEVICE_BLUETOOTH_SCO,
    DEVICE_DISABLE,
    DEVICE_UNKNOWN
};

/**
 * @class AudioDeviceManager
 * describes the available devices of a call , similar to a state machine.
 */
class AudioDeviceManager {
public:
    AudioDeviceManager();
    virtual ~AudioDeviceManager();
    void Init();
    void InitAudioDevice();
    void HandleEvent(int32_t event);
    static bool IsBtScoDevEnable();
    static bool IsWiredHeadsetDevEnable();
    static bool IsSpeakerDevEnable();
    static bool IsEarpieceDevEnable();
    static void SetBtScoDevEnable(bool enable);
    static void SetWiredHeadsetDevEnable(bool enable);
    static void SetSpeakerDevEnable(bool enable);
    static void SetEarpieceDevEnable(bool enable);
    static bool IsSpeakerAvailable();
    static bool IsBtScoAvailable();
    static bool IsWiredHeadsetAvailable();
    static bool IsEarpieceAvailable();
    static void SetEarpieceAvailable(bool available);
    static void SetSpeakerAvailable(bool available);
    static void SetWiredHeadsetAvailable(bool available);
    static void SetBtScoAvailable(bool available);
    static void SetCurrentAudioDevice(AudioDevice device);
    static AudioDevice GetCurrentAudioDevice();
    static constexpr uint32_t AUDIO_INTERRUPTED = 200;
    static constexpr uint32_t AUDIO_UN_INTERRUPT = 201;
    static constexpr uint32_t AUDIO_RINGING = 202;
    static constexpr uint32_t SWITCH_DEVICE_EARPIECE = 203;
    static constexpr uint32_t SWITCH_DEVICE_SPEAKER = 204;
    static constexpr uint32_t SWITCH_DEVICE_WIRED_HEADSET = 205;
    static constexpr uint32_t SWITCH_DEVICE_BLUETOOTH = 206;
    static constexpr uint32_t SWITCH_INACTIVE = 207;
    static constexpr uint32_t WIRED_HEADSET_AVAILABLE = 208; // wired headset , external audio device
    static constexpr uint32_t WIRED_HEADSET_UNAVAILABLE = 209;
    static constexpr uint32_t BLUETOOTH_SCO_AVAILABLE = 210; // bluetooth sco , external audio device
    static constexpr uint32_t BLUETOOTH_SCO_UNAVAILABLE = 211;
    static constexpr uint32_t INIT_AUDIO_DEVICE = 212;

private:
    static AudioDevice currentAudioDevice_;
    static bool isBtScoDevEnable_;
    static bool isWiredHeadsetDevEnable_;
    static bool isSpeakerDevEnable_;
    static bool isEarpieceDevEnable_;
    std::mutex audioDeviceMutex_;
    std::unique_ptr<AudioState> audioState_;
    void SwitchDevice(AudioDevice device);
    void HandleEnterAudioDeviceEvent(int32_t event);
    void DoSwitch(AudioDevice device);
    void SetMuteOn(bool on);
    static bool isEarpieceAvailable_;
    static bool isSpeakerAvailable_;
    static bool isWiredHeadsetAvailable_;
    static bool isBtScoAvailable_;
    bool isEnable_;
    std::mutex mutex_;
    template<typename T>
    void DoSwitch(const std::unique_ptr<T> &state);
};
} // namespace TelephonyCallManager
} // namespace OHOS

#endif // !AUDIO_DEVICE_MANAGER_H
