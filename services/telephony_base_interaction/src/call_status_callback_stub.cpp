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
#include "call_manager_log.h"
#include "cellular_call_types.h"

namespace OHOS {
namespace TelephonyCallManager {
CallStatusCallbackStub::CallStatusCallbackStub()
{
    CALLMANAGER_DEBUG_LOG("CellularCallCallbackStub");
    memberFuncMap_[UPDATE_CALL_INFO] = &CallStatusCallbackStub::OnUpdateCallReportInfoRequest;
    memberFuncMap_[UPDATE_CALLS_INFO] = &CallStatusCallbackStub::OnUpdateCallsReportInfoRequest;
    memberFuncMap_[UPDATE_DISCONNECTED_CAUSE] = &CallStatusCallbackStub::OnUpdateDisconnectedCauseRequest;
}

CallStatusCallbackStub::~CallStatusCallbackStub()
{
    memberFuncMap_.clear();
}

int CallStatusCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescripter = CallStatusCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        CALLMANAGER_ERR_LOG("descriptor checked failed");
        return TELEPHONY_BAD_TYPE;
    }
    CALLMANAGER_DEBUG_LOG("OnReceived, cmd = %{public}u", code);

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t CallStatusCallbackStub::OnUpdateCallReportInfoRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_NO_ERROR;
    const CallReportInfo *parcelPtr = nullptr;
    int32_t len = data.ReadInt32();
    if (len <= TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("Invalid parameter, len = %{public}d", len);
        return TELEPHONY_FAIL;
    }
    if (!data.ContainFileDescriptors()) {
        CALLMANAGER_WARNING_LOG("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const CallReportInfo *>(data.ReadRawData(len))) == nullptr) {
        CALLMANAGER_ERR_LOG("reading raw data failed, length = %d", len);
        return TELEPHONY_FAIL;
    }

    result = OnUpdateCallReportInfo(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        CALLMANAGER_ERR_LOG("writing parcel failed");
        return TELEPHONY_FAIL;
    }
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusCallbackStub::OnUpdateCallsReportInfoRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_NO_ERROR;
    int32_t cnt = data.ReadInt32();
    if (cnt <= TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("invalid parameter, cnt = %d", cnt);
        return TELEPHONY_FAIL;
    }
    CallsReportInfo callReportInfo;
    int32_t len = TELEPHONY_NO_ERROR;
    const CallReportInfo *parcelPtr = nullptr;
    for (int i = TELEPHONY_NO_ERROR; i < cnt; i++) {
        len = data.ReadInt32();
        if (len <= TELEPHONY_NO_ERROR) {
            CALLMANAGER_ERR_LOG("invalid parameter, len = %d", len);
            return TELEPHONY_FAIL;
        }
        if (!data.ContainFileDescriptors()) {
            CALLMANAGER_WARNING_LOG("sent raw data is less than 32k");
        }

        if ((parcelPtr = reinterpret_cast<const CallReportInfo *>(data.ReadRawData(len))) == nullptr) {
            CALLMANAGER_ERR_LOG("reading raw data failed, length = %d", len);
            if (reply.WriteInt32(0)) {
                CALLMANAGER_ERR_LOG("writing parcel failed");
            }
            return TELEPHONY_FAIL;
        }

        callReportInfo.callVec.push_back(*parcelPtr);
        CALLMANAGER_DEBUG_LOG("accountId:%{public}d,state:%{public}d", parcelPtr->accountId, parcelPtr->state);
    }
    callReportInfo.slotId = data.ReadInt32();
    CALLMANAGER_DEBUG_LOG("slotId:%{public}d", callReportInfo.slotId);
    result = OnUpdateCallsReportInfo(callReportInfo);
    if (!reply.WriteInt32(result)) {
        CALLMANAGER_ERR_LOG("writing parcel failed");
        return TELEPHONY_FAIL;
    }
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusCallbackStub::OnUpdateDisconnectedCauseRequest(MessageParcel &data, MessageParcel &reply)
{
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS
