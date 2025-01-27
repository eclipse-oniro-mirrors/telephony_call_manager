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

#include "antifraud_service.h"
#include "audio_control_manager.h"
#include "bluetooth_call.h"
#include "bluetooth_call_connection.h"
#include "bluetooth_call_service.h"
#include "bool_wrapper.h"
#include "call_ability_report_proxy.h"
#include "call_control_manager.h"
#include "call_earthquake_alarm_locator.h"
#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "call_number_utils.h"
#include "call_request_event_handler_helper.h"
#include "call_superprivacy_control_manager.h"
#include "call_voice_assistant_manager.h"
#include "cs_call.h"
#include "core_service_client.h"
#include "datashare_predicates.h"
#include "distributed_communication_manager.h"
#include "ffrt.h"
#include "hitrace_meter.h"
#include "ims_call.h"
#include "notification_helper.h"
#include "motion_recognition.h"
#include "os_account_manager.h"
#include "ott_call.h"
#include "parameters.h"
#include "report_call_info_handler.h"
#include "satellite_call.h"
#include "satellite_call_control.h"
#include "screen_sensor_plugin.h"
#include "settings_datashare_helper.h"
#include "spam_call_adapter.h"
#include "telephony_log_wrapper.h"
#include "uri.h"
#include "voip_call.h"
#include "want_params_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t INIT_INDEX = 0;
constexpr int32_t PRESENTATION_RESTRICTED = 3;

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
        tmpCallDetailsInfo_[i].callVec.clear();
    }
    for (int32_t i = 0; i < SLOT_NUM; i++) {
        priorVideoState_[i] = VideoStateType::TYPE_VOICE;
    }
    mEventIdTransferMap_.clear();
    mOttEventIdTransferMap_.clear();
    InitCallBaseEvent();
    CallIncomingFilterManagerPtr_ = (std::make_unique<CallIncomingFilterManager>()).release();
    std::shared_ptr<CallStatusManager> sharedPtr(this);
    DelayedSingleton<AntiFraudService>::GetInstance()->SetCallStatusManager(sharedPtr);
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
        tmpCallDetailsInfo_[i].callVec.clear();
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
        return HandleVoipCallReportInfo(info);
    }
    if (info.callType == CallType::TYPE_BLUETOOTH) {
        HandleBluetoothCallReportInfo(info);
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
            DelayedSingleton<CallControlManager>::GetInstance()->AcquireIncomingLock();
            ret = IncomingHandle(info);
            DelayedSingleton<CallControlManager>::GetInstance()->ReleaseIncomingLock();
            FinishAsyncTrace(HITRACE_TAG_OHOS, "InComingCall", getpid());
            DelayedSingleton<CallManagerHisysevent>::GetInstance()->JudgingIncomingTimeOut(
                info.accountId, static_cast<int32_t>(info.callType), static_cast<int32_t>(info.callMode));
            break;
        }
        case TelCallState::CALL_STATUS_WAITING:
        case TelCallState::CALL_STATUS_DISCONNECTED:
        case TelCallState::CALL_STATUS_DISCONNECTING:
            ret = HandleCallReportInfoEx(info);
        default:
            TELEPHONY_LOGE("Invalid call state!");
            break;
    }
    TELEPHONY_LOGI("Entry CallStatusManager HandleCallReportInfo");
    HandleDsdaInfo(info.accountId);
    DelayedSingleton<BluetoothCallService>::GetInstance()->GetCallState();
    TELEPHONY_LOGI("End CallStatusManager HandleCallReportInfo");
    return ret;
}

void CallStatusManager::HandleBluetoothCallReportInfo(const CallDetailInfo &info)
{
    if (info.state == TelCallState::CALL_STATUS_WAITING || info.state == TelCallState::CALL_STATUS_INCOMING) {
        return;
    }
    sptr<CallBase> call = nullptr;
    if (info.index > 0) {
        if (info.state == TelCallState::CALL_STATUS_DIALING || info.state == TelCallState::CALL_STATUS_ALERTING) {
            call = GetOneCallObjectByIndexSlotIdAndCallType(INIT_INDEX, info.accountId, info.callType);
            if (call != nullptr) {
                BtCallDialingHandleFirst(call, info);
                return;
            }
        }
        if (call == nullptr) {
            call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
        }
        if (call != nullptr) {
            return;
        }
    } else {
        return;
    }
    if (call == nullptr) {
        call = CreateNewCall(info, CallDirection::CALL_DIRECTION_OUT);
        if (call == nullptr) {
            TELEPHONY_LOGE("Call is NULL");
            return;
        }
        AddOneCallObject(call);
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyNewCallCreated(call);
    }
    return;
}

void CallStatusManager::HandleDsdaInfo(int32_t slotId)
{
    int32_t dsdsMode = DSDS_MODE_V2;
    bool noOtherCall = true;
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    int32_t currentCallNum = GetCurrentCallNum();
    DelayedSingleton<CallRequestProcess>::GetInstance()->IsExistCallOtherSlot(callIdList, slotId, noOtherCall);
    DelayedRefSingleton<CoreServiceClient>::GetInstance().GetDsdsMode(dsdsMode);
    TELEPHONY_LOGI("dsdsMode:%{public}d", dsdsMode);
    if ((dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_DSDA) ||
            dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_TDM)) &&
        !noOtherCall) {
        TELEPHONY_LOGI("Handle DsdaCallInfo");
        sptr<CallBase> holdCall = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
        if (holdCall != nullptr && currentCallNum > CALL_NUMBER) {
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
    tmpCallDetailsInfo_[curSlotId].callVec.clear();
    tmpCallDetailsInfo_[curSlotId] = info;
    for (auto &it : info.callVec) {
        for (const auto &it1 : callDetailsInfo_[curSlotId].callVec) {
            if (it.index == it1.index) {
                // call state changes
                if (it.state != it1.state || it.mpty != it1.mpty || it.callType != it1.callType
                    || it.callMode != it1.callMode || it.state == TelCallState::CALL_STATUS_ALERTING) {
                    TELEPHONY_LOGI("handle updated call state:%{public}d", it.state);
                    HandleCallReportInfo(it);
                }
                flag = true;
                break;
            }
        }
        // incoming/outgoing call handle
        if (!flag || callDetailsInfo_[curSlotId].callVec.empty()) {
            HandleConnectingCallReportInfo(it);
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
    UpdateCallDetailsInfo(info);
    return TELEPHONY_SUCCESS;
}

void CallStatusManager::HandleConnectingCallReportInfo(const CallDetailsInfo &info)
{
    int32_t callId = ERR_ID;
    bool isMeetimeActiveCallExist = CallObjectManager::IsVoipCallExist(TelCallState::CALL_STATUS_ACTIVE, callId);
    CallDetailInfo tempInfo = info;
    TELEPHONY_LOGI("handle new call state:%{public}d, isMeetimeActiveCallExist:%{public}d",
        info.state, isMeetimeActiveCallExist);
    if (isMeetimeActiveCallExist && info.state == TelCallState::CALL_STATUS_INCOMING) {
        tempInfo.state = TelCallState::CALL_STATUS_WAITING;
    }
    HandleCallReportInfo(tempInfo);
}

void CallStatusManager::UpdateCallDetailsInfo(const CallDetailsInfo &info)
{
    int32_t curSlotId = info.slotId;
    callDetailsInfo_[curSlotId].callVec.clear();
    callDetailsInfo_[curSlotId] = info;
    auto condition = [](CallDetailInfo i) { return i.state == TelCallState::CALL_STATUS_DISCONNECTED; };
    auto it_end = std::remove_if(callDetailsInfo_[curSlotId].callVec.begin(),
        callDetailsInfo_[curSlotId].callVec.end(), condition);
    callDetailsInfo_[curSlotId].callVec.erase(it_end, callDetailsInfo_[curSlotId].callVec.end());
    if (callDetailsInfo_[curSlotId].callVec.empty()) {
        TELEPHONY_LOGI("clear tmpCallDetailsInfo");
        tmpCallDetailsInfo_[curSlotId].callVec.clear();
    }
    TELEPHONY_LOGI("End CallStatusManager HandleCallsReportInfo slotId:%{public}d, "
        "callDetailsInfo_ size:%{public}zu", info.slotId, callDetailsInfo_[curSlotId].callVec.size());
}

int32_t CallStatusManager::HandleVoipCallReportInfo(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("Entry CallStatusManager HandleVoipCallReportInfo");
    int32_t ret = TELEPHONY_ERR_FAIL;
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
        case TelCallState::CALL_STATUS_DIALING:
            ret = OutgoingVoipCallHandle(info);
            break;
        case TelCallState::CALL_STATUS_ANSWERED:
            ret = AnsweredVoipCallHandle(info);
            break;
        case TelCallState::CALL_STATUS_DISCONNECTING:
            ret = DisconnectingVoipCallHandle(info);
            break;
        default:
            TELEPHONY_LOGE("Invalid call state!");
            break;
    }
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
        } else if (eventInfo.eventId == CallAbilityEventId::EVENT_HOLD_CALL_FAILED) {
            needWaitHold_ = false;
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

int32_t CallStatusManager::HandleVoipEventReportInfo(const VoipCallEventInfo &info)
{
    TELEPHONY_LOGI("recv one Event, eventId:%{public}d", info.voipCallEvent);
    sptr<CallBase> call = GetOneCallObjectByVoipCallId(info.voipCallId, info.bundleName, info.uid);
    if (call == nullptr) {
        TELEPHONY_LOGE("voip call is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (call->GetCallRunningState() != CallRunningState::CALL_RUNNING_STATE_ACTIVE
        && call->GetCallRunningState() != CallRunningState::CALL_RUNNING_STATE_DIALING) {
        return TELEPHONY_ERR_FAIL;
    }
    if (info.voipCallEvent == VoipCallEvent::VOIP_CALL_EVENT_MUTED) {
        call->SetMicPhoneState(true);
    } else if (info.voipCallEvent == VoipCallEvent::VOIP_CALL_EVENT_UNMUTED) {
        call->SetMicPhoneState(false);
    }
    DelayedSingleton<AudioDeviceManager>::GetInstance()->ReportAudioDeviceInfo(call);
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::IncomingHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle incoming state");
    detectStartTime = std::chrono::system_clock::from_time_t(0);
    int32_t ret = TELEPHONY_SUCCESS;
    bool isExisted = false;
    ret = RefreshOldCall(info, isExisted);
    if (isExisted) {
        return ret;
    }
    if (info.callType == CallType::TYPE_CS || info.callType == CallType::TYPE_IMS ||
        info.callType == CallType::TYPE_SATELLITE) {
        ret = IncomingFilterPolicy(info);
        if (ret != TELEPHONY_SUCCESS) {
            return ret;
        }
    }
    sptr<CallBase> call = CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateNewCall failed!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    if (IsFromTheSameNumberAtTheSameTime(call)) {
        ModifyEsimType();
        return ret;
    }
    SetContactInfo(call, std::string(info.phoneNum));
    bool block = false;
    if (IsRejectCall(call, info, block)) {
        return HandleRejectCall(call, block);
    }
    if (info.callType != CallType::TYPE_VOIP && info.callType != CallType::TYPE_BLUETOOTH &&
        IsRingOnceCall(call, info)) {
        return HandleRingOnceCall(call);
    }
    AddOneCallObject(call);
    StartInComingCallMotionRecognition();
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

void CallStatusManager::SetContactInfo(sptr<CallBase> &call, std::string phoneNum)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateVoipCall failed!");
        return;
    }
    ffrt::submit([=, &call]() {
        sptr<CallBase> callObjectPtr = call;
        // allow list filtering
        // Get the contact data from the database
        ContactInfo contactInfo = {
            .name = "",
            .number = phoneNum,
            .isContacterExists = false,
            .ringtonePath = "",
            .isSendToVoicemail = false,
            .isEcc = false,
            .isVoiceMail = false,
            .isQueryComplete = true,
        };
        QueryCallerInfo(contactInfo, phoneNum);
        callObjectPtr->SetCallerInfo(contactInfo);
        CallVoiceAssistantManager::GetInstance()->UpdateContactInfo(contactInfo, callObjectPtr->GetCallID());
        DelayedSingleton<DistributedCommunicationManager>::GetInstance()->ProcessCallInfo(callObjectPtr,
            DistributedDataType::NAME);
    });
}

int32_t CallStatusManager::HandleRejectCall(sptr<CallBase> &call, bool isBlock)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("call is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    if (ret != TELEPHONY_SUCCESS && ret != CALL_ERR_NOT_NEW_STATE) {
        TELEPHONY_LOGE("Set CallState failed!");
        return ret;
    }
    ret = call->RejectCall();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RejectCall failed!");
        return ret;
    }
    if (isBlock) {
        return DelayedSingleton<CallControlManager>::GetInstance()->AddBlockLogAndNotification(call);
    }
    return DelayedSingleton<CallControlManager>::GetInstance()->AddCallLogAndNotification(call);
}

int32_t CallStatusManager::IncomingVoipCallHandle(const CallDetailInfo &info)
{
    int32_t ret = TELEPHONY_ERROR;
    sptr<CallBase> call = GetOneCallObjectByVoipCallId(
        info.voipCallInfo.voipCallId, info.voipCallInfo.voipBundleName, info.voipCallInfo.uid);
    if (call != nullptr) {
        return TELEPHONY_SUCCESS;
    }
    call = CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateVoipCall failed!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    AddOneCallObject(call);
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyNewCallCreated(call);
    ret = UpdateCallState(call, info.state);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed!");
        return ret;
    }
    return ret;
}

int32_t CallStatusManager::OutgoingVoipCallHandle(const CallDetailInfo &info)
{
    int32_t ret = TELEPHONY_ERROR;
    sptr<CallBase> call = GetOneCallObjectByVoipCallId(
        info.voipCallInfo.voipCallId, info.voipCallInfo.voipBundleName, info.voipCallInfo.uid);
    if (call != nullptr) {
        VideoStateType originalType = call->GetVideoStateType();
        if (originalType != info.callMode) {
            TELEPHONY_LOGI("change VideoStateType from %{public}d to %{public}d",
                static_cast<int32_t>(originalType), static_cast<int32_t>(info.callMode));
            call->SetVideoStateType(info.callMode);
            return UpdateCallState(call, info.state);
        }
        return TELEPHONY_SUCCESS;
    }
    call = CreateNewCall(info, CallDirection::CALL_DIRECTION_OUT);
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateVoipCall failed!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    AddOneCallObject(call);
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyNewCallCreated(call);
    ret = UpdateCallState(call, info.state);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed!");
        return ret;
    }
    return ret;
}

int32_t CallStatusManager::AnsweredVoipCallHandle(const CallDetailInfo &info)
{
    int32_t ret = TELEPHONY_ERROR;
    sptr<CallBase> call = GetOneCallObjectByVoipCallId(
        info.voipCallInfo.voipCallId, info.voipCallInfo.voipBundleName, info.voipCallInfo.uid);
    if (call == nullptr) {
        return ret;
    }
    if (DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallStateUpdated(
        call, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_ANSWERED)) {
        return TELEPHONY_SUCCESS;
    } else {
        return ret;
    }
}

