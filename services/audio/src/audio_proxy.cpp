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

#include "audio_proxy.h"

#include "telephony_log_wrapper.h"
#include "bluetooth_call_manager.h"
#include "audio_control_manager.h"
#include "audio_group_manager.h"

namespace OHOS {
namespace Telephony {
const int32_t NO_DEVICE_VALID = 0;
const int32_t RENDERER_FLAG = 0;

AudioProxy::AudioProxy()
    : deviceCallback_(std::make_shared<AudioDeviceChangeCallback>()),
      preferredDeviceCallback_(std::make_shared<AudioPreferDeviceChangeCallback>())
{}

AudioProxy::~AudioProxy() {}

bool AudioProxy::SetAudioScene(AudioStandard::AudioScene audioScene)
{
    return (AudioStandard::AudioSystemManager::GetInstance()->SetAudioScene(audioScene) == TELEPHONY_SUCCESS);
}

int32_t AudioProxy::SetAudioDeviceChangeCallback()
{
    if (deviceCallback_ == nullptr) {
        TELEPHONY_LOGE("device callback nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return AudioStandard::AudioSystemManager::GetInstance()->SetDeviceChangeCallback(
        AudioStandard::DeviceFlag::ALL_DEVICES_FLAG, deviceCallback_);
}

int32_t AudioProxy::UnsetDeviceChangeCallback()
{
    if (deviceCallback_ == nullptr) {
        TELEPHONY_LOGI("device callback nullptr");
        return TELEPHONY_SUCCESS;
    }
    return AudioStandard::AudioSystemManager::GetInstance()->UnsetDeviceChangeCallback();
}

bool AudioProxy::SetBluetoothDevActive()
{
    if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(
        AudioStandard::ActiveDeviceType::BLUETOOTH_SCO)) {
        TELEPHONY_LOGI("bluetooth device is already active");
        return true;
    }
    bool ret = AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
        AudioStandard::ActiveDeviceType::BLUETOOTH_SCO, true);
    if (ret == ERR_NONE) {
        return true;
    }
    return false;
}

bool AudioProxy::SetSpeakerDevActive()
{
    if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(AudioStandard::ActiveDeviceType::SPEAKER)) {
        TELEPHONY_LOGI("speaker device is already active");
        return true;
    }
    bool ret = AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
        AudioStandard::ActiveDeviceType::SPEAKER, true);
    if (ret == ERR_NONE) {
        return true;
    }
    return false;
}

bool AudioProxy::SetWiredHeadsetDevActive()
{
    if (!isWiredHeadsetConnected_) {
        TELEPHONY_LOGE("SetWiredHeadsetDevActive wiredheadset is not connected");
        return false;
    }
    if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(AudioStandard::ActiveDeviceType::SPEAKER)) {
        int32_t ret = AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
            AudioStandard::ActiveDeviceType::SPEAKER, false);
        if (ret != ERR_NONE) {
            TELEPHONY_LOGE("SetWiredHeadsetDevActive speaker close fail");
            return false;
        }
    }
    if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(AudioStandard::ActiveDeviceType::EARPIECE)) {
        int32_t ret = AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
            AudioStandard::ActiveDeviceType::EARPIECE, false);
        if (ret != ERR_NONE) {
            TELEPHONY_LOGE("SetWiredHeadsetDevActive bluetooth sco close fail");
            return false;
        }
    }
    if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(
        AudioStandard::ActiveDeviceType::BLUETOOTH_SCO)) {
        int32_t ret = AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
            AudioStandard::ActiveDeviceType::BLUETOOTH_SCO, false);
        if (ret != ERR_NONE) {
            TELEPHONY_LOGE("SetWiredHeadsetDevActive bluetooth sco close fail");
            return false;
        }
    }
    return true;
}

