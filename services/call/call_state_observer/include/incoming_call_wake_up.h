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

#ifndef TELEPHONY_INCOMING_CALL_WAKE_UP_H
#define TELEPHONY_INCOMING_CALL_WAKE_UP_H
#ifdef ABILITY_POWER_SUPPORT
#include "power_mgr_client.h"
#endif
#include <mutex>
#include "call_object_manager.h"
#include "call_state_listener_base.h"

namespace OHOS {
namespace Telephony {
class IncomingCallWakeup : public CallStateListenerBase, public CallObjectManager {
public:
    IncomingCallWakeup() = default;
    ~IncomingCallWakeup() = default;
    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(const DisconnectedDetails &details) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    void AcquireIncomingLock();
    void ReleaseIncomingLock();

private:
    std::mutex mutex_;
    const std::string wakeupReason_ = "incoming call";
    /**
     * Wake up the device and set the screen on.
     */
    void WakeupDevice();
    bool IsPowerAbilityExist();
#ifdef ABILITY_POWER_SUPPORT
    std::shared_ptr<PowerMgr::RunningLock> screenRunningLock_;
    std::shared_ptr<PowerMgr::RunningLock> phoneRunningLock_;
#endif
    bool isPhoneLocked = false;
    bool isScreenOnLocked = false;
    const int32_t INCOMING_LOCK_TIMEOUT = 5000;
};
} // namespace Telephony
} // namespace OHOS
#endif