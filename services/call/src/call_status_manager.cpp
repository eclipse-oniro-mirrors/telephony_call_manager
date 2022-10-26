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

#include "call_status_manager.h"

#include <securec.h>

#include "audio_control_manager.h"
#include "bluetooth_call_service.h"
#include "call_control_manager.h"
#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "cs_call.h"
#include "hitrace_meter.h"
#include "ims_call.h"
#include "ott_call.h"
#include "report_call_info_handler.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallStatusManager::CallStatusManager()
{
    (void)memset_s(&callReportInfo_, sizeof(CallDetailInfo), 0, sizeof(CallDetailInfo));
    (void)memset_s(&callDetailsInfo_, sizeof(CallDetailsInfo), 0, sizeof(CallDetailsInfo));
}

CallStatusManager::~CallStatusManager()
{
    UnInit();
}

int32_t CallStatusManager::Init()
{
    callDetailsInfo_.callVec.clear();
    mEventIdTransferMap_.clear();
    mOttEventIdTransferMap_.clear();
    InitCallBaseEvent();
    CallIncomingFilterManagerPtr_ = (std::make_unique<CallIncomingFilterManager>()).release();
    return TELEPHONY_SUCCESS;
}

void CallStatusManager::InitCallBaseEvent()
{
    mEventIdTransferMap_[RequestResultEventId::RESULT_DIAL_NO_CARRIER] = CallAbilityEventId::EVENT_DIAL_NO_CARRIER;
    mOttEventIdTransferMap_[OttCallEventId::OTT_CALL_EVENT_FUNCTION_UNSUPPORTED] =
        CallAbilityEventId::EVENT_OTT_FUNCTION_UNSUPPORTED;
}

int32_t CallStatusManager::UnInit()
{
    callDetailsInfo_.callVec.clear();
    mEventIdTransferMap_.clear();
    mOttEventIdTransferMap_.clear();
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::HandleCallReportInfo(const CallDetailInfo &info)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    callReportInfo_ = info;
    switch (info.state) {
        case TelCallState::CALL_STATUS_ACTIVE:
            ret = ActiveHandle(info);
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            ret = HoldingHandle(info);
            break;
        case TelCallState::CALL_STATUS_DIALING: {
            ret = DialingHandle(info);
            FinishAsyncTrace(HITRACE_TAG_OHOS, "DialCall", getpid());
            DelayedSingleton<CallManagerHisysevent>::GetInstance()->JudgingDialTimeOut(
                info.accountId, static_cast<int32_t>(info.callType), static_cast<int32_t>(info.callMode));
            break;
        }
        case TelCallState::CALL_STATUS_ALERTING:
            ret = AlertHandle(info);
            break;
        case TelCallState::CALL_STATUS_INCOMING: {
            ret = IncomingHandle(info);
            FinishAsyncTrace(HITRACE_TAG_OHOS, "InComingCall", getpid());
            DelayedSingleton<CallManagerHisysevent>::GetInstance()->JudgingIncomingTimeOut(
                info.accountId, static_cast<int32_t>(info.callType), static_cast<int32_t>(info.callMode));
            break;
        }
        case TelCallState::CALL_STATUS_WAITING:
            ret = WaitingHandle(info);
            break;
        case TelCallState::CALL_STATUS_DISCONNECTED:
            ret = DisconnectedHandle(info);
            break;
        case TelCallState::CALL_STATUS_DISCONNECTING:
            ret = DisconnectingHandle(info);
            break;
        default:
            TELEPHONY_LOGE("Invalid call state!");
            break;
    }
    TELEPHONY_LOGI("Entry CallStatusManager HandleCallReportInfo");
    DelayedSingleton<BluetoothCallService>::GetInstance()->GetCallState();
    return ret;
}

