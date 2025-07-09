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
#include "call_dialog.h"
#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "call_number_utils.h"
#include "call_request_event_handler_helper.h"
#include "cellular_call_connection.h"
#include "common_type.h"
#include "core_service_client.h"
#include "core_service_connection.h"
#include "cs_call.h"
#include "ims_call.h"
#include "ott_call.h"
#include "report_call_info_handler.h"
#include "telephony_log_wrapper.h"
#include "bluetooth_call_connection.h"

namespace OHOS {
namespace Telephony {
bool g_flagForDsda = false;
constexpr int32_t INIT_INDEX = 0;

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
    bool isEcc = false;
    DelayedSingleton<CallNumberUtils>::GetInstance()->CheckNumberIsEmergency(info.number, info.accountId, isEcc);
    if (!isEcc && info.dialType == DialType::DIAL_CARRIER_TYPE &&
        DelayedSingleton<CoreServiceConnection>::GetInstance()->IsFdnEnabled(info.accountId)) {
        std::vector<std::u16string> fdnNumberList =
            DelayedSingleton<CoreServiceConnection>::GetInstance()->GetFdnNumberList(info.accountId);
        if (fdnNumberList.empty() || !IsFdnNumber(fdnNumberList, info.number)) {
            CallEventInfo eventInfo;
            (void)memset_s(eventInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen);
            eventInfo.eventId = CallAbilityEventId::EVENT_INVALID_FDN_NUMBER;
            (void)memcpy_s(eventInfo.phoneNum, kMaxNumberLen, info.number.c_str(), info.number.length());
            DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallEventUpdated(eventInfo);
            CallManagerHisysevent::WriteDialCallFaultEvent(info.accountId, static_cast<int32_t>(info.callType),
                static_cast<int32_t>(info.videoState),
                static_cast<int32_t>(CallErrorCode::CALL_ERROR_INVALID_FDN_NUMBER), "invalid fdn number!");
            DelayedSingleton<CallDialog>::GetInstance()->DialogConnectExtension("CALL_FAILED_DUE_TO_FDN");
            return CALL_ERR_DIAL_FAILED;
        }
    }
    TELEPHONY_LOGI("dialType:%{public}d", info.dialType);
    return HandleDialRequest(info);
}

int32_t CallRequestProcess::HandleDialRequest(DialParaInfo &info)
{
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
        case DialType::DIAL_BLUETOOTH_TYPE:
            ret  = BluetoothDialProcess(info);
            break;
        default:
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
    call->SetAnswerVideoState(videoState);
    if (call->GetCallType() == CallType::TYPE_VOIP) {
        int32_t ret = call->AnswerCall(videoState);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("AnswerCall failed!");
            return;
        }
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyIncomingCallAnswered(call);
        return;
    }
    AnswerRequestForDsda(call, callId, videoState);
}

void CallRequestProcess::AnswerRequestForDsda(sptr<CallBase> call, int32_t callId, int32_t videoState)
{
    int32_t slotId = call->GetSlotId();
    int32_t callCrsType = 2;
    if (IsDsdsMode3()) {
        DisconnectOtherSubIdCall(callId, slotId, videoState);
    } else if (IsDsdsMode5()) {
        if (NeedAnswerVTAndEndActiveVO(callId, videoState)) {
            TELEPHONY_LOGI("Answer videoCall for Dsda");
            DisconnectOtherCallForVideoCall(callId);
            call->SetAutoAnswerState(true);
            return;
        } else if (NeedAnswerVOAndEndActiveVT(callId, videoState)) {
            TELEPHONY_LOGI("Answer voiceCall for Dsda, but has video call");
            DisconnectOtherCallForVideoCall(callId);
            call->SetAutoAnswerState(true);
            return;
        }
        // There is already an incoming call to the CRS.
        int32_t otherRingCallId = GetOtherRingingCall(callId);
        if (otherRingCallId != INVALID_CALLID) {
            sptr<CallBase> ringingCall = GetOneCallObject(otherRingCallId);
            if (ringingCall != nullptr && ringingCall->GetCrsType() == callCrsType) {
                ringingCall->HangUpCall();
                call->SetAutoAnswerState(true);
                return;
            }
        }
        call->SetAutoAnswerState(true);
        HoldOrDisconnectedCall(callId, slotId, videoState);
    } else {
        int32_t ret = call->AnswerCall(videoState);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("AnswerCall failed!");
            return;
        }
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyIncomingCallAnswered(call);
    }
}

