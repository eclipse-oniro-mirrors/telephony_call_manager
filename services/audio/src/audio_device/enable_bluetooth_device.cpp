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

#include "enable_bluetooth_device.h"

#include "call_manager_log.h"

#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
void EnableBluetoothDevice::StateBegin(const std::string msg)
{
    CALLMANAGER_DEBUG_LOG("enable bluetooth device enter");
    // active available
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    audioProxy->SetEarpieceDevActive(false);
    audioProxy->SetSpeakerDevActive(false);
    audioProxy->SetWiredHeadsetDevActive(false);
    audioProxy->SetBluetoothDevActive(true);
    AudioDeviceManager::SetCurrentAudioDevice(AudioDevice::DEVICE_BLUETOOTH_SCO);
    AudioDeviceManager::SetBtScoDevEnable(true);
}

void EnableBluetoothDevice::StateProcess(int32_t event)
{
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case AudioDeviceManager::SWITCH_DEVICE_WIRED_HEADSET:
            AudioControlManager::HandleEvent(AudioDeviceManager::SWITCH_DEVICE_WIRED_HEADSET);
            break;
        case AudioDeviceManager::SWITCH_DEVICE_SPEAKER:
            AudioControlManager::HandleEvent(AudioDeviceManager::SWITCH_DEVICE_SPEAKER);
            break;
        case AudioDeviceManager::SWITCH_DEVICE_EARPIECE:
            AudioControlManager::HandleEvent(AudioDeviceManager::SWITCH_DEVICE_EARPIECE);
            break;
        case AudioDeviceManager::WIRED_HEADSET_AVAILABLE:
            // should switch to wired headset route when wired headset first available
            AudioControlManager::HandleEvent(AudioDeviceManager::SWITCH_DEVICE_WIRED_HEADSET);
            break;
        case AudioDeviceManager::BLUETOOTH_SCO_UNAVAILABLE:
            // should reinit audio device in order to switch to a proper audio device
            CALLMANAGER_DEBUG_LOG("bt sco unavailable , reinit audio device");
            AudioControlManager::HandleEvent(AudioDeviceManager::INIT_AUDIO_DEVICE);
            break;
        case AudioDeviceManager::AUDIO_UN_INTERRUPT:
            AudioControlManager::HandleEvent(AudioDeviceManager::SWITCH_INACTIVE);
            break;
        default:
            break;
    }
    CALLMANAGER_DEBUG_LOG("lock release");
}

void EnableBluetoothDevice::StateEnd(const std::string msg)
{
    // should not add lock
    AudioDeviceManager::SetBtScoDevEnable(false);
    CALLMANAGER_DEBUG_LOG("enable bluetooth device leave , bt sco device disable");
}

void EnableBluetoothDevice::UpdateAudioState() {}
} // namespace TelephonyCallManager
} // namespace OHOS