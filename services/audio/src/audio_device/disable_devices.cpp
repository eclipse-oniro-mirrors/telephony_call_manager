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

#include "disable_devices.h"

#include "call_manager_log.h"

#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
void DisableDevices::StateBegin(const std::string msg)
{
    CALLMANAGER_DEBUG_LOG("disable device enter");
    AudioDeviceManager::SetCurrentAudioDevice(AudioDevice::DEVICE_DISABLE);
}

void DisableDevices::StateProcess(int32_t event)
{
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case AudioDeviceManager::AUDIO_INTERRUPTED:
            // should reinit audio device when audio active
            CALLMANAGER_DEBUG_LOG("current disable device , audio interrupted , reinit audio device");
            AudioControlManager::HandleEvent(AudioDeviceManager::INIT_AUDIO_DEVICE);
            break;
        case AudioDeviceManager::AUDIO_UN_INTERRUPT:
            // do nothing
            break;
        case AudioDeviceManager::AUDIO_RINGING:
            // should reinit audio device when audio active
            CALLMANAGER_DEBUG_LOG("current disable device , audio ringing , reinit audio device");
            AudioControlManager::HandleEvent(AudioDeviceManager::INIT_AUDIO_DEVICE);
            break;
        default:
            break;
    }
    CALLMANAGER_DEBUG_LOG("lock release");
}

void DisableDevices::StateEnd(const std::string msg)
{
    CALLMANAGER_DEBUG_LOG("disable device exit");
}

void DisableDevices::UpdateAudioState() {}
} // namespace TelephonyCallManager
} // namespace OHOS