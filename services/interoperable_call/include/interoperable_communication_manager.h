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
 
#ifndef INTEROPERABLE_COMMUNICATION_MANAGER_H
#define INTEROPERABLE_COMMUNICATION_MANAGER_H

#include "singleton.h"
#include "ffrt.h"
#include "interoperable_device_observer.h"
#include "interoperable_data_controller.h"
#include "device_manager_callback.h"
#include "call_state_listener_base.h"

namespace OHOS {
namespace Telephony {
enum class InteroperableRole : int8_t {
    UNKNOWN = -1,
    PHONE = 0,
    OTHERS = 1
};
class InteroperableCommunicationManager : public CallStateListenerBase {
    DECLARE_DELAYED_SINGLETON(InteroperableCommunicationManager)
 
public:
    void OnDeviceOnline(const DistributedHardware::DmDeviceInfo &deviceInfo);
    void OnDeviceOffline(const DistributedHardware::DmDeviceInfo &deviceInfo);
    void SetMuted(bool isMute);
    void MuteRinger();
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    void NewCallCreated(sptr<CallBase> &call) override;
    void CallDestroyed(const DisconnectedDetails &details) override {}
    int32_t GetBtCallSlotId(const std::string &phoneNum);
 
private:
    ffrt::mutex mutex_{};
    std::list<std::string> peerDevices_{};
    std::shared_ptr<InteroperableDeviceObserver> devObserver_{nullptr};
    std::shared_ptr<InteroperableDataController> dataController_{nullptr};
    InteroperableRole role_{InteroperableRole::UNKNOWN};
};
}
}
#endif // INTEROPERABLE_COMMUNICATION_MANAGER_H