bool CallRequestProcess::IsDsdsMode3()
{
    int32_t dsdsMode = DSDS_MODE_V2;
    DelayedRefSingleton<CoreServiceClient>::GetInstance().GetDsdsMode(dsdsMode);
    TELEPHONY_LOGI("dsdsMode:%{public}d", dsdsMode);
    if (dsdsMode == DSDS_MODE_V3) {
        return true;
    }
    return false;
}

bool CallRequestProcess::IsDsdsMode5()
{
    int32_t dsdsMode = DSDS_MODE_V2;
    DelayedRefSingleton<CoreServiceClient>::GetInstance().GetDsdsMode(dsdsMode);
    TELEPHONY_LOGI("IsDsdsMode5:%{public}d", dsdsMode);
    if (dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_DSDA) ||
        dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_TDM)) {
        return true;
    }
    return false;
}

bool CallRequestProcess::HasConnectingCall(bool isIncludeVoipCall)
{
    int32_t dialingCallNum = GetCallNum(TelCallState::CALL_STATUS_DIALING, isIncludeVoipCall);
    int32_t alertingCallNum = GetCallNum(TelCallState::CALL_STATUS_ALERTING, isIncludeVoipCall);
    if (dialingCallNum == 0 && alertingCallNum == 0) {
        return false;
    }
    return true;
}

bool CallRequestProcess::HasActivedCall(bool isIncludeVoipCall)
{
    int32_t activeCallNum = GetCallNum(TelCallState::CALL_STATUS_ACTIVE, isIncludeVoipCall);
    int32_t holdingCallNum = GetCallNum(TelCallState::CALL_STATUS_HOLDING, isIncludeVoipCall);
    int32_t answeredCallNum = GetCallNum(TelCallState::CALL_STATUS_ANSWERED, isIncludeVoipCall);
    if (activeCallNum == 0 && holdingCallNum == 0 && answeredCallNum == 0) {
        return false;
    }
    return true;
}

bool CallRequestProcess::NeedAnswerVTAndEndActiveVO(int32_t callId, int32_t videoState)
{
    TELEPHONY_LOGI("Enter NeedAnswerVTAndEndActiveVO");
    sptr<CallBase> activeCall = GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    sptr<CallBase> holdingCall = GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
    // if this call is existed foreground or backgroud call, don't hang up it.
    if ((activeCall != nullptr && activeCall->GetCallID() == callId) ||
        (holdingCall != nullptr && holdingCall->GetCallID() == callId)) {
        return false;
    }
    if (HasConnectingCall(false) || HasActivedCall(false)) {
        if (videoState != static_cast<int32_t>(VideoStateType::TYPE_VOICE)) {
            TELEPHONY_LOGI("answer a new video call, need to hang up the exist call");
            return true;
        }
    }
    return false;
}

bool CallRequestProcess::NeedAnswerVOAndEndActiveVT(int32_t callId, int32_t videoState)
{
    if (videoState != static_cast<int32_t>(VideoStateType::TYPE_VOICE)) {
        return false;
    }
    if (HasActivedCall(false)) {
        sptr<CallBase> activeCall = GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
        sptr<CallBase> holdingCall = GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
        if ((activeCall != nullptr && activeCall->GetVideoStateType() != VideoStateType::TYPE_VOICE &&
                activeCall->GetCallID() != callId) ||
            (holdingCall != nullptr && holdingCall->GetVideoStateType() != VideoStateType::TYPE_VOICE &&
                holdingCall->GetCallID() != callId)) {
            TELEPHONY_LOGI("answer a new voice call, need to hang up the exist video call");
            return true;
        }
    }
    return false;
}

int32_t CallRequestProcess::GetOtherRingingCall(int32_t currentCallId)
{
    int32_t otherRingCallId = INVALID_CALLID;
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    for (int32_t otherCallId : callIdList) {
        if (otherCallId == currentCallId) {
            continue;
        }
        sptr<CallBase> call = GetOneCallObject(otherCallId);
        if (call != nullptr && call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_RINGING) {
            otherRingCallId = call->GetCallID();
            break;
        }
    }
    return otherRingCallId;
}

