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

#include "report_call_info_handler.h"

#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "ffrt.h"
#include "ims_call.h"
#include "telephony_log_wrapper.h"
#include "thread"

namespace OHOS {
namespace Telephony {
namespace {
ffrt::queue reportCallInfoQueue { "report_call_info_queue" };
}
ReportCallInfoHandler::ReportCallInfoHandler() {}

ReportCallInfoHandler::~ReportCallInfoHandler() {}

void ReportCallInfoHandler::Init()
{
    callStatusManagerPtr_ = std::make_shared<CallStatusManager>();
    callStatusManagerPtr_->Init();
    return;
}

int32_t ReportCallInfoHandler::UpdateCallReportInfo(const CallDetailInfo &info)
{
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    CallDetailInfo callDetailInfo = info;
    std::weak_ptr<CallStatusManager> callStatusManagerPtr = callStatusManagerPtr_;
    TELEPHONY_LOGW("UpdateCallReportInfo submit task enter");
    reportCallInfoQueue.submit([callStatusManagerPtr, callDetailInfo]() {
        std::shared_ptr<CallStatusManager> managerPtr = callStatusManagerPtr.lock();
        if (managerPtr == nullptr) {
            TELEPHONY_LOGE("managerPtr is null");
            return;
        }
        auto ret = managerPtr->HandleCallReportInfo(callDetailInfo);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("HandleCallReportInfo failed! ret:%{public}d", ret);
        }
    });
    return TELEPHONY_SUCCESS;
}

void ReportCallInfoHandler::BuildCallDetailsInfo(CallDetailsInfo &info, CallDetailsInfo &callDetailsInfo)
{
    CallDetailInfo callDetailInfo;
    std::vector<CallDetailInfo>::iterator iter = info.callVec.begin();
    for (; iter != info.callVec.end(); ++iter) {
        callDetailInfo.callType = (*iter).callType;
        callDetailInfo.accountId = (*iter).accountId;
        callDetailInfo.state = (*iter).state;
        callDetailInfo.callMode = (*iter).callMode;
        callDetailInfo.index = (*iter).index;
        callDetailInfo.voiceDomain = (*iter).voiceDomain;
        callDetailInfo.mpty = (*iter).mpty;
        callDetailInfo.crsType = (*iter).crsType;
        callDetailInfo.originalCallType = (*iter).originalCallType;
        (void)memcpy_s(callDetailInfo.phoneNum, kMaxNumberLen, (*iter).phoneNum, kMaxNumberLen);
        (void)memcpy_s(callDetailInfo.bundleName, kMaxBundleNameLen, (*iter).bundleName, kMaxBundleNameLen);
        callDetailInfo.name = (*iter).name;
        callDetailInfo.namePresentation = (*iter).namePresentation;
        callDetailsInfo.callVec.push_back(callDetailInfo);
    }
}

int32_t ReportCallInfoHandler::UpdateCallsReportInfo(CallDetailsInfo &info)
{
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    CallDetailsInfo callDetailsInfo;
    callDetailsInfo.slotId = info.slotId;
    (void)memcpy_s(callDetailsInfo.bundleName, kMaxBundleNameLen, info.bundleName, kMaxBundleNameLen);
    BuildCallDetailsInfo(info, callDetailsInfo);
    std::weak_ptr<CallStatusManager> callStatusManagerPtr = callStatusManagerPtr_;
    TELEPHONY_LOGW("UpdateCallsReportInfo submit task enter");
    reportCallInfoQueue.submit([callStatusManagerPtr, callDetailsInfo]() {
        std::shared_ptr<CallStatusManager> managerPtr = callStatusManagerPtr.lock();
        if (managerPtr == nullptr) {
            TELEPHONY_LOGE("managerPtr is null");
            return;
        }
        int32_t ret = managerPtr->HandleCallsReportInfo(callDetailsInfo);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("HandleCallsReportInfo failed! ret:%{public}d", ret);
        }
    });

