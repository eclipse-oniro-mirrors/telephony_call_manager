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

#include "audio_event.h"

#include "call_manager_log.h"

#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
AudioEvent::AudioEvent() : callStateManager_(nullptr), audioDeviceManager_(nullptr) {}

AudioEvent::~AudioEvent() {}

void AudioEvent::Init()
{
    CALLMANAGER_INFO_LOG("audio event init");
    callStateManager_ = std::make_unique<CallStateManager>();
    if (callStateManager_ == nullptr) {
        CALLMANAGER_ERR_LOG("call state manager nullptr");
        return;
    }
    audioDeviceManager_ = std::make_unique<AudioDeviceManager>();
    if (audioDeviceManager_ == nullptr) {
        CALLMANAGER_ERR_LOG("audio device manager nullptr");
        return;
    }
    callStateManager_->Init();
    audioDeviceManager_->Init();
}

void AudioEvent::HandleEvent(int32_t event)
{
    CALLMANAGER_INFO_LOG("audio event process event : %{public}d", event);
    switch (event) {
        case CallStateManager::SWITCH_CS_CALL:
        case CallStateManager::SWITCH_IMS_CALL:
        case CallStateManager::SWITCH_RINGING:
        case CallStateManager::SWITCH_HOLDING:
        case CallStateManager::ABANDON_AUDIO:
        case CallStateManager::COMING_CS_CALL:
        case CallStateManager::COMING_IMS_CALL:
        case CallStateManager::COMING_RINGING_CALL:
        case CallStateManager::NONE_ALERTING_OR_ACTIVE_CALLS:
        case CallStateManager::NONE_RINGING_CALLS:
            HandleCallStateEvent(event);
            break;
        case AudioDeviceManager::SWITCH_DEVICE_BLUETOOTH:
        case AudioDeviceManager::SWITCH_DEVICE_WIRED_HEADSET:
        case AudioDeviceManager::SWITCH_DEVICE_SPEAKER:
        case AudioDeviceManager::SWITCH_DEVICE_EARPIECE:
        case AudioDeviceManager::SWITCH_INACTIVE:
        case AudioDeviceManager::WIRED_HEADSET_AVAILABLE:
        case AudioDeviceManager::WIRED_HEADSET_UNAVAILABLE:
        case AudioDeviceManager::BLUETOOTH_SCO_AVAILABLE:
        case AudioDeviceManager::BLUETOOTH_SCO_UNAVAILABLE:
        case AudioDeviceManager::AUDIO_INTERRUPTED:
        case AudioDeviceManager::AUDIO_UN_INTERRUPT:
        case AudioDeviceManager::AUDIO_RINGING:
        case AudioDeviceManager::INIT_AUDIO_DEVICE:
            HandleAudioDeviceEvent(event);
            break;
        default:
            CALLMANAGER_ERR_LOG("invalid audio event");
            break;
    }
}

void AudioEvent::HandleCallStateEvent(int32_t event)
{
    if (callStateManager_ == nullptr) {
        CALLMANAGER_ERR_LOG("call state manager nullptr");
        return;
    }
    callStateManager_->HandleEvent(event);
}

void AudioEvent::HandleAudioDeviceEvent(int32_t event)
{
    if (audioDeviceManager_ == nullptr) {
        CALLMANAGER_ERR_LOG("audio device manager nullptr");
        return;
    }
    audioDeviceManager_->HandleEvent(event);
}
} // namespace TelephonyCallManager
} // namespace OHOS