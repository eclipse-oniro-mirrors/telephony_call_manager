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

#include "cellular_call_stub_test.h"

#include <securec.h>

#include "call_manager_errors.h"
#include "call_manager_log.h"
#include "cellular_call_types.h"

namespace OHOS {
namespace TelephonyCallManager {
CellularCallStubTest::CellularCallStubTest()
    : memberFuncMap_({
        {DIAL, &CellularCallStubTest::DialRequest}, {END, &CellularCallStubTest::EndRequest},
        {REJECT, &CellularCallStubTest::RejectRequest}, {ANSWER, &CellularCallStubTest::AnswerRequest},
        {REGISTER_CALLBACK, &CellularCallStubTest::RegisterCallBackRequest}
    }), callback_(nullptr), callInfoVec_(0)
{
    CALLMANAGER_DEBUG_LOG("CellularCallStub");
}

CellularCallStubTest::~CellularCallStubTest()
{
    if (callback_) {
        callback_.clear();
        callback_ = nullptr;
    }
    memberFuncMap_.clear();
    callInfoVec_.clear();
}

int CellularCallStubTest::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescripter = CellularCallStubTest::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        CALLMANAGER_ERR_LOG("descriptor checked fail");
        return TELEPHONY_FAIL;
    }
    CALLMANAGER_DEBUG_LOG("OnReceived, cmd = %{public}d", code);

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int CellularCallStubTest::Dial(const CellularCall::CellularCallInfo &callInfo)
{
    return TELEPHONY_NO_ERROR;
}

int CellularCallStubTest::End(const CellularCall::CellularCallInfo &callInfo)
{
    return TELEPHONY_NO_ERROR;
}

int CellularCallStubTest::Reject(const CellularCall::CellularCallInfo &callInfo)
{
    return TELEPHONY_NO_ERROR;
}

int CellularCallStubTest::Answer(const CellularCall::CellularCallInfo &callInfo)
{
    return TELEPHONY_NO_ERROR;
}

int CellularCallStubTest::IsUrgentCall(const std::string &phoneNum, const int32_t slotId)
{
    return TELEPHONY_NO_ERROR;
}

int CellularCallStubTest::RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback)
{
    return TELEPHONY_NO_ERROR;
}

int CellularCallStubTest::UnRegisterCallManagerCallBack()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CellularCallStubTest::DialRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_FAIL;
    CellularCall::CellularCallInfo *callInfo =
        (CellularCall::CellularCallInfo *)data.ReadRawData(sizeof(CellularCall::CellularCallInfo));
    if (callInfo == nullptr) {
        CALLMANAGER_ERR_LOG("DIAL. fail to write to parcel.\n");
        return TELEPHONY_FAIL;
    }
    callInfoVec_.push_back(*callInfo);
    CALLMANAGER_DEBUG_LOG(
        "callId:%{public}d,phoneNetType:%{public}d,callType:%{public}d,"
        "videoState:%{public}d",
        callInfo->callId, callInfo->phoneNetType, callInfo->callType, callInfo->videoState);
    CALLMANAGER_DEBUG_LOG("DIAL result=%{public}d\n", result);
    reply.WriteInt32(result);
    return result;
}

int32_t CellularCallStubTest::AnswerRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_FAIL;
    bool flag = false;
    CellularCall::CellularCallInfo *callInfo =
        (CellularCall::CellularCallInfo *)data.ReadRawData(sizeof(CellularCall::CellularCallInfo));
    if (callInfo == nullptr) {
        CALLMANAGER_ERR_LOG("ANSWER. fail to write to parcel.");
        return TELEPHONY_FAIL;
    }
    CALLMANAGER_DEBUG_LOG(
        "phoneNum:%s,callId:%{public}d,phoneNetType:%{public}d,callType:%{public}d,"
        "videoState:%{public}d",
        callInfo->phoneNum, callInfo->callId, callInfo->phoneNetType, callInfo->callType, callInfo->videoState);
    CALLMANAGER_DEBUG_LOG("ANSWER result=%{public}d\n", result);
    reply.WriteInt32(result);
    for (auto &it : callInfoVec_) {
        if (it.callId == callInfo->callId) {
            flag = true;
            break;
        }
    }
    if (flag) {
        ReportDialingState(callInfo->callId);
        ReportAlertingState(callInfo->callId);
        ReportActiveState(callInfo->callId);
        ReportHoldingState(callInfo->callId);
    } else {
        CALLMANAGER_DEBUG_LOG("this call does not exist! callId:%{public}d", callInfo->callId);
    }
    return result;
}

