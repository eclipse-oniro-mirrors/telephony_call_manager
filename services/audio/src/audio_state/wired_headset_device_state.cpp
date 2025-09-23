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

#include "wired_headset_device_state.h"

#include "telephony_log_wrapper.h"

#include "audio_control_manager.h"

namespace OHOS {
namespace Telephony {
bool WiredHeadsetDeviceState::ProcessEvent(int32_t event)
{
    bool result = false;
    std::lock_guard<ffrt::ffrt> lock(mutex_);
    switch (event) {
        case AudioEvent::WIRED_HEADSET_DISCONNECTED:
            // should reinitialize audio device in order to switch to a proper audio route
            result =
                DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(AudioEvent::INIT_AUDIO_DEVICE);
            break;
        case AudioEvent::BLUETOOTH_SCO_CONNECTED:
            // should switch to bluetooth route while bluetooth connected
            result = DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                AudioEvent::ENABLE_DEVICE_BLUETOOTH);
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("enable wired headset device lock release");
    return result;
}
} // namespace Telephony
} // namespace OHOS