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

#include "call_status_callback_stub.h"

#include <securec.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
const int32_t MAX_LEN = 100000;
const int32_t MAX_CALL_NUM = 10;
CallStatusCallbackStub::CallStatusCallbackStub()
{
    memberFuncMap_[static_cast<uint32_t>(UPDATE_CALL_INFO)] = &CallStatusCallbackStub::OnUpdateCallReportInfo;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_CALLS_INFO)] = &CallStatusCallbackStub::OnUpdateCallsReportInfo;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_DISCONNECTED_CAUSE)] =
        &CallStatusCallbackStub::OnUpdateDisconnectedCause;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_EVENT_RESULT_INFO)] = &CallStatusCallbackStub::OnUpdateEventReport;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_RBT_PLAY_INFO)] = &CallStatusCallbackStub::OnUpdateRBTPlayInfo;
    memberFuncMap_[static_cast<uint32_t>(START_DTMF)] = &CallStatusCallbackStub::OnStartDtmfResult;
    memberFuncMap_[static_cast<uint32_t>(STOP_DTMF)] = &CallStatusCallbackStub::OnStopDtmfResult;
    memberFuncMap_[static_cast<uint32_t>(SEND_USSD)] = &CallStatusCallbackStub::OnSendUssdResult;
    memberFuncMap_[static_cast<uint32_t>(GET_IMS_CALL_DATA)] = &CallStatusCallbackStub::OnGetImsCallDataResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_GET_WAITING)] = &CallStatusCallbackStub::OnUpdateGetWaitingResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_SET_WAITING)] = &CallStatusCallbackStub::OnUpdateSetWaitingResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_GET_RESTRICTION)] =
        &CallStatusCallbackStub::OnUpdateGetRestrictionResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_SET_RESTRICTION)] =
        &CallStatusCallbackStub::OnUpdateSetRestrictionResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_SET_RESTRICTION_PWD)] =
        &CallStatusCallbackStub::OnUpdateSetRestrictionPasswordResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_GET_TRANSFER)] = &CallStatusCallbackStub::OnUpdateGetTransferResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_SET_TRANSFER)] = &CallStatusCallbackStub::OnUpdateSetTransferResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_GET_CALL_CLIP)] = &CallStatusCallbackStub::OnUpdateGetCallClipResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_GET_CALL_CLIR)] = &CallStatusCallbackStub::OnUpdateGetCallClirResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_SET_CALL_CLIR)] = &CallStatusCallbackStub::OnUpdateSetCallClirResult;
    memberFuncMap_[static_cast<uint32_t>(GET_IMS_CONFIG)] = &CallStatusCallbackStub::OnGetImsConfigResult;
    memberFuncMap_[static_cast<uint32_t>(SET_IMS_CONFIG)] = &CallStatusCallbackStub::OnSetImsConfigResult;
    memberFuncMap_[static_cast<uint32_t>(GET_IMS_FEATURE_VALUE)] = &CallStatusCallbackStub::OnGetImsFeatureValueResult;
    memberFuncMap_[static_cast<uint32_t>(SET_IMS_FEATURE_VALUE)] = &CallStatusCallbackStub::OnSetImsFeatureValueResult;
    memberFuncMap_[static_cast<uint32_t>(RECEIVE_UPDATE_MEDIA_MODE_RESPONSE)] =
        &CallStatusCallbackStub::OnReceiveImsCallModeResponse;
    memberFuncMap_[static_cast<uint32_t>(RECEIVE_UPDATE_MEDIA_MODE_REQUEST)] =
        &CallStatusCallbackStub::OnReceiveImsCallModeRequest;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_STARTRTT_STATUS)] = &CallStatusCallbackStub::OnStartRttResult;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_STOPRTT_STATUS)] = &CallStatusCallbackStub::OnStopRttResult;
    memberFuncMap_[static_cast<uint32_t>(INVITE_TO_CONFERENCE)] = &CallStatusCallbackStub::OnInviteToConferenceResult;
    memberFuncMap_[static_cast<uint32_t>(MMI_CODE_INFO_RESPONSE)] = &CallStatusCallbackStub::OnSendMmiCodeResult;
    memberFuncMap_[static_cast<uint32_t>(CLOSE_UNFINISHED_USSD)] = &CallStatusCallbackStub::OnCloseUnFinishedUssdResult;
    memberFuncMap_[static_cast<uint32_t>(POST_DIAL_CHAR)] = &CallStatusCallbackStub::OnPostDialNextChar;
    memberFuncMap_[static_cast<uint32_t>(POST_DIAL_DELAY)] = &CallStatusCallbackStub::OnReportPostDialDelay;
    memberFuncMap_[static_cast<uint32_t>(CALL_SESSION_EVENT)] = &CallStatusCallbackStub::OnCallSessionEventChange;
    memberFuncMap_[static_cast<uint32_t>(PEER_DIMENSION_CHANGE)] = &CallStatusCallbackStub::OnPeerDimensionsChange;
    memberFuncMap_[static_cast<uint32_t>(CALL_DATA_USAGE)] = &CallStatusCallbackStub::OnCallDataUsageChange;
    memberFuncMap_[static_cast<uint32_t>(CAMERA_CAPBILITIES_CHANGE)] =
        &CallStatusCallbackStub::OnCameraCapabilitiesChange;
    memberFuncMap_[static_cast<uint32_t>(UPDATE_VOIP_EVENT_INFO)] = &CallStatusCallbackStub::OnUpdateVoipEventInfo;
}

