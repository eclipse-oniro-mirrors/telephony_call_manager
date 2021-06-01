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

#include "call_manager_service_stub.h"

#include "call_manager_errors.h"
#include "call_manager_log.h"

#include "message_option.h"
#include "message_parcel.h"

#include "call_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
CallManagerServiceStub::CallManagerServiceStub()
{
    memberFuncMap_[INTERFACE_DIAL_CALL] = &CallManagerServiceStub::DialCallRequest;
    memberFuncMap_[INTERFACE_ANSWER_CALL] = &CallManagerServiceStub::AcceptCallRequest;
    memberFuncMap_[INTERFACE_REJECT_CALL] = &CallManagerServiceStub::RejectCallRequest;
    memberFuncMap_[INTERFACE_DISCONNECT_CALL] = &CallManagerServiceStub::HangUpCallRequest;
    memberFuncMap_[INTERFACE_GET_CALL_STATE] = &CallManagerServiceStub::GetCallStateRequest;
}

CallManagerServiceStub::~CallManagerServiceStub()
{
    memberFuncMap_.clear();
}

int CallManagerServiceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescripter = CallManagerServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        CALLMANAGER_ERR_LOG("descriptor checked fail !");
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

int32_t CallManagerServiceStub::DialCallRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_FAIL;
    int32_t callId = TELEPHONY_NO_ERROR;
    std::u16string callNumber = data.ReadString16();
    int32_t accountId = data.ReadInt32();
    int32_t vedioState = data.ReadInt32();
    int32_t dialScene = data.ReadInt32();
    AppExecFwk::PacMap dialInfo;
    dialInfo.PutIntValue("accountId", accountId);
    dialInfo.PutIntValue("vedioState", vedioState);
    dialInfo.PutIntValue("dialScene", dialScene);
    if (!data.ContainFileDescriptors()) {
        CALLMANAGER_WARNING_LOG("sent raw data is less than 32k");
    }

    CALLMANAGER_DEBUG_LOG("accountId:%{public}d,videoState:%{public}d,dialScene:%{public}d",
        dialInfo.GetIntValue("accountId"), dialInfo.GetIntValue("videoState"), dialInfo.GetIntValue("dialScene"));
    result = DialCall(callNumber, dialInfo, callId);
    CALLMANAGER_DEBUG_LOG("result:%{public}d", result);
    if (!reply.WriteInt32(result) || !reply.WriteInt32(callId)) {
        CALLMANAGER_ERR_LOG("fail to write parcel");
        return TELEPHONY_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::AcceptCallRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t videoState = data.ReadInt32();
    int32_t result = AcceptCall(callId, videoState);
    CALLMANAGER_DEBUG_LOG("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        CALLMANAGER_ERR_LOG("fail to write parcel");
        return TELEPHONY_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::RejectCallRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    bool isSendSms = data.ReadBool();
    std::u16string content = data.ReadString16();
    int32_t result = RejectCall(callId, isSendSms, content);
    CALLMANAGER_DEBUG_LOG("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        CALLMANAGER_ERR_LOG("fail to write parcel");
        return TELEPHONY_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::HangUpCallRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t result = HangUpCall(callId);
    CALLMANAGER_DEBUG_LOG("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        CALLMANAGER_ERR_LOG("fail to write parcel");
        return TELEPHONY_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::GetCallStateRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetCallState();
    CALLMANAGER_DEBUG_LOG("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        CALLMANAGER_ERR_LOG("fail to write parcel");
        return TELEPHONY_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS