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

#ifdef ABILITY_AUDIO_SUPPORT
#include "if_audio_ability.h"
#include "audio_service.h"
#include "iaudio_service.h"
#include "sound_player.h"
#endif

#include "call_manager_errors.h"

namespace OHOS {
namespace TelephonyCallManager {
enum AudioRingMode { RINGER_MODE_NORMAL = 0, RINGER_MODE_SILENT, RINGER_MODE_VIBRATE };

enum AudioVolumeType { STREAM_BLUETOOTH_SCO = 0, STREAM_DTMF, STREAM_MUSIC, STREAM_RING, STREAM_VOICE_CALL };

enum RingtoneType { ALARM = 0, NOTIFICATION, RING };

enum AudioDeviceType { DEV_BLUETOOTH = 0, DEV_SPEAKERPHONE, DEV_WIRED_HEADSET };

enum ErrCode { ERR_OK = 0, ERR_INVALID_VALUE };

class AudioProxy : public std::enable_shared_from_this<AudioProxy> {
    DECLARE_DELAYED_SINGLETON(AudioProxy)
public:
    void Init();
    void ActivateAudioInterrupt();
    void DeactivateAudioInterrupt();
    void UpdateCallState(int state);
    bool IsDeviceActive(int deviceType);
    void SetDeviceActive(int deviceType, bool state);
    std::string GetDefaultRingerPath() const;
    void SetBluetoothDevActive(bool active);
    void SetSpeakerDevActive(bool active);
    void SetWiredHeadsetDevActive(bool active);
    void SetEarpieceDevActive(bool active);
    void SetAudioDeviceChangeObserver();
    void SetOnCreateCompleteListener(); // audio resource create complete listener
    AudioRingMode GetRingerMode() const;
    bool IsVibrateMode() const;
    int32_t CreateRing(const std::string &ringPath);
    int32_t CreateTone(int toneDescriptor, int durationMs);
    int32_t Play(int id, double volume, int loopNum, double speed); // play ringtone
    int32_t Play(const std::string &ringtonePath, double volume, int loopNum, double speed); // play ringtone
    int32_t Play(int toneDescriptor, int duration); // play tone
    int32_t Play();
    int32_t StopCallTone();
    int32_t StopRing(int id);
    int32_t Release();
    int32_t Release(int id);
    int32_t GetVolume(int audioVolumeType);
    void SetVolume(int audioVolumeType, int volume);
    bool IsMute(int audioVolumeType);
    int32_t GetMaxVolume(int audioVolumeType);
    int32_t GetMinVolume(int audioVolumeType);
    bool IsMicrophoneMute();
    void SetMicrophoneMute(bool mute);
    int32_t StartVibrate();
    int32_t CancelVibrate();

private:
    std::mutex proxyMutex_;
    sptr<IRemoteObject> remoteObject_;
    std::unique_ptr<AudioManagerProxy> audioManagerProxy_;
#ifdef ABILITY_AUDIO_SUPPORT
    sptr<IAudioAbility> audioAbility_;
    sptr<IAudioService> audioService_;
    sptr<SoundPlayer> player_;
#endif
};
} // namespace TelephonyCallManager
} // namespace OHOS
#endif // !AUDIO_PROXY_H