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
#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
#ifdef RECONNECT_MAX_TRY_COUNT
constexpr uint16_t CONNECT_MAX_TRY_COUNT = 5;
#endif
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
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
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
    if (cellularCallInterfacePtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallInterfacePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    cellularCallCallbackPtr_ = (std::make_unique<CallStatusCallback>()).release();
    if (cellularCallCallbackPtr_ == nullptr) {
        Clean();
        TELEPHONY_LOGE("cellularCallCallbackPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = cellularCallInterfacePtr_->RegisterCallManagerCallBack(cellularCallCallbackPtr_);
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
#ifdef CELLULAR_SUPPORT
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
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
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
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        CallManagerHisysevent::WriteDialCallFaultEvent(callInfo.accountId, static_cast<int32_t>(callInfo.callType),
            callInfo.videoState, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL, "ReConnectService failed");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    TELEPHONY_LOGI("callType:%{public}d", callInfo.callType);
    int errCode = cellularCallInterfacePtr_->Dial(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("dial failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::HangUp(const CellularCallInfo &callInfo, CallSupplementType type)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        CallManagerHisysevent::WriteHangUpFaultEvent(
            callInfo.accountId, callInfo.callId, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL, "HangUp ipc reconnect failed");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->HangUp(callInfo, type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("hangup call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::Reject(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        CallManagerHisysevent::WriteHangUpFaultEvent(
            callInfo.accountId, callInfo.callId, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL, "Reject ipc reconnect failed");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->Reject(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("rejecting call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::Answer(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        CallManagerHisysevent::WriteAnswerCallFaultEvent(callInfo.accountId, callInfo.callId, callInfo.videoState,
            TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL, "ipc reconnect failed");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->Answer(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("answering call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::HoldCall(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->HoldCall(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("holding call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::UnHoldCall(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->UnHoldCall(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("unhold call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::SwitchCall(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SwitchCall(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("switch call failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::IsEmergencyPhoneNumber(const std::string &phoneNum, int32_t slotId, bool &enabled)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr_->IsEmergencyPhoneNumber(slotId, phoneNum, enabled);
}

int CellularCallConnection::CombineConference(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->CombineConference(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("combine conference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::SeparateConference(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SeparateConference(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("separate conference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::StartDtmf(char cDTMFCode, const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->StartDtmf(cDTMFCode, callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("start dtmf failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::StopDtmf(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->StopDtmf(callInfo);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop dtmf failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::SendDtmf(char cDTMFCode, const std::string &phoneNum)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
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
    int errCode = cellularCallInterfacePtr_->SendDtmf(cDTMFCode, callInfo);
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
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr_->SetCallTransferInfo(slotId, info);
}

int CellularCallConnection::CanSetCallTransferTime(int32_t slotId, bool &result)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] ipc reconnect failed!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr_->CanSetCallTransferTime(slotId, result);
}

int CellularCallConnection::GetCallTransferInfo(CallTransferType type, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr_->GetCallTransferInfo(slotId, type);
}

int CellularCallConnection::SetCallWaiting(bool activate, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SetCallWaiting(slotId, activate);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallWaiting failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::GetCallWaiting(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->GetCallWaiting(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCallWaiting failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::SetCallRestriction(const CallRestrictionInfo &info, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr_->SetCallRestriction(slotId, info);
}

int CellularCallConnection::GetCallRestriction(CallRestrictionType facType, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return cellularCallInterfacePtr_->GetCallRestriction(slotId, facType);
}

int CellularCallConnection::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SetDomainPreferenceMode(slotId, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallPreferenceMode failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::StartRtt(const CellularCallInfo &callInfo, std::u16string &msg)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t slotId = callInfo.slotId;
    int errCode = cellularCallInterfacePtr_->StartRtt(slotId, Str16ToStr8(msg));
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StartRtt failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::StopRtt(const CellularCallInfo &callInfo)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t slotId = callInfo.slotId;
    int errCode = cellularCallInterfacePtr_->StopRtt(slotId);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StopRtt failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int CellularCallConnection::RegisterCallBack(const sptr<ICallStatusCallback> &callback)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->RegisterCallManagerCallBack(callback);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("registerCallBack failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::ControlCamera(std::u16string cameraId, int32_t callingUid, int32_t callingPid)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->CtrlCamera(cameraId, callingUid, callingPid);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("cellularCallInterface CtrlCamera failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}
int32_t CellularCallConnection::SetPreviewWindow(VideoWindow &window)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode =
        cellularCallInterfacePtr_->SetPreviewWindow(window.x, window.y, window.z, window.width, window.height);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreviewWindow failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetDisplayWindow(VideoWindow &window)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode =
        cellularCallInterfacePtr_->SetDisplayWindow(window.x, window.y, window.z, window.width, window.height);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetDisplayWindow failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetCameraZoom(float zoomRatio)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SetCameraZoom(zoomRatio);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCameraZoom failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetPausePicture(std::u16string path)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SetPauseImage(path);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPauseImage failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetDeviceDirection(int32_t rotation)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SetDeviceDirection(rotation);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetDeviceDirection failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetImsSwitchStatus(int32_t slotId, bool active)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SetImsSwitchStatus(slotId, active);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsSwitchStatus failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::GetImsSwitchStatus(int32_t slotId, bool &enabled)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->GetImsSwitchStatus(slotId, enabled);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetImsSwitchStatus failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SendUpdateCallMediaModeRequest(const CellularCallInfo &callInfo, ImsCallMode mode)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->UpdateImsCallMode(callInfo, mode);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("send media modify request failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetImsConfig(ImsConfigItem item, const std::string &value, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SetImsConfig(slotId, item, value);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsConfig for string value failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetImsConfig(ImsConfigItem item, int32_t value, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SetImsConfig(slotId, item, value);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsConfig for int value failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::GetImsConfig(ImsConfigItem item, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->GetImsConfig(slotId, item);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetImsConfig failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetImsFeatureValue(FeatureType type, int32_t value, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SetImsFeatureValue(slotId, type, value);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsFeatureValue failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::GetImsFeatureValue(FeatureType type, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->GetImsFeatureValue(slotId, type);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetImsFeatureValue failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::InviteToConference(const std::vector<std::string> &numberList, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->InviteToConference(slotId, numberList);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("InviteToConference failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallConnection::SetMute(int32_t mute, int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int errCode = cellularCallInterfacePtr_->SetMute(slotId, mute);
    if (errCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetMute failed, errcode:%{public}d", errCode);
        return errCode;
    }
    return TELEPHONY_SUCCESS;
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
} // namespace Telephony
} // namespace OHOS
