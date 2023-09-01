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

#include "call_request_process.h"

#include "call_ability_report_proxy.h"
#include "call_control_manager.h"
#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "call_number_utils.h"
#include "cellular_call_connection.h"
#include "common_type.h"
#include "core_service_connection.h"
#include "cs_call.h"
#include "ims_call.h"
#include "ott_call.h"
#include "report_call_info_handler.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallRequestProcess::CallRequestProcess() {}

CallRequestProcess::~CallRequestProcess() {}

int32_t CallRequestProcess::DialRequest()
{
    DialParaInfo info;
    DelayedSingleton<CallControlManager>::GetInstance()->GetDialParaInfo(info);
    if (!info.isDialing) {
        TELEPHONY_LOGE("the device is not dialing!");
        CallManagerHisysevent::WriteDialCallFaultEvent(info.accountId, static_cast<int32_t>(info.callType),
            static_cast<int32_t>(info.videoState), static_cast<int32_t>(CallErrorCode::CALL_ERROR_DEVICE_NOT_DIALING),
            "the device is not dialing");
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    if (info.number.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    if (info.dialType == DialType::DIAL_CARRIER_TYPE &&
        DelayedSingleton<CoreServiceConnection>::GetInstance()->IsFdnEnabled(info.accountId)) {
        std::vector<std::u16string> fdnNumberList =
            DelayedSingleton<CoreServiceConnection>::GetInstance()->GetFdnNumberList(info.accountId);
        if (!fdnNumberList.empty() && !IsFdnNumber(fdnNumberList, info.number)) {
            CallEventInfo eventInfo;
            (void)memset_s(eventInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen);
            eventInfo.eventId = CallAbilityEventId::EVENT_INVALID_FDN_NUMBER;
            (void)memcpy_s(eventInfo.phoneNum, kMaxNumberLen, info.number.c_str(), info.number.length());
            DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallEventUpdated(eventInfo);
            TELEPHONY_LOGW("invalid fdn number!");
            CallManagerHisysevent::WriteDialCallFaultEvent(info.accountId, static_cast<int32_t>(info.callType),
                static_cast<int32_t>(info.videoState),
                static_cast<int32_t>(CallErrorCode::CALL_ERROR_INVALID_FDN_NUMBER), "invalid fdn number!");
            return CALL_ERR_DIAL_FAILED;
        }
    }
    TELEPHONY_LOGI("dialType:%{public}d", info.dialType);
    int32_t ret = CALL_ERR_UNKNOW_DIAL_TYPE;
    switch (info.dialType) {
        case DialType::DIAL_CARRIER_TYPE:
            ret = CarrierDialProcess(info);
            break;
        case DialType::DIAL_VOICE_MAIL_TYPE:
            ret = VoiceMailDialProcess(info);
            break;
        case DialType::DIAL_OTT_TYPE:
            ret = OttDialProcess(info);
            break;
        default:
            TELEPHONY_LOGE("invalid dialType:%{public}d", info.dialType);
            break;
    }
    return ret;
}

void CallRequestProcess::AnswerRequest(int32_t callId, int32_t videoState)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    int32_t ret = call->AnswerCall(videoState);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("AnswerCall failed!");
        return;
    }
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyIncomingCallAnswered(call);
}

void CallRequestProcess::RejectRequest(int32_t callId, bool isSendSms, std::string &content)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }

    int32_t ret = call->HangUpCall();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RejectCall failed!");
        return;
    }
    TELEPHONY_LOGI("start to send reject message...");
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyIncomingCallRejected(call, isSendSms, content);
}

void CallRequestProcess::HangUpRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    int32_t waitingCallNum = GetCallNum(TelCallState::CALL_STATUS_WAITING);
    TelCallState state = call->GetTelCallState();
    TelConferenceState confState = call->GetTelConferenceState();
    if ((((state == TelCallState::CALL_STATUS_ACTIVE) &&
        (CallObjectManager::IsCallExist(call->GetCallType(), TelCallState::CALL_STATUS_HOLDING))) ||
        (confState == TelConferenceState::TEL_CONFERENCE_ACTIVE)) && waitingCallNum == 0) {
        TELEPHONY_LOGI("release the active call and recover the held call");
        call->SetPolicyFlag(PolicyFlag::POLICY_FLAG_HANG_UP_ACTIVE);
    } else if (confState == TelConferenceState::TEL_CONFERENCE_HOLDING && waitingCallNum == 0) {
        TELEPHONY_LOGI("release the held call and the wait call");
        call->SetPolicyFlag(PolicyFlag::POLICY_FLAG_HANG_UP_HOLD_WAIT);
    } else if (((confState == TelConferenceState::TEL_CONFERENCE_HOLDING) ||
        (confState == TelConferenceState::TEL_CONFERENCE_ACTIVE)) && waitingCallNum != 0) {
        TELEPHONY_LOGI("conference call and holding state, hangup conference call");
        std::vector<std::u16string> callIdList;
        call->GetSubCallIdList(callIdList);
        for (auto it = callIdList.begin(); it != callIdList.end(); ++it) {
            int32_t callId = -1;
            StrToInt(Str16ToStr8(*it), callId);
            KickOutFromConferenceRequest(callId);
        }
    }
    call->HangUpCall();
    if ((state == TelCallState::CALL_STATUS_DIALING || state == TelCallState::CALL_STATUS_ALERTING) &&
        waitingCallNum == 0) {
        sptr<CallBase> holdCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
        if (holdCall) {
            TELEPHONY_LOGI("release the dialing/alerting call and recover the held call");
            holdCall->UnHoldCall();
        }
    }
}