// handle call state changes, incoming call, outgoing call.
int32_t CallStatusManager::HandleCallsReportInfo(const CallDetailsInfo &info)
{
    bool flag = false;
    TELEPHONY_LOGI("call list size:%{public}zu,slotId:%{public}d", info.callVec.size(), info.slotId);
    for (auto &it : info.callVec) {
        for (auto &it1 : callDetailsInfo_.callVec) {
            if (strcmp(it.phoneNum, it1.phoneNum) == 0) {
                // call state changes
                if (it.state != it1.state) {
                    TELEPHONY_LOGI("handle updated call state:%{public}d", it.state);
                    HandleCallReportInfo(it);
                }
                flag = true;
                break;
            }
        }
        // incoming/outgoing call handle
        if (!flag || callDetailsInfo_.callVec.empty()) {
            TELEPHONY_LOGI("handle new call state:%{public}d", it.state);
            HandleCallReportInfo(it);
        }
        flag = false;
    }
    // disconnected calls handle
    for (auto &it2 : callDetailsInfo_.callVec) {
        for (auto &it3 : info.callVec) {
            if (strcmp(it2.phoneNum, it3.phoneNum) == 0) {
                TELEPHONY_LOGI("state:%{public}d", it2.state);
                flag = true;
                break;
            }
        }
        if (!flag) {
            it2.state = TelCallState::CALL_STATUS_DISCONNECTED;
            HandleCallReportInfo(it2);
        }
        flag = false;
    }
    callDetailsInfo_.callVec.clear();
    callDetailsInfo_ = info;
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::HandleDisconnectedCause(const DisconnectedDetails &details)
{
    bool ret = DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallDestroyed(details);
    if (!ret) {
        TELEPHONY_LOGI("NotifyCallDestroyed failed!");
        return CALL_ERR_PHONE_CALLSTATE_NOTIFY_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::HandleEventResultReportInfo(const CellularCallEventInfo &info)
{
    if (info.eventType != CellularCallEventType::EVENT_REQUEST_RESULT_TYPE) {
        TELEPHONY_LOGE("unexpected type event occurs, eventId:%{public}d", info.eventId);
        return CALL_ERR_PHONE_TYPE_UNEXPECTED;
    }
    TELEPHONY_LOGI("recv one Event, eventId:%{public}d", info.eventId);
    CallEventInfo eventInfo;
    (void)memset_s(&eventInfo, sizeof(CallEventInfo), 0, sizeof(CallEventInfo));
    if (mEventIdTransferMap_.find(info.eventId) != mEventIdTransferMap_.end()) {
        eventInfo.eventId = mEventIdTransferMap_[info.eventId];
        DialParaInfo dialInfo;
        if (eventInfo.eventId == CallAbilityEventId::EVENT_DIAL_NO_CARRIER) {
            DelayedSingleton<CallControlManager>::GetInstance()->GetDialParaInfo(dialInfo);
            if (memcpy_s(eventInfo.phoneNum, kMaxNumberLen, dialInfo.number.c_str(), dialInfo.number.length()) != EOK) {
                TELEPHONY_LOGE("memcpy_s failed!");
                return TELEPHONY_ERR_MEMCPY_FAIL;
            }
        }
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallEventUpdated(eventInfo);
    } else {
        TELEPHONY_LOGW("unknown type Event, eventid %{public}d", info.eventId);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::HandleOttEventReportInfo(const OttCallEventInfo &info)
{
    TELEPHONY_LOGI("recv one Event, eventId:%{public}d", info.ottCallEventId);
    CallEventInfo eventInfo;
    (void)memset_s(&eventInfo, sizeof(CallEventInfo), 0, sizeof(CallEventInfo));
    if (mOttEventIdTransferMap_.find(info.ottCallEventId) != mOttEventIdTransferMap_.end()) {
        eventInfo.eventId = mOttEventIdTransferMap_[info.ottCallEventId];
        if (memcpy_s(eventInfo.bundleName, kMaxNumberLen, info.bundleName, strlen(info.bundleName)) != EOK) {
            TELEPHONY_LOGE("memcpy_s failed!");
            return TELEPHONY_ERR_MEMCPY_FAIL;
        }
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallEventUpdated(eventInfo);
    } else {
        TELEPHONY_LOGW("unknown type Event, eventid %{public}d", info.ottCallEventId);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::IncomingHandle(const CallDetailInfo &info)
{
    int32_t ret = IncomingHandlePolicy(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("IncomingHandlePolicy failed!");
        if (info.state == TelCallState::CALL_STATUS_INCOMING) {
            CallManagerHisysevent::WriteIncomingCallFaultEvent(info.accountId, static_cast<int32_t>(info.callType),
                static_cast<int32_t>(info.callMode), ret, "IncomingHandlePolicy failed");
        }
        return ret;
    }
    if (info.callType == CallType::TYPE_CS || info.callType == CallType::TYPE_IMS) {
        ret = IncomingFilterPolicy(info);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("IncomingFilterPolicy failed!");
            return ret;
        }
    }
    sptr<CallBase> call = CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateNewCall failed!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }

    // allow list filtering
    // Get the contact data from the database
    ContactInfo contactInfo = {
        .name = "",
        .number = "",
        .isContacterExists = false,
        .ringtonePath = "",
        .isSendToVoicemail = false,
        .isEcc = false,
        .isVoiceMail = false,
    };
    QueryCallerInfo(contactInfo, std::string(info.phoneNum));
    call->SetCallerInfo(contactInfo);

    DelayedSingleton<CallControlManager>::GetInstance()->NotifyNewCallCreated(call);
    ret = UpdateCallState(call, info.state);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed!");
        return ret;
    }
    ret = FilterResultsDispose(call);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("FilterResultsDispose failed!");
    }
    return ret;
}

void CallStatusManager::QueryCallerInfo(ContactInfo &contactInfo, std::string phoneNum)
{
    TELEPHONY_LOGI("Entry CallStatusManager QueryCallerInfo");
    std::shared_ptr<CallDataBaseHelper> callDataPtr = DelayedSingleton<CallDataBaseHelper>::GetInstance();
    if (callDataPtr == nullptr) {
        TELEPHONY_LOGE("callDataPtr is nullptr!");
        return;
    }
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(DETAIL_INFO, phoneNum);
    predicates.And();
    predicates.EqualTo(CONTENT_TYPE, PHONE);
    bool ret = callDataPtr->Query(contactInfo, predicates);
    if (!ret) {
        TELEPHONY_LOGE("Query contact database fail!");
    }
}

int32_t CallStatusManager::IncomingFilterPolicy(const CallDetailInfo &info)
{
    if (CallIncomingFilterManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallIncomingFilterManagerPtr_ is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (CallIncomingFilterManagerPtr_->IsFirstIncoming()) {
        CallIncomingFilterManagerPtr_->UpdateIncomingFilterData();
    }
    return CallIncomingFilterManagerPtr_->doIncomingFilter(info);
}

void CallStatusManager::CallFilterCompleteResult(const CallDetailInfo &info)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    sptr<CallBase> call = CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateNewCall failed!");
        return;
    }
#ifdef ABILITY_DATABASE_SUPPORT
    // allow list filtering
    // Get the contact data from the database
    GetCallerInfoDate(ContactInfo);
    SetCallerInfo(contactInfo);
#endif
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyNewCallCreated(call);
    ret = UpdateCallState(call, info.state);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed!");
        return;
    }
    ret = FilterResultsDispose(call);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("FilterResultsDispose failed!");
        return;
    }
}

