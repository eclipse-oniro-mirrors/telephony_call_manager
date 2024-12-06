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

#include "distributed_device_observer.h"
#include "iservice_registry.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "distributed_communication_manager.h"

namespace OHOS {
namespace Telephony {
const int32_t DISTRIBUTED_COMMUNICATION_CALL_SA_ID = 66198;
const int32_t DISTRIBUTED_COMMUNICATION_PHONE = 0;
const int32_t DISTRIBUTED_COMMUNICATION_PAD = 1;
const int32_t DISTRIBUTED_COMMUNICATION_PC = 2;

void DistributedDeviceObserver::Init()
{
    saListener_ = sptr<DistributedSystemAbilityListener>::MakeSptr();
    if (saListener_ == nullptr) {
        TELEPHONY_LOGE("create distributed sa listener failed");
        return;
    }
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("get system ability manager failed");
        return;
    }
    int32_t ret = saManager->SubscribeSystemAbility(DISTRIBUTED_COMMUNICATION_CALL_SA_ID, saListener_);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("subscribe distributed sa fail %{public}d", ret);
        return;
    }
}

void DistributedDeviceObserver::RegisterDevStatusCallback(
    const std::shared_ptr<IDistributedDeviceStateCallback> &callback)
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

void DistributedDeviceObserver::UnRegisterDevStatusCallback(
    const std::shared_ptr<IDistributedDeviceStateCallback> &callback)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto iter = std::find(callbacks_.begin(), callbacks_.end(), callback);
    if (iter != callbacks_.end()) {
        callbacks_.erase(iter);
    }
    TELEPHONY_LOGI("un-reg dev status callback");
}

void DistributedDeviceObserver::RegisterDevCallback()
{
    if (deviceListener_ == nullptr) {
        deviceListener_ = std::make_shared<DistributedDeviceCallback>();
    }
    if (deviceListener_ == nullptr) {
        return;
    }
    auto distributedMgr = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    if (distributedMgr != nullptr) {
        auto ret = distributedMgr->RegDevCallbackWrapper(deviceListener_);
        TELEPHONY_LOGI("reg distributed device callback result[%{public}d]", ret);
    }
}

int32_t DistributedDeviceObserver::UnRegisterDevCallback()
{
    int32_t res = TELEPHONY_SUCCESS;
    if (deviceListener_ != nullptr) {
        auto distributedMgr = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
        if (distributedMgr != nullptr) {
            res = distributedMgr->UnRegDevCallbackWrapper();
            TELEPHONY_LOGI("un-reg distributed device callback result[%{public}d]", res);
        }
        deviceListener_.reset();
        deviceListener_ = nullptr;
    }
    return res;
}

void DistributedDeviceObserver::OnDeviceOnline(const std::string &devId, const std::string &devName,
                                               AudioDeviceType devType)
{
    std::list<std::shared_ptr<IDistributedDeviceStateCallback>> callbacks;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        callbacks = callbacks_;
    }
    for (auto& callback : callbacks) {
        if (callback != nullptr) {
            callback->OnDeviceOnline(devId, devName, devType);
        }
    }
}

void DistributedDeviceObserver::OnDeviceOffline(const std::string &devId, const std::string &devName,
                                                AudioDeviceType devType)
{
    std::list<std::shared_ptr<IDistributedDeviceStateCallback>> callbacks;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        callbacks = callbacks_;
    }
    for (auto& callback : callbacks) {
        if (callback != nullptr) {
            callback->OnDeviceOffline(devId, devName, devType);
        }
    }
}

void DistributedDeviceObserver::OnDistributedAudioDeviceChange(const std::string &devId, const std::string &devName,
    AudioDeviceType devType, int32_t devRole)
{
    std::list<std::shared_ptr<IDistributedDeviceStateCallback>> callbacks;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        callbacks = callbacks_;
    }
    for (auto& callback : callbacks) {
        if (callback != nullptr) {
            callback->OnDistributedAudioDeviceChange(devId, devName, devType, devRole);
        }
    }
}

void DistributedDeviceObserver::OnRemoveSystemAbility()
{
    std::list<std::shared_ptr<IDistributedDeviceStateCallback>> callbacks;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        callbacks = callbacks_;
    }
    for (auto& callback : callbacks) {
        if (callback != nullptr) {
            callback->OnRemoveSystemAbility();
        }
    }
}

