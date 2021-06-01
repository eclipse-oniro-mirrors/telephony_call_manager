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

#include "cellular_call_ipc_interface_proxy.h"

#include <thread>

#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "call_manager_errors.h"
#include "call_manager_log.h"
#include "cellular_call_death_recipient.h"

namespace OHOS {
namespace TelephonyCallManager {
constexpr uint32_t CONNECT_SERVICE_WAIT_TIME = 1000; // ms
#ifdef RECONNECT_MAX_TRY_COUNT
constexpr uint32_t CONNECT_MAX_TRY_COUNT = 5;
#endif
CellularCallIpcInterfaceProxy::CellularCallIpcInterfaceProxy()
    : systemAbilityId_(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID), cellularCallCallbackPtr_(nullptr),
      cellularCallInterfacePtr_(nullptr), connectState_(false)
{}

CellularCallIpcInterfaceProxy::~CellularCallIpcInterfaceProxy()
{
    unInit();
}

void CellularCallIpcInterfaceProxy::Init(int32_t systemAbilityId)
{
    systemAbilityId_ = systemAbilityId;

    int32_t result = ConnectService();
    if (result != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("connect service failed,errCode: %{public}X", result);
        Timer::start(CONNECT_SERVICE_WAIT_TIME, CellularCallIpcInterfaceProxy::task);
        return;
    }
    CALLMANAGER_INFO_LOG("connected to cellular call service successfully!");
    return;
}

void CellularCallIpcInterfaceProxy::unInit()
{
    DisconnectService();
    CALLMANAGER_INFO_LOG("unInit CellularCallIpcInterfaceProxy");
}

void CellularCallIpcInterfaceProxy::task()
{
    CALLMANAGER_DEBUG_LOG("start task...");
    int32_t ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->ConnectService();
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("cellular call service reconnection failed!errCode:%{public}X", ret);
        return;
    }
    DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->ThreadExit();
    CALLMANAGER_INFO_LOG("cellular call service reconnection successfully!");
}

int32_t CellularCallIpcInterfaceProxy::ConnectService()
{
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientlock_);
    if (cellularCallInterfacePtr_ != nullptr) {
        CALLMANAGER_ERR_LOG("already get telephony base service");
        return TELEPHONY_NO_ERROR;
    }
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        CALLMANAGER_ERR_LOG("GetSystemAbilityManager null");
        return TELEPHONY_FAIL;
    }
    sptr<CellularCall::CellularCallInterface> cellularCallInterfacePtr = nullptr;
    sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(systemAbilityId_);
    if (iRemoteObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("GetSystemAbility null");
        return TELEPHONY_CONNECT_SYSTEM_ABILITY_STUB_FAIL;
    }
    cellularCallInterfacePtr = iface_cast<CellularCall::CellularCallInterface>(iRemoteObjectPtr);
    if (!cellularCallInterfacePtr) {
        CALLMANAGER_ERR_LOG("get cellular call service is null");
        return TELEPHONY_LOCAL_PTR_NULL;
    }
    CALLMANAGER_INFO_LOG("Getting cellular call service succeeded");
    std::weak_ptr<CellularCallIpcInterfaceProxy> weakPtr = shared_from_this();
    auto deathCallback = [weakPtr](const wptr<IRemoteObject> &object) {
        auto sharedPtr = weakPtr.lock();
        if (sharedPtr) {
            sharedPtr->OnDeath();
        }
    };
    cellularCallRecipient_ = (std::make_unique<CellularCall::CellularCallDeathRecipient>(deathCallback)).release();
    if (cellularCallRecipient_ == nullptr) {
        CALLMANAGER_INFO_LOG("create CellularCallDeathRecipient object failed!");
        return TELEPHONY_LOCAL_PTR_NULL;
    }
    if (!iRemoteObjectPtr->AddDeathRecipient(cellularCallRecipient_)) {
        CALLMANAGER_INFO_LOG("failed to subscribing to the cellular call service death notification");
        return TELEPHONY_ADD_DEATH_RECIPIENT_FAIL;
    }
    CALLMANAGER_INFO_LOG("Succeeded in subscribing to the cellular call service death notification");
    cellularCallInterfacePtr_ = cellularCallInterfacePtr;
    int32_t ret = RegisterCallBackFun();
    if (ret != TELEPHONY_NO_ERROR) {
        return ret;
    }
    connectState_ = true;
    CALLMANAGER_INFO_LOG("registering callback for cellular call service succeeded!");
    return TELEPHONY_NO_ERROR;
}

