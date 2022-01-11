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

#ifndef BLUETOOTH_CONNECTION_H
#define BLUETOOTH_CONNECTION_H

#include <set>
#include <string>
#include <memory>

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

class BluetoothConnection {
public:
    BluetoothConnection();
    ~BluetoothConnection();
    bool ConnectBtSco();
    bool DisconnectBtSco();
    bool IsBtScoConnected();
    static const std::string EVENT_BLUETOOTH_SCO_CONNECTED;
    static const std::string EVENT_BLUETOOTH_SCO_DISCONNECTED;
    static BtScoState GetBtScoState();
    static void SetBtScoState(BtScoState state);
#ifdef ABILITY_BLUETOOTH_SUPPORT
    void OnScoStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t state) override;
    void OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t state) override;
#endif

private:
    bool IsAudioActivated();
    static BtScoState btScoState_;
    std::string connectedScoAddr_;
    std::set<std::string> connectedBtDevices_;
#ifdef ABILITY_BLUETOOTH_SUPPORT
    bool ConnectBtSco(const Bluetooth::BluetoothRemoteDevice &device);
    bool DisconnectBtSco(const Bluetooth::BluetoothRemoteDevice &device);
#endif
};
} // namespace Telephony
} // namespace OHOS
#endif