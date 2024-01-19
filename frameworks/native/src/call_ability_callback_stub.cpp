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

#include "call_ability_callback_stub.h"

#include <securec.h>

#include "call_manager_errors.h"
#include "pixel_map.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
const int32_t MAX_LEN = 100000;
CallAbilityCallbackStub::CallAbilityCallbackStub()
{
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_STATE_INFO)] =
        &CallAbilityCallbackStub::OnUpdateCallStateInfo;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_EVENT)] =
        &CallAbilityCallbackStub::OnUpdateCallEvent;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_DISCONNECTED_CAUSE)] =
        &CallAbilityCallbackStub::OnUpdateCallDisconnectedCause;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_ASYNC_RESULT_REQUEST)] =
        &CallAbilityCallbackStub::OnUpdateAysncResults;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::REPORT_OTT_CALL_REQUEST)] =
        &CallAbilityCallbackStub::OnUpdateOttCallRequest;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_MMI_CODE_RESULT_REQUEST)] =
        &CallAbilityCallbackStub::OnUpdateMmiCodeResults;
    memberFuncMap_[static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_AUDIO_DEVICE_CHANGE_RESULT_REQUEST)] =
        &CallAbilityCallbackStub::OnUpdateAudioDeviceChange;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::REPORT_POST_DIAL_DELAY)] =
        &CallAbilityCallbackStub::OnUpdatePostDialDelay;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_IMS_CALL_MODE_RECEIVE)] =
        &CallAbilityCallbackStub::OnUpdateImsCallModeChange;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::CALL_SESSION_EVENT_CHANGE)] =
        &CallAbilityCallbackStub::OnUpdateCallSessionEventChange;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::PEERD_DIMENSIONS_CHANGE)] =
        &CallAbilityCallbackStub::OnUpdatePeerDimensionsChange;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::CALL_DATA_USAGE_CHANGE)] =
        &CallAbilityCallbackStub::OnUpdateCallDataUsageChange;
    memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::CAMERA_CAPABILITIES_CHANGE)] =
        &CallAbilityCallbackStub::OnUpdateCameraCapabilities;
}

CallAbilityCallbackStub::~CallAbilityCallbackStub()
{
    memberFuncMap_.clear();
}

