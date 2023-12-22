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
#include "call_number_utils.h"
#include "core_service_client.h"
#include "cs_call.h"
#include "datashare_predicates.h"
#include "hitrace_meter.h"
#include "ims_call.h"
#include "ott_call.h"
#include "report_call_info_handler.h"
#include "telephony_log_wrapper.h"
#include "voip_call.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t INIT_INDEX = 0;
CallStatusManager::CallStatusManager()
{
    (void)memset_s(&callReportInfo_, sizeof(CallDetailInfo), 0, sizeof(CallDetailInfo));
    for (int32_t i = 0; i < SLOT_NUM; i++) {
        (void)memset_s(&callDetailsInfo_[i], sizeof(CallDetailsInfo), 0, sizeof(CallDetailsInfo));
    }
}

CallStatusManager::~CallStatusManager()
{
    UnInit();
}

int32_t CallStatusManager::Init()
{
    for (int32_t i = 0; i < SLOT_NUM; i++) {
        callDetailsInfo_[i].callVec.clear();
    }
    mEventIdTransferMap_.clear();
    mOttEventIdTransferMap_.clear();
    InitCallBaseEvent();
    CallIncomingFilterManagerPtr_ = (std::make_unique<CallIncomingFilterManager>()).release();
    return TELEPHONY_SUCCESS;
}

void CallStatusManager::InitCallBaseEvent()
{
    mEventIdTransferMap_[RequestResultEventId::RESULT_DIAL_NO_CARRIER] = CallAbilityEventId::EVENT_DIAL_NO_CARRIER;
    mEventIdTransferMap_[RequestResultEventId::RESULT_HOLD_SEND_FAILED] = CallAbilityEventId::EVENT_HOLD_CALL_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_SWAP_SEND_FAILED] = CallAbilityEventId::EVENT_SWAP_CALL_FAILED;
    mOttEventIdTransferMap_[OttCallEventId::OTT_CALL_EVENT_FUNCTION_UNSUPPORTED] =
        CallAbilityEventId::EVENT_OTT_FUNCTION_UNSUPPORTED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_COMBINE_SEND_FAILED] =
        CallAbilityEventId::EVENT_COMBINE_CALL_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_SPLIT_SEND_FAILED] =
        CallAbilityEventId::EVENT_SPLIT_CALL_FAILED;
}

int32_t CallStatusManager::UnInit()
{
    for (int32_t i = 0; i < SLOT_NUM; i++) {
        callDetailsInfo_[i].callVec.clear();
    }
    mEventIdTransferMap_.clear();
    mOttEventIdTransferMap_.clear();
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::HandleCallReportInfo(const CallDetailInfo &info)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    callReportInfo_ = info;
    if (info.callType == CallType::TYPE_VOIP) {
        ret = HandleVoipCallReportInfo(info);
        return ret;
    }
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
    HandleDsdaInfo(info.accountId);
    DelayedSingleton<BluetoothCallService>::GetInstance()->GetCallState();
    return ret;
}

void CallStatusManager::HandleDsdaInfo(int32_t slotId)
{
    int32_t dsdsMode = DSDS_MODE_V2;
    bool noOtherCall = true;
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    DelayedSingleton<CallRequestProcess>::GetInstance()->IsExistCallOtherSlot(callIdList, slotId, noOtherCall);
    DelayedRefSingleton<CoreServiceClient>::GetInstance().GetDsdsMode(dsdsMode);
    if ((dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_DSDA) ||
            dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_TDM)) &&
        !noOtherCall) {
        TELEPHONY_LOGI("Handle DsdaCallInfo");
        sptr<CallBase> holdCall = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
        if (holdCall != nullptr) {
            holdCall->SetCanUnHoldState(false);
        }
    }
}

