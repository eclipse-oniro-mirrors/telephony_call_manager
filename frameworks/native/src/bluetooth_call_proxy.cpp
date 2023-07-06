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

#include "bluetooth_call_proxy.h"

#include "message_option.h"
#include "message_parcel.h"

#include "call_manager_errors.h"

namespace OHOS {
namespace Telephony {
static const int32_t MAX_SIZE = 10000;
BluetoothCallProxy::BluetoothCallProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IBluetoothCall>(impl)
{}

int32_t BluetoothCallProxy::AnswerCall()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_ANSWER_CALL),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function AnswerCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::RejectCall()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_REJECT_CALL),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function RejectCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::HangUpCall()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(
        static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_DISCONNECT_CALL), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function HangUpCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::GetCallState()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_GET_CALL_STATE),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function GetCallState! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::HoldCall()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_HOLD_CALL),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function HoldCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::UnHoldCall()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_UNHOLD_CALL),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function UnHoldCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::SwitchCall()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_SWAP_CALL),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function UnHoldCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::StartDtmf(char str)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt8(str);
    int32_t error = Remote()->SendRequest(static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_START_DTMF),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StartDtmf! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::StopDtmf()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_STOP_DTMF),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StopDtmf! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::CombineConference()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(
        static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_COMBINE_CONFERENCE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function CombineConference failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::SeparateConference()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(
        static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_SEPARATE_CONFERENCE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SeparateConference call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

std::vector<CallAttributeInfo> BluetoothCallProxy::GetCurrentCallList(int32_t slotId)
{
    std::vector<CallAttributeInfo> callVec;
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return callVec;
    }
    dataParcel.WriteInt32(slotId);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return callVec;
    }
    int32_t error = Remote()->SendRequest(
        static_cast<int32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_GET_CURRENT_CALL_LIST), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetCurrentCallList call failed! errCode:%{public}d", error);
        return callVec;
    }
    int32_t vecCnt = replyParcel.ReadInt32();
    if (vecCnt <= 0 || vecCnt >= MAX_SIZE) {
        TELEPHONY_LOGE("vector size is error");
        return callVec;
    }
    for (int32_t i = 0; i < vecCnt; i++) {
        auto value = reinterpret_cast<const CallAttributeInfo *>(replyParcel.ReadRawData(sizeof(CallAttributeInfo)));
        if (value == nullptr) {
            TELEPHONY_LOGE("data error");
            return callVec;
        }
        callVec.push_back(*value);
    }
    return callVec;
}
} // namespace Telephony
} // namespace OHOS
