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

#ifndef TELEPHONY_DISTRIBUTED_COMMUNICATION_DEVICE_SWITCH_CONTROLLER_H
#define TELEPHONY_DISTRIBUTED_COMMUNICATION_DEVICE_SWITCH_CONTROLLER_H

#include "ffrt.h"
#include "i_distributed_device_state_callback.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t DEFAULT_HFP_FLAG_VALUE = -1;
constexpr int32_t WEAR_ACTION = 0;
constexpr int32_t UNWEAR_ACTION = 1;
constexpr int32_t ENABLE_FROM_REMOTE_ACTION = 2;
constexpr int32_t DISABLE_FROM_REMOTE_ACTION = 3;
constexpr int32_t USER_SELECTION_ACTION = 6;

class DistributedDeviceSwitchController : public IDistributedDeviceStateCallback {
public:
    DistributedDeviceSwitchController() = default;
    virtual ~DistributedDeviceSwitchController() = default;

    virtual bool SwitchDevice(const std::string &devId, int32_t direction)
    {
        return false;
    }

    virtual bool IsAudioOnSink()
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        return isAudioOnSink_;
    }

protected:
    ffrt::mutex mutex_{};
    bool isAudioOnSink_{false};
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_DISTRIBUTED_COMMUNICATION_DEVICE_SWITCH_CONTROLLER_H
