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
const std::string BluetoothConnection::EVENT_BLUETOOTH_SCO_CONNECTED = "usual.event.BLUETOOTH_SCO_CONNECTED";
const std::string BluetoothConnection::EVENT_BLUETOOTH_SCO_DISCONNECTED = "usual.event.BLUETOOTH_SCO_DISCONNECTED";

BluetoothConnection::BluetoothConnection() : connectedScoAddr_("") {}

BluetoothConnection::~BluetoothConnection()
{
    connectedBtDevices_.clear();
}

bool BluetoothConnection::ConnectBtSco()
{
    if (btScoState_ == BtScoState::SCO_STATE_CONNECTED) {
        TELEPHONY_LOGI("bluetooth sco is already connected");
        return true;
    }
    // connect recently connected sco address firstly
    if (!connectedScoAddr_.empty() && ConnectBtSco(connectedScoAddr_)) {
        return true;
    }
    // if recently sco address connect failed , try other connected devices
    if (connectedBtDevices_.size() > 0) {
        for (std::string address : connectedBtDevices_) {
            if (ConnectBtSco(address)) {
                return true;
            }
        }
    }
    return false;
}

bool BluetoothConnection::DisconnectBtSco()
{
    if (btScoState_ == BtScoState::SCO_STATE_DISCONNECTED) {
        TELEPHONY_LOGI("bluetooth sco is already disconnected");
        return true;
    }
    if (connectedScoAddr_.empty()) {
        TELEPHONY_LOGE("no bluetooth sco device is connected , disconnect failed");
        return false;
    }
    return DisconnectBtSco(connectedScoAddr_);
}

bool BluetoothConnection::ConnectBtSco(const std::string &macAddress)
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    bool result = BluetoothMgr::BluetoothMgrClient::GetInstance()->ConnectBtSco(macAddress);
    if (result) {
        connectedScoAddr_ = macAddress;
        btScoState_ = BtScoState::SCO_STATE_CONNECTED;
        TELEPHONY_LOGI("connect bluetooth sco success");
    } else {
        TELEPHONY_LOGE("connect bluetooth sco failed");
    }
    return result;
#endif
    return true;
}

bool BluetoothConnection::DisconnectBtSco(const std::string &macAddress)
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    bool result = BluetoothMgr::BluetoothMgrClient::GetInstance()->DisconnectBtSco(macAddress);
    if (result) {
        connectedScoAddr_ = "";
        btScoState_ = BtScoState::SCO_STATE_DISCONNECTED;
        TELEPHONY_LOGI("disconnect bluetooth sco success");
    } else {
        TELEPHONY_LOGE("disconnect bluetooth sco failed");
    }
    return result;
#endif
    return true;
}

bool BluetoothConnection::IsBtScoConnected()
{
    return btScoState_ == BtScoState::SCO_STATE_CONNECTED;
}

void BluetoothConnection::SetBtScoState(BtScoState state)
{
    btScoState_ = state;
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
void BluetoothConnection::OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state)
{
    std::string macAddress = device.GetDeviceAddr();
    switch (state) {
        case BLUETOOTH_DEVICE_CONNECTED:
            if (connectedBtDevices_.count(macAddress) == 0) {
                connectedBtDevices_.insert(macAddress);
                /** try to connect sco while new bluetooth device connected
                 *  if connect sco successfully , should switch current audio device to bluetooth sco
                 */
                if (BluetoothCallManager::GetBtScoState() == BtScoState::SCO_STATE_DISCONNECTED &&
                    IsAudioActivated() && ConnectBtSco(macAddress)) {
                    connectedScoAddr_ = macAddress;
                    DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                        AudioEvent::BLUETOOTH_SCO_AVAILABLE);
                }
            }
            break;
        case BLUETOOTH_DEVICE_DISCONNECTED:
            if (connectedBtDevices_.count(macAddress) > 0) {
                connectedBtDevices_.erase(macAddress);
            }
            break;
        default:
            break;
    }
}

void BluetoothConnection::OnScoStateChanged(const BluetoothRemoteDevice &device, int state)
{
    switch (state) {
        case BLUETOOTH_SCO_CONNECTED:
            if (connectedScoAddr_.empty()) {
                connectedScoAddr_ = device.GetDeviceAddr();
                BluetoothConnection::SetBtScoState(BtScoState::SCO_STATE_CONNECTED);
            }
            break;
        case BLUETOOTH_SCO_DISCONNECTED:
            if (device.GetDeviceAddr() == connectedScoAddr_) {
                connectedScoAddr_ = "";
                BluetoothConnection::SetBtScoState(BtScoState::SCO_STATE_DISCONNECTED);
                DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(
                    AudioEvent::BLUETOOTH_SCO_UNAVAILABLE);
            }
            break;
        default:
            break;
    }
}
#endif
} // namespace Telephony
} // namespace OHOS