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

#include "audio_control_manager.h"
#include "bluetooth_call_manager.h"
#include "telephony_log_wrapper.h"
#ifdef ABILITY_BLUETOOTH_SUPPORT
#include "bluetooth_host.h"

constexpr int32_t PHONE_NUMBER_TYPE = 0x81;
#endif

namespace OHOS {
namespace Telephony {
BluetoothConnection::BluetoothConnection() : connectedScoAddr_("") {}

BluetoothConnection::~BluetoothConnection()
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
    } else {
        profile->DeregisterObserver(this);
    }

    if (statusChangeListener_ != nullptr) {
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy != nullptr) {
            samgrProxy->UnSubscribeSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID, statusChangeListener_);
            statusChangeListener_ = nullptr;
        }
    }
    std::lock_guard<std::mutex> lock(bluetoothMutex_);
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
    std::string macAddress = "";
    for (auto device : devices) {
        if (profile->GetScoState(device) == (int32_t)Bluetooth::HfpScoConnectState::SCO_CONNECTED) {
            macAddress = device.GetDeviceAddr()
        }
    }
    SetConnectedScoAddr(device.GetDeviceAddr());
    DelayedSingleton<AudioDeviceManager>::GetInstance()->AddAudioDeviceList(macAddress, AudioDeviceType::DEVICE_BLUETOOTH_SCO);
    TELEPHONY_LOGI("BluetoothConnection init success!");
#endif
}

bool BluetoothConnection::ConnectBtSco()
{
    if (btScoState_.load() == BtScoState::SCO_STATE_CONNECTED) {
        TELEPHONY_LOGI("BluetoothConnection::ConnectBtSco bluetooth sco is already connected");
        return true;
    }
#ifdef ABILITY_BLUETOOTH_SUPPORT
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return false;
    }

    Bluetooth::BluetoothRemoteDevice *device = nullptr;
    std::string connectedScoAddr = GetConnectedScoAddr();
    if (!connectedScoAddr.empty()) {
        TELEPHONY_LOGI("connectedScoAddr_ is not empty");
        device = GetBtDevice(connectedScoAddr);
    } else {
        std::lock_guard<std::mutex> lock(bluetoothMutex_);
        if (!mapConnectedBtDevices_.empty()) {
            TELEPHONY_LOGI("mapConnectedBtDevices_ is not empty");
            device = &mapConnectedBtDevices_.begin()->second;
        } else {
            TELEPHONY_LOGE("device is invalid");
        }
    }

    if (device == nullptr) {
        TELEPHONY_LOGE("device is nullptr");
        return false;
    }
    if (profile->SetActiveDevice(*device) && profile->ConnectSco()) {
        return true;
    }
#endif
    TELEPHONY_LOGE("Connect Bluetooth Sco Fail !");
    return false;
}

bool BluetoothConnection::DisconnectBtSco()
{
    if (btScoState_.load() == BtScoState::SCO_STATE_DISCONNECTED) {
        TELEPHONY_LOGI("bluetooth sco is already disconnected");
        return true;
    }
#ifdef ABILITY_BLUETOOTH_SUPPORT
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return false;
    }
    return profile->DisconnectSco();
#endif
    TELEPHONY_LOGE("Disconnect Bluetooth Sco Fail !");
    return false;
}

#ifdef ABILITY_BLUETOOTH_SUPPORT
bool BluetoothConnection::ConnectBtSco(const std::string &bluetoothAddress)
{
    if (bluetoothAddress == GetConnectedScoAddr() && btScoState_.load() == BtScoState::SCO_STATE_CONNECTED) {
        TELEPHONY_LOGI("bluetooth sco is already connected");
        return true;
    }

    Bluetooth::BluetoothRemoteDevice device =
        Bluetooth::BluetoothHost::GetDefaultHost().GetRemoteDevice(bluetoothAddress, Bluetooth::BT_TRANSPORT_BREDR);
    AddBtDevice(bluetoothAddress, device);

    return ConnectBtSco(device);
}

bool BluetoothConnection::ConnectBtSco(const Bluetooth::BluetoothRemoteDevice &device)
{
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return false;
    }

    if (profile->SetActiveDevice(device) && profile->ConnectSco()) {
        SetConnectedScoAddr(device.GetDeviceAddr());
        btScoState_.store(BtScoState::SCO_STATE_CONNECTED);
        TELEPHONY_LOGI("connect bluetooth sco success");
        return true;
    }
    TELEPHONY_LOGE("connect bluetooth sco fail");
    return false;
}

bool BluetoothConnection::DisconnectBtSco(const Bluetooth::BluetoothRemoteDevice &device)
{
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return false;
    }

    if (profile->Disconnect(device)) {
        SetConnectedScoAddr("");
        btScoState_.store(BtScoState::SCO_STATE_DISCONNECTED);
        TELEPHONY_LOGI("disconnect bluetooth sco success");
        return true;
    }

    TELEPHONY_LOGE("Disconnect Bluetooth Sco Fail !");
    return false;
}