int32_t CallStatusManager::DisconnectingVoipCallHandle(const CallDetailInfo &info)
{
    sptr<CallBase> call = GetOneCallObjectByVoipCallId(
        info.voipCallInfo.voipCallId, info.voipCallInfo.voipBundleName, info.voipCallInfo.uid);
    if (call == nullptr) {
        return TELEPHONY_ERROR;
    }
    return UpdateCallState(call, TelCallState::CALL_STATUS_DISCONNECTING);
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
    predicates.EqualTo(TYPE_ID, 5); // type 5 means query number
    predicates.And();
    predicates.EqualTo(IS_DELETED, 0);
    predicates.And();
#ifdef TELEPHONY_CUST_SUPPORT
    if (phoneNum.length() >= static_cast<size_t>(QUERY_CONTACT_LEN)) {
        TELEPHONY_LOGI("phoneNum is longer than 7");
        predicates.EndsWith(DETAIL_INFO, phoneNum.substr(phoneNum.length() - QUERY_CONTACT_LEN));
        if (!callDataPtr->QueryContactInfoEnhanced(contactInfo, predicates)) {
            TELEPHONY_LOGE("Query contact database enhanced fail!");
        }
        return;
    }
#endif
    predicates.EqualTo(DETAIL_INFO, phoneNum);
    if (!callDataPtr->Query(contactInfo, predicates)) {
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
    AddOneCallObject(call);
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
    sptr<CallBase> call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
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
    ClearPendingState(call);
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::DialingHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle dialing state");
    bool isDistributedDeviceDialing = false;
    if (info.index > 0) {
        if (UpdateDialingHandle(info, isDistributedDeviceDialing)) {
            return UpdateDialingCallInfo(info);
        }
    }
    sptr<CallBase> call = CreateNewCall(info, CallDirection::CALL_DIRECTION_OUT);
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateNewCall failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    SetDistributedDeviceDialing(call, isDistributedDeviceDialing);
    if (IsDcCallConneceted()) {
        SetContactInfo(call, std::string(info.phoneNum));
    }
    AddOneCallObject(call);
    auto callRequestEventHandler = DelayedSingleton<CallRequestEventHandlerHelper>::GetInstance();
    if (info.index == INIT_INDEX) {
        callRequestEventHandler->SetPendingMo(true, call->GetCallID());
        call->SetPhoneOrWatchDial(static_cast<int32_t>(PhoneOrWatchDial::WATCH_DIAL));
        SetBtCallDialByPhone(call, false);
        StartOutGoingCallMotionRecognition();
    }
    callRequestEventHandler->RestoreDialingFlag(false);
    callRequestEventHandler->RemoveEventHandlerTask();
    int32_t ret = call->DialingProcess();
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyNewCallCreated(call);
    MyLocationEngine::StartEccService(call, info);
    ret = UpdateCallState(call, TelCallState::CALL_STATUS_DIALING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
    }
    return ret;
}

bool CallStatusManager::UpdateDialingHandle(const CallDetailInfo &info, bool &isDistributedDeviceDialing)
{
    sptr<CallBase> call = GetOneCallObjectByIndexSlotIdAndCallType(INIT_INDEX, info.accountId, info.callType);
    if (info.callType == CallType::TYPE_BLUETOOTH) {
        BtCallDialingHandle(call, info);
    } else {
        if (call == nullptr) {
            call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
            isDistributedDeviceDialing = IsDistributeCallSourceStatus();
        }
    }
    if (call != nullptr) {
        TELEPHONY_LOGI("need update call info");
        return true;
    }
    return false;
}

int32_t CallStatusManager::ActiveHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle active state");
    StopCallMotionRecognition(TelCallState::CALL_STATUS_ACTIVE);
    std::string tmpStr(info.phoneNum);
    sptr<CallBase> call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
    if (call == nullptr && IsDcCallConneceted()) {
        CreateAndSaveNewCall(info, CallDirection::CALL_DIRECTION_UNKNOW);
        call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
    }
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    OneCallAnswerAtPhone(call->GetCallID());
    ClearPendingState(call);
    call = RefreshCallIfNecessary(call, info);
    SetOriginalCallTypeForActiveState(call);
    // call state change active, need to judge if launching a conference
    std::vector<sptr<CallBase>> conferenceCallList = GetConferenceCallList(call->GetSlotId());
    if (info.mpty == 1 && conferenceCallList.size() > 1) {
        SetConferenceCall(conferenceCallList);
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

    bool isAntiFraudSupport = OHOS::system::GetBoolParameter(ANTIFRAUD_FEATURE, false);
    if (isAntiFraudSupport) {
        SetupAntiFraudService(call, info);
    }
    return ret;
}

void CallStatusManager::SetupAntiFraudService(const sptr<CallBase> &call, const CallDetailInfo &info)
{
    auto antiFraudService = DelayedSingleton<AntiFraudService>::GetInstance();
    NumberMarkInfo numberMarkInfo = call->GetNumberMarkInfo();
    std::string tmpStr(info.phoneNum);
    if (numberMarkInfo.markType != MarkType::MARK_TYPE_FRAUD &&
        !IsContactPhoneNum(tmpStr) && antiFraudService->IsAntiFraudSwitchOn()) {
        int32_t slotId = call->GetSlotId();
        if (slotId == -1) {
            return;
        }
        if (antiFraudSlotId_ != -1 || antiFraudIndex_ != -1) {
            return;
        }
        antiFraudSlotId_ = slotId;
        antiFraudIndex_ = info.index;
        antiFraudService->InitAntiFraudService();
    }
}

bool CallStatusManager::IsContactPhoneNum(const std::string &phoneNum)
{
    ContactInfo contactInfo = {
        .name = "",
        .number = phoneNum,
        .isContacterExists = false,
        .ringtonePath = "",
        .isSendToVoicemail = false,
        .isEcc = false,
        .isVoiceMail = false,
        .isQueryComplete = true,
    };
    QueryCallerInfo(contactInfo, phoneNum);
    if (contactInfo.name.length() < 1) {
        TELEPHONY_LOGI("no corresponding contact found");
        return false;
    } else {
        return true;
    }
}

void CallStatusManager::TriggerAntiFraud(int32_t antiFraudState)
{
    TELEPHONY_LOGI("TriggerAntiState, antiFraudState = %{public}d", antiFraudState);
    sptr<CallBase> call = nullptr;
    for (auto &it : callDetailsInfo_[antiFraudSlotId_].callVec) {
        if (it.index == antiFraudIndex_) {
            it.antiFraudState = antiFraudState;
            call = GetOneCallObjectByIndexSlotIdAndCallType(it.index, it.accountId, it.callType);
            break;
        }
    }

    if (call == nullptr) {
        return;
    }
    if (antiFraudState != static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_DEFAULT)) {
        AAFwk::WantParams extraParams;
        extraParams.SetParam("antiFraudState", AAFwk::Integer::Box(antiFraudState));
        call->SetExtraParams(extraParams);
    }

    if (call->GetTelCallState() == TelCallState::CALL_STATUS_ACTIVE) {
        int32_t ret = UpdateCallState(call, TelCallState::CALL_STATUS_ACTIVE);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
        }
    }
}

void CallStatusManager::SetConferenceCall(std::vector<sptr<CallBase>> conferenceCallList)
{
    for (auto conferenceCall : conferenceCallList) {
        TELEPHONY_LOGI("SetConferenceCall callid : %{public}d; State : %{public}d",
            conferenceCall->GetCallID(), conferenceCall->GetTelConferenceState());
        if (conferenceCall->GetTelConferenceState() != TelConferenceState::TEL_CONFERENCE_ACTIVE) {
            conferenceCall->LaunchConference();
            UpdateCallState(conferenceCall, conferenceCall->GetTelCallState());
        }
    }
}

int32_t CallStatusManager::ActiveVoipCallHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle active state");
    sptr<CallBase> call = GetOneCallObjectByVoipCallId(
        info.voipCallInfo.voipCallId, info.voipCallInfo.voipBundleName, info.voipCallInfo.uid);
    if (call == nullptr) {
        TELEPHONY_LOGE("voip Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    VideoStateType originalType = call->GetVideoStateType();
    if (originalType != info.callMode) {
        TELEPHONY_LOGI("change VideoStateType from %{public}d to %{public}d",
            static_cast<int32_t>(originalType), static_cast<int32_t>(info.callMode));
        call->SetVideoStateType(info.callMode);
    }
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
    sptr<CallBase> call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
    if (call == nullptr && IsDcCallConneceted()) {
        CreateAndSaveNewCall(info, CallDirection::CALL_DIRECTION_UNKNOW);
        call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
    }
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
    return UpdateCallStateAndHandleDsdsMode(info, call);
}

int32_t CallStatusManager::WaitingHandle(const CallDetailInfo &info)
{
    DelayedSingleton<CallControlManager>::GetInstance()->AcquireIncomingLock();
    int32_t ret = IncomingHandle(info);
    DelayedSingleton<CallControlManager>::GetInstance()->ReleaseIncomingLock();
    return ret;
}

int32_t CallStatusManager::AlertHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle alerting state");
    std::string tmpStr(info.phoneNum);
    sptr<CallBase> call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
    if (call == nullptr && IsDcCallConneceted()) {
        CreateAndSaveNewCall(info, CallDirection::CALL_DIRECTION_OUT);
        call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
    }
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ClearPendingState(call);
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
    sptr<CallBase> call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    call = RefreshCallIfNecessary(call, info);
    SetOriginalCallTypeForDisconnectState(call);
    int32_t ret = UpdateCallState(call, TelCallState::CALL_STATUS_DISCONNECTING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
    }
    return ret;
}

