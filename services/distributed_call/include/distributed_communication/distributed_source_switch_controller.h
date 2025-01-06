/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_DISTRIBUTED_COMMUNICATION_SOURCE_SWITCH_CONTROLLER_H
#define TELEPHONY_DISTRIBUTED_COMMUNICATION_SOURCE_SWITCH_CONTROLLER_H

#ifdef ABILITY_BLUETOOTH_SUPPORT
#include "bluetooth_hfp_ag.h"
#endif

#include "distributed_device_switch_controller.h"

namespace OHOS {
namespace Telephony {
#ifdef ABILITY_BLUETOOTH_SUPPORT
class DcCallSourceHfpListener : public Bluetooth::HandsFreeAudioGatewayObserver {
public:
    DcCallSourceHfpListener() = default;
    ~DcCallSourceHfpListener() override = default;
    void OnHfpStackChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t action) override;

private:
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(DcCallSourceHfpListener);
    bool IsNeedSwitchToSource(const Bluetooth::BluetoothRemoteDevice &device, int32_t action);
};
#endif

class DistributedSourceSwitchController : public DistributedDeviceSwitchController {
public:
    DistributedSourceSwitchController() = default;
    ~DistributedSourceSwitchController() override = default;
    void OnDeviceOnline(const std::string &devId, const std::string &devName, AudioDeviceType devType) override;
    void OnDeviceOffline(const std::string &devId, const std::string &devName, AudioDeviceType devType) override;
    void OnDistributedAudioDeviceChange(const std::string &devId, const std::string &devName,
        AudioDeviceType devType, int32_t devRole) override;
    void OnRemoveSystemAbility() override;

    bool SwitchDevice(const std::string &devId, int32_t direction) override;

private:
    std::string GetDevAddress(const std::string &devId, const std::string &devName);

private:
#ifdef ABILITY_BLUETOOTH_SUPPORT
    std::shared_ptr<DcCallSourceHfpListener> hfpListener_{nullptr};
#endif
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_DISTRIBUTED_COMMUNICATION_SOURCE_SWITCH_CONTROLLER_H
