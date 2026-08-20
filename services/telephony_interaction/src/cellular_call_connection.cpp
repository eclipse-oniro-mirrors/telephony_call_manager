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

#include "cellular_call_connection.h"

#include <thread>

#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "cellular_call_proxy.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
#ifdef RECONNECT_MAX_TRY_COUNT
constexpr uint16_t CONNECT_MAX_TRY_COUNT = 5;
#endif
constexpr uint16_t DELAY_TIME = 100;

CellularCallConnection::CellularCallConnection()
    : systemAbilityId_(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID), cellularCallCallbackPtr_(nullptr),
    cellularCallInterfacePtr_(nullptr), connectState_(false)
{}

CellularCallConnection::~CellularCallConnection()
{
    UnInit();
}

void CellularCallConnection::Init(int32_t systemAbilityId)
{
    TELEPHONY_LOGI("CellularCallConnection Init start");
    if (connectState_) {
        TELEPHONY_LOGE("Init, connectState is true");
        return;
    }
    systemAbilityId_ = systemAbilityId;
    ConnectService();

    statusChangeListener_ = new (std::nothrow) SystemAbilityListener();
    if (statusChangeListener_ == nullptr) {
        TELEPHONY_LOGE("Init, failed to create statusChangeListener.");
        return;
    }
    auto managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("Init, get system ability manager error.");
        return;
    }
    int32_t ret = managerPtr->SubscribeSystemAbility(systemAbilityId_, statusChangeListener_);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Init, failed to subscribe sa:%{public}d", systemAbilityId_);
        return;
    }

    TELEPHONY_LOGI("connected to cellular call service successfully!");
}

void CellularCallConnection::UnInit()
{
    DisconnectService();
    if (statusChangeListener_ != nullptr) {
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy != nullptr) {
            samgrProxy->UnSubscribeSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID, statusChangeListener_);
            statusChangeListener_ = nullptr;
        }
    }
}

bool CellularCallConnection::IsConnect() const
{
    return connectState_;
}

