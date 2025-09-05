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

#include "call_object_manager.h"

#include "call_connect_ability.h"
#include "call_control_manager.h"
#include "call_manager_errors.h"
#include "call_number_utils.h"
#include "call_wired_headset.h"
#include "conference_base.h"
#include "ims_conference.h"
#include "report_call_info_handler.h"
#include "telephony_log_wrapper.h"
#include "voip_call.h"
#include "fold_status_manager.h"

namespace OHOS {
namespace Telephony {
std::list<sptr<CallBase>> CallObjectManager::callObjectPtrList_;
std::map<int32_t, CallAttributeInfo> CallObjectManager::voipCallObjectList_;
ffrt::mutex CallObjectManager::listMutex_;
int32_t CallObjectManager::callId_ = CALL_START_ID;
std::condition_variable CallObjectManager::cv_;
bool CallObjectManager::isFirstDialCallAdded_ = false;
bool CallObjectManager::needWaitHold_ = false;
CellularCallInfo CallObjectManager::dialCallInfo_;
constexpr int32_t CRS_TYPE = 2;
constexpr uint64_t DISCONNECT_DELAY_TIME = 2000000;
static constexpr const char *VIDEO_RING_PATH_FIX_TAIL = ".mp4";
constexpr int32_t VIDEO_RING_PATH_FIX_TAIL_LENGTH = 4;
static constexpr const char *SYSTEM_VIDEO_RING = "system_video_ring";
#ifdef NOT_SUPPORT_MULTICALL
constexpr int32_t CALL_MAX_COUNT = 2;
#endif
CallObjectManager::CallObjectManager()
{
}

CallObjectManager::~CallObjectManager()
{
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    while (it != callObjectPtrList_.end()) {
        (*it) = nullptr;
        callObjectPtrList_.erase(it++);
    }
    voipCallObjectList_.clear();
}

int32_t CallObjectManager::AddOneCallObject(sptr<CallBase> &call)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("call is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallID() == call->GetCallID()) {
            TELEPHONY_LOGE("this call has existed yet!");
            return CALL_ERR_PHONE_CALL_ALREADY_EXISTS;
        }
    }
    CallAttributeInfo info;
    call->GetCallAttributeInfo(info);
    int32_t state;
    bool isVoIPCallExists = false;
    DelayedSingleton<CallControlManager>::GetInstance()->GetVoIPCallState(state);
    if (state == (int32_t)CallStateToApp::CALL_STATE_RINGING) {
        isVoIPCallExists = true;
    }
    if (callObjectPtrList_.size() == NO_CALL_EXIST && (!isVoIPCallExists || info.isEcc)) {
        DelayedSingleton<CallConnectAbility>::GetInstance()->ConnectAbility();
    }
    callObjectPtrList_.emplace_back(call);
    if (callObjectPtrList_.size() == ONE_CALL_EXIST) {
        DelayedSingleton<CallWiredHeadSet>::GetInstance()->Init();
        if (callObjectPtrList_.front()->GetTelCallState() == TelCallState::CALL_STATUS_DIALING) {
            isFirstDialCallAdded_ = true;
            cv_.notify_all();
        }
    }
    if (FoldStatusManager::IsSmallFoldDevice()) {
        DelayedSingleton<FoldStatusManager>::GetInstance()->RegisterFoldableListener();
    }
    TELEPHONY_LOGI("AddOneCallObject success! callId:%{public}d,call list size:%{public}zu", call->GetCallID(),
        callObjectPtrList_.size());
    return TELEPHONY_SUCCESS;
}

int32_t CallObjectManager::AddOneVoipCallObject(CallAttributeInfo info)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::map<int32_t, CallAttributeInfo>::iterator it = voipCallObjectList_.find(info.callId);
    if (it == voipCallObjectList_.end()) {
        voipCallObjectList_[info.callId] = info;
        TELEPHONY_LOGI("AddOneVoipCallObject success! callList size:%{public}zu", voipCallObjectList_.size());
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGI("AddOneVoipCallObject failed!");
    return CALL_ERR_PHONE_CALL_ALREADY_EXISTS;
}

int32_t CallObjectManager::DeleteOneVoipCallObject(int32_t callId)
{
    std::unique_lock<ffrt::mutex> lock(listMutex_);
    std::map<int32_t, CallAttributeInfo>::iterator it = voipCallObjectList_.find(callId);
    if (it != voipCallObjectList_.end()) {
        voipCallObjectList_.erase(callId);
        TELEPHONY_LOGI("DeleteOneVoipCallObject success! callList size:%{public}zu", voipCallObjectList_.size());
    }
    return TELEPHONY_SUCCESS;
}