void CallRequestProcess::HoldRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    call->HoldCall();
}

void CallRequestProcess::UnHoldRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    call->UnHoldCall();
}

void CallRequestProcess::SwitchRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    call->SwitchCall();
}

void CallRequestProcess::CombineConferenceRequest(int32_t mainCallId)
{
    sptr<CallBase> call = GetOneCallObject(mainCallId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, mainCallId:%{public}d", mainCallId);
        return;
    }
    int32_t ret = call->CombineConference();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CombineConference failed");
    }
}

void CallRequestProcess::SeparateConferenceRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    int32_t ret = call->SeparateConference();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SeparateConference failed");
    }
}

void CallRequestProcess::KickOutFromConferenceRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    int32_t ret = call->KickOutFromConference();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("KickOutFormConference failed");
    }
}

int32_t CallRequestProcess::UpdateImsCallMode(int32_t callId, ImsCallMode mode)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return ret;
    }
    // only netcall type support update call media mode
    if (call->GetCallType() == CallType::TYPE_IMS) {
        sptr<IMSCall> netCall = reinterpret_cast<IMSCall *>(call.GetRefPtr());
        TELEPHONY_LOGI("ims call update media request");
        ret = netCall->SendUpdateCallMediaModeRequest(mode);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("SendUpdateCallMediaModeRequest failed. %{public}d", ret);
        }
    } else {
        TELEPHONY_LOGE("the call object not support upgrade/downgrad media, callId:%{public}d", callId);
    }
    return ret;
}

void CallRequestProcess::UpdateCallMediaModeRequest(int32_t callId, ImsCallMode mode)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    AppExecFwk::PacMap resultInfo;
    ret = UpdateImsCallMode(callId, mode);
    if (ret != TELEPHONY_SUCCESS) {
        resultInfo.PutIntValue("result", ret);
        DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportAsyncResults(
            CallResultReportId::UPDATE_MEDIA_MODE_REPORT_ID, resultInfo);
    }
}

void CallRequestProcess::StartRttRequest(int32_t callId, std::u16string &msg)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    if (call->GetCallType() != CallType::TYPE_IMS) {
        TELEPHONY_LOGE("Unsupported Network type, callId:%{public}d", callId);
        return;
    } else {
        sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(call.GetRefPtr());
        imsCall->StartRtt(msg);
    }
}

void CallRequestProcess::StopRttRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    if (call->GetCallType() != CallType::TYPE_IMS) {
        TELEPHONY_LOGE("Unsupported Network type, callId:%{public}d", callId);
        return;
    } else {
        sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(call.GetRefPtr());
        imsCall->StopRtt();
    }
}

void CallRequestProcess::JoinConference(int32_t callId, std::vector<std::string> &numberList)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    int32_t ret =
        DelayedSingleton<CellularCallConnection>::GetInstance()->InviteToConference(numberList, call->GetSlotId());
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invite to conference failed!");
        return;
    }
}

