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
    mapConnectedBtDevices_.clear();
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
    } else {
        profile->DeregisterObserver(this);
    }
#endif
}

void BluetoothConnection::Init()
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    Bluetooth::HandsFreeAudioGateway *profile = Bluetooth::HandsFreeAudioGateway::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("BluetoothConnection init fail!");
        return;
    }

    profile->RegisterObserver(this);
    TELEPHONY_LOGI("BluetoothConnection init success!");
#endif
}

bool BluetoothConnection::ConnectBtSco()
{
    if (btScoState_ == BtScoState::SCO_STATE_CONNECTED) {
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
    if (!connectedScoAddr_.empty()) {
        TELEPHONY_LOGI("connectedScoAddr_ is not empty");
        device = GetBtDevice(connectedScoAddr_);
    } else if (!mapConnectedBtDevices_.empty()) {
        TELEPHONY_LOGI("mapConnectedBtDevices_ is not empty");
        device = &mapConnectedBtDevices_.begin()->second;
    } else {
        TELEPHONY_LOGE("device is invalid");
    }

    if (device == nullptr) {
        TELEPHONY_LOGE("device is nullptr");
        return false;
    }
    if (profile->SetActiveDevice(*device) && profile->ConnectSco()) {
        btScoState_ = BtScoState::SCO_STATE_CONNECTED;
        connectedScoAddr_ = device->GetDeviceAddr();
        DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(AudioEvent::BLUETOOTH_SCO_CONNECTED);
        TELEPHONY_LOGI("bluetooth sco connected successfully.");
        return true;
    }
#endif
    TELEPHONY_LOGE("Connect Bluetooth Sco Fail !");
    return false;
}

bool BluetoothConnection::DisconnectBtSco()
{
    if (btScoState_ == BtScoState::SCO_STATE_DISCONNECTED) {
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
    if (bluetoothAddress == connectedScoAddr_ && btScoState_ == BtScoState::SCO_STATE_CONNECTED) {
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
        connectedScoAddr_ = device.GetDeviceAddr();
        btScoState_ = BtScoState::SCO_STATE_CONNECTED;
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
        connectedScoAddr_ = "";
        btScoState_ = BtScoState::SCO_STATE_DISCONNECTED;
        TELEPHONY_LOGI("disconnect bluetooth sco success");
        return true;
    }

    TELEPHONY_LOGE("Disconnect Bluetooth Sco Fail !");
    return false;
}

bool BluetoothConnection::IsBtAvailble()
{
    if (mapConnectedBtDevices_.empty()) {
        TELEPHONY_LOGE("mapConnectedBtDevices_ is empty");
        return false;
    }

    return true;
}
#endif

bool BluetoothConnection::IsBtScoConnected()
{
    return btScoState_ == BtScoState::SCO_STATE_CONNECTED;
}

void BluetoothConnection::SetBtScoState(BtScoState state)
{
    btScoState_ = state;
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
    TELEPHONY_LOGI("current bluetooth sco state : %{public}d", btScoState_);
    return btScoState_;
}

bool BluetoothConnection::IsAudioActivated()
{
    return DelayedSingleton<AudioControlManager>::GetInstance()->IsAudioActivated();
}

#ifdef ABILITY_BLUETOOTH_SUPPORT
void BluetoothConnection::OnScoStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t state)
{
    TELEPHONY_LOGI("BluetoothConnection::OnScoStateChanged state : %{public}d", state);
    switch (state) {
        case (int32_t)Bluetooth::HfpScoConnectState::SCO_CONNECTED:
            connectedScoAddr_ = device.GetDeviceAddr();
            btScoState_ = BtScoState::SCO_STATE_CONNECTED;
            DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(AudioEvent::BLUETOOTH_SCO_CONNECTED);
            break;
        case (int32_t)Bluetooth::HfpScoConnectState::SCO_DISCONNECTED:
            if (device.GetDeviceAddr() == connectedScoAddr_) {
                connectedScoAddr_ = "";
                btScoState_ = BtScoState::SCO_STATE_DISCONNECTED;
                DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                    AudioEvent::BLUETOOTH_SCO_DISCONNECTED);
            }
            break;
        default:
            break;
    }
}

void BluetoothConnection::AddBtDevice(const std::string &address, Bluetooth::BluetoothRemoteDevice device)
{
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
    if (mapConnectedBtDevices_.count(address) > 0) {
        mapConnectedBtDevices_.erase(address);
        TELEPHONY_LOGI("RemoveBtDevice success");
    } else {
        TELEPHONY_LOGE("device is not existenced");
    }
}

Bluetooth::BluetoothRemoteDevice *BluetoothConnection::GetBtDevice(const std::string &address)
{
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
            AddBtDevice(macAddress, device);
            /** try to connect sco while new bluetooth device connected
             *  if connect sco successfully , should switch current audio device to bluetooth sco
             */
            if (BluetoothConnection::GetBtScoState() == BtScoState::SCO_STATE_DISCONNECTED && IsAudioActivated()) {
                ConnectBtSco(device);
            }
            break;
        case (int32_t)Bluetooth::BTConnectState::DISCONNECTED:
            RemoveBtDevice(macAddress);
            break;
        default:
            break;
    }
}
#endif
} // namespace Telephony
} // namespace OHOS
