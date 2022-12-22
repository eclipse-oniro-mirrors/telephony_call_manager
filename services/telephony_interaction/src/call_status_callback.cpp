/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "call_status_callback.h"

#include "audio_control_manager.h"
#include "call_ability_report_proxy.h"
#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "hitrace_meter.h"
#include "report_call_info_handler.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallStatusCallback::CallStatusCallback() {}

CallStatusCallback::~CallStatusCallback() {}

int32_t CallStatusCallback::UpdateCallReportInfo(const CallReportInfo &info)
{
    CallDetailInfo detailInfo;
    detailInfo.callType = info.callType;
    detailInfo.accountId = info.accountId;
    detailInfo.index = info.index;
    detailInfo.state = info.state;
    detailInfo.callMode = info.callMode;
    detailInfo.voiceDomain = info.voiceDomain;
    (void)memcpy_s(detailInfo.phoneNum, kMaxNumberLen, info.accountNum, kMaxNumberLen);
    (void)memset_s(detailInfo.bundleName, kMaxBundleNameLen, 0, kMaxBundleNameLen);
    int32_t ret = DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateCallReportInfo(detailInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallReportInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateCallReportInfo success! state:%{public}d", info.state);
    }
    return ret;
}

int32_t CallStatusCallback::UpdateCallsReportInfo(const CallsReportInfo &info)
{
    CallDetailsInfo detailsInfo;
    CallDetailInfo detailInfo;
    detailInfo.index = 0;
    detailInfo.state = TelCallState::CALL_STATUS_UNKNOWN;
    detailInfo.index = 0;
    CallsReportInfo callsInfo = info;
    std::vector<CallReportInfo>::iterator it = callsInfo.callVec.begin();
    for (; it != callsInfo.callVec.end(); ++it) {
        detailInfo.callType = (*it).callType;
        detailInfo.accountId = (*it).accountId;
        detailInfo.index = (*it).index;
        detailInfo.state = (*it).state;
        detailInfo.callMode = (*it).callMode;
        detailInfo.voiceDomain = (*it).voiceDomain;
        (void)memcpy_s(detailInfo.phoneNum, kMaxNumberLen, (*it).accountNum, kMaxNumberLen);
        (void)memset_s(detailInfo.bundleName, kMaxBundleNameLen, 0, kMaxBundleNameLen);
        detailsInfo.callVec.push_back(detailInfo);
    }
    detailsInfo.slotId = callsInfo.slotId;
    CallManagerHisysevent::WriteCallStateBehaviorEvent(
        detailsInfo.slotId, static_cast<int32_t>(detailInfo.state), detailInfo.index);
    (void)memset_s(detailsInfo.bundleName, kMaxBundleNameLen, 0, kMaxBundleNameLen);

    if (detailInfo.state == TelCallState::CALL_STATUS_INCOMING) {
        CallManagerHisysevent::WriteIncomingCallBehaviorEvent(
            detailsInfo.slotId, static_cast<int32_t>(detailInfo.callType), static_cast<int32_t>(detailInfo.callMode));
        TELEPHONY_LOGI("CallStatusCallback InComingCall StartAsyncTrace!");
        DelayedSingleton<CallManagerHisysevent>::GetInstance()->SetIncomingStartTime();
        StartAsyncTrace(HITRACE_TAG_OHOS, "InComingCall", getpid());
    }
    int32_t ret = DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateCallsReportInfo(detailsInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallsReportInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateCallsReportInfo success!");
    }
    return ret;
}

int32_t CallStatusCallback::UpdateDisconnectedCause(const DisconnectedDetails &details)
{
    int32_t ret = DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateDisconnectedCause(details);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateDisconnectedCause failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateDisconnectedCause success!");
    }
    return ret;
}

int32_t CallStatusCallback::UpdateEventResultInfo(const CellularCallEventInfo &info)
{
    int32_t ret = DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateEventResultInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateEventResultInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateEventResultInfo success!");
    }
    return ret;
}