int32_t CellularCallStubTest::RejectRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_FAIL;
    CellularCall::CellularCallInfo *callInfo =
        (CellularCall::CellularCallInfo *)data.ReadRawData(sizeof(CellularCall::CellularCallInfo));
    if (callInfo == nullptr) {
        CALLMANAGER_ERR_LOG("REJECT. fail to write to parcel.");
        return TELEPHONY_FAIL;
    }
    CALLMANAGER_DEBUG_LOG(
        "phoneNum:%s,callId:%{public}d,phoneNetType:%{public}d,callType:%{public}d,"
        "videoState:%{public}d",
        callInfo->phoneNum, callInfo->callId, callInfo->phoneNetType, callInfo->callType, callInfo->videoState);
    CALLMANAGER_DEBUG_LOG("REJECT result=%{public}d\n", result);
    reply.WriteInt32(result);
    return result;
}

int32_t CellularCallStubTest::EndRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_FAIL;
    CellularCall::CellularCallInfo *callInfo =
        (CellularCall::CellularCallInfo *)data.ReadRawData(sizeof(CellularCall::CellularCallInfo));
    if (callInfo == nullptr) {
        CALLMANAGER_ERR_LOG("fail to write to parcel.");
        return TELEPHONY_FAIL;
    }
    ReportDisconnectingState(callInfo->callId);
    ReportDisconnectedState(callInfo->callId);

    std::vector<CellularCall::CellularCallInfo>::iterator it = callInfoVec_.begin();
    for (; it != callInfoVec_.end(); it++) {
        if ((*it).callId == callInfo->callId) {
            callInfoVec_.erase(it);
            break;
        }
    }
    CALLMANAGER_DEBUG_LOG(
        "phoneNum:%s,callId:%{public}d,phoneNetType:%{public}d,callType:%{public}d,"
        "videoState:%{public}d",
        callInfo->phoneNum, callInfo->callId, callInfo->phoneNetType, callInfo->callType, callInfo->videoState);
    CALLMANAGER_DEBUG_LOG("END result=%{public}d", result);
    reply.WriteInt32(result);

    ReportIncomingState();
    ReportWaitingState();
    return result;
}

int32_t CellularCallStubTest::RegisterCallBackRequest(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_FAIL;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        CALLMANAGER_ERR_LOG("REGISTER_CALLBACK, remote is nullptr.\n");
        result = -1;
        reply.WriteInt32(result);
        return TELEPHONY_FAIL;
    }
    callback_ = iface_cast<ICallStatusCallback>(remote);
    if (callback_ == nullptr) {
        CALLMANAGER_ERR_LOG("callback is nullptr.\n");
        result = -1;
        reply.WriteInt32(result);
        return TELEPHONY_FAIL;
    }

    CALLMANAGER_DEBUG_LOG("REGISTER_CALLBACK result=%{public}d.\n", result);
    reply.WriteInt32(result);
    return result;
}

int32_t CellularCallStubTest::ReportDialingState(int32_t callId) const
{
    CallReportInfo cellularCallInfo;
    for (auto &it : callInfoVec_) {
        if (it.callId == callId) {
            if (memcpy_s(cellularCallInfo.accountNum, kMaxNumberLen, it.phoneNum, kMaxNumberLen) != 0) {
                return TELEPHONY_MEMCPY_FAIL;
            }
            cellularCallInfo.callType = it.callType;
            cellularCallInfo.callMode = (VedioStateType)it.videoState;
            break;
        }
    }
    cellularCallInfo.state = TelCallStates::CALL_STATUS_DIALING;
    if (callback_ != nullptr) {
        callback_->OnUpdateCallReportInfo(cellularCallInfo);
    }
    CALLMANAGER_DEBUG_LOG("test CALL_STATUS_DIALING state update");
    return TELEPHONY_NO_ERROR;
}

int32_t CellularCallStubTest::ReportAlertingState(int32_t callId) const
{
    CallReportInfo cellularCallInfo;
    for (auto &it : callInfoVec_) {
        if (it.callId == callId) {
            if (memcpy_s(cellularCallInfo.accountNum, kMaxNumberLen, it.phoneNum, kMaxNumberLen) != 0) {
                return TELEPHONY_MEMCPY_FAIL;
            }
            cellularCallInfo.callType = it.callType;
            cellularCallInfo.callMode = (VedioStateType)it.videoState;
            break;
        }
    }
    cellularCallInfo.state = TelCallStates::CALL_STATUS_ALERTING;
    if (callback_ != nullptr) {
        callback_->OnUpdateCallReportInfo(cellularCallInfo);
    }
    CALLMANAGER_DEBUG_LOG("test CALL_STATUS_ALERTING state update");
    return TELEPHONY_NO_ERROR;
}

int32_t CellularCallStubTest::ReportActiveState(int32_t callId) const
{
    CallReportInfo cellularCallInfo;
    for (auto &it : callInfoVec_) {
        if (it.callId == callId) {
            if (memcpy_s(cellularCallInfo.accountNum, kMaxNumberLen, it.phoneNum, kMaxNumberLen) != 0) {
                return TELEPHONY_MEMCPY_FAIL;
            }
            cellularCallInfo.callType = it.callType;
            cellularCallInfo.callMode = (VedioStateType)it.videoState;
            break;
        }
    }
    cellularCallInfo.state = TelCallStates::CALL_STATUS_ACTIVE;
    if (callback_ != nullptr) {
        callback_->OnUpdateCallReportInfo(cellularCallInfo);
    }
    CALLMANAGER_DEBUG_LOG("test CALL_STATUS_ACTIVE state update");
    return TELEPHONY_NO_ERROR;
}

