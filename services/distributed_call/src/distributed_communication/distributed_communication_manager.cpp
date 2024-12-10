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

#include "distributed_communication_manager.h"
#include <dlfcn.h>
#include "cJSON.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "distributed_data_sink_controller.h"
#include "distributed_data_source_controller.h"
#include "distributed_sink_switch_controller.h"
#include "distributed_source_switch_controller.h"

namespace OHOS {
namespace Telephony {
DistributedCommunicationManager::DistributedCommunicationManager()
{
}

DistributedCommunicationManager::~DistributedCommunicationManager()
{
}

void DistributedCommunicationManager::Init()
{
    devObserver_ = std::make_shared<DistributedDeviceObserver>();
    if (devObserver_ != nullptr) {
        devObserver_->Init();
    }
}

void DistributedCommunicationManager::InitExtWrapper()
{
    std::lock_guard<ffrt::mutex> lock(mutexFunc_);
    if (extWrapperHandler_ != nullptr) {
        return;
    }
    extWrapperHandler_ = dlopen("libtelephony_ext_innerkits.z.so", RTLD_NOW);
    if (extWrapperHandler_ == nullptr) {
        TELEPHONY_LOGE("open ext so failed");
        return;
    }
    regDevCallbackFunc_ = (REGISTER_DEVICE_CALLBACK)dlsym(extWrapperHandler_, "RegisterDistributedDevState");
    unRegDevCallbackFunc_ = (UN_REGISTER_DEVICE_CALLBACK)dlsym(extWrapperHandler_, "UnRegisterDistributedDevState");
    switchDevFunc_ = (SWITCH_DEVICE)dlsym(extWrapperHandler_, "SwitchDistributedDevice");
    TELEPHONY_LOGI("init ext wrapper finish");
}

void DistributedCommunicationManager::DeInitExtWrapper()
{
    std::lock_guard<ffrt::mutex> lock(mutexFunc_);
    if (extWrapperHandler_ != nullptr) {
        dlclose(extWrapperHandler_);
        extWrapperHandler_ = nullptr;
        regDevCallbackFunc_ = nullptr;
        unRegDevCallbackFunc_ = nullptr;
        switchDevFunc_ = nullptr;
    }
    TELEPHONY_LOGI("de-init ext wrapper finish");
}

int32_t DistributedCommunicationManager::RegDevCallbackWrapper(
    const std::shared_ptr<IDistributedDeviceCallback> &callback)
{
    std::lock_guard<ffrt::mutex> lock(mutexFunc_);
    if (regDevCallbackFunc_ == nullptr) {
        TELEPHONY_LOGE("reg dev callback func is null");
        return TELEPHONY_ERROR;
    }
    return regDevCallbackFunc_(callback);
}

int32_t DistributedCommunicationManager::UnRegDevCallbackWrapper()
{
    std::lock_guard<ffrt::mutex> lock(mutexFunc_);
    if (unRegDevCallbackFunc_ == nullptr) {
        TELEPHONY_LOGE("un-reg dev callback func is null");
        return TELEPHONY_ERROR;
    }
    return unRegDevCallbackFunc_();
}

int32_t DistributedCommunicationManager::SwitchDevWrapper(const std::string &devId, int32_t direction)
{
    std::lock_guard<ffrt::mutex> lock(mutexFunc_);
    if (switchDevFunc_ == nullptr) {
        TELEPHONY_LOGE("switch dev func is null");
        return TELEPHONY_ERROR;
    }
    return switchDevFunc_(devId, direction);
}

void DistributedCommunicationManager::OnDeviceOnline(const std::string &devId, const std::string &devName,
    AudioDeviceType deviceType, int32_t devRole)
{
    if (devObserver_ == nullptr) {
        return;
    }
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        status_ = DistributedStatus::CONNECT;
        role_ = (devRole == 0 ? DistributedRole::SINK : DistributedRole::SOURCE); // peer role 0 means local role sink
        auto iter = std::find(peerDevices_.begin(), peerDevices_.end(), devId);
        if (iter == peerDevices_.end()) {
            peerDevices_.emplace_back(devId);
        }
        if (dataController_ == nullptr) {
            if (role_ == DistributedRole::SINK) {
                dataController_ = std::make_shared<DistributedDataSinkController>();
            } else {
                dataController_ = std::make_shared<DistributedDataSourceController>();
            }
        }
        if (devSwitchController_ == nullptr) {
            if (role_ == DistributedRole::SINK) {
                devSwitchController_ = std::make_shared<DistributedSinkSwitchController>();
            } else {
                devSwitchController_ = std::make_shared<DistributedSourceSwitchController>();
            }
        }
    }
    devObserver_->RegisterDevStatusCallback(dataController_);
    devObserver_->RegisterDevStatusCallback(devSwitchController_);
    devObserver_->OnDeviceOnline(devId, devName, deviceType);
    TELEPHONY_LOGI("distributed device online");
}

void DistributedCommunicationManager::OnDeviceOffline(const std::string &devId, const std::string &devName,
    AudioDeviceType deviceType, int32_t devRole)
{
    if (devObserver_ == nullptr) {
        return;
    }
    devObserver_->OnDeviceOffline(devId, devName, deviceType);
    devObserver_->UnRegisterDevStatusCallback(dataController_);
    devObserver_->UnRegisterDevStatusCallback(devSwitchController_);

    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto iter = std::find(peerDevices_.begin(), peerDevices_.end(), devId);
    if (iter != peerDevices_.end()) {
        peerDevices_.erase(iter);
    }
    if (!peerDevices_.empty()) {
        TELEPHONY_LOGI("device list not empty");
        return;
    }

    status_ = DistributedStatus::DISCONNECT;
    role_ = DistributedRole::UNKNOWN;
    if (dataController_ != nullptr) {
        dataController_.reset();
        dataController_ = nullptr;
    }
    if (devSwitchController_ != nullptr) {
        devSwitchController_.reset();
        devSwitchController_ = nullptr;
    }
    TELEPHONY_LOGI("distributed device offline");
}

void DistributedCommunicationManager::OnRemoveSystemAbility()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    peerDevices_.clear();
    status_ = DistributedStatus::DISCONNECT;
    role_ = DistributedRole::UNKNOWN;
    dataController_ = nullptr;
    devSwitchController_ = nullptr;
    TELEPHONY_LOGI("distributed system ability remove");
}

