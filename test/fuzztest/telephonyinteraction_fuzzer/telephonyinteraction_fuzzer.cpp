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
#include "call_manager_service.h"
#include "call_status_callback_proxy.h"
#include "system_ability_definition.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t ACCOUNT_ID_NUM = 10;
constexpr int32_t MULTI_PARTY_NUM = 10;
constexpr int32_t VOICE_DOMAIN_NUM = 10;
std::unique_ptr<CallStatusCallback> CallStatusCallbackPtr_ = nullptr;

bool IsServiceInited()
{
    if (!g_isInited) {
        DelayedSingleton<CallManagerService>::GetInstance()->OnStart();
        if (DelayedSingleton<CallManagerService>::GetInstance()->GetServiceRunningState() ==
            static_cast<int32_t>(CallManagerService::ServiceRunningState::STATE_RUNNING)) {
            g_isInited = true;
        }
    }
    CallStatusCallbackPtr_ = std::make_unique<CallStatusCallback>();
    if (CallStatusCallbackPtr_ == nullptr) {
        g_isInited = false;
    }
    return g_isInited;
}

int32_t OnRemoteRequest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataMessageParcel;
    if (!dataMessageParcel.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataMessageParcel.RewindRead(0);
    uint32_t code = static_cast<uint32_t>(size);
    MessageParcel reply;
    MessageOption option;
    return CallStatusCallbackPtr_->OnRemoteRequest(code, dataMessageParcel, reply, option);
}

int32_t UpdateCallReportInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallReportInfo info;
    int32_t length = sizeof(CallReportInfo);
    info.index = static_cast<int32_t>(size);
    info.accountId = static_cast<int32_t>(size % ACCOUNT_ID_NUM);
    info.voiceDomain = static_cast<int32_t>(size % VOICE_DOMAIN_NUM);
    info.mpty = static_cast<int32_t>(size % MULTI_PARTY_NUM);
    info.callType = CallType::TYPE_ERR_CALL;
    info.callMode = VideoStateType::TYPE_VOICE;
    info.state = TelCallState::CALL_STATUS_UNKNOWN;
    memcpy_s(info.accountNum, kMaxNumberLen, reinterpret_cast<const char *>(data),
        strlen(reinterpret_cast<const char *>(data)));
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateCallReportInfo(dataParcel, reply);
}

int32_t UpdateCallsReportInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallReportInfo info;
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    int32_t length = sizeof(CallReportInfo);
    int32_t vecSize = static_cast<int32_t>(size);
    info.index = static_cast<int32_t>(size);
    info.accountId = static_cast<int32_t>(size % ACCOUNT_ID_NUM);
    info.voiceDomain = static_cast<int32_t>(size % VOICE_DOMAIN_NUM);
    info.mpty = static_cast<int32_t>(size % MULTI_PARTY_NUM);
    info.callType = CallType::TYPE_ERR_CALL;
    info.callMode = VideoStateType::TYPE_VOICE;
    info.state = TelCallState::CALL_STATUS_UNKNOWN;
    memcpy_s(info.accountNum, kMaxNumberLen, reinterpret_cast<const char *>(data),
        strlen(reinterpret_cast<const char *>(data)));
    dataParcel.WriteInt32(vecSize);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    dataParcel.WriteInt32(slotId);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateCallsReportInfo(dataParcel, reply);
}

int32_t UpdateEventReport(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
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
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallWaitResponse callWaitResponse;
    int32_t length = sizeof(CallWaitResponse);
    dataParcel.WriteInt32(length);
    callWaitResponse.result = static_cast<int32_t>(size);
    callWaitResponse.status = static_cast<int32_t>(size);
    callWaitResponse.classCw = static_cast<int32_t>(size);
    dataParcel.WriteRawData((const void *)&callWaitResponse, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetWaitingResult(dataParcel, reply);
}

int32_t UpdateGetRestrictionResult(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallRestrictionResponse callRestrictionResult;
    int32_t length = sizeof(CallRestrictionResponse);
    dataParcel.WriteInt32(length);
    callRestrictionResult.result = static_cast<int32_t>(size);
    callRestrictionResult.status = static_cast<int32_t>(size);
    callRestrictionResult.classCw = static_cast<int32_t>(size);
    dataParcel.WriteRawData((const void *)&callRestrictionResult, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetRestrictionResult(dataParcel, reply);
}

int32_t UpdateGetTransferResult(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
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
    memcpy_s(callTransferResponse.number, kMaxNumberLen, reinterpret_cast<const char *>(data),
        strlen(reinterpret_cast<const char *>(data)));
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
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    ClipResponse clipResponse;
    int32_t length = sizeof(ClipResponse);
    dataParcel.WriteInt32(length);
    clipResponse.result = static_cast<int32_t>(size);
    clipResponse.action = static_cast<int32_t>(size);
    clipResponse.clipStat = static_cast<int32_t>(size);
    dataParcel.WriteRawData((const void *)&clipResponse, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnUpdateGetCallClipResult(dataParcel, reply);
}

int32_t GetImsConfigResult(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    GetImsConfigResponse response;
    int32_t length = sizeof(GetImsConfigResponse);
    dataParcel.WriteInt32(length);
    response.result = static_cast<int32_t>(size);
    response.value = static_cast<int32_t>(size);
    dataParcel.WriteRawData((const void *)&response, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnGetImsConfigResult(dataParcel, reply);
}

int32_t GetImsFeatureValueResult(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    GetImsFeatureValueResponse response;
    int32_t length = sizeof(GetImsFeatureValueResponse);
    dataParcel.WriteInt32(length);
    response.result = static_cast<int32_t>(size);
    response.value = static_cast<int32_t>(size);
    dataParcel.WriteRawData((const void *)&response, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnGetImsFeatureValueResult(dataParcel, reply);
}

int32_t ReceiveUpdateMediaModeResponse(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallStatusCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    CallMediaModeResponse response;
    int32_t length = sizeof(CallMediaModeResponse);
    dataParcel.WriteInt32(length);
    response.result = static_cast<int32_t>(size);
    memcpy_s(response.phoneNum, kMaxNumberLen, reinterpret_cast<const char *>(data),
        strlen(reinterpret_cast<const char *>(data)));
    dataParcel.WriteRawData((const void *)&response, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnReceiveUpdateMediaModeResponse(dataParcel, reply);
}

int32_t SendMmiCodeResult(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
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
    memcpy_s(info.message, kMaxNumberLen, reinterpret_cast<const char *>(data),
        strlen(reinterpret_cast<const char *>(data)));
    dataParcel.WriteRawData((const void *)&info, length);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return CallStatusCallbackPtr_->OnSendMmiCodeResult(dataParcel, reply);
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
    ReceiveUpdateMediaModeResponse(data, size);
    SendMmiCodeResult(data, size);
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