bool CallObjectManager::IsVoipCallExist()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    bool res = (voipCallObjectList_.size() != 0);
    TELEPHONY_LOGI("has voip call exist:%{public}d", res);
    return res;
}

bool CallObjectManager::IsVoipCallExist(TelCallState callState, int32_t &callId)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::map<int32_t, CallAttributeInfo>::iterator it;
    for (it = voipCallObjectList_.begin(); it != voipCallObjectList_.end(); ++it) {
        if (it->second.callState == callState) {
            callId = it->first;
            return true;
        }
    }
    TELEPHONY_LOGI("the voip call is does not exist");
    return false;
}

void CallObjectManager::ClearVoipList()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    if (voipCallObjectList_.size() != 0) {
        voipCallObjectList_.clear();
    }
    bool res = DelayedSingleton<CallControlManager>::GetInstance()->SetVirtualCall(true);
    TELEPHONY_LOGI("SetVirtualCall res: %{public}d.", res);
}

int32_t CallObjectManager::UpdateOneVoipCallObjectByCallId(int32_t callId, TelCallState nextCallState)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::map<int32_t, CallAttributeInfo>::iterator it = voipCallObjectList_.find(callId);
    if (it != voipCallObjectList_.end()) {
        it->second.callState = nextCallState;
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGI("UpdateOneVoipCallObjectByCallId failed!");
    return TELEPHONY_ERROR;
}

CallAttributeInfo CallObjectManager::GetVoipCallInfo()
{
    CallAttributeInfo res;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::map<int32_t, CallAttributeInfo>::iterator it = voipCallObjectList_.begin();
    if (it != voipCallObjectList_.end()) {
        res = it->second;
        return res;
    }
    return res;
}

CallAttributeInfo CallObjectManager::GetActiveVoipCallInfo()
{
    CallAttributeInfo res;
    std::map<int32_t, CallAttributeInfo>::iterator it = voipCallObjectList_.begin();
    for (; it != voipCallObjectList_.end(); ++it) {
        if (it->second.callState == TelCallState::CALL_STATUS_ACTIVE) {
            res = it->second;
            return res;
        }
    }
    return res;
}


void CallObjectManager::DelayedDisconnectCallConnectAbility(uint64_t time = DISCONNECT_DELAY_TIME)
{
    ffrt::submit_h(
        []() {
            std::lock_guard<ffrt::mutex> lock(listMutex_);
            TELEPHONY_LOGI("delayed disconnect callback begin");
            auto controlManager = DelayedSingleton<CallControlManager>::GetInstance();
            if (callObjectPtrList_.size() == NO_CALL_EXIST && controlManager->ShouldDisconnectService()) {
                auto callConnectAbility = DelayedSingleton<CallConnectAbility>::GetInstance();
                callConnectAbility->DisconnectAbility();
                TELEPHONY_LOGI("delayed disconnect done");
            }
        },
        {}, {}, ffrt::task_attr().delay(time));
}

int32_t CallObjectManager::DeleteOneCallObject(int32_t callId)
{
    TELEPHONY_LOGI("delete one call object, callId:%{public}d", callId);
    std::unique_lock<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallID() == callId) {
            callObjectPtrList_.erase(it);
            TELEPHONY_LOGI("DeleteOneCallObject success! call list size:%{public}zu", callObjectPtrList_.size());
            break;
        }
    }
    if (callObjectPtrList_.size() == NO_CALL_EXIST) {
        DelayedSingleton<CallWiredHeadSet>::GetInstance()->DeInit();
        if (FoldStatusManager::IsSmallFoldDevice()) {
            DelayedSingleton<FoldStatusManager>::GetInstance()->UnregisterFoldableListener();
        }
        if (DelayedSingleton<CallControlManager>::GetInstance()->ShouldDisconnectService()) {
            lock.unlock();
            DelayedDisconnectCallConnectAbility();
        }
    }
    return TELEPHONY_SUCCESS;
}

void CallObjectManager::DeleteOneCallObject(sptr<CallBase> &call)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("call is null!");
        return;
    }
    std::unique_lock<ffrt::mutex> lock(listMutex_);
    callObjectPtrList_.remove(call);
    if (callObjectPtrList_.size() == NO_CALL_EXIST) {
        if (FoldStatusManager::IsSmallFoldDevice()) {
            DelayedSingleton<FoldStatusManager>::GetInstance()->UnregisterFoldableListener();
        }
        if (DelayedSingleton<CallControlManager>::GetInstance()->ShouldDisconnectService()) {
            lock.unlock();
            DelayedDisconnectCallConnectAbility();
        }
    }
    TELEPHONY_LOGI("DeleteOneCallObject success! callList size:%{public}zu", callObjectPtrList_.size());
}

