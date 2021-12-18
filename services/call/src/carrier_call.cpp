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

#include "carrier_call.h"

#include <securec.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "call_number_utils.h"
#include "cellular_call_ipc_interface_proxy.h"

namespace OHOS {
namespace Telephony {
CarrierCall::CarrierCall(DialParaInfo &info)
    : CallBase(info), dialScene_(CALL_NORMAL), slotId_(info.accountId), index_(info.index), isEcc_(info.isEcc)
{}

CarrierCall::CarrierCall(DialParaInfo &info, AppExecFwk::PacMap &extras)
    : CallBase(info, extras), dialScene_((DialScene)extras.GetIntValue("dialScene")), slotId_(info.accountId),
    index_(info.index), isEcc_(info.isEcc)
{}

CarrierCall::~CarrierCall() {}

int32_t CarrierCall::CarrierDialingProcess()
{
    int32_t ret = DialCallBase();
    if (ret != TELEPHONY_SUCCESS) {
        CarrierHangUpCall();
    }
    return ret;
}

int32_t CarrierCall::CarrierAcceptCall(int32_t videoState)
{
    CellularCallInfo callInfo;
    int32_t ret = AcceptCallBase();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Accept failed!");
        return CALL_ERR_ACCEPT_FAILED;
    }
    PackCellularCallInfo(callInfo);
    ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->Answer(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Accept failed!");
        return CALL_ERR_ACCEPT_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CarrierCall::CarrierRejectCall(bool isSendSms, std::string &content)
{
    CellularCallInfo callInfo;
    int32_t ret = RejectCallBase();
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    PackCellularCallInfo(callInfo);
    TelCallState state = GetTelCallState();
    if (state == CALL_STATUS_INCOMING) {
        ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->Reject(callInfo);
    } else {
        ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->HangUp(callInfo);
    }
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Reject failed!");
        return CALL_ERR_REJECT_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CarrierCall::CarrierHangUpCall()
{
    int32_t ret = CALL_ERR_HANGUP_FAILED;
    CellularCallInfo callInfo;
    PackCellularCallInfo(callInfo);
    int64_t policyFlag = GetPolicyFlag();
    if (policyFlag & POLICY_FLAG_HANG_UP_ACTIVE) {
        ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->CallSupplement(TYPE_HANG_UP_ACTIVE);
    } else {
        ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->HangUp(callInfo);
    }
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("End failed!");
        return CALL_ERR_HANGUP_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CarrierCall::CarrierHoldCall()
{
    CellularCallInfo callInfo;
    PackCellularCallInfo(callInfo);
    int32_t ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->HoldCall(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Hold failed!");
        return CALL_ERR_HOLD_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CarrierCall::CarrierUnHoldCall()
{
    CellularCallInfo callInfo;
    PackCellularCallInfo(callInfo);
    int32_t ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->UnHoldCall(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UnHold failed!");
        return CALL_ERR_UNHOLD_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

void CarrierCall::GetCallAttributeCarrierInfo(CallAttributeInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    info.isEcc = isEcc_;
    info.accountId = slotId_;
    GetCallAttributeBaseInfo(info);
}

bool CarrierCall::GetEmergencyState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return isEcc_;
}

int32_t CarrierCall::CarrierSwitchCall()
{
    int32_t ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->SwitchCall();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SwitchCall failed!");
        return CALL_ERR_UNHOLD_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CarrierCall::GetSlotId()
{
    return slotId_;
}

int32_t CarrierCall::CarrierCombineConference()
{
    CellularCallInfo callInfo;
    PackCellularCallInfo(callInfo);
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->CombineConference(callInfo);
}

int32_t CarrierCall::CarrierSeparateConference()
{
    CellularCallInfo callInfo;
    PackCellularCallInfo(callInfo);
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->SeparateConference(callInfo);
}

int32_t CarrierCall::IsSupportConferenceable()
{
    return GetEmergencyState() != true ? TELEPHONY_SUCCESS : CALL_ERR_EMERGENCY_UNSOPPORT_CONFERENCEABLE;
}

void CarrierCall::PackCellularCallInfo(CellularCallInfo &callInfo)
{
    callInfo.callId = callId_;
    callInfo.callType = callType_;
    callInfo.videoState = (int32_t)videoState_;
    callInfo.index = index_;
    callInfo.slotId = slotId_;
    callInfo.accountId = slotId_;
    (void)memset_s(callInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen);
    if (memcpy_s(callInfo.phoneNum, kMaxNumberLen, accountNumber_.c_str(), accountNumber_.length()) != 0) {
        TELEPHONY_LOGW("memcpy_s failed!");
        return;
    }
}

int32_t CarrierCall::StartDtmf(char str)
{
    CellularCallInfo callInfo;
    PackCellularCallInfo(callInfo);
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->StartDtmf(str, callInfo);
}

int32_t CarrierCall::StopDtmf()
{
    CellularCallInfo callInfo;
    PackCellularCallInfo(callInfo);
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->StopDtmf(callInfo);
}

int32_t CarrierCall::SendDtmf(std::string &phoneNum, char str)
{
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->SendDtmf(str, phoneNum);
}

int32_t CarrierCall::SendBurstDtmf(std::string &phoneNum, std::string str, int32_t on, int32_t off)
{
    PhoneNetType phoneNetType = PhoneNetType::PHONE_TYPE_CDMA;
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->SendDtmfString(
        str, phoneNum, phoneNetType, on, off);
}
} // namespace Telephony
} // namespace OHOS