bool AudioProxy::SetEarpieceDevActive()
{
    if (isWiredHeadsetConnected_) {
        TELEPHONY_LOGE("SetEarpieceDevActive wiredheadset is connected, no need set earpiece dev active");
        return false;
    }
    if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(AudioStandard::ActiveDeviceType::EARPIECE)) {
        TELEPHONY_LOGI("earpiece device is already active");
        return true;
    }
    if (AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
        AudioStandard::ActiveDeviceType::EARPIECE, true) != ERR_NONE) {
        TELEPHONY_LOGE("SetEarpieceDevActive earpiece active fail");
        return false;
    }
    return true;
}

int32_t AudioProxy::GetVolume(AudioStandard::AudioVolumeType audioVolumeType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->GetVolume(audioVolumeType);
}

int32_t AudioProxy::SetVolume(AudioStandard::AudioVolumeType audioVolumeType, int32_t volume)
{
    return AudioStandard::AudioSystemManager::GetInstance()->SetVolume(audioVolumeType, volume);
}

int32_t AudioProxy::SetMaxVolume(AudioStandard::AudioVolumeType audioVolumeType)
{
    int32_t maxVolume = GetMaxVolume(audioVolumeType);
    return AudioStandard::AudioSystemManager::GetInstance()->SetVolume(audioVolumeType, maxVolume);
}

void AudioProxy::SetVolumeAudible()
{
    int32_t volume = GetMaxVolume(AudioStandard::AudioVolumeType::STREAM_VOICE_CALL);
    SetVolume(AudioStandard::AudioVolumeType::STREAM_VOICE_CALL,
        (int32_t)(volume / VOLUME_AUDIBLE_DIVISOR));
}

bool AudioProxy::IsStreamActive(AudioStandard::AudioVolumeType audioVolumeType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->IsStreamActive(audioVolumeType);
}

bool AudioProxy::IsStreamMute(AudioStandard::AudioVolumeType audioVolumeType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->IsStreamMute(audioVolumeType);
}

int32_t AudioProxy::GetMaxVolume(AudioStandard::AudioVolumeType audioVolumeType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->GetMaxVolume(audioVolumeType);
}

int32_t AudioProxy::GetMinVolume(AudioStandard::AudioVolumeType audioVolumeType)
{
    return AudioStandard::AudioSystemManager::GetInstance()->GetMinVolume(audioVolumeType);
}

bool AudioProxy::IsMicrophoneMute()
{
    std::shared_ptr<AudioStandard::AudioGroupManager> AudioGroupManager =
        AudioStandard::AudioSystemManager::GetInstance()->GetGroupManager(AudioStandard::DEFAULT_VOLUME_GROUP_ID);
    return AudioGroupManager->IsMicrophoneMute();
}

bool AudioProxy::SetMicrophoneMute(bool mute)
{
    if (mute == IsMicrophoneMute()) {
        return true;
    }
    std::shared_ptr<AudioStandard::AudioGroupManager> AudioGroupManager =
        AudioStandard::AudioSystemManager::GetInstance()->GetGroupManager(AudioStandard::DEFAULT_VOLUME_GROUP_ID);
    int32_t muteResult = AudioGroupManager->SetMicrophoneMute(mute);
    TELEPHONY_LOGI("set microphone mute result : %{public}d", muteResult);
    return (muteResult == TELEPHONY_SUCCESS);
}

AudioStandard::AudioRingerMode AudioProxy::GetRingerMode() const
{
    return AudioStandard::AudioSystemManager::GetInstance()->GetRingerMode();
}

bool AudioProxy::IsVibrateMode() const
{
    return (AudioStandard::AudioRingerMode::RINGER_MODE_VIBRATE == GetRingerMode());
}