sptr<CallBase> CallObjectManager::GetOneCallObject(int32_t callId)
{
    sptr<CallBase> retPtr = nullptr;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = CallObjectManager::callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallID() == callId) {
            retPtr = *it;
            break;
        }
    }
    return retPtr;
}

sptr<CallBase> CallObjectManager::GetOneCallObject(std::string &phoneNumber)
{
    if (phoneNumber.empty()) {
        TELEPHONY_LOGE("call is null!");
        return nullptr;
    }
    sptr<CallBase> retPtr = nullptr;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); ++it) {
        std::string networkAddress =
            DelayedSingleton<CallNumberUtils>::GetInstance()->RemovePostDialPhoneNumber((*it)->GetAccountNumber());
        if (networkAddress == phoneNumber) {
            TELEPHONY_LOGI("GetOneCallObject success!");
            retPtr = *it;
            break;
        }
    }
    return retPtr;
}

int32_t CallObjectManager::HasNewCall()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() != CallType::TYPE_VOIP &&
            ((*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_CREATE ||
            (*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_CONNECTING ||
            (*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_DIALING ||
            (*it)->GetCallType() == CallType::TYPE_SATELLITE)) {
            TELEPHONY_LOGE("there is already a new call[callId:%{public}d,state:%{public}d], please redial later",
                (*it)->GetCallID(), (*it)->GetCallRunningState());
            return CALL_ERR_CALL_COUNTS_EXCEED_LIMIT;
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallObjectManager::IsNewCallAllowedCreate(bool &enabled)
{
    enabled = true;
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() != CallType::TYPE_VOIP &&
            ((*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_CREATE ||
            (*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_CONNECTING ||
            (*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_DIALING ||
            (*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_RINGING)) {
            TELEPHONY_LOGE("there is already a new call, please redial later");
            enabled = false;
            return TELEPHONY_ERR_SUCCESS;
        }
    }
    int32_t count = 0;
    int32_t callNum = 2;
#ifdef NOT_SUPPORT_MULTICALL
    callNum = 1;
#endif
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    for (int32_t otherCallId : callIdList) {
        sptr<CallBase> call = GetOneCallObject(otherCallId);
        if (call != nullptr) {
            TelConferenceState confState = call->GetTelConferenceState();
            int32_t conferenceId = DelayedSingleton<ImsConference>::GetInstance()->GetMainCall();
            if (confState != TelConferenceState::TEL_CONFERENCE_IDLE && conferenceId == otherCallId) {
                TELEPHONY_LOGI("there is conference call");
                count++;
            } else if (confState == TelConferenceState::TEL_CONFERENCE_IDLE) {
                count++;
            }
        }
    }
    TELEPHONY_LOGI("the count is:%{public}d", count);
    if (count >= callNum) {
        enabled = false;
    }
    return TELEPHONY_ERR_SUCCESS;
}

int32_t CallObjectManager::GetCurrentCallNum()
{
    int32_t count = 0;
    std::list<int32_t> callIdList;
    GetCarrierCallList(callIdList);
    for (int32_t otherCallId : callIdList) {
        sptr<CallBase> call = GetOneCallObject(otherCallId);
        if (call != nullptr) {
            TelConferenceState confState = call->GetTelConferenceState();
            int32_t conferenceId = DelayedSingleton<ImsConference>::GetInstance()->GetMainCall();
            if (confState != TelConferenceState::TEL_CONFERENCE_IDLE && conferenceId == otherCallId) {
                TELEPHONY_LOGI("there is conference call");
                count++;
            } else if (confState == TelConferenceState::TEL_CONFERENCE_IDLE) {
                count++;
            }
        }
    }
    TELEPHONY_LOGI("the count is %{public}d", count);
    return count;
}

int32_t CallObjectManager::GetCarrierCallList(std::list<int32_t> &list)
{
    list.clear();
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == CallType::TYPE_CS || (*it)->GetCallType() == CallType::TYPE_IMS ||
            (*it)->GetCallType() == CallType::TYPE_SATELLITE ||
            (*it)->GetCallType() == CallType::TYPE_BLUETOOTH) {
            list.emplace_back((*it)->GetCallID());
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallObjectManager::GetVoipCallNum()
{
    int32_t count = 0;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == CallType::TYPE_VOIP) {
            count++;
        }
    }
    return count;
}

int32_t CallObjectManager::GetVoipCallList(std::list<int32_t> &list)
{
    list.clear();
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == CallType::TYPE_VOIP) {
            list.emplace_back((*it)->GetCallID());
        }
    }
    return TELEPHONY_SUCCESS;
}

bool CallObjectManager::HasRingingMaximum()
{
    int32_t ringingCount = 0;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        // Count the number of calls in the ringing state
        if ((*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_RINGING) {
            ringingCount++;
        }
    }
    if (ringingCount >= RINGING_CALL_NUMBER_LEN) {
        return true;
    }
    return false;
}

bool CallObjectManager::HasDialingMaximum()
{
    int32_t dialingCount = 0;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        // Count the number of calls in the active state
        if ((*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_ACTIVE) {
            dialingCount++;
        }
    }
    if (dialingCount >= DIALING_CALL_NUMBER_LEN) {
        return true;
    }
    return false;
}

int32_t CallObjectManager::HasEmergencyCall(bool &enabled)
{
    enabled = false;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetEmergencyState()) {
            enabled = true;
        }
    }
    return TELEPHONY_ERR_SUCCESS;
}

