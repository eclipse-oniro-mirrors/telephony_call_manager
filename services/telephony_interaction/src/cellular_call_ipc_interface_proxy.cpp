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
#include "cellular_call_death_recipient.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
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
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("connect service failed,errCode: %{public}X", result);
        Timer::start(CONNECT_SERVICE_WAIT_TIME, CellularCallIpcInterfaceProxy::task);
        return;
    }
    TELEPHONY_LOGI("connected to cellular call service successfully!");
}

void CellularCallIpcInterfaceProxy::unInit()
{
    DisconnectService();
}

void CellularCallIpcInterfaceProxy::task()
{
    int32_t ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->ConnectService();
    if (ret != TELEPHONY_SUCCESS) {
        return;
    }
    DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->ThreadExit();
}

int32_t CellularCallIpcInterfaceProxy::ConnectService()
{
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
    if (cellularCallInterfacePtr_ != nullptr) {
        TELEPHONY_LOGE("cellularCallInterfacePtr_ is nullptr!");
        return TELEPHONY_SUCCESS;
    }
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("managerPtr is nullptr!");
        return TELEPHONY_ERR_FAIL;
    }
    sptr<CellularCallInterface> cellularCallInterfacePtr = nullptr;
    sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(systemAbilityId_);
    if (iRemoteObjectPtr == nullptr) {
        TELEPHONY_LOGE("iRemoteObjectPtr is nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    cellularCallInterfacePtr = iface_cast<CellularCallInterface>(iRemoteObjectPtr);
    if (!cellularCallInterfacePtr) {
        TELEPHONY_LOGE("cellularCallInterfacePtr is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::weak_ptr<CellularCallIpcInterfaceProxy> weakPtr = shared_from_this();
    auto deathCallback = [weakPtr](const wptr<IRemoteObject> &object) {
        auto sharedPtr = weakPtr.lock();
        if (sharedPtr) {
            sharedPtr->OnDeath();
        }
    };
    cellularCallRecipient_ = (std::make_unique<CellularCallDeathRecipient>(deathCallback)).release();
    if (cellularCallRecipient_ == nullptr) {
        TELEPHONY_LOGE("cellularCallRecipient_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!iRemoteObjectPtr->AddDeathRecipient(cellularCallRecipient_)) {
        TELEPHONY_LOGE("AddDeathRecipient failed!");
        return TELEPHONY_ERR_ADD_DEATH_RECIPIENT_FAIL;
    }
    cellularCallInterfacePtr_ = cellularCallInterfacePtr;
    int32_t ret = RegisterCallBackFun();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterCallBackFun failed!");
        return ret;
    }
    connectState_ = true;
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallIpcInterfaceProxy::RegisterCallBackFun()
{
    if (cellularCallInterfacePtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallInterfacePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    cellularCallCallbackPtr_ = (std::make_unique<CallStatusCallback>()).release();
    if (cellularCallCallbackPtr_ == nullptr) {
        Clean();
        TELEPHONY_LOGE("cellularCallCallbackPtr_ is nullptr!");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t ret = cellularCallInterfacePtr_->RegisterCallManagerCallBack(cellularCallCallbackPtr_);
    if (ret != TELEPHONY_SUCCESS) {
        Clean();
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

void CellularCallIpcInterfaceProxy::DisconnectService()
{
    Clean();
}

int32_t CellularCallIpcInterfaceProxy::ReConnectService()
{
    if (cellularCallInterfacePtr_ == nullptr) {
        TELEPHONY_LOGI("try to reconnect cellular call service now...");
        int32_t result = ConnectService();
        if (result != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("Connect service: %{public}d", result);
            return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
        }
    }
    return TELEPHONY_SUCCESS;
}

void CellularCallIpcInterfaceProxy::OnDeath()
{
    Clean();
    NotifyDeath();
}

void CellularCallIpcInterfaceProxy::Clean()
{
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
    if (cellularCallRecipient_ != nullptr) {
        cellularCallRecipient_.clear();
        cellularCallRecipient_ = nullptr;
    }
    if (cellularCallInterfacePtr_ != nullptr) {
        cellularCallInterfacePtr_.clear();
        cellularCallInterfacePtr_ = nullptr;
    }
    if (cellularCallCallbackPtr_ != nullptr) {
        cellularCallCallbackPtr_.clear();
        cellularCallCallbackPtr_ = nullptr;
    }
    connectState_ = false;
}

void CellularCallIpcInterfaceProxy::NotifyDeath()
{
    TELEPHONY_LOGI("service is dead, connect again");
#ifdef RECONNECT_MAX_TRY_COUNT
    for (uint32_t i = 0; i < CONNECT_MAX_TRY_COUNT; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(CONNECT_SERVICE_WAIT_TIME));
        int32_t result = ConnectService();
        if (result != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGI("connect cellular call service successful");
            return;
        }
    }
    TELEPHONY_LOGI("connect cellular call service failed");
#endif
    Timer::start(CONNECT_SERVICE_WAIT_TIME, CellularCallIpcInterfaceProxy::task);
}

int CellularCallIpcInterfaceProxy::Dial(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->Dial(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("dial failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::HangUp(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->HangUp(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("hangup call failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::Reject(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->Reject(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("rejecting call failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::Answer(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->Answer(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("answering call failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::HoldCall(const CellularCallInfo &callInfo)
{
    ReConnectService();
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->HoldCall(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("holding call failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::UnHoldCall(const CellularCallInfo &callInfo)
{
    ReConnectService();
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->UnHoldCall(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("unhold call failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::SwitchCall()
{
    ReConnectService();
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        TELEPHONY_LOGE("SwitchCall return, memset_s failed.");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->SwitchCall(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("switch call failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::IsEmergencyPhoneNumber(
    const std::string &phoneNum, int32_t slotId, int32_t &errorCode)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return cellularCallInterfacePtr_->IsEmergencyPhoneNumber(phoneNum, slotId, errorCode);
}

int CellularCallIpcInterfaceProxy::CombineConference(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->CombineConference(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("combine conference failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::SeparateConference(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->SeparateConference(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("separate conference failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::CallSupplement(CallSupplementType type)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->CallSupplement(type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("call supplement failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::StartDtmf(char cDTMFCode, const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->StartDtmf(cDTMFCode, callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("start dtmf failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::StopDtmf(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->StopDtmf(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop dtmf failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::SendDtmf(char cDTMFCode, const std::string &phoneNum)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        TELEPHONY_LOGE("send dtmf return, memset_s failed.");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    if (strcpy_s(callInfo.phoneNum, strlen(phoneNum.c_str()) + 1, phoneNum.c_str()) != EOK) {
        TELEPHONY_LOGE("send dtmf return, strcpy_s fail.");
        return TELEPHONY_ERR_STRCPY_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->SendDtmf(cDTMFCode, callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("send dtmf failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::SendDtmfString(const std::string &dtmfCodeStr, const std::string &phoneNum,
    PhoneNetType phoneNetType, int32_t switchOn, int32_t switchOff)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->SendDtmfString(dtmfCodeStr, phoneNum, switchOn, switchOff);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("send dtmf string failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::SetCallTransferInfo(const CallTransferInfo &info, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return cellularCallInterfacePtr_->SetCallTransferInfo(info, slotId);
}

int CellularCallIpcInterfaceProxy::GetCallTransferInfo(CallTransferType type, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return cellularCallInterfacePtr_->GetCallTransferInfo(type, slotId);
}

int CellularCallIpcInterfaceProxy::SetCallWaiting(bool activate, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->SetCallWaiting(activate, slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallWaiting failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::GetCallWaiting(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->GetCallWaiting(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCallWaiting failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::SetCallRestriction(const CallRestrictionInfo &info, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return cellularCallInterfacePtr_->SetCallRestriction(info, slotId);
}

int CellularCallIpcInterfaceProxy::GetCallRestriction(CallRestrictionType facType, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return cellularCallInterfacePtr_->GetCallRestriction(facType, slotId);
}

int CellularCallIpcInterfaceProxy::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->SetCallPreferenceMode(slotId, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallPreferenceMode failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallIpcInterfaceProxy::RegisterCallBack(const sptr<ICallStatusCallback> &callback)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->RegisterCallManagerCallBack(callback);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("registerCallBack failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallIpcInterfaceProxy::ControlCamera(
    std::u16string cameraId, std::u16string callingPackage, int32_t callingUid, int32_t callingPid)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->CtrlCamera(cameraId, callingPackage, callingUid, callingPid);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("cellularCallInterface CtrlCamera failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallIpcInterfaceProxy::SetPreviewWindow(VideoWindow &window)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode =
        cellularCallInterfacePtr_->SetPreviewWindow(window.x, window.y, window.z, window.width, window.height);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreviewWindow failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallIpcInterfaceProxy::SetDisplayWindow(VideoWindow &window)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode =
        cellularCallInterfacePtr_->SetDisplayWindow(window.x, window.y, window.z, window.width, window.height);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetDisplayWindow failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallIpcInterfaceProxy::SetCameraZoom(float zoomRatio)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->SetCameraZoom(zoomRatio);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCameraZoom failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallIpcInterfaceProxy::SetPausePicture(std::u16string path)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->SetPauseImage(path);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPauseImage failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallIpcInterfaceProxy::SetDeviceDirection(int32_t rotation)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->SetDeviceDirection(rotation);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetDeviceDirection failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallIpcInterfaceProxy::SetLteImsSwitchStatus(int32_t slotId, bool active)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->SetLteImsSwitchStatus(slotId, active);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetLteImsSwitchStatus failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    TELEPHONY_LOGI("CallManager SetLteImsSwitchStatus successfully!");
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallIpcInterfaceProxy::GetLteImsSwitchStatus(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int errCode = cellularCallInterfacePtr_->GetLteImsSwitchStatus(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetLteImsSwitchStatus failed, errcode:%{public}d", errCode);
        return TELEPHONY_ERR_FAIL;
    }
    TELEPHONY_LOGI("CallManager GetLteImsSwitchStatus successfully!");
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
