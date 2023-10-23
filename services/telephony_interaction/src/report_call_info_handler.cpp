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

#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "ffrt.h"
#include "ims_call.h"
#include "report_call_info_handler.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

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
    auto ret = callStatusManagerPtr_->HandleCallReportInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HandleCallReportInfo failed! ret:%{public}d", ret);
    }
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandler::UpdateCallsReportInfo(CallDetailsInfo &info)
{
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = callStatusManagerPtr_->HandleCallsReportInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HandleCallsReportInfo failed! ret:%{public}d", ret);
    }

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
    int32_t ret = callStatusManagerPtr_->HandleDisconnectedCause(details);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HandleDisconnectedCause failed! ret:%{public}d", ret);
    }
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandler::UpdateEventResultInfo(const CellularCallEventInfo &info)
{
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = callStatusManagerPtr_->HandleEventResultReportInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HandleEventResultReportInfo failed! ret:%{public}d", ret);
    }
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandler::UpdateOttEventInfo(const OttCallEventInfo &info)
{
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = callStatusManagerPtr_->HandleOttEventReportInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HandleOttEventReportInfo failed! ret:%{public}d", ret);
    }
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandler::UpdateMediaModeResponse(const CallMediaModeResponse &response)
{
    sptr<CallBase> call = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    if (call == nullptr) {
        TELEPHONY_LOGE("call not exists");
        return;
    }
    if (call->GetCallType() == CallType::TYPE_IMS) {
        sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(call.GetRefPtr());
        imsCall->ReceiveUpdateCallMediaModeResponse(*const_cast<CallMediaModeResponse *>(&response));
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
