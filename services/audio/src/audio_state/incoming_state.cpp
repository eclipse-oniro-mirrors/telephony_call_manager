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

#include "incoming_state.h"

#include "telephony_log_wrapper.h"

#include "call_state_processor.h"
#include "audio_scene_processor.h"

namespace OHOS {
namespace Telephony {
bool IncomingState::ProcessEvent(int32_t event)
{
    bool result = false;
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case AudioEvent::NO_MORE_INCOMING_CALL:
            result = DelayedSingleton<CallStateProcessor>::GetInstance()->UpdateCurrentCallState();
            break;
        case AudioEvent::NEW_ACTIVE_CS_CALL:
            // switch to cs call state anyway.
            if (DelayedSingleton<CallStateProcessor>::GetInstance()->
                ShouldSwitchState(TelCallState::CALL_STATUS_ACTIVE)) {
                TELEPHONY_LOGI("incoming state switch cs call to active state");
                result = DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::SWITCH_CS_CALL_STATE);
            }
            break;
        case AudioEvent::NEW_ACTIVE_IMS_CALL:
            if (DelayedSingleton<CallStateProcessor>::GetInstance()->
                ShouldSwitchState(TelCallState::CALL_STATUS_ACTIVE)) {
                // switch to ims call state anyway.
                TELEPHONY_LOGI("incoming state switch ims call to active state");
                result = DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::SWITCH_IMS_CALL_STATE);
            }
            break;
        case AudioEvent::NEW_INCOMING_CALL:
            TELEPHONY_LOGI("incoming state switch incoming state");
            result = DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(
                AudioEvent::SWITCH_INCOMING_STATE);
        default:
            break;
    }
    TELEPHONY_LOGI("incoming state lock release");
    return result;
}
} // namespace Telephony
} // namespace OHOS