bool BluetoothConnection::IsBtAvailble()
{
    std::lock_guard<std::mutex> lock(bluetoothMutex_);
    if (mapConnectedBtDevices_.empty()) {
        TELEPHONY_LOGE("mapConnectedBtDevices_ is empty");
        return false;
    }

    return true;
}
#endif

bool BluetoothConnection::IsBtScoConnected()
{
    return btScoState_.load() == BtScoState::SCO_STATE_CONNECTED;
}

void BluetoothConnection::SetBtScoState(BtScoState state)
{
    btScoState_.store(state);
}

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

BtScoState BluetoothConnection::GetBtScoState()
{
    BtScoState btScoState = btScoState_.load();
    TELEPHONY_LOGI("current bluetooth sco state : %{public}d", btScoState);
    return btScoState;
}

bool BluetoothConnection::IsAudioActivated()
{
    return DelayedSingleton<AudioControlManager>::GetInstance()->IsAudioActivated();
}

std::string BluetoothConnection::GetConnectedScoAddr()
{
    std::lock_guard<std::mutex> lock(scoAddrMutex_);
    return connectedScoAddr_;
}

#ifdef ABILITY_BLUETOOTH_SUPPORT
void BluetoothConnection::SetConnectedScoAddr(std::string connectedScoAddr)
{
    std::lock_guard<std::mutex> lock(scoAddrMutex_);
    connectedScoAddr_ = connectedScoAddr;
}

void BluetoothConnection::ResetBtConnection()
{
    SetConnectedScoAddr("");
    btScoState_.store(BtScoState::SCO_STATE_DISCONNECTED);
    std::lock_guard<std::mutex> lock(bluetoothMutex_);
    mapConnectedBtDevices_.clear();
}

void BluetoothConnection::RegisterObserver()
{
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("BluetoothConnection RegisterObserver fail!");
        return;
    }

    profile->RegisterObserver(this);
}

void BluetoothConnection::OnScoStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t state)
{
    TELEPHONY_LOGI("BluetoothConnection::OnScoStateChanged state : %{public}d", state);
    switch (state) {
        case (int32_t)Bluetooth::HfpScoConnectState::SCO_CONNECTED:
            SetConnectedScoAddr(device.GetDeviceAddr());
            btScoState_.store(BtScoState::SCO_STATE_CONNECTED);
            break;
        case (int32_t)Bluetooth::HfpScoConnectState::SCO_DISCONNECTED:
            if (device.GetDeviceAddr() == GetConnectedScoAddr()) {
                SetConnectedScoAddr("");
                btScoState_.store(BtScoState::SCO_STATE_DISCONNECTED);
            }
            break;
        default:
            break;
    }
}

void BluetoothConnection::AddBtDevice(const std::string &address, Bluetooth::BluetoothRemoteDevice device)
{
    std::lock_guard<std::mutex> lock(bluetoothMutex_);
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
    std::lock_guard<std::mutex> lock(bluetoothMutex_);
    if (mapConnectedBtDevices_.count(address) > 0) {
        mapConnectedBtDevices_.erase(address);
        TELEPHONY_LOGI("RemoveBtDevice success");
    } else {
        TELEPHONY_LOGE("device is not existenced");
    }
}

Bluetooth::BluetoothRemoteDevice *BluetoothConnection::GetBtDevice(const std::string &address)
{
    std::lock_guard<std::mutex> lock(bluetoothMutex_);
    if (mapConnectedBtDevices_.count(address) > 0) {
        auto iter = mapConnectedBtDevices_.find(address);
        if (iter != mapConnectedBtDevices_.end()) {
            return &iter->second;
        }
    }
    TELEPHONY_LOGE("device is not existenced");
    return nullptr;
}

void BluetoothConnection::OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t state)
{
    TELEPHONY_LOGI("BluetoothConnection::OnConnectionStateChanged state : %{public}d", state);
    std::string macAddress = device.GetDeviceAddr();
    switch (state) {
        case (int32_t)Bluetooth::BTConnectState::CONNECTED:
            DelayedSingleton<AudioDeviceManager>::GetInstance()->AddAudioDeviceList(
                macAddress, AudioDeviceType::DEVICE_BLUETOOTH_SCO);
            AddBtDevice(macAddress, device);
            /** try to connect sco while new bluetooth device connected
             *  if connect sco successfully , should switch current audio device to bluetooth sco
             */
            if (BluetoothConnection::GetBtScoState() == BtScoState::SCO_STATE_DISCONNECTED && IsAudioActivated()) {
                ConnectBtSco(device);
            }
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

    DelayedSingleton<BluetoothConnection>::GetInstance()->ResetBtConnection();
    std::shared_ptr<AudioDeviceManager> audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    audioDeviceManager->ResetBtAudioDevicesList();
    audioDeviceManager->ProcessEvent(AudioEvent::INIT_AUDIO_DEVICE);
}
#endif
} // namespace Telephony
} // namespace OHOS