void CallRequestProcess::HoldOrDisconnectedCall(int32_t callId, int32_t slotId, int32_t videoState)
{
    TELEPHONY_LOGI("Enter HoldOrDisconnectedCall");
    std::list<int32_t> callIdList;
    bool noOtherCall = true;
    bool flagForConference = false;
    GetCarrierCallList(callIdList);
    IsExistCallOtherSlot(callIdList, slotId, noOtherCall);
    if (noOtherCall) {
        TELEPHONY_LOGI("no Other Slot Call");
        sptr<CallBase> call = GetOneCallObject(callId);
        if (call == nullptr) {
            TELEPHONY_LOGE("call is nullptr");
            return;
        }
        int32_t ret = call->AnswerCall(videoState);
        call->SetAutoAnswerState(false);
        if (ret != TELEPHONY_SUCCESS) {
            return;
        }
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyIncomingCallAnswered(call);
        return;
    }
    sptr<CallBase> incomingCall = GetOneCallObject(callId);
    int32_t waitingCallNum = GetCallNum(TelCallState::CALL_STATUS_WAITING);
    int32_t activeCallNum = GetCallNum(TelCallState::CALL_STATUS_ACTIVE);
    int32_t callNum = 4;
    for (int32_t otherCallId : callIdList) {
        sptr<CallBase> call = GetOneCallObject(otherCallId);
        if (call != nullptr && incomingCall != nullptr && call != incomingCall) {
            if (HandleDsdaIncomingCall(call, activeCallNum, slotId, videoState, incomingCall)) {
                continue;
            }
            if (call->GetSlotId() != slotId) {
                TELEPHONY_LOGI("exist other slot call");
                noOtherCall = false;
            }
            int32_t currentCallNum = GetCurrentCallNum();
            if (waitingCallNum > 1 || currentCallNum == callNum) {
                HandleCallWaitingNumTwo(incomingCall, call, slotId, activeCallNum, flagForConference);
            } else if (waitingCallNum == 1) {
                HandleCallWaitingNumOne(incomingCall, call, slotId, activeCallNum, flagForConference);
            } else {
                HandleCallWaitingNumZero(incomingCall, call, slotId, activeCallNum, flagForConference);
            }
        }
    }
}

bool CallRequestProcess::HandleDsdaIncomingCall(
    sptr<CallBase> call, int32_t activeCallNum, int32_t slotId, int32_t videoState, sptr<CallBase> incomingCall)
{
    int32_t alertingCallNum = GetCallNum(TelCallState::CALL_STATUS_ALERTING);
    int32_t dialingCallNum = GetCallNum(TelCallState::CALL_STATUS_DIALING);
    int32_t answeredCallNum = GetCallNum(TelCallState::CALL_STATUS_ANSWERED);
    if ((call->GetTelCallState() == TelCallState::CALL_STATUS_DISCONNECTING ||
            call->GetTelCallState() == TelCallState::CALL_STATUS_HOLDING) &&
        (activeCallNum == 0 && alertingCallNum == 0 && dialingCallNum == 0 && answeredCallNum == 0)) {
        if (call->GetSlotId() != slotId) {
            TELEPHONY_LOGI("enter HandleDsdaIncomingCall");
            incomingCall->AnswerCall(videoState);
            incomingCall->SetAutoAnswerState(false);
        }
        return true;
    }
    return false;
}

void CallRequestProcess::IsExistCallOtherSlot(std::list<int32_t> &list, int32_t slotId, bool &noOtherCall)
{
    if (list.size() > 1) {
        for (int32_t otherCallId : list) {
            sptr<CallBase> call = GetOneCallObject(otherCallId);
            if (call != nullptr && call->GetSlotId() != slotId) {
                noOtherCall = false;
                break;
            }
        }
    }
}

