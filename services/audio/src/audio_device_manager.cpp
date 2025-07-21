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
#include "earpiece_device_state.h"
#include "inactive_device_state.h"
#include "speaker_device_state.h"
#include "telephony_log_wrapper.h"
#include "wired_headset_device_state.h"
#include "distributed_call_manager.h"
#include "audio_system_manager.h"
#include "audio_device_info.h"
#include "distributed_communication_manager.h"
#include "bluetooth_call_connection.h"

namespace OHOS {
namespace Telephony {
using namespace AudioStandard;

constexpr int32_t DEVICE_ADDR_LEN = 7;
constexpr int32_t ADDR_HEAD_VALID_LEN = 5;
constexpr int32_t ADDR_TAIL_VALID_LEN = 2;
bool AudioDeviceManager::isBtScoDevEnable_ = false;
bool AudioDeviceManager::isDCallDevEnable_ = false;
bool AudioDeviceManager::isSpeakerAvailable_ = true; // default available
bool AudioDeviceManager::isEarpieceAvailable_ = true;
bool AudioDeviceManager::isUpdateEarpieceDevice_ = false;
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
    memberFuncMap_[AudioEvent::ENABLE_DEVICE_EARPIECE] = [this]() { return EnableEarpiece(); };
    memberFuncMap_[AudioEvent::ENABLE_DEVICE_SPEAKER] = [this]() { return EnableSpeaker(); };
    memberFuncMap_[AudioEvent::ENABLE_DEVICE_WIRED_HEADSET] = [this]() { return EnableWiredHeadset(); };
    memberFuncMap_[AudioEvent::ENABLE_DEVICE_BLUETOOTH] = [this]() { return EnableBtSco(); };
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

bool AudioDeviceManager::IsSupportEarpiece()
{
    std::vector<std::shared_ptr<AudioDeviceDescriptor>> audioDeviceList =
        AudioStandard::AudioRoutingManager::GetInstance()->GetAvailableDevices(AudioDeviceUsage::CALL_OUTPUT_DEVICES);
    for (auto& audioDevice : audioDeviceList) {
        TELEPHONY_LOGI("available deviceType : %{public}d", audioDevice->deviceType_);
        if (audioDevice->deviceType_ == AudioStandard::DeviceType::DEVICE_TYPE_EARPIECE) {
            return true;
        }
    }
    return false;
}

void AudioDeviceManager::UpdateEarpieceDevice()
{
    if (isUpdateEarpieceDevice_ || IsSupportEarpiece()) {
        isUpdateEarpieceDevice_ = true;
        return;
    }
    isUpdateEarpieceDevice_ = true;
    std::lock_guard<std::mutex> lock(infoMutex_);
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    while (it != info_.audioDeviceList.end()) {
        if (it->deviceType == AudioDeviceType::DEVICE_EARPIECE) {
            it = info_.audioDeviceList.erase(it);
            TELEPHONY_LOGI("not support Earpice, remove Earpice device success");
            return;
        } else {
            ++it;
        }
    }
}

void AudioDeviceManager::UpdateBluetoothDeviceName(const std::string &macAddress, const std::string &deviceName)
{
    std::lock_guard<std::mutex> lock(infoMutex_);
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    while (it != info_.audioDeviceList.end()) {
        if (it->address == macAddress && it->deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
            if (deviceName.length() > kMaxDeviceNameLen) {
                TELEPHONY_LOGE("deviceName is too long");
                return;
            }
            if (memset_s(it->deviceName, sizeof(it->deviceName), 0, sizeof(it->deviceName)) != EOK) {
                TELEPHONY_LOGE("memset_s fail");
                return;
            }
            if (memcpy_s(it->deviceName, kMaxDeviceNameLen, deviceName.c_str(), deviceName.length()) != EOK) {
                TELEPHONY_LOGE("memcpy_s deviceName fail");
                return;
            }
            TELEPHONY_LOGI("UpdateBluetoothDeviceName");
            ReportAudioDeviceInfo();
            return;
        }
        ++it;
    }
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
        TELEPHONY_LOGE("address is too long");
        return;
    }
    if (memcpy_s(audioDevice.address, kMaxAddressLen, address.c_str(), address.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s address fail");
        return;
    }
    if (deviceName.length() > kMaxDeviceNameLen) {
        TELEPHONY_LOGE("deviceName is too long");
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
        AddEarpiece();
    }
    sptr<CallBase> liveCall = CallObjectManager::GetAudioLiveCall();
    if (liveCall != nullptr && (liveCall->GetVideoStateType() == VideoStateType::TYPE_VIDEO ||
        liveCall->GetCallType() == CallType::TYPE_SATELLITE)) {
        DelayedSingleton<AudioControlManager>::GetInstance()->UpdateDeviceTypeForVideoOrSatelliteCall();
    }
    ReportAudioDeviceInfo();
    TELEPHONY_LOGI("RemoveAudioDeviceList success");
}

void AudioDeviceManager::AddEarpiece()
{
    if (!IsSupportEarpiece()) {
        TELEPHONY_LOGI("not support Earpiece device");
        return;
    }
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_EARPIECE,
        .address = { 0 },
    };
    info_.audioDeviceList.push_back(audioDevice);
    TELEPHONY_LOGI("add Earpiece device success");
}

void AudioDeviceManager::ResetBtAudioDevicesList()
{
    std::lock_guard<std::mutex> lock(infoMutex_);
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    bool hadBtActived = false;
    while (it != info_.audioDeviceList.end()) {
        if (it->deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
            hadBtActived = true;
            it = info_.audioDeviceList.erase(it);
        } else {
            ++it;
        }
    }
    SetDeviceAvailable(AudioDeviceType::DEVICE_BLUETOOTH_SCO, false);
    if (hadBtActived) {
        ReportAudioDeviceInfo();
    }
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
    SetDeviceAvailable(AudioDeviceType::DEVICE_DISTRIBUTED_PC, false);
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAudioDeviceChange(info_);
    TELEPHONY_LOGI("Reset Distributed Audio Devices List success");
}

void AudioDeviceManager::ResetNearlinkAudioDevicesList()
{
    std::unique_lock<std::mutex> lock(infoMutex_);
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    bool hadNearlinkActived = false;
    while (it != info_.audioDeviceList.end()) {
        if (it->deviceType == AudioDeviceType::DEVICE_NEARLINK) {
            hadNearlinkActived = true;
            it = info_.audioDeviceList.erase(it);
        } else {
            ++it;
        }
    }
    if (hadNearlinkActived) {
        ReportAudioDeviceInfo();
    }
    lock.unlock();
    if (audioDeviceType_ == AudioDeviceType::DEVICE_NEARLINK) {
        TELEPHONY_LOGI("Nearlink SA removed, init audio device");
        ProcessEvent(AudioEvent::INIT_AUDIO_DEVICE);
    }
    TELEPHONY_LOGI("ResetNearlinkAudioDevicesList success");
}

void AudioDeviceManager::ResetBtHearingAidDeviceList()
{
    std::unique_lock<std::mutex> lock(infoMutex_);
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    bool hadBtHearingAidActived = false;
    while (it != info_.audioDeviceList.end()) {
        if (it->deviceType == AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID) {
            hadBtHearingAidActived = true;
            it = info_.audioDeviceList.erase(it);
        } else {
            ++it;
        }
    }
    if (hadBtHearingAidActived) {
        ReportAudioDeviceInfo();
    }
    lock.unlock();
    if (audioDeviceType_ == AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID) {
        TELEPHONY_LOGI("Bluetooth SA removed, init audio device");
        ProcessEvent(AudioEvent::INIT_AUDIO_DEVICE);
    }
    TELEPHONY_LOGI("ResetBtHearingAidDeviceList success");
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
            CheckAndSwitchDistributedAudioDevice();
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
        return memberFunc();
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
        case AudioDeviceType::DEVICE_NEARLINK:
            result = EnableNearlink();
            break;
        case AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID:
            result = EnableBtHearingAid();
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("switch device lock release");
    return result;
}

bool AudioDeviceManager::EnableSpeaker()
{
    if (isSpeakerAvailable_ && DelayedSingleton<AudioProxy>::GetInstance()->SetSpeakerDevActive(true)) {
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
    if (IsBtActived()) {
        TELEPHONY_LOGI("bluetooth sco enabled , current audio device : bluetooth sco");
        SetCurrentAudioDevice(AudioDeviceType::DEVICE_BLUETOOTH_SCO);
        return true;
    }
    TELEPHONY_LOGI("enable bluetooth sco device failed");
    return false;
}

bool AudioDeviceManager::EnableNearlink()
{
    AudioDevice device;
    if (IsNearlinkActived(device)) {
        TELEPHONY_LOGI("nearlink enabled , current audio device : nearlink");
        SetCurrentAudioDevice(AudioDeviceType::DEVICE_NEARLINK);
        return true;
    }
    TELEPHONY_LOGI("enable nearlink device failed");
    return false;
}

bool AudioDeviceManager::EnableBtHearingAid()
{
    AudioDevice device;
    if (IsBtHearingAidActived(device)) {
        TELEPHONY_LOGI("bt hearing aid enabled , current audio device : bt hearing aid");
        SetCurrentAudioDevice(AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID);
        return true;
    }
    TELEPHONY_LOGI("enable bt hearing aid device failed");
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
    if (deviceType == AudioDeviceType::DEVICE_EARPIECE && CallObjectManager::HasSatelliteCallExist()) {
        audioDeviceType_ = AudioDeviceType::DEVICE_SPEAKER;
        AudioStandard::AudioSystemManager::GetInstance()->
            SetDeviceActive(AudioStandard::DeviceType::DEVICE_TYPE_SPEAKER, true);
        return;
    }
    AudioDevice device = {
        .deviceType = deviceType,
        .address = { 0 },
    };
    SetCurrentAudioDevice(device);
}

bool AudioDeviceManager::SetVirtualCall(bool isVirtual)
{
    TELEPHONY_LOGI("Virtualcall SetVirtualCall: %{public}d.", isVirtual);
    return AudioStandard::AudioSystemManager::GetInstance()->SetVirtualCall(isVirtual);
}

void AudioDeviceManager::SetCurrentAudioDevice(const AudioDevice &device)
{
    AudioDeviceType deviceType = device.deviceType;
    TELEPHONY_LOGI("set current audio device, audioDeviceType = %{public}d.", deviceType);
    if (!IsDistributedAudioDeviceType(deviceType) && IsDistributedAudioDeviceType(audioDeviceType_)) {
        DelayedSingleton<DistributedCallManager>::GetInstance()->SwitchOffDCallDeviceSync();
    }
    audioDeviceType_ = deviceType;
    ReportAudioDeviceChange(device);
}

bool AudioDeviceManager::CheckAndSwitchDistributedAudioDevice()
{
    TELEPHONY_LOGI("check and switch distributed audio device.");
    std::lock_guard<std::mutex> lock(infoMutex_);
    DelayedSingleton<DistributedCallManager>::GetInstance()->SetCallState(true);
    std::vector<AudioDevice>::iterator it = info_.audioDeviceList.begin();
    while (it != info_.audioDeviceList.end()) {
        if (it->deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE &&
            DelayedSingleton<DistributedCallManager>::GetInstance()->IsSelectVirtualModem()) {
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

int32_t AudioDeviceManager::ReportAudioDeviceChange(const AudioDevice &device)
{
    if (audioDeviceType_ == AudioDeviceType::DEVICE_UNKNOWN) {
        audioDeviceType_ = DelayedSingleton<AudioControlManager>::GetInstance()->GetInitAudioDeviceType();
        info_.currentAudioDevice.deviceType = audioDeviceType_;
    } else {
        info_.currentAudioDevice.deviceType = audioDeviceType_;
    }
    std::string address = device.address;
    std::string deviceName = device.deviceName;
    if (audioDeviceType_ == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
        UpdateBtDevice(address, deviceName);
    } else if (DelayedSingleton<DistributedCommunicationManager>::GetInstance()->IsDistributedDev(device)) {
        TELEPHONY_LOGI("audio device is distributed communication dev");
    } else if (IsDistributedAudioDeviceType(audioDeviceType_)) {
        address = DelayedSingleton<DistributedCallManager>::GetInstance()->GetConnectedDCallDeviceAddr();
    } else if (audioDeviceType_ == AudioDeviceType::DEVICE_NEARLINK) {
        UpdateNearlinkDevice(address, deviceName);
    } else if (audioDeviceType_ == AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID) {
        UpdateBtHearingAidDevice(address, deviceName);
    }
    if (address.length() > kMaxAddressLen) {
        TELEPHONY_LOGE("address is not too long");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (memset_s(info_.currentAudioDevice.address, kMaxAddressLen + 1, 0, kMaxAddressLen + 1) != EOK) {
        TELEPHONY_LOGE("failed to memset_s currentAudioDevice.address");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    if (memcpy_s(info_.currentAudioDevice.address, kMaxAddressLen, address.c_str(), address.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s address fail");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    if (deviceName.length() > kMaxDeviceNameLen) {
        TELEPHONY_LOGE("deviceName is too long");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (memset_s(info_.currentAudioDevice.deviceName, kMaxDeviceNameLen + 1, 0, kMaxDeviceNameLen + 1) != EOK) {
        TELEPHONY_LOGE("failed to memset_s currentAudioDevice.deviceName");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    if (memcpy_s(info_.currentAudioDevice.deviceName, kMaxDeviceNameLen,
        deviceName.c_str(), deviceName.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s deviceName fail");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    return ReportAudioDeviceInfo();
}

void AudioDeviceManager::UpdateBtDevice(std::string &address, std::string &deviceName)
{
    if (!address.empty() && !deviceName.empty()) {
        return;
    }

    std::shared_ptr<AudioStandard::AudioDeviceDescriptor> activeBluetoothDevice =
        AudioStandard::AudioRoutingManager::GetInstance()->GetActiveBluetoothDevice();
    if (activeBluetoothDevice != nullptr && !activeBluetoothDevice->macAddress_.empty()) {
        address = activeBluetoothDevice->macAddress_;
        deviceName = activeBluetoothDevice->deviceName_;
    }
}

void AudioDeviceManager::UpdateNearlinkDevice(std::string &address, std::string &deviceName)
{
    if (!address.empty() && !deviceName.empty()) {
        return;
    }

    AudioDevice device;
    if (!IsNearlinkActived(device)) {
        TELEPHONY_LOGE("Get active nearlink device failed.");
        return;
    }
    address = device.address;
    deviceName = device.deviceName;
}

void AudioDeviceManager::UpdateBtHearingAidDevice(std::string &address, std::string &deviceName)
{
    if (!address.empty() && !deviceName.empty()) {
        return;
    }

    AudioDevice device;
    if (!IsBtHearingAidActived(device)) {
        TELEPHONY_LOGE("Get active bt hearing aid device failed.");
        return;
    }
    address = device.address;
    deviceName = device.deviceName;
}

int32_t AudioDeviceManager::ReportAudioDeviceInfo()
{
    sptr<CallBase> liveCall = CallObjectManager::GetAudioLiveCall();
    return ReportAudioDeviceInfo(liveCall);
}

std::string AudioDeviceManager::ConvertAddress()
{
    std::string addr = info_.currentAudioDevice.address;
    if (info_.currentAudioDevice.deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO ||
        info_.currentAudioDevice.deviceType == AudioDeviceType::DEVICE_NEARLINK ||
        info_.currentAudioDevice.deviceType == AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID) {
        if (!addr.empty() && addr.length() > DEVICE_ADDR_LEN) {
            return (addr.substr(0, ADDR_HEAD_VALID_LEN) + ":*:*:*:" +
                addr.substr(addr.length() - ADDR_TAIL_VALID_LEN));
        }
    } else {
        addr = "";
    }
    return addr;
}

int32_t AudioDeviceManager::ReportAudioDeviceInfo(sptr<CallBase> call)
{
    if (call != nullptr && (call->GetCallType() == CallType::TYPE_VOIP ||
        call->GetCallType() == CallType::TYPE_BLUETOOTH)) {
        info_.isMuted = call->IsMuted();
    } else {
        info_.isMuted = DelayedSingleton<AudioProxy>::GetInstance()->IsMicrophoneMute();
    }
    if (call != nullptr) {
        info_.callId = call->GetCallID();
    }
    AudioDeviceType deviceType = info_.currentAudioDevice.deviceType;
    if (call != nullptr && call->GetCallType() == CallType::TYPE_BLUETOOTH &&
        (call->GetTelCallState() == TelCallState::CALL_STATUS_ACTIVE ||
        call->GetTelCallState() == TelCallState::CALL_STATUS_DIALING ||
        call->GetTelCallState() == TelCallState::CALL_STATUS_ALERTING)) {
        bool state = DelayedSingleton<BluetoothCallConnection>::GetInstance()->GetBtCallScoConnected();
        if (state) {
            info_.currentAudioDevice.deviceType = AudioDeviceType::DEVICE_SPEAKER;
        } else {
            info_.currentAudioDevice.deviceType = AudioDeviceType::DEVICE_EARPIECE;
        }
    }
    TELEPHONY_LOGI("report audio device info, currentAudioDeviceType:%{public}d, currentAddress:%{public}s, "
        "mute:%{public}d, callId:%{public}d", info_.currentAudioDevice.deviceType, ConvertAddress().c_str(),
        info_.isMuted, info_.callId);
    int32_t ret = DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAudioDeviceChange(info_);
    info_.currentAudioDevice.deviceType = deviceType;
    return ret;
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

bool AudioDeviceManager::IsBtActived()
{
    std::shared_ptr<AudioStandard::AudioDeviceDescriptor> activeBluetoothDevice =
        AudioStandard::AudioRoutingManager::GetInstance()->GetActiveBluetoothDevice();
    if (activeBluetoothDevice != nullptr && !activeBluetoothDevice->macAddress_.empty()) {
        TELEPHONY_LOGI("has actived bt device");
        return true;
    }
    return false;
}

bool AudioDeviceManager::IsNearlinkActived(AudioDevice &device)
{
    if (DelayedSingleton<AudioProxy>::GetInstance()->GetPreferredOutputAudioDevice(device) != TELEPHONY_SUCCESS) {
        return false;
    }
    if (device.deviceType != AudioDeviceType::DEVICE_NEARLINK || strlen(device.address) == 0) {
        return false;
    }
    return true;
}

bool AudioDeviceManager::IsBtHearingAidActived(AudioDevice &device)
{
    if (DelayedSingleton<AudioProxy>::GetInstance()->GetPreferredOutputAudioDevice(device) != TELEPHONY_SUCCESS) {
        return false;
    }
    if (device.deviceType != AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID || strlen(device.address) == 0) {
        return false;
    }
    return true;
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
        (deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_PAD) ||
        (deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_PC))) {
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
        case AudioDeviceType::DEVICE_DISTRIBUTED_PC:
            isDCallDevConnected_ = available;
            break;
        default:
            break;
    }
}
} // namespace Telephony
} // namespace OHOS