int32_t CallStatusManager::DialingHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle dialing state");
    int32_t ret = DialingHandlePolicy(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("DialingHandlePolicy failed!");
        return ret;
    }
    sptr<CallBase> call = CreateNewCall(info, CallDirection::CALL_DIRECTION_OUT);
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateNewCall failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = call->DialingProcess();
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyNewCallCreated(call);
    ret = UpdateCallState(call, TelCallState::CALL_STATUS_DIALING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
    }
    return ret;
}

int32_t CallStatusManager::ActiveHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle active state");
    std::string tmpStr(info.phoneNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    call = RefreshCallIfNecessary(call, info);
    // call state change active, need to judge if launching a conference
    int32_t ret = call->LaunchConference();
    if (ret == TELEPHONY_SUCCESS) {
        int32_t mainCallId = call->GetMainCallId();
        sptr<CallBase> mainCall = GetOneCallObject(mainCallId);
        if (mainCall != nullptr) {
            mainCall->SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
        }
    }
    ret = UpdateCallState(call, TelCallState::CALL_STATUS_ACTIVE);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
        return ret;
    }
#ifdef AUDIO_SUPPORT
    ToSpeakerPhone(call);
    DelayedSingleton<AudioControlManager>::GetInstance()->SetVolumeAudible();
