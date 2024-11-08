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

#ifndef TELEPHONY_DISTRIBUTED_COMMUNICATION_DEVICE_OBSERVER_H
#define TELEPHONY_DISTRIBUTED_COMMUNICATION_DEVICE_OBSERVER_H

#include "ffrt.h"
#include "call_manager_base.h"
#include "system_ability_status_change_stub.h"
#include "i_distributed_device_callback.h"
#include "i_distributed_device_state_callback.h"

namespace OHOS {
namespace Telephony {
class DistributedDeviceCallback : public IDistributedDeviceCallback {
public:
    DistributedDeviceCallback() = default;
    ~DistributedDeviceCallback() override = default;
    int32_t OnDistributedDeviceOnline(const std::string &devId, const std::string &devName,
        int32_t devType, int32_t devRole) override;
    int32_t OnDistributedDeviceOffline(const std::string &devId, const std::string &devName,
        int32_t devType, int32_t devRole) override;
    int32_t OnDistributedAudioDeviceChange(const std::string &devId, const std::string &devName,
        int32_t devType, int32_t devRole) override;

private:
    AudioDeviceType ConvertDeviceType(int32_t devType);
};

class DistributedSystemAbilityListener : public SystemAbilityStatusChangeStub {
public:
    DistributedSystemAbilityListener() = default;
    ~DistributedSystemAbilityListener() = default;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
};

class DistributedDeviceObserver {
public:
    DistributedDeviceObserver() = default;
    ~DistributedDeviceObserver() = default;
    void Init();
    void RegisterDevCallback();
    int32_t UnRegisterDevCallback();
    void RegisterDevStatusCallback(const std::shared_ptr<IDistributedDeviceStateCallback> &callback);
    void UnRegisterDevStatusCallback(const std::shared_ptr<IDistributedDeviceStateCallback> &callback);
    void OnDeviceOnline(const std::string &devId, const std::string &devName, AudioDeviceType devType);
    void OnDeviceOffline(const std::string &devId, const std::string &devName, AudioDeviceType devType);
    void OnDistributedAudioDeviceChange(const std::string &devId, const std::string &devName,
        AudioDeviceType devType, int32_t devRole);
    void OnRemoveSystemAbility();

private:
    sptr<DistributedSystemAbilityListener> saListener_{nullptr};
    ffrt::mutex mutex_{};
    std::list<std::shared_ptr<IDistributedDeviceStateCallback>> callbacks_{};
    std::shared_ptr<IDistributedDeviceCallback> deviceListener_{nullptr};
};

} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_DISTRIBUTED_COMMUNICATION_DEVICE_OBSERVER_H