void CallRequestProcess::HandleCallWaitingNumTwo(
    sptr<CallBase> incomingCall, sptr<CallBase> call, int32_t slotId, int32_t activeCallNum, bool &flagForConference)
{
    TELEPHONY_LOGI("enter HandleCallWaitingNumTwo");
    sptr<CallBase> holdCall = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
    int32_t callNum = 3;
    int32_t currentCallNum = GetCurrentCallNum();
    if (currentCallNum == callNum) {
        TELEPHONY_LOGI("enter two waitingCall process");
        HandleCallWaitingNumOne(incomingCall, call, slotId, activeCallNum, flagForConference);
        return;
    }
    if (holdCall != nullptr) {
        TELEPHONY_LOGI("enter two holdcall hangup");
        holdCall->HangUpCall();
    }
    TELEPHONY_LOGI("enter two GetTelCallState =:%{public}d", call->GetCallRunningState());
    if (call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_ACTIVE && !flagForConference) {
        if (call->GetSlotId() == slotId) {
            TELEPHONY_LOGI("enter two activecall hold");
            call->HoldCall();
            flagForConference = true;
        } else {
            TELEPHONY_LOGI(" enter two  activecall hangup");
            call->HangUpCall();
        }
    }
}

void CallRequestProcess::HandleCallWaitingNumOne(
    sptr<CallBase> incomingCall, sptr<CallBase> call, int32_t slotId, int32_t activeCallNum, bool &flagForConference)
{
    TELEPHONY_LOGI("enter HandleCallWaitingNumOne");
    sptr<CallBase> holdCall = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
    TELEPHONY_LOGI("enter one GetTelCallState =:%{public}d", call->GetTelCallState());
    int32_t callNum = 2;
    int32_t currentCallNum = GetCurrentCallNum();
    if (holdCall != nullptr) {
        HandleCallWaitingNumOneNext(incomingCall, call, holdCall, slotId, flagForConference);
    } else if (currentCallNum == callNum) {
        TELEPHONY_LOGI("enter two call process");
        HandleCallWaitingNumZero(incomingCall, call, slotId, activeCallNum, flagForConference);
    } else if (call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_ACTIVE && !flagForConference) {
        if (call->GetSlotId() != slotId && g_flagForDsda == true) {
            TELEPHONY_LOGI("enter one hold call is null active call hold for Special Dsda Scenario");
            call->HoldCall();
            flagForConference = true;
            g_flagForDsda = false;
        } else if (call->GetSlotId() != slotId) {
            TELEPHONY_LOGI("enter one hold call is null active call hangup");
            call->HangUpCall();
        } else {
            TELEPHONY_LOGI("enter one hold call is null active call hold");
            call->HoldCall();
            flagForConference = true;
        }
    }
}

void CallRequestProcess::HandleCallWaitingNumOneNext(
    sptr<CallBase> incomingCall, sptr<CallBase> call, sptr<CallBase> holdCall, int32_t slotId, bool &flagForConference)
{
    TELEPHONY_LOGI("enter HandleCallWaitingNumOneNext");
    int32_t activeCallNum = GetCallNum(TelCallState::CALL_STATUS_ACTIVE);
    int32_t dialingCallNum = GetCallNum(TelCallState::CALL_STATUS_DIALING);
    int32_t alertingCallNum = GetCallNum(TelCallState::CALL_STATUS_ALERTING);
    if (call->GetTelCallState() == TelCallState ::CALL_STATUS_DIALING ||
        call->GetTelCallState() == TelCallState ::CALL_STATUS_ALERTING) {
        TELEPHONY_LOGI("enter one dialing call hangup");
        call->HangUpCall();
    } else if (activeCallNum > 0) {
        TELEPHONY_LOGI("enter one hold call hangup");
        holdCall->HangUpCall();
        g_flagForDsda = true;
    }
    if (call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_ACTIVE && !flagForConference) {
        if (CallObjectManager::IsCallExist(call->GetCallType(), TelCallState::CALL_STATUS_INCOMING) &&
            call->GetSlotId() != slotId) {
            TELEPHONY_LOGI("enter one active call hangup");
            call->HangUpCall();
        } else {
            TELEPHONY_LOGI("enter one active call hold");
            call->HoldCall();
            flagForConference = true;
        }
    } else if (activeCallNum == 0 && incomingCall->GetAutoAnswerState() && alertingCallNum == 0 &&
               dialingCallNum == 0) {
        TELEPHONY_LOGI("enter one active with two incoming call");
        incomingCall->AnswerCall(static_cast<int32_t>(incomingCall->GetVideoStateType()));
        incomingCall->SetAutoAnswerState(false);
    }
}

