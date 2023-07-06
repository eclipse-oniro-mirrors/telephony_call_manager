/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#include "cellular_call_proxy.h"
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t MAX_SIZE = 10;

int32_t CellularCallProxy::Dial(const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::DIAL), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::HangUp(const CellularCallInfo &callInfo, CallSupplementType type)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32((int32_t)type)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::HANG_UP), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::Reject(const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::REJECT), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::Answer(const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::ANSWER), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::HoldCall(const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::HOLD_CALL), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::UnHoldCall(const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::UN_HOLD_CALL), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SwitchCall(const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SWITCH_CALL), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback)
{
    if (callback == nullptr) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRemoteObject(callback->AsObject().GetRefPtr())) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::REGISTER_CALLBACK), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::UnRegisterCallManagerCallBack()
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::UNREGISTER_CALLBACK), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::IsEmergencyPhoneNumber(int32_t slotId, const std::string &phoneNum, bool &enabled)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteString(phoneNum)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::EMERGENCY_CALL), in,
        out, option);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function FormatPhoneNumberToE164 call failed! errCode:%{public}d", ret);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    result = out.ReadInt32();
    if (result == TELEPHONY_SUCCESS) {
        enabled = out.ReadBool();
    }
    return result;
}

int32_t CellularCallProxy::CombineConference(const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::COMBINE_CONFERENCE), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SeparateConference(const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SEPARATE_CONFERENCE), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::InviteToConference(int32_t slotId, const std::vector<std::string> &numberList)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteStringVector(numberList)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::INVITE_TO_CONFERENCE), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::KickOutFromConference(int32_t slotId, const std::vector<std::string> &numberList)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteStringVector(numberList)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::KICK_OUT_CONFERENCE), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::HangUpAllConnection()
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::HANG_UP_ALL_CONNECTION), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetReadyToCall(int32_t slotId, int32_t callType, bool isReadyToCall)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(callType)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteBool(isReadyToCall)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d] function Remote() return nullptr!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error =
        remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_READY_TO_CALL), in, out, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("Function SetReadyToCall! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return out.ReadInt32();
}

int32_t CellularCallProxy::HangUpAllConnection(int32_t slotId)
{
    return TELEPHONY_ERR_SUCCESS;
}

int32_t CellularCallProxy::UpdateImsCallMode(const CellularCallInfo &callInfo, ImsCallMode mode)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32((int32_t)mode)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::UPDATE_CALL_MEDIA_MODE), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::StartDtmf(char cDtmfCode, const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt8(cDtmfCode)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::START_DTMF), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::StopDtmf(const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::STOP_DTMF), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SendDtmf(char cDtmfCode, const CellularCallInfo &callInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt8(cDtmfCode)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&callInfo, sizeof(CellularCallInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SEND_DTMF), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::StartRtt(int32_t slotId, const std::string &msg)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteString(msg)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::START_RTT), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::StopRtt(int32_t slotId)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::STOP_RTT), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetCallTransferInfo(int32_t slotId, const CallTransferInfo &ctInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteRawData((const void *)&ctInfo, sizeof(CallTransferInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_CALL_TRANSFER), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::CanSetCallTransferTime(int32_t slotId, bool &result)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t ret = TELEPHONY_SUCCESS;
    ret = SetCommonParamForMessageParcel(slotId, in);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteBool(result)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d] function Remote() return nullptr!", slotId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::CAN_SET_CALL_TRANSFER_TIME),
        in, out, option);
    if (error == ERR_NONE) {
        result = out.ReadBool();
        return out.ReadInt32();
    }

    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t CellularCallProxy::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteInt32((int32_t)type)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::GET_CALL_TRANSFER), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetCallWaiting(int32_t slotId, bool activate)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteBool(activate)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_CALL_WAITING), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::GetCallWaiting(int32_t slotId)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::GET_CALL_WAITING), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetCallRestriction(int32_t slotId, const CallRestrictionInfo &crInfo)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteRawData((const void *)&crInfo, sizeof(CallRestrictionInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_CALL_RESTRICTION), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::GetCallRestriction(int32_t slotId, CallRestrictionType facType)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteInt32((int32_t)facType)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::GET_CALL_RESTRICTION), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetDomainPreferenceMode(int32_t slotId, int32_t mode)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteInt32(mode)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_DOMAIN_PREFERENCE_MODE),
        in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::GetDomainPreferenceMode(int32_t slotId)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::GET_DOMAIN_PREFERENCE_MODE),
        in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetImsSwitchStatus(int32_t slotId, bool active)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteBool(active)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_IMS_SWITCH_STATUS),
        in, out, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function SetImsSwitchStatus failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return out.ReadInt32();
}