int32_t CallAbilityCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescriptor = CallAbilityCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        TELEPHONY_LOGE("descriptor checked failed");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }
    TELEPHONY_LOGI("OnReceived, cmd = %{public}u", code);
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t CallAbilityCallbackStub::OnUpdateCallStateInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    CallAttributeInfo parcelPtr;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    strncpy_s(parcelPtr.accountNumber, kMaxNumberLen + 1, data.ReadCString(), kMaxNumberLen + 1);
    strncpy_s(parcelPtr.bundleName, kMaxNumberLen + 1, data.ReadCString(), kMaxNumberLen + 1);
    parcelPtr.speakerphoneOn = data.ReadBool();
    parcelPtr.accountId = data.ReadInt32();
    parcelPtr.videoState = static_cast<VideoStateType>(data.ReadInt32());
    parcelPtr.startTime = data.ReadInt64();
    parcelPtr.isEcc = data.ReadBool();
    parcelPtr.callType = static_cast<CallType>(data.ReadInt32());
    parcelPtr.callId = data.ReadInt32();
    parcelPtr.callState = static_cast<TelCallState>(data.ReadInt32());
    parcelPtr.conferenceState = static_cast<TelConferenceState>(data.ReadInt32());
    parcelPtr.callBeginTime = data.ReadInt64();
    parcelPtr.callEndTime = data.ReadInt64();
    parcelPtr.ringBeginTime = data.ReadInt64();
    parcelPtr.ringEndTime = data.ReadInt64();
    parcelPtr.callDirection = static_cast<CallDirection>(data.ReadInt32());
    parcelPtr.answerType = static_cast<CallAnswerType>(data.ReadInt32());
    parcelPtr.index = data.ReadInt32();
    parcelPtr.crsType = data.ReadInt32();
    parcelPtr.originalCallType = data.ReadInt32();
    parcelPtr.voipCallInfo.voipCallId = data.ReadString();
    parcelPtr.voipCallInfo.userName = data.ReadString();
    parcelPtr.voipCallInfo.abilityName = data.ReadString();
    parcelPtr.voipCallInfo.extensionId = data.ReadString();
    parcelPtr.voipCallInfo.voipBundleName = data.ReadString();
    parcelPtr.voipCallInfo.pixelMap = std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
    result = OnCallDetailsChange(parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateCallEvent(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    const CallEventInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const CallEventInfo *>(data.ReadRawData(sizeof(CallEventInfo)))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    CallEventInfo info = *parcelPtr;
    result = OnCallEventChange(info);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateCallDisconnectedCause(MessageParcel &data, MessageParcel &reply)
{
    DisconnectedDetails dcDetails;
    dcDetails.reason = static_cast<DisconnectedReason>(data.ReadInt32());
    dcDetails.message = data.ReadString();
    int32_t result = OnCallDisconnectedCause(dcDetails);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateAysncResults(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    AppExecFwk::PacMap resultInfo;
    CallResultReportId reportId = static_cast<CallResultReportId>(data.ReadInt32());
    resultInfo.PutIntValue("result", data.ReadInt32());
    switch (reportId) {
        case CallResultReportId::GET_CALL_WAITING_REPORT_ID:
        case CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID:
            resultInfo.PutIntValue("status", data.ReadInt32());
            resultInfo.PutIntValue("classCw", data.ReadInt32());
            break;
        case CallResultReportId::GET_CALL_TRANSFER_REPORT_ID:
            resultInfo.PutIntValue("status", data.ReadInt32());
            resultInfo.PutIntValue("classx", data.ReadInt32());
            resultInfo.PutStringValue("number", data.ReadString());
            resultInfo.PutIntValue("type", data.ReadInt32());
            resultInfo.PutIntValue("reason", data.ReadInt32());
            resultInfo.PutIntValue("time", data.ReadInt32());
            break;
        case CallResultReportId::GET_CALL_CLIP_ID:
            resultInfo.PutIntValue("action", data.ReadInt32());
            resultInfo.PutIntValue("clipStat", data.ReadInt32());
            break;
        case CallResultReportId::GET_CALL_CLIR_ID:
            resultInfo.PutIntValue("action", data.ReadInt32());
            resultInfo.PutIntValue("clirStat", data.ReadInt32());
            break;
        case CallResultReportId::START_RTT_REPORT_ID:
            resultInfo.PutIntValue("active", data.ReadInt32());
            break;
        case CallResultReportId::GET_IMS_CONFIG_REPORT_ID:
        case CallResultReportId::GET_IMS_FEATURE_VALUE_REPORT_ID:
            resultInfo.PutIntValue("value", data.ReadInt32());
            break;
        case CallResultReportId::STOP_RTT_REPORT_ID:
            resultInfo.PutIntValue("inactive", data.ReadInt32());
            break;
        default:
            break;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = OnReportAsyncResults(reportId, resultInfo);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateMmiCodeResults(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    const MmiCodeInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const MmiCodeInfo *>(data.ReadRawData(sizeof(MmiCodeInfo)))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    MmiCodeInfo info = *parcelPtr;
    result = OnReportMmiCodeResult(info);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateAudioDeviceChange(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    AudioDeviceInfo info;
    if (memset_s(&info, sizeof(AudioDeviceInfo), 0, sizeof(AudioDeviceInfo)) != EOK) {
        TELEPHONY_LOGE("memset_s address fail");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    AudioDevice *audioDevicePtr = nullptr;
    for (int32_t i = 0; i < len + 1; i++) {
        audioDevicePtr = static_cast<AudioDevice *>(const_cast<void *>(data.ReadRawData(sizeof(AudioDevice))));
        if (audioDevicePtr == nullptr) {
            TELEPHONY_LOGE("Invalid parameter audioDevicePtr");
            return TELEPHONY_ERR_ARGUMENT_INVALID;
        }
        if (i < len) {
            info.audioDeviceList.push_back(*audioDevicePtr);
        } else {
            info.currentAudioDevice = *audioDevicePtr;
        }
    }

    info.isMuted = data.ReadBool();
    result = OnReportAudioDeviceChange(info);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateOttCallRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    AppExecFwk::PacMap resultInfo;
    OttCallRequestId requestId = static_cast<OttCallRequestId>(data.ReadInt32());
    resultInfo.PutStringValue("phoneNumber", data.ReadString());
    resultInfo.PutStringValue("bundleName", data.ReadString());
    resultInfo.PutIntValue("videoState", data.ReadInt32());
    switch (requestId) {
        case OttCallRequestId::OTT_REQUEST_INVITE_TO_CONFERENCE:
            resultInfo.PutStringValue("number", data.ReadString());
            break;
        case OttCallRequestId::OTT_REQUEST_UPDATE_CALL_MEDIA_MODE:
            resultInfo.PutIntValue("callMediaMode", data.ReadInt32());
            break;
        default:
            break;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = OnOttCallRequest(requestId, resultInfo);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdatePostDialDelay(MessageParcel &data, MessageParcel &reply)
{
    std::string remainPostDial = data.ReadString();
    int32_t result = OnReportPostDialDelay(remainPostDial);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateImsCallModeChange(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    const CallMediaModeInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const CallMediaModeInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    result = OnReportImsCallModeChange(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateCallSessionEventChange(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }

    const CallSessionEvent *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const CallSessionEvent *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    result = OnReportCallSessionEventChange(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdatePeerDimensionsChange(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }

    const PeerDimensionsDetail *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const PeerDimensionsDetail *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    result = OnReportPeerDimensionsChange(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateCallDataUsageChange(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }

    int64_t dataUsage = data.ReadInt64();
    result = OnReportCallDataUsageChange(dataUsage);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateCameraCapabilities(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }

    const CameraCapabilities *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const CameraCapabilities *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    result = OnReportCameraCapabilities(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