void CallRequestProcess::HandleCallWaitingNumZero(
    sptr<CallBase> incomingCall, sptr<CallBase> call, int32_t slotId, int32_t activeCallNum, bool &flagForConference)
{
    TELEPHONY_LOGI("enter HandleCallWaitingNumZero");
    sptr<CallBase> holdCall = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
    if (holdCall != nullptr) {
        TELEPHONY_LOGI("enter zero holdcall is not null");
        if (call->GetTelCallState() == TelCallState ::CALL_STATUS_DIALING ||
            call->GetTelCallState() == TelCallState ::CALL_STATUS_ALERTING) {
            TELEPHONY_LOGI("enter zero dialing call hangup");
            call->HangUpCall();
        } else if (activeCallNum > 0) {
            TELEPHONY_LOGI("enter zero hold call hangup");
            holdCall->HangUpCall();
        }
        if (call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_ACTIVE && !flagForConference) {
            TELEPHONY_LOGI("enter active call hangup");
            call->HoldCall();
            flagForConference = true;
        }
    } else {
        TELEPHONY_LOGI("enter zero holdcall is null");
        if (call->GetTelCallState() == TelCallState ::CALL_STATUS_DIALING ||
            call->GetTelCallState() == TelCallState ::CALL_STATUS_ALERTING) {
            TELEPHONY_LOGI("enter zero dialing incoming call hangup");
            call->HangUpCall();
        } else if (call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_ACTIVE && !flagForConference) {
            sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(call.GetRefPtr());
            if (imsCall != nullptr && imsCall->IsVoiceModifyToVideo()) {
                TELEPHONY_LOGI("hangup call during voice to video when answerCall");
                call->HangUpCall();
                return;
            }
            TELEPHONY_LOGI("enter zero active call hold");
            call->HoldCall();
            flagForConference = true;
        } else if (incomingCall->GetAutoAnswerState() &&
                   (call->GetTelCallState() == TelCallState ::CALL_STATUS_INCOMING ||
                       call->GetTelCallState() == TelCallState ::CALL_STATUS_WAITING)) {
            TELEPHONY_LOGI("enter two incoming call active");
            incomingCall->AnswerCall(static_cast<int32_t>(incomingCall->GetVideoStateType()));
            incomingCall->SetAutoAnswerState(false);
        }
    }
}

void CallRequestProcess::DisconnectOtherSubIdCall(int32_t callId, int32_t slotId, int32_t videoState)
{
    sptr<CallBase> incomingCall = GetOneCallObject(callId);
    if (incomingCall == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    std::list<int32_t> callIdList;
    bool noOtherCall = true;
    GetCarrierCallList(callIdList);
    if (callIdList.size() > 1) {
        for (int32_t otherCallId : callIdList) {
            sptr<CallBase> call = GetOneCallObject(otherCallId);
            if (call != nullptr && call->GetSlotId() != slotId) {
                incomingCall->SetAutoAnswerState(true);
                TELEPHONY_LOGI("Hangup call callid:%{public}d", call->GetCallID());
                call->HangUpCall();
                noOtherCall = false;
            }
        }
    }
    if (noOtherCall == true) {
        int32_t ret = incomingCall->AnswerCall(videoState);
        if (ret != TELEPHONY_SUCCESS) {
            return;
        }
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyIncomingCallAnswered(incomingCall);
    }
}

void CallRequestProcess::DisconnectOtherCallForVideoCall(int32_t callId)
{
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    for (int32_t otherCallId : callIdList) {
        sptr<CallBase> call = GetOneCallObject(otherCallId);
        if (call != nullptr && call->GetCallID() != callId &&
            (call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_ACTIVE ||
                call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_DIALING ||
                call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_HOLD)) {
            TELEPHONY_LOGI("Hangup call callid:%{public}d", call->GetCallID());
            call->HangUpCall();
        }
    }
}

void CallRequestProcess::RejectRequest(int32_t callId, bool isSendSms, std::string &content)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }

    int32_t ret = call->RejectCall();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RejectCall failed!");
        return;
    }
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    sptr<CallBase> holdCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
    if (holdCall) {
        TELEPHONY_LOGI("release the incoming/waiting call but can not recover the held call");
        holdCall->SetCanUnHoldState(false);
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
        if (HangUpForDsdaRequest(call)) {
            TELEPHONY_LOGI("hangup for dsda Request success");
        } else {
            TELEPHONY_LOGI("release the active call and recover the held call");
            call->SetPolicyFlag(PolicyFlag::POLICY_FLAG_HANG_UP_ACTIVE);
        }
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
}

