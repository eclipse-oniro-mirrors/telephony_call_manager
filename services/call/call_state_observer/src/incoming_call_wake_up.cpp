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

#include "incoming_call_wake_up.h"

#ifdef ABILITY_POWER_SUPPORT
#include "power_mgr_client.h"
#endif
#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
void IncomingCallWakeup::NewCallCreated(sptr<CallBase> &callObjectPtr)
{
    // Should wake up the device and set the screen on while a new incoming call created.
    if (callObjectPtr != nullptr && callObjectPtr->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING) {
        WakeupDevice();
    }
}

void IncomingCallWakeup::WakeupDevice()
{
#ifdef ABILITY_POWER_SUPPORT
    if (phoneRunningLock_ == nullptr) {
        phoneRunningLock_ = PowerMgr::PowerMgrClient::GetInstance().
            CreateRunningLock("phonerunninglock", PowerMgr::RunningLockType::RUNNINGLOCK_BACKGROUND_PHONE);
    }
    if (phoneRunningLock_ != nullptr && !isPhoneLocked) {
        phoneRunningLock_->Lock();
        isPhoneLocked = true;
        TELEPHONY_LOGI("phoneRunningLock_ locked");
    }
    if (screenRunningLock_ == nullptr) {
        screenRunningLock_ = PowerMgr::PowerMgrClient::GetInstance().
            CreateRunningLock("screenonrunninglock", PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN);
    }
#endif
    if (IsScreenOn()) {
        TELEPHONY_LOGI("screen already up");
    #ifdef ABILITY_POWER_SUPPORT
        if (screenRunningLock_ != nullptr && !isScreenOnLocked) {
            screenRunningLock_->Lock();
            isScreenOnLocked = true;
            TELEPHONY_LOGI("screenRunningLock_ locked");
        }
    #endif
        return;
    }
#ifdef ABILITY_POWER_SUPPORT
    PowerMgr::PowerMgrClient::GetInstance().WakeupDevice(
        PowerMgr::WakeupDeviceType::WAKEUP_DEVICE_APPLICATION, wakeupReason_);
    if (screenRunningLock_ != nullptr && !isScreenOnLocked) {
        screenRunningLock_->Lock();
        isScreenOnLocked = true;
        TELEPHONY_LOGI("screenRunningLock_ locked");
    }
#endif
}

bool IncomingCallWakeup::IsScreenOn()
{
    bool isScreenOn = false;
#ifdef ABILITY_POWER_SUPPORT
    isScreenOn = PowerMgr::PowerMgrClient::GetInstance().IsScreenOn();
#endif
    return isScreenOn;
}

bool IncomingCallWakeup::IsPowerAbilityExist()
{
    std::lock_guard<std::mutex> lock(mutex_);
    sptr<ISystemAbilityManager> sysAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!sysAbilityMgr) {
        TELEPHONY_LOGE("system ability manager nullptr");
        return false;
    }
    sptr<IRemoteObject> remote = sysAbilityMgr->CheckSystemAbility(POWER_MANAGER_SERVICE_ID);
    if (!remote) {
        TELEPHONY_LOGE("no power ability");
        return false;
    }
    return true;
}

void IncomingCallWakeup::CallDestroyed(const DisconnectedDetails &details) {}

void IncomingCallWakeup::IncomingCallActivated(sptr<CallBase> &callObjectPtr) {}

void IncomingCallWakeup::IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) {}

void IncomingCallWakeup::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (!isPhoneLocked && !isScreenOnLocked) {
        return;
    }
    bool hasRingCall = false;
    CallObjectManager::HasRingingCall(hasRingCall);
    if (!hasRingCall) {
    #ifdef ABILITY_POWER_SUPPORT
        if (screenRunningLock_ != nullptr && isScreenOnLocked) {
            screenRunningLock_->UnLock();
            isScreenOnLocked = false;
            TELEPHONY_LOGI("screenRunningLock_ unlocked");
        }
        if (phoneRunningLock_ != nullptr && isPhoneLocked) {
            phoneRunningLock_->UnLock();
            isPhoneLocked = false;
            TELEPHONY_LOGI("phoneRunningLock_ unlocked");
        }
    #endif
    }
}
} // namespace Telephony
} // namespace OHOS