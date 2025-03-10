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

#include "interoperable_device_observer.h"
#include "interoperable_communication_manager.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"

namespace OHOS {
namespace Telephony {
InteroperableDeviceObserver::InteroperableDeviceObserver()
{
}
 
InteroperableDeviceObserver::~InteroperableDeviceObserver()
{
}
 
void InteroperableDeviceObserver::Init()
{
    auto dmInitCallback = std::make_shared<DeviceInitCallback>();
    DistributedHardware::DeviceManager::GetInstance().InitDeviceManager(PACKAGE_NAME, dmInitCallback);
    auto stateCallback = std::make_shared<DmStateCallback>();
    int32_t ret = DistributedHardware::DeviceManager::
        GetInstance().RegisterDevStateCallback(PACKAGE_NAME, "", stateCallback);
    if (ret != 0) {
        TELEPHONY_LOGE("RegisterDevStateCallback fail");
    }
}
 
void InteroperableDeviceObserver::RegisterDevStatusCallback(
    const std::shared_ptr<IInteroperableDeviceStateCallback> &callback)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (callback == nullptr) {
        TELEPHONY_LOGE("reg dev status callback null");
        return;
    }
    auto iter = std::find(callbacks_.begin(), callbacks_.end(), callback);
    if (iter != callbacks_.end()) {
        TELEPHONY_LOGI("callback already reg");
        return;
    }
    callbacks_.push_back(callback);
    TELEPHONY_LOGI("reg dev status callback");
}
 
void InteroperableDeviceObserver::UnRegisterDevStatusCallback(
    const std::shared_ptr<IInteroperableDeviceStateCallback> &callback)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto iter = std::find(callbacks_.begin(), callbacks_.end(), callback);
    if (iter != callbacks_.end()) {
        callbacks_.erase(iter);
    }
    TELEPHONY_LOGI("un-reg dev status callback");
}
 
void InteroperableDeviceObserver::OnDeviceOnline(const std::string &networkId, const std::string &devName,
                                                 uint16_t devType)
{
    std::list<std::shared_ptr<IInteroperableDeviceStateCallback>> callbacks;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        callbacks = callbacks_;
    }
    for (auto& callback : callbacks) {
        if (callback != nullptr) {
            callback->OnDeviceOnline(networkId, devName, devType);
        }
    }
}
 
void InteroperableDeviceObserver::OnDeviceOffline(const std::string &networkId, const std::string &devName,
                                                  uint16_t devType)
{
    std::list<std::shared_ptr<IInteroperableDeviceStateCallback>> callbacks;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        callbacks = callbacks_;
    }
    for (auto& callback : callbacks) {
        if (callback != nullptr) {
            callback->OnDeviceOffline(networkId, devName, devType);
        }
    }
}
 
void DmStateCallback::OnDeviceOnline(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    TELEPHONY_LOGI("DmStateCallback receive device online");
    DelayedSingleton<InteroperableCommunicationManager>::GetInstance()->OnDeviceOnline(deviceInfo);
}
 
void DmStateCallback::OnDeviceOffline(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    TELEPHONY_LOGI("DmStateCallback receive device offline");
    DelayedSingleton<InteroperableCommunicationManager>::GetInstance()->OnDeviceOffline(deviceInfo);
}
}
}