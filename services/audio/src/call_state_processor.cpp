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

#include "call_state_processor.h"

#include "audio_control_manager.h"
#include "audio_scene_processor.h"
#include "call_object_manager.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallStateProcessor::CallStateProcessor() {}

CallStateProcessor::~CallStateProcessor()
{
    activeCalls_.clear();
    alertingCalls_.clear();
    incomingCalls_.clear();
    holdingCalls_.clear();
    dialingCalls_.clear();
}

void CallStateProcessor::AddCall(int32_t callId, TelCallState state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    switch (state) {
        case TelCallState::CALL_STATUS_DIALING:
            if (dialingCalls_.count(callId) == EMPTY_VALUE) {
                TELEPHONY_LOGI("add call , state : dialing");
                dialingCalls_.insert(callId);
            }
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            if (alertingCalls_.count(callId) == EMPTY_VALUE) {
                TELEPHONY_LOGI("add call , state : alerting");
                alertingCalls_.insert(callId);
            }
            break;
        case TelCallState::CALL_STATUS_WAITING:
        case TelCallState::CALL_STATUS_INCOMING:
            if (incomingCalls_.count(callId) == EMPTY_VALUE) {
                TELEPHONY_LOGI("add call , state : incoming");
                incomingCalls_.insert(callId);
            }
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
        case TelCallState::CALL_STATUS_ANSWERED:
            if (activeCalls_.count(callId) == EMPTY_VALUE) {
                TELEPHONY_LOGI("add call , state : active");
                activeCalls_.insert(callId);
            }
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            if (holdingCalls_.count(callId) == EMPTY_VALUE) {
                TELEPHONY_LOGI("add call , state : holding");
                holdingCalls_.insert(callId);
            }
            break;
        default:
            break;
    }
}

void CallStateProcessor::DeleteCall(int32_t callId, TelCallState state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    switch (state) {
        case TelCallState::CALL_STATUS_DIALING:
            if (dialingCalls_.count(callId) > EMPTY_VALUE) {
                TELEPHONY_LOGI("erase call , state : dialing");
                dialingCalls_.erase(callId);
            }
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            if (alertingCalls_.count(callId) > EMPTY_VALUE) {
                TELEPHONY_LOGI("erase call , state : alerting");
                alertingCalls_.erase(callId);
            }
            break;
        case TelCallState::CALL_STATUS_WAITING:
        case TelCallState::CALL_STATUS_INCOMING:
            if (incomingCalls_.count(callId) > EMPTY_VALUE) {
                TELEPHONY_LOGI("erase call , state : incoming");
                incomingCalls_.erase(callId);
            }
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            if (activeCalls_.count(callId) > EMPTY_VALUE) {
                TELEPHONY_LOGI("erase call , state : active");
                activeCalls_.erase(callId);
            }
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            if (holdingCalls_.count(callId) > EMPTY_VALUE) {
                TELEPHONY_LOGI("erase call , state : holding");
                holdingCalls_.erase(callId);
            }
            break;
        default:
            break;
    }
}

int32_t CallStateProcessor::GetCallNumber(TelCallState state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t number = EMPTY_VALUE;
    switch (state) {
        case TelCallState::CALL_STATUS_DIALING:
            number = static_cast<int32_t>(dialingCalls_.size());
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            number = static_cast<int32_t>(alertingCalls_.size());
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            number = static_cast<int32_t>(incomingCalls_.size());
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            number = static_cast<int32_t>(activeCalls_.size());
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            number = static_cast<int32_t>(holdingCalls_.size());
            break;
        default:
            break;
    }
    return number;
}

bool CallStateProcessor::ShouldSwitchState(TelCallState callState)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bool shouldSwitch = false;
    switch (callState) {
        case TelCallState::CALL_STATUS_DIALING:
            shouldSwitch = (dialingCalls_.size() == EXIST_ONLY_ONE_CALL && activeCalls_.empty() &&
                incomingCalls_.empty() && alertingCalls_.empty());
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            shouldSwitch = (alertingCalls_.size() == EXIST_ONLY_ONE_CALL && activeCalls_.empty() &&
                incomingCalls_.empty() && dialingCalls_.empty());
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            shouldSwitch = (incomingCalls_.size() == EXIST_ONLY_ONE_CALL && activeCalls_.empty() &&
                dialingCalls_.empty() && alertingCalls_.empty());
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            shouldSwitch = (activeCalls_.size() == EXIST_ONLY_ONE_CALL);
            break;
        default:
            break;
    }
    return shouldSwitch;
}

bool CallStateProcessor::UpdateCurrentCallState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (activeCalls_.size() > EMPTY_VALUE) {
        // no need to update call state while active calls exists
        return false;
    }
    AudioEvent event = AudioEvent::UNKNOWN_EVENT;
    if (holdingCalls_.size() > EMPTY_VALUE) {
        event = AudioEvent::SWITCH_HOLDING_STATE;
    } else if (incomingCalls_.size() > EMPTY_VALUE) {
        event = AudioEvent::SWITCH_INCOMING_STATE;
    } else if (dialingCalls_.size() > EMPTY_VALUE) {
        event = AudioEvent::SWITCH_DIALING_STATE;
    } else if (alertingCalls_.size() > EMPTY_VALUE) {
        event = AudioEvent::SWITCH_ALERTING_STATE;
    } else {
        event = AudioEvent::SWITCH_AUDIO_INACTIVE_STATE;
    }
    TELEPHONY_LOGI("UpdateCurrentCallState ProcessEvent event=%{public}d", event);
    return DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(event);
}

bool CallStateProcessor::ShouldStopSoundtone()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (activeCalls_.size() > EMPTY_VALUE
        || (incomingCalls_.size() > EMPTY_VALUE && CallObjectManager::HasIncomingCallCrsType())) {
        // no need to stop soundtone
        return false;
    }
    if (holdingCalls_.size() == EMPTY_VALUE && dialingCalls_.size() == EMPTY_VALUE &&
        alertingCalls_.size() == EMPTY_VALUE) {
        return true;
    }
    return false;
}

int32_t CallStateProcessor::GetAudioForegroundLiveCall()
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t callId = INVALID_CALLID;
    if (activeCalls_.size() > EMPTY_VALUE) {
        callId = *activeCalls_.begin();
    } else if (dialingCalls_.size() > EMPTY_VALUE) {
        callId = *dialingCalls_.begin();
    } else if (alertingCalls_.size() > EMPTY_VALUE) {
        callId = *alertingCalls_.begin();
    } else if (incomingCalls_.size() > EMPTY_VALUE) {
        callId = *incomingCalls_.begin();
    } else if (holdingCalls_.size() > EMPTY_VALUE) {
        callId = *holdingCalls_.begin();
    }
    return callId;
}

int32_t CallStateProcessor::GetCurrentActiveCall()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (activeCalls_.size() > EMPTY_VALUE) {
        return (*activeCalls_.begin());
    }
    return INVALID_CALLID;
}
} // namespace Telephony
} // namespace OHOS