int32_t CallStatusManager::DisconnectedVoipCallHandle(const CallDetailInfo &info)
{
    TELEPHONY_LOGI("handle disconnected voip call state");
    sptr<CallBase> call = GetOneCallObjectByVoipCallId(
        info.voipCallInfo.voipCallId, info.voipCallInfo.voipBundleName, info.voipCallInfo.uid);
    if (call == nullptr) {
        TELEPHONY_LOGE("voip Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
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
    if (timeWaitHelper_ !=  nullptr) {
        TELEPHONY_LOGI("ringtone once");
        timeWaitHelper_->NotifyAll();
        timeWaitHelper_ = nullptr;
    }
    std::string tmpStr(info.phoneNum);
    sptr<CallBase> call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
    if (call == nullptr) {
        TELEPHONY_LOGE("call is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    StopCallMotionRecognition(TelCallState::CALL_STATUS_DISCONNECTED);
    bool isTwoCallBtCallAndESIM = CallObjectManager::IsTwoCallBtCallAndESIM();
    call = RefreshCallIfNecessary(call, info);
    RefreshCallDisconnectReason(call, static_cast<int32_t>(info.reason));
    ClearPendingState(call);
    SetOriginalCallTypeForDisconnectState(call);
    std::vector<std::u16string> callIdList;
    call->GetSubCallIdList(callIdList);
    CallRunningState previousState = call->GetCallRunningState();
    call->ExitConference();
    TelCallState priorState = call->GetTelCallState();
    UpdateCallState(call, TelCallState::CALL_STATUS_DISCONNECTED);
    MyLocationEngine::StopEccService(call->GetCallID());
    HandleHoldCallOrAutoAnswerCall(call, callIdList, previousState, priorState);
    std::vector<sptr<CallBase>> conferenceCallList = GetConferenceCallList(call->GetSlotId());
    if (conferenceCallList.size() == 1) {
        sptr<CallBase> leftOneConferenceCall = conferenceCallList[0];
        if (leftOneConferenceCall != nullptr &&
            leftOneConferenceCall->GetTelConferenceState() != TelConferenceState::TEL_CONFERENCE_IDLE) {
            TELEPHONY_LOGI("Not enough calls to be a conference!");
            leftOneConferenceCall->SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_IDLE);
            UpdateCallState(leftOneConferenceCall, leftOneConferenceCall->GetTelCallState());
        }
    }
    int32_t currentCallNum = CallObjectManager::GetCurrentCallNum();
    if (currentCallNum <= 0) {
        DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->RestoreSuperPrivacyMode();
    }
    if (isTwoCallBtCallAndESIM) {
        TELEPHONY_LOGI("Watch Auto AnswerCall");
        AutoAnswerSecondCall();
    }
    return TELEPHONY_SUCCESS;
}

void CallStatusManager::StopCallMotionRecognition(TelCallState nextState)
{
    switch (nextState) {
        case TelCallState::CALL_STATUS_ACTIVE:
            MotionRecogntion::UnsubscribePickupSensor();
            MotionRecogntion::UnsubscribeFlipSensor();
            break;
        case TelCallState::CALL_STATUS_DISCONNECTED:
            if (!CallObjectManager::HasCellularCallExist()) {
                MotionRecogntion::UnsubscribePickupSensor();
                MotionRecogntion::UnsubscribeFlipSensor();
                MotionRecogntion::UnsubscribeCloseToEarSensor();
                Rosen::UnloadMotionSensor();
            }
            break;
        default:
            break;
    }
}

std::vector<sptr<CallBase>> CallStatusManager::GetConferenceCallList(int32_t slotId)
{
    std::vector<sptr<CallBase>> conferenceCallList;
    for (const auto &it : tmpCallDetailsInfo_[slotId].callVec) {
        if (it.mpty == 1) {
            sptr<CallBase> conferenceCall = GetOneCallObjectByIndexAndSlotId(it.index, it.accountId);
            if (conferenceCall != nullptr) {
                conferenceCallList.emplace_back(conferenceCall);
            }
        }
    }
    TELEPHONY_LOGI("Conference call list size:%{public}zu", conferenceCallList.size());
    return conferenceCallList;
}

void CallStatusManager::HandleHoldCallOrAutoAnswerCall(const sptr<CallBase> call,
    std::vector<std::u16string> callIdList, CallRunningState previousState, TelCallState priorState)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("call is null");
        return;
    }
    bool canUnHold = false;
    size_t size = callIdList.size();
    int32_t activeCallNum = GetCallNum(TelCallState::CALL_STATUS_ACTIVE);
    int32_t waitingCallNum = GetCallNum(TelCallState::CALL_STATUS_WAITING);
    IsCanUnHold(activeCallNum, waitingCallNum, size, canUnHold);
    sptr<CallBase> holdCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
    if (previousState != CallRunningState::CALL_RUNNING_STATE_HOLD &&
        previousState != CallRunningState::CALL_RUNNING_STATE_ACTIVE &&
        priorState == TelCallState::CALL_STATUS_DISCONNECTING) {
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
    if (dsdsMode == DSDS_MODE_V3) {
        AutoAnswer(activeCallNum, waitingCallNum);
    } else if (dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_DSDA) ||
        dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_TDM)) {
        bool canSwitchCallState = call->GetCanSwitchCallState();
        AutoHandleForDsda(canSwitchCallState, priorState, activeCallNum, call->GetSlotId(), true);
    }
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

void CallStatusManager::AutoHandleForDsda(
    bool canSwitchCallState, TelCallState priorState, int32_t activeCallNum, int32_t slotId, bool continueAnswer)
{
    int32_t dialingCallNum = GetCallNum(TelCallState::CALL_STATUS_DIALING);
    int32_t alertingCallNum = GetCallNum(TelCallState::CALL_STATUS_ALERTING);
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    for (int32_t ringCallId : callIdList) {
        sptr<CallBase> ringCall = GetOneCallObject(ringCallId);
        if (ringCall != nullptr && ringCall->GetAutoAnswerState()) {
            TELEPHONY_LOGI("ringCall is not nullptr");
            int32_t videoState = static_cast<int32_t>(ringCall->GetVideoStateType());
            if (videoState == static_cast<int32_t>(VideoStateType::TYPE_VIDEO)) {
                TELEPHONY_LOGI("AutoAnswer VideoCall for Dsda");
                AutoAnswerForVideoCall(activeCallNum);
                AutoUnHoldForDsda(canSwitchCallState, priorState, activeCallNum, slotId);
                return;
            }
            if (dialingCallNum == 0 && alertingCallNum == 0 && activeCallNum == 0 &&
                ringCall->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_RINGING) {
                TELEPHONY_LOGI("AutoAnswer VoiceCall for Dsda");
                AutoAnswerForVoiceCall(ringCall, slotId, continueAnswer);
                AutoUnHoldForDsda(canSwitchCallState, priorState, activeCallNum, slotId);
                return;
            }
        }
    }
    AutoUnHoldForDsda(canSwitchCallState, priorState, activeCallNum, slotId);
}

void CallStatusManager::AutoUnHoldForDsda(
    bool canSwitchCallState, TelCallState priorState, int32_t activeCallNum, int32_t slotId)
{
    int32_t dialingCallNum = GetCallNum(TelCallState::CALL_STATUS_DIALING);
    int32_t waitingCallNum = GetCallNum(TelCallState::CALL_STATUS_WAITING);
    int32_t callNum = 2;
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    int32_t currentCallNum = GetCurrentCallNum();
    for (int32_t otherCallId : callIdList) {
        sptr<CallBase> otherCall = GetOneCallObject(otherCallId);
        if (otherCall == nullptr) {
            TELEPHONY_LOGE("otherCall is nullptr");
            continue;
        }
        TelCallState state = otherCall->GetTelCallState();
        TelConferenceState confState = otherCall->GetTelConferenceState();
        int32_t conferenceId = ERR_ID;
        otherCall->GetMainCallId(conferenceId);
        if (slotId != otherCall->GetSlotId() && state == TelCallState::CALL_STATUS_HOLDING &&
            otherCall->GetCanUnHoldState() && activeCallNum == 0 && waitingCallNum == 0 &&
            dialingCallNum == 0 &&
            ((confState != TelConferenceState::TEL_CONFERENCE_IDLE && conferenceId == otherCallId) ||
                confState == TelConferenceState::TEL_CONFERENCE_IDLE)) {
            // Actively hang up the processing unhold state or exchange call
            if (priorState == TelCallState::CALL_STATUS_DISCONNECTING ||
                (!canSwitchCallState && currentCallNum == callNum)) {
                otherCall->UnHoldCall();
                return;
            }
        }
    }
    for (int32_t otherCallId : callIdList) {
        sptr<CallBase> holdCall = GetOneCallObject(otherCallId);
        if (holdCall != nullptr && holdCall->GetTelCallState() == TelCallState::CALL_STATUS_HOLDING) {
            if (currentCallNum == callNum) {
                holdCall->SetCanUnHoldState(true);
            }
        }
    }
}

void CallStatusManager::AutoAnswerForVoiceCall(sptr<CallBase> ringCall, int32_t slotId, bool continueAnswer)
{
    /* Need to check whether the autoAnswer call and the holding call are on the same slotid
     * To prevent repeated AT command delivery.
     */
    if (continueAnswer || slotId != ringCall->GetSlotId()) {
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallStateUpdated(
            ringCall, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_ANSWERED);
        int ret = ringCall->AnswerCall(ringCall->GetAnswerVideoState());
        if (ret == TELEPHONY_SUCCESS) {
            DelayedSingleton<CallControlManager>::GetInstance()->NotifyIncomingCallAnswered(ringCall);
        }
        TELEPHONY_LOGI("ret = %{public}d", ret);
    }
    ringCall->SetAutoAnswerState(false);
}

void CallStatusManager::AutoAnswerForVideoCall(int32_t activeCallNum)
{
    int32_t holdingCallNum = GetCallNum(TelCallState::CALL_STATUS_HOLDING);
    int32_t dialingCallNum = GetCallNum(TelCallState::CALL_STATUS_DIALING);
    int32_t alertingCallNum = GetCallNum(TelCallState::CALL_STATUS_ALERTING);
    if (activeCallNum == 0 && holdingCallNum == 0 && dialingCallNum == 0 && alertingCallNum == 0) {
        std::list<int32_t> ringCallIdList;
        GetCarrierCallList(ringCallIdList);
        for (int32_t ringingCallId : ringCallIdList) {
            sptr<CallBase> ringingCall = GetOneCallObject(ringingCallId);
            if (ringingCall == nullptr) {
                TELEPHONY_LOGE("ringingCall is nullptr");
                return;
            }
            CallRunningState ringingCallState = ringingCall->GetCallRunningState();
            if ((ringingCallState == CallRunningState::CALL_RUNNING_STATE_RINGING &&
                    (ringingCall->GetAutoAnswerState()))) {
                ringingCall->SetAutoAnswerState(false);
                int ret = ringingCall->AnswerCall(ringingCall->GetAnswerVideoState());
                TELEPHONY_LOGI("ret = %{public}d", ret);
                break;
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
            if (ringingCall == nullptr) {
                TELEPHONY_LOGE("ringingCall is nullptr");
                return;
            }
            CallRunningState ringingCallState = ringingCall->GetCallRunningState();
            if ((ringingCallState == CallRunningState::CALL_RUNNING_STATE_RINGING &&
                    (ringingCall->GetAutoAnswerState()))) {
                ringingCall->SetAutoAnswerState(false);
                int ret = ringingCall->AnswerCall(ringingCall->GetAnswerVideoState());
                TELEPHONY_LOGI("ret = %{public}d", ret);
                break;
            }
        }
    }
}

int32_t CallStatusManager::UpdateCallState(sptr<CallBase> &call, TelCallState nextState)
{
    TELEPHONY_LOGW("UpdateCallState start");
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    TelCallState priorState = call->GetTelCallState();
    VideoStateType videoState = call->GetVideoStateType();
    TELEPHONY_LOGI(
        "callIndex:%{public}d, callId:%{public}d, priorState:%{public}d, nextState:%{public}d, videoState:%{public}d",
        call->GetCallIndex(), call->GetCallID(), priorState, nextState, videoState);
    if (call->GetCallType() == CallType::TYPE_SATELLITE) {
        DelayedSingleton<SatelliteCallControl>::GetInstance()->
            HandleSatelliteCallStateUpdate(call, priorState, nextState);
    }
    if (priorState == TelCallState::CALL_STATUS_INCOMING && nextState == TelCallState::CALL_STATUS_ACTIVE) {
        DelayedSingleton<CallManagerHisysevent>::GetInstance()->JudgingAnswerTimeOut(
            call->GetSlotId(), call->GetCallID(), static_cast<int32_t>(call->GetVideoStateType()));
    }
    int32_t ret = call->SetTelCallState(nextState);
    UpdateOneCallObjectByCallId(call->GetCallID(), nextState);
    if (ret != TELEPHONY_SUCCESS && ret != CALL_ERR_NOT_NEW_STATE) {
        TELEPHONY_LOGE("SetTelCallState failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
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
    SetVideoCallState(call, nextState);
    return TELEPHONY_SUCCESS;
}

void CallStatusManager::SetVideoCallState(sptr<CallBase> &call, TelCallState nextState)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return;
    }
    int slotId = call->GetSlotId();
    bool isSlotIdValid = false;
    if (slotId < SLOT_NUM && slotId >= 0) {
        isSlotIdValid = true;
    }
    VideoStateType videoState = call->GetVideoStateType();
    TELEPHONY_LOGI("nextVideoState:%{public}d, priorVideoState:%{public}d, isSlotIdValid:%{public}d", videoState,
        priorVideoState_[slotId], isSlotIdValid);
    if (isSlotIdValid && (priorVideoState_[slotId] != videoState)) {
        DelayedSingleton<AudioControlManager>::GetInstance()->VideoStateUpdated(
            call, priorVideoState_[slotId], videoState);
        priorVideoState_[slotId] = videoState;
    }
    if (isSlotIdValid && (nextState == TelCallState::CALL_STATUS_DISCONNECTED)) {
        priorVideoState_[slotId] = VideoStateType::TYPE_VOICE;
    }
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
    AddOneCallObject(newCall);
    newCall->SetCallRunningState(call->GetCallRunningState());
    newCall->SetTelConferenceState(call->GetTelConferenceState());
    newCall->SetStartTime(attrInfo.startTime);
    newCall->SetPolicyFlag(PolicyFlag(call->GetPolicyFlag()));
    newCall->SetSpeakerphoneOn(call->IsSpeakerphoneOn());
    newCall->SetCallEndedType(call->GetCallEndedType());
    newCall->SetCallBeginTime(attrInfo.callBeginTime);
    newCall->SetCallCreateTime(attrInfo.callCreateTime);
    newCall->SetCallEndTime(attrInfo.callEndTime);
    newCall->SetRingBeginTime(attrInfo.ringBeginTime);
    newCall->SetRingEndTime(attrInfo.ringEndTime);
    newCall->SetAnswerType(attrInfo.answerType);
    newCall->SetMicPhoneState(call->IsMuted());
    DeleteOneCallObject(call->GetCallID());
    newCall->SetCallId(call->GetCallID());
    newCall->SetTelCallState(priorState);
    if (call->GetNumberLocation() != "default") {
        newCall->SetNumberLocation(call->GetNumberLocation());
    }
    NumberMarkInfo numberMarkInfo = call->GetNumberMarkInfo();
    if (numberMarkInfo.markType != MarkType::MARK_TYPE_NONE) {
        newCall->SetNumberMarkInfo(numberMarkInfo);
    }
    return newCall;
}

void CallStatusManager::SetOriginalCallTypeForActiveState(sptr<CallBase> &call)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return;
    }
    TelCallState priorState = call->GetTelCallState();
    VideoStateType videoState = call->GetVideoStateType();
    int32_t videoStateHistory = call->GetOriginalCallType();
    if (priorState == TelCallState::CALL_STATUS_ALERTING || priorState == TelCallState::CALL_STATUS_INCOMING ||
        priorState == TelCallState::CALL_STATUS_WAITING) {
        // outgoing/incoming video call, but accepted/answered with voice call
        if (videoStateHistory != static_cast<int32_t>(videoState)) {
            TELEPHONY_LOGD("set videoState:%{public}d as original call type", static_cast<int32_t>(videoState));
            call->SetOriginalCallType(static_cast<int32_t>(videoState));
        }
    } else if (priorState == TelCallState::CALL_STATUS_ACTIVE || priorState == TelCallState::CALL_STATUS_HOLDING) {
        int32_t videoStateCurrent =
            static_cast<int32_t>(static_cast<uint32_t>(videoStateHistory) | static_cast<uint32_t>(videoState));
        TELEPHONY_LOGD("maybe upgrade/downgrade operation, keep video record always, videoStateCurrent:%{public}d",
            videoStateCurrent);
        call->SetOriginalCallType(videoStateCurrent);
    }
}

