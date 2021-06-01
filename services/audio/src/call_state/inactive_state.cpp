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

#include "inactive_state.h"

#include "call_manager_log.h"

#include "audio_proxy.h"
#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
void InActiveState::StateBegin(const std::string msg)
{
    CALLMANAGER_DEBUG_LOG("inactive state enter");
    if (!CallStateManager::IsInitialized()) {
        return;
    }
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return;
    }
    if (AudioControlManager::IsAudioActive()) {
        audioProxy->DeactivateAudioInterrupt(); // abandon audio interrupt
    }
    audioProxy->UpdateCallState(AudioCallState::IN_IDLE);
    CallStateManager::SetCurrentCallState(AudioCallState::IN_IDLE);
    AudioControlManager::SetAudioInterruptState(AudioInterruptState::UN_INTERRUPT);
}

void InActiveState::StateProcess(int32_t event)
{
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case CallStateManager::COMING_CS_CALL:
            AudioControlManager::HandleEvent(CallStateManager::SWITCH_CS_CALL); // switch to cs call state
            break;
        case CallStateManager::COMING_IMS_CALL:
            AudioControlManager::HandleEvent(CallStateManager::SWITCH_IMS_CALL); // switch to ims call state
            break;
        case CallStateManager::COMING_RINGING_CALL: // new incoming call
            AudioControlManager::HandleEvent(CallStateManager::SWITCH_RINGING); // switch to ringing state
            break;
        default:
            break;
    }
    CALLMANAGER_DEBUG_LOG("lock release");
}

void InActiveState::StateEnd(const std::string msg)
{
    CALLMANAGER_DEBUG_LOG("inactive state leave");
}
} // namespace TelephonyCallManager
} // namespace OHOS