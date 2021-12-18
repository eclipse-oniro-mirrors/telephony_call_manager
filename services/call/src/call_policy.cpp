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
    int32_t accountId = extras.GetIntValue("accountId");
    if (!DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(accountId)) {
        TELEPHONY_LOGE("invalid accountId!");
        return CALL_ERR_INVALID_SLOT_ID;
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
    if (videoState != TYPE_VOICE && videoState != TYPE_VIDEO) {
        TELEPHONY_LOGE("videoState is invalid!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!IsCallExist(callId)) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_CALLID_INVALID;
    }
    TelCallState state = GetCallState(callId);
    if (state != CALL_STATUS_INCOMING && state != CALL_STATUS_WAITING) {
        TELEPHONY_LOGE("current call state is:%{public}d, accept call not allowed", state);
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::RejectCallPolicy(int32_t callId)
{
    if (!IsCallExist(callId)) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_CALLID_INVALID;
    }
    TelCallState state = GetCallState(callId);
    if (state != CALL_STATUS_INCOMING && state != CALL_STATUS_WAITING) {
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
        return CALL_ERR_CALLID_INVALID;
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
        return CALL_ERR_CALLID_INVALID;
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
        return CALL_ERR_CALLID_INVALID;
    }
    TelCallState state = GetCallState(callId);
    if (state == CALL_STATUS_IDLE || state == CALL_STATUS_DISCONNECTING || state == CALL_STATUS_DISCONNECTED) {
        TELEPHONY_LOGE("current call state is:%{public}d, hang up call not allowed", state);
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::SwitchCallPolicy(int32_t &callId)
{
    std::list<int32_t> callIdList;
    if (!IsCallExist(callId)) {
        TELEPHONY_LOGE("callId is invalid");
        return CALL_ERR_CALLID_INVALID;
    }
    GetCarrierCallList(callIdList);
    if (callIdList.size() < onlyTwoCall_) {
        callIdList.clear();
        return TELEPHONY_ERR_FAIL;
    }
    if (GetCallState(callId) != CALL_STATUS_HOLDING) {
        TELEPHONY_LOGE("the call is not on hold, callId:%{public}d", callId);
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    callIdList.clear();
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::UpgradeCallPolicy(int32_t callId)
{
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_CALLID_INVALID;
    }
    if (callPtr->GetCallType() != CallType::TYPE_IMS && callPtr->GetCallType() != CallType::TYPE_OTT) {
        TELEPHONY_LOGE("calltype is illegal, calltype:%{public}d", callPtr->GetCallType());
        return CALL_ERR_VIDEO_ILLEGAL_CALL_TYPE;
    }
    if (callPtr->GetVideoStateType() == VideoStateType::TYPE_VIDEO) {
        TELEPHONY_LOGE("already in video mode while upgradecall");
        return CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallPolicy::DowngradeCallPolicy(int32_t callId)
{
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("callId is invalid, callId:%{public}d", callId);
        return CALL_ERR_CALLID_INVALID;
    }
    if (callPtr->GetCallType() != CallType::TYPE_IMS && callPtr->GetCallType() != CallType::TYPE_OTT) {
        TELEPHONY_LOGE("calltype is illegal, calltype:%{public}d", callPtr->GetCallType());
        return CALL_ERR_VIDEO_ILLEGAL_CALL_TYPE;
    }
    if (callPtr->GetVideoStateType() != VideoStateType::TYPE_VIDEO) {
        TELEPHONY_LOGE("not in video mode while downgradecall");
        return CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