void AudioDeviceChangeCallback::OnDeviceChange(const AudioStandard::DeviceChangeAction &deviceChangeAction)
{
    TELEPHONY_LOGI("AudioDeviceChangeCallback::OnDeviceChange enter");
    for (auto &audioDeviceDescriptor : deviceChangeAction.deviceDescriptors) {
        if (audioDeviceDescriptor->deviceType_ == AudioStandard::DEVICE_TYPE_WIRED_HEADSET ||
            audioDeviceDescriptor->deviceType_ == AudioStandard::DEVICE_TYPE_WIRED_HEADPHONES ||
            audioDeviceDescriptor->deviceType_ == AudioStandard::DEVICE_TYPE_USB_HEADSET) {
            if (deviceChangeAction.type == AudioStandard::CONNECT) {
                TELEPHONY_LOGI("WiredHeadset connected");
                DelayedSingleton<AudioProxy>::GetInstance()->SetWiredHeadsetState(true);
                DelayedSingleton<AudioDeviceManager>::GetInstance()->AddAudioDeviceList(
                    "", AudioDeviceType::DEVICE_WIRED_HEADSET);
            } else {
                TELEPHONY_LOGI("WiredHeadset disConnected");
                DelayedSingleton<AudioProxy>::GetInstance()->SetWiredHeadsetState(false);
                DelayedSingleton<AudioDeviceManager>::GetInstance()->RemoveAudioDeviceList(
                    "", AudioDeviceType::DEVICE_WIRED_HEADSET);
                DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                    AudioEvent::WIRED_HEADSET_DISCONNECTED);
            }
        }
    }
}

int32_t AudioProxy::StartVibrate()
{
#ifdef ABILITY_SENSOR_SUPPORT
    return VibratorManager::GetInstance()->StartVibrate();
#endif
    return TELEPHONY_SUCCESS;
}

int32_t AudioProxy::CancelVibrate()
{
#ifdef ABILITY_SENSOR_SUPPORT
    return VibratorManager::GetInstance()->CancelVibrate();
#endif
    return TELEPHONY_SUCCESS;
}

std::string AudioProxy::GetDefaultRingPath() const
{
    return defaultRingPath_;
}

std::string AudioProxy::GetDefaultTonePath() const
{
    return defaultTonePath_;
}

std::string AudioProxy::GetDefaultDtmfPath() const
{
    return defaultDtmfPath_;
}

void AudioProxy::SetWiredHeadsetState(bool isConnected)
{
    isWiredHeadsetConnected_ = isConnected;
}

int32_t AudioProxy::GetPreferredOutputAudioDevice(AudioDevice &device)
{
    AudioStandard::AudioRendererInfo rendererInfo;
    rendererInfo.contentType = AudioStandard::ContentType::CONTENT_TYPE_UNKNOWN;
    rendererInfo.streamUsage = AudioStandard::StreamUsage::SOURCE_TYPE_VIRTUAL_CAPTURE;
    rendererInfo.rendererFlags = RENDERER_FLAG;
    std::vector<sptr<AudioStandard::AudioDeviceDescriptor>> desc;
    int32_t ret =
        AudioStandard::AudioRoutingManager::GetInstance()->GetPreferredOutputDeviceForRendererInfo(rendererInfo, desc);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetPreferredOutputDeviceForRendererInfo fail");
        return CALL_ERR_AUDIO_OPERATE_FAILED;
    }
    if (desc.size() == NO_DEVICE_VALID) {
        TELEPHONY_LOGE("desc size is zero");
        return CALL_ERR_AUDIO_OPERATE_FAILED;
    }
    switch (desc[0]->deviceType_) {
        case AudioStandard::DEVICE_TYPE_BLUETOOTH_SCO:
            device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
            if (memset_s(&device.address, kMaxAddressLen + 1, 0, kMaxAddressLen + 1) != EOK) {
                TELEPHONY_LOGE("memset_s address fail");
                return TELEPHONY_ERR_MEMSET_FAIL;
            }
            if (memcpy_s(device.address, kMaxAddressLen, desc[0]->macAddress_.c_str(),
                desc[0]->macAddress_.length()) != EOK) {
                TELEPHONY_LOGE("memcpy_s address fail");
                return TELEPHONY_ERR_MEMCPY_FAIL;
            }
            break;
        case AudioStandard::DEVICE_TYPE_EARPIECE:
            device.deviceType = AudioDeviceType::DEVICE_EARPIECE;
            break;
        case AudioStandard::DEVICE_TYPE_SPEAKER:
            device.deviceType = AudioDeviceType::DEVICE_SPEAKER;
            break;
        case AudioStandard::DEVICE_TYPE_WIRED_HEADSET:
        case AudioStandard::DEVICE_TYPE_WIRED_HEADPHONES:
        case AudioStandard::DEVICE_TYPE_USB_HEADSET:
            device.deviceType = AudioDeviceType::DEVICE_WIRED_HEADSET;
            break;
        default:
            break;
    }
    return TELEPHONY_SUCCESS;
}

