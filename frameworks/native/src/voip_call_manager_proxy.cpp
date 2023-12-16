/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "voip_call_manager_proxy.h"

#include "message_option.h"
#include "message_parcel.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
VoipCallManagerProxy::VoipCallManagerProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IVoipCallManagerService>(impl)
{}

int32_t VoipCallManagerProxy::ReportIncomingCall(
    AppExecFwk::PacMap &extras, std::shared_ptr<OHOS::Media::PixelMap> &userProfile, ErrorReason &reason)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(extras.GetStringValue("callId"));
    dataParcel.WriteInt32(extras.GetIntValue("voipCallType"));
    dataParcel.WriteString(extras.GetStringValue("userName"));
    dataParcel.WriteString(extras.GetStringValue("abilityName"));
    dataParcel.WriteInt32(extras.GetIntValue("voipCallState"));
    dataParcel.WriteParcelable(userProfile.get());
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("ReportIncomingCall Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error =
        remote->SendRequest(static_cast<int32_t>(INTERFACE_REPORT_INCOMING_CALL), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportIncomingCall call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    reason = static_cast<ErrorReason>(replyParcel.ReadInt32());
    return result;
}

int32_t VoipCallManagerProxy::ReportIncomingCallError(AppExecFwk::PacMap &extras)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(extras.GetStringValue("callId"));
    dataParcel.WriteInt32(extras.GetIntValue("reportVoipCallFailedCause"));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("ReportIncomingCallError Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(INTERFACE_REPORT_INCOMING_CALL_ERROR), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportIncomingCallError call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::ReportCallStateChange(std::string &callId, const VoipCallState &state)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(callId);
    dataParcel.WriteInt32(static_cast<int32_t>(state));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("ReportCallStateChange Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error =
        remote->SendRequest(static_cast<int32_t>(INTERFACE_REPORT_CALL_STATE_CHANGE), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportCallStateChange call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::RegisterCallBack(const sptr<IVoipCallManagerCallback> &callback)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("RegisterCallBack Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error =
        remote->SendRequest(static_cast<int32_t>(INTERFACE_REGISTER_CALLBACK), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function RegisterCallBack call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::UnRegisterCallBack()
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UnRegisterCallBack Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error =
        remote->SendRequest(static_cast<int32_t>(INTERFACE_UN_REGISTER_CALLBACK), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function UnRegisterCallBack call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::ReportVoipIncomingCall(std::string &callId, std::string &bundleName)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(callId);
    dataParcel.WriteString(bundleName);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("ReportVoipIncomingCall Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error =
        remote->SendRequest(static_cast<int32_t>(INTERFACE_REPORT_VOIP_INCOMING_CALL), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportVoipIncomingCall call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::ReportVoipCallExtensionId(
    std::string &callId, std::string &bundleName, std::string &extensionId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(callId);
    dataParcel.WriteString(bundleName);
    dataParcel.WriteString(extensionId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("ReportVoipCallExtensionId Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(INTERFACE_REPORT_VOIP_CALL_EXTENSIONID), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportVoipCallExtensionId call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::ReportVoipCallEventChange(const VoipCallEvents &events)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(events.voipCallId);
    dataParcel.WriteString(events.bundleName);
    dataParcel.WriteInt32(static_cast<int32_t>(events.voipCallEvent));
    dataParcel.WriteInt32(static_cast<int32_t>(events.errorReason));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("ReportVoipCallEventChange Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(INTERFACE_REPORT_VOIP_CALL_EVENT_CHANGE), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportVoipCallEventChange call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback)
{
    if (callback == nullptr) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("RegisterCallManagerCallBack Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(INTERFACE_REGISTER_CALL_MANAGER_CALLBACK), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function RegisterCallManagerCallBack call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::UnRegisterCallManagerCallBack()
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UnRegisterCallManagerCallBack Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(INTERFACE_UNREGISTER_CALL_MANAGER_CALLBACK), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function UnRegisterCallManagerCallBack call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

} // namespace Telephony
} // namespace OHOS
