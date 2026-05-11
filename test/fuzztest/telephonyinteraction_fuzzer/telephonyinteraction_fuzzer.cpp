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
#include "fuzzer/FuzzedDataProvider.h"

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

int32_t OnRemoteRequest(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    messageParcel.RewindRead(0);
    uint32_t code = provider.ConsumeIntergral<uint32_t>();
    MessageParcel reply;
    MessageOption option;
    return CallStatusCallbackPtr_->OnRemoteRequest(code, messageParcel, reply, option);
}

int32_t UpdateCallReportInfo(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallReportInfo callReportInfo;
    callReportInfo.index = provider.ConsumeIntergral<int32_t>();
    callReportInfo.accountId = provider.ConsumeIntergral<int32_t>() % ACCOUNT_ID_NUM;
    callReportInfo.voiceDomain = provider.ConsumeIntergral<int32_t>() % VOICE_DOMAIN_NUM;
    callReportInfo.mpty = provider.ConsumeIntergral<int32_t>() % MULTI_PARTY_NUM;
    callReportInfo.callType = CallType::TYPE_ERR_CALL;
    callReportInfo.callMode = VideoStateType::TYPE_VOICE;
    callReportInfo.state = TelCallState::CALL_STATUS_UNKNOWN;
    std::string msg = provider.consumeString();
    int32_t accountLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(callReportInfo.accountNum, kMaxNumberLen, msg.c_str(), accountLength);
    messageParcel.WriteInt32(callReportInfo.index);
    messageParcel.WriteCString(callReportInfo.accountNum);
    messageParcel.WriteInt32(callReportInfo.accountId);
    messageParcel.WriteInt32(static_cast<int32_t>(callReportInfo.callType));
    messageParcel.WriteInt32(static_cast<int32_t>(callReportInfo.callMode));
    messageParcel.WriteInt32(static_cast<int32_t>(callReportInfo.state));
    messageParcel.WriteInt32(callReportInfo.voiceDomain);
    messageParcel.WriteInt32(callReportInfo.mpty);
    messageParcel.WriteInt32(callReportInfo.crsType);
    messageParcel.WriteInt32(callReportInfo.originalCallType);
    messageParcel.WriteString(callReportInfo.voipCallInfo.voipCallId);
    messageParcel.WriteString(callReportInfo.voipCallInfo.userName);
    messageParcel.WriteString(callReportInfo.voipCallInfo.abilityName);
    messageParcel.WriteString(callReportInfo.voipCallInfo.extensionId);
    messageParcel.WriteString(callReportInfo.voipCallInfo.voipBundleName);
    messageParcel.WriteUInt8Vector(callReportInfo.voipCallInfo.userProfile);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateCallReportInfo(messageParcel, reply);
}

int32_t UpdateCallsReportInfo(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallReportInfo info;
    int32_t slotId = provider.ConsumeIntergral<int32_t>() % SLOT_NUM;
    int32_t vecSize = 1;
    info.index = provider.ConsumeIntergral<int32_t>();
    info.accountId = provider.ConsumeIntergral<int32_t>() % ACCOUNT_ID_NUM;
    info.voiceDomain = provider.ConsumeIntergral<int32_t>() % VOICE_DOMAIN_NUM;
    info.mpty = provider.ConsumeIntergral<int32_t>() % MULTI_PARTY_NUM;
    info.callType = CallType::TYPE_ERR_CALL;
    info.callMode = VideoStateType::TYPE_VOICE;
    info.state = TelCallState::CALL_STATUS_UNKNOWN;
    std::string msg = provider.consumeString();
    int32_t accountLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(info.accountNum, kMaxNumberLen, msg.c_str(), accountLength);
    messageParcel.WriteInt32(vecSize);
    messageParcel.WriteInt32(info.index);
    messageParcel.WriteCString(info.accountNum);
    messageParcel.WriteInt32(info.accountId);
    messageParcel.WriteInt32(static_cast<int32_t>(info.callType));
    messageParcel.WriteInt32(static_cast<int32_t>(info.callMode));
    messageParcel.WriteInt32(static_cast<int32_t>(info.state));
    messageParcel.WriteInt32(info.voiceDomain);
    messageParcel.WriteInt32(info.mpty);
    messageParcel.WriteInt32(info.crsType);
    messageParcel.WriteInt32(info.originalCallType);
    messageParcel.WriteString(info.voipCallInfo.voipCallId);
    messageParcel.WriteString(info.voipCallInfo.userName);
    messageParcel.WriteString(info.voipCallInfo.abilityName);
    messageParcel.WriteString(info.voipCallInfo.extensionId);
    messageParcel.WriteString(info.voipCallInfo.voipBundleName);
    messageParcel.WriteUInt8Vector(info.voipCallInfo.userProfile);
    messageParcel.WriteString(info.name);
    messageParcel.WriteInt32(info.namePresentation);
    messageParcel.WriteInt32(static_cast<int32_t>(info.reason));
    messageParcel.WriteString(info.message);
    messageParcel.WriteInt32(slotId);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateCallsReportInfo(messageParcel, reply);
}

