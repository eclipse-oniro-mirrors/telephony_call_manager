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
#include "call_manager_log.h"

#include "cellular_call_info_handler.h"
#include "cs_call.h"
#include "ims_call.h"
#include "ott_call.h"
#include "audio_control_manager.h"
#include "call_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
using namespace TelephonyCallManager;

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
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusManager::UnInit()
{
    callsReportInfo_.callVec.clear();
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusManager::HandleCallReportInfo(const CallReportInfo &info)
{
    int32_t ret = TELEPHONY_FAIL;
    callReportInfo_ = info;
    switch (info.state) {
        case TelCallStates::CALL_STATUS_ACTIVE:
            ret = ActiveHandle(info);
            break;
        case TelCallStates::CALL_STATUS_HOLDING:
            ret = HoldingHandle(info);
            break;
        case TelCallStates::CALL_STATUS_DIALING:
            ret = DialingHandle(info);
            break;
        case TelCallStates::CALL_STATUS_ALERTING:
            ret = AlertHandle(info);
            break;
        case TelCallStates::CALL_STATUS_INCOMING:
            ret = IncomingHandle(info);
            break;
        case TelCallStates::CALL_STATUS_WAITING:
            ret = WaitingHandle(info);
            break;
        case TelCallStates::CALL_STATUS_DISCONNECTED:
            ret = DisconnectedHandle(info);
            break;
        case TelCallStates::CALL_STATUS_DISCONNECTING:
            ret = DisconnectingHandle(info);
            break;
        case TelCallStates::CALL_STATUS_IDLE:
            ret = IdleHandle(info);
            break;
        default:
            CALLMANAGER_ERR_LOG("Invalid call state!");
            break;
    }
    return ret;
}

int32_t CallStatusManager::HandleCallsReportInfo(const CallsReportInfo &info)
{
    bool flag = false;
    for (auto &it : info.callVec) {
        for (auto &it1 : callsReportInfo_.callVec) {
            if (strcmp(it.accountNum, it1.accountNum) == 0) {
                CALLMANAGER_DEBUG_LOG("state:%{public}d", it.state);
                if (it.state != it1.state) {
                    HandleCallReportInfo(it);
                }
                flag = true;
                break;
            }
        }
        if (!flag || callsReportInfo_.callVec.empty()) {
            HandleCallReportInfo(it);
        }
        flag = false;
    }

    for (auto &it2 : callsReportInfo_.callVec) {
        for (auto &it3 : info.callVec) {
            if (strcmp(it2.accountNum, it3.accountNum) == 0) {
                CALLMANAGER_DEBUG_LOG("state:%{public}d", it2.state);
                flag = true;
                break;
            }
        }
        if (!flag) {
            it2.state = TelCallStates::CALL_STATUS_DISCONNECTED;
            HandleCallReportInfo(it2);
        }
        flag = false;
    }
    callsReportInfo_.callVec.clear();
    callsReportInfo_ = info;
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusManager::IncomingHandle(const CallReportInfo &info)
{
    int32_t ret = TELEPHONY_FAIL;
    CallInfo callInfo;
    ContactInfo contactInfo;
    sptr<CallBase> baseCall = nullptr;
    (void)memset_s(&callInfo, sizeof(CallInfo), 0, sizeof(CallInfo));
    (void)memset_s(&contactInfo, sizeof(ContactInfo), 0, sizeof(ContactInfo));
#ifdef ABILITY_SIM_SUPPORT
    // According to the SIM card information to determine whether triple-management
    IsTripleManaged(accountId);
#endif
    if (strlen(info.accountNum) == 0) {
        CALLMANAGER_ERR_LOG("phone number is NULL!");
        return CALL_MANAGER_PHONENUM_NULL;
    }
    std::string tmpStr(info.accountNum);
    if (IsCallExist(tmpStr)) {
        CALLMANAGER_ERR_LOG("the call already exists!");
        return CALL_MANAGER_CALL_EXIST;
    }
    ret = InitCallInfo(callInfo, info);
    CALLMANAGER_DEBUG_LOG("Call id is %{public}d", callInfo.callId);
    ret = CreateCall(info.callType, baseCall, callInfo);

#ifdef ABILITY_DATABASE_SUPPORT
    // allow list filtering
    // Get the contact data from the database
    GetCallerInfoDate(ContactInfo);
#endif
    baseCall->SetCallerInfo(contactInfo);
    if (baseCall->GetState() == CallStateType::CALL_STATE_ENDED_TYPE ||
        baseCall->GetState() == CallStateType::CALL_STATE_ENDING_TYPE) {
        return CALL_MANAGER_CALL_DISCONNECTED;
    } else {
        ret = UpdateCallState(baseCall, TelCallStates::CALL_STATUS_INCOMING);
    }
    ret = FilterResultsDispose(baseCall);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("FilterResultsDispose return err!");
        return ret;
    }
    return ret;
}

int32_t CallStatusManager::DialingHandle(const CallReportInfo &info)
{
    int32_t ret = TELEPHONY_FAIL;
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        CALLMANAGER_ERR_LOG("Call is NULL");
        return CALL_MANAGER_CALL_NULL;
    }
    ret = UpdateCallState(call, TelCallStates::CALL_STATUS_DIALING);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("UpdateCallState return err, errCode:%{public}d", ret);
    }
    return ret;
}

