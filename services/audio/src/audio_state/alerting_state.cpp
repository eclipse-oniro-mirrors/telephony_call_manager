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

#include "alerting_state.h"

#include "telephony_log_wrapper.h"

#include "call_state_processor.h"
#include "audio_scene_processor.h"
#include "audio_control_manager.h"

namespace OHOS {
namespace Telephony {
bool AlertingState::ProcessEvent(int32_t event)
{
    bool result = false;
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto callStateProcessor = DelayedSingleton<CallStateProcessor>::GetInstance();
    auto audioSceneProcessor = DelayedSingleton<AudioSceneProcessor>::GetInstance();
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    switch (event) {
        case AudioEvent::NO_MORE_ALERTING_CALL:
            if (callStateProcessor != nullptr) {
                result = callStateProcessor->UpdateCurrentCallState();
            }
            break;
        case AudioEvent::NEW_ACTIVE_CS_CALL:
            if (callStateProcessor != nullptr &&
                callStateProcessor->ShouldSwitchState(TelCallState::CALL_STATUS_ACTIVE)) {
                TELEPHONY_LOGI("alerting state switch cs call to active state");
                if (audioSceneProcessor != nullptr) {
                    result = audioSceneProcessor->ProcessEvent(AudioEvent::SWITCH_CS_CALL_STATE);
                }
            }
            break;
        case AudioEvent::NEW_ACTIVE_IMS_CALL:
            if (callStateProcessor != nullptr &&
                callStateProcessor->ShouldSwitchState(TelCallState::CALL_STATUS_ACTIVE)) {
                TELEPHONY_LOGI("alerting state switch ims call to active state");
                if (audioSceneProcessor != nullptr) {
                    result = audioSceneProcessor->ProcessEvent(AudioEvent::SWITCH_IMS_CALL_STATE);
                }
            }
            break;
        case AudioEvent::NEW_ALERTING_CALL:
            if (callStateProcessor != nullptr &&
                callStateProcessor->ShouldSwitchState(TelCallState::CALL_STATUS_ALERTING)) {
                TELEPHONY_LOGI("alerting state: check is should play ringback tone.");
                if (audioSceneProcessor != nullptr) {
                    result = audioSceneProcessor->ProcessEvent(AudioEvent::SWITCH_ALERTING_STATE);
                }
            }
            break;
        case AudioEvent::NEW_INCOMING_CALL:
            if (audioControlManager != nullptr) {
                result = audioControlManager->PlayWaitingTone();
            }
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("alerting state lock release");
    return result;
}
} // namespace Telephony
} // namespace OHOS