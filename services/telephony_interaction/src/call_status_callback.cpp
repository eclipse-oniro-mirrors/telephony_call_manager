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
#include "cpp/task_ext.h"
#include "hitrace_meter.h"
#include "report_call_info_handler.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr size_t CHAR_INDEX = 0;
constexpr int32_t ERR_CALL_ID = 0;
const uint64_t DELAY_STOP_PLAY_TIME = 3000000;
const int32_t MIN_MULITY_CALL_COUNT = 2;

CallStatusCallback::CallStatusCallback() {}

CallStatusCallback::~CallStatusCallback() {}

int32_t CallStatusCallback::UpdateCallReportInfo(const CallReportInfo &info)
{
    CallDetailInfo detailInfo;
    if (info.callType == CallType::TYPE_VOIP) {
        detailInfo.voipCallInfo.voipCallId = info.voipCallInfo.voipCallId;
        detailInfo.voipCallInfo.userName = info.voipCallInfo.userName;
        (detailInfo.voipCallInfo.userProfile).assign(
            (info.voipCallInfo.userProfile).begin(), (info.voipCallInfo.userProfile).end());
        detailInfo.voipCallInfo.extensionId = info.voipCallInfo.extensionId;
        detailInfo.voipCallInfo.voipBundleName = info.voipCallInfo.voipBundleName;
        detailInfo.voipCallInfo.abilityName = info.voipCallInfo.abilityName;
        detailInfo.voipCallInfo.showBannerForIncomingCall = info.voipCallInfo.showBannerForIncomingCall;
        detailInfo.voipCallInfo.isConferenceCall = info.voipCallInfo.isConferenceCall;
        detailInfo.voipCallInfo.isVoiceAnswerSupported = info.voipCallInfo.isVoiceAnswerSupported;
        detailInfo.voipCallInfo.hasMicPermission = info.voipCallInfo.hasMicPermission;
        detailInfo.voipCallInfo.uid = info.voipCallInfo.uid;
    }
    detailInfo.callType = info.callType;
    detailInfo.accountId = info.accountId;
    detailInfo.index = info.index;
    detailInfo.state = info.state;
    detailInfo.callMode = info.callMode;
    detailInfo.voiceDomain = info.voiceDomain;
    detailInfo.mpty = info.mpty;
    (void)memcpy_s(detailInfo.phoneNum, kMaxNumberLen, info.accountNum, kMaxNumberLen);
    (void)memset_s(detailInfo.bundleName, kMaxBundleNameLen, 0, kMaxBundleNameLen);
    int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallReportInfo(detailInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallReportInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGW("UpdateCallReportInfo success! state:%{public}d, index:%{public}d", info.state, info.index);
    }
    return ret;
}

int32_t CallStatusCallback::UpdateCallsReportInfo(const CallsReportInfo &info)
{
    CallDetailsInfo detailsInfo;
    CallDetailInfo detailInfo;
    detailInfo.index = 0;
    detailInfo.state = TelCallState::CALL_STATUS_UNKNOWN;
    CallsReportInfo callsInfo = info;
    std::vector<CallReportInfo>::iterator it = callsInfo.callVec.begin();
    for (; it != callsInfo.callVec.end(); ++it) {
        detailInfo.callType = (*it).callType;
        detailInfo.accountId = (*it).accountId;
        detailInfo.index = (*it).index;
        detailInfo.state = (*it).state;
        detailInfo.callMode = (*it).callMode;
        detailInfo.voiceDomain = (*it).voiceDomain;
        detailInfo.mpty = (*it).mpty;
        detailInfo.crsType = (*it).crsType;
        detailInfo.originalCallType = (*it).originalCallType;
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
    int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallsReportInfo(detailsInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallsReportInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGW("UpdateCallsReportInfo success!");
    }
    return ret;
}

int32_t CallStatusCallback::UpdateDisconnectedCause(const DisconnectedDetails &details)
{
    int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateDisconnectedCause(details);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateDisconnectedCause failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateDisconnectedCause success!");
    }
    return ret;
}

int32_t CallStatusCallback::UpdateEventResultInfo(const CellularCallEventInfo &info)
{
    int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateEventResultInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateEventResultInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateEventResultInfo success!");
    }
    return ret;
}

int32_t CallStatusCallback::UpdateVoipEventInfo(const VoipCallEventInfo &info)
{
    int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateVoipEventInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateVoipEventInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateVoipEventInfo success!");
    }
    return ret;
}

int32_t CallStatusCallback::UpdateRBTPlayInfo(const RBTPlayInfo info)
{
    TELEPHONY_LOGI("UpdateRBTPlayInfo info = %{public}d", info);
    bool isLocalRingbackNeeded = false;
    if (info == RBTPlayInfo::LOCAL_ALERTING) {
        isLocalRingbackNeeded = true;
        CallEventInfo eventInfo;
        eventInfo.eventId = CallAbilityEventId::EVENT_LOCAL_ALERTING;
        DelayedSingleton<CallAbilityReportProxy>::GetInstance()->CallEventUpdated(eventInfo);
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

int32_t CallStatusCallback::UpdateSetRestrictionPasswordResult(const int32_t result)
{
    TELEPHONY_LOGI("SetRestrictionPassword result = %{public}d", result);
    CallResultReportId reportId = CallResultReportId::SET_CALL_RESTRICTION_PWD_REPORT_ID;
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

int32_t CallStatusCallback::ReceiveUpdateCallMediaModeRequest(const CallModeReportInfo &response)
{
    auto weak = weak_from_this();
    TELEPHONY_LOGI("ReceiveUpdateCallMediaModeRequest result = %{public}d", response.result);
    int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->ReceiveImsCallModeRequest(response);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ReceiveUpdateCallMediaModeRequest failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("ReceiveUpdateCallMediaModeRequest success!");
    }
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObjectByIndexAndSlotId(response.callIndex, response.slotId);
    if (callPtr != nullptr && callPtr->GetTelCallState() == TelCallState::CALL_STATUS_ACTIVE
        && response.callMode == ImsCallMode::CALL_MODE_SEND_RECEIVE) {
        if (DelayedSingleton<AudioControlManager>::GetInstance()->PlayWaitingTone() == TELEPHONY_SUCCESS) {
            waitingToneHandle_ = ffrt::submit_h([weak]() {
                auto strong = weak.lock();
                if (strong) {
                    strong->ShouldStopWaitingTone();
                }
            }, {}, {}, ffrt::task_attr().delay(DELAY_STOP_PLAY_TIME));
        }
    }
    return ret;
}

int32_t CallStatusCallback::ReceiveUpdateCallMediaModeResponse(const CallModeReportInfo &response)
{
    TELEPHONY_LOGI("ReceiveUpdateCallMediaModeResponse result = %{public}d", response.result);
    int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->ReceiveImsCallModeResponse(response);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ReceiveUpdateCallMediaModeResponse failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("ReceiveUpdateCallMediaModeResponse success!");
    }
    if (waitingToneHandle_ != nullptr) {
        if (int result = ffrt::skip(waitingToneHandle_) != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("ReceiveUpdateCallMediaModeResponse failed! result:%{public}d", result);
        }
        ShouldStopWaitingTone();
    }
    return ret;
}

void CallStatusCallback::ShouldStopWaitingTone()
{
    bool hasRingingCall = false;
    CallObjectManager::HasRingingCall(hasRingingCall);
    if (!(hasRingingCall && (CallObjectManager::GetCurrentCallNum() >= MIN_MULITY_CALL_COUNT))) {
        DelayedSingleton<AudioControlManager>::GetInstance()->StopWaitingTone();
    }
    waitingToneHandle_ = nullptr;
}

int32_t CallStatusCallback::InviteToConferenceResult(const int32_t result)
{
    CallResultReportId reportId = CallResultReportId::INVITE_TO_CONFERENCE_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    TELEPHONY_LOGI("InviteToConferenceResult result = %{public}d", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::CloseUnFinishedUssdResult(const int32_t result)
{
    CallResultReportId reportId = CallResultReportId::CLOSE_UNFINISHED_USSD_REPORT_ID;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("result", result);
    TELEPHONY_LOGI("CloseUnFinishedUssdResult result = %{public}d", result);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(reportId, resultInfo);
}

int32_t CallStatusCallback::ReportPostDialChar(const std::string &c)
{
    TELEPHONY_LOGI("ReportPostDialChar");
    char nextDtmf = ' ';
    if (!c.empty() && c.length() > CHAR_INDEX) {
        nextDtmf = c[CHAR_INDEX];
    }
    return DelayedSingleton<AudioControlManager>::GetInstance()->OnPostDialNextChar(nextDtmf);
}

int32_t CallStatusCallback::ReportPostDialDelay(const std::string &str)
{
    TELEPHONY_LOGI("ReportPostDialDelay");
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportPostDialDelay(str);
}

int32_t CallStatusCallback::HandleCallSessionEventChanged(const CallSessionReportInfo &reportInfo)
{
    TELEPHONY_LOGI("HandleCallSessionEventChanged");
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObjectByIndex(reportInfo.index);
    CallSessionEvent sessionEvent;
    if (callPtr == nullptr) {
        sessionEvent.callId = ERR_CALL_ID;
    } else {
        sessionEvent.callId = callPtr->GetCallID();
    }
    sessionEvent.eventId = reportInfo.eventId;
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportCallSessionEventChange(sessionEvent);
}

int32_t CallStatusCallback::HandlePeerDimensionsChanged(const PeerDimensionsReportInfo &dimensionsReportInfo)
{
    TELEPHONY_LOGI("HandlePeerDimensionsChanged");
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObjectByIndex(dimensionsReportInfo.index);
    PeerDimensionsDetail detail;
    if (callPtr == nullptr) {
        detail.callId = ERR_CALL_ID;
    } else {
        detail.callId = callPtr->GetCallID();
    }
    detail.width = dimensionsReportInfo.width;
    detail.height = dimensionsReportInfo.height;
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportPeerDimensionsChange(detail);
}

int32_t CallStatusCallback::HandleCallDataUsageChanged(const int64_t dataUsage)
{
    TELEPHONY_LOGI("HandleCallDataUsageChanged");
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportCallDataUsageChange(dataUsage);
}

int32_t CallStatusCallback::HandleCameraCapabilitiesChanged(
    const CameraCapabilitiesReportInfo &cameraCapabilitiesReportInfo)
{
    TELEPHONY_LOGI("CameraCapabilitiesChange");
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObjectByIndex(cameraCapabilitiesReportInfo.index);
    CameraCapabilities cameraCapabilities;
    if (callPtr == nullptr) {
        TELEPHONY_LOGI("callPtr is null, set callId as ERR_CALL_ID");
        cameraCapabilities.callId = ERR_CALL_ID;
    } else {
        cameraCapabilities.callId = callPtr->GetCallID();
    }
    cameraCapabilities.width = cameraCapabilitiesReportInfo.width;
    cameraCapabilities.height = cameraCapabilitiesReportInfo.height;
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportCameraCapabilities(cameraCapabilities);
}
} // namespace Telephony
} // namespace OHOS