int32_t CallStatusManager::ActiveHandle(const CallReportInfo &info)
{
    int32_t ret = TELEPHONY_FAIL;
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        CALLMANAGER_ERR_LOG("Call is NULL");
        return CALL_MANAGER_CALL_NULL;
    }
    ret = UpdateCallState(call, TelCallStates::CALL_STATUS_ACTIVE);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("UpdateCallState return err, errCode:%{public}d", ret);
        return ret;
    }
    ToSpeakerPhone(call);
    DelayedSingleton<AudioControlManager>::GetInstance()->SetVolumeAudible();
    return ret;
}

int32_t CallStatusManager::HoldingHandle(const CallReportInfo &info)
{
    int32_t ret = TELEPHONY_FAIL;
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        CALLMANAGER_ERR_LOG("Call is NULL");
        return CALL_MANAGER_CALL_NULL;
    }
    ret = UpdateCallState(call, TelCallStates::CALL_STATUS_HOLDING);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("UpdateCallState return err, errCode:%{public}d", ret);
    }
    return ret;
}

int32_t CallStatusManager::WaitingHandle(const CallReportInfo &info)
{
    int32_t ret = TELEPHONY_FAIL;
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        CALLMANAGER_ERR_LOG("Call is NULL");
        return CALL_MANAGER_CALL_NULL;
    }
    ret = UpdateCallState(call, TelCallStates::CALL_STATUS_WAITING);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("UpdateCallState return err, errCode:%{public}d", ret);
    }
    return ret;
}

int32_t CallStatusManager::AlertHandle(const CallReportInfo &info)
{
    int32_t ret = TELEPHONY_FAIL;
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        CALLMANAGER_ERR_LOG("Call is NULL");
        return CALL_MANAGER_CALL_NULL;
    }
    ret = UpdateCallState(call, TelCallStates::CALL_STATUS_ALERTING);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("UpdateCallState return err, errCode:%{public}d", ret);
        return ret;
    }
    ToSpeakerPhone(call);
    TurnOffMute(call);
    DelayedSingleton<AudioControlManager>::GetInstance()->SetVolumeAudible();
    return ret;
}

int32_t CallStatusManager::DisconnectingHandle(const CallReportInfo &info)
{
    int32_t ret = TELEPHONY_FAIL;
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        CALLMANAGER_ERR_LOG("Call is NULL");
        return CALL_MANAGER_CALL_NULL;
    }
    ret = UpdateCallState(call, TelCallStates::CALL_STATUS_DISCONNECTING);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("UpdateCallState return err, errCode:%{public}d", ret);
    }
    return ret;
}