bool DistributedCommunicationManager::IsSinkRole()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return role_ == DistributedRole::SINK;
}

bool DistributedCommunicationManager::IsConnected()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return status_ == DistributedStatus::CONNECT;
}

bool DistributedCommunicationManager::IsDistributedDev(const AudioDevice& device)
{
    std::string devId = ParseDevIdFromAudioDevice(device);
    return IsDistributedDev(devId);
}

const std::shared_ptr<DistributedDeviceObserver>& DistributedCommunicationManager::GetDistributedDeviceObserver()
{
    return devObserver_;
}

bool DistributedCommunicationManager::SwitchToSinkDevice(const AudioDevice& device)
{
    std::string devId = ParseDevIdFromAudioDevice(device);
    if (devId.empty()) {
        TELEPHONY_LOGE("parse dev id fail");
        return false;
    }
    if (!IsDistributedDev(devId)) {
        TELEPHONY_LOGE("not distributed dev");
        return false;
    }

    TELEPHONY_LOGI("switch to remote.");
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (devSwitchController_ == nullptr) {
        return false;
    }
    if (devSwitchController_->IsAudioOnSink()) {
        TELEPHONY_LOGE("call is already on sink, no need to switch");
        return true;
    }

    return devSwitchController_->SwitchDevice(devId, static_cast<int32_t>(DistributedRole::SINK));
}

bool DistributedCommunicationManager::SwitchToSourceDevice()
{
    TELEPHONY_LOGI("switch to local.");
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (devSwitchController_ == nullptr) {
        return false;
    }
    if (!devSwitchController_->IsAudioOnSink()) {
        return true;
    }
    return devSwitchController_->SwitchDevice("", static_cast<int32_t>(DistributedRole::SOURCE));
}

bool DistributedCommunicationManager::IsAudioOnSink()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (devSwitchController_ == nullptr) {
        return false;
    }
    return devSwitchController_->IsAudioOnSink();
}

void DistributedCommunicationManager::SetMuted(bool isMute)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (dataController_ != nullptr) {
        dataController_->SetMuted(isMute);
    }
    TELEPHONY_LOGI("set muted %{public}d", isMute);
}

void DistributedCommunicationManager::MuteRinger()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (dataController_ != nullptr) {
        dataController_->MuteRinger();
    }
}

void DistributedCommunicationManager::ProcessCallInfo(const sptr<CallBase> &call, DistributedDataType type)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (dataController_ != nullptr) {
        dataController_->ProcessCallInfo(call, type);
    }
}

void DistributedCommunicationManager::NewCallCreated(sptr<CallBase> &call)
{
    if (call == nullptr) {
        return;
    }
    auto callType = call->GetCallType();
    if (callType != CallType::TYPE_IMS && callType != CallType::TYPE_CS) {
        return;
    }

    if (role_ == DistributedRole::SINK) {
        DistributedSinkSwitchController::TrySwitchToBtHeadset();
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (dataController_ != nullptr) {
        if (peerDevices_.empty()) {
            TELEPHONY_LOGI("call created but no peer device");
            return;
        }
        TELEPHONY_LOGI("call %{public}d created, role %{public}hhd", call->GetCallID(), role_);
        dataController_->OnCallCreated(call, peerDevices_.front());
    }
}

void DistributedCommunicationManager::CallDestroyed(const DisconnectedDetails &details)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (dataController_ != nullptr) {
        TELEPHONY_LOGI("call destroyed, role %{public}hhd", role_);
        dataController_->OnCallDestroyed();
    }
}

bool DistributedCommunicationManager::IsDistributedDev(const std::string &devId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto iter = std::find(peerDevices_.begin(), peerDevices_.end(), devId);
    return iter != peerDevices_.end();
}

std::string DistributedCommunicationManager::ParseDevIdFromAudioDevice(const AudioDevice& device)
{
    std::string devId = "";
    std::string address = device.address;
    if (address.empty()) {
        TELEPHONY_LOGI("address invalid, not distributed device");
        return devId;
    }
    cJSON *root = cJSON_Parse(address.c_str());
    if (root == nullptr) {
        TELEPHONY_LOGE("json string invalid");
        return devId;
    }
    cJSON *devIdJson = cJSON_GetObjectItem(root, "devId");
    if (devIdJson != nullptr) {
        if (cJSON_IsString(devIdJson)) {
            char *value = cJSON_GetStringValue(devIdJson);
            devId = value;
        }
    }
    cJSON_Delete(root);
    return devId;
}

} // namespace Telephony
} // namespace OHOS