bool CallRequestProcess::HangUpForDsdaRequest(sptr<CallBase> call)
{
    int32_t dsdsMode = DSDS_MODE_V2;
    DelayedRefSingleton<CoreServiceClient>::GetInstance().GetDsdsMode(dsdsMode);
    bool noOtherCall = true;
    std::list<int32_t> allCallIdList;
    GetCarrierCallList(allCallIdList);
    IsExistCallOtherSlot(allCallIdList, call->GetSlotId(), noOtherCall);
    if ((dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_DSDA) ||
            dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_TDM)) &&
        (!noOtherCall)) {
        TELEPHONY_LOGI("release the active call but not recover the held call for dsda");
        std::vector<std::u16string> callIdList;
        call->GetSubCallIdList(callIdList);
        for (auto it = callIdList.begin(); it != callIdList.end(); ++it) {
            int32_t callId = -1;
            StrToInt(Str16ToStr8(*it), callId);
            KickOutFromConferenceRequest(callId);
        }
        return true;
    }
    return false;
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
    TELEPHONY_LOGI("Enter UnHoldRequest");
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    call->SetCanUnHoldState(true);
    bool noOtherCall = true;
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    IsExistCallOtherSlot(callIdList, call->GetSlotId(), noOtherCall);
    for (int32_t otherCallId : callIdList) {
        sptr<CallBase> otherCall = GetOneCallObject(otherCallId);
        if (otherCall == nullptr) {
            TELEPHONY_LOGE("otherCall is nullptr");
            return;
        }
        TelCallState state = otherCall->GetTelCallState();
        TelConferenceState confState = otherCall->GetTelConferenceState();
        int32_t conferenceId = ERR_ID;
        otherCall->GetMainCallId(conferenceId);
        TELEPHONY_LOGI("otherCall->GetTelCallState(): %{public}d ,callid:%{public}d", state, otherCallId);
        if (IsDsdsMode5() && !noOtherCall && state == TelCallState::CALL_STATUS_ACTIVE &&
            ((confState != TelConferenceState::TEL_CONFERENCE_IDLE && conferenceId == otherCallId) ||
                confState == TelConferenceState::TEL_CONFERENCE_IDLE)) {
            if (otherCall->GetCanSwitchCallState()) {
                TELEPHONY_LOGI("Hold other call in other slotId for switch Dsda call");
                otherCall->SetCanSwitchCallState(false);
                otherCall->HoldCall();
                return;
            } else {
                TELEPHONY_LOGI("Currently can not swap this call");
                return;
            }
        }
    }
    if (noOtherCall) {
        call->UnHoldCall();
    }
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
    callDetatilInfo.originalCallType = info.originalCallType;
    callDetatilInfo.voiceDomain = static_cast<int32_t>(info.callType);
    callDetatilInfo.phoneOrWatch = info.phoneOrWatch;
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
    sptr<CallBase> call = nullptr;
    while (!isFirstDialCallAdded_) {
        if (cv_.wait_for(lock, std::chrono::seconds(WAIT_TIME_ONE_SECOND)) == std::cv_status::timeout) {
            TELEPHONY_LOGE("CarrierDialProcess call is not added");
            SelectDialFailCall(call);
            if (call != nullptr) {
                DealFailDial(call);
            }
            return CALL_ERR_DIAL_FAILED;
        }
    }
    SelectDialFailCall(call);
    if (call != nullptr) {
        return DealFailDial(call);
    }
    TELEPHONY_LOGE("can not find connect call or dialing call");
    return CALL_ERR_CALL_STATE;
}

void CallRequestProcess::SelectDialFailCall(sptr<CallBase> &call)
{
    call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_CREATE);
    if (call != nullptr) {
        return;
    }
    call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_CONNECTING);
    if (call != nullptr) {
        return;
    }
    call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING);
    return;
}