int32_t CellularCallProxy::GetImsSwitchStatus(int32_t slotId, bool &enabled)
{
    MessageOption option;
    MessageParcel out;
    MessageParcel in;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::GET_IMS_SWITCH_STATUS),
        in, out, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function GetImsSwitchStatus failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    enabled = out.ReadBool();
    return out.ReadInt32();
}

int32_t CellularCallProxy::SetVoNRState(int32_t slotId, int32_t state)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteInt32(state)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_VONR_SWITCH_STATUS), in,
        out, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function SetVoNRState failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return out.ReadInt32();
}

int32_t CellularCallProxy::GetVoNRState(int32_t slotId, int32_t &state)
{
    MessageOption option;
    MessageParcel out;
    MessageParcel in;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::GET_VONR_SWITCH_STATUS), in,
        out, option);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function GetImsSwitchStatus failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    state = out.ReadInt32();
    return out.ReadInt32();
}

int32_t CellularCallProxy::SetImsConfig(int32_t slotId, ImsConfigItem item, const std::string &value)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteInt32(item)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteString(value)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_IMS_CONFIG_STRING), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetImsConfig(int32_t slotId, ImsConfigItem item, int32_t value)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteInt32(item)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(value)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_IMS_CONFIG_INT), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::GetImsConfig(int32_t slotId, ImsConfigItem item)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteInt32(item)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::GET_IMS_CONFIG), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteInt32(type)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(value)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_IMS_FEATURE), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::GetImsFeatureValue(int32_t slotId, FeatureType type)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteInt32(type)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::GET_IMS_FEATURE), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::CtrlCamera(const std::u16string &cameraId, int32_t callingUid, int32_t callingPid)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteString16(cameraId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(callingUid)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(callingPid)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::CTRL_CAMERA), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetPreviewWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(x)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(y)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(z)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(width)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(height)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_PREVIEW_WINDOW), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetDisplayWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(x)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(y)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(z)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(width)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(height)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_DISPLAY_WINDOW), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetCameraZoom(float zoomRatio)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteFloat(zoomRatio)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_CAMERA_ZOOM), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetPauseImage(const std::u16string &path)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteString16(path)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_PAUSE_IMAGE), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetDeviceDirection(int32_t rotation)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(rotation)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_DEVICE_DIRECTION), in,
        out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetMute(int32_t slotId, int32_t mute)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    if (!in.WriteInt32(mute)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_MUTE), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::GetMute(int32_t slotId)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::GET_MUTE), in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetEmergencyCallList(int32_t slotId, std::vector<EmergencyCall>  &eccVec)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }

    if (eccVec.size() <= 0) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    if (!in.WriteInt32(static_cast<int32_t>(eccVec.size()))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    for (auto ecc : eccVec) {
        if (!(in.WriteString(ecc.eccNum) && in.WriteString(ecc.mcc) &&
                in.WriteInt32(static_cast<int32_t>(ecc.eccType)) &&
                in.WriteInt32(static_cast<int32_t>(ecc.simpresent)) &&
                in.WriteInt32(static_cast<int32_t>(ecc.abnormalService)))) {
            return TELEPHONY_ERR_WRITE_DATA_FAIL;
        }
    }

    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::SET_EMERGENCY_CALL_LIST),
        in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t CellularCallProxy::SetCommonParamForMessageParcel(int32_t slotId, MessageParcel &in)
{
    if (!in.WriteInterfaceToken(CellularCallProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(MAX_SIZE)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CellularCallProxy::CloseUnFinishedUssd(int32_t slotId)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    int32_t result = TELEPHONY_SUCCESS;
    result = SetCommonParamForMessageParcel(slotId, in);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(CellularCallInterfaceCode::CLOSE_UNFINISHED_USSD),
        in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}
} // namespace Telephony
} // namespace OHOS
