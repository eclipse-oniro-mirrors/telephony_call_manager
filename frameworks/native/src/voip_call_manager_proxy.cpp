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
    AppExecFwk::PacMap &extras, std::vector<uint8_t> &userProfile, ErrorReason &reason)
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
    dataParcel.WriteBool(extras.GetBooleanValue("showBannerForIncomingCall"));
    dataParcel.WriteBool(extras.GetBooleanValue("isConferenceCall"));
    if (!dataParcel.WriteUInt8Vector(userProfile)) {
        TELEPHONY_LOGE("ReportIncomingCall userProfile write fail, size:%{public}u",
            static_cast<uint32_t>(userProfile.size()));
    }
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

int32_t VoipCallManagerProxy::ReportCallStateChange(std::string callId, const VoipCallState &state)
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

int32_t VoipCallManagerProxy::ReportOutgoingCall(
    AppExecFwk::PacMap &extras, std::vector<uint8_t> &userProfile, ErrorReason &reason)
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
    dataParcel.WriteBool(extras.GetBooleanValue("showBannerForIncomingCall"));
    dataParcel.WriteBool(extras.GetBooleanValue("isConferenceCall"));
    if (!dataParcel.WriteUInt8Vector(userProfile)) {
        TELEPHONY_LOGE("ReportOutgoingCall userProfile write fail, size:%{public}u",
            static_cast<uint32_t>(userProfile.size()));
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("ReportOutgoingCall Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error =
        remote->SendRequest(static_cast<int32_t>(INTERFACE_REPORT_OUTGOING_CALL), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportOutgoingCall call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    reason = static_cast<ErrorReason>(replyParcel.ReadInt32());
    return result;
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

int32_t VoipCallManagerProxy::ReportVoipIncomingCall(
    std::string callId, std::string bundleName, std::string processMode, int32_t uid)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(callId);
    dataParcel.WriteString(bundleName);
    dataParcel.WriteString(processMode);
    dataParcel.WriteInt32(uid);
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
    std::string callId, std::string bundleName, std::string extensionId, int32_t uid)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(callId);
    dataParcel.WriteString(bundleName);
    dataParcel.WriteString(extensionId);
    dataParcel.WriteInt32(uid);
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

int32_t VoipCallManagerProxy::Answer(const VoipCallEventInfo &events, int32_t videoState)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(events.voipCallId);
    dataParcel.WriteString(events.bundleName);
    dataParcel.WriteInt32(events.uid);
    dataParcel.WriteInt32(videoState);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("Answer voip Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(INTERFACE_ANSWER_VOIP_CALL), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function Answer voip call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::HangUp(const VoipCallEventInfo &events)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(events.voipCallId);
    dataParcel.WriteString(events.bundleName);
    dataParcel.WriteInt32(events.uid);
    dataParcel.WriteInt32(static_cast<int32_t>(events.errorReason));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("HangUp voip Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(INTERFACE_HANGUP_VOIP_CALL), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function HangUp voip call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::Reject(const VoipCallEventInfo &events)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(events.voipCallId);
    dataParcel.WriteString(events.bundleName);
    dataParcel.WriteInt32(events.uid);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("Reject voip  Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(INTERFACE_REJECT_VOIP_CALL), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function Reject voip call failed! errCode:%{public}d", error);
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

int32_t VoipCallManagerProxy::UnloadVoipSa()
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UnloadVoipSa Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageOption option;
    MessageParcel replyParcel;
    int32_t error =
        remote->SendRequest(static_cast<int32_t>(INTERFACE_UNLOAD_VOIP_SA), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function UnloadVoipSa failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::SendCallUiEvent(std::string voipCallId, const CallAudioEvent &callAudioEvent)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(voipCallId);
    dataParcel.WriteInt32(static_cast<int32_t>(callAudioEvent));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendCallUiEvent Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error =
        remote->SendRequest(static_cast<int32_t>(INTERFACE_SEND_CALL_UI_EVENT), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function SendCallUiEvent call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t VoipCallManagerProxy::ReportCallAudioEventChange(std::string voipCallId, const CallAudioEvent &callAudioEvent)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(VoipCallManagerProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(voipCallId);
    dataParcel.WriteInt32(static_cast<int32_t>(callAudioEvent));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("ReportCallAudioEventChange Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    MessageParcel replyParcel;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(INTERFACE_REPORT_CALL_AUDIO_EVENT_CHANGE), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportCallAudioEventChange call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

} // namespace Telephony
} // namespace OHOS