#endif
    TELEPHONY_LOGI("handle active state success");
    return ret;
}

int32_t CallStatusManager::HoldingHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle holding state");
    std::string tmpStr(info.phoneNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    // if the call is in a conference, it will exit, otherwise just set it holding
    call = RefreshCallIfNecessary(call, info);
    int32_t ret = call->HoldConference();
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("HoldConference success");
    }
    ret = UpdateCallState(call, TelCallState::CALL_STATUS_HOLDING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
    }
    return ret;
}

int32_t CallStatusManager::WaitingHandle(const CallDetailInfo &info)
{
    return IncomingHandle(info);
}

int32_t CallStatusManager::AlertHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle alerting state");
    std::string tmpStr(info.phoneNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    call = RefreshCallIfNecessary(call, info);
    int32_t ret = UpdateCallState(call, TelCallState::CALL_STATUS_ALERTING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
        return ret;
    }
#ifdef AUDIO_SUPPORT
    ToSpeakerPhone(call);
    TurnOffMute(call);
    DelayedSingleton<AudioControlManager>::GetInstance()->SetVolumeAudible();
#endif
    return ret;
}

int32_t CallStatusManager::DisconnectingHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle disconnecting state");
    std::string tmpStr(info.phoneNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    call = RefreshCallIfNecessary(call, info);
    int32_t ret = UpdateCallState(call, TelCallState::CALL_STATUS_DISCONNECTING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
    }
    return ret;
}

int32_t CallStatusManager::DisconnectedHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle disconnected state");
    std::string tmpStr(info.phoneNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    call = RefreshCallIfNecessary(call, info);
    int32_t ret = call->ExitConference();
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("SubCallSeparateFromConference success");
    }
    ret = UpdateCallState(call, TelCallState::CALL_STATUS_DISCONNECTED);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
        return ret;
    }
    DeleteOneCallObject(call->GetCallID());
    return ret;
}

