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

#include "ims_call_state.h"

#include "call_manager_log.h"

#include "audio_proxy.h"
#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
void IMSCallState::StateBegin(const std::string msg)
{
    CALLMANAGER_DEBUG_LOG("ims call state enter");
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return;
    }
    if (!AudioControlManager::IsAudioActive()) {
        audioProxy->ActivateAudioInterrupt();
    }
    audioProxy->UpdateCallState(AudioCallState::IN_VOIP);
    CallStateManager::SetCurrentCallState(AudioCallState::IN_VOIP);
    AudioControlManager::SetAudioInterruptState(AudioInterruptState::IN_INTERRUPT);
}

void IMSCallState::StateProcess(int32_t event)
{
    auto acm = DelayedSingleton<AudioControlManager>::GetInstance();
    if (acm == nullptr) {
        CALLMANAGER_ERR_LOG("audio control manager nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case CallStateManager::NONE_ALERTING_OR_ACTIVE_CALLS:
            HandleNoneAlertingOrActiveCall();
            break;
        case CallStateManager::NONE_RINGING_CALLS:
            if (acm->IsTonePlaying()) {
                acm->StopCallTone();
            }
            break;
        case CallStateManager::COMING_RINGING_CALL:
            acm->PlayWaitingTone(); // should play call waiting tone while in ims call state
            break;
        case CallStateManager::VIDEO_STATE_CHANGED:
            if (!acm->IsCurrentCallIms()) {
                acm->HandleEvent(CallStateManager::SWITCH_CS_CALL);
            }
            break;
        default:
            break;
    }
    CALLMANAGER_DEBUG_LOG("lock release");
}

void IMSCallState::StateEnd(const std::string msg)
{
    CALLMANAGER_DEBUG_LOG("ims call state leave");
}

void IMSCallState::HandleNoneAlertingOrActiveCall()
{
    CALLMANAGER_DEBUG_LOG("ims call state , handle none alerting or active calls");
    auto acm = DelayedSingleton<AudioControlManager>::GetInstance();
    if (acm == nullptr) {
        CALLMANAGER_ERR_LOG("audio control manager nullptr");
        return;
    }
    acm->UpdateCallState();
}
} // namespace TelephonyCallManager
} // namespace OHOS