int32_t CallObjectManager::GetNewCallId()
{
    int32_t ret = 0;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    ret = ++callId_;
    return ret;
}

bool CallObjectManager::IsCallExist(int32_t callId)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallID() == callId) {
            TELEPHONY_LOGW("the call is exist.");
            return true;
        }
    }
    return false;
}

bool CallObjectManager::IsCallExist(std::string &phoneNumber)
{
    if (phoneNumber.empty()) {
        return false;
    }
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); ++it) {
        std::string networkAddress =
            DelayedSingleton<CallNumberUtils>::GetInstance()->RemovePostDialPhoneNumber((*it)->GetAccountNumber());
        if (networkAddress == phoneNumber) {
            return true;
        }
    }
    TELEPHONY_LOGI("the call is does not exist.");
    return false;
}

bool CallObjectManager::HasCallExist()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    if (callObjectPtrList_.empty()) {
        TELEPHONY_LOGI("call list size:%{public}zu", callObjectPtrList_.size());
        return false;
    }
    return true;
}

std::list<sptr<CallBase>> CallObjectManager::GetAllCallList()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    return callObjectPtrList_;
}

bool CallObjectManager::HasCellularCallExist()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == CallType::TYPE_CS || (*it)->GetCallType() == CallType::TYPE_IMS ||
            (*it)->GetCallType() == CallType::TYPE_SATELLITE ||
            (*it)->GetCallType() == CallType::TYPE_BLUETOOTH) {
            if ((*it)->GetTelCallState() != TelCallState::CALL_STATUS_DISCONNECTED &&
                (*it)->GetTelCallState() != TelCallState::CALL_STATUS_DISCONNECTING) {
                return true;
            }
        }
    }
    return false;
}

bool CallObjectManager::HasVoipCallExist()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == CallType::TYPE_VOIP) {
            return true;
        }
    }
    return false;
}

bool CallObjectManager::HasIncomingCallCrsType()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_RINGING &&
            (*it)->GetCrsType() == CRS_TYPE) {
            return true;
        }
    }
    return false;
}

bool CallObjectManager::HasIncomingCallVideoRingType()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() != CallType::TYPE_VOIP &&
            (*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_RINGING) {
            ContactInfo contactInfo = (*it)->GetCallerInfo();
            if (IsVideoRing(contactInfo.personalNotificationRingtone, contactInfo.ringtonePath)) {
                return true;
            }
        }
    }
    return false;
}

bool CallObjectManager::IsVideoRing(const std::string &personalNotificationRingtone, const std::string &ringtonePath)
{
    if ((personalNotificationRingtone.length() > VIDEO_RING_PATH_FIX_TAIL_LENGTH &&
        personalNotificationRingtone.substr(personalNotificationRingtone.length() - VIDEO_RING_PATH_FIX_TAIL_LENGTH,
        VIDEO_RING_PATH_FIX_TAIL_LENGTH) == VIDEO_RING_PATH_FIX_TAIL) || ringtonePath == SYSTEM_VIDEO_RING) {
        TELEPHONY_LOGI("Is video ring.");
        return true;
    }
    return false;
}