int32_t CallRequestProcess::CarrierDialProcess(DialParaInfo &info)
{
    auto callRequestEventHandler = DelayedSingleton<CallRequestEventHandlerHelper>::GetInstance();
    if (callRequestEventHandler->IsDialingCallProcessing()) {
        return CALL_ERR_CALL_COUNTS_EXCEED_LIMIT;
    }
    callRequestEventHandler->RestoreDialingFlag(true);
    callRequestEventHandler->SetDialingCallProcessing();
    std::string newPhoneNum =
        DelayedSingleton<CallNumberUtils>::GetInstance()->RemoveSeparatorsPhoneNumber(info.number);
    int32_t ret = HandleDialingInfo(newPhoneNum, info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HandleDialingInfo failed!");
        callRequestEventHandler->RestoreDialingFlag(false);
        callRequestEventHandler->RemoveEventHandlerTask();
        needWaitHold_ = false;
        return ret;
    }
    std::string tempNumber = info.number;
    bool isMMiCode = false;
    if (!info.isEcc) {
        isMMiCode = DelayedSingleton<CellularCallConnection>::GetInstance()->IsMmiCode(info.accountId, newPhoneNum);
    }
    if (!isMMiCode) {
        isFirstDialCallAdded_ = false;
        info.number = newPhoneNum;
        ret = UpdateCallReportInfo(info, TelCallState::CALL_STATUS_DIALING);
        info.number = tempNumber;
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("UpdateCallReportInfo failed!");
            needWaitHold_ = false;
            return ret;
        }
    } else {
        callRequestEventHandler->RestoreDialingFlag(false);
        callRequestEventHandler->RemoveEventHandlerTask();
        TELEPHONY_LOGI("CarrierDialProcess: isMMiCode is true!");
        DelayedSingleton<CallDialog>::GetInstance()->DialogProcessMMICodeExtension();
    }
    return HandleStartDial(isMMiCode, info);
}

