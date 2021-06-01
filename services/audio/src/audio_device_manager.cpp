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

#include "audio_device_manager.h"

#include "call_manager_log.h"

#include "audio_control_manager.h"
#include "enable_bluetooth_device.h"
#include "enable_earpiece_device.h"
#include "enable_speaker_device.h"
#include "enable_wired_headset_device.h"
#include "disable_devices.h"

namespace OHOS {
namespace TelephonyCallManager {
AudioDevice AudioDeviceManager::currentAudioDevice_ = AudioDevice::DEVICE_UNKNOWN;
bool AudioDeviceManager::isBtScoDevEnable_ = false;
bool AudioDeviceManager::isWiredHeadsetDevEnable_ = false;
bool AudioDeviceManager::isSpeakerDevEnable_ = false;
bool AudioDeviceManager::isEarpieceDevEnable_ = false;
bool AudioDeviceManager::isEarpieceAvailable_ = true; // default available
bool AudioDeviceManager::isSpeakerAvailable_ = true; // default available
bool AudioDeviceManager::isWiredHeadsetAvailable_ = false;
bool AudioDeviceManager::isBtScoAvailable_ = false;

AudioDeviceManager::AudioDeviceManager() : audioState_(nullptr), isEnable_(false) {}

AudioDeviceManager::~AudioDeviceManager() {}

void AudioDeviceManager::Init()
{
    CALLMANAGER_INFO_LOG("audio device manager init");
    auto device = DelayedSingleton<AudioControlManager>::GetInstance()->GetInitAudioDevice();
    SwitchDevice(device); // audio device init
}

void AudioDeviceManager::InitAudioDevice()
{
    CALLMANAGER_INFO_LOG("audio device manager reinit");
    // when audio deactivate interrupt , reinit
    // when audio external device available state changed , reinit
    auto device = DelayedSingleton<AudioControlManager>::GetInstance()->GetInitAudioDevice();
    SwitchDevice(device); // audio device init
}

void AudioDeviceManager::HandleEvent(int32_t event)
{
    CALLMANAGER_INFO_LOG("audio device manager process event %{public}d : ", event);
    if (audioState_ == nullptr) {
        CALLMANAGER_INFO_LOG("audio state nullptr");
        return;
    }
    switch (event) {
        case AudioDeviceManager::SWITCH_DEVICE_BLUETOOTH:
        case AudioDeviceManager::SWITCH_DEVICE_WIRED_HEADSET:
        case AudioDeviceManager::SWITCH_DEVICE_SPEAKER:
        case AudioDeviceManager::SWITCH_DEVICE_EARPIECE:
        case AudioDeviceManager::SWITCH_INACTIVE:
            HandleEnterAudioDeviceEvent(event);
            break;
        case AudioDeviceManager::WIRED_HEADSET_AVAILABLE:
        case AudioDeviceManager::WIRED_HEADSET_UNAVAILABLE:
        case AudioDeviceManager::BLUETOOTH_SCO_AVAILABLE:
        case AudioDeviceManager::BLUETOOTH_SCO_UNAVAILABLE:
            audioState_->StateProcess(event);
            break;
        case AudioDeviceManager::AUDIO_INTERRUPTED:
        case AudioDeviceManager::AUDIO_RINGING:
            isEnable_ = true;
            audioState_->StateProcess(event);
            break;
        case AudioDeviceManager::AUDIO_UN_INTERRUPT:
            isEnable_ = false;
            audioState_->StateProcess(AudioDeviceManager::AUDIO_UN_INTERRUPT);
            break;
        case AudioDeviceManager::INIT_AUDIO_DEVICE:
            InitAudioDevice();
            break;
        default:
            break;
    }
}

void AudioDeviceManager::HandleEnterAudioDeviceEvent(int32_t event)
{
    switch (event) {
        case AudioDeviceManager::SWITCH_DEVICE_BLUETOOTH:
            SwitchDevice(AudioDevice::DEVICE_BLUETOOTH_SCO);
            break;
        case AudioDeviceManager::SWITCH_DEVICE_WIRED_HEADSET:
            SwitchDevice(AudioDevice::DEVICE_WIRED_HEADSET);
            break;
        case AudioDeviceManager::SWITCH_DEVICE_SPEAKER:
            SwitchDevice(AudioDevice::DEVICE_SPEAKER);
            break;
        case AudioDeviceManager::SWITCH_DEVICE_EARPIECE:
            SwitchDevice(AudioDevice::DEVICE_EARPIECE);
            break;
        case AudioDeviceManager::SWITCH_INACTIVE:
            SwitchDevice(AudioDevice::DEVICE_DISABLE);
            break;
        default:
            break;
    }
}

void AudioDeviceManager::SwitchDevice(AudioDevice device)
{
    if (currentAudioDevice_ == device) {
        CALLMANAGER_INFO_LOG("no need to switch device");
        return;
    }
    switch (device) {
        case AudioDevice::DEVICE_BLUETOOTH_SCO:
            if (isBtScoAvailable_) {
                DoSwitch(AudioDevice::DEVICE_BLUETOOTH_SCO);
            } else {
                CALLMANAGER_INFO_LOG("device bt sco unavailable");
            }
            break;
        case AudioDevice::DEVICE_WIRED_HEADSET:
            if (isWiredHeadsetAvailable_) {
                DoSwitch(AudioDevice::DEVICE_WIRED_HEADSET);
            } else {
                CALLMANAGER_INFO_LOG("device wired headset unavailable");
            }
            break;
        case AudioDevice::DEVICE_SPEAKER:
            if (isSpeakerAvailable_) {
                DoSwitch(AudioDevice::DEVICE_SPEAKER);
            } else {
                CALLMANAGER_INFO_LOG("device speaker unavailable");
            }
            break;
        case AudioDevice::DEVICE_EARPIECE:
            if (isEarpieceAvailable_) {
                DoSwitch(AudioDevice::DEVICE_EARPIECE);
            } else {
                CALLMANAGER_INFO_LOG("device earpiece unavailable");
            }
            break;
        case AudioDevice::DEVICE_DISABLE:
            DoSwitch(AudioDevice::DEVICE_DISABLE);
            break;
        default:
            break;
    }
}

void AudioDeviceManager::DoSwitch(AudioDevice device)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (audioState_ != nullptr) {
        audioState_->StateEnd(""); // leave pre audio device
    }
    switch (device) {
        case AudioDevice::DEVICE_BLUETOOTH_SCO:
            CALLMANAGER_INFO_LOG("switch device : bluetooth");
            audioState_ = std::make_unique<EnableBluetoothDevice>();
            break;
        case AudioDevice::DEVICE_WIRED_HEADSET:
            CALLMANAGER_INFO_LOG("switch device : wired headset");
            audioState_ = std::make_unique<EnableWiredHeadsetDevice>();
            break;
        case AudioDevice::DEVICE_SPEAKER:
            CALLMANAGER_INFO_LOG("switch device : speaker");
            audioState_ = std::make_unique<EnableSpeakerDevice>();
            break;
        case AudioDevice::DEVICE_EARPIECE:
            CALLMANAGER_INFO_LOG("switch device : earpiece");
            audioState_ = std::make_unique<EnableEarpieceDevice>();
            break;
        case AudioDevice::DEVICE_DISABLE:
            CALLMANAGER_INFO_LOG("switch device : disable");
            audioState_ = std::make_unique<DisableDevices>();
            break;
        default:
            break;
    }
    if (audioState_ != nullptr) {
        audioState_->StateBegin(""); // switch a  new audio device
    }
}

template<typename T>
void AudioDeviceManager::DoSwitch(const std::unique_ptr<T> &state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (audioState_ != nullptr) {
        audioState_->StateEnd(""); // leave pre audio device
    }
    audioState_ = state;
    if (audioState_ != nullptr) {
        audioState_->StateBegin(""); // switch a new audio device
    }
}

/**
 * mute or unmute microphone
 */
void AudioDeviceManager::SetMuteOn(bool on)
{
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return;
    }
    audioProxy->SetMicrophoneMute(on);
}