bool CallObjectManager::HasVideoCall()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetVideoStateType() == VideoStateType::TYPE_VIDEO && (*it)->GetCallType() != CallType::TYPE_VOIP) {
            return true;
        }
    }
    return false;
}

bool CallObjectManager::HasSatelliteCallExist()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == CallType::TYPE_SATELLITE) {
            return true;
        }
    }
    return false;
}

int32_t CallObjectManager::GetSatelliteCallList(std::list<int32_t> &list)
{
    list.clear();
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == CallType::TYPE_SATELLITE) {
            list.emplace_back((*it)->GetCallID());
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallObjectManager::HasRingingCall(bool &hasRingingCall)
{
    hasRingingCall = false;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        // Count the number of calls in the ringing state
        if ((*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_RINGING) {
            hasRingingCall = true;
            break;
        }
    }
    return TELEPHONY_ERR_SUCCESS;
}

int32_t CallObjectManager::HasHoldCall(bool &hasHoldCall)
{
    hasHoldCall = false;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        // Count the number of calls in the hold state
        if ((*it)->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_HOLD) {
            hasHoldCall = true;
            break;
        }
    }
    return TELEPHONY_ERR_SUCCESS;
}

TelCallState CallObjectManager::GetCallState(int32_t callId)
{
    TelCallState retState = TelCallState::CALL_STATUS_IDLE;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = CallObjectManager::callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallID() == callId) {
            retState = (*it)->GetTelCallState();
            break;
        }
    }
    return retState;
}

sptr<CallBase> CallObjectManager::GetOneCallObject(CallRunningState callState)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::reverse_iterator it;
    for (it = callObjectPtrList_.rbegin(); it != callObjectPtrList_.rend(); ++it) {
        if ((*it)->GetCallRunningState() == callState) {
            return (*it);
        }
    }
    return nullptr;
}

sptr<CallBase> CallObjectManager::GetOneCarrierCallObject(CallRunningState callState)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::reverse_iterator it;
    for (it = callObjectPtrList_.rbegin(); it!= callObjectPtrList_.rend(); ++it) {
        if ((*it)->GetCallRunningState() == callState && (*it)->GetCallType() != CallType::TYPE_VOIP) {
            return (*it);
        }
    }
    return nullptr;
}

sptr<CallBase> CallObjectManager::GetOneCallObjectByIndex(int32_t index)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallIndex() == index && (*it)->GetCallType() != CallType::TYPE_VOIP) {
            return (*it);
        }
    }
    return nullptr;
}

sptr<CallBase> CallObjectManager::GetOneCallObjectByIndexAndSlotId(int32_t index, int32_t slotId)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallIndex() == index) {
            if ((*it)->GetSlotId() == slotId && (*it)->GetCallType() != CallType::TYPE_VOIP) {
                return (*it);
            }
        }
    }
    return nullptr;
}

sptr<CallBase> CallObjectManager::GetOneCallObjectByIndexSlotIdAndCallType(int32_t index, int32_t slotId,
    CallType callType)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    if (callType == CallType::TYPE_BLUETOOTH) {
        std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
        for (; it != callObjectPtrList_.end(); ++it) {
            if ((*it)->GetCallType() == CallType::TYPE_BLUETOOTH && (*it)->GetCallIndex() == index &&
                ((*it)->GetSlotId() == slotId || (*it)->GetPhoneOrWatchDial() ==
                    static_cast<int32_t>(PhoneOrWatchDial::WATCH_DIAL))) {
                return (*it);
            }
        }
    } else {
        std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
        for (; it != callObjectPtrList_.end(); ++it) {
            if ((*it)->GetCallType() != CallType::TYPE_BLUETOOTH && (*it)->GetCallIndex() == index &&
                (*it)->GetSlotId() == slotId && (*it)->GetCallType() != CallType::TYPE_VOIP) {
                return (*it);
            }
        }
    }
    return nullptr;
}

sptr<CallBase> CallObjectManager::GetOneCallObjectByVoipCallId(
    std::string voipCallId, std::string bundleName, int32_t uid)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == CallType::TYPE_VOIP) {
            sptr<VoIPCall> voipCall = reinterpret_cast<VoIPCall *>((*it).GetRefPtr());
            if (voipCall->GetVoipCallId() == voipCallId && voipCall->GetVoipBundleName() == bundleName &&
                voipCall->GetVoipUid() == uid) {
                return (*it);
            }
        }
    }
    return nullptr;
}

