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

#include "call_manager_type.h"

#include "audio_event.h"
#include "audio_event_handler.h"
#include "audio_proxy.h"
#include "call_state_listener_base.h"
#include "ring.h"
#include "tone.h"

namespace OHOS {
namespace TelephonyCallManager {
enum AudioInterruptState { UN_INTERRUPT = 0, IN_INTERRUPT, IN_RINGING };

enum CallDirection { ENTERING = 0, LEAVING };

enum DisconnectReason { UNKNOWN = 0, ENGAGED, FINISHED, NO_SERVICE, INVALID_NUMBER };

struct CallRecord {
    std::string phoneNum;
    bool isIms;
};

class AudioControlManager : public CallStateListenerBase, public std::enable_shared_from_this<AudioControlManager> {
    DECLARE_DELAYED_SINGLETON(AudioControlManager)
public:
    void Init();
    void Reset();
    void SetAudioDevice(AudioDevice device);
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
    void SetVolumeAudible();
    void SetIsTonePlaying(bool isPlaying);
    bool GetIsTonePlaying() const;
    bool IsCurrentCallIms() const; // whether foreground call is ims or not
    bool IsTonePlaying() const;
    static bool IsCurrentRinging();
    static bool IsAudioActive();
    bool NeedToPlayCallEndedTone(TelCallStates priorState, TelCallStates nextState) const;
    static void SetAudioInterruptState(AudioInterruptState state);
    static AudioInterruptState GetAudioInterruptState();
    AudioDevice GetInitAudioDevice() const;
    void UpdateCallState();
    void UpdateMuteState(bool on);
    void TurnOffVoice();
    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(sptr<CallBase> &callObjectPtr) override;
    void CallStateUpdated(
        sptr<CallBase> &callObjectPtr, TelCallStates priorState, TelCallStates nextState) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;
    sptr<CallBase> GetCurrentCall() const;
    static void HandleEvent(int32_t event);
    static bool isAudioInterruptActivated_;

private:
    uint32_t existOneCall_ = 1;
    void HandleCallAdded(TelCallStates state);
    void HandleCallDeleted(TelCallStates state);
    void UpdateCurrentCall(); // update foreground call
    void AddCallByState(const std::string &phoneNum, TelCallStates stateType);
    void DeleteCallByState(const std::string &phoneNum, TelCallStates stateType);
    void DeleteCall(const std::string &phoneNum, TelCallStates stateType);
    bool IsBlackNumber(const std::string &phoneNum);
    void PlayCallEndedTone(CallEndedType type);
    sptr<CallBase> GetCallBase(const std::string &phoneNum) const;
    static AudioInterruptState currentAudioInterruptState_;
    static bool isRinging_;
    bool IsRingingCallExists() const;
    bool IsHoldingCallExists() const;
    bool IsActiveCallExists() const;
    bool IsEmergencyCallExists() const;
    bool isTonePlaying_;
    std::string foregroundCallNum_; // foreground call number
    std::set<std::string> alertingCalls_;
    std::set<std::string> activeCalls_;
    std::set<std::string> ringingCalls_;
    std::set<std::string> holdingCalls_;
    std::set<sptr<CallBase>> totalCalls_;
    std::unique_ptr<Ring> ring_;
    std::unique_ptr<Tone> tone_;
    std::shared_ptr<AppExecFwk::EventRunner> audioEventLoop_;
    std::shared_ptr<AudioEvent> audioEvent_;
    static std::shared_ptr<AudioEventHandler> audioEventHandler_;
};
} // namespace TelephonyCallManager
} // namespace OHOS
#endif // TELEPHONY_AUDIO_MANAGER_H
