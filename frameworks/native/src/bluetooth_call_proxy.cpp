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

#include "bluetooth_call_proxy.h"

#include "message_option.h"
#include "message_parcel.h"
#include "native_call_manager_utils.h"

#include "call_manager_errors.h"

namespace OHOS {
namespace Telephony {
static const int32_t MAX_SIZE = 10000;
BluetoothCallProxy::BluetoothCallProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IBluetoothCall>(impl)
{}

int32_t BluetoothCallProxy::AnswerCall()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_ANSWER_CALL);
}

int32_t BluetoothCallProxy::RejectCall()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_REJECT_CALL);
}

int32_t BluetoothCallProxy::HangUpCall()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_DISCONNECT_CALL);
}

int32_t BluetoothCallProxy::GetCallState()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_GET_CALL_STATE);
}

int32_t BluetoothCallProxy::HoldCall()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_HOLD_CALL);
}

int32_t BluetoothCallProxy::UnHoldCall()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_UNHOLD_CALL);
}

int32_t BluetoothCallProxy::SwitchCall()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_SWAP_CALL);
}

int32_t BluetoothCallProxy::StartDtmf(char str)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt8(str);
    MessageParcel replyParcel;
    int32_t error = SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_START_DTMF, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StartDtmf! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::StopDtmf()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_STOP_DTMF);
}

int32_t BluetoothCallProxy::CombineConference()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_COMBINE_CONFERENCE);
}

int32_t BluetoothCallProxy::SeparateConference()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_SEPARATE_CONFERENCE);
}

int32_t BluetoothCallProxy::KickOutFromConference()
{
    return SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_KICK_OUT_CONFERENCE);
}

std::vector<CallAttributeInfo> BluetoothCallProxy::GetCurrentCallList(int32_t slotId)
{
    std::vector<CallAttributeInfo> callVec;
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return callVec;
    }
    dataParcel.WriteInt32(slotId);
    MessageParcel replyParcel;
    int32_t error =
        SendRequest(BluetoothCallInterfaceCode::INTERFACE_BT_GET_CURRENT_CALL_LIST, dataParcel, replyParcel);
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
        CallAttributeInfo callAttributeInfo = NativeCallManagerUtils::ReadCallAttributeInfo(replyParcel);
        TELEPHONY_LOGI("ready to push callAttributeInfo, callId: %{public}d", callAttributeInfo.callId);
        callVec.push_back(callAttributeInfo);
    }
    return callVec;
}

int32_t BluetoothCallProxy::SendRequest(BluetoothCallInterfaceCode code)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(BluetoothCallProxy::GetDescriptor())) {
        TELEPHONY_LOGE("BluetoothCallInterfaceCode:%{public}d write descriptor fail", static_cast<int32_t>(code));
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(code, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("BluetoothCallInterfaceCode:%{public}d errCode:%{public}d", static_cast<int32_t>(code), error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t BluetoothCallProxy::SendRequest(
    BluetoothCallInterfaceCode code, MessageParcel &dataParcel, MessageParcel &replyParcel)
{
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE(
            "BluetoothCallInterfaceCode:%{public}d function Remote() return nullptr!", static_cast<int32_t>(code));
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    return remote->SendRequest(static_cast<int32_t>(code), dataParcel, replyParcel, option);
}
} // namespace Telephony
} // namespace OHOS