int32_t CellularCallConnection::ConnectService()
{
    std::lock_guard<ffrt::recursive_mutex> lock(clientLock_);
    if (cellularCallInterfacePtr_ != nullptr) {
        return TELEPHONY_SUCCESS;
    }
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    sptr<CellularCallInterface> cellularCallInterfacePtr = nullptr;
    sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(systemAbilityId_);
    if (iRemoteObjectPtr == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    cellularCallInterfacePtr = iface_cast<CellularCallInterface>(iRemoteObjectPtr);
    if (!cellularCallInterfacePtr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    cellularCallInterfacePtr_ = cellularCallInterfacePtr;
    int32_t ret = RegisterCallBackFun();
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    connectState_ = true;
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::RegisterCallBackFun()
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("cellularCallInterfacePtr is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    cellularCallCallbackPtr_ = (std::make_unique<CallStatusCallback>()).release();
    if (cellularCallCallbackPtr_ == nullptr) {
        Clean();
        TELEPHONY_LOGE("cellularCallCallbackPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = cellularCallInterfacePtr->RegisterCallManagerCallBack(cellularCallCallbackPtr_);
    if (ret != TELEPHONY_SUCCESS) {
        Clean();
        return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

void CellularCallConnection::DisconnectService()
{
    Clean();
}

int32_t CellularCallConnection::ReConnectService()
{
#ifdef ABILITY_CELLULAR_SUPPORT
    if (cellularCallInterfacePtr_ == nullptr) {
        TELEPHONY_LOGI("try to reconnect cellular call service now...");
        int32_t result = ConnectService();
        if (result != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("Connect service: %{public}d", result);
            return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
        }
    }
#endif
    return TELEPHONY_SUCCESS;
}

void CellularCallConnection::Clean()
{
    std::lock_guard<ffrt::recursive_mutex> lock(clientLock_);
    UnRegisterCallBack();
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

int CellularCallConnection::Dial(const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        CallManagerHisysevent::WriteDialCallFaultEvent(callInfo.accountId, static_cast<int32_t>(callInfo.callType),
            callInfo.videoState, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL, "ReConnectService failed");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    TELEPHONY_LOGI("callType:%{public}d", callInfo.callType);
    int errCode = cellularCallInterfacePtr->Dial(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("dial failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::HangUp(const CellularCallInfo &callInfo, CallSupplementType type)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        CallManagerHisysevent::WriteHangUpFaultEvent(
            callInfo.accountId, callInfo.callId, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL, "HangUp ipc reconnect failed");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->HangUp(callInfo, type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("hangup call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::Reject(const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        CallManagerHisysevent::WriteHangUpFaultEvent(
            callInfo.accountId, callInfo.callId, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL, "Reject ipc reconnect failed");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->Reject(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("rejecting call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::Answer(const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        CallManagerHisysevent::WriteAnswerCallFaultEvent(callInfo.accountId, callInfo.callId, callInfo.videoState,
            TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL, "ipc reconnect failed");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->Answer(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("answering call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::HoldCall(const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->HoldCall(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("holding call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::UnHoldCall(const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->UnHoldCall(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("unhold call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::SwitchCall(const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SwitchCall(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("switch call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::IsEmergencyPhoneNumber(const std::string &phoneNum, int32_t slotId, bool &enabled)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr->IsEmergencyPhoneNumber(slotId, phoneNum, enabled);
}

int CellularCallConnection::CombineConference(const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->CombineConference(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("combine conference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::SeparateConference(const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SeparateConference(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("separate conference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::KickOutFromConference(const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->KickOutFromConference(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Kick out from conference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::StartDtmf(char cDTMFCode, const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->StartDtmf(cDTMFCode, callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("start dtmf failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::StopDtmf(const CellularCallInfo &callInfo)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->StopDtmf(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop dtmf failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::PostDialProceed(const CellularCallInfo &callInfo, const bool proceed)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->PostDialProceed(callInfo, proceed);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("post dial continue failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::SendDtmf(char cDTMFCode, const std::string &phoneNum)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        TELEPHONY_LOGE("send dtmf return, memset_s failed.");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    if (phoneNum.length() + 1 > static_cast<size_t>(kMaxNumberLen)) {
        return TELEPHONY_ERR_STRCPY_FAIL;
    }
    if (strcpy_s(callInfo.phoneNum, strlen(phoneNum.c_str()) + 1, phoneNum.c_str()) != EOK) {
        TELEPHONY_LOGE("send dtmf return, strcpy_s fail.");
        return TELEPHONY_ERR_STRCPY_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SendDtmf(cDTMFCode, callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("send dtmf failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::SendDtmfString(const std::string &dtmfCodeStr, const std::string &phoneNum,
    PhoneNetType phoneNetType, int32_t switchOn, int32_t switchOff)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::SetCallTransferInfo(const CallTransferInfo &info, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr->SetCallTransferInfo(slotId, info);
}

int CellularCallConnection::CanSetCallTransferTime(int32_t slotId, bool &result)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr->CanSetCallTransferTime(slotId, result);
}

int CellularCallConnection::GetCallTransferInfo(CallTransferType type, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr->GetCallTransferInfo(slotId, type);
}

int CellularCallConnection::SetCallWaiting(bool activate, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetCallWaiting(slotId, activate);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallWaiting failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::GetCallWaiting(int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->GetCallWaiting(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCallWaiting failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::GetVideoCallWaiting(int32_t slotId, bool &enabled)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->GetVideoCallWaiting(slotId, enabled);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetVideoCallWaiting failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::SetCallRestriction(const CallRestrictionInfo &info, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr->SetCallRestriction(slotId, info);
}

int CellularCallConnection::GetCallRestriction(CallRestrictionType facType, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr->GetCallRestriction(slotId, facType);
}

int CellularCallConnection::SetCallRestrictionPassword(
    int32_t slotId, CallRestrictionType fac, const char *oldPassword, const char *newPassword)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr->SetCallRestrictionPassword(slotId, fac, oldPassword, newPassword);
}

int CellularCallConnection::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetDomainPreferenceMode(slotId, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallPreferenceMode failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::RegisterCallBack(const sptr<ICallStatusCallback> &callback)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->RegisterCallManagerCallBack(callback);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("registerCallBack failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::UnRegisterCallBack()
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("cellularCallInterfacePtr is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int errCode = cellularCallInterfacePtr->UnRegisterCallManagerCallBack();
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UnRegisterCallBack failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::ControlCamera(
    int32_t slotId, int32_t index, std::string &cameraId, int32_t callingUid, int32_t callingPid)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    TELEPHONY_LOGI("CtrlCamera slotId:%{public}d, callingUid:%{public}d, callingPid:%{public}d",
        slotId, callingUid, callingPid);
    int errCode = cellularCallInterfacePtr->ControlCamera(slotId, index, cameraId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("cellularCallInterface CtrlCamera failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetPreviewWindow(
    int32_t slotId, int32_t index, std::string &surfaceId, sptr<Surface> surface)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetPreviewWindow(slotId, index, surfaceId, surface);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreviewWindow failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetDisplayWindow(
    int32_t slotId, int32_t index, std::string &surfaceId, sptr<Surface> surface)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetDisplayWindow(slotId, index, surfaceId, surface);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetDisplayWindow failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetCameraZoom(float zoomRatio)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetCameraZoom(zoomRatio);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCameraZoom failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetPausePicture(int32_t slotId, int32_t index, std::string &path)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetPausePicture(slotId, index, path);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPausePicture failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetDeviceDirection(int32_t slotId, int32_t index, int32_t rotation)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetDeviceDirection(slotId, index, rotation);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetDeviceDirection failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetImsSwitchStatus(int32_t slotId, bool active)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetImsSwitchStatus(slotId, active);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsSwitchStatus failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::GetImsSwitchStatus(int32_t slotId, bool &enabled)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->GetImsSwitchStatus(slotId, enabled);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetImsSwitchStatus failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::GetCarrierVtConfig(int32_t slotId, bool &enabled)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->GetCarrierVtConfig(slotId, enabled);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCarrierVtConfig failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetVoNRState(int32_t slotId, int32_t state)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetVoNRState(slotId, state);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetVoNRState failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::GetVoNRState(int32_t slotId, int32_t &state)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->GetVoNRState(slotId, state);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetVoNRState failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SendUpdateCallMediaModeRequest(const CellularCallInfo &callInfo, ImsCallMode mode)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SendUpdateCallMediaModeRequest(callInfo, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("send media modify request failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SendUpdateCallMediaModeResponse(const CellularCallInfo &callInfo, ImsCallMode mode)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SendUpdateCallMediaModeResponse(callInfo, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("send media modify request failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetImsConfig(ImsConfigItem item, const std::string &value, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetImsConfig(slotId, item, value);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsConfig for string value failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetImsConfig(ImsConfigItem item, int32_t value, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetImsConfig(slotId, item, value);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsConfig for int value failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::GetImsConfig(ImsConfigItem item, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->GetImsConfig(slotId, item);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetImsConfig failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetImsFeatureValue(FeatureType type, int32_t value, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetImsFeatureValue(slotId, type, value);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsFeatureValue failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::GetImsFeatureValue(FeatureType type, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->GetImsFeatureValue(slotId, type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetImsFeatureValue failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::InviteToConference(const std::vector<std::string> &numberList, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->InviteToConference(slotId, numberList);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("InviteToConference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetMute(int32_t mute, int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetMute(slotId, mute);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetMute failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::CloseUnFinishedUssd(int32_t slotId)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->CloseUnFinishedUssd(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CloseUnFinishedUssd failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::CancelCallUpgrade(int32_t slotId, int32_t index)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->CancelCallUpgrade(slotId, index);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CancelCallUpgrade failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::RequestCameraCapabilities(int32_t slotId, int32_t index)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->RequestCameraCapabilities(slotId, index);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RequestCameraCapabilities failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::ClearAllCalls()
{
    if (!CallObjectManager::HasCallExist()) {
        TELEPHONY_LOGI("no call exist, no need to clear");
        return TELEPHONY_SUCCESS;
    }
    std::vector<CellularCallInfo> callsInfo;
    std::vector<CallAttributeInfo> infos = CallObjectManager::GetAllCallInfoList();
    for (auto &info : infos) {
        CellularCallInfo callInfo;
        callInfo.callId = info.callId;
        if (memset_s(callInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen) != EOK) {
            TELEPHONY_LOGE("memset_s fail");
            return TELEPHONY_ERR_MEMSET_FAIL;
        }
        if (memcpy_s(callInfo.phoneNum, kMaxNumberLen, info.accountNumber, strlen(info.accountNumber)) != EOK) {
            TELEPHONY_LOGE("memcpy_s fail");
            return TELEPHONY_ERR_MEMCPY_FAIL;
        }
        callInfo.slotId = info.accountId;
        callInfo.accountId = info.accountId;
        callInfo.callType = info.callType;
        callInfo.videoState = static_cast<int32_t>(info.videoState);
        callInfo.index = info.index;
        callsInfo.push_back(callInfo);
    }
    if (callsInfo.empty()) {
        TELEPHONY_LOGE("callsInfo is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_TIME));
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t errCode = cellularCallInterfacePtr->ClearAllCalls(callsInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ClearAllCalls fail, errcode:%{public}d", errCode);
    }
    return errCode;
}

void CellularCallConnection::SystemAbilityListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("SA:%{public}d is added!", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("add SA:%{public}d is invalid!", systemAbilityId);
        return;
    }

    auto cellularCallConnection = DelayedSingleton<CellularCallConnection>::GetInstance();
    if (cellularCallConnection == nullptr) {
        TELEPHONY_LOGE("cellularCallConnection is nullptr");
        return;
    }

    if (cellularCallConnection->IsConnect()) {
        TELEPHONY_LOGI("SA:%{public}d already connected!", systemAbilityId);
        return;
    }

    cellularCallConnection->Clean();
    int32_t res = cellularCallConnection->ReConnectService();
    if (res != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SA:%{public}d reconnect service failed!", systemAbilityId);
        return;
    }
    cellularCallConnection->ClearAllCalls();
    TELEPHONY_LOGI("SA:%{public}d reconnect service successfully!", systemAbilityId);
}

void CellularCallConnection::SystemAbilityListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("SA:%{public}d is removed!", systemAbilityId);
    auto cellularCallConnection = DelayedSingleton<CellularCallConnection>::GetInstance();
    if (cellularCallConnection == nullptr) {
        TELEPHONY_LOGE("cellularCallConnection is nullptr");
        return;
    }

    if (!cellularCallConnection->IsConnect()) {
        return;
    }

    cellularCallConnection->Clean();
}

int32_t CellularCallConnection::SendUssdResponse(int32_t slotId, const std::string &content)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t errCode = cellularCallInterfacePtr->SendUssdResponse(slotId, content);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SendUssdResponse failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

bool CellularCallConnection::IsMmiCode(int32_t slotId, std::string &number)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr->IsMmiCode(slotId, number);
}

#ifdef SUPPORT_RTT_CALL
int32_t CellularCallConnection::SetRttCapability(int32_t slotId, bool isEnable)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr->SetRttCapability(slotId, isEnable);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetRttCapability failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::UpdateImsRttCallMode(const CellularCallInfo &callInfo, ImsRTTCallMode mode)
{
    auto cellularCallInterfacePtr = GetCellCallInterface();
    if (cellularCallInterfacePtr == nullptr) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t slotId = callInfo.slotId;
    int32_t callId = callInfo.index;
    int errCode = cellularCallInterfacePtr->UpdateImsRttCallMode(slotId, callId, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateImsRttCallMode failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}
#endif

sptr<CellularCallInterface> CellularCallConnection::GetCellCallInterface()
{
    std::lock_guard<ffrt::recursive_mutex> lock(clientLock_);
    if (cellularCallInterfacePtr_ == nullptr) {
        if (ReConnectService() != TELEPHONY_SUCCESS) {
            return nullptr;
        }
    }
    return cellularCallInterfacePtr_;
}
} // namespace Telephony
} // namespace OHOS