int32_t CallRequestProcess::UpdateCallReportInfo(const DialParaInfo &info, TelCallState state)
{
    CallDetailInfo callDetatilInfo;
    if (memset_s(&callDetatilInfo, sizeof(CallDetailInfo), 0, sizeof(CallDetailInfo)) != EOK) {
        TELEPHONY_LOGE("memset_s callDetatilInfo fail");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    callDetatilInfo.callType = info.callType;
    callDetatilInfo.accountId = info.accountId;
    callDetatilInfo.index = info.index;
    callDetatilInfo.state = state;
    callDetatilInfo.callMode = info.videoState;
    callDetatilInfo.voiceDomain = static_cast<int32_t>(info.callType);
    if (info.number.length() > kMaxNumberLen) {
        TELEPHONY_LOGE("numbser length out of range");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    if (memcpy_s(&callDetatilInfo.phoneNum, kMaxNumberLen, info.number.c_str(), info.number.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s number failed!");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    return DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallReportInfo(callDetatilInfo);
}

int32_t CallRequestProcess::HandleDialFail()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while (!isFirstDialCallAdded_) {
        if (cv_.wait_for(lock, std::chrono::seconds(WAIT_TIME_ONE_SECOND)) == std::cv_status::timeout) {
            TELEPHONY_LOGE("CarrierDialProcess call is not added");
            return CALL_ERR_DIAL_FAILED;
        }
    }
    sptr<CallBase> call = nullptr;
    call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_CREATE);
    if (call != nullptr) {
        return DealFailDial(call);
    }
    call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_CONNECTING);
    if (call != nullptr) {
        return DealFailDial(call);
    }
    call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING);
    if (call != nullptr) {
        return DealFailDial(call);
    }
    TELEPHONY_LOGE("can not find connect call or dialing call");
    return CALL_ERR_CALL_STATE;
}

int32_t CallRequestProcess::CarrierDialProcess(DialParaInfo &info)
{
    std::string newPhoneNumer =
        DelayedSingleton<CallNumberUtils>::GetInstance()->RemoveSeparatorsPhoneNumber(info.number);
    bool isMMiCode = DelayedSingleton<CallNumberUtils>::GetInstance()->IsMMICode(newPhoneNumer);
    int32_t ret = TELEPHONY_ERROR;
    if (!isMMiCode) {
        isFirstDialCallAdded_ = false;
        info.number = newPhoneNumer;
        ret = UpdateCallReportInfo(info, TelCallState::CALL_STATUS_DIALING);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("UpdateCallReportInfo failed!");
            return ret;
        }
    }
    CellularCallInfo callInfo;
    ret = PackCellularCallInfo(info, callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("PackCellularCallInfo failed!");
        CallManagerHisysevent::WriteDialCallFaultEvent(info.accountId, static_cast<int32_t>(info.callType),
            static_cast<int32_t>(info.videoState), ret, "Carrier type PackCellularCallInfo failed");
        return ret;
    }
    // Obtain gateway information
    ret = DelayedSingleton<CellularCallConnection>::GetInstance()->Dial(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Dial failed!");
        if (isMMiCode) {
            return ret;
        }
        int32_t handleRet = HandleDialFail();
        if (handleRet != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("HandleDialFail failed!");
            return handleRet;
        }
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestProcess::VoiceMailDialProcess(DialParaInfo &info)
{
    return CarrierDialProcess(info);
}

int32_t CallRequestProcess::OttDialProcess(DialParaInfo &info)
{
    AppExecFwk::PacMap callInfo;
    callInfo.PutStringValue("phoneNumber", info.number);
    callInfo.PutStringValue("bundleName", info.bundleName);
    callInfo.PutIntValue("videoState", static_cast<int32_t>(info.videoState));
    int32_t ret = DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_DIAL, callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("OTT call Dial failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestProcess::PackCellularCallInfo(DialParaInfo &info, CellularCallInfo &callInfo)
{
    callInfo.callId = info.callId;
    callInfo.accountId = info.accountId;
    callInfo.callType = info.callType;
    callInfo.videoState = static_cast<int32_t>(info.videoState);
    callInfo.index = info.index;
    callInfo.slotId = info.accountId;
    if (memset_s(callInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen) != EOK) {
        TELEPHONY_LOGW("memset_s failed!");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    if (info.number.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    if (memcpy_s(callInfo.phoneNum, kMaxNumberLen, info.number.c_str(), info.number.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

bool CallRequestProcess::IsFdnNumber(std::vector<std::u16string> fdnNumberList, std::string phoneNumber)
{
    char number[kMaxNumberLen + 1] = { 0 };
    int32_t j = 0;
    for (int32_t i = 0; i < static_cast<int32_t>(phoneNumber.length()); i++) {
        if (i >= kMaxNumberLen) {
            break;
        }
        if (*(phoneNumber.c_str() + i) != ' ') {
            number[j++] = *(phoneNumber.c_str() + i);
        }
    }
    for (std::vector<std::u16string>::iterator it = fdnNumberList.begin(); it != fdnNumberList.end(); ++it) {
        if (strstr(number, Str16ToStr8(*it).c_str()) != nullptr) {
            TELEPHONY_LOGI("you are allowed to dial!");
            return true;
        }
    }
    TELEPHONY_LOGW("There is no fixed number.");
    return false;
}
} // namespace Telephony
} // namespace OHOS
