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

#include "cs_call_state.h"

#include "telephony_log_wrapper.h"

#include "call_state_processor.h"
#include "audio_control_manager.h"

namespace OHOS {
namespace Telephony {
bool CSCallState::ProcessEvent(int32_t event)
{
    bool result = false;
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case AudioEvent::NEW_INCOMING_CALL:
            result = DelayedSingleton<AudioControlManager>::GetInstance()->PlayWaitingTone();
            break;
        case AudioEvent::NO_MORE_ACTIVE_CALL:
        case AudioEvent::NO_MORE_INCOMING_CALL:
            result = DelayedSingleton<CallStateProcessor>::GetInstance()->UpdateCurrentCallState();
            break;
        case AudioEvent::CALL_TYPE_CS_CHANGE_IMS:
            result = DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(
                AudioEvent::SWITCH_IMS_CALL_STATE);
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("cs call state lock release");
    return result;
}
} // namespace Telephony
} // namespace OHOS