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

#include "enable_wired_headset_device.h"

#include "call_manager_log.h"

#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
void EnableWiredHeadsetDevice::StateBegin(const std::string msg)
{
    CALLMANAGER_DEBUG_LOG("enable wired headset device enter");
    // active available
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    audioProxy->SetBluetoothDevActive(false);
    audioProxy->SetEarpieceDevActive(false);
    audioProxy->SetSpeakerDevActive(false);
    audioProxy->SetWiredHeadsetDevActive(true);
    AudioDeviceManager::SetCurrentAudioDevice(AudioDevice::DEVICE_WIRED_HEADSET);
    AudioDeviceManager::SetWiredHeadsetDevEnable(true);
}

void EnableWiredHeadsetDevice::StateProcess(int32_t event)
{
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case AudioDeviceManager::SWITCH_DEVICE_BLUETOOTH:
            AudioControlManager::HandleEvent(AudioDeviceManager::SWITCH_DEVICE_BLUETOOTH);
            break;
        case AudioDeviceManager::SWITCH_DEVICE_SPEAKER:
            AudioControlManager::HandleEvent(AudioDeviceManager::SWITCH_DEVICE_BLUETOOTH);
            break;
        case AudioDeviceManager::SWITCH_DEVICE_EARPIECE:
            AudioControlManager::HandleEvent(AudioDeviceManager::SWITCH_DEVICE_EARPIECE);
            break;
        case AudioDeviceManager::WIRED_HEADSET_UNAVAILABLE:
            // should reinit audio device in order to switch to a proper audio device
            CALLMANAGER_DEBUG_LOG("wired headset unavailable , reinit audio device");
            AudioControlManager::HandleEvent(AudioDeviceManager::INIT_AUDIO_DEVICE);
            break;
        case AudioDeviceManager::BLUETOOTH_SCO_AVAILABLE:
            // should switch to bluetooth route when bluetooth first available
            AudioControlManager::HandleEvent(AudioDeviceManager::SWITCH_DEVICE_BLUETOOTH);
            break;
        case AudioDeviceManager::AUDIO_UN_INTERRUPT:
            AudioControlManager::HandleEvent(AudioDeviceManager::SWITCH_INACTIVE);
            break;
        default:
            break;
    }
    CALLMANAGER_DEBUG_LOG("lock release");
}

void EnableWiredHeadsetDevice::StateEnd(const std::string msg)
{
    // should not add lock
    AudioDeviceManager::SetWiredHeadsetDevEnable(false);
    CALLMANAGER_DEBUG_LOG("enable wired headset device leave and wired headset device disable");
}

void EnableWiredHeadsetDevice::UpdateAudioState() {}
} // namespace TelephonyCallManager
} // namespace OHOS