int32_t DistributedDeviceCallback::OnDistributedDeviceOnline(const std::string &devId, const std::string &devName,
                                                             int32_t devType, int32_t devRole)
{
    TELEPHONY_LOGI("dev online, type %{public}d, role %{public}d", devType, devRole);
    AudioDeviceType deviceType = ConvertDeviceType(devType);
    if (deviceType == AudioDeviceType::DEVICE_UNKNOWN) {
        return TELEPHONY_ERROR;
    }
    auto distributedMgr = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    if (distributedMgr == nullptr) {
        return TELEPHONY_ERROR;
    }
    distributedMgr->OnDeviceOnline(devId, devName, deviceType, devRole);
    return TELEPHONY_SUCCESS;
}

int32_t DistributedDeviceCallback::OnDistributedAudioDeviceChange(const std::string &devId, const std::string &devName,
    int32_t devType, int32_t devRole)
{
    TELEPHONY_LOGI("distributed device change, current device[%{public}d]", devRole);
    auto distributedMgr = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    if (distributedMgr == nullptr) {
        return TELEPHONY_ERROR;
    }
    AudioDeviceType deviceType = ConvertDeviceType(devType);
    if (deviceType == AudioDeviceType::DEVICE_UNKNOWN) {
        return TELEPHONY_ERROR;
    }
    auto deviceObserver = distributedMgr->GetDistributedDeviceObserver();
    if (deviceObserver != nullptr) {
        deviceObserver->OnDistributedAudioDeviceChange(devId, devName, deviceType, devRole);
    }
    return TELEPHONY_SUCCESS;
}

int32_t DistributedDeviceCallback::OnDistributedDeviceOffline(const std::string &devId, const std::string &devName,
                                                              int32_t devType, int32_t devRole)
{
    TELEPHONY_LOGI("dev offline, type %{public}d, role %{public}d", devType, devRole);
    AudioDeviceType deviceType = ConvertDeviceType(devType);
    if (deviceType == AudioDeviceType::DEVICE_UNKNOWN) {
        return TELEPHONY_ERROR;
    }
    auto distributedMgr = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    if (distributedMgr == nullptr) {
        return TELEPHONY_ERROR;
    }
    distributedMgr->OnDeviceOffline(devId, devName, deviceType, devRole);
    return TELEPHONY_SUCCESS;
}

AudioDeviceType DistributedDeviceCallback::ConvertDeviceType(int32_t devType)
{
    switch (devType) {
        case DISTRIBUTED_COMMUNICATION_PAD:
            return AudioDeviceType::DEVICE_DISTRIBUTED_PAD;
        case DISTRIBUTED_COMMUNICATION_PC:
            return AudioDeviceType::DEVICE_DISTRIBUTED_PC;
        case DISTRIBUTED_COMMUNICATION_PHONE:
            return AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
        default:
            return AudioDeviceType::DEVICE_UNKNOWN;
    }
}

void DistributedSystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != DISTRIBUTED_COMMUNICATION_CALL_SA_ID) {
        TELEPHONY_LOGE("not distributed sa id");
        return;
    }
    TELEPHONY_LOGI("distribute communication sa online");
    auto distributedMgr = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    if (distributedMgr == nullptr) {
        return;
    }
    distributedMgr->InitExtWrapper();
    auto deviceObserver = distributedMgr->GetDistributedDeviceObserver();
    if (deviceObserver != nullptr) {
        deviceObserver->RegisterDevCallback();
    }
}

void DistributedSystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != DISTRIBUTED_COMMUNICATION_CALL_SA_ID) {
        TELEPHONY_LOGE("not distributed sa id");
        return;
    }
    TELEPHONY_LOGI("distribute communication sa offline");
    auto distributedMgr = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    if (distributedMgr == nullptr) {
        return;
    }
    auto deviceObserver = distributedMgr->GetDistributedDeviceObserver();
    if (deviceObserver != nullptr) {
        deviceObserver->OnRemoveSystemAbility();
        if (deviceObserver->UnRegisterDevCallback() != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("un-reg distributed device callback failed, can't dlclose ext lib");
            return;
        }
    }
    distributedMgr->OnRemoveSystemAbility();
    distributedMgr->DeInitExtWrapper();
}

} // namespace Telephony
} // namespace OHOS
