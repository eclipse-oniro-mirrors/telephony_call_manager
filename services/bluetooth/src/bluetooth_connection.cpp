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

#include "telephony_log_wrapper.h"

#include "audio_control_manager.h"
#include "bluetooth_call_manager.h"

namespace OHOS {
namespace Telephony {
BtScoState BluetoothConnection::btScoState_ = BtScoState::SCO_STATE_DISCONNECTED;

BluetoothConnection::BluetoothConnection() : connectedScoAddr_("") {}

BluetoothConnection::~BluetoothConnection()
{
    connectedBtDevices_.clear();
#ifdef ABILITY_BLUETOOTH_SUPPORT
    Bluetooth::HandsFreeAudioGateway::GetProfile()->DeregisterObserver(this);
#endif
}

void BluetoothConnection::Init()
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    TELEPHONY_LOGI("BluetoothConnection init success!");
    Bluetooth::HandsFreeAudioGateway::GetProfile()->RegisterObserver(this);
#endif  
  
}

bool BluetoothConnection::ConnectBtSco()
{
    if (btScoState_ == BtScoState::SCO_STATE_CONNECTED) {
        TELEPHONY_LOGI("bluetooth sco is already connected");
        return true;
    }
#ifdef ABILITY_BLUETOOTH_SUPPORT
    return Bluetooth::HandsFreeAudioGateway::GetProfile()->ConnectSco();
#endif
    return true;
}

bool BluetoothConnection::DisconnectBtSco()
{
    if (btScoState_ == BtScoState::SCO_STATE_DISCONNECTED) {
        TELEPHONY_LOGI("bluetooth sco is already disconnected");
        return true;
    }
#ifdef ABILITY_BLUETOOTH_SUPPORT
    return Bluetooth::HandsFreeAudioGateway::GetProfile()->DisconnectSco();
#endif
    return true;
}

#ifdef ABILITY_BLUETOOTH_SUPPORT
bool BluetoothConnection::ConnectBtSco(const Bluetooth::BluetoothRemoteDevice &device)
{
    bool result = Bluetooth::HandsFreeAudioGateway::GetProfile()->Connect(device);
    if (result) {
        connectedScoAddr_ = device.GetDeviceAddr();
        btScoState_ = BtScoState::SCO_STATE_CONNECTED;
        TELEPHONY_LOGI("connect bluetooth sco success");
    } else {
        TELEPHONY_LOGE("connect bluetooth sco failed");
    }
    return result;
}

bool BluetoothConnection::DisconnectBtSco(const Bluetooth::BluetoothRemoteDevice &device)
{
    bool result = Bluetooth::HandsFreeAudioGateway::GetProfile()->Disconnect(device);
    if (result) {
        connectedScoAddr_ = "";
        btScoState_ = BtScoState::SCO_STATE_DISCONNECTED;
        TELEPHONY_LOGI("disconnect bluetooth sco success");
    } else {
        TELEPHONY_LOGE("disconnect bluetooth sco failed");
    }
    return result;
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
    std::string nickName = "";
    constexpr int32_t numberType = 0x81;
    Bluetooth::HandsFreeAudioGateway::GetProfile()->
        PhoneStateChanged(numActive, numHeld, callState, number, numberType, nickName);
#endif
    TELEPHONY_LOGI("PhoneStateChanged,numActive:%{public}d,numHeld:%{public}d,callState:%{public}d",
        numActive, numHeld, callState);
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
    switch (state) {
        case (int32_t)Bluetooth::BTConnectState::CONNECTED:
            if (connectedScoAddr_.empty()) {
                connectedScoAddr_ = device.GetDeviceAddr();
                BluetoothConnection::SetBtScoState(BtScoState::SCO_STATE_CONNECTED);
            }
            break;
        case (int32_t)Bluetooth::BTConnectState::DISCONNECTED:
            if (device.GetDeviceAddr() == connectedScoAddr_) {
                connectedScoAddr_ = "";
                BluetoothConnection::SetBtScoState(BtScoState::SCO_STATE_DISCONNECTED);
                DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                    AudioEvent::BLUETOOTH_SCO_DISCONNECTED);
            }
            break;
        default:
            break;
    }
}

void BluetoothConnection::OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t state)
{
    std::string macAddress = device.GetDeviceAddr();
    switch (state) {
        case (int32_t)Bluetooth::BTConnectState::CONNECTED:
            if (connectedBtDevices_.count(macAddress) == 0) {
                connectedBtDevices_.insert(macAddress);
                /** try to connect sco while new bluetooth device connected
                 *  if connect sco successfully , should switch current audio device to bluetooth sco
                 */
                if (BluetoothConnection::GetBtScoState() == BtScoState::SCO_STATE_DISCONNECTED &&
                    IsAudioActivated() && ConnectBtSco(device)) {
                    connectedScoAddr_ = macAddress;
                    DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                        AudioEvent::BLUETOOTH_SCO_CONNECTED);
                }
            }
            break;
        case (int32_t)Bluetooth::BTConnectState::DISCONNECTED:
            if (connectedBtDevices_.count(macAddress) > 0) {
                connectedBtDevices_.erase(macAddress);
            }
            break;
        default:
            break;
    }
}
#endif
} // namespace Telephony
} // namespace OHOS