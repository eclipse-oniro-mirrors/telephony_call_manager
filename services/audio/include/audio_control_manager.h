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

#ifndef TELEPHONY_AUDIO_MANAGER_H
#define TELEPHONY_AUDIO_MANAGER_H

#include <mutex>
#include <set>

#include "singleton.h"

#include "call_manager_inner_type.h"

#include "audio_proxy.h"
#include "call_state_listener_base.h"
#include "audio_tone.h"
#include "audio_ring.h"
#include "audio_device_manager.h"
#include "call_state_processor.h"

namespace OHOS {
namespace Telephony {
constexpr uint32_t EXIST_ONLY_ONE_CALL = 1;
struct CallRecord {
    std::string phoneNum;
    bool isIms;
};

class AudioControlManager : public CallStateListenerBase, public std::enable_shared_from_this<AudioControlManager> {
    DECLARE_DELAYED_SINGLETON(AudioControlManager)
public:
    void Init();
    bool SetAudioDevice(int32_t device);
    bool SetAudioDevice(AudioDevice device);
    bool PlayRingtone(); // plays the default ringtone
    bool PlayRingtone(const std::string &phoneNum); // plays the default ringtone
    bool PlayRingtone(const std::string &phoneNum, const std::string &ringtonePath); // plays the specific ringtone
    bool StopRingtone();
    int32_t PlayRingback();
    int32_t StopRingback();
    int32_t PlayWaitingTone();
    int32_t StopWaitingTone();
    int32_t PlayHoldingTone();
    int32_t StopHoldingTone();
    int32_t PlayCallEndedTone(ToneDescriptor type);
    int32_t PlayCallTone(ToneDescriptor type);
    int32_t StopCallTone();
    int32_t MuteRinger();
    int32_t SetMute(bool on);
    void TurnOffVoice();
    void SetVolumeAudible();
    bool IsTonePlaying() const;
    bool IsAudioActivated() const;
    bool IsCurrentRinging() const;
    bool IsCurrentVideoCall() const;
    bool IsActiveCallExists() const;
    bool IsOnlyOneActiveCall() const;
    bool IsOnlyOneAlertingCall() const;
    bool IsOnlyOneIncomingCall() const;
    bool ShouldSwitchAlertingState() const;
    bool ShouldSwitchIncomingState() const;
    AudioDevice GetInitAudioDevice() const;
    sptr<CallBase> GetCurrentCall() const;
    std::set<sptr<CallBase>> GetCallList();
    AudioInterruptState GetAudioInterruptState();
    bool UpdateCurrentCallState();
    void SetRingState(RingState state);
    void SetAudioInterruptState(AudioInterruptState state);
    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(sptr<CallBase> &callObjectPtr) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;

private:
    RingState ringState_ = RingState::STOPPED;
    void AddCall(const std::string &phoneNum, TelCallState state);
    void DeleteCall(const std::string &phoneNum, TelCallState state);
    void HandleCallStateUpdated(TelCallState stateType, bool isAdded, CallType callType);
    void HandleCallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState);
    void AddNewActiveCall(CallType callType);
    bool IsAllowNumber(const std::string &phoneNum);
    void PlayCallEndedTone(TelCallState priorState, TelCallState nextState, CallEndedType type);
    sptr<CallBase> GetCallBase(const std::string &phoneNum) const;
    std::string GetIncomingCallRingtonePath();
    AudioInterruptState audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_DEACTIVATED;
    bool IsHoldingCallExists() const;
    bool IsAlertingCallExists() const;
    bool IsIncomingCallExists() const;
    bool IsEmergencyCallExists() const;
    bool isTonePlaying_;
    std::set<std::string> activeCalls_;
    std::set<std::string> holdingCalls_;
    std::set<std::string> alertingCalls_;
    std::set<std::string> incomingCalls_;
    std::set<sptr<CallBase>> totalCalls_;
    std::unique_ptr<AudioRing> ring_;
    std::unique_ptr<AudioTone> tone_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_MANAGER_H