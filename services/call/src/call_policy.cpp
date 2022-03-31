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

#include "call_policy.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "ims_conference.h"

#include "call_number_utils.h"

namespace OHOS {
namespace Telephony {
CallPolicy::CallPolicy() {}

CallPolicy::~CallPolicy() {}

int32_t CallPolicy::DialPolicy(std::u16string &number, AppExecFwk::PacMap &extras, bool isEcc)
{
    DialType dialType = (DialType)extras.GetIntValue("dialType");
    if (dialType != DialType::DIAL_CARRIER_TYPE && dialType != DialType::DIAL_VOICE_MAIL_TYPE &&
        dialType != DialType::DIAL_OTT_TYPE) {
        TELEPHONY_LOGE("dial type invalid!");
        return CALL_ERR_UNKNOW_DIAL_TYPE;
    }
    if (dialType == DialType::DIAL_CARRIER_TYPE) {
        int32_t accountId = extras.GetIntValue("accountId");
        if (!DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(accountId)) {
            TELEPHONY_LOGE("invalid accountId!");
            return CALL_ERR_INVALID_SLOT_ID;
        }
    }
    CallType callType = (CallType)extras.GetIntValue("callType");
    if (callType != CallType::TYPE_CS && callType != CallType::TYPE_IMS && callType != CallType::TYPE_OTT) {
        TELEPHONY_LOGE("invalid call type!");
        return CALL_ERR_UNKNOW_CALL_TYPE;
    }
    DialScene dialScene = (DialScene)extras.GetIntValue("dialScene");
    if ((dialScene != DialScene::CALL_NORMAL && dialScene != DialScene::CALL_PRIVILEGED &&
            dialScene != DialScene::CALL_EMERGENCY) ||
        (dialScene == DialScene::CALL_NORMAL && isEcc) || (dialScene == DialScene::CALL_EMERGENCY && (!isEcc)) ||
        (dialType == DialType::DIAL_VOICE_MAIL_TYPE && dialScene == DialScene::CALL_EMERGENCY)) {
        TELEPHONY_LOGE("invalid dial scene!");
        return CALL_ERR_INVALID_DIAL_SCENE;
    }
    VideoStateType videoState = (VideoStateType)extras.GetIntValue("videoState");
    if (videoState != VideoStateType::TYPE_VOICE && videoState != VideoStateType::TYPE_VIDEO) {
        TELEPHONY_LOGE("invalid video state!");
        return CALL_ERR_INVALID_VIDEO_STATE;
    }
    return HasNewCall();
}

int32_t CallPolicy::AnswerCallPolicy(int32_t callId, int32_t videoState)
{
    if (videoState != static_cast<int32_t>(VideoStateType::TYPE_VOICE) &&
        videoState != static_cast<int32_t>(VideoStateType::TYPE_VIDEO)) {
        TELEPHONY_LOGE("videoState is invalid!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!IsCallExist(callId)) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_INVALID_CALLID;
    }
    TelCallState state = GetCallState(callId);
    if (state != TelCallState::CALL_STATUS_INCOMING && state != TelCallState::CALL_STATUS_WAITING) {
        TELEPHONY_LOGE("current call state is:%{public}d, accept call not allowed", state);
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::RejectCallPolicy(int32_t callId)
{
    if (!IsCallExist(callId)) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_INVALID_CALLID;
    }
    TelCallState state = GetCallState(callId);
    if (state != TelCallState::CALL_STATUS_INCOMING && state != TelCallState::CALL_STATUS_WAITING) {
        TELEPHONY_LOGE("current call state is:%{public}d, reject call not allowed", state);
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::HoldCallPolicy(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("GetOneCallObject failed, this callId is invalid! callId:%{public}d", callId);
        return CALL_ERR_INVALID_CALLID;
    }
    if (call->GetCallRunningState() != CallRunningState::CALL_RUNNING_STATE_ACTIVE) {
        TELEPHONY_LOGE("this call is not activated! callId:%{public}d", callId);
        return CALL_ERR_CALL_IS_NOT_ACTIVATED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::UnHoldCallPolicy(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("GetOneCallObject failed, this callId is invalid! callId:%{public}d", callId);
        return CALL_ERR_INVALID_CALLID;
    }
    if (call->GetCallRunningState() != CallRunningState::CALL_RUNNING_STATE_HOLD) {
        TELEPHONY_LOGE("this call is not on holding state! callId:%{public}d", callId);
        return CALL_ERR_CALL_IS_NOT_ON_HOLDING;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::HangUpPolicy(int32_t callId)
{
    if (!IsCallExist(callId)) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_INVALID_CALLID;
    }
    TelCallState state = GetCallState(callId);
    if (state == TelCallState::CALL_STATUS_IDLE || state == TelCallState::CALL_STATUS_DISCONNECTING ||
        state == TelCallState::CALL_STATUS_DISCONNECTED) {
        TELEPHONY_LOGE("current call state is:%{public}d, hang up call not allowed", state);
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::SwitchCallPolicy(int32_t callId)
{
    std::list<int32_t> callIdList;
    if (!IsCallExist(callId)) {
        TELEPHONY_LOGE("callId is invalid");
        return CALL_ERR_INVALID_CALLID;
    }
    GetCarrierCallList(callIdList);
    if (callIdList.size() < onlyTwoCall_) {
        callIdList.clear();
        return CALL_ERR_PHONE_CALLS_TOO_FEW;
    }
    if (GetCallState(callId) != TelCallState::CALL_STATUS_HOLDING ||
        IsCallExist(TelCallState::CALL_STATUS_DIALING) || IsCallExist(TelCallState::CALL_STATUS_ALERTING)) {
        TELEPHONY_LOGE("the call is not on hold, callId:%{public}d", callId);
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    callIdList.clear();
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::UpdateCallMediaModePolicy(int32_t callId, ImsCallMode mode)
{
    TELEPHONY_LOGI("callid %{public}d, mode:%{public}d", callId, mode);
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_INVALID_CALLID;
    }
    if (callPtr->GetCallType() != CallType::TYPE_IMS && callPtr->GetCallType() != CallType::TYPE_OTT) {
        TELEPHONY_LOGE("calltype is illegal, calltype:%{public}d", callPtr->GetCallType());
        return CALL_ERR_VIDEO_ILLEGAL_CALL_TYPE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::StartRttPolicy(int32_t callId)
{
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_INVALID_CALLID;
    }
    if (callPtr->GetCallType() != CallType::TYPE_IMS) {
        TELEPHONY_LOGE("calltype is illegal, calltype:%{public}d", callPtr->GetCallType());
        return CALL_ERR_UNSUPPORTED_NETWORK_TYPE;
    }
    TelCallState state = GetCallState(callId);
    if (state != TelCallState::CALL_STATUS_ACTIVE) {
        TELEPHONY_LOGE("current call state is:%{public}d, StartRtt not allowed", state);
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::StopRttPolicy(int32_t callId)
{
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_INVALID_CALLID;
    }
    if (callPtr->GetCallType() != CallType::TYPE_IMS) {
        TELEPHONY_LOGE("calltype is illegal, calltype:%{public}d", callPtr->GetCallType());
        return CALL_ERR_UNSUPPORTED_NETWORK_TYPE;
    }
    TelCallState state = GetCallState(callId);
    if (state != TelCallState::CALL_STATUS_ACTIVE) {
        TELEPHONY_LOGE("current call state is:%{public}d, StopRtt not allowed", state);
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::IsValidSlotId(int32_t slotId)
{
    bool result = DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(slotId);
    if (!result) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::EnableVoLtePolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::DisableVoLtePolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::IsVoLteEnabledPolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::GetCallWaitingPolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::SetCallWaitingPolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::GetCallRestrictionPolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::SetCallRestrictionPolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::GetCallTransferInfoPolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::SetCallTransferInfoPolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::SetCallPreferenceModePolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::GetImsConfigPolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::SetImsConfigPolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::GetImsFeatureValuePolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::SetImsFeatureValuePolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::GetLteEnhanceModePolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::SetLteEnhanceModePolicy(int32_t slotId)
{
    if (IsValidSlotId(slotId) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::InviteToConferencePolicy(int32_t callId, std::vector<std::string> &numberList)
{
    if (!IsCallExist(callId)) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_INVALID_CALLID;
    }
    // check number legality
    if (numberList.empty()) {
        TELEPHONY_LOGE("empty phone number list!");
        return CALL_ERR_PHONE_NUMBER_EMPTY;
    }
    for (size_t index = 0; index < numberList.size(); ++index) {
        if (numberList[index].empty()) {
            TELEPHONY_LOGE("empty phone number !");
            return CALL_ERR_PHONE_NUMBER_EMPTY;
        }
        if (numberList[index].length() > kMaxNumberLen) {
            TELEPHONY_LOGE("phone number too long !");
            return CALL_ERR_NUMBER_OUT_OF_RANGE;
        }
    }
    if (DelayedSingleton<ImsConference>::GetInstance()->GetMainCall() != callId) {
        TELEPHONY_LOGE("conference with main callId %{public}d not exist", callId);
        return CALL_ERR_CONFERENCE_NOT_EXISTS;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
