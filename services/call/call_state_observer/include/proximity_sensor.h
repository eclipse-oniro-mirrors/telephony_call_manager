/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_PROXIMITY_SENSOR_H
#define TELEPHONY_PROXIMITY_SENSOR_H

#include "singleton.h"
#include "call_state_listener_base.h"
#ifdef ABILITY_POWER_SUPPORT
#include "power_mgr_client.h"
#endif

namespace OHOS {
namespace Telephony {
class ProximitySensor : public CallStateListenerBase {
    DECLARE_DELAYED_SINGLETON(ProximitySensor)
public:
    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(const DisconnectedDetails &details) override;
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;

private:
#ifdef ABILITY_POWER_SUPPORT
    void ShutDown(PowerMgr::SuspendDeviceType operationType, bool suspendImmed);
    void StartUp(PowerMgr::WakeupDeviceType operationType, std::string wakeupReason);
#endif
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_PROXIMITY_SENSOR_H