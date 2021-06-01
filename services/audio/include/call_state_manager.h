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

#ifndef CALL_STATE_MANAGER_H
#define CALL_STATE_MANAGER_H
#include "audio_state.h"

namespace OHOS {
namespace TelephonyCallManager {
enum InCallState {
    INACTIVE_STATE = 0,
    RINGING_STATE,
    CS_CALL_STATE,
    IMS_CALL_STATE,
    HOLDING_STATE,
    UNKNOWN_STATE
};

enum AudioCallState { IN_IDLE = 0, IN_CALL, IN_VOIP, RINGTONE };

enum AudioMode { MODE_NORMAL = 0, MODE_IN_CALL, MODE_IN_COMMUNICATION, MODE_RINGTONE };

/**
 * @class CallStateManager
 * describes the available call states of a call , similar to a state machine.
 */
class CallStateManager {
public:
    CallStateManager();
    ~CallStateManager();
    void Init();
    void HandleEvent(int32_t event);
    static bool IsInitialized();
    static void SetCurrentCallState(AudioCallState state); // audio call state
    static AudioCallState GetCurrentCallState(); // audio call state
    static constexpr uint32_t INCOMING_CALL_ANSWERED = 100;
    static constexpr uint32_t INCOMING_CALL_REJECTED = 101;
    static constexpr uint32_t SWITCH_CS_CALL = 102;
    static constexpr uint32_t SWITCH_IMS_CALL = 103;
    static constexpr uint32_t SWITCH_RINGING = 104;
    static constexpr uint32_t SWITCH_HOLDING = 105;
    static constexpr uint32_t ABANDON_AUDIO = 106;
    static constexpr uint32_t COMING_CS_CALL = 107; // active calls == 1
    static constexpr uint32_t COMING_IMS_CALL = 108; // active calls == 1
    static constexpr uint32_t COMING_RINGING_CALL = 109; // ringing calls == 1
    static constexpr uint32_t NONE_ALERTING_OR_ACTIVE_CALLS = 110; // alerting or active calls == 0
    static constexpr uint32_t NONE_RINGING_CALLS = 111; // ringing calls == 0
    static constexpr uint32_t VIDEO_STATE_CHANGED = 112;

private:
    std::unique_ptr<AudioState> audioState_;
    void SwitchState(InCallState state);
    void DoSwitch(InCallState state);
    static AudioCallState currentCallState_; // audio call state
    static bool isInitialized_;
    InCallState currentInCallState_; // in call state
    std::mutex mutex_;
    template<typename T>
    void DoSwitch(const std::unique_ptr<T> &state);
};
} // namespace TelephonyCallManager
} // namespace OHOS

#endif // !CALL_STATE_MANAGER_H
