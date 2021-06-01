/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "call_state_report_proxy.h"

#include <string_ex.h>

#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "call_manager_errors.h"
#include "call_manager_log.h"
#include "state_registry_death_recipient.h"

namespace OHOS {
namespace TelephonyCallManager {
namespace {
constexpr uint32_t CONNECT_STATE_REGISTRY_TIME = 1000; // ms
}
CallStateReportProxy::CallStateReportProxy()
    : systemAbilityId_(TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID), telephonyStateNotifyPtr_(nullptr),
      stateRegistryRecipient_(nullptr), connectState_(false)
{}

CallStateReportProxy::~CallStateReportProxy()
{
    UnInit();
}

int32_t CallStateReportProxy::Init(int32_t systemAbilityId)
{
    systemAbilityId_ = systemAbilityId;
    CALLMANAGER_INFO_LOG("init reportCallStateClient");

    int32_t result = ConnectService();
    if (result != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("connect state registry service failed! errCode:%{public}d", result);
        Timer::start(CONNECT_STATE_REGISTRY_TIME, CallStateReportProxy::task);
        return result;
    }
    CALLMANAGER_INFO_LOG("connected to state registry service successfully");
    return result;
}

void CallStateReportProxy::UnInit()
{
    DisconnectService();
    CALLMANAGER_INFO_LOG("disconnect service");
}

void CallStateReportProxy::task()
{
    CALLMANAGER_DEBUG_LOG("start task...");
    int32_t ret = DelayedSingleton<CallStateReportProxy>::GetInstance()->ConnectService();
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("state registry service reconnection failed!errCode:%{public}X", ret);
        return;
    }
    DelayedSingleton<CallStateReportProxy>::GetInstance()->ThreadExit();
    CALLMANAGER_INFO_LOG("state registry service reconnection successfully!");
}

int32_t CallStateReportProxy::ReportCallState(int32_t callState, std::u16string phoneNumber)
{
    int ret = TELEPHONY_FAIL;
    if (telephonyStateNotifyPtr_ == nullptr) {
        CALLMANAGER_INFO_LOG(
            "telephony state notify service haven't connected,try to connect telephony state notify service "
            "now...");
        int32_t result = ConnectService();
        if (result != TELEPHONY_NO_ERROR) {
            CALLMANAGER_ERR_LOG("Connect service: %{public}X", result);
            return result;
        }
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (telephonyStateNotifyPtr_ != nullptr) {
        ret = telephonyStateNotifyPtr_->UpdateCallState(callState, phoneNumber);
        if (ret != TELEPHONY_NO_ERROR) {
            CALLMANAGER_ERR_LOG("notifyCallStateUpdated failed, errcode:%{public}d", ret);
            return ret;
        }
    }
    return ret;
}

int32_t CallStateReportProxy::ReportCallStateForCallId(
    int32_t subId, int32_t callId, int32_t callState, std::u16string incomingNumber)
{
    int ret = TELEPHONY_FAIL;
    if (telephonyStateNotifyPtr_ == nullptr) {
        CALLMANAGER_INFO_LOG(
            "telephony state notify service haven't connected,try to connect telephony state notify service "
            "now...");
        int32_t result = ConnectService();
        if (result != TELEPHONY_NO_ERROR) {
            CALLMANAGER_ERR_LOG("connect service: %{public}X", result);
            return result;
        }
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (telephonyStateNotifyPtr_ != nullptr) {
        ret = telephonyStateNotifyPtr_->UpdateCallStateForSlotIndex(subId, callId, callState, incomingNumber);
        if (ret != TELEPHONY_NO_ERROR) {
            CALLMANAGER_ERR_LOG("NotifyCallStateUpdated failed, errcode:%{public}d", ret);
            return ret;
        }
    }
    return ret;
}

int32_t CallStateReportProxy::ConnectService()
{
    int32_t ret = TELEPHONY_FAIL;
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientlock_);
    if (telephonyStateNotifyPtr_ != nullptr) {
        CALLMANAGER_ERR_LOG("Already get telephony state notify service");
        return ret;
    }

    CALLMANAGER_DEBUG_LOG("Get telephony state notify service %{public}d start", systemAbilityId_);
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        CALLMANAGER_ERR_LOG("GetSystemAbilityManager null");
        return ret;
    }
    sptr<TelephonyState::ITelephonyStateNotify> telephonyStateNotifyPtr = nullptr;
    sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(systemAbilityId_);
    if (iRemoteObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("GetSystemAbility null");
        return TELEPHONY_CONNECT_SYSTEM_ABILITY_STUB_FAIL;
    }
    telephonyStateNotifyPtr = iface_cast<TelephonyState::ITelephonyStateNotify>(iRemoteObjectPtr);
    if (telephonyStateNotifyPtr == nullptr) {
        CALLMANAGER_ERR_LOG("GetSystemAbility null");
        return ret;
    }
#ifdef SUBSCRIBE_DEATH_NOTICE
    std::weak_ptr<CallStateReportProxy> weakPtr = shared_from_this();
    auto deathCallback = [weakPtr](const wptr<IRemoteObject> &object) {
        auto sharedPtr = weakPtr.lock();
        if (sharedPtr) {
            sharedPtr->OnDeath();
        }
    };
    stateRegistryRecipient_ = new TelephonyState::StateRegistryDeathRecipient(deathCallback);
    if (stateRegistryRecipient_ == nullptr) {
        CALLMANAGER_INFO_LOG("create StateRegistryDeathRecipient object failed!");
        return TELEPHONY_LOCAL_PTR_NULL;
    }
    if (!iRemoteObjectPtr->AddDeathRecipient(stateRegistryRecipient_)) {
        CALLMANAGER_INFO_LOG("failed to subscribing to the state registry service death notification");
        return TELEPHONY_ADD_DEATH_RECIPIENT_FAIL;
    }
    CALLMANAGER_INFO_LOG("Succeeded in subscribing to the state registry service death notification");
#endif
    CALLMANAGER_INFO_LOG("Get telephony state notify service finish");
    telephonyStateNotifyPtr_ = telephonyStateNotifyPtr;
    return TELEPHONY_NO_ERROR;
}

void CallStateReportProxy::DisconnectService()
{
    Clean();
}

void CallStateReportProxy::ReConnectService()
{
    if (telephonyStateNotifyPtr_ == nullptr) {
        CALLMANAGER_INFO_LOG(
            "telephony state notify service haven't connected,try to connect telephony state "
            "notify "
            "service now...");
        int32_t result = ConnectService();
        if (result != TELEPHONY_NO_ERROR) {
            CALLMANAGER_ERR_LOG("Connect service: %{public}X", result);
            return;
        }
    }
}

void CallStateReportProxy::OnDeath()
{
    Clean();
    NotifyDeath();
}

void CallStateReportProxy::Clean()
{
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientlock_);
    if (stateRegistryRecipient_) {
        stateRegistryRecipient_.clear();
        stateRegistryRecipient_ = nullptr;
    }
    if (telephonyStateNotifyPtr_) {
        telephonyStateNotifyPtr_.clear();
        telephonyStateNotifyPtr_ = nullptr;
    }
    connectState_ = false;
}

void CallStateReportProxy::NotifyDeath()
{
    CALLMANAGER_INFO_LOG("service is dead, connect again");
    Timer::start(CONNECT_STATE_REGISTRY_TIME, CallStateReportProxy::task);
}
} // namespace TelephonyCallManager
} // namespace OHOS