int32_t UpdateEventReport(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CellularCallEventInfo info;
    int32_t length = sizeof(CellularCallEventInfo);
    info.eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;
    info.eventId = RequestResultEventId::INVALID_REQUEST_RESULT_EVENT_ID;
    messageParcel.WriteInt32(length);
    messageParcel.WriteRawData((const void *)&info, length);
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateEventReport(messageParcel, reply);
}

int32_t UpdateGetWaitingResult(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallWaitResponse callWaitResponse;
    int32_t length = sizeof(CallWaitResponse);
    messageParcel.WriteInt32(length);
    callWaitResponse.result = provider.ConsumeIntergral<int32_t>();
    callWaitResponse.status = provider.ConsumeIntergral<int32_t>();
    callWaitResponse.classCw = provider.ConsumeIntergral<int32_t>();
    messageParcel.WriteRawData((const void *)&callWaitResponse, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetWaitingResult(messageParcel, reply);
}

int32_t UpdateGetRestrictionResult(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallRestrictionResponse callRestrictionResult;
    int32_t length = sizeof(CallRestrictionResponse);
    messageParcel.WriteInt32(length);
    callRestrictionResult.result = provider.ConsumeIntergral<int32_t>();
    callRestrictionResult.status = provider.ConsumeIntergral<int32_t>();
    callRestrictionResult.classCw = provider.ConsumeIntergral<int32_t>();
    messageParcel.WriteRawData((const void *)&callRestrictionResult, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetRestrictionResult(messageParcel, reply);
}

int32_t UpdateGetTransferResult(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallTransferResponse callTransferResponse;
    int32_t length = sizeof(CallTransferResponse);
    messageParcel.WriteInt32(length);
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
    messageParcel.WriteRawData((const void *)&callTransferResponse, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetTransferResult(messageParcel, reply);
}

int32_t UpdateGetCallClipResult(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    ClipResponse clipResponse;
    int32_t length = sizeof(ClipResponse);
    messageParcel.WriteInt32(length);
    clipResponse.result = provider.ConsumeIntergral<int32_t>();
    clipResponse.action = provider.ConsumeIntergral<int32_t>();
    clipResponse.clipStat = provider.ConsumeIntergral<int32_t>();
    messageParcel.WriteRawData((const void *)&clipResponse, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetCallClipResult(messageParcel, reply);
}

int32_t GetImsConfigResult(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    GetImsConfigResponse response;
    int32_t length = sizeof(GetImsConfigResponse);
    messageParcel.WriteInt32(length);
    response.result = provider.ConsumeIntergral<int32_t>();
    response.value = provider.ConsumeIntergral<int32_t>();
    messageParcel.WriteRawData((const void *)&response, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnGetImsConfigResult(messageParcel, reply);
}

int32_t GetImsFeatureValueResult(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    GetImsFeatureValueResponse response;
    int32_t length = sizeof(GetImsFeatureValueResponse);
    messageParcel.WriteInt32(length);
    response.result = provider.ConsumeIntergral<int32_t>();
    response.value = provider.ConsumeIntergral<int32_t>();
    messageParcel.WriteRawData((const void *)&response, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnGetImsFeatureValueResult(messageParcel, reply);
}

int32_t SendMmiCodeResult(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MmiCodeInfo info;
    int32_t length = sizeof(MmiCodeInfo);
    messageParcel.WriteInt32(length);
    info.result = provider.ConsumeIntergral<int32_t>();
    std::string msg = provider.consumeString();
    int32_t msgLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(info.message, kMaxNumberLen, msg.c_str(), msgLength);
    messageParcel.WriteRawData((const void *)&info, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnSendMmiCodeResult(messageParcel, reply);
}

int32_t ReceiveUpdateCallMediaModeRequest(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t length = sizeof(CallModeReportInfo);
    messageParcel.WriteInt32(length);
    CallModeReportInfo callModeReportInfo;
    callModeReportInfo.callIndex = provider.ConsumeIntergral<int32_t>() % CALL_INDEX_MAX_NUM;
    callModeReportInfo.callMode = provider.ConsumeIntergral<int32_t>() % IMS_CALL_MODE_NUM;
    callModeReportInfo.result = provider.ConsumeIntergral<int32_t>() % VIDEO_REQUEST_RESULT_TYPE_NUM;
    messageParcel.WriteRawData((const void *)&callModeReportInfo, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnReceiveImsCallModeRequest(messageParcel, reply);
}

int32_t ReceiveUpdateCallMediaModeResponse(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t length = sizeof(CallModeReportInfo);
    messageParcel.WriteInt32(length);
    CallModeReportInfo callModeReportInfo;
    callModeReportInfo.callIndex = provider.ConsumeIntergral<int32_t>() % CALL_INDEX_MAX_NUM;
    callModeReportInfo.callMode = provider.ConsumeIntergral<int32_t>() % IMS_CALL_MODE_NUM;
    callModeReportInfo.result = provider.ConsumeIntergral<VideoRequestResultType>() % VIDEO_REQUEST_RESULT_TYPE_NUM;
    messageParcel.WriteRawData((const void *)&callModeReportInfo, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnReceiveImsCallModeResponse(messageParcel, reply);
}

int32_t HandleCallSessionEventChanged(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t length = sizeof(CallSessionReportInfo);
    messageParcel.WriteInt32(length);
    CallSessionReportInfo callSessionReportInfo;
    callSessionReportInfo.index = provider.ConsumeIntergral<int32_t>() % CALL_INDEX_MAX_NUM;
    callSessionReportInfo.eventId = provider.ConsumeIntergral<int32_t>() % CALL_SESSION_EVENT_ID_NUM;
    messageParcel.WriteRawData((const void *)&callSessionReportInfo, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnCallSessionEventChange(messageParcel, reply);
}

int32_t HandlePeerDimensionsChanged(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t length = sizeof(PeerDimensionsReportInfo);
    messageParcel.WriteInt32(length);
    PeerDimensionsReportInfo dimensionsReportInfo;
    dimensionsReportInfo.index = provider.ConsumeIntergral<int32_t>() % CALL_INDEX_MAX_NUM;
    dimensionsReportInfo.width = provider.ConsumeIntergral<int32_t>();
    dimensionsReportInfo.height = provider.ConsumeIntergral<int32_t>();
    messageParcel.WriteRawData((const void *)&dimensionsReportInfo, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnPeerDimensionsChange(messageParcel, reply);
}

int32_t HandleCallDataUsageChanged(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int64_t reportInfo = provider.ConsumeIntergral<int64_t>();
    messageParcel.WriteInt64(reportInfo);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnCallDataUsageChange(messageParcel, reply);
}

int32_t HandleCameraCapabilitiesChanged(FuzzedDataProvider &provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t length = sizeof(CameraCapabilitiesReportInfo);
    messageParcel.WriteInt32(length);
    CameraCapabilitiesReportInfo cameraCapabilitiesReportInfo;
    cameraCapabilitiesReportInfo.index = provider.ConsumeIntergral<int32_t>() % CALL_INDEX_MAX_NUM;
    cameraCapabilitiesReportInfo.width = provider.ConsumeIntergral<int32_t>();
    cameraCapabilitiesReportInfo.height = provider.ConsumeIntergral<int32_t>();
    messageParcel.WriteRawData((const void *)&cameraCapabilitiesReportInfo, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnCameraCapabilitiesChange(messageParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    FuzzedDataProvider provider(data, size);
    OnRemoteRequest(provider);
    UpdateCallReportInfo(provider);
    UpdateCallsReportInfo(provider);
    UpdateEventReport(provider);
    UpdateGetWaitingResult(provider);
    UpdateGetRestrictionResult(provider);
    UpdateGetTransferResult(provider);
    UpdateGetCallClipResult(provider);
    GetImsConfigResult(provider);
    GetImsFeatureValueResult(provider);
    SendMmiCodeResult(provider);
    ReceiveUpdateCallMediaModeRequest(provider);
    ReceiveUpdateCallMediaModeResponse(provider);
    HandleCallSessionEventChanged(provider);
    HandlePeerDimensionsChanged(provider);
    HandleCallDataUsageChanged(provider);
    HandleCameraCapabilitiesChanged(provider);
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
