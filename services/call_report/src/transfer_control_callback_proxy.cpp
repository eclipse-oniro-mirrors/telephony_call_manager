/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "transfer_control_callback_proxy.h"

#include "call_manager_errors.h"
#include "message_option.h"
#include "message_parcel.h"
#include "transfer_control_callback_ipc_interface_code.h"

namespace OHOS {
namespace Telephony {
TransferControlCallbackProxy::TransferControlCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ITransferControlCallback>(impl)
{}

__attribute__((noinline)) int32_t TransferControlCallbackProxy::SendRequest(int32_t msgId, MessageParcel &dataParcel,
    MessageParcel &replyParcel, MessageOption &option)
{
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("TransferControlCallbackProxy::SendRequest remote is nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }

    return remote->SendRequest(msgId, dataParcel, replyParcel, option);
}

int32_t TransferControlCallbackProxy::OnGetTransferResult(const TransferCallInfo &info, TransferResultInfo &result)
{
    TELEPHONY_LOGI("TransferControlCallbackProxy::OnGetTransferResult");
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(TransferControlCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("TransferControlCallbackProxy::OnGetTransferResult WriteInterfaceToken fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    dataParcel.WriteString(info.devMac);
    dataParcel.WriteUint32(info.callId);
    dataParcel.WriteInt32(static_cast<int32_t>(info.callDirect));
    dataParcel.WriteInt32(static_cast<int32_t>(info.callState));
    dataParcel.WriteInt32(static_cast<int32_t>(info.callType));
    int32_t error = SendRequest(static_cast<uint32_t>(TransferControlInterfaceCode::IS_TRANSFER_CALL_ALLOW),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("TransferControlCallbackProxy OnGetTransferResult error=%{public}d", error);
        return error;
    }

    int32_t replyResult = replyParcel.ReadInt32();
    result.deviceShowName = replyParcel.ReadString();
    result.secPolicy = replyParcel.ReadUint32();
    return replyResult;
}

int32_t TransferControlCallbackProxy::OnUpdateTransferCall(const TransferCallInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(TransferControlCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("TransferControlCallbackProxy::OnUpdateTransferCall WriteInterfaceToken fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    dataParcel.WriteString(info.devMac);
    dataParcel.WriteUint32(info.callId);
    dataParcel.WriteInt32(static_cast<int32_t>(info.callDirect));
    dataParcel.WriteInt32(static_cast<int32_t>(info.callState));
    dataParcel.WriteInt32(static_cast<int32_t>(info.callType));
    int32_t error = SendRequest(static_cast<uint32_t>(TransferControlInterfaceCode::UPDATE_TRANSFER_CALL),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("TransferControlCallbackProxy OnUpdateTransferCall error=%{public}d", error);
        return error;
    }

    int32_t replyResult = replyParcel.ReadInt32();
    TELEPHONY_LOGI("TransferControlCallbackProxy OnUpdateTransferCall replyResult = %{public}d", replyResult);
    return replyResult;
}

int32_t TransferControlCallbackProxy::OnDisconnectSco(const std::string &devMac)
{
    TELEPHONY_LOGI("TransferControlCallbackProxy::OnDisconnectSco");
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(TransferControlCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("TransferControlCallbackProxy::OnDisconnectSco WriteInterfaceToken fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!dataParcel.WriteString(devMac)) {
        TELEPHONY_LOGE("TransferControlCallbackProxy::OnDisconnectSco WriteString fail");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    int32_t error = SendRequest(static_cast<uint32_t>(TransferControlInterfaceCode::DISCONNECT_SCO),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("TransferControlCallbackProxy OnDisconnectSco error=%{public}d", error);
        return error;
    }

    int32_t replyResult = replyParcel.ReadInt32();
    TELEPHONY_LOGI("TransferControlCallbackProxy OnDisconnectSco replyResult = %{public}d", replyResult);
    return replyResult;
}

} // namespace Telephony
} // namespace OHOS
