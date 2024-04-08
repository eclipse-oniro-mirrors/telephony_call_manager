/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Telephony {
CallAbilityCallbackProxy::CallAbilityCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ICallAbilityCallback>(impl)
{}

int32_t CallAbilityCallbackProxy::OnCallDetailsChange(const CallAttributeInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteCString(info.accountNumber);
    dataParcel.WriteCString(info.bundleName);
    dataParcel.WriteBool(info.speakerphoneOn);
    dataParcel.WriteInt32(info.accountId);
    dataParcel.WriteInt32(static_cast<int32_t>(info.videoState));
    dataParcel.WriteInt64(info.startTime);
    dataParcel.WriteBool(info.isEcc);
    dataParcel.WriteInt32(static_cast<int32_t>(info.callType));
    dataParcel.WriteInt32(info.callId);
    dataParcel.WriteInt32(static_cast<int32_t>(info.callState));
    dataParcel.WriteInt32(static_cast<int32_t>(info.conferenceState));
    dataParcel.WriteInt64(info.callBeginTime);
    dataParcel.WriteInt64(info.callEndTime);
    dataParcel.WriteInt64(info.ringBeginTime);
    dataParcel.WriteInt64(info.ringEndTime);
    dataParcel.WriteInt32(static_cast<int32_t>(info.callDirection));
    dataParcel.WriteInt32(static_cast<int32_t>(info.answerType));
    dataParcel.WriteInt32(info.index);
    dataParcel.WriteInt32(info.crsType);
    dataParcel.WriteInt32(info.originalCallType);
    dataParcel.WriteCString(info.numberLocation);
    if (info.callType == CallType::TYPE_VOIP) {
        dataParcel.WriteString(info.voipCallInfo.voipCallId);
        dataParcel.WriteString(info.voipCallInfo.userName);
        dataParcel.WriteString(info.voipCallInfo.abilityName);
        dataParcel.WriteString(info.voipCallInfo.extensionId);
        dataParcel.WriteString(info.voipCallInfo.voipBundleName);
        dataParcel.WriteUInt8Vector(info.voipCallInfo.userProfile);
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error =
        Remote()->SendRequest(static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_STATE_INFO),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGD("update call state info errcode: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

void CallAbilityCallbackProxy::SetVoipCallInfo(const CallAttributeInfo &info, MessageParcel &dataParcel)
{
    if (info.callType == CallType::TYPE_VOIP) {
        dataParcel.WriteString(info.voipCallInfo.voipCallId);
        dataParcel.WriteString(info.voipCallInfo.userName);
        dataParcel.WriteString(info.voipCallInfo.abilityName);
        dataParcel.WriteString(info.voipCallInfo.extensionId);
        dataParcel.WriteString(info.voipCallInfo.voipBundleName);
        dataParcel.WriteBool(info.voipCallInfo.showBannerForIncomingCall);
        dataParcel.WriteUInt8Vector(info.voipCallInfo.userProfile);
    }
}

int32_t CallAbilityCallbackProxy::OnCallEventChange(const CallEventInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallEventInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_EVENT),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("update call event failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnCallDisconnectedCause(const DisconnectedDetails &details)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
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
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_DISCONNECTED_CAUSE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGD("report call disconnected cause errcode: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    PackDataParcel(reportId, resultInfo, dataParcel);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_ASYNC_RESULT_REQUEST), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report async results failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportMmiCodeResult(const MmiCodeInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(MmiCodeInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_MMI_CODE_RESULT_REQUEST), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report async results failed, error: %{public}d", error);
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
        case CallResultReportId::START_RTT_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("active"));
            break;
        case CallResultReportId::STOP_RTT_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("inactive"));
            break;
        default:
            break;
    }
}

int32_t CallAbilityCallbackProxy::OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
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
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error =
        Remote()->SendRequest(static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::REPORT_OTT_CALL_REQUEST),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report ott call request failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportAudioDeviceChange(const AudioDeviceInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    size_t audioDeviceListLength = info.audioDeviceList.size();
    dataParcel.WriteInt32(static_cast<int32_t>(audioDeviceListLength));
    for (auto &audioDevice : info.audioDeviceList) {
        dataParcel.WriteRawData((const void *)&audioDevice, sizeof(AudioDevice));
    }
    dataParcel.WriteRawData((const void *)&info.currentAudioDevice, sizeof(AudioDevice));
    dataParcel.WriteBool(info.isMuted);

    TELEPHONY_LOGD("audioDeviceListLength=%{public}zu", audioDeviceListLength);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_AUDIO_DEVICE_CHANGE_RESULT_REQUEST),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGD("report audio device info errcode: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportPostDialDelay(const std::string &str)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("WriteInterfaceToken fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(str);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::REPORT_POST_DIAL_DELAY),
        dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report post-dial wait failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallMediaModeInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&imsCallModeInfo, length);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_IMS_CALL_MODE_RECEIVE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGD("report async results errcode: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportCallSessionEventChange(
    const CallSessionEvent &callSessionEventOptions)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallSessionEvent);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&callSessionEventOptions, length);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::CALL_SESSION_EVENT_CHANGE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGD("report async results errcode: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(PeerDimensionsDetail);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&peerDimensionsDetail, length);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::PEERD_DIMENSIONS_CHANGE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report async results failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportCallDataUsageChange(const int64_t dataUsage)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt64(dataUsage);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::CALL_DATA_USAGE_CHANGE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report async results failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportCameraCapabilities(const CameraCapabilities &cameraCapabilities)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CameraCapabilities);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&cameraCapabilities, length);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::CAMERA_CAPABILITIES_CHANGE), dataParcel,
        replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report async results failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}
} // namespace Telephony
} // namespace OHOS