void AudioDeviceManager::SetCurrentAudioDevice(AudioDevice device)
{
    currentAudioDevice_ = device;
}

AudioDevice AudioDeviceManager::GetCurrentAudioDevice()
{
    return currentAudioDevice_;
}

void AudioDeviceManager::SetEarpieceDevEnable(bool enable)
{
    isEarpieceDevEnable_ = enable;
}

void AudioDeviceManager::SetBtScoDevEnable(bool enable)
{
    isBtScoDevEnable_ = enable;
}

void AudioDeviceManager::SetWiredHeadsetDevEnable(bool enable)
{
    isWiredHeadsetDevEnable_ = enable;
}

void AudioDeviceManager::SetSpeakerDevEnable(bool enable)
{
    isSpeakerDevEnable_ = enable;
}

bool AudioDeviceManager::IsEarpieceDevEnable()
{
    return isEarpieceDevEnable_;
}

bool AudioDeviceManager::IsWiredHeadsetDevEnable()
{
    return isWiredHeadsetDevEnable_;
}

bool AudioDeviceManager::IsSpeakerDevEnable()
{
    return isSpeakerDevEnable_;
}

bool AudioDeviceManager::IsBtScoDevEnable()
{
    return isBtScoDevEnable_;
}

bool AudioDeviceManager::IsBtScoAvailable()
{
    return isBtScoAvailable_;
}

void AudioDeviceManager::SetBtScoAvailable(bool available)
{
    isBtScoAvailable_ = available;
}

bool AudioDeviceManager::IsWiredHeadsetAvailable()
{
    return isWiredHeadsetAvailable_;
}

void AudioDeviceManager::SetWiredHeadsetAvailable(bool available)
{
    isWiredHeadsetAvailable_ = available;
}

bool AudioDeviceManager::IsSpeakerAvailable()
{
    return isSpeakerAvailable_;
}

void AudioDeviceManager::SetSpeakerAvailable(bool available)
{
    isSpeakerAvailable_ = available;
}
} // namespace TelephonyCallManager
} // namespace OHOS