int32_t CellularCallStubTest::ReportHoldingState(int32_t callId) const
{
    CallReportInfo cellularCallInfo;
    for (auto &it : callInfoVec_) {
        if (it.callId == callId) {
            if (memcpy_s(cellularCallInfo.accountNum, kMaxNumberLen, it.phoneNum, kMaxNumberLen) != 0) {
                return TELEPHONY_MEMCPY_FAIL;
            }
            cellularCallInfo.callType = it.callType;
            cellularCallInfo.callMode = (VedioStateType)it.videoState;
            break;
        }
    }
    cellularCallInfo.state = TelCallStates::CALL_STATUS_HOLDING;
    if (callback_ != nullptr) {
        callback_->OnUpdateCallReportInfo(cellularCallInfo);
    }
    CALLMANAGER_DEBUG_LOG("test CALL_STATUS_HOLDING state update");
    return TELEPHONY_NO_ERROR;
}

int32_t CellularCallStubTest::ReportIncomingState() const
{
    char phoneNum[] = "11425453223456685";
    CallReportInfo cellularCallInfo;
    if (memcpy_s(cellularCallInfo.accountNum, kMaxNumberLen, phoneNum, sizeof(phoneNum)) != 0) {
        return TELEPHONY_MEMCPY_FAIL;
    }
    cellularCallInfo.callMode = VedioStateType::TYPE_VOICE;
    cellularCallInfo.callType = TYPE_CS;
    cellularCallInfo.state = TelCallStates::CALL_STATUS_INCOMING;
    if (callback_ != nullptr) {
        callback_->OnUpdateCallReportInfo(cellularCallInfo);
    }
    CALLMANAGER_DEBUG_LOG("test CALL_STATUS_INCOMING state");
    return TELEPHONY_NO_ERROR;
}

int32_t CellularCallStubTest::ReportWaitingState() const
{
    char phoneNum[] = "11105651630265564";
    CallReportInfo cellularCallInfo;
    if (memcpy_s(cellularCallInfo.accountNum, kMaxNumberLen, phoneNum, sizeof(phoneNum)) != 0) {
        return TELEPHONY_MEMCPY_FAIL;
    }
    cellularCallInfo.callMode = VedioStateType::TYPE_VOICE;
    cellularCallInfo.callType = TYPE_CS;
    cellularCallInfo.state = TelCallStates::CALL_STATUS_WAITING;
    if (callback_ != nullptr) {
        callback_->OnUpdateCallReportInfo(cellularCallInfo);
    }
    CALLMANAGER_DEBUG_LOG("test CALL_STATUS_WAITING state");
    return TELEPHONY_NO_ERROR;
}

int32_t CellularCallStubTest::ReportDisconnectingState(int32_t callId) const
{
    CallReportInfo cellularCallInfo;
    for (auto &it : callInfoVec_) {
        if (it.callId == callId) {
            if (memcpy_s(cellularCallInfo.accountNum, kMaxNumberLen, it.phoneNum, kMaxNumberLen) != 0) {
                return TELEPHONY_MEMCPY_FAIL;
            }
            cellularCallInfo.callType = it.callType;
            cellularCallInfo.callMode = (VedioStateType)it.videoState;
            break;
        }
    }
    cellularCallInfo.state = TelCallStates::CALL_STATUS_DISCONNECTING;
    if (callback_ != nullptr) {
        callback_->OnUpdateCallReportInfo(cellularCallInfo);
    }
    CALLMANAGER_DEBUG_LOG("test CALL_STATUS_DISCONNECTING state update");
    return TELEPHONY_NO_ERROR;
}

int32_t CellularCallStubTest::ReportDisconnectedState(int32_t callId) const
{
    CallReportInfo cellularCallInfo;
    for (auto &it : callInfoVec_) {
        if (it.callId == callId) {
            if (memcpy_s(cellularCallInfo.accountNum, kMaxNumberLen, it.phoneNum, kMaxNumberLen) != 0) {
                return TELEPHONY_MEMCPY_FAIL;
            }
            cellularCallInfo.callType = it.callType;
            cellularCallInfo.callMode = (VedioStateType)it.videoState;
            break;
        }
    }
    cellularCallInfo.state = TelCallStates::CALL_STATUS_DISCONNECTED;
    if (callback_ != nullptr) {
        callback_->OnUpdateCallReportInfo(cellularCallInfo);
    }
    CALLMANAGER_DEBUG_LOG("test CALL_STATUS_DISCONNECTED state update");
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS
