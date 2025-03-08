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

#include "interoperable_communication_manager.h"
#include "interoperable_server_manager.h"
#include "interoperable_client_manager.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
InteroperableCommunicationManager::InteroperableCommunicationManager()
{
    devObserver_ = DelayedSingleton<InteroperableDeviceObserver>::GetInstance();
}
 
InteroperableCommunicationManager::~InteroperableCommunicationManager()
{
    devObserver_ = nullptr;
}
 
void InteroperableCommunicationManager::OnDeviceOnline(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    TELEPHONY_LOGI("Interoperable device online, networkId = %{public}s, devrole = %{public}d",
        deviceInfo.networkId, deviceInfo.deviceTypeId);
    std::string networkId = deviceInfo.networkId;
    std::string devName = deviceInfo.deviceName;
    uint16_t devType = deviceInfo.deviceTypeId;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        if (devType == 0x0E) {                           // 0x0E手机
            role_ = InteroperableRole::OTHERS;
        } else if (devType == 0x6D || devType == 0x83) { // 0x6D手表，0x83车机
            role_ = InteroperableRole::PHONE;
        } else {
            return;
        }
        auto iter = std::find(peerDevices_.begin(), peerDevices_.end(), networkId);
        if (iter == peerDevices_.end()) {
            peerDevices_.emplace_back(networkId);
        } else {
            TELEPHONY_LOGE("device already online");
            return;
        }
        if (dataController_ == nullptr) {
            if (role_ == InteroperableRole::OTHERS) {
                dataController_ = std::make_shared<InteroperableClientManager>();
            } else {
                dataController_ = std::make_shared<InteroperableServerManager>();
            }
        }
    }
    devObserver_->RegisterDevStatusCallback(dataController_);
    devObserver_->OnDeviceOnline(networkId, devName, devType);
}
 
void InteroperableCommunicationManager::OnDeviceOffline(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    TELEPHONY_LOGI("DSHT Interoperable device offline, networkId = %{public}s, devrole = %{public}d",
        deviceInfo.networkId, deviceInfo.deviceTypeId);
    if (devObserver_ == nullptr) {
        return;
    }
    std::string networkId = deviceInfo.networkId;
    std::string devName = deviceInfo.deviceName;
    uint16_t devType = deviceInfo.deviceTypeId;
    devObserver_->OnDeviceOffline(networkId, devName, devType);
    devObserver_->UnRegisterDevStatusCallback(dataController_);

    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto iter = std::find(peerDevices_.begin(), peerDevices_.end(), networkId);
    if (iter != peerDevices_.end()) {
        peerDevices_.erase(iter);
    }
    if (!peerDevices_.empty()) {
        TELEPHONY_LOGI("device list not empty");
        return;
    }
 
    role_ = InteroperableRole::UNKNOWN;
    if (dataController_ != nullptr) {
        dataController_.reset();
        dataController_ = nullptr;
    }
}
 
void InteroperableCommunicationManager::SetMuted(bool isMute)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (dataController_ == nullptr) {
        return;
    }
    TELEPHONY_LOGI("InteroperableCommunicationManager set muted %{public}d", isMute);
    dataController_->SetMuted(isMute);
}
 
void InteroperableCommunicationManager::MuteRinger()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (dataController_ == nullptr) {
        return;
    }
    TELEPHONY_LOGI("InteroperableCommunicationManager mute ringer");
    dataController_->MuteRinger();
}
 
void InteroperableCommunicationManager::NewCallCreated(sptr<CallBase> &call)
{
    if (call == nullptr || role_ == InteroperableRole::PHONE) {
        TELEPHONY_LOGE("no found call or phone recv call");
        return;
    }
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (dataController_ != nullptr) {
        if (peerDevices_.empty()) {
            TELEPHONY_LOGE("call created but no peer device");
            return;
        }
        dataController_->OnCallCreated(call, peerDevices_.front());
    }
}
 
void InteroperableCommunicationManager::CallDestroyed(const DisconnectedDetails &details)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (dataController_ != nullptr) {
        dataController_->OnCallDestroyed();
    }
}
}
}