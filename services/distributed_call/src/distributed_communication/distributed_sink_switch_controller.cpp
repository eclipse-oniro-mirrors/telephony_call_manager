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

#include "distributed_sink_switch_controller.h"

#include <chrono>
#include "distributed_communication_manager.h"
#include "audio_device_manager.h"
#include "audio_control_manager.h"
#include "bluetooth_connection.h"

namespace OHOS {
namespace Telephony {
using namespace std::chrono;
void DistributedSinkSwitchController::OnDeviceOnline(const std::string &devId, const std::string &devName,
    AudioDeviceType devType)
{
}

void DistributedSinkSwitchController::OnDeviceOffline(const std::string &devId, const std::string &devName,
    AudioDeviceType devType)
{
}

void DistributedSinkSwitchController::OnDistributedAudioDeviceChange(const std::string &devId,
    const std::string &devName, AudioDeviceType devType, int32_t devRole)
{
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        isAudioOnSink_ = (devRole == static_cast<int32_t>(DistributedRole::SINK));
        TELEPHONY_LOGI("OnDistributedAudioDeviceChange isAudioOnSink[%{public}d]", isAudioOnSink_);
    }
}

void DistributedSinkSwitchController::OnRemoveSystemAbility()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    isAudioOnSink_ = false;
}

} // namespace Telephony
} // namespace OHOS