int32_t CallStatusManager::DisconnectedHandle(const CallReportInfo &info)
{
    int32_t ret = TELEPHONY_FAIL;
    std::string tmpStr(info.accountNum);
    sptr<CallBase> call = GetOneCallObject(tmpStr);
    if (call == nullptr) {
        CALLMANAGER_ERR_LOG("Call is NULL");
        return CALL_MANAGER_CALL_NULL;
    }
    ret = UpdateCallState(call, TelCallStates::CALL_STATUS_DISCONNECTED);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("UpdateCallState return err, errCode:%{public}d", ret);
        return ret;
    }
    DeleteOneCallObject(call);
    return ret;
}

int32_t CallStatusManager::IdleHandle(const CallReportInfo &info)
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusManager::ConferenceHandle()
{
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusManager::UpdateCallState(sptr<CallBase> &call, TelCallStates nextState)
{
    int32_t ret = TELEPHONY_FAIL;
    CallInfo info;
    if (call == nullptr) {
        CALLMANAGER_ERR_LOG("Call is NULL");
        return CALL_MANAGER_CALL_NULL;
    }
    if (!call->GetBaseCallInfo(info)) {
        CALLMANAGER_ERR_LOG("GetBaseCallInfo failed!");
        return ret;
    }
    TelCallStates priorState = info.state;
    CALLMANAGER_DEBUG_LOG("priorState:%{public}d,nextState:%{public}d", priorState, nextState);
    if (priorState == nextState) {
        CALLMANAGER_INFO_LOG("Call state duplication %d", nextState);
        return CALL_MANAGER_NOT_NEW_STATE;
    }
    // need DTMF judge
    call->SetState(nextState);

    // State radio
    if (!DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallStateUpdated(call, priorState, nextState)) {
        CALLMANAGER_ERR_LOG(
            "NotifyCallStateUpdated failed! priorState:%{public}d,nextState:%{public}d", priorState, nextState);
        return ret;
    }
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusManager::ToSpeakerPhone(sptr<CallBase> &call)
{
    int32_t ret = TELEPHONY_FAIL;
    if (call == nullptr) {
        CALLMANAGER_ERR_LOG("Call is NULL");
        return CALL_MANAGER_CALL_NULL;
    }
    if (call->GetState() == CallStateType::CALL_STATE_DIALING_TYPE) {
        CALLMANAGER_INFO_LOG("Call is CALL_STATUS_DIALING");
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
        DelayedSingleton<AudioControlManager>::GetInstance()->UpdateMuteState(false);
    } else {
        DelayedSingleton<AudioControlManager>::GetInstance()->UpdateMuteState(true);
    }
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusManager::InitCallInfo(CallInfo &callInfo, const CallReportInfo &info)
{
    (void)memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo));
    callInfo.speakerphoneOn = false;
    callInfo.dialScene = CALL_NORMAL;
    callInfo.isDefault = false;
    callInfo.isEcc = false;
    callInfo.startime = 0;
    callInfo.policyFlags = 0;
    callInfo.callId = GetNextCallId();
    callInfo.accountId = info.accountId;
    callInfo.videoState = 0;
    if (memcpy_s(callInfo.phoneNum, kMaxNumberLen, info.accountNum, strlen(info.accountNum)) != 0) {
        return TELEPHONY_MEMCPY_FAIL;
    }
    callInfo.callType = info.callType;
    callInfo.state = TelCallStates::CALL_STATUS_IDLE;
    return TELEPHONY_NO_ERROR;
}

int32_t CallStatusManager::CreateCall(const CallType &callType, sptr<CallBase> &call, const CallInfo &info)
{
    if (callType == CallType::TYPE_CS) {
        call = (std::make_unique<CSCall>(info)).release();
    } else if (callType == CallType::TYPE_IMS) {
        call = (std::make_unique<IMSCall>(info)).release();
    } else {
        call = (std::make_unique<OTTCall>(info)).release();
    }
    if (call == nullptr) {
        CALLMANAGER_ERR_LOG("create call object failed!");
        return CALL_MANAGER_CALL_NULL;
    }
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS