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

#include "holding_state.h"

#include "telephony_log_wrapper.h"

#include "call_state_processor.h"
#include "audio_scene_processor.h"

namespace OHOS {
namespace Telephony {
bool HoldingState::ProcessEvent(int32_t event)
{
    bool result = false;
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case AudioEvent::NEW_ACTIVE_CS_CALL:
            if (DelayedSingleton<CallStateProcessor>::GetInstance()->
                ShouldSwitchState(TelCallState::CALL_STATUS_ACTIVE)) {
                TELEPHONY_LOGI("holding state switch cs call to active state");
                result = DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::SWITCH_CS_CALL_STATE);
            }
            break;
        case AudioEvent::NEW_ACTIVE_IMS_CALL:
            if (DelayedSingleton<CallStateProcessor>::GetInstance()->
                ShouldSwitchState(TelCallState::CALL_STATUS_ACTIVE)) {
                TELEPHONY_LOGI("holding state switch ims call to active state");
                result = DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::SWITCH_IMS_CALL_STATE);
            }
            break;
        case AudioEvent::NEW_INCOMING_CALL:
            // should switch incoming state while only one incoming call exists
            if (DelayedSingleton<CallStateProcessor>::GetInstance()->
                ShouldSwitchState(TelCallState::CALL_STATUS_INCOMING)) {
                TELEPHONY_LOGI("holding state switch call to incoming state");
                result = DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::SWITCH_INCOMING_STATE); // switch to incoming state
            }
            break;
        case AudioEvent::NO_MORE_HOLDING_CALL:
            result = DelayedSingleton<CallStateProcessor>::GetInstance()->UpdateCurrentCallState();
            break;
        case AudioEvent::NEW_ALERTING_CALL:
            if (DelayedSingleton<CallStateProcessor>::GetInstance()->
                ShouldSwitchState(TelCallState::CALL_STATUS_ALERTING)) {
                TELEPHONY_LOGI("holding state switch call to alerting state and ignore dialing state");
                result = DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::SWITCH_ALERTING_STATE);
            }
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("holding state lock release");
    return result;
}
} // namespace Telephony
} // namespace OHOS