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

#include "bluetooth_connection.h"
#include "bluetooth_call_connection.h"
#include "audio_control_manager.h"
#include "bluetooth_call_manager.h"
#include "telephony_log_wrapper.h"
#ifdef ABILITY_BLUETOOTH_SUPPORT
#include "bluetooth_host.h"
#include "bluetooth_audio_manager.h"
#include "bluetooth_device.h"

constexpr int32_t PHONE_NUMBER_TYPE = 0x81;
constexpr int32_t VIRTUAL_DEVICE_ADD = 0;
constexpr int32_t VIRTUAL_DEVICE_REMOVE = 1;
#endif

namespace OHOS {
namespace Telephony {
#ifdef ABILITY_BLUETOOTH_SUPPORT
constexpr int32_t DEFAULT_BT_VALUE = -1;
#endif

BluetoothConnection::BluetoothConnection() : connectedScoAddr_("") {}

BluetoothConnection::~BluetoothConnection()
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    if (statusChangeListener_ != nullptr) {
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy != nullptr) {
            samgrProxy->UnSubscribeSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID, statusChangeListener_);
            statusChangeListener_ = nullptr;
        }
    }
    std::lock_guard<ffrt::ffrt> lock(bluetoothMutex_);
    mapConnectedBtDevices_.clear();
#endif
}

void BluetoothConnection::Init()
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    statusChangeListener_ = new (std::nothrow) SystemAbilityListener();
    if (statusChangeListener_ == nullptr) {
        TELEPHONY_LOGE("failed to create statusChangeListener");
        return;
    }
    auto managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("get system ability manager error");
        return;
    }
    int32_t ret = managerPtr->SubscribeSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID, statusChangeListener_);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("failed to subscribe bluetooth service SA:%{public}d", BLUETOOTH_HOST_SYS_ABILITY_ID);
        return;
    }
    std::vector<Bluetooth::BluetoothRemoteDevice> devices;
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return;
    }
    int32_t result = profile->GetConnectedDevices(devices);
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("get connected devices fail");
        return;
    }
    for (auto device : devices) {
        std::string macAddress = device.GetDeviceAddr();
        std::string deviceName = device.GetDeviceName();
        AddBtDevice(macAddress, device);
        DelayedSingleton<AudioDeviceManager>::GetInstance()->AddAudioDeviceList(macAddress,
            AudioDeviceType::DEVICE_BLUETOOTH_SCO, deviceName);
    }
    TELEPHONY_LOGI("BluetoothConnection init success!");
#endif
}

#ifdef ABILITY_BLUETOOTH_SUPPORT

bool BluetoothConnection::IsBtAvailble()
{
    std::lock_guard<ffrt::ffrt> lock(bluetoothMutex_);
    if (mapConnectedBtDevices_.empty()) {
        TELEPHONY_LOGE("mapConnectedBtDevices_ is empty");
        return false;
    }

    return true;
}
#endif

int32_t BluetoothConnection::SendBtCallState(
    int32_t numActive, int32_t numHeld, int32_t callState, const std::string &number)
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return TELEPHONY_ERROR;
    }

    std::string nickName = "";
    profile->PhoneStateChanged(numActive, numHeld, callState, number, PHONE_NUMBER_TYPE, nickName);
#endif
    TELEPHONY_LOGI("PhoneStateChanged,numActive:%{public}d,numHeld:%{public}d,callState:%{public}d", numActive, numHeld,
        callState);
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothConnection::SendCallDetailsChange(int32_t callId, int32_t callState)
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return TELEPHONY_ERROR;
    }

    profile->CallDetailsChanged(callId, callState);
#endif
    TELEPHONY_LOGI("Send CallDetails");
    return TELEPHONY_SUCCESS;
}

bool BluetoothConnection::IsAudioActivated()
{
    return DelayedSingleton<AudioControlManager>::GetInstance()->IsAudioActivated();
}

#ifdef ABILITY_BLUETOOTH_SUPPORT

void BluetoothConnection::ResetBtConnection()
{
    std::lock_guard<ffrt::ffrt> lock(bluetoothMutex_);
    mapConnectedBtDevices_.clear();
}

void BluetoothConnection::RegisterObserver()
{
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("BluetoothConnection RegisterObserver fail!");
        return;
    }

    profile->RegisterObserver(shared_from_this());
}

void BluetoothConnection::AddBtDevice(const std::string &address, Bluetooth::BluetoothRemoteDevice device)
{
    std::lock_guard<ffrt::ffrt> lock(bluetoothMutex_);
    auto iter = mapConnectedBtDevices_.find(address);
    if (iter != mapConnectedBtDevices_.end()) {
        TELEPHONY_LOGI("device is existenced");
    } else {
        mapConnectedBtDevices_.insert(std::pair<std::string, const Bluetooth::BluetoothRemoteDevice>(address, device));
        TELEPHONY_LOGI("AddBtDevice success");
    }
}

void BluetoothConnection::RemoveBtDevice(const std::string &address)
{
    std::lock_guard<ffrt::ffrt> lock(bluetoothMutex_);
    if (mapConnectedBtDevices_.count(address) > 0) {
        mapConnectedBtDevices_.erase(address);
        TELEPHONY_LOGI("RemoveBtDevice success");
    } else {
        TELEPHONY_LOGE("device is not existenced");
    }
}