bool CallObjectManager::IsCallExist(CallType callType, TelCallState callState)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == callType && (*it)->GetTelCallState() == callState) {
            return true;
        }
    }
    TELEPHONY_LOGI("the call is does not exist.");
    return false;
}

bool CallObjectManager::IsCallExist(TelCallState callState)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetTelCallState() == callState) {
            return true;
        }
    }
    TELEPHONY_LOGI("the call is does not exist.");
    return false;
}

bool CallObjectManager::IsCallExist(TelCallState callState, int32_t &callId)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetTelCallState() == callState) {
            callId = (*it)->GetCallID();
            return true;
        }
    }
    TELEPHONY_LOGI("the call is does not exist.");
    return false;
}

bool CallObjectManager::IsConferenceCallExist(TelConferenceState state, int32_t &callId)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetTelConferenceState() == state) {
            callId = (*it)->GetCallID();
            return true;
        }
    }
    TELEPHONY_LOGI("the call is does not exist.");
    return false;
}

bool CallObjectManager::HasActivedCallExist(int32_t &callId, bool isIncludeCallServiceKitCall)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetTelCallState() == TelCallState::CALL_STATUS_ACTIVE &&
            (isIncludeCallServiceKitCall || (*it)->GetCallType() != CallType::TYPE_VOIP)) {
            callId = (*it)->GetCallID();
            return true;
        }
    }
    CallAttributeInfo res = GetActiveVoipCallInfo();
    if (res.callId >= VOIP_CALL_MINIMUM) {
        callId = res.callId;
        return true;
    }
    TELEPHONY_LOGI("the call is does not exist.");
    return false;
}

int32_t CallObjectManager::GetCallNum(TelCallState callState, bool isIncludeVoipCall)
{
    int32_t num = 0;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetTelCallState() == callState) {
            if (!isIncludeVoipCall && (*it)->GetCallType() == CallType::TYPE_VOIP) {
                continue;
            } else {
                ++num;
            }
        }
    }
    TELEPHONY_LOGI("callState:%{public}d, num:%{public}d", callState, num);
    return num;
}

std::string CallObjectManager::GetCallNumber(TelCallState callState, bool isIncludeVoipCall)
{
    std::string number = "";
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetTelCallState() == callState) {
            if (!isIncludeVoipCall && (*it)->GetCallType() == CallType::TYPE_VOIP) {
                continue;
            } else {
                number = (*it)->GetAccountNumber();
                break;
            }
        }
    }
    return number;
}

std::vector<CallAttributeInfo> CallObjectManager::GetCallInfoList(int32_t slotId, bool isIncludeVoipCall)
{
    std::vector<CallAttributeInfo> callVec;
    CallAttributeInfo info;
    callVec.clear();
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        (void)memset_s(&info, sizeof(CallAttributeInfo), 0, sizeof(CallAttributeInfo));
        (*it)->GetCallAttributeInfo(info);
        if (info.accountId == slotId && info.callType != CallType::TYPE_OTT &&
            (isIncludeVoipCall || info.callType != CallType::TYPE_VOIP)) {
            callVec.emplace_back(info);
        }
    }
    std::vector<CallAttributeInfo> voipCallVec = GetVoipCallInfoList();
    for (CallAttributeInfo voipCall : voipCallVec) {
        callVec.emplace_back(voipCall);
    }
    TELEPHONY_LOGI("call list size is %{public}u", static_cast<uint32_t>(callVec.size()));
    return callVec;
}

std::vector<CallAttributeInfo> CallObjectManager::GetVoipCallInfoList()
{
    std::vector<CallAttributeInfo> callVec;
    int32_t voipState = DelayedSingleton<CallControlManager>::GetInstance()->GetMeetimeCallState();
    TELEPHONY_LOGI("voip state is :%{public}d", voipState);
    if (voipState == (int32_t)TelCallState::CALL_STATUS_INCOMING ||
        voipState == (int32_t)TelCallState::CALL_STATUS_IDLE ||
        voipState == (int32_t)TelCallState::CALL_STATUS_ALERTING) {
        std::map<int32_t, CallAttributeInfo>::iterator it = voipCallObjectList_.begin();
        for (; it != voipCallObjectList_.end(); ++it) {
            callVec.emplace_back(it->second);
        }
    }
    return callVec;
}

void CallObjectManager::UpdateOneCallObjectByCallId(int32_t callId, TelCallState nextCallState)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallID() == callId) {
            (*it)->SetTelCallState(nextCallState);
        }
    }
}

