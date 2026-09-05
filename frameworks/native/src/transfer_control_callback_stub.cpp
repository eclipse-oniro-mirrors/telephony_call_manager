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

#include "transfer_control_callback_stub.h"

#include <securec.h>

#include "call_manager_errors.h"
#include "iservice_registry.h"
#include "native_call_manager_utils.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
TransferControlCallbackStub::TransferControlCallbackStub()
{
    memberFuncMap_[static_cast<uint32_t>(TransferControlInterfaceCode::IS_TRANSFER_CALL_ALLOW)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetTransferResultInner(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(TransferControlInterfaceCode::UPDATE_TRANSFER_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnUpdateTransferCallInner(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(TransferControlInterfaceCode::DISCONNECT_SCO)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnDisconnectScoInner(data, reply); };
}

TransferControlCallbackStub::~TransferControlCallbackStub()
{
}

int32_t TransferControlCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::u16string myDescriptor = TransferControlCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        TELEPHONY_LOGE("TransferControlCallbackStub::OnRemoteRequest descriptor checked failed");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }

    TELEPHONY_LOGI("TransferControlCallbackStub::OnRemoteRequest, code = %{public}u", code);
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return memberFunc(data, reply);
        }
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t TransferControlCallbackStub::OnGetTransferResultInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("TransferControlCallbackStub::OnGetTransferResultInner");
    TransferCallInfo transferCallInfo;
    transferCallInfo.devMac = data.ReadString();
    transferCallInfo.callId = data.ReadUint32();
    transferCallInfo.callDirect = static_cast<CallDirection>(data.ReadInt32());
    transferCallInfo.callState = static_cast<TelCallState>(data.ReadInt32());
    transferCallInfo.callType = static_cast<CallType>(data.ReadInt32());

    TransferResultInfo transferResultInfo;
    int32_t result = OnGetTransferResult(transferCallInfo, transferResultInfo);
    reply.WriteInt32(result);
    reply.WriteString(transferResultInfo.deviceShowName);
    reply.WriteUint32(transferResultInfo.secPolicy);
    return TELEPHONY_SUCCESS;
}

int32_t TransferControlCallbackStub::OnUpdateTransferCallInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("TransferControlCallbackStub::OnUpdateTransferCallInner");
    TransferCallInfo transferCallInfo;
    transferCallInfo.devMac = data.ReadString();
    transferCallInfo.callId = data.ReadUint32();
    transferCallInfo.callDirect = static_cast<CallDirection>(data.ReadInt32());
    transferCallInfo.callState = static_cast<TelCallState>(data.ReadInt32());
    transferCallInfo.callType = static_cast<CallType>(data.ReadInt32());

    int32_t result = OnUpdateTransferCall(transferCallInfo);
    reply.WriteInt32(result);
    return TELEPHONY_SUCCESS;
}

int32_t TransferControlCallbackStub::OnDisconnectScoInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("TransferControlCallbackStub::OnDisconnectScoInner");
    std::string devMac = data.ReadString();
    if (devMac.empty()) {
        TELEPHONY_LOGE("OnDisconnectScoInner devMac is empty");
        reply.WriteInt32(TELEPHONY_ERR_ARGUMENT_INVALID);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    int32_t result = OnDisconnectSco(devMac);
    reply.WriteInt32(result);
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS