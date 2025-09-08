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
#include "parameters.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr uint16_t DEV_PHONE = 0x0E;
constexpr uint16_t DEV_WATCH = 0x6D;
constexpr uint16_t DEV_CAR = 0x83;
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
    std::string networkId = deviceInfo.networkId;
    std::string devName = deviceInfo.deviceName;
    uint16_t devType = deviceInfo.deviceTypeId;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        std::string ownType = system::GetParameter("const.product.devicetype", "");
        if (devType == DEV_PHONE && (ownType == "wearable" || ownType == "car")) {
            role_ = InteroperableRole::OTHERS;
        } else if (devType == DEV_WATCH || devType == DEV_CAR) {
            role_ = InteroperableRole::PHONE;
        } else {
            TELEPHONY_LOGE("not interoperable device");
            return;
        }
        auto iter = std::find(peerDevices_.begin(), peerDevices_.end(), networkId);
        if (iter == peerDevices_.end()) {
            peerDevices_.emplace_back(networkId);
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
    if (devObserver_ == nullptr) {
        return;
    }
    std::string networkId = deviceInfo.networkId;
    std::string devName = deviceInfo.deviceName;
    uint16_t devType = deviceInfo.deviceTypeId;
    std::string ownType = system::GetParameter("const.product.devicetype", "");
    if (devType != DEV_WATCH && devType != DEV_CAR && ownType != "wearable" && ownType != "car") {
        TELEPHONY_LOGE("not interoperable device");
        return;
    }
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
 
void InteroperableCommunicationManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (callObjectPtr == nullptr || dataController_ == nullptr) {
        TELEPHONY_LOGE("callObjectPtr is null or dataController_ is null");
        return;
    }
    if (nextState == TelCallState::CALL_STATUS_INCOMING) {
        TELEPHONY_LOGI("interoperable new call created");
        if (role_ == InteroperableRole::PHONE) {
            TELEPHONY_LOGI("phone recv call");
            return;
        }
        if (peerDevices_.empty()) {
            TELEPHONY_LOGE("call created but no peer device");
            return;
        }
        dataController_->OnCallCreated(callObjectPtr, peerDevices_.front());
    } else if (nextState == TelCallState::CALL_STATUS_DISCONNECTED ||
        nextState == TelCallState::CALL_STATUS_DISCONNECTING) {
        TELEPHONY_LOGI("interoperable call destroyed");
        dataController_->OnCallDestroyed();
    }
}

void InteroperableCommunicationManager::NewCallCreated(sptr<CallBase> &call)
{
    TELEPHONY_LOGI("interoperable NewCallCreated");
    if (dataController_ == nullptr) {
        TELEPHONY_LOGE("dataController is nullptr");
        return;
    }
    if (peerDevices_.empty() || call == nullptr) {
        TELEPHONY_LOGE("no peer device or call is nullptr");
        return;
    }
    dataController_->CallCreated(call, peerDevices_.front());
}

int32_t InteroperableCommunicationManager::GetBtCallSlotId(const std::string &phoneNum)
{
    int32_t btCallSlot = BT_CALL_INVALID_SLOT;
    if (dataController_ == nullptr) {
        return btCallSlot;
    }

    btCallSlot = dataController_->GetBtSlotIdByPhoneNumber(phoneNum);
    if (btCallSlot == BT_CALL_INVALID_SLOT) { // slotId has not been received yet, need wait.
        dataController_->WaitForBtSlotId(phoneNum);
        btCallSlot = dataController_->GetBtSlotIdByPhoneNumber(phoneNum);
    }
    dataController_->DeleteBtSlotIdByPhoneNumber(phoneNum); // delete after query
    return btCallSlot;
}

bool InteroperableCommunicationManager::IsSlotIdVisible()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (dataController_ == nullptr) {
        TELEPHONY_LOGE("dataController is nullptr");
        return false;
    }
    return dataController_->IsSlotIdVisible();
}
}
}
