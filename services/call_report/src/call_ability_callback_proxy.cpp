/*
 * Copyright (C) 2021-2026 Huawei Device Co., Ltd.
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

#include "call_ability_callback_proxy.h"

#include "call_manager_errors.h"
#include "message_option.h"
#include "message_parcel.h"
#include "call_manager_utils.h"

namespace OHOS {
namespace Telephony {
CallAbilityCallbackProxy::CallAbilityCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ICallAbilityCallback>(impl)
{}

__attribute__((noinline)) int32_t CallAbilityCallbackProxy::SendRequest(int32_t msgId, MessageParcel &dataParcel,
    MessageParcel &replyParcel, MessageOption &option)
{
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("CallAbilityCallbackProxy Remote return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return remote->SendRequest(msgId, dataParcel, replyParcel, option);
}

int32_t CallAbilityCallbackProxy::OnCallDetailsChange(const CallAttributeInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallManagerUtils::WriteCallAttributeInfo(info, dataParcel);
    int32_t error = SendRequest(static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_STATE_INFO),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnMeeTimeDetailsChange(const CallAttributeInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallManagerUtils::WriteCallAttributeInfo(info, dataParcel);
    int32_t error = SendRequest(static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_MEETIME_STATE_INFO),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnCallEventChange(const CallEventInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallEventInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    int32_t error = SendRequest(static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_EVENT),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnCallDisconnectedCause(const DisconnectedDetails &details)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(static_cast<int32_t>(details.reason))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!dataParcel.WriteString(details.message)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    int32_t error = SendRequest(
        static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_DISCONNECTED_CAUSE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    PackDataParcel(reportId, resultInfo, dataParcel);
    int32_t error = SendRequest(
        static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_ASYNC_RESULT_REQUEST), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportMmiCodeResult(const MmiCodeInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(MmiCodeInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    int32_t error = SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_MMI_CODE_RESULT_REQUEST), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

void CallAbilityCallbackProxy::PackDataParcel(
    CallResultReportId reportId, AppExecFwk::PacMap &resultInfo, MessageParcel &dataParcel)
{
    dataParcel.WriteInt32(static_cast<int32_t>(reportId));
    dataParcel.WriteInt32(resultInfo.GetIntValue("result"));
    switch (reportId) {
        case CallResultReportId::GET_CALL_WAITING_REPORT_ID:
        case CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("status"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("classCw"));
            break;
        case CallResultReportId::GET_CALL_TRANSFER_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("status"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("classx"));
            dataParcel.WriteString(resultInfo.GetStringValue("number"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("type"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("reason"));
            break;
        case CallResultReportId::GET_CALL_CLIP_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("action"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("clipStat"));
            break;
        case CallResultReportId::GET_CALL_CLIR_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("action"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("clirStat"));
            break;
        case CallResultReportId::GET_IMS_CONFIG_REPORT_ID:
        case CallResultReportId::GET_IMS_FEATURE_VALUE_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("value"));
            break;
        default:
            break;
    }
}

int32_t CallAbilityCallbackProxy::OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(static_cast<int32_t>(requestId));
    dataParcel.WriteString(info.GetStringValue("phoneNumber"));
    dataParcel.WriteString(info.GetStringValue("bundleName"));
    dataParcel.WriteInt32(info.GetIntValue("videoState"));
    switch (requestId) {
        case OttCallRequestId::OTT_REQUEST_INVITE_TO_CONFERENCE:
            if (info.GetIntValue("listCnt") > 0) {
                dataParcel.WriteString(info.GetStringValue("number"));
            }
            break;
        case OttCallRequestId::OTT_REQUEST_UPDATE_CALL_MEDIA_MODE:
            dataParcel.WriteInt32(info.GetIntValue("callMediaMode"));
            break;
        default:
            break;
    }
    int32_t error = SendRequest(static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::REPORT_OTT_CALL_REQUEST),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportAudioDeviceChange(const AudioDeviceInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    size_t audioDeviceListLength = info.audioDeviceList.size();
    dataParcel.WriteInt32(static_cast<int32_t>(audioDeviceListLength));
    for (auto &audioDevice : info.audioDeviceList) {
        dataParcel.WriteRawData((const void *)&audioDevice, sizeof(AudioDevice));
    }
    dataParcel.WriteRawData((const void *)&info.currentAudioDevice, sizeof(AudioDevice));
    dataParcel.WriteBool(info.isMuted);
    dataParcel.WriteInt32(info.callId);

    int32_t error = SendRequest(
        static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_AUDIO_DEVICE_CHANGE_RESULT_REQUEST),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportPostDialDelay(const std::string &str)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(str);
    int32_t error = SendRequest(
        static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::REPORT_POST_DIAL_DELAY),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallMediaModeInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&imsCallModeInfo, length);
    int32_t error = SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_IMS_CALL_MODE_RECEIVE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportCallSessionEventChange(
    const CallSessionEvent &callSessionEventOptions)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallSessionEvent);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&callSessionEventOptions, length);
    int32_t error = SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::CALL_SESSION_EVENT_CHANGE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(PeerDimensionsDetail);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&peerDimensionsDetail, length);
    int32_t error = SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::PEERD_DIMENSIONS_CHANGE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportCallDataUsageChange(const int64_t dataUsage)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt64(dataUsage);
    int32_t error = SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::CALL_DATA_USAGE_CHANGE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportCameraCapabilities(const CameraCapabilities &cameraCapabilities)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CameraCapabilities);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&cameraCapabilities, length);
    int32_t error = SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::CAMERA_CAPABILITIES_CHANGE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnPhoneStateChange(int32_t numActive, int32_t numHeld, int32_t callState,
    const std::string &number)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(numActive);
    dataParcel.WriteInt32(numHeld);
    dataParcel.WriteInt32(callState);
    dataParcel.WriteString(number);
    int32_t error = SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_PHONE_STATE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

#ifdef SUPPORT_RTT_CALL
int32_t CallAbilityCallbackProxy::OnReportRttCallEvtChanged(const RttEvent &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(info.callId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!dataParcel.WriteInt32(info.eventType)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!dataParcel.WriteInt32(info.reason)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    int32_t error = SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::RTT_CALL_EVENT_CHANGE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportRttCallError(const RttError &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(info.callId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!dataParcel.WriteInt32(info.causeCode)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!dataParcel.WriteInt32(info.operationType)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!dataParcel.WriteString(info.reasonText)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    int32_t error = SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::RTT_CALL_ERROR_REPORT), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportRttCallMessage(AppExecFwk::PacMap &msgResult)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    dataParcel.WriteInt32(msgResult.GetIntValue("callId"));
    dataParcel.WriteString(msgResult.GetStringValue("rttMessage"));
    int32_t error = SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::RTT_CALL_SEND_MESSAGE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}
#endif
} // namespace Telephony
} // namespace OHOS