void CallStatusManager::SetOriginalCallTypeForDisconnectState(sptr<CallBase> &call)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return;
    }
    TelCallState priorState = call->GetTelCallState();
    CallAttributeInfo attrInfo;
    (void)memset_s(&attrInfo, sizeof(CallAttributeInfo), 0, sizeof(CallAttributeInfo));
    call->GetCallAttributeBaseInfo(attrInfo);
    if (priorState == TelCallState::CALL_STATUS_DIALING || priorState == TelCallState::CALL_STATUS_ALERTING ||
        ((priorState == TelCallState::CALL_STATUS_INCOMING || priorState == TelCallState::CALL_STATUS_WAITING) &&
        attrInfo.answerType != CallAnswerType::CALL_ANSWER_REJECT)) {
        // outgoing/incoming video call, but canceled or missed
        TELEPHONY_LOGD("canceled or missed call, set voice type as original call type");
        call->SetOriginalCallType(static_cast<int32_t>(VideoStateType::TYPE_VOICE));
    }
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
    TELEPHONY_LOGI("CreateNewCall");
    DialParaInfo paraInfo;
    AppExecFwk::PacMap extras;
    extras.Clear();
    PackParaInfo(paraInfo, info, dir, extras);

    sptr<CallBase> callPtr = CreateNewCallByCallType(paraInfo, info, dir, extras);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("CreateNewCall failed!");
        return nullptr;
    }
    DialScene dialScene = (DialScene)extras.GetIntValue("dialScene");
    if (dialScene == DialScene::CALL_EMERGENCY) {
        callPtr->SetIsEccContact(true);
    }
    callPtr->SetOriginalCallType(info.originalCallType);
    TELEPHONY_LOGD("originalCallType:%{public}d", info.originalCallType);
    if (info.callType == CallType::TYPE_VOIP) {
        return callPtr;
    }
    if (info.state == TelCallState::CALL_STATUS_INCOMING || info.state == TelCallState::CALL_STATUS_WAITING ||
        (info.state == TelCallState::CALL_STATUS_DIALING && (info.index == 0 || IsDcCallConneceted()))) {
        TELEPHONY_LOGI("NumberLocationUpdate start");
        wptr<CallBase> callBaseWeakPtr = callPtr;
        ffrt::submit([callBaseWeakPtr, info]() {
            sptr<CallBase> callBasePtr = callBaseWeakPtr.promote();
            if (callBasePtr == nullptr) {
                TELEPHONY_LOGE("callBasePtr is nullptr");
                return;
            }
            DelayedSingleton<CallNumberUtils>::GetInstance()->NumberLocationUpdate(callBasePtr);
            DelayedSingleton<DistributedCommunicationManager>::GetInstance()->ProcessCallInfo(callBasePtr,
                DistributedDataType::LOCATION);
            if (info.state == TelCallState::CALL_STATUS_DIALING) {
                DelayedSingleton<CallNumberUtils>::GetInstance()->YellowPageAndMarkUpdate(callBasePtr);
            }
        });
    }
    time_t createTime = time(nullptr);
    if (createTime < 0) {
        createTime = 0;
    }
    callPtr->SetCallCreateTime(createTime);
    return callPtr;
}

sptr<CallBase> CallStatusManager::CreateNewCallByCallType(
    DialParaInfo &paraInfo, const CallDetailInfo &info, CallDirection dir, AppExecFwk::PacMap &extras)
{
    sptr<CallBase> callPtr = nullptr;
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
        case CallType::TYPE_VOIP:
        case CallType::TYPE_SATELLITE:
        case CallType::TYPE_BLUETOOTH: {
            callPtr = CreateNewCallByCallTypeEx(paraInfo, info, dir, extras);
            break;
        }
        default:
            return nullptr;
    }
    return callPtr;
}

sptr<CallBase> CallStatusManager::CreateNewCallByCallTypeEx(
    DialParaInfo &paraInfo, const CallDetailInfo &info, CallDirection dir, AppExecFwk::PacMap &extras)
{
    sptr<CallBase> callPtr = nullptr;
    switch (info.callType) {
        case CallType::TYPE_VOIP: {
            callPtr = (std::make_unique<VoIPCall>(paraInfo)).release();
            break;
        }
        case CallType::TYPE_SATELLITE: {
            if (dir == CallDirection::CALL_DIRECTION_OUT) {
                callPtr = (std::make_unique<SatelliteCall>(paraInfo, extras)).release();
            } else {
                callPtr = (std::make_unique<SatelliteCall>(paraInfo)).release();
            }
            break;
        }
        case CallType::TYPE_BLUETOOTH: {
            std::string macAddress = DelayedSingleton<BluetoothCallConnection>::GetInstance()->GetMacAddress();
            if (dir == CallDirection::CALL_DIRECTION_OUT) {
                callPtr = (std::make_unique<BluetoothCall>(paraInfo, extras, macAddress)).release();
            } else {
                callPtr = (std::make_unique<BluetoothCall>(paraInfo, macAddress)).release();
            }
            break;
        }
        default:
            return nullptr;
    }
    return callPtr;
}

