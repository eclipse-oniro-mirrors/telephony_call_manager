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

#include "audio_control_manager.h"

namespace OHOS {
namespace Telephony {
bool HoldingState::ProcessEvent(int32_t event)
{
    bool result = false;
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case AudioEvent::NEW_ACTIVE_CS_CALL:
            if (DelayedSingleton<AudioControlManager>::GetInstance()->IsOnlyOneActiveCall()) {
                result = DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::SWITCH_CS_CALL_STATE);
            }
            break;
        case AudioEvent::NEW_ACTIVE_IMS_CALL:
            if (DelayedSingleton<AudioControlManager>::GetInstance()->IsOnlyOneActiveCall()) {
                result = DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::SWITCH_IMS_CALL_STATE);
            }
            break;
        case AudioEvent::NEW_INCOMING_CALL:
            // should switch incoming state while only one incoming call exists
            if (DelayedSingleton<AudioControlManager>::GetInstance()->IsOnlyOneIncomingCall()) {
                result = DelayedSingleton<CallStateProcessor>::GetInstance()->ProcessEvent(
                    AudioEvent::SWITCH_INCOMING_STATE); // switch to incoming state
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