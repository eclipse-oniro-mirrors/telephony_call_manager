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

#include "audio_device_manager.h"

#include "audio_control_manager.h"
#include "bluetooth_call_manager.h"
#include "bluetooth_device_state.h"
#include "call_ability_report_proxy.h"
#include "call_object_manager.h"
#include "call_state_processor.h"
#include "earpiece_device_state.h"
#include "inactive_device_state.h"
#include "speaker_device_state.h"
#include "telephony_log_wrapper.h"
#include "wired_headset_device_state.h"
#include "distributed_call_manager.h"
#include "audio_system_manager.h"
#include "audio_device_info.h"

namespace OHOS {
namespace Telephony {
using namespace AudioStandard;
bool AudioDeviceManager::isBtScoDevEnable_ = false;
bool AudioDeviceManager::isDCallDevEnable_ = false;
bool AudioDeviceManager::isSpeakerAvailable_ = true; // default available
bool AudioDeviceManager::isEarpieceAvailable_ = true;
bool AudioDeviceManager::isWiredHeadsetConnected_ = false;
bool AudioDeviceManager::isBtScoConnected_ = false;
bool AudioDeviceManager::isDCallDevConnected_ = false;

AudioDeviceManager::AudioDeviceManager()
    : audioDeviceType_(AudioDeviceType::DEVICE_UNKNOWN), currentAudioDevice_(nullptr), isAudioActivated_(false)
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
    if (memset_s(&info_, sizeof(AudioDeviceInfo), 0, sizeof(AudioDeviceInfo)) != EOK) {
        TELEPHONY_LOGE("memset_s address fail");
        return;
    }
    AudioDevice speaker = {
        .deviceType = AudioDeviceType::DEVICE_SPEAKER,
        .address = { 0 },
    };
    info_.audioDeviceList.push_back(speaker);
    AudioDevice earpiece = {
        .deviceType = AudioDeviceType::DEVICE_EARPIECE,
        .address = { 0 },
    };
    info_.audioDeviceList.push_back(earpiece);
}

void AudioDeviceManager::AddAudioDeviceList(const std::string &address, AudioDeviceType deviceType,
    const std::string &deviceName)
{
    std::lock_guard<std::mutex> lock(infoMutex_);
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    while (it != info_.audioDeviceList.end()) {
        if (it->address == address && it->deviceType == deviceType) {
            TELEPHONY_LOGI("device is already existenced");
            return;
        }
        if (deviceType == AudioDeviceType::DEVICE_WIRED_HEADSET && it->deviceType == AudioDeviceType::DEVICE_EARPIECE) {
            it = info_.audioDeviceList.erase(it);
            TELEPHONY_LOGI("remove Earpiece device success");
        } else {
            ++it;
        }
    }
    AudioDevice audioDevice;
    if (memset_s(&audioDevice, sizeof(AudioDevice), 0, sizeof(AudioDevice)) != EOK) {
        TELEPHONY_LOGE("memset_s fail");
        return;
    }
    audioDevice.deviceType = deviceType;
    if (address.length() > kMaxAddressLen) {
        TELEPHONY_LOGE("address is not too long");
        return;
    }
    if (memcpy_s(audioDevice.address, kMaxAddressLen, address.c_str(), address.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s address fail");
        return;
    }
    if (deviceName.length() > kMaxDeviceNameLen) {
        TELEPHONY_LOGE("deviceName is not too long");
        return;
    }
    if (memcpy_s(audioDevice.deviceName, kMaxDeviceNameLen, deviceName.c_str(), deviceName.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s deviceName fail");
        return;
    }
    info_.audioDeviceList.push_back(audioDevice);
    if (deviceType == AudioDeviceType::DEVICE_WIRED_HEADSET) {
        SetDeviceAvailable(AudioDeviceType::DEVICE_WIRED_HEADSET, true);
    }
    if (deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
        SetDeviceAvailable(AudioDeviceType::DEVICE_BLUETOOTH_SCO, true);
    }
    if (IsDistributedAudioDeviceType(deviceType)) {
        SetDeviceAvailable(deviceType, true);
    }
    ReportAudioDeviceInfo();
    TELEPHONY_LOGI("AddAudioDeviceList success");
}

void AudioDeviceManager::RemoveAudioDeviceList(const std::string &address, AudioDeviceType deviceType)
{
    std::lock_guard<std::mutex> lock(infoMutex_);
    bool needAddEarpiece = true;
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    while (it != info_.audioDeviceList.end()) {
        if (it->deviceType == AudioDeviceType::DEVICE_EARPIECE) {
            needAddEarpiece = false;
        }
        if (it->address == address && it->deviceType == deviceType) {
            it = info_.audioDeviceList.erase(it);
        } else {
            ++it;
        }
    }

    bool wiredHeadsetExist = false;
    bool blueToothScoExist = false;
    for (auto &elem : info_.audioDeviceList) {
        if (elem.deviceType == AudioDeviceType::DEVICE_WIRED_HEADSET) {
            wiredHeadsetExist = true;
        }
        if (elem.deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
            blueToothScoExist = true;
        }
    }
    if (deviceType == AudioDeviceType::DEVICE_WIRED_HEADSET && !wiredHeadsetExist) {
        SetDeviceAvailable(AudioDeviceType::DEVICE_WIRED_HEADSET, false);
    }
    if (deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO && !blueToothScoExist) {
        SetDeviceAvailable(AudioDeviceType::DEVICE_BLUETOOTH_SCO, false);
    }
    if (IsDistributedAudioDeviceType(deviceType)) {
        SetDeviceAvailable(deviceType, false);
    }
    if (needAddEarpiece && deviceType == AudioDeviceType::DEVICE_WIRED_HEADSET && !wiredHeadsetExist) {
        AudioDevice audioDevice = {
            .deviceType = AudioDeviceType::DEVICE_EARPIECE,
            .address = { 0 },
        };
        info_.audioDeviceList.push_back(audioDevice);
        TELEPHONY_LOGI("add Earpiece device success");
    }
    DelayedSingleton<AudioControlManager>::GetInstance()->UpdateDeviceTypeForVideoOrSatelliteCall();
    ReportAudioDeviceInfo();
    TELEPHONY_LOGI("RemoveAudioDeviceList success");
}

void AudioDeviceManager::ResetBtAudioDevicesList()
{
    std::lock_guard<std::mutex> lock(infoMutex_);
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    while (it != info_.audioDeviceList.end()) {
        if (it->deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
            it = info_.audioDeviceList.erase(it);
        } else {
            ++it;
        }
    }
    SetDeviceAvailable(AudioDeviceType::DEVICE_BLUETOOTH_SCO, false);
    ReportAudioDeviceInfo();
    TELEPHONY_LOGI("ResetBtAudioDevicesList success");
}

void AudioDeviceManager::ResetDistributedCallDevicesList()
{
    std::lock_guard<std::mutex> lock(infoMutex_);
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    while (it != info_.audioDeviceList.end()) {
        if (IsDistributedAudioDeviceType(it->deviceType)) {
            it = info_.audioDeviceList.erase(it);
        } else {
            ++it;
        }
    }
    SetDeviceAvailable(AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE, false);
    SetDeviceAvailable(AudioDeviceType::DEVICE_DISTRIBUTED_PHONE, false);
    SetDeviceAvailable(AudioDeviceType::DEVICE_DISTRIBUTED_PAD, false);
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAudioDeviceChange(info_);
    TELEPHONY_LOGI("Reset Distributed Audio Devices List success");
}

bool AudioDeviceManager::InitAudioDevice()
{
    // when audio deactivate interrupt , reinit
    // when external audio device connection state changed , reinit
    auto device = DelayedSingleton<AudioControlManager>::GetInstance()->GetInitAudioDeviceType();
    return SwitchDevice(device);
}

bool AudioDeviceManager::ProcessEvent(AudioEvent event)
{
    bool result = false;
    switch (event) {
        case AudioEvent::AUDIO_ACTIVATED:
        case AudioEvent::AUDIO_RINGING:
            if (!isAudioActivated_) {
                isAudioActivated_ = true;
                std::unique_ptr<AudioStandard::AudioDeviceDescriptor> activeBluetoothDevice =
                    AudioStandard::AudioRoutingManager::GetInstance()->GetActiveBluetoothDevice();
                if (activeBluetoothDevice != nullptr && !activeBluetoothDevice->macAddress_.empty()) {
                    AudioSystemManager* audioSystemManager = AudioSystemManager::GetInstance();
                    int32_t ret = audioSystemManager->SetCallDeviceActive(ActiveDeviceType::BLUETOOTH_SCO,
                        true, activeBluetoothDevice->macAddress_);
                    return ret == 0 ? true : false;
                }
                AudioDevice device = {
                    .deviceType = AudioDeviceType::DEVICE_EARPIECE,
                    .address = { 0 },
                };
                if (DelayedSingleton<AudioProxy>::GetInstance()->GetPreferredOutputAudioDevice(device) !=
                    TELEPHONY_SUCCESS) {
                    TELEPHONY_LOGE("current audio device nullptr");
                    return false;
                }
                SetCurrentAudioDevice(device.deviceType);
            }
            break;
        case AudioEvent::AUDIO_DEACTIVATED:
            if (isAudioActivated_) {
                isAudioActivated_ = false;
                result = InitAudioDevice();
            }
            break;
        case AudioEvent::INIT_AUDIO_DEVICE:
            result = InitAudioDevice();
            break;
        case AudioEvent::WIRED_HEADSET_DISCONNECTED: {
            if (!isAudioActivated_) {
                TELEPHONY_LOGE("call is not active, no need to connect sco");
                return false;
            }
            break;
        }
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

bool AudioDeviceManager::SwitchDevice(AudioDeviceType device)
{
    bool result = false;
    std::lock_guard<std::mutex> lock(mutex_);
    switch (device) {
        case AudioDeviceType::DEVICE_EARPIECE:
            result = EnableEarpiece();
            break;
        case AudioDeviceType::DEVICE_SPEAKER:
            result = EnableSpeaker();
            break;
        case AudioDeviceType::DEVICE_WIRED_HEADSET:
            result = EnableWiredHeadset();
            break;
        case AudioDeviceType::DEVICE_BLUETOOTH_SCO:
            result = EnableBtSco();
            break;
        case AudioDeviceType::DEVICE_DISABLE:
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
        TELEPHONY_LOGI("speaker enabled , current audio device : speaker");
        SetCurrentAudioDevice(AudioDeviceType::DEVICE_SPEAKER);
        return true;
    }
    TELEPHONY_LOGI("enable speaker device failed");
    return false;
}

bool AudioDeviceManager::EnableEarpiece()
{
    if (isEarpieceAvailable_ && DelayedSingleton<AudioProxy>::GetInstance()->SetEarpieceDevActive()) {
        TELEPHONY_LOGI("earpiece enabled , current audio device : earpiece");
        SetCurrentAudioDevice(AudioDeviceType::DEVICE_EARPIECE);
        return true;
    }
    TELEPHONY_LOGI("enable earpiece device failed");
    return false;
}

bool AudioDeviceManager::EnableWiredHeadset()
{
    if (isWiredHeadsetConnected_ && DelayedSingleton<AudioProxy>::GetInstance()->SetWiredHeadsetDevActive()) {
        TELEPHONY_LOGI("wired headset enabled , current audio device : wired headset");
        SetCurrentAudioDevice(AudioDeviceType::DEVICE_WIRED_HEADSET);
        return true;
    }
    TELEPHONY_LOGI("enable wired headset device failed");
    return false;
}

bool AudioDeviceManager::EnableBtSco()
{
    if (isBtScoConnected_) {
        TELEPHONY_LOGI("bluetooth sco enabled , current audio device : bluetooth sco");
        SetCurrentAudioDevice(AudioDeviceType::DEVICE_BLUETOOTH_SCO);
        return true;
    }
    TELEPHONY_LOGI("enable bluetooth sco device failed");
    return false;
}

bool AudioDeviceManager::EnableDistributedCall()
{
    if (isDCallDevConnected_) {
        AudioDeviceType type = DelayedSingleton<DistributedCallManager>::GetInstance()->GetConnectedDCallDeviceType();
        TELEPHONY_LOGI("distributed call enabled, current audio device: %d", static_cast<int32_t>(type));
        SetCurrentAudioDevice(type);
        return true;
    }
    TELEPHONY_LOGI("enable distributed call device failed");
    return false;
}

bool AudioDeviceManager::DisableAll()
{
    audioDeviceType_ = AudioDeviceType::DEVICE_UNKNOWN;
    isBtScoDevEnable_ = false;
    isDCallDevEnable_ = false;
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

void AudioDeviceManager::SetCurrentAudioDevice(AudioDeviceType deviceType)
{
    TELEPHONY_LOGI("set current audio device, deviceType: %{public}d.", deviceType);
    if (!IsDistributedAudioDeviceType(deviceType) && IsDistributedAudioDeviceType(audioDeviceType_)) {
        DelayedSingleton<DistributedCallManager>::GetInstance()->SwitchOffDCallDeviceSync();
    }
    if (deviceType == AudioDeviceType::DEVICE_EARPIECE &&
        DelayedSingleton<AudioControlManager>::GetInstance()->IsSatelliteExists()) {
        audioDeviceType_ = AudioDeviceType::DEVICE_SPEAKER;
        AudioStandard::AudioSystemManager::GetInstance()->
            SetDeviceActive(AudioStandard::ActiveDeviceType::SPEAKER, true);
        return;
    }
    audioDeviceType_ = deviceType;
    ReportAudioDeviceChange();
}

bool AudioDeviceManager::CheckAndSwitchDistributedAudioDevice()
{
    TELEPHONY_LOGI("check and switch distributed audio device.");
    std::lock_guard<std::mutex> lock(infoMutex_);
    DelayedSingleton<DistributedCallManager>::GetInstance()->SetCallState(true);
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    while (it != info_.audioDeviceList.end()) {
        if (it->deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE) {
            DelayedSingleton<DistributedCallManager>::GetInstance()->SwitchOnDCallDeviceAsync(*it);
            return true;
        } else {
            ++it;
        }
    }
    return false;
}

void AudioDeviceManager::OnActivedCallDisconnected()
{
    DelayedSingleton<DistributedCallManager>::GetInstance()->SetCallState(false);
    DelayedSingleton<DistributedCallManager>::GetInstance()->DealDisconnectCall();
}

int32_t AudioDeviceManager::ReportAudioDeviceChange()
{
    if (audioDeviceType_ == AudioDeviceType::DEVICE_UNKNOWN) {
        audioDeviceType_ = DelayedSingleton<AudioControlManager>::GetInstance()->GetInitAudioDeviceType();
        info_.currentAudioDevice.deviceType = audioDeviceType_;
    } else {
        info_.currentAudioDevice.deviceType = audioDeviceType_;
    }
    std::string address = "";
    std::string deviceName = "";
    if (audioDeviceType_ == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
        std::shared_ptr<BluetoothCallManager> bluetoothCallManager = std::make_shared<BluetoothCallManager>();
        address = bluetoothCallManager->GetConnectedScoAddr();
        deviceName = bluetoothCallManager->GetConnectedScoName();
    } else if (IsDistributedAudioDeviceType(audioDeviceType_)) {
        address = DelayedSingleton<DistributedCallManager>::GetInstance()->GetConnectedDCallDeviceAddr();
    }
    if (address.length() > kMaxAddressLen) {
        TELEPHONY_LOGE("address is not too long");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (memset_s(info_.currentAudioDevice.address, kMaxAddressLen + 1, 0, kMaxAddressLen + 1) != EOK) {
        TELEPHONY_LOGE("failed to memset_s currentAudioDevice.address");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    if (memcpy_s(info_.currentAudioDevice.address, kMaxAddressLen, address.c_str(), address.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s address fail");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    if (deviceName.length() > kMaxDeviceNameLen) {
        TELEPHONY_LOGE("deviceName is not too long");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (memcpy_s(info_.currentAudioDevice.deviceName, kMaxDeviceNameLen,
        deviceName.c_str(), deviceName.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s deviceName fail");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    return ReportAudioDeviceInfo();
}

int32_t AudioDeviceManager::ReportAudioDeviceInfo()
{
    int32_t callId = DelayedSingleton<CallStateProcessor>::GetInstance()->GetAudioForegroundLiveCall();
    if (callId == INVALID_CALLID) {
        TELEPHONY_LOGE("callId is invalid");
        return ReportAudioDeviceInfo(nullptr);
    }
    sptr<CallBase> liveCall = CallObjectManager::GetOneCallObject(callId);
    return ReportAudioDeviceInfo(liveCall);
}

int32_t AudioDeviceManager::ReportAudioDeviceInfo(sptr<CallBase> call)
{
    if (call != nullptr && call->GetCallType() == CallType::TYPE_VOIP) {
        info_.isMuted = call->IsMuted();
    } else {
        info_.isMuted = DelayedSingleton<AudioProxy>::GetInstance()->IsMicrophoneMute();
    }
    TELEPHONY_LOGI("report audio device info, currentAudioDeviceType:%{public}d, currentAddress:%{public}s, "
        "mute:%{public}d", info_.currentAudioDevice.deviceType, info_.currentAudioDevice.address, info_.isMuted);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAudioDeviceChange(info_);
}

AudioDeviceType AudioDeviceManager::GetCurrentAudioDevice()
{
    return audioDeviceType_;
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

bool AudioDeviceManager::IsDCallDevEnable()
{
    return isDCallDevEnable_;
}

bool AudioDeviceManager::IsBtScoConnected()
{
    return isBtScoConnected_;
}

bool AudioDeviceManager::IsDistributedCallConnected()
{
    return isDCallDevConnected_;
}

bool AudioDeviceManager::IsWiredHeadsetConnected()
{
    return isWiredHeadsetConnected_;
}

bool AudioDeviceManager::IsEarpieceAvailable()
{
    return isEarpieceAvailable_;
}

bool AudioDeviceManager::IsSpeakerAvailable()
{
    return isSpeakerAvailable_;
}

bool AudioDeviceManager::IsDistributedAudioDeviceType(AudioDeviceType deviceType)
{
    if (((deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE) ||
        (deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_PHONE) ||
        (deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_PAD))) {
        return true;
    }
    return false;
}

void AudioDeviceManager::SetDeviceAvailable(AudioDeviceType deviceType, bool available)
{
    switch (deviceType) {
        case AudioDeviceType::DEVICE_SPEAKER:
            isSpeakerAvailable_ = available;
            break;
        case AudioDeviceType::DEVICE_EARPIECE:
            isEarpieceAvailable_ = available;
            break;
        case AudioDeviceType::DEVICE_BLUETOOTH_SCO:
            isBtScoConnected_ = available;
            break;
        case AudioDeviceType::DEVICE_WIRED_HEADSET:
            isWiredHeadsetConnected_ = available;
            break;
        case AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE:
        case AudioDeviceType::DEVICE_DISTRIBUTED_PHONE:
        case AudioDeviceType::DEVICE_DISTRIBUTED_PAD:
            isDCallDevConnected_ = available;
            break;
        default:
            break;
    }
}
} // namespace Telephony
} // namespace OHOS