sptr<CallBase> CallObjectManager::GetForegroundCall(bool isIncludeVoipCall)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    sptr<CallBase> liveCall = nullptr;
    for (std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if (!isIncludeVoipCall && (*it)->GetCallType() == CallType::TYPE_VOIP) {
            continue;
        }
        TelCallState telCallState = (*it)->GetTelCallState();
        if (telCallState == TelCallState::CALL_STATUS_WAITING ||
            telCallState == TelCallState::CALL_STATUS_INCOMING) {
            liveCall = (*it);
            break;
        }
        if (telCallState == TelCallState::CALL_STATUS_ALERTING ||
            telCallState == TelCallState::CALL_STATUS_DIALING) {
            liveCall = (*it);
            continue;
        }
        if (telCallState == TelCallState::CALL_STATUS_ACTIVE) {
            liveCall = (*it);
            continue;
        }
        if (telCallState == TelCallState::CALL_STATUS_HOLDING) {
            liveCall = (*it);
            continue;
        }
    }
    return liveCall;
}

sptr<CallBase> CallObjectManager::GetForegroundLiveCall(bool isIncludeVoipCall)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    sptr<CallBase> liveCall = nullptr;
    for (std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if (!isIncludeVoipCall && (*it)->GetCallType() == CallType::TYPE_VOIP) {
            continue;
        }
        TelCallState telCallState = (*it)->GetTelCallState();
        if (telCallState == TelCallState::CALL_STATUS_ACTIVE) {
            liveCall = (*it);
            break;
        }
        if (telCallState == TelCallState::CALL_STATUS_ALERTING ||
            telCallState == TelCallState::CALL_STATUS_DIALING) {
            liveCall = (*it);
            break;
        }
    }
    return liveCall;
}

sptr<CallBase> CallObjectManager::GetIncomingCall(bool isIncludeVoipCall)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    sptr<CallBase> call = nullptr;
    for (std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if (!isIncludeVoipCall && (*it)->GetCallType() == CallType::TYPE_VOIP) {
            continue;
        }
        TelCallState callState = (*it)->GetTelCallState();
        if (callState == TelCallState::CALL_STATUS_INCOMING || callState == TelCallState::CALL_STATUS_WAITING) {
            call = (*it);
            break;
        }
    }
    return call;
}

sptr<CallBase> CallObjectManager::GetAudioLiveCall()
{
    sptr<CallBase> call = GetForegroundLiveCall(false);
    if (call == nullptr) {
        call = GetForegroundLiveCall();
    }
    if (call == nullptr) {
        call = GetIncomingCall(false);
    }
    return call;
}

CellularCallInfo CallObjectManager::GetDialCallInfo()
{
    return dialCallInfo_;
}

int32_t CallObjectManager::DealFailDial(sptr<CallBase> call)
{
    TELEPHONY_LOGI("DealFailDial");
    CallDetailInfo callDetatilInfo;
    if (memset_s(&callDetatilInfo, sizeof(CallDetailInfo), 0, sizeof(CallDetailInfo)) != EOK) {
        TELEPHONY_LOGE("memset_s callDetatilInfo fail");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    std::string number = call->GetAccountNumber();
    callDetatilInfo.callType = call->GetCallType();
    callDetatilInfo.accountId = call->GetSlotId();
    callDetatilInfo.state = TelCallState::CALL_STATUS_DISCONNECTED;
    callDetatilInfo.callMode = call->GetVideoStateType();
    callDetatilInfo.voiceDomain = static_cast<int32_t>(call->GetCallType());
    if (number.length() > kMaxNumberLen) {
        TELEPHONY_LOGE("numbser length out of range");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    if (memcpy_s(&callDetatilInfo.phoneNum, kMaxNumberLen, number.c_str(), number.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s number failed!");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }

    return DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallReportInfo(callDetatilInfo);
}

std::vector<CallAttributeInfo> CallObjectManager::GetAllCallInfoList(bool isIncludeVoipCall)
{
    std::vector<CallAttributeInfo> callVec;
    callVec.clear();
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        CallAttributeInfo info;
        if ((*it) == nullptr) {
            TELEPHONY_LOGE("call is nullptr");
            continue;
        }
        (*it)->GetCallAttributeInfo(info);
        if (isIncludeVoipCall || info.callType != CallType::TYPE_VOIP) {
            callVec.emplace_back(info);
        }
    }
    std::vector<CallAttributeInfo> voipCallVec = GetVoipCallInfoList();
    for (CallAttributeInfo voipCall : voipCallVec) {
        callVec.emplace_back(voipCall);
    }
    return callVec;
}

int32_t CallObjectManager::GetCallNumByRunningState(CallRunningState callState)
{
    int32_t count = 0;
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallRunningState() == callState) {
            count++;
            continue;
        }
    }
    TELEPHONY_LOGI("callState:%{public}d, count:%{public}d", callState, count);
    return count; 
}

sptr<CallBase> CallObjectManager::GetForegroundLiveCallByCallId(int32_t callId)
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    sptr<CallBase> liveCall = nullptr;
    for (std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        TelCallState telCallState = (*it)->GetTelCallState();
        if (telCallState == TelCallState::CALL_STATUS_ACTIVE && (*it)->GetCallID() == callId) {
            liveCall = (*it);
            break;
        }
        if ((telCallState == TelCallState::CALL_STATUS_ALERTING ||
            telCallState == TelCallState::CALL_STATUS_DIALING) && (*it)->GetCallID() == callId) {
            liveCall = (*it);
            break;
        }
        if ((telCallState == TelCallState::CALL_STATUS_WAITING ||
            telCallState == TelCallState::CALL_STATUS_INCOMING) && (*it)->GetCallID() == callId) {
            liveCall = (*it);
            continue;
        }
    }
    return liveCall;
}

