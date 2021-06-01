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

#include "call_state_manager.h"

#include "call_manager_log.h"

#include "cs_call_state.h"
#include "holding_state.h"
#include "ims_call_state.h"
#include "inactive_state.h"
#include "ringing_state.h"

namespace OHOS {
namespace TelephonyCallManager {
AudioCallState CallStateManager::currentCallState_ = AudioCallState::IN_IDLE;
bool CallStateManager::isInitialized_ = false;

CallStateManager::CallStateManager() : audioState_(nullptr), currentInCallState_(UNKNOWN_STATE) {}

CallStateManager::~CallStateManager() {}

void CallStateManager::Init()
{
    CALLMANAGER_INFO_LOG("call state manager init");
    SwitchState(InCallState::INACTIVE_STATE); // first enter inactive state , no need to deactivate audio interrupt
    isInitialized_ = true; // if enter inactive state again , should deactivate audio interrupt
}

void CallStateManager::HandleEvent(int32_t event)
{
    if (audioState_ == nullptr) {
        CALLMANAGER_INFO_LOG("audio state nullptr");
        return;
    }
    switch (event) {
        case CallStateManager::SWITCH_CS_CALL:
            SwitchState(InCallState::CS_CALL_STATE);
            break;
        case CallStateManager::SWITCH_IMS_CALL:
            SwitchState(InCallState::IMS_CALL_STATE);
            break;
        case CallStateManager::SWITCH_RINGING:
            SwitchState(InCallState::RINGING_STATE);
            break;
        case CallStateManager::SWITCH_HOLDING:
            SwitchState(InCallState::HOLDING_STATE);
            break;
        case CallStateManager::ABANDON_AUDIO:
            SwitchState(InCallState::INACTIVE_STATE);
            break;
        case CallStateManager::COMING_CS_CALL:
        case CallStateManager::COMING_IMS_CALL:
        case CallStateManager::COMING_RINGING_CALL:
        case CallStateManager::NONE_ALERTING_OR_ACTIVE_CALLS:
        case CallStateManager::NONE_RINGING_CALLS:
            audioState_->StateProcess(event);
            break;
        default:
            break;
    }
}

void CallStateManager::SwitchState(InCallState state)
{
    CALLMANAGER_INFO_LOG("switch state : %{public}d", state);
    if (currentInCallState_ == state) {
        CALLMANAGER_INFO_LOG("no need to switch call state");
        return;
    }
    switch (state) {
        case InCallState::INACTIVE_STATE: // transfer to inactive state
            currentInCallState_ = INACTIVE_STATE;
            CALLMANAGER_INFO_LOG("current call state : inactive state");
            DoSwitch(InCallState::INACTIVE_STATE);
            break;
        case InCallState::RINGING_STATE: // transfer to ringing state
            currentInCallState_ = RINGING_STATE;
            CALLMANAGER_INFO_LOG("current call state : ringing state");
            DoSwitch(InCallState::RINGING_STATE);
            break;
        case InCallState::CS_CALL_STATE: // transfer to cs call state
            currentInCallState_ = CS_CALL_STATE;
            CALLMANAGER_INFO_LOG("current call state : cs call state");
            DoSwitch(InCallState::CS_CALL_STATE);
            break;
        case InCallState::IMS_CALL_STATE: // transfer to ims call state
            currentInCallState_ = IMS_CALL_STATE;
            CALLMANAGER_INFO_LOG("current call state : ims call state");
            DoSwitch(InCallState::IMS_CALL_STATE);
            break;
        case InCallState::HOLDING_STATE: // transfer to holding state
            currentInCallState_ = HOLDING_STATE;
            CALLMANAGER_INFO_LOG("current call state : holding state");
            DoSwitch(InCallState::HOLDING_STATE);
            break;
        default:
            CALLMANAGER_INFO_LOG("invalid in call state");
            break;
    }
}

void CallStateManager::DoSwitch(InCallState state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (audioState_ != nullptr) {
        audioState_->StateEnd(""); // leave pre audio state
    }
    switch (state) {
        case InCallState::INACTIVE_STATE:
            audioState_ = std::make_unique<InActiveState>();
            break;
        case InCallState::RINGING_STATE:
            audioState_ = std::make_unique<RingingState>();
            break;
        case InCallState::CS_CALL_STATE:
            audioState_ = std::make_unique<CSCallState>();
            break;
        case InCallState::IMS_CALL_STATE:
            audioState_ = std::make_unique<IMSCallState>();
            break;
        case InCallState::HOLDING_STATE:
            audioState_ = std::make_unique<HoldingState>();
            break;
        default:
            break;
    }
    if (audioState_ != nullptr) {
        audioState_->StateBegin(""); // switch a new call state
    }
}

template<typename T>
void CallStateManager::DoSwitch(const std::unique_ptr<T> &state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (audioState_ != nullptr) {
        audioState_->StateEnd(""); // leave pre call state
    }
    audioState_ = state;
    if (audioState_ != nullptr) {
        audioState_->StateBegin(""); // switch a new call state
    }
}

void CallStateManager::SetCurrentCallState(AudioCallState state)
{
    currentCallState_ = state;
}

AudioCallState CallStateManager::GetCurrentCallState()
{
    return currentCallState_;
}

bool CallStateManager::IsInitialized()
{
    return isInitialized_;
}
} // namespace TelephonyCallManager
} // namespace OHOS