bool CallStatusManager::ShouldRejectIncomingCall()
{
    bool hasEcc = false;
    if (HasEmergencyCall(hasEcc) == TELEPHONY_SUCCESS && hasEcc) {
        TELEPHONY_LOGI("HasEmergencyCall reject incoming call.");
        return true;
    }

    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    std::string device_provisioned {"0"};
    OHOS::Uri uri(
        "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=device_provisioned");
    int resp = datashareHelper->Query(uri, "device_provisioned", device_provisioned);
    if (resp == TELEPHONY_SUCCESS && (device_provisioned == "0" || device_provisioned.empty())) {
        TELEPHONY_LOGW("ShouldRejectIncomingCall: device_provisioned = 0");
        return true;
    }

    std::string user_setup_complete {"1"};
    std::vector<int> activedOsAccountIds;
    OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(activedOsAccountIds);
    if (activedOsAccountIds.empty()) {
        TELEPHONY_LOGW("ShouldRejectIncomingCall: activedOsAccountIds is empty");
        return false;
    }
    int userId = activedOsAccountIds[0];
    OHOS::Uri uri_setup(
        "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_SECURE_"
        + std::to_string(userId) + "?Proxy=true&key=user_setup_complete");
    int resp_userSetup = datashareHelper->Query(uri_setup, "user_setup_complete", user_setup_complete);
    if (resp_userSetup == TELEPHONY_SUCCESS && (user_setup_complete == "0" || user_setup_complete.empty())) {
        TELEPHONY_LOGW("ShouldRejectIncomingCall: user_setup_complete = 0");
        return true;
    }
    return false;
}

bool CallStatusManager::ShouldBlockIncomingCall(const sptr<CallBase> &call, const CallDetailInfo &info)
{
    bool isEcc = false;
    DelayedSingleton<CellularCallConnection>::GetInstance()->IsEmergencyPhoneNumber(
        info.phoneNum, info.accountId, isEcc);
    if (isEcc) {
        TELEPHONY_LOGW("incoming phoneNumber is ecc.");
        return false;
    }
    std::shared_ptr<SpamCallAdapter> spamCallAdapterPtr_ = std::make_shared<SpamCallAdapter>();
    if (spamCallAdapterPtr_ == nullptr) {
        TELEPHONY_LOGE("create SpamCallAdapter object failed!");
        return false;
    }
    bool isDetectedSpamCall = spamCallAdapterPtr_->DetectSpamCall(std::string(info.phoneNum), info.accountId);
    if (!isDetectedSpamCall) {
        TELEPHONY_LOGE("DetectSpamCall failed!");
        return false;
    }
    detectStartTime = std::chrono::system_clock::now();
    if (spamCallAdapterPtr_->WaitForDetectResult()) {
        TELEPHONY_LOGW("DetectSpamCall no time out");
        NumberMarkInfo numberMarkInfo;
        bool isBlock = false;
        int32_t blockReason;
        std::string detectDetails = "";
        spamCallAdapterPtr_->GetParseResult(isBlock, numberMarkInfo, blockReason, detectDetails);
        call->SetNumberMarkInfo(numberMarkInfo);
        call->SetBlockReason(blockReason);
        call->SetDetectDetails(detectDetails);
        if (isBlock) {
            return true;
        }
    }
    return false;
}

bool CallStatusManager::IsRingOnceCall(const sptr<CallBase> &call, const CallDetailInfo &info)
{
    NumberMarkInfo numberMarkInfo = call->GetNumberMarkInfo();
    ContactInfo contactInfo = call->GetCallerInfo();
    if (numberMarkInfo.markType == MarkType::MARK_TYPE_YELLOW_PAGE || contactInfo.name != "") {
        TELEPHONY_LOGW("yellowpage or contact, no need check ring once call");
        return false;
    }
    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    std::string is_check_ring_once {"0"};
    std::string key = "spamshield_sim" + std::to_string(info.accountId + 1) + "_phone_switch_ring_once";
    OHOS::Uri uri(
        "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=" + key);
    int32_t ret = datashareHelper->Query(uri, key, is_check_ring_once);
    TELEPHONY_LOGW("is_check_ring_once = %{public}s", is_check_ring_once.c_str());
    if (ret != TELEPHONY_SUCCESS || is_check_ring_once == "0") {
        return false;
    }
    auto waitTime = WAIT_TIME_THREE_SECOND;
    if (detectStartTime != std::chrono::system_clock::from_time_t(0)) {
        auto detectEndTime = std::chrono::system_clock::now();
        auto diff = detectEndTime - detectStartTime;
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
        if (diff >= WAIT_TIME_THREE_SECOND) {
            TELEPHONY_LOGW("cost time over 3s, non need check ring once call");
        }
        waitTime = WAIT_TIME_THREE_SECOND - ms;
    }
    if (timeWaitHelper_ == nullptr) {
        timeWaitHelper_ = std::make_unique<TimeWaitHelper>(waitTime);
    }
    if (!timeWaitHelper_->WaitForResult()) {
        TELEPHONY_LOGW("is not ring once");
        timeWaitHelper_ = nullptr;
        return false;
    }
    timeWaitHelper_ = nullptr;
    return true;
}

int32_t CallStatusManager::HandleRingOnceCall(sptr<CallBase> &call)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("call is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    if (ret != TELEPHONY_SUCCESS && ret != CALL_ERR_NOT_NEW_STATE) {
        TELEPHONY_LOGE("Set CallState failed!");
        return ret;
    }
    return DelayedSingleton<CallControlManager>::GetInstance()->AddCallLogAndNotification(call);
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
        (paraInfo.voipCallInfo.userProfile).assign(
            (info.voipCallInfo.userProfile).begin(), (info.voipCallInfo.userProfile).end());
        paraInfo.voipCallInfo.abilityName = info.voipCallInfo.abilityName;
        paraInfo.voipCallInfo.extensionId = info.voipCallInfo.extensionId;
        paraInfo.voipCallInfo.voipBundleName = info.voipCallInfo.voipBundleName;
        paraInfo.voipCallInfo.showBannerForIncomingCall = info.voipCallInfo.showBannerForIncomingCall;
        paraInfo.voipCallInfo.isConferenceCall = info.voipCallInfo.isConferenceCall;
        paraInfo.voipCallInfo.isVoiceAnswerSupported = info.voipCallInfo.isVoiceAnswerSupported;
        paraInfo.voipCallInfo.hasMicPermission = info.voipCallInfo.hasMicPermission;
        paraInfo.voipCallInfo.isCapsuleSticky = info.voipCallInfo.isCapsuleSticky;
        paraInfo.voipCallInfo.uid = info.voipCallInfo.uid;
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
    paraInfo.extraParams =
        AAFwk::WantParamWrapper::ParseWantParamsWithBrackets(extras.GetStringValue("extraParams"));
    bool value = false;
    int32_t namePresentation;
    OperatorConfig config;
    CoreServiceClient::GetInstance().GetOperatorConfigs(info.accountId, config);
    if (config.boolValue.find("support_cnap_bool") != config.boolValue.end()) {
        value = config.boolValue["support_cnap_bool"];
    }
    namePresentation = value ? info.namePresentation : PRESENTATION_RESTRICTED;
    paraInfo.extraParams.SetParam("namePresentation", AAFwk::Integer::Box(namePresentation));
    paraInfo.extraParams.SetParam("name", AAFwk::String::Box(namePresentation == 0 ? info.name : ""));
    paraInfo.phoneOrWatch = info.phoneOrWatch;
}

bool CallStatusManager::IsFocusModeOpen()
{
    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    std::string focusModeEnable {"0"};
    std::vector<int> activedOsAccountIds;
    OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(activedOsAccountIds);
    if (activedOsAccountIds.empty()) {
        TELEPHONY_LOGW("ShouldRejectIncomingCall: activedOsAccountIds is empty");
        return false;
    }
    int userId = activedOsAccountIds[0];
    OHOS::Uri uri(
        "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_SECURE_"
        + std::to_string(userId) + "?Proxy=true&key=focus_mode_enable");
    int resp = datashareHelper->Query(uri, "focus_mode_enable", focusModeEnable);
    TELEPHONY_LOGI("IsFocusModeOpen: userId = %{public}d, focus_mode_enable = %{public}s",
        userId, focusModeEnable.c_str());
    if (resp == TELEPHONY_SUCCESS && focusModeEnable == "1") {
        return true;
    }
    return false;
}

