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

#include <atomic>
#include <memory>
#include <string>

#include "singleton.h"
#include "unordered_map"

#ifdef ABILITY_BLUETOOTH_SUPPORT
#include "bluetooth_hfp_ag.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "system_ability_status_change_stub.h"
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
class BluetoothConnection : public OHOS::Bluetooth::HandsFreeAudioGatewayObserver,
    public std::enable_shared_from_this<BluetoothConnection> {
#else
class BluetoothConnection {
#endif
    DECLARE_DELAYED_SINGLETON(BluetoothConnection)
public:
    void Init();
    int32_t SendBtCallState(int32_t numActive, int32_t numHeld, int32_t callState, const std::string &number);
    int32_t SendCallDetailsChange(int32_t callId, int32_t callState);
    void RemoveBtDevice(const std::string &address);
    bool IsBtAvailble();
    void ResetBtConnection();
    void RegisterObserver();

#ifdef ABILITY_BLUETOOTH_SUPPORT
    void OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device,
        int32_t state, int32_t cause) override;
    Bluetooth::BluetoothRemoteDevice *GetBtDevice(const std::string &address);
    void AddBtDevice(const std::string &address, Bluetooth::BluetoothRemoteDevice device);
    std::string GetWearBtHeadsetAddress();
#endif

private:
    bool IsAudioActivated();
    std::atomic<BtScoState> btScoState_{BtScoState::SCO_STATE_DISCONNECTED};
    std::mutex scoAddrMutex_;
    std::mutex scoNameMutex_;
    std::string connectedScoAddr_;
    std::string connectedScoName_;

#ifdef ABILITY_BLUETOOTH_SUPPORT
private:
    sptr<ISystemAbilityStatusChange> statusChangeListener_ = nullptr;
    std::mutex bluetoothMutex_;
    std::unordered_map<std::string, Bluetooth::BluetoothRemoteDevice> mapConnectedBtDevices_;
#endif
};

#ifdef ABILITY_BLUETOOTH_SUPPORT
class SystemAbilityListener : public SystemAbilityStatusChangeStub {
public:
    SystemAbilityListener() = default;
    ~SystemAbilityListener() = default;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
};
#endif
} // namespace Telephony
} // namespace OHOS
#endif