int32_t CallStatusManager::UpdateCallState(sptr<CallBase> &call, TelCallState nextState)
{
    TELEPHONY_LOGI("UpdateCallState start");
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    TelCallState priorState = call->GetTelCallState();
    TELEPHONY_LOGI("priorState:%{public}d, nextState:%{public}d", priorState, nextState);
    if (priorState == TelCallState::CALL_STATUS_INCOMING && nextState == TelCallState::CALL_STATUS_ACTIVE) {
        DelayedSingleton<CallManagerHisysevent>::GetInstance()->JudgingAnswerTimeOut(
            call->GetSlotId(), call->GetCallID(), static_cast<int32_t>(call->GetVideoStateType()));
    }
    // need DTMF judge
    int32_t ret = call->SetTelCallState(nextState);
    if (ret != TELEPHONY_SUCCESS && ret != CALL_ERR_NOT_NEW_STATE) {
        TELEPHONY_LOGE("SetTelCallState failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    // notify state changed
    if (!DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallStateUpdated(call, priorState, nextState)) {
        TELEPHONY_LOGE(
            "NotifyCallStateUpdated failed! priorState:%{public}d,nextState:%{public}d", priorState, nextState);
        if (nextState == TelCallState::CALL_STATUS_INCOMING) {
            CallManagerHisysevent::WriteIncomingCallFaultEvent(call->GetSlotId(),
                static_cast<int32_t>(call->GetCallType()), static_cast<int32_t>(call->GetVideoStateType()), ret,
                "NotifyCallStateUpdated failed");
        }
        return CALL_ERR_PHONE_CALLSTATE_NOTIFY_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

sptr<CallBase> CallStatusManager::RefreshCallIfNecessary(const sptr<CallBase> &call, const CallDetailInfo &info)
{
    TELEPHONY_LOGI("RefreshCallIfNecessary");
    if (call->GetCallType() == info.callType) {
        TELEPHONY_LOGI("RefreshCallIfNecessary not need Refresh");
        return call;
    }
    CallAttributeInfo attrInfo;
    (void)memset_s(&attrInfo, sizeof(CallAttributeInfo), 0, sizeof(CallAttributeInfo));
    call->GetCallAttributeBaseInfo(attrInfo);
    sptr<CallBase> newCall = CreateNewCall(info, attrInfo.callDirection);
    if (newCall == nullptr) {
        TELEPHONY_LOGE("RefreshCallIfNecessary createCallFail");
        return call;
    }
    newCall->SetCallRunningState(call->GetCallRunningState());
    newCall->SetTelConferenceState(call->GetTelConferenceState());
    newCall->SetStartTime(attrInfo.startTime);
    newCall->SetPolicyFlag(PolicyFlag(call->GetPolicyFlag()));
    newCall->SetSpeakerphoneOn(call->IsSpeakerphoneOn());
    newCall->SetCallEndedType(call->GetCallEndedType());
    newCall->SetCallEndTime(attrInfo.callBeginTime);
    newCall->SetCallEndTime(attrInfo.callEndTime);
    newCall->SetRingBeginTime(attrInfo.ringBeginTime);
    newCall->SetRingEndTime(attrInfo.ringEndTime);
    newCall->SetAnswerType(attrInfo.answerType);
    DeleteOneCallObject(call->GetCallID());
    newCall->SetCallId(call->GetCallID());
    return newCall;
}

int32_t CallStatusManager::ToSpeakerPhone(sptr<CallBase> &call)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_DIALING) {
        TELEPHONY_LOGI("Call is CALL_STATUS_DIALING");
        return ret;
    }
    if (call->IsSpeakerphoneOn()) {
        DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioDevice(AudioDevice::DEVICE_SPEAKER);
        ret = call->SetSpeakerphoneOn(false);
    }
    return ret;
}

int32_t CallStatusManager::TurnOffMute(sptr<CallBase> &call)
{
    if (call->GetEmergencyState() || HasEmergencyCall()) {
        DelayedSingleton<AudioControlManager>::GetInstance()->SetMute(false);
    } else {
        DelayedSingleton<AudioControlManager>::GetInstance()->SetMute(true);
    }
    return TELEPHONY_SUCCESS;
}

sptr<CallBase> CallStatusManager::CreateNewCall(const CallDetailInfo &info, CallDirection dir)
{
    sptr<CallBase> callPtr = nullptr;
    DialParaInfo paraInfo;
    AppExecFwk::PacMap extras;
    extras.Clear();
    PackParaInfo(paraInfo, info, dir, extras);
    switch (info.callType) {
        case CallType::TYPE_CS: {
            if (dir == CallDirection::CALL_DIRECTION_OUT) {
                callPtr = (std::make_unique<CSCall>(paraInfo, extras)).release();
            } else {
                callPtr = (std::make_unique<CSCall>(paraInfo)).release();
            }
            break;
        }
        case CallType::TYPE_IMS: {
            if (dir == CallDirection::CALL_DIRECTION_OUT) {
                callPtr = (std::make_unique<IMSCall>(paraInfo, extras)).release();
            } else {
                callPtr = (std::make_unique<IMSCall>(paraInfo)).release();
            }
            break;
        }
        case CallType::TYPE_OTT: {
            if (dir == CallDirection::CALL_DIRECTION_OUT) {
                callPtr = (std::make_unique<OTTCall>(paraInfo, extras)).release();
            } else {
                callPtr = (std::make_unique<OTTCall>(paraInfo)).release();
            }
            break;
        }
        default:
            return nullptr;
    }
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("CreateNewCall failed!");
        return nullptr;
    }
    AddOneCallObject(callPtr);
    return callPtr;
}

void CallStatusManager::PackParaInfo(
    DialParaInfo &paraInfo, const CallDetailInfo &info, CallDirection dir, AppExecFwk::PacMap &extras)
{
    paraInfo.isEcc = false;
    paraInfo.dialType = DialType::DIAL_CARRIER_TYPE;
    if (dir == CallDirection::CALL_DIRECTION_OUT) {
        DelayedSingleton<CallControlManager>::GetInstance()->GetDialParaInfo(paraInfo, extras);
    }
    paraInfo.number = info.phoneNum;
    paraInfo.callId = GetNewCallId();
    paraInfo.index = info.index;
    paraInfo.videoState = VideoStateType::TYPE_VOICE;
    paraInfo.accountId = info.accountId;
    paraInfo.callType = info.callType;
    paraInfo.callState = info.state;
    paraInfo.bundleName = info.bundleName;
}
} // namespace Telephony
} // namespace OHOS