// handle call state changes, incoming call, outgoing call.
int32_t CallStatusManager::HandleCallsReportInfo(const CallDetailsInfo &info)
{
    bool flag = false;
    TELEPHONY_LOGI("call list size:%{public}zu,slotId:%{public}d", info.callVec.size(), info.slotId);
    int32_t curSlotId = info.slotId;
    if (!DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(curSlotId)) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    for (auto &it : info.callVec) {
        for (const auto &it1 : callDetailsInfo_[curSlotId].callVec) {
            if (it.index == it1.index) {
                // call state changes
                if (it.state != it1.state || it.mpty != it1.mpty ||
                    it.callType != it1.callType || it.callMode != it1.callMode) {
                    TELEPHONY_LOGI("handle updated call state:%{public}d", it.state);
                    HandleCallReportInfo(it);
                }
                flag = true;
                break;
            }
        }
        // incoming/outgoing call handle
        if (!flag || callDetailsInfo_[curSlotId].callVec.empty()) {
            TELEPHONY_LOGI("handle new call state:%{public}d", it.state);
            HandleCallReportInfo(it);
        }
        flag = false;
    }
    // disconnected calls handle
    for (auto &it2 : callDetailsInfo_[curSlotId].callVec) {
        for (const auto &it3 : info.callVec) {
            if (it2.index == it3.index) {
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
    callDetailsInfo_[curSlotId].callVec.clear();
    callDetailsInfo_[curSlotId] = info;
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::HandleVoipCallReportInfo(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("Entry CallStatusManager HandleVoipCallReportInfo");
    int32_t ret = TELEPHONY_ERR_FAIL;
    callReportInfo_ = info;
    switch (info.state) {
        case TelCallState::CALL_STATUS_ACTIVE:
            ret = ActiveVoipCallHandle(info);
            break;
        case TelCallState::CALL_STATUS_INCOMING: {
            ret = IncomingVoipCallHandle(info);
            break;
        }
        case TelCallState::CALL_STATUS_DISCONNECTED:
            ret = DisconnectedVoipCallHandle(info);
            break;
        default:
            TELEPHONY_LOGE("Invalid call state!");
            break;
    }
    DelayedSingleton<BluetoothCallService>::GetInstance()->GetCallState();
    return ret;
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
    sptr<CallBase> call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING);
    if (call != nullptr) {
        int32_t ret = DealFailDial(call);
        TELEPHONY_LOGI("DealFailDial ret:%{public}d", ret);
    }
    CallEventInfo eventInfo;
    (void)memset_s(&eventInfo, sizeof(CallEventInfo), 0, sizeof(CallEventInfo));
    if (mEventIdTransferMap_.find(info.eventId) != mEventIdTransferMap_.end()) {
        eventInfo.eventId = mEventIdTransferMap_[info.eventId];
        DialParaInfo dialInfo;
        if (eventInfo.eventId == CallAbilityEventId::EVENT_DIAL_NO_CARRIER) {
            DelayedSingleton<CallControlManager>::GetInstance()->GetDialParaInfo(dialInfo);
            if (dialInfo.number.length() > static_cast<size_t>(kMaxNumberLen)) {
                TELEPHONY_LOGE("Number out of limit!");
                return CALL_ERR_NUMBER_OUT_OF_RANGE;
            }
            if (memcpy_s(eventInfo.phoneNum, kMaxNumberLen, dialInfo.number.c_str(), dialInfo.number.length()) != EOK) {
                TELEPHONY_LOGE("memcpy_s failed!");
                return TELEPHONY_ERR_MEMCPY_FAIL;
            }
        } else if (eventInfo.eventId == CallAbilityEventId::EVENT_COMBINE_CALL_FAILED) {
            sptr<CallBase> activeCall = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
            if (activeCall != nullptr) {
                activeCall->HandleCombineConferenceFailEvent();
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
        if (strlen(info.bundleName) > static_cast<size_t>(kMaxNumberLen)) {
            TELEPHONY_LOGE("Number out of limit!");
            return CALL_ERR_NUMBER_OUT_OF_RANGE;
        }
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
    sptr<CallBase> call = GetOneCallObjectByIndex(info.index);
    if (call != nullptr && call->GetCallType() != info.callType) {
        call = RefreshCallIfNecessary(call, info);
        return TELEPHONY_SUCCESS;
    }
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
    call = CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
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

int32_t CallStatusManager::IncomingVoipCallHandle(const CallDetailInfo &info)
{
    int32_t ret = TELEPHONY_ERROR;
    sptr<CallBase> call = GetOneCallObjectByIndex(info.index);
    if (call != nullptr && call->GetCallType() != info.callType) {
        call = RefreshCallIfNecessary(call, info);
        return TELEPHONY_SUCCESS;
    }
    call = CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateVoipCall failed!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyNewCallCreated(call);
    ret = UpdateCallState(call, info.state);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed!");
        return ret;
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
    DataShare::DataSharePredicates predicates;
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
    return CallIncomingFilterManagerPtr_->DoIncomingFilter(info);
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

int32_t CallStatusManager::UpdateDialingCallInfo(const CallDetailInfo &info)
{
    sptr<CallBase> call = GetOneCallObjectByIndexAndSlotId(info.index, info.accountId);
    if (call != nullptr) {
        call = RefreshCallIfNecessary(call, info);
        return TELEPHONY_SUCCESS;
    }
    call = GetOneCallObjectByIndex(INIT_INDEX);
    if (call == nullptr) {
        TELEPHONY_LOGE("call is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    std::string oriNum = call->GetAccountNumber();
    call = RefreshCallIfNecessary(call, info);
    call->SetCallIndex(info.index);
    call->SetBundleName(info.bundleName);
    call->SetSlotId(info.accountId);
    call->SetTelCallState(info.state);
    call->SetVideoStateType(info.callMode);
    call->SetCallType(info.callType);
    call->SetAccountNumber(oriNum);
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::DialingHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle dialing state");
    if (info.index > 0) {
        TELEPHONY_LOGI("need update call info");
        return UpdateDialingCallInfo(info);
    }
    sptr<CallBase> call = CreateNewCall(info, CallDirection::CALL_DIRECTION_OUT);
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateNewCall failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = call->DialingProcess();
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
    sptr<CallBase> call = GetOneCallObjectByIndexAndSlotId(info.index, info.accountId);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    call = RefreshCallIfNecessary(call, info);
    // call state change active, need to judge if launching a conference
    if (info.mpty == 1) {
        int32_t mainCallId = ERR_ID;
        call->LaunchConference();
        call->GetMainCallId(mainCallId);
        sptr<CallBase> mainCall = GetOneCallObject(mainCallId);
        if (mainCall != nullptr) {
            mainCall->SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
        }
    } else if (call->ExitConference() == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("SubCallSeparateFromConference success!");
    } else {
        TELEPHONY_LOGI("SubCallSeparateFromConference fail!");
    }
    int32_t ret = UpdateCallState(call, TelCallState::CALL_STATUS_ACTIVE);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
        return ret;
    }
    sptr<CallBase> holdCall = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
    if (holdCall != nullptr) {
        holdCall->SetCanSwitchCallState(true);
        TELEPHONY_LOGI("holdcall:%{public}d can swap", holdCall->GetCallID());
    }
#ifdef AUDIO_SUPPORT
    ToSpeakerPhone(call);
    DelayedSingleton<AudioControlManager>::GetInstance()->SetVolumeAudible();
#endif
    TELEPHONY_LOGI("handle active state success");
    return ret;
}

int32_t CallStatusManager::ActiveVoipCallHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle active state");
    sptr<CallBase> call = GetOneCallObjectByIndexAndSlotId(info.index, info.accountId);
    if (call == nullptr) {
        TELEPHONY_LOGE("voip Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    call = RefreshCallIfNecessary(call, info);
    int32_t ret = UpdateCallState(call, TelCallState::CALL_STATUS_ACTIVE);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
        return ret;
    }
    TELEPHONY_LOGI("handle active state success");
    return ret;
}

int32_t CallStatusManager::HoldingHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle holding state");
    std::string tmpStr(info.phoneNum);
    sptr<CallBase> call = GetOneCallObjectByIndexAndSlotId(info.index, info.accountId);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    // if the call is in a conference, it will exit, otherwise just set it holding
    call = RefreshCallIfNecessary(call, info);
    if (info.mpty == 1) {
        int32_t ret = call->HoldConference();
        if (ret == TELEPHONY_SUCCESS) {
            TELEPHONY_LOGI("HoldConference success");
        }
    }
    int32_t ret = UpdateCallState(call, TelCallState::CALL_STATUS_HOLDING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
    }
    int32_t callId = call->GetCallID();
    int32_t dsdsMode = DSDS_MODE_V2;
    DelayedRefSingleton<CoreServiceClient>::GetInstance().GetDsdsMode(dsdsMode);
    TELEPHONY_LOGE("HoldingHandle dsdsMode:%{public}d", dsdsMode);
    if (dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_DSDA) ||
        dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_TDM)) {
        int32_t activeCallNum = GetCallNum(TelCallState::CALL_STATUS_ACTIVE);
        TelConferenceState confState = call->GetTelConferenceState();
        int32_t conferenceId = ERR_ID;
        call->GetMainCallId(conferenceId);
        if (confState != TelConferenceState::TEL_CONFERENCE_IDLE && conferenceId == callId) {
            AutoAnswerForDsda(activeCallNum, call->GetSlotId());
        } else if (confState == TelConferenceState::TEL_CONFERENCE_IDLE) {
            AutoAnswerForDsda(activeCallNum, call->GetSlotId());
        }
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
    sptr<CallBase> call = GetOneCallObjectByIndexAndSlotId(info.index, info.accountId);
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
    sptr<CallBase> call = GetOneCallObjectByIndexAndSlotId(info.index, info.accountId);
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

int32_t CallStatusManager::DisconnectedVoipCallHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle disconnected voip call state");
    sptr<CallBase> call = GetOneCallObjectByIndexAndSlotId(info.index, info.accountId);
    if (call == nullptr) {
        TELEPHONY_LOGE("voip Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    call = RefreshCallIfNecessary(call, info);
    int32_t ret = UpdateCallState(call, TelCallState::CALL_STATUS_DISCONNECTED);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
        return ret;
    }
    DeleteOneCallObject(call->GetCallID());
    TELEPHONY_LOGI("handle disconnected voip call state success");
    return ret;
}

int32_t CallStatusManager::DisconnectedHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle disconnected state");
    std::string tmpStr(info.phoneNum);
    sptr<CallBase> call = GetOneCallObjectByIndexAndSlotId(info.index, info.accountId);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    call = RefreshCallIfNecessary(call, info);
    bool canUnHold = false;
    std::vector<std::u16string> callIdList;
    call->GetSubCallIdList(callIdList);
    CallRunningState previousState = call->GetCallRunningState();
    int32_t ret = call->ExitConference();
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("SubCallSeparateFromConference success");
    }
    ret = UpdateCallState(call, TelCallState::CALL_STATUS_DISCONNECTED);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
        return ret;
    }
    size_t size = callIdList.size();
    int32_t activeCallNum = GetCallNum(TelCallState::CALL_STATUS_ACTIVE);
    int32_t waitingCallNum = GetCallNum(TelCallState::CALL_STATUS_WAITING);
    IsCanUnHold(activeCallNum, waitingCallNum, size, canUnHold);
    sptr<CallBase> holdCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
    if (previousState != CallRunningState::CALL_RUNNING_STATE_HOLD &&
        previousState != CallRunningState::CALL_RUNNING_STATE_ACTIVE) {
        if (holdCall != nullptr && canUnHold && holdCall->GetCanUnHoldState()) {
            if (holdCall->GetSlotId() == call->GetSlotId()) {
                TELEPHONY_LOGI("release call and recover the held call");
                holdCall->UnHoldCall();
            }
        }
    }
    DeleteOneCallObject(call->GetCallID());
    int32_t dsdsMode = DSDS_MODE_V2;
    DelayedRefSingleton<CoreServiceClient>::GetInstance().GetDsdsMode(dsdsMode);
    TELEPHONY_LOGI("DisconnectedHandle dsdsMode:%{public}d", dsdsMode);
    if (dsdsMode == DSDS_MODE_V3) {
        AutoAnswer(activeCallNum, waitingCallNum);
    } else if (dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_DSDA) ||
               dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_TDM)) {
        AutoAnswerForDsda(activeCallNum, call->GetSlotId());
    }
    return ret;
}

void CallStatusManager::IsCanUnHold(int32_t activeCallNum, int32_t waitingCallNum, int32_t size, bool &canUnHold)
{
    int32_t incomingCallNum = GetCallNum(TelCallState::CALL_STATUS_INCOMING);
    int32_t answeredCallNum = GetCallNum(TelCallState::CALL_STATUS_ANSWERED);
    int32_t dialingCallNum = GetCallNum(TelCallState::CALL_STATUS_ALERTING);
    if (answeredCallNum == 0 && incomingCallNum == 0 && (size == 0 || size == 1) && activeCallNum == 0 &&
        waitingCallNum == 0 && dialingCallNum == 0) {
        canUnHold = true;
    }
    TELEPHONY_LOGI("CanUnHold state: %{public}d", canUnHold);
}

void CallStatusManager::AutoAnswerForDsda(int32_t activeCallNum, int32_t slotId)
{
    int32_t dialingCallNum = GetCallNum(TelCallState::CALL_STATUS_DIALING);
    int32_t alertingCallNum = GetCallNum(TelCallState::CALL_STATUS_ALERTING);
    int32_t waitingCallNum = GetCallNum(TelCallState::CALL_STATUS_WAITING);
    int32_t answeredCallNum = GetCallNum(TelCallState::CALL_STATUS_ANSWERED);
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    for (int32_t ringCallId : callIdList) {
        sptr<CallBase> ringCall = GetOneCallObject(ringCallId);
        if (ringCall != nullptr && ringCall->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_RINGING) {
            TELEPHONY_LOGI("ringCall is not nullptr");
            if (dialingCallNum == 0 && alertingCallNum == 0 && activeCallNum == 0 && answeredCallNum == 0 &&
                ringCall->GetAutoAnswerState()) {
                int32_t videoState = static_cast<int32_t>(ringCall->GetVideoStateType());
                int ret = ringCall->AnswerCall(videoState);
                TELEPHONY_LOGI("ret = %{public}d", ret);
                ringCall->SetAutoAnswerState(false);
                return;
            }
        }
    }
    for (int32_t otherCallId : callIdList) {
        sptr<CallBase> otherCall = GetOneCallObject(otherCallId);
        TelCallState state = otherCall->GetTelCallState();
        TelConferenceState confState = otherCall->GetTelConferenceState();
        int32_t conferenceId = ERR_ID;
        otherCall->GetMainCallId(conferenceId);
        if (slotId != otherCall->GetSlotId() && state == TelCallState::CALL_STATUS_HOLDING &&
            otherCall->GetCanUnHoldState() && answeredCallNum == 0 && activeCallNum == 0 && waitingCallNum == 0 &&
            dialingCallNum == 0) {
            if (confState != TelConferenceState::TEL_CONFERENCE_IDLE && conferenceId == otherCallId) {
                otherCall->UnHoldCall();
                return;
            } else if (confState == TelConferenceState::TEL_CONFERENCE_IDLE) {
                otherCall->UnHoldCall();
                return;
            }
        }
    }
}

void CallStatusManager::AutoAnswer(int32_t activeCallNum, int32_t waitingCallNum)
{
    int32_t holdingCallNum = GetCallNum(TelCallState::CALL_STATUS_HOLDING);
    int32_t dialingCallNum = GetCallNum(TelCallState::CALL_STATUS_DIALING);
    int32_t alertingCallNum = GetCallNum(TelCallState::CALL_STATUS_ALERTING);
    if (activeCallNum == 0 && waitingCallNum == 0 && holdingCallNum == 0 && dialingCallNum == 0 &&
        alertingCallNum == 0) {
        std::list<int32_t> ringCallIdList;
        GetCarrierCallList(ringCallIdList);
        for (int32_t ringingCallId : ringCallIdList) {
            sptr<CallBase> ringingCall = GetOneCallObject(ringingCallId);
            CallRunningState ringingCallState = ringingCall->GetCallRunningState();
            if ((ringingCallState == CallRunningState::CALL_RUNNING_STATE_RINGING &&
                    (ringingCall->GetAutoAnswerState()))) {
                ringingCall->SetAutoAnswerState(false);
                int32_t videoState = static_cast<int32_t>(ringingCall->GetVideoStateType());
                int ret = ringingCall->AnswerCall(videoState);
                TELEPHONY_LOGI("ret = %{public}d", ret);
                break;
            }
        }
    }
}

int32_t CallStatusManager::UpdateCallState(sptr<CallBase> &call, TelCallState nextState)
{
    TELEPHONY_LOGI("UpdateCallState start");
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    TelCallState priorState = call->GetTelCallState();
    VideoStateType videoState = call->GetVideoStateType();
    TELEPHONY_LOGI(
        "callIndex:%{public}d, callId:%{public}d, priorState:%{public}d, nextState:%{public}d, videoState:%{public}d",
        call->GetCallIndex(), call->GetCallID(), priorState, nextState, videoState);
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
    if (call->GetCallType() == CallType::TYPE_IMS && call->GetVideoStateType() != info.callMode) {
        call->SetVideoStateType(info.callMode);
        sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(call.GetRefPtr());
        imsCall->InitVideoCall();
    }
    if (call->GetCallType() == CallType::TYPE_IMS) {
        call->SetCrsType(info.crsType);
        call->SetOriginalCallType(info.originalCallType);
    }
    if (call->GetCallType() == info.callType) {
        TELEPHONY_LOGI("RefreshCallIfNecessary not need Refresh");
        return call;
    }
    TelCallState priorState = call->GetTelCallState();
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
    newCall->SetCallBeginTime(attrInfo.callBeginTime);
    newCall->SetCallEndTime(attrInfo.callEndTime);
    newCall->SetRingBeginTime(attrInfo.ringBeginTime);
    newCall->SetRingEndTime(attrInfo.ringEndTime);
    newCall->SetAnswerType(attrInfo.answerType);
    DeleteOneCallObject(call->GetCallID());
    newCall->SetCallId(call->GetCallID());
    newCall->SetTelCallState(priorState);
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
        AudioDevice device = {
            .deviceType = AudioDeviceType::DEVICE_SPEAKER,
            .address = { 0 },
        };
        DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioDevice(device);
        ret = call->SetSpeakerphoneOn(false);
    }
    return ret;
}

int32_t CallStatusManager::TurnOffMute(sptr<CallBase> &call)
{
    bool enabled = true;
    if (HasEmergencyCall(enabled) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("CallStatusManager::TurnOffMute HasEmergencyCall failed.");
    }
    if (call->GetEmergencyState() || enabled) {
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
            if (callPtr->GetCallType() == CallType::TYPE_IMS) {
                sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(callPtr.GetRefPtr());
                imsCall->InitVideoCall();
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
        case CallType::TYPE_VOIP: {
            callPtr = (std::make_unique<VoIPCall>(paraInfo)).release();
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
    if (info.callType == CallType::TYPE_VOIP) {
        paraInfo.voipCallInfo.voipCallId = info.voipCallInfo.voipCallId;
        paraInfo.voipCallInfo.userName = info.voipCallInfo.userName;
        paraInfo.voipCallInfo.pixelMap = info.voipCallInfo.pixelMap;
        paraInfo.voipCallInfo.abilityName = info.voipCallInfo.abilityName;
        paraInfo.voipCallInfo.extensionId = info.voipCallInfo.extensionId;
        paraInfo.voipCallInfo.voipBundleName = info.voipCallInfo.voipBundleName;
    }
    paraInfo.number = info.phoneNum;
    paraInfo.callId = GetNewCallId();
    paraInfo.index = info.index;
    paraInfo.videoState = info.callMode;
    paraInfo.accountId = info.accountId;
    paraInfo.callType = info.callType;
    paraInfo.callState = info.state;
    paraInfo.bundleName = info.bundleName;
    paraInfo.crsType = info.crsType;
    paraInfo.originalCallType = info.originalCallType;
}
} // namespace Telephony
} // namespace OHOS