int32_t CallRequestProcess::HandleDialingInfo(std::string newPhoneNum, DialParaInfo &info)
{
    bool isEcc = false;
    int32_t ret = HandleEccCallForDsda(newPhoneNum, info, isEcc);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!isEcc) {
        bool canDial = true;
        IsNewCallAllowedCreate(canDial);
        if (!canDial) {
            return CALL_ERR_CALL_COUNTS_EXCEED_LIMIT;
        }
        if (IsDsdsMode5()) {
            ret = IsDialCallForDsda(info);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("IsDialCallForDsda failed!");
                return ret;
            }
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestProcess::HandleStartDial(bool isMMiCode, DialParaInfo &info)
{
    CellularCallInfo callInfo;
    int32_t ret = PackCellularCallInfo(info, callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("PackCellularCallInfo failed!");
        CallManagerHisysevent::WriteDialCallFaultEvent(info.accountId, static_cast<int32_t>(info.callType),
            static_cast<int32_t>(info.videoState), ret, "Carrier type PackCellularCallInfo failed");
        needWaitHold_ = false;
        return ret;
    }
    if (needWaitHold_ && !isMMiCode) {
        TELEPHONY_LOGI("waitting for call hold");
        dialCallInfo_ = callInfo;
        return ret;
    }
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

int32_t CallRequestProcess::IsDialCallForDsda(DialParaInfo &info)
{
    sptr<CallBase> activeCall = GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    TELEPHONY_LOGI("Is dsdsmode5 dial call info.accountId = %{public}d", info.accountId);
    if (activeCall != nullptr && activeCall->GetSlotId() != info.accountId) {
        int32_t ret = activeCall->HoldCall();
        if (ret == TELEPHONY_SUCCESS) {
            needWaitHold_ = true;
        }
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestProcess::HandleEccCallForDsda(std::string newPhoneNum, DialParaInfo &info, bool &isEcc)
{
    int32_t ret =
        DelayedSingleton<CallNumberUtils>::GetInstance()->CheckNumberIsEmergency(newPhoneNum, info.accountId, isEcc);
    TELEPHONY_LOGE("CheckNumberIsEmergency ret is %{public}d, isEcc: %{public}d", ret, isEcc);
    if (isEcc) {
        return EccDialPolicy();
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
    std::string withoutSpaceNumber = phoneNumber;
    withoutSpaceNumber.erase(std::remove(withoutSpaceNumber.begin(),
        withoutSpaceNumber.end(), ' '), withoutSpaceNumber.end());
    for (const auto& fdnNumber : fdnNumberList) {
        if (withoutSpaceNumber.compare(Str16ToStr8(fdnNumber)) == 0) {
            TELEPHONY_LOGI("you are allowed to dial!");
            return true;
        }
    }
    TELEPHONY_LOGW("There is no fixed number.");
    return false;
}

int32_t CallRequestProcess::EccDialPolicy()
{
    std::list<int32_t> callIdList;
    std::list<sptr<CallBase>> hangupList;
    std::list<sptr<CallBase>> rejectList;
    GetCarrierCallList(callIdList);
    for (int32_t callId : callIdList) {
        sptr<CallBase> call = GetOneCallObject(callId);
        if (call == nullptr) {
            continue;
        }
        CallRunningState crState = call->GetCallRunningState();
        if (call->GetCallType() == CallType::TYPE_SATELLITE) {
            hangupList.emplace_back(call);
        } else if (crState == CallRunningState::CALL_RUNNING_STATE_CREATE ||
            crState == CallRunningState::CALL_RUNNING_STATE_CONNECTING ||
            crState == CallRunningState::CALL_RUNNING_STATE_DIALING) {
            if (call->GetEmergencyState()) {
                hangupList.clear();
                rejectList.clear();
                TELEPHONY_LOGE("already has ecc call dailing!");
                return CALL_ERR_CALL_COUNTS_EXCEED_LIMIT;
            }
            hangupList.emplace_back(call);
        } else if (crState == CallRunningState::CALL_RUNNING_STATE_RINGING) {
            rejectList.emplace_back(call);
        } else if (crState == CallRunningState::CALL_RUNNING_STATE_ACTIVE ||
            crState == CallRunningState::CALL_RUNNING_STATE_HOLD) {
            hangupList.emplace_back(call);
        }
    }
    for (sptr<CallBase> call : hangupList) {
        int32_t callId = call->GetCallID();
        CallRunningState crState = call->GetCallRunningState();
        TELEPHONY_LOGE("HangUpCall call[id:%{public}d crState:%{public}d]", callId, crState);
        call->HangUpCall();
    }
    for (sptr<CallBase> call : rejectList) {
        int32_t callId = call->GetCallID();
        CallRunningState crState = call->GetCallRunningState();
        TELEPHONY_LOGE("RejectCall call[id:%{public}d crState:%{public}d]", callId, crState);
        call->RejectCall();
    }
    hangupList.clear();
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestProcess::BluetoothDialProcess(DialParaInfo &info)
{
    TELEPHONY_LOGI("CallRequestProcess BluetoothDialProcess start");
    if (CallObjectManager::HasCallExist()) {
        TELEPHONY_LOGW("BluetoothCall Dial has Call Exist.");
        return CALL_ERR_CALL_COUNTS_EXCEED_LIMIT;
    }
    auto callRequestEventHandler = DelayedSingleton<CallRequestEventHandlerHelper>::GetInstance();
    if (callRequestEventHandler->IsDialingCallProcessing()) {
        TELEPHONY_LOGW("BluetoothCall Dial has Call Exist.");
        return CALL_ERR_CALL_COUNTS_EXCEED_LIMIT;
    }
    callRequestEventHandler->RestoreDialingFlag(true);
    callRequestEventHandler->SetDialingCallProcessing();
    int32_t ret = DelayedSingleton<BluetoothCallConnection>::GetInstance()->Dial(info);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("BluetoothCall Dial Success.");
        info.index = INIT_INDEX;
        ret = UpdateCallReportInfo(info, TelCallState::CALL_STATUS_DIALING);
    } else {
        TELEPHONY_LOGE("BluetoothCall Dial failed. errorcode=%{public}d", ret);
        callRequestEventHandler->RestoreDialingFlag(false);
        callRequestEventHandler->RemoveEventHandlerTask();
    }
    return ret;
}
} // namespace Telephony
} // namespace OHOS