Bluetooth::BluetoothRemoteDevice *BluetoothConnection::GetBtDevice(const std::string &address)
{
    std::lock_guard<ffrt::ffrt> lock(bluetoothMutex_);
    if (mapConnectedBtDevices_.count(address) > 0) {
        auto iter = mapConnectedBtDevices_.find(address);
        if (iter != mapConnectedBtDevices_.end()) {
            return &iter->second;
        }
    }
    TELEPHONY_LOGE("device is not existenced");
    return nullptr;
}

void BluetoothConnection::OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device,
    int32_t state, int32_t cause)
{
    TELEPHONY_LOGI("BluetoothConnection::OnConnectionStateChanged state : %{public}d", state);
    std::string macAddress = device.GetDeviceAddr();
    std::string deviceName = device.GetDeviceName();
    switch (state) {
        case (int32_t)Bluetooth::BTConnectState::CONNECTED:
            DelayedSingleton<AudioDeviceManager>::GetInstance()->AddAudioDeviceList(
                macAddress, AudioDeviceType::DEVICE_BLUETOOTH_SCO, deviceName);
            AddBtDevice(macAddress, device);
            /** try to connect sco while new bluetooth device connected
             *  if connect sco successfully , should switch current audio device to bluetooth sco
             */
            break;
        case (int32_t)Bluetooth::BTConnectState::DISCONNECTED:
            DelayedSingleton<AudioDeviceManager>::GetInstance()->RemoveAudioDeviceList(
                macAddress, AudioDeviceType::DEVICE_BLUETOOTH_SCO);
            RemoveBtDevice(macAddress);
            break;
        default:
            break;
    }
}

void BluetoothConnection::OnVirtualDeviceChanged(int32_t action, std::string address)
{
    TELEPHONY_LOGI("BluetoothConnection::OnVirtualDeviceChanged action : %{public}d", action);
    Bluetooth::BluetoothRemoteDevice device(address);
    std::string deviceName = device.GetDeviceName();
    int state = (int32_t)Bluetooth::BTConnectState::DISCONNECTED;
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile != nullptr) {
        profile->GetDeviceState(device, state);
    } else {
        TELEPHONY_LOGE("profile is nullptr");
        return;
    }
    switch (action) {
        case VIRTUAL_DEVICE_ADD:
            DelayedSingleton<AudioDeviceManager>::GetInstance()->AddAudioDeviceList(
                address, AudioDeviceType::DEVICE_BLUETOOTH_SCO, deviceName);
            AddBtDevice(address, device);
            /** try to connect sco while new bluetooth device connected
             *  if connect sco successfully , should switch current audio device to bluetooth sco
             */
            break;
        case VIRTUAL_DEVICE_REMOVE:
            if (state != (int32_t)Bluetooth::BTConnectState::CONNECTED) {
                DelayedSingleton<AudioDeviceManager>::GetInstance()->RemoveAudioDeviceList(
                    address, AudioDeviceType::DEVICE_BLUETOOTH_SCO);
                RemoveBtDevice(address);
            }
            break;
        default:
            break;
    }
}

void SystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("SA:%{public}d is added!", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("added SA is invalid!");
        return;
    }
    if (systemAbilityId != BLUETOOTH_HOST_SYS_ABILITY_ID) {
        TELEPHONY_LOGE("added SA is not bluetooth service, ignored.");
        return;
    }

    DelayedSingleton<BluetoothConnection>::GetInstance()->RegisterObserver();
}

void SystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("SA:%{public}d is removed!", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("removed SA is invalid!");
        return;
    }
    if (systemAbilityId != BLUETOOTH_HOST_SYS_ABILITY_ID) {
        TELEPHONY_LOGE("removed SA is not bluetooth service, ignored.");
        return;
    }
    DelayedSingleton<BluetoothCallConnection>::GetInstance()->HfpDisConnectedEndBtCall();
    DelayedSingleton<BluetoothConnection>::GetInstance()->ResetBtConnection();
    std::shared_ptr<AudioDeviceManager> audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    audioDeviceManager->ResetBtAudioDevicesList();
    AudioDeviceType currentDeviceType = audioDeviceManager->GetCurrentAudioDevice();
    if (currentDeviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
        TELEPHONY_LOGI("BlueTooth SA removed, init audio device");
        audioDeviceManager->ProcessEvent(AudioEvent::INIT_AUDIO_DEVICE);
    }
}

std::string BluetoothConnection::GetWearBtHeadsetAddress()
{
    int32_t cod = DEFAULT_BT_VALUE;
    int32_t majorClass = DEFAULT_BT_VALUE;
    int32_t majorMinorClass = DEFAULT_BT_VALUE;
    std::lock_guard<ffrt::ffrt> lock(bluetoothMutex_);
    for (auto &[address, device] : mapConnectedBtDevices_) {
        device.GetDeviceProductType(cod, majorClass, majorMinorClass);
        TELEPHONY_LOGI("Device type majorClass: %{public}d, majorMinorClass: %{public}d.", majorClass, majorMinorClass);
        bool isWearing = Bluetooth::BluetoothAudioManager::GetInstance().IsDeviceWearing(device);
        bool isBtHeadset = (majorClass == Bluetooth::BluetoothDevice::MAJOR_AUDIO_VIDEO &&
                            (majorMinorClass == Bluetooth::BluetoothDevice::AUDIO_VIDEO_HEADPHONES ||
                             majorMinorClass == Bluetooth::BluetoothDevice::AUDIO_VIDEO_WEARABLE_HEADSET));
        if (isWearing && isBtHeadset) {
            return address;
        }
    }
    TELEPHONY_LOGI("not wearing bt headset");
    return "";
}

#endif
} // namespace Telephony
} // namespace OHOS
