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

#ifndef TELEPHONY_AUDIO_MANAGER_H
#define TELEPHONY_AUDIO_MANAGER_H

#include <mutex>
#include <set>

#include "audio_device_manager.h"
#include "audio_proxy.h"
#include "audio_scene_processor.h"
#include "call_manager_inner_type.h"
#include "call_state_listener_base.h"
#include "ring.h"
#include "singleton.h"
#include "sound.h"
#include "tone.h"

namespace OHOS {
namespace Telephony {
class AudioControlManager : public CallStateListenerBase, public std::enable_shared_from_this<AudioControlManager> {
    DECLARE_DELAYED_SINGLETON(AudioControlManager)

public:
    void Init();
    void UnInit();
    int32_t SetAudioDevice(const AudioDevice &device);
    int32_t SetAudioDevice(const AudioDevice &device, bool isByUser);
    bool PlayRingtone(); // plays the default ringtone
    bool StopRingtone();
    int32_t PlayRingback();
    int32_t StopRingback();
    int32_t PlayWaitingTone();
    int32_t StopWaitingTone();
    int32_t PlayDtmfTone(char str);
    int32_t StopDtmfTone();
    int32_t OnPostDialNextChar(char str);
    int32_t PlayCallTone(ToneDescriptor type);
    int32_t StopCallTone();
    int32_t MuteRinger();
    int32_t SetMute(bool on);
    void SetVolumeAudible();
    bool IsTonePlaying();
    bool IsAudioActivated() const;
    bool IsCurrentRinging() const;
    bool IsActiveCallExists() const;
    bool ShouldSwitchActive() const;
    bool ShouldSwitchDialing() const;
    bool ShouldSwitchAlerting() const;
    bool ShouldSwitchIncoming() const;
    AudioDeviceType GetInitAudioDeviceType() const;
    std::set<sptr<CallBase>> GetCallList();
    sptr<CallBase> GetCurrentActiveCall();
    AudioInterruptState GetAudioInterruptState();
    bool UpdateCurrentCallState();
    void SetRingState(RingState state);
    void SetSoundState(SoundState state);
    void SetToneState(ToneState state);
    void SetLocalRingbackNeeded(bool isNeeded);
    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(const DisconnectedDetails &details) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    void VideoStateUpdated(
        sptr<CallBase> &callObjectPtr, VideoStateType priorVideoState, VideoStateType nextVideoState);
    void CheckTypeAndSetAudioDevice(sptr<CallBase> &callObjectPtr, VideoStateType priorVideoState,
        VideoStateType nextVideoState, AudioDeviceType &initDeviceType, AudioDevice &device);
    void UpdateDeviceTypeForVideoOrSatelliteCall();
    void UpdateDeviceTypeForCrs(AudioDeviceType deviceType);
    void UpdateDeviceTypeForVideoDialing();
    void MuteNetWorkRingTone();
    bool IsVideoCall(VideoStateType videoState);
    bool IsSoundPlaying();
    bool StopSoundtone();
    bool PlaySoundtone();
    void PlayCallEndedTone(CallEndedType type);
    void HandleNotNormalRingerMode(CallEndedType type);
    bool IsDistributeCallSinkStatus();
    void SetRingToneVolume(float volume);
    bool IsScoTemporarilyDisabled();
    void ExcludeBluetoothSco();
    void UnexcludeBluetoothSco();

private:
    RingState ringState_ = RingState::STOPPED;
    void HandleNextState(sptr<CallBase> &callObjectPtr, TelCallState nextState);
    void HandlePriorState(sptr<CallBase> &callObjectPtr, TelCallState priorState);
    void HandleCallStateUpdatedForVoip(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState);
    void HandleCallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState);
    void HandleNewActiveCall(sptr<CallBase> &callObjectPtr);
    bool IsNumberAllowed(const std::string &phoneNum);
    sptr<CallBase> GetCallBase(int32_t callId);
    AudioInterruptState audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_DEACTIVATED;
    bool ShouldPlayRingtone() const;
    bool IsEmergencyCallExists();
    void UpdateForegroundLiveCall();
    bool IsBtOrWireHeadPlugin();
    void ProcessAudioWhenCallActive(sptr<CallBase> &callObjectPtr);
    int32_t HandleDistributeAudioDevice(const AudioDevice &device);
    int32_t HandleBluetoothOrNearlinkAudioDevice(const AudioDevice &device);
    void SendMuteRingEvent();
    bool IsRingingVibrateModeOn();
    bool IsVoIPCallActived();
    int32_t SwitchAudioDevice(AudioDeviceType audioDeviceType);
    bool IsBtCallDisconnected();
    void AdjustVolumesForCrs();
    void SaveVoiceVolume(int32_t volume);
    int32_t GetBackupVoiceVolume();
    void RestoreVoiceValumeIfNecessary();
    void PostProcessRingtone();
    ToneState toneState_ = ToneState::STOPPED;
    SoundState soundState_ = SoundState::STOPPED;
    bool isLocalRingbackNeeded_ = false;
    bool isCrsVibrating_ = false;
    std::set<sptr<CallBase>> totalCalls_;
    std::unique_ptr<Ring> ring_;
    std::unique_ptr<Tone> tone_;
    std::unique_ptr<Sound> sound_;
    std::mutex mutex_;
    std::recursive_mutex toneStateLock_;
    sptr<CallBase> frontCall_ = nullptr;
    bool isSetAudioDeviceByUser_ = false;
    bool isScoTemporarilyDisabled_ = false;
    int32_t voiceVolume_ = -1;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_MANAGER_H