int32_t AudioProxy::SetAudioPreferDeviceChangeCallback()
{
    if (preferredDeviceCallback_ == nullptr) {
        TELEPHONY_LOGE("preferredDeviceCallback_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    AudioStandard::AudioRendererInfo rendererInfo;
    rendererInfo.contentType = AudioStandard::ContentType::CONTENT_TYPE_SPEECH;
    rendererInfo.streamUsage = AudioStandard::StreamUsage::SOURCE_TYPE_VIRTUAL_CAPTURE;
    rendererInfo.rendererFlags = RENDERER_FLAG;
    int32_t ret = AudioStandard::AudioRoutingManager::GetInstance()->SetPreferredOutputDeviceChangeCallback(
        rendererInfo, preferredDeviceCallback_);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreferredOutputDeviceChangeCallback fail");
        return CALL_ERR_AUDIO_OPERATE_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t AudioProxy::UnsetAudioPreferDeviceChangeCallback()
{
    if (preferredDeviceCallback_ == nullptr) {
        TELEPHONY_LOGE("preferredDeviceCallback_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = AudioStandard::AudioRoutingManager::GetInstance()->UnsetPreferredOutputDeviceChangeCallback();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UnsetPreferredOutputDeviceChangeCallback fail");
        return CALL_ERR_AUDIO_OPERATE_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

void AudioPreferDeviceChangeCallback::OnPreferredOutputDeviceUpdated(
    const std::vector<sptr<AudioStandard::AudioDeviceDescriptor>> &desc)
{
    AudioDevice device;
    if (desc.size() == NO_DEVICE_VALID) {
        TELEPHONY_LOGE("desc size is zero");
        return;
    }
    switch (desc[0]->deviceType_) {
        case AudioStandard::DEVICE_TYPE_BLUETOOTH_SCO:
            device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
            if (memset_s(&device.address, kMaxAddressLen + 1, 0, kMaxAddressLen + 1) != EOK) {
                TELEPHONY_LOGE("memset_s address fail");
                return;
            }
            if (memcpy_s(device.address, kMaxAddressLen, desc[0]->macAddress_.c_str(),
                desc[0]->macAddress_.length()) != EOK) {
                TELEPHONY_LOGE("memcpy_s address fail");
                return;
            }
            DelayedSingleton<AudioDeviceManager>::GetInstance()->SetCurrentAudioDevice(device.deviceType);
            break;
        case AudioStandard::DEVICE_TYPE_EARPIECE:
            device.deviceType = AudioDeviceType::DEVICE_EARPIECE;
            DelayedSingleton<AudioDeviceManager>::GetInstance()->SetCurrentAudioDevice(device.deviceType);
            break;
        case AudioStandard::DEVICE_TYPE_SPEAKER:
            device.deviceType = AudioDeviceType::DEVICE_SPEAKER;
            DelayedSingleton<AudioDeviceManager>::GetInstance()->SetCurrentAudioDevice(device.deviceType);
            break;
        case AudioStandard::DEVICE_TYPE_WIRED_HEADSET:
        case AudioStandard::DEVICE_TYPE_WIRED_HEADPHONES:
        case AudioStandard::DEVICE_TYPE_USB_HEADSET:
            device.deviceType = AudioDeviceType::DEVICE_WIRED_HEADSET;
            DelayedSingleton<AudioDeviceManager>::GetInstance()->AddAudioDeviceList(
                "", AudioDeviceType::DEVICE_WIRED_HEADSET);
            DelayedSingleton<AudioDeviceManager>::GetInstance()->SetCurrentAudioDevice(device.deviceType);
            break;
        default:
            break;
    }
}
} // namespace Telephony
} // namespace OHOS
