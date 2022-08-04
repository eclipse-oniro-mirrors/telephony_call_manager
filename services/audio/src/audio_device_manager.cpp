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

#include "telephony_log_wrapper.h"

#include "audio_control_manager.h"
#include "inactive_device_state.h"
#include "bluetooth_device_state.h"
#include "earpiece_device_state.h"
#include "speaker_device_state.h"
#include "wired_headset_device_state.h"
#include "bluetooth_call_manager.h"

namespace OHOS {
namespace Telephony {
bool AudioDeviceManager::isBtScoDevEnable_ = false;
bool AudioDeviceManager::isSpeakerAvailable_ = true; // default available
bool AudioDeviceManager::isEarpieceAvailable_ = false;
bool AudioDeviceManager::isWiredHeadsetConnected_ = false;
bool AudioDeviceManager::isBtScoConnected_ = false;

AudioDeviceManager::AudioDeviceManager()
    : audioDevice_(AudioDevice::DEVICE_UNKNOWN), currentAudioDevice_(nullptr), isAudioActivated_(false)
{}

AudioDeviceManager::~AudioDeviceManager()
{
    memberFuncMap_.clear();
}

void AudioDeviceManager::Init()
{
    memberFuncMap_[AudioEvent::ENABLE_DEVICE_EARPIECE] = &AudioDeviceManager::EnableEarpiece;
    memberFuncMap_[AudioEvent::ENABLE_DEVICE_SPEAKER] = &AudioDeviceManager::EnableSpeaker;
    memberFuncMap_[AudioEvent::ENABLE_DEVICE_WIRED_HEADSET] = &AudioDeviceManager::EnableWiredHeadset;
    memberFuncMap_[AudioEvent::ENABLE_DEVICE_BLUETOOTH] = &AudioDeviceManager::EnableBtSco;
    currentAudioDevice_ = std::make_unique<InactiveDeviceState>();
    if (currentAudioDevice_ == nullptr) {
        TELEPHONY_LOGE("current audio device nullptr");
    }
}

bool AudioDeviceManager::InitAudioDevice()
{
    // when audio deactivate interrupt , reinit
    // when external audio device connection state changed , reinit
    auto device = DelayedSingleton<AudioControlManager>::GetInstance()->GetInitAudioDevice();
    return SwitchDevice(device);
}

bool AudioDeviceManager::ProcessEvent(AudioEvent event)
{
    if (currentAudioDevice_ == nullptr) {
        TELEPHONY_LOGE("current audio device nullptr");
        return false;
    }
    bool result = false;
    switch (event) {
        case AudioEvent::ENABLE_DEVICE_EARPIECE:
        case AudioEvent::ENABLE_DEVICE_SPEAKER:
        case AudioEvent::ENABLE_DEVICE_BLUETOOTH:
        case AudioEvent::ENABLE_DEVICE_WIRED_HEADSET:
            result = SwitchDevice(event);
            break;
        case AudioEvent::AUDIO_ACTIVATED:
        case AudioEvent::AUDIO_RINGING:
            if (!isAudioActivated_) {
                isAudioActivated_ = true;
                std::shared_ptr<BluetoothCallManager> bluetoothCallManager = std::make_shared<BluetoothCallManager>();
                // Gets whether the device can be started from the configuration
                if (bluetoothCallManager->IsBtAvailble()) {
                    return DelayedSingleton<BluetoothConnection>::GetInstance()->ConnectBtSco();
                }
                result = currentAudioDevice_->ProcessEvent(event);
            }
            break;
        case AudioEvent::AUDIO_DEACTIVATED:
            if (isAudioActivated_) {
                isAudioActivated_ = false;
                result = InitAudioDevice();
            }
            break;
        case AudioEvent::BLUETOOTH_SCO_CONNECTED:
            isBtScoConnected_ = true;
            currentAudioDevice_ = std::make_unique<BluetoothDeviceState>();
            if (currentAudioDevice_ == nullptr) {
                return false;
            }
            result = currentAudioDevice_->ProcessEvent(event);
            break;
        case AudioEvent::BLUETOOTH_SCO_DISCONNECTED:
            isBtScoConnected_ = false;
            result = currentAudioDevice_->ProcessEvent(event);
            break;
        case AudioEvent::INIT_AUDIO_DEVICE:
            result = InitAudioDevice();
            break;
        default:
            break;
    }
    return result;
}

bool AudioDeviceManager::SwitchDevice(AudioEvent event)
{
    auto itFunc = memberFuncMap_.find(event);
    if (itFunc != memberFuncMap_.end() && itFunc->second != nullptr) {
        auto memberFunc = itFunc->second;
        return (this->*memberFunc)();
    }
    return false;
}

bool AudioDeviceManager::ConnectBtScoWithAddress(const std::string &bluetoothAddress)
{
    std::shared_ptr<BluetoothCallManager> bluetoothCallManager = std::make_shared<BluetoothCallManager>();
    if (bluetoothCallManager->ConnectBtSco(bluetoothAddress)) {
        return true;
    }
    return false;
}

bool AudioDeviceManager::SwitchDevice(AudioDevice device)
{
    bool result = false;
    std::lock_guard<std::mutex> lock(mutex_);
    switch (device) {
        case AudioDevice::DEVICE_EARPIECE:
            result = EnableEarpiece();
            break;
        case AudioDevice::DEVICE_SPEAKER:
            result = EnableSpeaker();
            break;
        case AudioDevice::DEVICE_WIRED_HEADSET:
            result = EnableWiredHeadset();
            break;
        case AudioDevice::DEVICE_BLUETOOTH_SCO:
            result = EnableBtSco();
            break;
        case AudioDevice::DEVICE_DISABLE:
            result = DisableAll();
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("switch device lock release");
    return result;
}

bool AudioDeviceManager::EnableSpeaker()
{
    if (isSpeakerAvailable_ && DelayedSingleton<AudioProxy>::GetInstance()->SetSpeakerDevActive()) {
        currentAudioDevice_ = std::make_unique<SpeakerDeviceState>();
        if (currentAudioDevice_ == nullptr) {
            TELEPHONY_LOGE("make_unique SpeakerDeviceState failed");
            return false;
        }
        TELEPHONY_LOGI("speaker enabled , current audio device : speaker");
        SetCurrentAudioDevice(AudioDevice::DEVICE_SPEAKER);
        SetSpeakerDevEnable();
        return true;
    }
    TELEPHONY_LOGI("enable speaker device failed");
    return false;
}

bool AudioDeviceManager::EnableEarpiece()
{
    if (isEarpieceAvailable_ && DelayedSingleton<AudioProxy>::GetInstance()->SetEarpieceDevActive()) {
        currentAudioDevice_ = std::make_unique<EarpieceDeviceState>();
        if (currentAudioDevice_ == nullptr) {
            TELEPHONY_LOGE("make_unique EarpieceDeviceState failed");
            return false;
        }
        TELEPHONY_LOGI("earpiece enabled , current audio device : earpiece");
        SetCurrentAudioDevice(AudioDevice::DEVICE_EARPIECE);
        SetEarpieceDevEnable();
        return true;
    }
    TELEPHONY_LOGI("enable earpiece device failed");
    return false;
}

bool AudioDeviceManager::EnableWiredHeadset()
{
    if (isWiredHeadsetConnected_ && DelayedSingleton<AudioProxy>::GetInstance()->SetWiredHeadsetDevActive()) {
        currentAudioDevice_ = std::make_unique<WiredHeadsetDeviceState>();
        if (currentAudioDevice_ == nullptr) {
            TELEPHONY_LOGE("make_unique WiredHeadsetDeviceState failed");
            return false;
        }
        TELEPHONY_LOGI("wired headset enabled , current audio device : wired headset");
        SetCurrentAudioDevice(AudioDevice::DEVICE_WIRED_HEADSET);
        SetWiredHeadsetDevEnable();
        return true;
    }
    TELEPHONY_LOGI("enable wired headset device failed");
    return false;
}

bool AudioDeviceManager::EnableBtSco()
{
    if (isBtScoConnected_ && DelayedSingleton<AudioProxy>::GetInstance()->SetBluetoothDevActive()) {
        currentAudioDevice_ = std::make_unique<BluetoothDeviceState>();
        if (currentAudioDevice_ == nullptr) {
            TELEPHONY_LOGE("make_unique BluetoothDeviceState failed");
            return false;
        }
        TELEPHONY_LOGI("bluetooth sco enabled , current audio device : bluetooth sco");
        SetCurrentAudioDevice(AudioDevice::DEVICE_BLUETOOTH_SCO);
        SetBtScoDevEnable();
        return true;
    }
    TELEPHONY_LOGI("enable bluetooth sco device failed");
    return false;
}

bool AudioDeviceManager::DisableAll()
{
    isBtScoDevEnable_ = false;
    isWiredHeadsetDevEnable_ = false;
    isSpeakerDevEnable_ = false;
    isEarpieceDevEnable_ = false;
    currentAudioDevice_ = std::make_unique<InactiveDeviceState>();
    if (currentAudioDevice_ == nullptr) {
        TELEPHONY_LOGE("make_unique InactiveDeviceState failed");
        return false;
    }
    TELEPHONY_LOGI("current audio device : all audio devices disabled");
    return true;
}

void AudioDeviceManager::SetCurrentAudioDevice(AudioDevice device)
{
    if (audioDevice_ == AudioDevice::DEVICE_BLUETOOTH_SCO && audioDevice_ != device) {
        DelayedSingleton<BluetoothConnection>::GetInstance()->SetBtScoState(SCO_STATE_PENDING);
    } else if (audioDevice_ != AudioDevice::DEVICE_BLUETOOTH_SCO && device == AudioDevice::DEVICE_BLUETOOTH_SCO) {
        DelayedSingleton<BluetoothConnection>::GetInstance()->SetBtScoState(SCO_STATE_CONNECTED);
    }
    audioDevice_ = device;
}

AudioDevice AudioDeviceManager::GetCurrentAudioDevice()
{
    return audioDevice_;
}

void AudioDeviceManager::SetEarpieceDevEnable()
{
    isEarpieceDevEnable_ = true;
    isBtScoDevEnable_ = false;
    isWiredHeadsetDevEnable_ = false;
    isSpeakerDevEnable_ = false;
}

void AudioDeviceManager::SetSpeakerDevEnable()
{
    isSpeakerDevEnable_ = true;
    isWiredHeadsetDevEnable_ = false;
    isBtScoDevEnable_ = false;
    isEarpieceDevEnable_ = false;
}

void AudioDeviceManager::SetBtScoDevEnable()
{
    isBtScoDevEnable_ = true;
    isWiredHeadsetDevEnable_ = false;
    isSpeakerDevEnable_ = false;
    isEarpieceDevEnable_ = false;
}

void AudioDeviceManager::SetWiredHeadsetDevEnable()
{
    isWiredHeadsetDevEnable_ = true;
    isBtScoDevEnable_ = false;
    isSpeakerDevEnable_ = false;
    isEarpieceDevEnable_ = false;
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

bool AudioDeviceManager::IsBtScoConnected()
{
    return isBtScoConnected_;
}

void AudioDeviceManager::SetBtScoAvailable(bool connected)
{
    isBtScoConnected_ = connected;
}

bool AudioDeviceManager::IsWiredHeadsetConnected()
{
    return isWiredHeadsetConnected_;
}

void AudioDeviceManager::SetWiredHeadsetAvailable(bool connected)
{
    isWiredHeadsetConnected_ = connected;
}

bool AudioDeviceManager::IsEarpieceAvailable()
{
    return isEarpieceAvailable_;
}

bool AudioDeviceManager::IsSpeakerAvailable()
{
    return isSpeakerAvailable_;
}

void AudioDeviceManager::SetSpeakerAvailable(bool available)
{
    isSpeakerAvailable_ = available;
}
} // namespace Telephony
} // namespace OHOS