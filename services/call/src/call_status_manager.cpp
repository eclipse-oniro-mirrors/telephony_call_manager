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

#include "call_status_manager.h"

#include <securec.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "cellular_call_info_handler.h"
#include "cs_call.h"
#include "ims_call.h"
#include "ott_call.h"
#include "audio_control_manager.h"
#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
CallStatusManager::CallStatusManager()
{
    (void)memset_s(&callReportInfo_, sizeof(CallReportInfo), 0, sizeof(CallReportInfo));
    (void)memset_s(&callsReportInfo_, sizeof(CallsReportInfo), 0, sizeof(CallsReportInfo));
}

CallStatusManager::~CallStatusManager()
{
    UnInit();
}

int32_t CallStatusManager::Init()
{
    callsReportInfo_.callVec.clear();
    mEventIdTransferMap_.clear();
    mEventIdTransferMap_[RequestResultEventId::RESULT_HOLD_SEND_FAILED] =
        CallAbilityEventId::EVENT_HOLD_SEND_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_ACTIVE_SEND_FAILED] =
        CallAbilityEventId::EVENT_ACTIVE_SEND_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_INITIATE_DTMF_SUCCESS] =
        CallAbilityEventId::EVENT_START_DTMF_SUCCESS;
    mEventIdTransferMap_[RequestResultEventId::RESULT_INITIATE_DTMF_FAILED] =
        CallAbilityEventId::EVENT_START_DTMF_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_CEASE_DTMF_SUCCESS] =
        CallAbilityEventId::EVENT_STOP_DTMF_SUCCESS;
    mEventIdTransferMap_[RequestResultEventId::RESULT_CEASE_DTMF_FAILED] =
        CallAbilityEventId::EVENT_STOP_DTMF_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_TRANSMIT_DTMF_SUCCESS] =
        CallAbilityEventId::EVENT_SEND_DTMF_SUCCESS;
    mEventIdTransferMap_[RequestResultEventId::RESULT_TRANSMIT_DTMF_FAILED] =
        CallAbilityEventId::EVENT_SEND_DTMF_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_TRANSMIT_DTMF_STRING_SUCCESS] =
        CallAbilityEventId::EVENT_SEND_DTMF_STRING_SUCCESS;
    mEventIdTransferMap_[RequestResultEventId::RESULT_TRANSMIT_DTMF_STRING_FAILED] =
        CallAbilityEventId::EVENT_SEND_DTMF_STRING_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_DIAL_SEND_FAILED] =
        CallAbilityEventId::EVENT_DIAL_AT_CMD_SEND_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_DIAL_NO_CARRIER] = CallAbilityEventId::EVENT_DIAL_NO_CARRIER;
    mEventIdTransferMap_[RequestResultEventId::RESULT_SET_CALL_PREFERENCE_MODE_SUCCESS] =
        CallAbilityEventId::EVENT_SET_CALL_PREFERENCE_MODE_SUCCESS;
    mEventIdTransferMap_[RequestResultEventId::RESULT_SET_CALL_PREFERENCE_MODE_FAILED] =
        CallAbilityEventId::EVENT_SET_CALL_PREFERENCE_MODE_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_SET_LTE_IMS_SWITCH_STATUS_SUCCESS] =
        CallAbilityEventId::EVENT_SET_IMS_VOLTE_SUCCESS;
    mEventIdTransferMap_[RequestResultEventId::RESULT_SET_LTE_IMS_SWITCH_STATUS_FAILED] =
        CallAbilityEventId::EVENT_SET_IMS_VOLTE_FAILED;
    mEventIdTransferMap_[RequestResultEventId::RESULT_GET_IMS_CALLS_DATA_FAILED] =
        CallAbilityEventId::EVENT_GET_IMS_DATA_FAILED;
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::UnInit()
{
    callsReportInfo_.callVec.clear();
    mEventIdTransferMap_.clear();
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::HandleCallReportInfo(const CallReportInfo &info)
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
        case TelCallState::CALL_STATUS_DIALING:
            ret = DialingHandle(info);
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            ret = AlertHandle(info);
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            ret = IncomingHandle(info);
            break;
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

int32_t CallStatusManager::HandleCallsReportInfo(const CallsReportInfo &info)
{
    bool flag = false;
    TELEPHONY_LOGI("call list size:%{public}zu,slotId:%{public}d", info.callVec.size(), info.slotId);
    for (auto &it : info.callVec) {
        for (auto &it1 : callsReportInfo_.callVec) {
            if (strcmp(it.accountNum, it1.accountNum) == 0) {
                TELEPHONY_LOGI("state:%{public}d", it.state);
                if (it.state != it1.state) {
                    TELEPHONY_LOGI("handle updated call state:%{public}d", it.state);
                    HandleCallReportInfo(it);
                }
                flag = true;
                break;
            }
        }
        if (!flag || callsReportInfo_.callVec.empty()) {
            TELEPHONY_LOGI("handle new call state:%{public}d", it.state);
            HandleCallReportInfo(it);
        }
        flag = false;
    }

    for (auto &it2 : callsReportInfo_.callVec) {
        for (auto &it3 : info.callVec) {
            if (strcmp(it2.accountNum, it3.accountNum) == 0) {
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
    callsReportInfo_.callVec.clear();
    callsReportInfo_ = info;
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::HandleDisconnectedCause(int32_t cause)
{
    bool ret = DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallDestroyed(cause);
    if (!ret) {
        TELEPHONY_LOGE("NotifyCallDestroyed failed!");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::HandleEventResultReportInfo(const CellularCallEventInfo &info)
{
    if (info.eventType != EVENT_REQUEST_RESULT_TYPE) {
        return TELEPHONY_ERR_FAIL;
    }
    TELEPHONY_LOGI("recv one Event, eventId:%{public}d", info.eventId);
    CallEventInfo eventInfo;
    eventInfo.eventId = CallAbilityEventId::EVENT_UNKNOW_ID;
    if (mEventIdTransferMap_.find(info.eventId) != mEventIdTransferMap_.end()) {
        eventInfo.eventId = mEventIdTransferMap_[info.eventId];
    } else {
        TELEPHONY_LOGE("unkown type Event, eventid %{public}d", info.eventId);
    }
    if (eventInfo.eventId != CallAbilityEventId::EVENT_UNKNOW_ID) {
        DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallEventUpdated(eventInfo);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::IncomingHandle(const CallReportInfo &info)
{
    TELEPHONY_LOGI("handle incoming state");
    int32_t ret = IncomingHandlePolicy(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("IncomingHandlePolicy failed!");
        return ret;
    }
    sptr<CallBase> call = CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    if (call == nullptr) {
        TELEPHONY_LOGE("CreateNewCall failed!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
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
        return ret;
    }
    ret = FilterResultsDispose(call);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("FilterResultsDispose failed!");
    }
    return ret;
}

int32_t CallStatusManager::DialingHandle(const CallReportInfo &info)
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
        return CALL_ERR_CALL_OBJECT_IS_NULL;
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

int32_t CallStatusManager::ActiveHandle(const CallReportInfo &info)
{
    TELEPHONY_LOGI("handle active state");
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    int32_t ret = call->LunchConference();
    if (ret == TELEPHONY_SUCCESS) {
        int32_t mainCallId = call->GetMainCallId();
        sptr<CallBase> mainCall = GetOneCallObject(mainCallId);
        if (mainCall != nullptr) {
            mainCall->SetTelConferenceState(TEL_CONFERENCE_ACTIVE);
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
    return ret;
}

int32_t CallStatusManager::HoldingHandle(const CallReportInfo &info)
{
    TELEPHONY_LOGI("handle holding state");
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    int32_t ret = call->ExitConference();
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("SubCallSeparateFromConference success");
    }
    ret = UpdateCallState(call, TelCallState::CALL_STATUS_HOLDING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
    }
    return ret;
}

int32_t CallStatusManager::WaitingHandle(const CallReportInfo &info)
{
    return IncomingHandle(info);
}

int32_t CallStatusManager::AlertHandle(const CallReportInfo &info)
{
    TELEPHONY_LOGI("handle alerting state");
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
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

int32_t CallStatusManager::DisconnectingHandle(const CallReportInfo &info)
{
    TELEPHONY_LOGI("handle disconnecting state");
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    int32_t ret = UpdateCallState(call, TelCallState::CALL_STATUS_DISCONNECTING);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallState failed, errCode:%{public}d", ret);
    }
    return ret;
}

int32_t CallStatusManager::DisconnectedHandle(const CallReportInfo &info)
{
    TELEPHONY_LOGI("handle disconnected state");
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
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
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    TelCallState priorState = call->GetTelCallState();
    TELEPHONY_LOGI("priorState:%{public}d, nextState:%{public}d", priorState, nextState);
    int32_t ret = call->SetTelCallState(nextState);
    if (ret != TELEPHONY_SUCCESS && ret != CALL_ERR_NOT_NEW_STATE) {
        TELEPHONY_LOGE("SetTelCallState failed!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    // State radio
    if (!DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallStateUpdated(call, priorState, nextState)) {
        TELEPHONY_LOGE(
            "NotifyCallStateUpdated failed! priorState:%{public}d,nextState:%{public}d", priorState, nextState);
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallStatusManager::ToSpeakerPhone(sptr<CallBase> &call)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    if (call == nullptr) {
        TELEPHONY_LOGE("Call is NULL");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    if (call->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_DIALING) {
        TELEPHONY_LOGI("Call is CALL_STATUS_DIALING");
        return ret;
    }
    if (call->IsSpeakerphoneOn()) {
        DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioDevice(DEVICE_SPEAKER);
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

sptr<CallBase> CallStatusManager::CreateNewCall(const CallReportInfo &info, CallDirection dir)
{
    sptr<CallBase> callPtr = nullptr;
    DialParaInfo paraInfo;
    AppExecFwk::PacMap extras;
    extras.Clear();
    paraInfo.isEcc = false;
    paraInfo.dialType = DialType::DIAL_CARRIER_TYPE;
    if (dir == CallDirection::CALL_DIRECTION_OUT) {
        DelayedSingleton<CallControlManager>::GetInstance()->GetDialParaInfo(paraInfo, extras);
    }
    paraInfo.number = info.accountNum;
    paraInfo.callId = GetNewCallId();
    paraInfo.index = info.index;
    paraInfo.videoState = info.callMode;
    paraInfo.accountId = info.accountId;
    paraInfo.callType = info.callType;
    paraInfo.callState = info.state;
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
} // namespace Telephony
} // namespace OHOS