bool CallObjectManager::IsNeedSilentInDoNotDisturbMode()
{
    sptr<CallBase> foregroundCall = CallObjectManager::GetForegroundCall(false);
    if (foregroundCall == nullptr) {
        TELEPHONY_LOGE("call object nullptr");
        return false;
    }
    TelCallState telCallState = foregroundCall->GetTelCallState();
    if (telCallState == TelCallState::CALL_STATUS_INCOMING) {
        int value = foregroundCall->GetParamsByKey("IsNeedSilentInDoNotDisturbMode", 0);
        TELEPHONY_LOGI("CallObjectManager::IsNeedSilentInDoNotDisturbMode: %{public}d", value);
        if (value == 1) {
            return true;
        }
    }
    return false;
}
#ifdef NOT_SUPPORT_MULTICALL
bool CallObjectManager::IsTwoCallBtCallAndESIM()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    if (callObjectPtrList_.size() != CALL_MAX_COUNT) {
        return false;
    }
    std::string numberEsim;
    std::string numberBtCall;
    bool hasEsim = false;
    bool hasBtCall = false;
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == CallType::TYPE_CS || (*it)->GetCallType() == CallType::TYPE_IMS) {
            hasEsim = true;
            numberEsim = (*it)->GetAccountNumber();
        }
        if ((*it)->GetCallType() == CallType::TYPE_BLUETOOTH) {
            hasBtCall = true;
            numberBtCall = (*it)->GetAccountNumber();
        }
    }
    if (hasEsim && hasBtCall) {
        if (!numberEsim.empty() && (numberEsim == numberBtCall)) {
            return false;
        }
        return true;
    }
    return false;
}

bool CallObjectManager::IsTwoCallBtCall()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    if (callObjectPtrList_.size() != CALL_MAX_COUNT) {
        return false;
    }
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() != CallType::TYPE_BLUETOOTH) {
            return false;
        }
    }
    return true;
}

bool CallObjectManager::IsTwoCallESIMCall()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    if (callObjectPtrList_.size() != CALL_MAX_COUNT) {
        return false;
    }
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() != CallType::TYPE_CS || (*it)->GetCallType() != CallType::TYPE_IMS) {
            return false;
        }
    }
    return true;
}

bool CallObjectManager::IsOneNumberDualTerminal()
{
    std::lock_guard<ffrt::mutex> lock(listMutex_);
    if (callObjectPtrList_.size() != CALL_MAX_COUNT) {
        return false;
    }
    std::string numberEsim;
    std::string numberBtCall;
    bool hasEsim = false;
    bool hasBtCall = false;
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); ++it) {
        if ((*it)->GetCallType() == CallType::TYPE_CS || (*it)->GetCallType() == CallType::TYPE_IMS) {
            hasEsim = true;
            numberEsim = (*it)->GetAccountNumber();
        }
        if ((*it)->GetCallType() == CallType::TYPE_BLUETOOTH) {
            hasBtCall = true;
            numberBtCall = (*it)->GetAccountNumber();
        }
    }
    if (hasEsim && hasBtCall) {
        if (!numberEsim.empty() && (numberEsim == numberBtCall)) {
            return true;
        }
    }
    return false;
}
#endif
} // namespace Telephony
} // namespace OHOS
