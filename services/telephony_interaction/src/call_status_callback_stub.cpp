/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "cellular_call_types.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallStatusCallbackStub::CallStatusCallbackStub()
{
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_CALL_INFO] =
        &CallStatusCallbackStub::OnUpdateCallReportInfo;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_CALLS_INFO] =
        &CallStatusCallbackStub::OnUpdateCallsReportInfo;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_DISCONNECTED_CAUSE] =
        &CallStatusCallbackStub::OnUpdateDisconnectedCause;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_EVENT_RESULT_INFO] =
        &CallStatusCallbackStub::OnUpdateEventReport;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_GET_WAITING] =
        &CallStatusCallbackStub::OnUpdateGetWaitingResult;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_SET_WAITING] =
        &CallStatusCallbackStub::OnUpdateSetWaitingResult;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_GET_RESTRICTION] =
        &CallStatusCallbackStub::OnUpdateGetRestrictionResult;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_SET_RESTRICTION] =
        &CallStatusCallbackStub::OnUpdateSetRestrictionResult;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_GET_TRANSFER] =
        &CallStatusCallbackStub::OnUpdateGetTransferResult;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_SET_TRANSFER] =
        &CallStatusCallbackStub::OnUpdateSetTransferResult;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_GET_CALL_CLIP] =
        &CallStatusCallbackStub::OnUpdateGetCallClipResult;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_GET_CALL_CLIR] =
        &CallStatusCallbackStub::OnUpdateGetCallClirResult;
    memberFuncMap_[(uint32_t)CallManagerCallStatusCode::UPDATE_SET_CALL_CLIR] =
        &CallStatusCallbackStub::OnUpdateSetCallClirResult;
}

CallStatusCallbackStub::~CallStatusCallbackStub()
{
    memberFuncMap_.clear();
}

int CallStatusCallbackStub::OnRemoteRequest(
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
    int32_t result = TELEPHONY_SUCCESS;
    const CallReportInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_FAIL;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const CallReportInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_FAIL;
    }

    result = UpdateCallReportInfo(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateCallsReportInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    int32_t cnt = data.ReadInt32();
    if (cnt <= 0) {
        TELEPHONY_LOGE("invalid parameter, cnt = %{public}d", cnt);
        return TELEPHONY_ERR_FAIL;
    }
    TELEPHONY_LOGI("call list size:%{public}d", cnt);
    CallsReportInfo callReportInfo;
    int32_t len = 0;
    const CallReportInfo *parcelPtr = nullptr;
    for (int i = 0; i < cnt; i++) {
        len = data.ReadInt32();
        if (len <= 0) {
            TELEPHONY_LOGE("invalid parameter, len = %d", len);
            return TELEPHONY_ERR_FAIL;
        }
        if (!data.ContainFileDescriptors()) {
            TELEPHONY_LOGW("sent raw data is less than 32k");
        }

        if ((parcelPtr = reinterpret_cast<const CallReportInfo *>(data.ReadRawData(len))) == nullptr) {
            TELEPHONY_LOGE("reading raw data failed, length = %d", len);
            if (reply.WriteInt32(0)) {
                TELEPHONY_LOGE("writing parcel failed");
            }
            return TELEPHONY_ERR_FAIL;
        }

        callReportInfo.callVec.push_back(*parcelPtr);
        TELEPHONY_LOGI("accountId:%{public}d,state:%{public}d", parcelPtr->accountId, parcelPtr->state);
    }
    callReportInfo.slotId = data.ReadInt32();
    result = UpdateCallsReportInfo(callReportInfo);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateDisconnectedCause(MessageParcel &data, MessageParcel &reply)
{
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateEventReport(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    const CellularCallEventInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_FAIL;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const CellularCallEventInfo *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_FAIL;
    }

    result = UpdateEventResultInfo(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateGetWaitingResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    const CallWaitResponse *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_FAIL;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const CallWaitResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_FAIL;
    }

    result = UpdateGetWaitingResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateSetWaitingResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t callWaitResult = data.ReadInt32();
    result = UpdateSetWaitingResult(callWaitResult);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateGetRestrictionResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    const CallRestrictionResponse *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_FAIL;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const CallRestrictionResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_FAIL;
    }
    result = UpdateGetRestrictionResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateSetRestrictionResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_SUCCESS;
    int32_t result = TELEPHONY_SUCCESS;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = UpdateSetRestrictionResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateGetTransferResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    const CallTransferResponse *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_FAIL;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const CallTransferResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_FAIL;
    }
    result = UpdateGetTransferResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateSetTransferResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_SUCCESS;
    int32_t result = TELEPHONY_SUCCESS;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = UpdateSetTransferResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateGetCallClipResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    const ClipResponse *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_FAIL;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const ClipResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_FAIL;
    }
    result = UpdateGetCallClipResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateGetCallClirResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_SUCCESS;
    const ClirResponse *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_ERR_FAIL;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const ClirResponse *>(data.ReadRawData(len))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", len);
        return TELEPHONY_ERR_FAIL;
    }
    result = UpdateGetCallClirResult(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallbackStub::OnUpdateSetCallClirResult(MessageParcel &data, MessageParcel &reply)
{
    int32_t error = TELEPHONY_SUCCESS;
    int32_t result = TELEPHONY_SUCCESS;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    result = data.ReadInt32();
    error = UpdateSetCallClirResult(result);
    if (!reply.WriteInt32(error)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