CallStatusCallbackStub::~CallStatusCallbackStub()
{
    memberFuncMap_.clear();
}

int32_t CallStatusCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescriptor = CallStatusCallbackStub::GetDescriptor();
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

int32_t CallStatusCallbackStub::OnUpdateCallReportInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    CallReportInfo parcelPtr;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    parcelPtr.index = data.ReadInt32();
    strncpy_s(parcelPtr.accountNum, kMaxNumberLen + 1, data.ReadCString(), kMaxNumberLen + 1);
    parcelPtr.accountId = data.ReadInt32();
    parcelPtr.callType = static_cast<CallType>(data.ReadInt32());
    parcelPtr.callMode = static_cast<VideoStateType>(data.ReadInt32());
    parcelPtr.state = static_cast<TelCallState>(data.ReadInt32());
    parcelPtr.voiceDomain = data.ReadInt32();
    parcelPtr.mpty = data.ReadInt32();
    parcelPtr.crsType = data.ReadInt32();
    parcelPtr.originalCallType = data.ReadInt32();
    if (parcelPtr.callType == CallType::TYPE_VOIP) {
        parcelPtr.voipCallInfo.voipCallId = data.ReadString();
        parcelPtr.voipCallInfo.userName = data.ReadString();
        parcelPtr.voipCallInfo.abilityName = data.ReadString();
        parcelPtr.voipCallInfo.extensionId = data.ReadString();
        parcelPtr.voipCallInfo.voipBundleName = data.ReadString();
        parcelPtr.voipCallInfo.showBannerForIncomingCall = data.ReadBool();
        std::vector<uint8_t> userProfile = {};
        data.ReadUInt8Vector(&userProfile);
        (parcelPtr.voipCallInfo.userProfile).assign(userProfile.begin(), userProfile.end());
    }
    result = UpdateCallReportInfo(parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateCallsReportInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t cnt = data.ReadInt32();
    if (cnt <= 0 || cnt > MAX_CALL_NUM) {
        TELEPHONY_LOGE("invalid parameter, cnt = %{public}d", cnt);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    TELEPHONY_LOGI("call list size:%{public}d", cnt);
    CallsReportInfo callReportInfo;
    CallReportInfo parcelPtr;
    for (int32_t i = 0; i < cnt; i++) {
        parcelPtr.index = data.ReadInt32();
        strncpy_s(parcelPtr.accountNum, kMaxNumberLen + 1, data.ReadCString(), kMaxNumberLen + 1);
        parcelPtr.accountId = data.ReadInt32();
        parcelPtr.callType = static_cast<CallType>(data.ReadInt32());
        parcelPtr.callMode = static_cast<VideoStateType>(data.ReadInt32());
        parcelPtr.state = static_cast<TelCallState>(data.ReadInt32());
        parcelPtr.voiceDomain = data.ReadInt32();
        parcelPtr.mpty = data.ReadInt32();
        parcelPtr.crsType = data.ReadInt32();
        parcelPtr.originalCallType = data.ReadInt32();
        if (parcelPtr.callType == CallType::TYPE_VOIP) {
            parcelPtr.voipCallInfo.voipCallId = data.ReadString();
            parcelPtr.voipCallInfo.userName = data.ReadString();
            parcelPtr.voipCallInfo.abilityName = data.ReadString();
            parcelPtr.voipCallInfo.extensionId = data.ReadString();
            parcelPtr.voipCallInfo.voipBundleName = data.ReadString();
            parcelPtr.voipCallInfo.showBannerForIncomingCall = data.ReadBool();
            std::vector<uint8_t> userProfile = {};
            data.ReadUInt8Vector(&userProfile);
            (parcelPtr.voipCallInfo.userProfile).assign(userProfile.begin(), userProfile.end());
        }
        callReportInfo.callVec.push_back(parcelPtr);
        TELEPHONY_LOGI("accountId:%{public}d,state:%{public}d", parcelPtr.accountId, parcelPtr.state);
    }
    callReportInfo.slotId = data.ReadInt32();
    TELEPHONY_LOGI("slotId:%{public}d", callReportInfo.slotId);
    result = UpdateCallsReportInfo(callReportInfo);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateDisconnectedCause(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    DisconnectedDetails dcDetails;
    dcDetails.reason = static_cast<DisconnectedReason>(data.ReadInt32());
    dcDetails.message = data.ReadString();
    result = UpdateDisconnectedCause(dcDetails);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateEventReport(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    const CellularCallEventInfo *parcelPtr = nullptr;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const CellularCallEventInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    result = UpdateEventResultInfo(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateRBTPlayInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    RBTPlayInfo rbtInfo = static_cast<RBTPlayInfo>(data.ReadInt32());
    result = UpdateRBTPlayInfo(rbtInfo);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateGetWaitingResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    const CallWaitResponse *parcelPtr = nullptr;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const CallWaitResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    result = UpdateGetWaitingResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateSetWaitingResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t callWaitResult = data.ReadInt32();
    result = UpdateSetWaitingResult(callWaitResult);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateGetRestrictionResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    const CallRestrictionResponse *parcelPtr = nullptr;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const CallRestrictionResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    result = UpdateGetRestrictionResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateSetRestrictionResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = UpdateSetRestrictionResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateSetRestrictionPasswordResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = UpdateSetRestrictionPasswordResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateGetTransferResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    const CallTransferResponse *parcelPtr = nullptr;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const CallTransferResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    result = UpdateGetTransferResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateSetTransferResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = UpdateSetTransferResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateGetCallClipResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    const ClipResponse *parcelPtr = nullptr;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const ClipResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    result = UpdateGetCallClipResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateGetCallClirResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    const ClirResponse *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const ClirResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    result = UpdateGetCallClirResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateSetCallClirResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = UpdateSetCallClirResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnStartRttResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = StartRttResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnStopRttResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = StopRttResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnGetImsConfigResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    const GetImsConfigResponse *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const GetImsConfigResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    error = GetImsConfigResult(*parcelPtr);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnSetImsConfigResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = SetImsConfigResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnGetImsFeatureValueResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    const GetImsFeatureValueResponse *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const GetImsFeatureValueResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    error = GetImsFeatureValueResult(*parcelPtr);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnSetImsFeatureValueResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = SetImsFeatureValueResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnReceiveImsCallModeRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    const CallModeReportInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const CallModeReportInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    error = ReceiveUpdateCallMediaModeRequest(*parcelPtr);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnReceiveImsCallModeResponse(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    const CallModeReportInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const CallModeReportInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    error = ReceiveUpdateCallMediaModeResponse(*parcelPtr);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnInviteToConferenceResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = InviteToConferenceResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnStartDtmfResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = StartDtmfResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnStopDtmfResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = StopDtmfResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnSendUssdResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = SendUssdResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnSendMmiCodeResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    const MmiCodeInfo *parcelPtr = nullptr;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const MmiCodeInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    result = SendMmiCodeResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnGetImsCallDataResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = GetImsCallDataResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnCloseUnFinishedUssdResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = CloseUnFinishedUssdResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnPostDialNextChar(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    std::string c = data.ReadString();
    error = ReportPostDialChar(c);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnReportPostDialDelay(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    std::string remainPostDial = data.ReadString();
    error = ReportPostDialDelay(remainPostDial);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnCallSessionEventChange(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    const CallSessionReportInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const CallSessionReportInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    error = HandleCallSessionEventChanged(*parcelPtr);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnPeerDimensionsChange(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    const PeerDimensionsReportInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const PeerDimensionsReportInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    error = HandlePeerDimensionsChanged(*parcelPtr);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnCallDataUsageChange(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int64_t result = data.ReadInt64();
    error = HandleCallDataUsageChanged(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnCameraCapabilitiesChange(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    const CameraCapabilitiesReportInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const CameraCapabilitiesReportInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    error = HandleCameraCapabilitiesChanged(*parcelPtr);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateVoipEventInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    const VoipCallEventInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= 0 || len >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if ((parcelPtr = reinterpret_cast<const VoipCallEventInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    error = UpdateVoipEventInfo(*parcelPtr);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