bool CallStatusManager::IsRejectCall(sptr<CallBase> &call, const CallDetailInfo &info, bool &block)
{
    int32_t state;
    DelayedSingleton<CallControlManager>::GetInstance()->GetVoIPCallState(state);
    if (ShouldRejectIncomingCall() || state == (int32_t)CallStateToApp::CALL_STATE_RINGING) {
        CallManagerHisysevent::HiWriteBehaviorEventPhoneUE(
            CALL_INCOMING_REJECT_BY_SYSTEM, PNAMEID_KEY, KEY_CALL_MANAGER, PVERSIONID_KEY, "",
            ACTION_TYPE, REJECT_BY_OOBE);
        block = false;
        return true;
    }
    if (info.callType != CallType::TYPE_VOIP && info.callType != CallType::TYPE_BLUETOOTH &&
        ShouldBlockIncomingCall(call, info)) {
        CallManagerHisysevent::HiWriteBehaviorEventPhoneUE(
            CALL_INCOMING_REJECT_BY_SYSTEM, PNAMEID_KEY, KEY_CALL_MANAGER, PVERSIONID_KEY, "",
            ACTION_TYPE, REJECT_BY_NUM_BLOCK);
        block = true;
        return true;
    }
    if (IsFocusModeOpen()) {
        int ret = Notification::NotificationHelper::IsNeedSilentInDoNotDisturbMode(info.phoneNum, 0);
        TELEPHONY_LOGW("IsRejectCall IsNeedSilentInDoNotDisturbMode ret:%{public}d", ret);
        if (ret == 0) {
            CallManagerHisysevent::HiWriteBehaviorEventPhoneUE(
                CALL_INCOMING_REJECT_BY_SYSTEM, PNAMEID_KEY, KEY_CALL_MANAGER, PVERSIONID_KEY, "",
                ACTION_TYPE, REJECT_IN_FOCUSMODE);
            AAFwk::WantParams params = call->GetExtraParams();
            params.SetParam("IsNeedSilentInDoNotDisturbMode", AAFwk::Integer::Box(1));
            call->SetExtraParams(params);
        }
    }
    return false;
}

void CallStatusManager::CreateAndSaveNewCall(const CallDetailInfo &info, CallDirection direction)
{
    auto call = CreateNewCall(info, direction);
    if (call != nullptr) {
        ffrt::submit([=]() {
            DelayedSingleton<CallNumberUtils>::GetInstance()->YellowPageAndMarkUpdate(call);
        });
        AddOneCallObject(call);
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyNewCallCreated(call);
    }
}

int32_t CallStatusManager::UpdateCallStateAndHandleDsdsMode(const CallDetailInfo &info, sptr<CallBase> &call)
{
    if (call == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    TelCallState priorState = call->GetTelCallState();
    int32_t ret = UpdateCallState(call, TelCallState::CALL_STATUS_HOLDING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
    }
    int32_t callId = call->GetCallID();
    int32_t dsdsMode = DSDS_MODE_V2;
    DelayedRefSingleton<CoreServiceClient>::GetInstance().GetDsdsMode(dsdsMode);
    TELEPHONY_LOGE("HoldingHandle dsdsMode:%{public}d", dsdsMode);
    bool canSwitchCallState = call->GetCanSwitchCallState();
    if (dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_DSDA) ||
        dsdsMode == static_cast<int32_t>(DsdsMode::DSDS_MODE_V5_TDM)) {
        int32_t activeCallNum = GetCallNum(TelCallState::CALL_STATUS_ACTIVE);
        if (needWaitHold_ && activeCallNum == 0) {
            needWaitHold_ = false;
            HandleDialWhenHolding(callId, call);
        } else {
            TelConferenceState confState = call->GetTelConferenceState();
            int32_t conferenceId = ERR_ID;
            call->GetMainCallId(conferenceId);
            if (confState != TelConferenceState::TEL_CONFERENCE_IDLE && conferenceId == callId) {
                AutoHandleForDsda(canSwitchCallState, priorState, activeCallNum, call->GetSlotId(), false);
            } else if (confState == TelConferenceState::TEL_CONFERENCE_IDLE) {
                AutoHandleForDsda(canSwitchCallState, priorState, activeCallNum, call->GetSlotId(), false);
            }
        }
    }
    return ret;
}

void CallStatusManager::HandleDialWhenHolding(int32_t callId, sptr<CallBase> &call)
{
    auto callRequestEventHandler = DelayedSingleton<CallRequestEventHandlerHelper>::GetInstance();
    if (callRequestEventHandler->IsPendingHangup()) {
        sptr<CallBase> holdCall = CallObjectManager::GetOneCallObject(callId);
        if (holdCall != nullptr) {
            holdCall->UnHoldCall();
        }
        int32_t pendingHangupCallId = callRequestEventHandler->GetPendingHangupCallId();
        sptr<CallBase> pendingHangupCall = CallObjectManager::GetOneCallObject(pendingHangupCallId);
        if (pendingHangupCall != nullptr) {
            UpdateCallState(pendingHangupCall, TelCallState::CALL_STATUS_DISCONNECTED);
            DeleteOneCallObject(pendingHangupCallId);
        }
        callRequestEventHandler->SetPendingHangup(false, -1);
    } else {
        int32_t result = DelayedSingleton<CellularCallConnection>::GetInstance()->Dial(GetDialCallInfo());
        sptr<CallBase> dialCall = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING);
        if (result != TELEPHONY_SUCCESS && dialCall != nullptr) {
            DealFailDial(call);
            TELEPHONY_LOGI("Dial call fail");
        }
    }
}

bool CallStatusManager::IsDcCallConneceted()
{
    std::string dcStatus = "";
    auto settingHelper = SettingsDataShareHelper::GetInstance();
    if (settingHelper != nullptr) {
        OHOS::Uri settingUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI);
        settingHelper->Query(settingUri, "distributed_modem_state", dcStatus);
    }
    if (dcStatus == "1_sink" || dcStatus == "1_source") {
        return true;
    }
    return false;
}

bool CallStatusManager::IsDistributeCallSourceStatus()
{
    std::string dcStatus = "";
    auto settingHelper = SettingsDataShareHelper::GetInstance();
    if (settingHelper != nullptr) {
        OHOS::Uri settingUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI);
        settingHelper->Query(settingUri, "distributed_modem_state", dcStatus);
    }
    if (dcStatus == "1_source") {
        return true;
    }
    return false;
}

void CallStatusManager::SetBtCallDialByPhone(const sptr<CallBase> &call, bool isBtCallDialByPhone)
{
    if (call == nullptr) {
        return;
    }
    CallAttributeInfo info;
    call->GetCallAttributeInfo(info);
    AAFwk::WantParams object = AAFwk::WantParamWrapper::ParseWantParamsWithBrackets(info.extraParamsString);
    object.SetParam("isBtCallDialByPhone", AAFwk::Boolean::Box(isBtCallDialByPhone));
    call->SetExtraParams(object);
    call->GetCallAttributeBaseInfo(info);
}

void CallStatusManager::BtCallDialingHandle(sptr<CallBase> &call, const CallDetailInfo &info)
{
    if (call != nullptr) {
        call->SetPhoneOrWatchDial(static_cast<int32_t>(PhoneOrWatchDial::WATCH_DIAL));
        SetBtCallDialByPhone(call, false);
    } else {
        call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
        if (call != nullptr) {
            call->SetPhoneOrWatchDial(static_cast<int32_t>(PhoneOrWatchDial::PHONE_DIAL));
            SetBtCallDialByPhone(call, true);
        }
    }
}

void CallStatusManager::SetDistributedDeviceDialing(sptr<CallBase> call, bool isDistributedDeviceDialing)
{
    if (call == nullptr) {
        return;
    }
    if (isDistributedDeviceDialing) {
        AAFwk::WantParams extraParams;
        extraParams.SetParam("isDistributedDeviceDialing", AAFwk::String::Box("true"));
        call->SetExtraParams(extraParams);
    }
}

void CallStatusManager::BtCallDialingHandleFirst(sptr<CallBase> call, const CallDetailInfo &info)
{
    if (call == nullptr) {
        return;
    }
    call->SetPhoneOrWatchDial(static_cast<int32_t>(PhoneOrWatchDial::WATCH_DIAL));
    SetBtCallDialByPhone(call, false);
    if (info.state == TelCallState::CALL_STATUS_ALERTING) {
        UpdateDialingCallInfo(info);
        if (call->GetTelCallState() == TelCallState::CALL_STATUS_ALERTING) {
            call->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
        }
    }
}

int32_t CallStatusManager::HandleCallReportInfoEx(const CallDetailInfo &info)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    switch (info.state) {
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
    return ret;
}

void CallStatusManager::ClearPendingState(sptr<CallBase> &call)
{
    auto callRequestEventHandler = DelayedSingleton<CallRequestEventHandlerHelper>::GetInstance();
    int32_t callId = call->GetCallID();
    TELEPHONY_LOGI("check clear pending state callId = %{public}d", callId);
    if (callRequestEventHandler->HasPendingMo(callId)) {
        callRequestEventHandler->SetPendingMo(false, -1);
    }
    if (callRequestEventHandler->HasPendingHangup(callId)) {
        if (call->GetCallRunningState() != CallRunningState::CALL_RUNNING_STATE_ENDED) {
            call->HangUpCall();
        }
        callRequestEventHandler->SetPendingHangup(false, -1);
    }
}

void CallStatusManager::RefreshCallDisconnectReason(const sptr<CallBase> &call, int32_t reason)
{
    switch (reason) {
        case static_cast<int32_t>(RilDisconnectedReason::DISCONNECTED_REASON_ANSWERED_ELSEWHER):
            if (DelayedSingleton<DistributedCommunicationManager>::GetInstance()->IsSinkRole()) {
                call->SetAnswerType(CallAnswerType::CALL_ANSWERED_ELSEWHER);
                TELEPHONY_LOGI("call answered elsewhere");
            }
            break;
        default:
            break;
    }
}

