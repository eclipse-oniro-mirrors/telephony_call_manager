/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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

#ifdef SUPPORT_VIBRATOR
#include "vibrator_agent.h"
#endif

#include "telephony_log_wrapper.h"
#include "call_control_manager.h"
#include "bluetooth_call_manager.h"
#include "audio_control_manager.h"
#include "audio_group_manager.h"
#include "distributed_call_manager.h"

namespace OHOS {
namespace Telephony {
#ifdef SUPPORT_VIBRATOR
const std::unordered_map<VibrationType, VibratorUsage> VIBRATOR_USAGE_MAP = {
    {VibrationType::VIBRATION_RINGTONE, USAGE_RING},
};

const std::unordered_map<VibrationType, int32_t> LOOP_COUNT_MAP = {
    // Default loop count. Ringtone need be repeated.
    {VibrationType::VIBRATION_RINGTONE, 10},
};

const std::unordered_map<VibrationType, std::string> EFFECT_ID_MAP = {
    // Default effectId
    {VibrationType::VIBRATION_RINGTONE, "haptic.ringtone.Dream_It_Possible"},
};
#endif

const int32_t NO_DEVICE_VALID = 0;
const int32_t RENDERER_FLAG = 0;

AudioProxy::AudioProxy()
    : deviceCallback_(std::make_shared<AudioDeviceChangeCallback>()),
      preferredDeviceCallback_(std::make_shared<AudioPreferDeviceChangeCallback>()),
      audioMicStateChangeCallback_(std::make_shared<AudioMicStateChangeCallback>())
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
    if (AudioStandard::AudioSystemManager::GetInstance()->
        IsDeviceActive(AudioStandard::ActiveDeviceType::USB_HEADSET)) {
        TELEPHONY_LOGI("wired headset device is already active");
        return true;
    }
    bool ret = AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
        AudioStandard::ActiveDeviceType::USB_HEADSET, true);
    if (ret == ERR_NONE) {
        return true;
    }
    return false;
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

int32_t AudioProxy::StartVibrator()
{
    VibrationType type = VibrationType::VIBRATION_RINGTONE;
    TELEPHONY_LOGE("StartVibrator: for vibration type %{public}d", type);
    int32_t result = TELEPHONY_SUCCESS;
#ifdef SUPPORT_VIBRATOR
    bool setUsageRet = Sensors::SetUsage(VIBRATOR_USAGE_MAP.at(type));
    bool setLoopRet = Sensors::SetLoopCount(LOOP_COUNT_MAP.at(type));
    result = Sensors::StartVibrator(EFFECT_ID_MAP.at(type).c_str());
    TELEPHONY_LOGE("StartVibrator: setUsageRet %{public}d, setLoopRet %{public}d, startRet %{public}d",
        setUsageRet, setLoopRet, result);
#endif
    return result;
}

int32_t AudioProxy::StopVibrator()
{
    int32_t result = TELEPHONY_SUCCESS;
#ifdef SUPPORT_VIBRATOR
    result = Sensors::Cancel();
    TELEPHONY_LOGE("StopVibrator: %{public}d", result);
#endif
    return result;
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
    std::shared_ptr<AudioStandard::AudioGroupManager> audioGroupManager =
        AudioStandard::AudioSystemManager::GetInstance()->GetGroupManager(AudioStandard::DEFAULT_VOLUME_GROUP_ID);
    if (audioGroupManager == nullptr) {
        TELEPHONY_LOGE("IsMicrophoneMute fail, audioGroupManager is nullptr");
        return false;
    }
    return audioGroupManager->IsMicrophoneMute();
}

bool AudioProxy::SetMicrophoneMute(bool mute)
{
    if (mute == IsMicrophoneMute()) {
        return true;
    }
    std::shared_ptr<AudioStandard::AudioGroupManager> audioGroupManager =
        AudioStandard::AudioSystemManager::GetInstance()->GetGroupManager(AudioStandard::DEFAULT_VOLUME_GROUP_ID);
    if (audioGroupManager == nullptr) {
        TELEPHONY_LOGE("SetMicrophoneMute fail, audioGroupManager is nullptr");
        return false;
    }
    int32_t muteResult = audioGroupManager->SetMicrophoneMute(mute);
    TELEPHONY_LOGI("set microphone mute result : %{public}d, %{public}d ", muteResult, mute);
    return (muteResult == TELEPHONY_SUCCESS);
}

AudioStandard::AudioRingerMode AudioProxy::GetRingerMode() const
{
    std::shared_ptr<AudioStandard::AudioGroupManager> audioGroupManager =
        AudioStandard::AudioSystemManager::GetInstance()->GetGroupManager(AudioStandard::DEFAULT_VOLUME_GROUP_ID);
    if (audioGroupManager == nullptr) {
        TELEPHONY_LOGE("GetRingerMode fail, audioGroupManager is nullptr");
        return AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL;
    }
    return audioGroupManager->GetRingerMode();
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
                    "", AudioDeviceType::DEVICE_WIRED_HEADSET, "");
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
    rendererInfo.streamUsage = AudioStandard::StreamUsage::STREAM_USAGE_VOICE_MODEM_COMMUNICATION;
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
            if (memset_s(&device.address, kMaxAddressLen + 1, 0, kMaxAddressLen + 1) != EOK ||
                memset_s(&device.deviceName, kMaxDeviceNameLen + 1, 0, kMaxDeviceNameLen + 1) != EOK) {
                TELEPHONY_LOGE("memset_s address fail");
                return TELEPHONY_ERR_MEMSET_FAIL;
            }
            if (memcpy_s(device.address, kMaxAddressLen, desc[0]->macAddress_.c_str(),
                desc[0]->macAddress_.length()) != EOK ||
                memcpy_s(&device.deviceName, kMaxDeviceNameLen, desc[0]->deviceName_.c_str(),
                desc[0]->deviceName_.length()) != EOK) {
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
    rendererInfo.streamUsage = AudioStandard::StreamUsage::STREAM_USAGE_VOICE_MODEM_COMMUNICATION;
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

    TELEPHONY_LOGI("OnPreferredOutputDeviceUpdated type: %{public}d", desc[0]->deviceType_);

    size_t size = desc.size();
    TELEPHONY_LOGI("desc size is: %{public}zu", size);
    std::string deviceId = DelayedSingleton<DistributedCallManager>::GetInstance()->GetConnectedDCallDeviceId();
    for (auto iter = desc.begin(); iter != desc.end(); iter++) {
        std::string networkId = (*iter)->networkId_;
        if (networkId.empty() || deviceId != networkId) {
            continue;
        }
        DelayedSingleton<AudioDeviceManager>::GetInstance()->SetCurrentAudioDevice(
            AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE);
        TELEPHONY_LOGI("switch to distributed audio device");
        return;
    }

    switch (desc[0]->deviceType_) {
        case AudioStandard::DEVICE_TYPE_BLUETOOTH_SCO:
            device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
            if (memset_s(&device.address, kMaxAddressLen + 1, 0, kMaxAddressLen + 1) != EOK ||
                memset_s(&device.deviceName, kMaxDeviceNameLen + 1, 0, kMaxDeviceNameLen + 1) != EOK) {
                TELEPHONY_LOGE("memset_s address fail");
                return;
            }
            if (memcpy_s(device.address, kMaxAddressLen, desc[0]->macAddress_.c_str(),
                desc[0]->macAddress_.length()) != EOK ||
                memcpy_s(device.deviceName, kMaxDeviceNameLen, desc[0]->deviceName_.c_str(),
                desc[0]->deviceName_.length()) != EOK) {
                TELEPHONY_LOGE("memcpy_s address fail");
                return;
            }
            DelayedSingleton<AudioDeviceManager>::GetInstance()->SetCurrentAudioDevice(device);
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
                "", AudioDeviceType::DEVICE_WIRED_HEADSET, "");
            DelayedSingleton<AudioDeviceManager>::GetInstance()->SetCurrentAudioDevice(device.deviceType);
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("OnPreferredOutputDeviceUpdated, type: %{public}d", static_cast<int32_t>(desc[0]->deviceType_));
    if (desc[0]->deviceType_ != AudioStandard::DEVICE_TYPE_SPEAKER) {
        DelayedSingleton<AudioControlManager>::GetInstance()->UpdateDeviceTypeForCrs();
    }
}

int32_t AudioProxy::SetAudioMicStateChangeCallback()
{
    if (audioMicStateChangeCallback_ == nullptr) {
        TELEPHONY_LOGE("audioMicStateChangeCallback_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::shared_ptr<AudioStandard::AudioGroupManager> audioGroupManager =
        AudioStandard::AudioSystemManager::GetInstance()->GetGroupManager(AudioStandard::DEFAULT_VOLUME_GROUP_ID);
    if (audioGroupManager == nullptr) {
        TELEPHONY_LOGE("SetAudioMicStateChangeCallback fail, audioGroupManager is nullptr");
        return false;
    }
    int32_t ret = audioGroupManager->SetMicStateChangeCallback(audioMicStateChangeCallback_);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreferredOutputDeviceChangeCallback fail");
        return CALL_ERR_AUDIO_OPERATE_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t AudioProxy::UnsetAudioMicStateChangeCallback()
{
    if (audioMicStateChangeCallback_ == nullptr) {
        TELEPHONY_LOGE("audioMicStateChangeCallback_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::shared_ptr<AudioStandard::AudioGroupManager> audioGroupManager =
        AudioStandard::AudioSystemManager::GetInstance()->GetGroupManager(AudioStandard::DEFAULT_VOLUME_GROUP_ID);
    if (audioGroupManager == nullptr) {
        TELEPHONY_LOGE("UnsetAudioMicStateChangeCallback fail, audioGroupManager is nullptr");
        return false;
    }
    int32_t ret = audioGroupManager->UnsetMicStateChangeCallback(audioMicStateChangeCallback_);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UnsetAudioMicStateChangeCallback fail");
        return CALL_ERR_AUDIO_OPERATE_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

void AudioMicStateChangeCallback::OnMicStateUpdated(
    const AudioStandard::MicStateChangeEvent &micStateChangeEvent)
{
    std::shared_ptr<AudioStandard::AudioGroupManager> audioGroupManager =
        AudioStandard::AudioSystemManager::GetInstance()->GetGroupManager(AudioStandard::DEFAULT_VOLUME_GROUP_ID);
    if (audioGroupManager == nullptr) {
        TELEPHONY_LOGE("OnMicStateUpdated fail, audioGroupManager is nullptr");
        return;
    }
    DelayedSingleton<CallControlManager>::GetInstance()->SetMuted(audioGroupManager->IsMicrophoneMute());
}

} // namespace Telephony
} // namespace OHOS