int32_t CellularCallIpcInterfaceProxy::RegisterCallBackFun()
{
    if (cellularCallInterfacePtr_ == nullptr) {
        CALLMANAGER_INFO_LOG("cellularCallInterfacePtr_ is null");
        return TELEPHONY_LOCAL_PTR_NULL;
    }
    cellularCallCallbackPtr_ = (std::make_unique<CallStatusCallback>()).release();
    if (cellularCallCallbackPtr_ == nullptr) {
        Clean();
        CALLMANAGER_ERR_LOG("create CellularCallCallback object failed!");
        return TELEPHONY_FAIL;
    }
    CALLMANAGER_INFO_LOG("create CellularCallCallback object success!");
    int32_t ret = cellularCallInterfacePtr_->RegisterCallManagerCallBack(cellularCallCallbackPtr_);
    if (ret != TELEPHONY_NO_ERROR) {
        Clean();
        CALLMANAGER_ERR_LOG("register callback to cellular call service failed,result: %{public}X", ret);
        return TELEPHONY_REGISTER_CALLBACK_FAIL;
    }
    return TELEPHONY_NO_ERROR;
}

void CellularCallIpcInterfaceProxy::DisconnectService()
{
    Clean();
}

void CellularCallIpcInterfaceProxy::ReConnectService()
{
    if (cellularCallInterfacePtr_ == nullptr) {
        CALLMANAGER_INFO_LOG(
            "cellular call service not yet connected, try to connect cellular call service now...");
        int32_t result = ConnectService();
        if (result != TELEPHONY_NO_ERROR) {
            CALLMANAGER_ERR_LOG("Connect service: %{public}X", result);
            return;
        }
    }
}

void CellularCallIpcInterfaceProxy::OnDeath()
{
    Clean();
    NotifyDeath();
}

void CellularCallIpcInterfaceProxy::Clean()
{
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientlock_);
    if (cellularCallRecipient_) {
        cellularCallRecipient_.clear();
        cellularCallRecipient_ = nullptr;
    }
    if (cellularCallInterfacePtr_) {
        cellularCallInterfacePtr_.clear();
        cellularCallInterfacePtr_ = nullptr;
    }
    if (cellularCallCallbackPtr_) {
        cellularCallCallbackPtr_.clear();
        cellularCallCallbackPtr_ = nullptr;
    }
    connectState_ = false;
}

void CellularCallIpcInterfaceProxy::NotifyDeath()
{
    CALLMANAGER_INFO_LOG("service is dead, connect again");
#ifdef RECONNECT_MAX_TRY_COUNT
    for (uint32_t i = 0; i < CONNECT_MAX_TRY_COUNT; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(CONNECT_SERVICE_WAIT_TIME));
        int32_t result = ConnectService();
        if (result != TELEPHONY_NO_ERROR) {
            CALLMANAGER_INFO_LOG("connect cellular call service successful");
            return;
        }
    }
    CALLMANAGER_INFO_LOG("connect cellular call service failed");
#endif
    Timer::start(CONNECT_SERVICE_WAIT_TIME, CellularCallIpcInterfaceProxy::task);
}

int CellularCallIpcInterfaceProxy::Dial(const CellularCall::CellularCallInfo &callInfo)
{
    ReConnectService();
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->Dial(callInfo);
    if (errCode != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("dial failed, errcode:%{public}d", errCode);
        return TELEPHONY_FAIL;
    }
    return TELEPHONY_NO_ERROR;
}

int CellularCallIpcInterfaceProxy::End(const CellularCall::CellularCallInfo &callInfo)
{
    ReConnectService();
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->End(callInfo);
    if (errCode != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("ending call failed, errcode:%{public}d", errCode);
        return TELEPHONY_FAIL;
    }
    return TELEPHONY_NO_ERROR;
}

int CellularCallIpcInterfaceProxy::Reject(const CellularCall::CellularCallInfo &callInfo)
{
    ReConnectService();
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->Reject(callInfo);
    if (errCode != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("rejecting call failed, errcode:%{public}d", errCode);
        return TELEPHONY_FAIL;
    }
    return TELEPHONY_NO_ERROR;
}

int CellularCallIpcInterfaceProxy::Answer(const CellularCall::CellularCallInfo &callInfo)
{
    ReConnectService();
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->Answer(callInfo);
    if (errCode != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("answering call failed, errcode:%{public}d", errCode);
        return TELEPHONY_FAIL;
    }
    return TELEPHONY_NO_ERROR;
}

int CellularCallIpcInterfaceProxy::IsUrgentCall(const std::string &phoneNum, int32_t slotId)
{
    ReConnectService();
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->IsUrgentCall(phoneNum, slotId);
    if (errCode != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("IsUrgentCall failed, errcode:%{public}d", errCode);
        return TELEPHONY_FAIL;
    }
    return TELEPHONY_NO_ERROR;
}

int CellularCallIpcInterfaceProxy::RegisterCallBack(const sptr<TelephonyCallManager::ICallStatusCallback> &callback)
{
    ReConnectService();
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->RegisterCallManagerCallBack(callback);
    if (errCode != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("registerCallBack failed, errcode:%{public}d", errCode);
        return TELEPHONY_FAIL;
    }
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS