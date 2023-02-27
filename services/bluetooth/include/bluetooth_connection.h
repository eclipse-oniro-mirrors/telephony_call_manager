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

#ifndef TELEPHONY_BLUETOOTH_CONNECTION_H
#define TELEPHONY_BLUETOOTH_CONNECTION_H

#include <memory>
#include <string>

#include "singleton.h"
#include "unordered_map"

#ifdef ABILITY_BLUETOOTH_SUPPORT
#include "bluetooth_hfp_ag.h"
#endif

namespace OHOS {
namespace Telephony {
enum BtScoState {
    SCO_STATE_UNKNOWN = 0,
    SCO_STATE_CONNECTED,
    SCO_STATE_DISCONNECTED,
    SCO_STATE_PENDING,
};

#ifdef ABILITY_BLUETOOTH_SUPPORT
class BluetoothConnection : public OHOS::Bluetooth::HandsFreeAudioGatewayObserver {
#else
class BluetoothConnection {
#endif
    DECLARE_DELAYED_SINGLETON(BluetoothConnection)
public:
    void Init();
    bool ConnectBtSco();
    bool ConnectBtSco(const std::string &bluetoothAddress);
    bool DisconnectBtSco();
    bool IsBtScoConnected();
    BtScoState GetBtScoState();
    void SetBtScoState(BtScoState state);
    int32_t SendBtCallState(int32_t numActive, int32_t numHeld, int32_t callState, const std::string &number);
    void RemoveBtDevice(const std::string &address);
    bool IsBtAvailble();
    std::string GetConnectedScoAddr();

#ifdef ABILITY_BLUETOOTH_SUPPORT
    void OnScoStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t state) override;
    void OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t state) override;
    Bluetooth::BluetoothRemoteDevice *GetBtDevice(const std::string &address);
    void AddBtDevice(const std::string &address, Bluetooth::BluetoothRemoteDevice device);
#endif

private:
    bool IsAudioActivated();
    BtScoState btScoState_ = BtScoState::SCO_STATE_DISCONNECTED;
    std::string connectedScoAddr_;

#ifdef ABILITY_BLUETOOTH_SUPPORT
    bool ConnectBtSco(const Bluetooth::BluetoothRemoteDevice &device);
    bool DisconnectBtSco(const Bluetooth::BluetoothRemoteDevice &device);

private:
    std::unordered_map<std::string, Bluetooth::BluetoothRemoteDevice> mapConnectedBtDevices_;
#endif
};
} // namespace Telephony
} // namespace OHOS
#endif