int32_t CallStatusCallback::UpdateRBTPlayInfo(const RBTPlayInfo info)
{
    TELEPHONY_LOGI("UpdateRBTPlayInfo info = %{public}d", info);
    bool isLocalRingbackNeeded = false;
    if (info == RBTPlayInfo::LOCAL_ALERTING) {
        isLocalRingbackNeeded = true;
    }
    DelayedSingleton<AudioControlManager>::GetInstance()->SetLocalRingbackNeeded(isLocalRingbackNeeded);
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallback::StartDtmfResult(const int32_t result)
{
    CallResultReportId reportId = CallResultReportId::START_DTMF_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    TELEPHONY_LOGI("StartDtmfResult result = %{public}d", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::StopDtmfResult(const int32_t result)
{
    CallResultReportId reportId = CallResultReportId::STOP_DTMF_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    TELEPHONY_LOGI("StopDtmfResult result = %{public}d", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::SendUssdResult(const int32_t result)
{
    CallResultReportId reportId = CallResultReportId::SEND_USSD_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    TELEPHONY_LOGI("SendUssdResult result = %{public}d", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::SendMmiCodeResult(const MmiCodeInfo &info)
{
    TELEPHONY_LOGI("SendMmiCodeResult result = %{public}d, message = %{public}s", info.result, info.message);
    int32_t ret = DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportMmiCodeResult(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateDisconnectedCause failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateDisconnectedCause success!");
    }
    return ret;
}

int32_t CallStatusCallback::GetImsCallDataResult(const int32_t result)
{
    CallResultReportId reportId = CallResultReportId::GET_IMS_CALL_DATA_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    TELEPHONY_LOGI("GetImsCallDataResult result = %{public}d", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::UpdateGetWaitingResult(const CallWaitResponse &response)
{
    TELEPHONY_LOGI("GetWaitingResult status = %{public}d, classCw = %{public}d, result = %{public}d", response.status,
        response.classCw, response.result);
    CallResultReportId reportId = CallResultReportId::GET_CALL_WAITING_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", response.result);
    resultInfo.PutIntValue("status", response.status);
    resultInfo.PutIntValue("classCw", response.classCw);
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallback::UpdateSetWaitingResult(const int32_t result)
{
    TELEPHONY_LOGI("SetWaitingResult result = %{public}d", result);
    CallResultReportId reportId = CallResultReportId::SET_CALL_WAITING_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusCallback::UpdateGetRestrictionResult(const CallRestrictionResponse &result)
{
    TELEPHONY_LOGI("GetRestrictionResult status = %{public}d, classCw = %{public}d, result = %{public}d",
        result.status, result.classCw, result.result);
    CallResultReportId reportId = CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result.result);
    resultInfo.PutIntValue("status", result.status);
    resultInfo.PutIntValue("classCw", result.classCw);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::UpdateSetRestrictionResult(const int32_t result)
{
    TELEPHONY_LOGI("SetRestrictionResult result = %{public}d", result);
    CallResultReportId reportId = CallResultReportId::SET_CALL_RESTRICTION_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::UpdateGetTransferResult(const CallTransferResponse &response)
{
    CallResultReportId reportId = CallResultReportId::GET_CALL_TRANSFER_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", response.result);
    resultInfo.PutIntValue("status", response.status);
    resultInfo.PutIntValue("classx", response.classx);
    resultInfo.PutStringValue("number", response.number);
    resultInfo.PutIntValue("type", response.type);
    resultInfo.PutIntValue("reason", response.reason);
    resultInfo.PutIntValue("time", response.time);
    resultInfo.PutIntValue("startHour", response.startHour);
    resultInfo.PutIntValue("startMinute", response.startMinute);
    resultInfo.PutIntValue("endHour", response.endHour);
    resultInfo.PutIntValue("endMinute", response.endMinute);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::UpdateSetTransferResult(const int32_t result)
{
    TELEPHONY_LOGI("SetTransferResult result = %{public}d", result);
    CallResultReportId reportId = CallResultReportId::SET_CALL_TRANSFER_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::UpdateGetCallClipResult(const ClipResponse &clipResponse)
{
    TELEPHONY_LOGI("GetCallClipResult action = %{public}d, clipStat = %{public}d, result = %{public}d",
        clipResponse.action, clipResponse.clipStat, clipResponse.result);
    CallResultReportId reportId = CallResultReportId::GET_CALL_CLIP_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", clipResponse.result);
    resultInfo.PutIntValue("action", clipResponse.action);
    resultInfo.PutIntValue("clipStat", clipResponse.clipStat);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::UpdateGetCallClirResult(const ClirResponse &clirResponse)
{
    TELEPHONY_LOGI("GetCallClirResult action = %{public}d, clirStat = %{public}d, result = %{public}d",
        clirResponse.action, clirResponse.clirStat, clirResponse.result);
    CallResultReportId reportId = CallResultReportId::GET_CALL_CLIR_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", clirResponse.result);
    resultInfo.PutIntValue("action", clirResponse.action);
    resultInfo.PutIntValue("clirStat", clirResponse.clirStat);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::UpdateSetCallClirResult(const int32_t result)
{
    TELEPHONY_LOGI("GetCallClirResult result = %{public}d", result);
    CallResultReportId reportId = CallResultReportId::SET_CALL_CLIR_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::StartRttResult(const int32_t result)
{
    TELEPHONY_LOGI("StartRttResult result = %{public}d", result);
    CallResultReportId reportId = CallResultReportId::START_RTT_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::StopRttResult(const int32_t result)
{
    TELEPHONY_LOGI("StopRttResult result = %{public}d", result);
    CallResultReportId reportId = CallResultReportId::STOP_RTT_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::GetImsConfigResult(const GetImsConfigResponse &response)
{
    CallResultReportId reportId = CallResultReportId::GET_IMS_CONFIG_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", response.result);
    resultInfo.PutIntValue("value", response.value);
    TELEPHONY_LOGI("result = %{public}d value = %{public}d", response.result, response.value);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::SetImsConfigResult(const int32_t result)
{
    CallResultReportId reportId = CallResultReportId::SET_IMS_CONFIG_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    TELEPHONY_LOGI("result = %{public}d", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::GetImsFeatureValueResult(const GetImsFeatureValueResponse &response)
{
    CallResultReportId reportId = CallResultReportId::GET_IMS_FEATURE_VALUE_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", response.result);
    resultInfo.PutIntValue("value", response.value);
    TELEPHONY_LOGI("result = %{public}d value = %{public}d", response.result, response.value);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::SetImsFeatureValueResult(const int32_t result)
{
    CallResultReportId reportId = CallResultReportId::SET_IMS_FEATURE_VALUE_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    TELEPHONY_LOGI("result = %{public}d", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::ReceiveUpdateCallMediaModeResponse(const CallMediaModeResponse &response)
{
    CallResultReportId reportId = CallResultReportId::UPDATE_MEDIA_MODE_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", response.result);
    TELEPHONY_LOGI("UpdateImsCallMode result = %{public}d", response.result);
    (void)DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateMediaModeResponse(response);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::InviteToConferenceResult(const int32_t result)
{
    CallResultReportId reportId = CallResultReportId::INVITE_TO_CONFERENCE_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    TELEPHONY_LOGI("InviteToConferenceResult result = %{public}d", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}
} // namespace Telephony
} // namespace OHOS