bool CallStatusManager::IsFromTheSameNumberAtTheSameTime(const sptr<CallBase> &newCall)
{
    if (newCall == nullptr) {
        return false;
    }
    // get the old call obj with phoneNumber of new call.
    std::string phoneNumber = newCall->GetAccountNumber();
    sptr<CallBase> oldCall = GetOneCallObject(phoneNumber);
    if (oldCall == nullptr) {
        return false;
    }
    // compare old with new in scene of one-number-dual-terminal
    if ((abs(newCall->GetCallID() - oldCall->GetCallID()) != 1)) {
        return false;
    }
    CallAttributeInfo oldAttrInfo;
    oldCall->GetCallAttributeBaseInfo(oldAttrInfo);
    if (oldAttrInfo.callState != TelCallState::CALL_STATUS_INCOMING) {
        return false;
    }
    CallAttributeInfo newAttrInfo;
    newCall->GetCallAttributeBaseInfo(newAttrInfo);
    constexpr int32_t CALLS_COMING_MAX_INTERVAL_DURATION = 10 * 1000;
    if ((newAttrInfo.callCreateTime - oldAttrInfo.callCreateTime) > CALLS_COMING_MAX_INTERVAL_DURATION) {
        return false;
    }
    TELEPHONY_LOGI("the new call is from the same number at the same time.");
    return true;
}

void CallStatusManager::ModifyEsimType()
{
    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    if (datashareHelper == nullptr) {
        return;
    }
    OHOS::Uri settingUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI);
    std::string esimCardType = "";
    
    constexpr const char* ESIM_TYPE_ONE_NUMBER_DUAL_TERMINAL = "1";
    int32_t retCode = datashareHelper->Query(settingUri, "key_esim_card_type", esimCardType);
    if ((retCode == TELEPHONY_SUCCESS) && (esimCardType == ESIM_TYPE_ONE_NUMBER_DUAL_TERMINAL ||
        esimCardType == "")) {
        TELEPHONY_LOGI("do not need to fix esim card type, current type: %{public}s", esimCardType.c_str());
        return;
    }
   
    retCode = datashareHelper->Update(settingUri, "key_esim_card_type", ESIM_TYPE_ONE_NUMBER_DUAL_TERMINAL);
    if (retCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("update esim card type failed, retCode: %{public}d", retCode);
        return;
    }
    std::string manullySetState = "";
    retCode = datashareHelper->Query(settingUri, "key_manually_set_net", manullySetState);
    if (manullySetState == "") {
        manullySetState = "000";
    }
    constexpr int32_t STATUS_VALUE_LEN = 3;
    const char *changeStatus = manullySetState.c_str();
    if (strlen(changeStatus) != STATUS_VALUE_LEN) {
        return;
    }
    constexpr char SMARTOFF_NETWORK_MODIFIED = '1';
    int32_t index = 0;
    if (changeStatus[index++] != SMARTOFF_NETWORK_MODIFIED) {
        retCode = datashareHelper->Update(settingUri, "key_smartoff_network_bluetooth_connection", "1");
    }
    if (changeStatus[index++] != SMARTOFF_NETWORK_MODIFIED) {
        retCode = datashareHelper->Update(settingUri, "key_smartoff_network_not_wear", "true");
    }
    if (changeStatus[index++] != SMARTOFF_NETWORK_MODIFIED) {
        retCode = datashareHelper->Update(settingUri, "key_smartoff_network_sleep_mode", "true");
    }
}

int32_t CallStatusManager::RefreshOldCall(const CallDetailInfo &info, bool &isExistedOldCall)
{
    TELEPHONY_LOGI("RefreshOldCall enter.");
    sptr<CallBase> call = GetOneCallObjectByIndexSlotIdAndCallType(info.index, info.accountId, info.callType);
    if (call == nullptr) {
        isExistedOldCall = false;
        return TELEPHONY_SUCCESS;
    }
    isExistedOldCall = true;
    auto oldCallType = call->GetCallType();
    auto videoState = call->GetVideoStateType();
    if (oldCallType != info.callType || call->GetTelCallState() != info.state || videoState != info.callMode) {
        call = RefreshCallIfNecessary(call, info);
        if (oldCallType != info.callType || videoState != info.callMode) {
            return UpdateCallState(call, info.state);
        }
    }
    
    return TELEPHONY_SUCCESS;
}

void CallStatusManager::AutoAnswerSecondCall()
{
    std::list<sptr<CallBase>> allCallList = CallObjectManager::GetAllCallList();
    for (auto call : allCallList) {
        if (call->GetTelCallState() != TelCallState::CALL_STATUS_INCOMING &&
            call->GetTelCallState() != TelCallState::CALL_STATUS_WAITING) {
            continue;
        }
        if (call->GetAutoAnswerState()) {
            TELEPHONY_LOGI("Auto AnswerCall callid=%{public}d", call->GetCallID());
            int ret = DelayedSingleton<CallControlManager>::GetInstance()->AnswerCall(call->GetCallID(),
                static_cast<int32_t>(call->GetVideoStateType()));
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("Auto AnswerCall failed callid=%{public}d", call->GetCallID());
            }
            return;
        }
    }
    return;
}
void CallStatusManager::OneCallAnswerAtPhone(int32_t callId)
{
    TELEPHONY_LOGI("One Call AnswerCall AtPhone callid=%{public}d", callId);
    if (CallObjectManager::IsOneNumberDualTerminal()) {
        return;
    }
    if (!CallObjectManager::IsTwoCallBtCallAndESIM()) {
        return;
    }
    std::list<sptr<CallBase>> allCallList = CallObjectManager::GetAllCallList();
    for (auto call : allCallList) {
        if (call->GetCallID() == callId) {
            continue;
        }
        if (call->GetTelCallState() == TelCallState::CALL_STATUS_DIALING ||
            call->GetTelCallState() == TelCallState::CALL_STATUS_ALERTING ||
            call->GetTelCallState() == TelCallState::CALL_STATUS_ACTIVE) {
            int32_t ret = DelayedSingleton<CallControlManager>::GetInstance()->HangUpCall(call->GetCallID());
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("One Call AnswerCall AtPhone HangUpCall failed callid=%{public}d", call->GetCallID());
            }
        }
        if (call->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING ||
            call->GetTelCallState() == TelCallState::CALL_STATUS_WAITING) {
            TELEPHONY_LOGI("Need AnswerCall callid=%{public}d", call->GetCallID());
            int32_t ret = DelayedSingleton<CallControlManager>::GetInstance()->RejectCall(
                call->GetCallID(), false, u"");
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("One Call AnswerCall AtPhone RejectCall failed callid=%{public}d", call->GetCallID());
            }
        }
    }
    return;
}

bool CallStatusManager::IsCallMotionRecognitionEnable(const std::string& key)
{
    std::vector<int> activedOsAccountIds;
    OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(activedOsAccountIds);
    if (activedOsAccountIds.empty()) {
        TELEPHONY_LOGW("activedOsAccountIds is empty");
        return false;
    }
    int userId = activedOsAccountIds[0];
    OHOS::Uri uri(
        "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_"
        + std::to_string(userId) + "?Proxy=true");
    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    std::string is_motion_switch_on {"0"};
    int resp = datashareHelper->Query(uri, key, is_motion_switch_on);
    TELEPHONY_LOGI("key = %{public}s, value = %{public}s", key.c_str(), is_motion_switch_on.c_str());
    if (resp == TELEPHONY_SUCCESS && is_motion_switch_on == "1") {
        return true;
    }
    return false;
}

void CallStatusManager::StartInComingCallMotionRecognition()
{
    bool isPickupReduceVolumeSwitchOn =
        IsCallMotionRecognitionEnable(SettingsDataShareHelper::QUERY_MOTION_PICKUP_REDUCE_KEY);
    bool isFlipMuteSwitchOn = IsCallMotionRecognitionEnable(SettingsDataShareHelper::QUERY_MOTION_FLIP_MUTE_KEY);
    bool isCloseToEarQuickAnswerSwitchOn =
        IsCallMotionRecognitionEnable(SettingsDataShareHelper::QUERY_MOTION_CLOSE_TO_EAR_KEY);
    if (isPickupReduceVolumeSwitchOn || isFlipMuteSwitchOn || isCloseToEarQuickAnswerSwitchOn) {
        if (!Rosen::LoadMotionSensor()) {
            TELEPHONY_LOGE("LoadMotionSensor failed");
            return;
        }
        TELEPHONY_LOGI("LoadMotionSensor success");
        if (isPickupReduceVolumeSwitchOn) {
            MotionRecogntion::SubscribePickupSensor();
        }
        if (isFlipMuteSwitchOn) {
            MotionRecogntion::SubscribeFlipSensor();
        }
        if (isCloseToEarQuickAnswerSwitchOn) {
            MotionRecogntion::SubscribeCloseToEarSensor();
        }
    }
}

void CallStatusManager::StartOutGoingCallMotionRecognition()
{
    bool isCloseToEarQuickAnswerSwitchOn =
        IsCallMotionRecognitionEnable(SettingsDataShareHelper::QUERY_MOTION_CLOSE_TO_EAR_KEY);
    if (isCloseToEarQuickAnswerSwitchOn) {
        if (!Rosen::LoadMotionSensor()) {
            TELEPHONY_LOGE("LoadMotionSensor failed");
            return;
        }
        TELEPHONY_LOGI("LoadMotionSensor success");
        MotionRecogntion::SubscribeCloseToEarSensor();
    }
}
} // namespace Telephony
} // namespace OHOS
