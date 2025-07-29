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

#include "call_status_callback_proxy.h"

#include "call_manager_errors.h"
#include "message_option.h"
#include "message_parcel.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallStatusCallbackProxy::CallStatusCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ICallStatusCallback>(impl)
{}

int32_t CallStatusCallbackProxy::UpdateCallReportInfo(const CallReportInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(info.index);
    dataParcel.WriteCString(info.accountNum);
    dataParcel.WriteInt32(info.accountId);
    dataParcel.WriteInt32(static_cast<int32_t>(info.callType));
    dataParcel.WriteInt32(static_cast<int32_t>(info.callMode));
    dataParcel.WriteInt32(static_cast<int32_t>(info.state));
    dataParcel.WriteInt32(info.voiceDomain);
    dataParcel.WriteInt32(info.mpty);
    dataParcel.WriteInt32(info.crsType);
    dataParcel.WriteInt32(info.originalCallType);
    if (info.callType == CallType::TYPE_VOIP) {
        dataParcel.WriteString(info.voipCallInfo.voipCallId);
        dataParcel.WriteString(info.voipCallInfo.userName);
        dataParcel.WriteString(info.voipCallInfo.abilityName);
        dataParcel.WriteString(info.voipCallInfo.extensionId);
        dataParcel.WriteString(info.voipCallInfo.voipBundleName);
        dataParcel.WriteBool(info.voipCallInfo.showBannerForIncomingCall);
        dataParcel.WriteBool(info.voipCallInfo.isConferenceCall);
        dataParcel.WriteBool(info.voipCallInfo.isVoiceAnswerSupported);
        dataParcel.WriteBool(info.voipCallInfo.hasMicPermission);
        dataParcel.WriteBool(info.voipCallInfo.isCapsuleSticky);
        dataParcel.WriteInt32(info.voipCallInfo.uid);
        dataParcel.WriteUInt8Vector(info.voipCallInfo.userProfile);
    }
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_CALL_INFO), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateCallsReportInfo(const CallsReportInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(info.callVec.size());
    for (auto &it : info.callVec) {
        dataParcel.WriteInt32(it.index);
        dataParcel.WriteCString(it.accountNum);
        dataParcel.WriteInt32(it.accountId);
        dataParcel.WriteInt32(static_cast<int32_t>(it.callType));
        dataParcel.WriteInt32(static_cast<int32_t>(it.callMode));
        dataParcel.WriteInt32(static_cast<int32_t>(it.state));
        dataParcel.WriteInt32(it.voiceDomain);
        dataParcel.WriteInt32(it.mpty);
        dataParcel.WriteInt32(it.crsType);
        dataParcel.WriteInt32(it.originalCallType);
        if (it.callType == CallType::TYPE_VOIP) {
            dataParcel.WriteString(it.voipCallInfo.voipCallId);
            dataParcel.WriteString(it.voipCallInfo.userName);
            dataParcel.WriteString(it.voipCallInfo.abilityName);
            dataParcel.WriteString(it.voipCallInfo.extensionId);
            dataParcel.WriteString(it.voipCallInfo.voipBundleName);
            dataParcel.WriteBool(it.voipCallInfo.showBannerForIncomingCall);
            dataParcel.WriteBool(it.voipCallInfo.isConferenceCall);
            dataParcel.WriteBool(it.voipCallInfo.isVoiceAnswerSupported);
            dataParcel.WriteBool(it.voipCallInfo.hasMicPermission);
            dataParcel.WriteBool(it.voipCallInfo.isCapsuleSticky);
            dataParcel.WriteInt32(it.voipCallInfo.uid);
            dataParcel.WriteUInt8Vector(it.voipCallInfo.userProfile);
        }
        dataParcel.WriteString(it.name);
        dataParcel.WriteInt32(it.namePresentation);
        dataParcel.WriteInt32(it.newCallUseBox);
        dataParcel.WriteInt32(static_cast<int32_t>(it.reason));
        dataParcel.WriteString(it.message);
    }
    dataParcel.WriteInt32(info.slotId);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_CALLS_INFO), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateDisconnectedCause(const DisconnectedDetails &details)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(static_cast<int32_t>(details.reason))) {
        TELEPHONY_LOGE("write reason fail");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!dataParcel.WriteString(details.message)) {
        TELEPHONY_LOGE("write message fail");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_DISCONNECTED_CAUSE), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateEventResultInfo(const CellularCallEventInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CellularCallEventInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_EVENT_RESULT_INFO), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateRBTPlayInfo(const RBTPlayInfo info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32((int32_t)info);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_RBT_PLAY_INFO), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateGetWaitingResult(const CallWaitResponse &callWaitResponse)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallWaitResponse);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&callWaitResponse, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_GET_WAITING), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateSetWaitingResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_SET_WAITING), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateGetRestrictionResult(const CallRestrictionResponse &callRestrictionResult)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallRestrictionResponse);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&callRestrictionResult, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_GET_RESTRICTION), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateSetRestrictionResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_SET_RESTRICTION), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateSetRestrictionPasswordResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_SET_RESTRICTION_PWD), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateGetTransferResult(const CallTransferResponse &callTransferResponse)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallTransferResponse);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&callTransferResponse, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_GET_TRANSFER), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateSetTransferResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_SET_TRANSFER), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateGetCallClipResult(const ClipResponse &clipResponse)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(ClipResponse);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&clipResponse, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_GET_CALL_CLIP), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateGetCallClirResult(const ClirResponse &clirResponse)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(ClirResponse);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&clirResponse, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_GET_CALL_CLIR), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateSetCallClirResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_SET_CALL_CLIR), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::StartRttResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_STARTRTT_STATUS), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::StopRttResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(UPDATE_STOPRTT_STATUS), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::GetImsConfigResult(const GetImsConfigResponse &response)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(GetImsConfigResponse);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&response, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(GET_IMS_CONFIG), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::SetImsConfigResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(SET_IMS_CONFIG), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::GetImsFeatureValueResult(const GetImsFeatureValueResponse &response)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(GetImsFeatureValueResponse);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&response, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(GET_IMS_FEATURE_VALUE), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::SetImsFeatureValueResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(SET_IMS_FEATURE_VALUE), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::ReceiveUpdateCallMediaModeRequest(const CallModeReportInfo &response)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallModeReportInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&response, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(RECEIVE_UPDATE_MEDIA_MODE_REQUEST), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::ReceiveUpdateCallMediaModeResponse(const CallModeReportInfo &response)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallModeReportInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&response, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(RECEIVE_UPDATE_MEDIA_MODE_RESPONSE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::InviteToConferenceResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(INVITE_TO_CONFERENCE), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::StartDtmfResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(START_DTMF), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::StopDtmfResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(STOP_DTMF), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::SendUssdResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(SEND_USSD), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::SendMmiCodeResult(const MmiCodeInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(MmiCodeInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(MMI_CODE_INFO_RESPONSE), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::GetImsCallDataResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(GET_IMS_CALL_DATA), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::CloseUnFinishedUssdResult(const int32_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = CALL_ERR_ILLEGAL_CALL_OPERATION;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(static_cast<int32_t>(CLOSE_UNFINISHED_USSD), dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::ReportPostDialChar(const std::string &c)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(c);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(POST_DIAL_CHAR, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::ReportPostDialDelay(const std::string &str)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(str);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(POST_DIAL_DELAY, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::UpdateVoipEventInfo(const VoipCallEventInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(info.voipCallId);
    dataParcel.WriteString(info.bundleName);
    dataParcel.WriteInt32(info.uid);
    dataParcel.WriteInt32(static_cast<int32_t>(info.voipCallEvent));
    dataParcel.WriteInt32(static_cast<int32_t>(info.errorReason));
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    error = Remote()->SendRequest(UPDATE_VOIP_EVENT_INFO, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::HandleCallSessionEventChanged(const CallSessionReportInfo &eventOptions)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallSessionReportInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&eventOptions, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(CALL_SESSION_EVENT, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::HandlePeerDimensionsChanged(const PeerDimensionsReportInfo &dimensionsDetail)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(PeerDimensionsReportInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&dimensionsDetail, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(PEER_DIMENSION_CHANGE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::HandleCallDataUsageChanged(const int64_t result)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt64(result);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(CALL_DATA_USAGE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallStatusCallbackProxy::HandleCameraCapabilitiesChanged(const CameraCapabilitiesReportInfo &cameraCapabilities)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CameraCapabilitiesReportInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&cameraCapabilities, length);
    if (Remote() == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(CAMERA_CAPBILITIES_CHANGE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}
} // namespace Telephony
} // namespace OHOS
