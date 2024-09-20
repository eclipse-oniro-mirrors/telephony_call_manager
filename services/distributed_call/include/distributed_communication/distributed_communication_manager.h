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

#ifndef TELEPHONY_DISTRIBUTED_COMMUNICATION_MANAGER_H
#define TELEPHONY_DISTRIBUTED_COMMUNICATION_MANAGER_H

#include "singleton.h"
#include "ffrt.h"
#include "call_manager_inner_type.h"
#include "call_state_listener_base.h"
#include "distributed_device_observer.h"
#include "distributed_data_controller.h"
#include "distributed_device_switch_controller.h"

namespace OHOS {
namespace Telephony {
enum class DistributedRole : int8_t {
    UNKNOWN = -1,
    SOURCE = 0,
    SINK = 1
};

enum class DistributedStatus : int8_t {
    UNKNOWN = -1,
    DISCONNECT = 0,
    CONNECT = 1
};

class DistributedCommunicationManager : public CallStateListenerBase {
    DECLARE_DELAYED_SINGLETON(DistributedCommunicationManager)
public:
    void Init();
    void InitExtWrapper();
    void DeInitExtWrapper();
    int32_t RegDevCallbackWrapper(const std::shared_ptr<IDistributedDeviceCallback> &callback);
    int32_t UnRegDevCallbackWrapper();
    int32_t SwitchDevWrapper(const std::string &devId, int32_t direction);
    void OnDeviceOnline(const std::string &devId, const std::string &devName, AudioDeviceType deviceType,
        int32_t devRole);
    void OnDeviceOffline(const std::string &devId, const std::string &devName, AudioDeviceType deviceType,
        int32_t devRole);
    bool IsSinkRole();
    bool IsConnected();
    bool IsDistributedDev(const AudioDevice& device);
    const std::shared_ptr<DistributedDeviceObserver>& GetDistributedDeviceObserver();
    bool SwitchToSourceDevice();
    bool SwitchToSinkDevice(const AudioDevice& device);
    bool IsAudioOnSink();
    void SetMuted(bool isMute);
    void MuteRinger();
    void ProcessCallInfo(const sptr<CallBase> &call, DistributedDataType type);

    void NewCallCreated(sptr<CallBase> &call) override;
    void CallDestroyed(const DisconnectedDetails &details) override;

private:
    bool IsDistributedDev(const std::string &devId);
    std::string ParseDevIdFromAudioDevice(const AudioDevice& device);

private:
    typedef int32_t (*REGISTER_DEVICE_CALLBACK)(const std::shared_ptr<OHOS::Telephony::IDistributedDeviceCallback>&);
    typedef int32_t (*UN_REGISTER_DEVICE_CALLBACK)(void);
    typedef int32_t (*SWITCH_DEVICE)(const std::string&, int32_t);
    void *extWrapperHandler_{nullptr};
    REGISTER_DEVICE_CALLBACK regDevCallbackFunc_{nullptr};
    UN_REGISTER_DEVICE_CALLBACK unRegDevCallbackFunc_{nullptr};
    SWITCH_DEVICE switchDevFunc_{nullptr};

    ffrt::mutex mutex_{};
    std::list<std::string> peerDevices_{};
    std::shared_ptr<DistributedDeviceObserver> devObserver_{nullptr};
    std::shared_ptr<DistributedDataController> dataController_{nullptr};
    std::shared_ptr<DistributedDeviceSwitchController> devSwitchController_{nullptr};
    DistributedStatus status_{DistributedStatus::UNKNOWN};
    DistributedRole role_{DistributedRole::UNKNOWN};
};

} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_DISTRIBUTED_COMMUNICATION_MANAGER_H
