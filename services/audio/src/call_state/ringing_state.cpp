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

#include "ringing_state.h"

#include "call_manager_log.h"

#include "audio_proxy.h"
#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
void RingingState::StateBegin(const std::string msg)
{
    CALLMANAGER_DEBUG_LOG("ringing state enter");
    auto acm = DelayedSingleton<AudioControlManager>::GetInstance();
    if (acm == nullptr) {
        CALLMANAGER_ERR_LOG("audio control manager nullptr");
        return;
    }
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return;
    }
    if (!AudioControlManager::IsAudioActive()) {
        audioProxy->ActivateAudioInterrupt();
    }
    audioProxy->UpdateCallState(AudioCallState::RINGTONE);
    CallStateManager::SetCurrentCallState(AudioCallState::RINGTONE);
    AudioControlManager::SetAudioInterruptState(AudioInterruptState::IN_RINGING);
}

void RingingState::StateProcess(int32_t event)
{
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case CallStateManager::NONE_RINGING_CALLS:
            HandleNoneRingingCall();
            break;
        case CallStateManager::COMING_CS_CALL:
            // switch to cs call state anyway.
            AudioControlManager::HandleEvent(CallStateManager::SWITCH_CS_CALL);
            break;
        case CallStateManager::COMING_IMS_CALL:
            // switch to ims call state anyway.
            AudioControlManager::HandleEvent(CallStateManager::SWITCH_IMS_CALL);
            break;
        default:
            break;
    }
    CALLMANAGER_DEBUG_LOG("lock release");
}

void RingingState::HandleNoneRingingCall()
{
    CALLMANAGER_DEBUG_LOG("ringing state , handle none ringing calls");
    auto acm = DelayedSingleton<AudioControlManager>::GetInstance();
    if (acm == nullptr) {
        CALLMANAGER_ERR_LOG("audio control manager nullptr");
        return;
    }
    if (acm->IsCurrentRinging()) {
        acm->StopRingtone(); // should stop ringtone while no more ringing calls
    }
    acm->UpdateCallState();
}

void RingingState::StateEnd(const std::string msg)
{
    CALLMANAGER_DEBUG_LOG("ringing state leave , stop ringtone");
    // should not add lock
    auto acm = DelayedSingleton<AudioControlManager>::GetInstance();
    if (acm == nullptr) {
        CALLMANAGER_ERR_LOG("audio control manager nullptr");
        return;
    }
    if (acm->IsCurrentRinging()) {
        acm->StopRingtone(); // should stop ringtone while no more ringing calls
    }
}
} // namespace TelephonyCallManager
} // namespace OHOS