/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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
 
#ifndef INTEROPERABLE_DEVICE_OBSERVER_H
#define INTEROPERABLE_DEVICE_OBSERVER_H

#include <list>
#include <string>
#include <unordered_set>
#include <shared_mutex>
#include "singleton.h"
#include "ffrt.h"
#include "i_interoperable_device_state_callback.h"
#include "device_manager.h"

namespace OHOS {
namespace Telephony {
constexpr const char* PACKAGE_NAME = "ohos.telephony.callmanager";
 
class DeviceInitCallback : public DistributedHardware::DmInitCallback {
public:
    DeviceInitCallback() = default;
    virtual ~DeviceInitCallback() = default;
    void OnRemoteDied() override {}
};
 
class DmStateCallback : public DistributedHardware::DeviceStateCallback {
public:
    void OnDeviceOnline(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceOffline(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceChanged(const DistributedHardware::DmDeviceInfo &deviceInfo) override {}
    void OnDeviceReady(const DistributedHardware::DmDeviceInfo &deviceInfo) override {}
};
 
class InteroperableDeviceObserver {
    DECLARE_DELAYED_SINGLETON(InteroperableDeviceObserver)
 
public:
    void Init();
    void RegisterDevStatusCallback(const std::shared_ptr<IInteroperableDeviceStateCallback> &callback);
    void UnRegisterDevStatusCallback(const std::shared_ptr<IInteroperableDeviceStateCallback> &callback);
    void OnDeviceOnline(const std::string &networkId, const std::string &devName, uint16_t devType);
    void OnDeviceOffline(const std::string &networkId, const std::string &devName, uint16_t devType);
 
private:
    ffrt::mutex mutex_{};
    std::list<std::shared_ptr<IInteroperableDeviceStateCallback>> callbacks_{};
};
}
}
#endif // INTEROPERABLE_DEVICE_OBSERVER_H