    CallDetailInfo detailInfo;
    detailInfo.state = TelCallState::CALL_STATUS_UNKNOWN;
    std::vector<CallDetailInfo>::iterator it = info.callVec.begin();
    for (; it != info.callVec.end(); ++it) {
        detailInfo.callType = (*it).callType;
        detailInfo.accountId = (*it).accountId;
        detailInfo.state = (*it).state;
        detailInfo.callMode = (*it).callMode;
    }
    if (detailInfo.state == TelCallState::CALL_STATUS_INCOMING) {
        CallManagerHisysevent::WriteIncomingCallFaultEvent(info.slotId, static_cast<int32_t>(detailInfo.callType),
            static_cast<int32_t>(detailInfo.callMode), CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE,
            "ID HANDLER_UPDATE_CALL_INFO_LIST");
    }
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandler::UpdateDisconnectedCause(const DisconnectedDetails &details)
{
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    DisconnectedDetails disconnectedDetails = details;
    if (static_cast<RilDisconnectedReason>(details.reason) == RilDisconnectedReason::DISCONNECTED_REASON_NORMAL) {
        disconnectedDetails.reason = DisconnectedReason::NORMAL_CALL_CLEARING;
    }
    std::weak_ptr<CallStatusManager> callStatusManagerPtr = callStatusManagerPtr_;
    TELEPHONY_LOGI("UpdateDisconnectedCause submit task enter");
    reportCallInfoQueue.submit([callStatusManagerPtr, disconnectedDetails]() {
        std::shared_ptr<CallStatusManager> managerPtr = callStatusManagerPtr.lock();
        if (managerPtr == nullptr) {
            TELEPHONY_LOGE("managerPtr is null");
            return;
        }
        int32_t ret = managerPtr->HandleDisconnectedCause(disconnectedDetails);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("HandleDisconnectedCause failed! ret:%{public}d", ret);
        }
    });

    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandler::UpdateEventResultInfo(const CellularCallEventInfo &info)
{
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::weak_ptr<CallStatusManager> callStatusManagerPtr = callStatusManagerPtr_;
    TELEPHONY_LOGI("UpdateEventResultInfo submit task enter");
    reportCallInfoQueue.submit([callStatusManagerPtr, info]() {
        std::shared_ptr<CallStatusManager> managerPtr = callStatusManagerPtr.lock();
        if (managerPtr == nullptr) {
            TELEPHONY_LOGE("managerPtr is null");
            return;
        }
        int32_t ret = managerPtr->HandleEventResultReportInfo(info);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("HandleEventResultReportInfo failed! ret:%{public}d", ret);
        }
    });

    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandler::UpdateOttEventInfo(const OttCallEventInfo &info)
{
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    OttCallEventInfo ottCallEventInfo = info;
    std::weak_ptr<CallStatusManager> callStatusManagerPtr = callStatusManagerPtr_;
    TELEPHONY_LOGI("UpdateOttEventInfo submit task enter");
    reportCallInfoQueue.submit([callStatusManagerPtr, ottCallEventInfo]() {
        std::shared_ptr<CallStatusManager> managerPtr = callStatusManagerPtr.lock();
        if (managerPtr == nullptr) {
            TELEPHONY_LOGE("managerPtr is null");
            return;
        }
        int32_t ret = managerPtr->HandleOttEventReportInfo(ottCallEventInfo);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("HandleOttEventReportInfo failed! ret:%{public}d", ret);
        }
    });

    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandler::ReceiveImsCallModeRequest(const CallModeReportInfo &response)
{
    TELEPHONY_LOGI("ReceiveImsCallModeRequest submit task enter");
    reportCallInfoQueue.submit([response]() {
        CallModeReportInfo reportInfo = response;
        sptr<CallBase> call = CallObjectManager::GetOneCallObjectByIndex(response.callIndex);
        if (call == nullptr) {
            TELEPHONY_LOGE("call not exists");
            return;
        }
        if (call->GetCallType() == CallType::TYPE_IMS) {
            sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(call.GetRefPtr());
            imsCall->RecieveUpdateCallMediaModeRequest(reportInfo);
        }
    });
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandler::ReceiveImsCallModeResponse(const CallModeReportInfo &response)
{
    TELEPHONY_LOGI("ReceiveImsCallModeResponse submit task enter");
    reportCallInfoQueue.submit([response]() {
        CallModeReportInfo reportInfo = response;
        sptr<CallBase> call = nullptr;
        if (response.slotId != -1) {
            call = CallObjectManager::GetOneCallObjectByIndexAndSlotId(response.callIndex, response.slotId);
        }
        if (call == nullptr) {
            call = CallObjectManager::GetOneCallObjectByIndex(response.callIndex);
        }
        if (call == nullptr) {
            TELEPHONY_LOGE("call not exists");
            return;
        }
        if (call->GetCallType() == CallType::TYPE_IMS) {
            sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(call.GetRefPtr());
            imsCall->ReceiveUpdateCallMediaModeResponse(reportInfo);
        }
    });
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandler::UpdateVoipEventInfo(const VoipCallEventInfo &info)
{
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    VoipCallEventInfo voipCallEventInfo = info;
    std::weak_ptr<CallStatusManager> callStatusManagerPtr = callStatusManagerPtr_;
    TELEPHONY_LOGI("UpdateVoipEventInfo submit task enter");
    reportCallInfoQueue.submit([callStatusManagerPtr, voipCallEventInfo]() {
        std::shared_ptr<CallStatusManager> managerPtr = callStatusManagerPtr.lock();
        if (managerPtr == nullptr) {
            TELEPHONY_LOGE("managerPtr is null");
            return;
        }
        int32_t ret = managerPtr->HandleVoipEventReportInfo(voipCallEventInfo);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("UpdateVoipEventInfo failed! ret:%{public}d", ret);
        }
    });
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
