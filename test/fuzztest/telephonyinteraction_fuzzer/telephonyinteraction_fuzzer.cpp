/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "telephonyinteraction_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_status_callback_proxy.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t ACCOUNT_ID_NUM = 10;
constexpr int32_t MULTI_PARTY_NUM = 10;
constexpr int32_t VOICE_DOMAIN_NUM = 10;
constexpr int32_t IMS_CALL_MODE_NUM = 5;
constexpr int32_t CALL_INDEX_MAX_NUM = 8;
constexpr int32_t VIDEO_REQUEST_RESULT_TYPE_NUM = 102;
constexpr int32_t CALL_SESSION_EVENT_ID_NUM = 4;
std::unique_ptr<CallStatusCallback> CallStatusCallbackPtr_ = nullptr;

bool ServiceInited()
{
    bool result = true;
    if (!IsServiceInited()) {
        return false;
    }
    CallStatusCallbackPtr_ = std::make_unique<CallStatusCallback>();
    if (CallStatusCallbackPtr_ == nullptr) {
        result = false;
    }
    return result;
}

int32_t OnRemoteRequest(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataMessageParcel;
    if (!dataMessageParcel.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataMessageParcel.RewindRead(0);
    uint32_t code = static_cast<uint32_t>(*data);
    MessageParcel reply;
    MessageOption option;
    return CallStatusCallbackPtr_->OnRemoteRequest(code, dataMessageParcel, reply, option);
}

int32_t UpdateCallReportInfo(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallReportInfo callReportInfo;
    callReportInfo.index = static_cast<int32_t>(size);
    callReportInfo.accountId = static_cast<int32_t>(size % ACCOUNT_ID_NUM);
    callReportInfo.voiceDomain = static_cast<int32_t>(size % VOICE_DOMAIN_NUM);
    callReportInfo.mpty = static_cast<int32_t>(size % MULTI_PARTY_NUM);
    callReportInfo.callType = CallType::TYPE_ERR_CALL;
    callReportInfo.callMode = VideoStateType::TYPE_VOICE;
    callReportInfo.state = TelCallState::CALL_STATUS_UNKNOWN;
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t accountLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(callReportInfo.accountNum, kMaxNumberLen, msg.c_str(), accountLength);
    dataParcel.WriteInt32(callReportInfo.index);
    dataParcel.WriteCString(callReportInfo.accountNum);
    dataParcel.WriteInt32(callReportInfo.accountId);
    dataParcel.WriteInt32(static_cast<int32_t>(callReportInfo.callType));
    dataParcel.WriteInt32(static_cast<int32_t>(callReportInfo.callMode));
    dataParcel.WriteInt32(static_cast<int32_t>(callReportInfo.state));
    dataParcel.WriteInt32(callReportInfo.voiceDomain);
    dataParcel.WriteInt32(callReportInfo.mpty);
    dataParcel.WriteInt32(callReportInfo.crsType);
    dataParcel.WriteInt32(callReportInfo.originalCallType);
    dataParcel.WriteString(callReportInfo.voipCallInfo.voipCallId);
    dataParcel.WriteString(callReportInfo.voipCallInfo.userName);
    dataParcel.WriteString(callReportInfo.voipCallInfo.abilityName);
    dataParcel.WriteString(callReportInfo.voipCallInfo.extensionId);
    dataParcel.WriteString(callReportInfo.voipCallInfo.voipBundleName);
    dataParcel.WriteUInt8Vector(callReportInfo.voipCallInfo.userProfile);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateCallReportInfo(dataParcel, reply);
}

int32_t UpdateCallsReportInfo(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallReportInfo info;
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    int32_t vecSize = 1;
    info.index = static_cast<int32_t>(size);
    info.accountId = static_cast<int32_t>(size % ACCOUNT_ID_NUM);
    info.voiceDomain = static_cast<int32_t>(size % VOICE_DOMAIN_NUM);
    info.mpty = static_cast<int32_t>(size % MULTI_PARTY_NUM);
    info.callType = CallType::TYPE_ERR_CALL;
    info.callMode = VideoStateType::TYPE_VOICE;
    info.state = TelCallState::CALL_STATUS_UNKNOWN;
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t accountLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(info.accountNum, kMaxNumberLen, msg.c_str(), accountLength);
    dataParcel.WriteInt32(vecSize);
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
    dataParcel.WriteString(info.voipCallInfo.voipCallId);
    dataParcel.WriteString(info.voipCallInfo.userName);
    dataParcel.WriteString(info.voipCallInfo.abilityName);
    dataParcel.WriteString(info.voipCallInfo.extensionId);
    dataParcel.WriteString(info.voipCallInfo.voipBundleName);
    dataParcel.WriteUInt8Vector(info.voipCallInfo.userProfile);
    dataParcel.WriteInt32(slotId);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateCallsReportInfo(dataParcel, reply);
}

int32_t UpdateEventReport(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CellularCallEventInfo info;
    int32_t length = sizeof(CellularCallEventInfo);
    info.eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;
    info.eventId = RequestResultEventId::INVALID_REQUEST_RESULT_EVENT_ID;
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateEventReport(dataParcel, reply);
}

int32_t UpdateGetWaitingResult(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallWaitResponse callWaitResponse;
    int32_t length = sizeof(CallWaitResponse);
    dataParcel.WriteInt32(length);
    callWaitResponse.result = static_cast<int32_t>(*data);
    callWaitResponse.status = static_cast<int32_t>(*data);
    callWaitResponse.classCw = static_cast<int32_t>(*data);
    dataParcel.WriteRawData((const void *)&callWaitResponse, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetWaitingResult(dataParcel, reply);
}

int32_t UpdateGetRestrictionResult(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallRestrictionResponse callRestrictionResult;
    int32_t length = sizeof(CallRestrictionResponse);
    dataParcel.WriteInt32(length);
    callRestrictionResult.result = static_cast<int32_t>(*data);
    callRestrictionResult.status = static_cast<int32_t>(*data);
    callRestrictionResult.classCw = static_cast<int32_t>(*data);
    dataParcel.WriteRawData((const void *)&callRestrictionResult, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetRestrictionResult(dataParcel, reply);
}

int32_t UpdateGetTransferResult(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallTransferResponse callTransferResponse;
    int32_t length = sizeof(CallTransferResponse);
    dataParcel.WriteInt32(length);
    callTransferResponse.result = static_cast<int32_t>(size);
    callTransferResponse.status = static_cast<int32_t>(size);
    callTransferResponse.classx = static_cast<int32_t>(size);
    callTransferResponse.type = static_cast<int32_t>(size);
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t accountLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(callTransferResponse.number, kMaxNumberLen, msg.c_str(), accountLength);
    callTransferResponse.reason = static_cast<int32_t>(size);
    callTransferResponse.time = static_cast<int32_t>(size);
    callTransferResponse.startHour = static_cast<int32_t>(size);
    callTransferResponse.startMinute = static_cast<int32_t>(size);
    callTransferResponse.endHour = static_cast<int32_t>(size);
    callTransferResponse.endMinute = static_cast<int32_t>(size);
    dataParcel.WriteRawData((const void *)&callTransferResponse, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetTransferResult(dataParcel, reply);
}

int32_t UpdateGetCallClipResult(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    ClipResponse clipResponse;
    int32_t length = sizeof(ClipResponse);
    dataParcel.WriteInt32(length);
    clipResponse.result = static_cast<int32_t>(*data);
    clipResponse.action = static_cast<int32_t>(*data);
    clipResponse.clipStat = static_cast<int32_t>(*data);
    dataParcel.WriteRawData((const void *)&clipResponse, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetCallClipResult(dataParcel, reply);
}

int32_t GetImsConfigResult(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    GetImsConfigResponse response;
    int32_t length = sizeof(GetImsConfigResponse);
    dataParcel.WriteInt32(length);
    response.result = static_cast<int32_t>(*data);
    response.value = static_cast<int32_t>(*data);
    dataParcel.WriteRawData((const void *)&response, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnGetImsConfigResult(dataParcel, reply);
}

int32_t GetImsFeatureValueResult(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    GetImsFeatureValueResponse response;
    int32_t length = sizeof(GetImsFeatureValueResponse);
    dataParcel.WriteInt32(length);
    response.result = static_cast<int32_t>(*data);
    response.value = static_cast<int32_t>(*data);
    dataParcel.WriteRawData((const void *)&response, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnGetImsFeatureValueResult(dataParcel, reply);
}

int32_t SendMmiCodeResult(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MmiCodeInfo info;
    int32_t length = sizeof(MmiCodeInfo);
    dataParcel.WriteInt32(length);
    info.result = static_cast<int32_t>(size);
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t msgLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(info.message, kMaxNumberLen, msg.c_str(), msgLength);
    dataParcel.WriteRawData((const void *)&info, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnSendMmiCodeResult(dataParcel, reply);
}

int32_t ReceiveUpdateCallMediaModeRequest(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t length = sizeof(CallModeReportInfo);
    dataParcel.WriteInt32(length);
    CallModeReportInfo callModeReportInfo;
    callModeReportInfo.callIndex = static_cast<int32_t>(*data % CALL_INDEX_MAX_NUM);
    callModeReportInfo.callMode = static_cast<ImsCallMode>(*data % IMS_CALL_MODE_NUM);
    callModeReportInfo.result = static_cast<VideoRequestResultType>(*data % VIDEO_REQUEST_RESULT_TYPE_NUM);
    dataParcel.WriteRawData((const void *)&callModeReportInfo, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnReceiveImsCallModeRequest(dataParcel, reply);
}

int32_t ReceiveUpdateCallMediaModeResponse(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t length = sizeof(CallModeReportInfo);
    dataParcel.WriteInt32(length);
    CallModeReportInfo callModeReportInfo;
    callModeReportInfo.callIndex = static_cast<int32_t>(*data % CALL_INDEX_MAX_NUM);
    callModeReportInfo.callMode = static_cast<ImsCallMode>(*data % IMS_CALL_MODE_NUM);
    callModeReportInfo.result = static_cast<VideoRequestResultType>(*data % VIDEO_REQUEST_RESULT_TYPE_NUM);
    dataParcel.WriteRawData((const void *)&callModeReportInfo, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnReceiveImsCallModeResponse(dataParcel, reply);
}

int32_t HandleCallSessionEventChanged(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t length = sizeof(CallSessionReportInfo);
    dataParcel.WriteInt32(length);
    CallSessionReportInfo callSessionReportInfo;
    callSessionReportInfo.index = static_cast<int32_t>(*data % CALL_INDEX_MAX_NUM);
    callSessionReportInfo.eventId = static_cast<CallSessionEventId>(*data % CALL_SESSION_EVENT_ID_NUM);
    dataParcel.WriteRawData((const void *)&callSessionReportInfo, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnCallSessionEventChange(dataParcel, reply);
}

int32_t HandlePeerDimensionsChanged(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t length = sizeof(PeerDimensionsReportInfo);
    dataParcel.WriteInt32(length);
    PeerDimensionsReportInfo dimensionsReportInfo;
    dimensionsReportInfo.index = static_cast<int32_t>(*data % CALL_INDEX_MAX_NUM);
    dimensionsReportInfo.width = static_cast<int32_t>(*data);
    dimensionsReportInfo.height = static_cast<int32_t>(*data);
    dataParcel.WriteRawData((const void *)&dimensionsReportInfo, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnPeerDimensionsChange(dataParcel, reply);
}

int32_t HandleCallDataUsageChanged(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int64_t reportInfo = static_cast<int64_t>(*data);
    dataParcel.WriteInt64(reportInfo);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnCallDataUsageChange(dataParcel, reply);
}

int32_t HandleCameraCapabilitiesChanged(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t length = sizeof(CameraCapabilitiesReportInfo);
    dataParcel.WriteInt32(length);
    CameraCapabilitiesReportInfo cameraCapabilitiesReportInfo;
    cameraCapabilitiesReportInfo.index = static_cast<int32_t>(*data % CALL_INDEX_MAX_NUM);
    cameraCapabilitiesReportInfo.width = static_cast<int32_t>(*data);
    cameraCapabilitiesReportInfo.height = static_cast<int32_t>(*data);
    dataParcel.WriteRawData((const void *)&cameraCapabilitiesReportInfo, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnCameraCapabilitiesChange(dataParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    OnRemoteRequest(data, size);
    UpdateCallReportInfo(data, size);
    UpdateCallsReportInfo(data, size);
    UpdateEventReport(data, size);
    UpdateGetWaitingResult(data, size);
    UpdateGetRestrictionResult(data, size);
    UpdateGetTransferResult(data, size);
    UpdateGetCallClipResult(data, size);
    GetImsConfigResult(data, size);
    GetImsFeatureValueResult(data, size);
    SendMmiCodeResult(data, size);
    ReceiveUpdateCallMediaModeRequest(data, size);
    ReceiveUpdateCallMediaModeResponse(data, size);
    HandleCallSessionEventChanged(data, size);
    HandlePeerDimensionsChanged(data, size);
    HandleCallDataUsageChanged(data, size);
    HandleCameraCapabilitiesChanged(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
