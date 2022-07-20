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

#include "call_control_manager.h"

#include <securec.h>
#include <string_ex.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "common_type.h"
#include "audio_control_manager.h"
#include "video_control_manager.h"
#include "bluetooth_call_manager.h"
#include "call_ability_report_proxy.h"
#include "call_state_broadcast.h"
#include "call_state_report_proxy.h"
#include "cellular_call_connection.h"
#include "call_records_manager.h"
#include "call_number_utils.h"
#include "reject_call_sms.h"
#include "ims_call.h"
#include "iservice_registry.h"

namespace OHOS {
namespace Telephony {
CallControlManager::CallControlManager()
    : callStateListenerPtr_(nullptr), callRequestHandlerServicePtr_(nullptr), incomingCallWakeup_(nullptr),
      missedCallNotification_(nullptr), callSettingManagerPtr_(nullptr)
{
    dialSrcInfo_.callId = ERR_ID;
    dialSrcInfo_.number = "";
    extras_.Clear();
    dialSrcInfo_.isDialing = false;
}

CallControlManager::~CallControlManager()
{
    if (statusChangeListener_ != nullptr) {
        statusChangeListener_.clear();
        statusChangeListener_ = nullptr;
    }
}

bool CallControlManager::Init()
{
    callStateListenerPtr_ = std::make_unique<CallStateListener>();
    if (callStateListenerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStateListenerPtr_ is null");
        return false;
    }
    callRequestHandlerServicePtr_ = std::make_unique<CallRequestHandlerService>();
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is null");
        return false;
    }
    callRequestHandlerServicePtr_->Start();
    incomingCallWakeup_ = std::make_unique<IncomingCallWakeup>();
    if (incomingCallWakeup_ == nullptr) {
        TELEPHONY_LOGE("incomingCallWakeup_ is null");
        return false;
    }
    missedCallNotification_ = std::make_unique<MissedCallNotification>();
    if (missedCallNotification_ == nullptr) {
        TELEPHONY_LOGE("missedCallNotification_ is null");
        return false;
    }
    callSettingManagerPtr_ = std::make_unique<CallSettingManager>();
    if (callSettingManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return false;
    }
    if (BroadcastSubscriber() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("BroadcastSubscriber failed!");
        return false;
    }
    DelayedSingleton<AudioControlManager>::GetInstance()->Init();
    CallStateObserve();
    return true;
}

int32_t CallControlManager::DialCall(std::u16string &number, AppExecFwk::PacMap &extras)
{
    int32_t errorCode = TELEPHONY_ERR_FAIL;
    sptr<CallBase> callObjectPtr = nullptr;
    std::string accountNumber(Str16ToStr8(number));
    int32_t ret = NumberLegalityCheck(accountNumber);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid number!");
        return ret;
    }
    bool isEcc = DelayedSingleton<CallNumberUtils>::GetInstance()->CheckNumberIsEmergency(
        accountNumber, extras.GetIntValue("accountId"), errorCode);
    ret = DialPolicy(number, extras, isEcc);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("dial policy result:%{public}d", ret);
        return ret;
    }
    // temporarily save dial information
    {
        std::lock_guard<std::mutex> lock(mutex_);
        dialSrcInfo_.callId = ERR_ID;
        dialSrcInfo_.number = accountNumber;
        dialSrcInfo_.isDialing = true;
        dialSrcInfo_.isEcc = isEcc;
        dialSrcInfo_.callType = (CallType)extras.GetIntValue("callType");
        dialSrcInfo_.accountId = extras.GetIntValue("accountId");
        dialSrcInfo_.dialType = (DialType)extras.GetIntValue("dialType");
        dialSrcInfo_.videoState = (VideoStateType)extras.GetIntValue("videoState");
        dialSrcInfo_.bundleName = extras.GetStringValue("bundleName");
        extras_.Clear();
        extras_ = extras;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->DialCall();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("DialCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::AnswerCall(int32_t callId, int32_t videoState)
{
    if (callId == INVALID_CALLID) {
        sptr<CallBase> call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
        if (call == nullptr) {
            TELEPHONY_LOGE("call is nullptr");
            return TELEPHONY_ERROR;
        }
        callId = call->GetCallID();
    }

    int32_t ret = AnswerCallPolicy(callId, videoState);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("AnswerCallPolicy failed!");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->AnswerCall(callId, videoState);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("AnswerCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::RejectCall(int32_t callId, bool rejectWithMessage, std::u16string textMessage)
{
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    if (callId == INVALID_CALLID) {
        sptr<CallBase> call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
        if (call == nullptr) {
            TELEPHONY_LOGE("call is nullptr");
            return TELEPHONY_ERROR;
        }
        callId = call->GetCallID();
    }

    int32_t ret = RejectCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RejectCallPolicy failed!");
        return ret;
    }
    std::string messageStr(Str16ToStr8(textMessage));
    ret = callRequestHandlerServicePtr_->RejectCall(callId, rejectWithMessage, messageStr);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RejectCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::HangUpCall(int32_t callId)
{
    if (callId == INVALID_CALLID) {
        sptr<CallBase> call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
        if (call == nullptr) {
            TELEPHONY_LOGE("call is nullptr");
            return TELEPHONY_ERROR;
        }
        callId = call->GetCallID();
    }

    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = HangUpPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HangUpPolicy failed!");
        return ret;
    }
    ret = callRequestHandlerServicePtr_->HangUpCall(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HangUpCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::GetCallState()
{
    CallStateToApp callState = CallStateToApp::CALL_STATE_UNKNOWN;
    if (!HasCallExist()) {
        callState = CallStateToApp::CALL_STATE_IDLE;
    } else {
        callState = CallStateToApp::CALL_STATE_OFFHOOK;
        if (HasRingingCall()) {
            callState = CallStateToApp::CALL_STATE_RINGING;
        }
    }
    return static_cast<int32_t>(callState);
}

int32_t CallControlManager::HoldCall(int32_t callId)
{
    int32_t ret = HoldCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HoldCall failed!");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->HoldCall(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HoldCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::UnHoldCall(const int32_t callId)
{
    int32_t ret = UnHoldCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UnHoldCall failed!");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->UnHoldCall(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UnHoldCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

// swap two calls state, turn active call into holding, and turn holding call into active
int32_t CallControlManager::SwitchCall(int32_t callId)
{
    int32_t ret = SwitchCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SwitchCall failed!");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->SwitchCall(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SwitchCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

bool CallControlManager::HasCall()
{
    return HasCallExist();
}

bool CallControlManager::IsNewCallAllowed()
{
    return IsNewCallAllowedCreate();
}

bool CallControlManager::IsRinging()
{
    return HasRingingCall();
}

bool CallControlManager::HasEmergency()
{
    return HasEmergencyCall();
}

bool CallControlManager::NotifyNewCallCreated(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("callObjectPtr is null!");
        return false;
    }
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->NewCallCreated(callObjectPtr);
    }
    return true;
}

bool CallControlManager::NotifyCallDestroyed(int32_t cause)
{
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->CallDestroyed(cause);
        return true;
    }
    return false;
}

bool CallControlManager::NotifyCallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("callObjectPtr is null!");
        return false;
    }
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->CallStateUpdated(callObjectPtr, priorState, nextState);
        TELEPHONY_LOGI("NotifyCallStateUpdated priorState:%{public}d,nextState:%{public}d", priorState, nextState);
        return true;
    }
    return false;
}

bool CallControlManager::NotifyIncomingCallAnswered(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("callObjectPtr is null!");
        return false;
    }
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->IncomingCallActivated(callObjectPtr);
        return true;
    }
    return false;
}

bool CallControlManager::NotifyIncomingCallRejected(
    sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("callObjectPtr is null!");
        return false;
    }
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->IncomingCallHungUp(callObjectPtr, isSendSms, content);
        return true;
    }
    return false;
}

bool CallControlManager::NotifyCallEventUpdated(CallEventInfo &info)
{
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->CallEventUpdated(info);
        return true;
    }
    return false;
}

int32_t CallControlManager::StartDtmf(int32_t callId, char str)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!call->IsAliveState()) {
        return CALL_ERR_CALL_STATE_MISMATCH_OPERATION;
    }

    int32_t ret = call->StartDtmf(str);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StartDtmf failed, return:%{public}d", ret);
    }
    return ret;
}

int32_t CallControlManager::StopDtmf(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!call->IsAliveState()) {
        return CALL_ERR_CALL_STATE_MISMATCH_OPERATION;
    }

    int32_t ret = call->StopDtmf();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StopDtmf failed, return:%{public}d", ret);
    }
    return ret;
}

int32_t CallControlManager::GetCallWaiting(int32_t slotId)
{
    int32_t ret = CallPolicy::GetCallWaitingPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCallWaiting failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->GetCallWaiting(slotId);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetCallWaiting(int32_t slotId, bool activate)
{
    int32_t ret = CallPolicy::SetCallWaitingPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallWaiting failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetCallWaiting(slotId, activate);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    int32_t ret = CallPolicy::GetCallRestrictionPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCallRestriction failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->GetCallRestriction(slotId, type);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    int32_t ret = CallPolicy::SetCallRestrictionPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallRestriction failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetCallRestriction(slotId, info);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    int32_t ret = CallPolicy::GetCallTransferInfoPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetCallTransferInfo failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->GetCallTransferInfo(slotId, type);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    int32_t ret = CallPolicy::SetCallTransferInfoPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallTransferInfo failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetCallTransferInfo(slotId, info);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    int32_t ret = CallPolicy::SetCallPreferenceModePolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetCallPreferenceMode failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetCallPreferenceMode(slotId, mode);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

/**
 * start a telephone conference by merging three-way calls,steps as follows:
 * 1.A call B: A<---->B,set holding
 * 2.A call C: A<---->C, active
 * 3.A initial merge request by CombineConference
 * @param mainCallId:active call id
 */
int32_t CallControlManager::CombineConference(int32_t mainCallId)
{
    sptr<CallBase> mainCall = GetOneCallObject(mainCallId);
    if (mainCall == nullptr) {
        TELEPHONY_LOGE("GetOneCallObject failed, mainCallId:%{public}d", mainCallId);
        return CALL_ERR_INVALID_CALLID;
    }
    if (mainCall->GetTelCallState() != TelCallState::CALL_STATUS_ACTIVE) {
        TELEPHONY_LOGE("mainCall state should be active ");
        return CALL_ERR_CALL_STATE_MISMATCH_OPERATION;
    }
    if (!CallObjectManager::IsCallExist(mainCall->GetCallType(), TelCallState::CALL_STATUS_HOLDING)) {
        TELEPHONY_LOGE("callType:%{public}d,callState:CALL_STATUS_HOLDING is not exist!", mainCall->GetCallType());
        return CALL_ERR_CALL_STATE_MISMATCH_OPERATION;
    }
    int32_t ret = mainCall->CanCombineConference();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CanCombineConference failed");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->CombineConference(mainCallId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CombineConference failed!");
    }
    return ret;
}

int32_t CallControlManager::SeparateConference(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("GetOneCallObject failed, callId:%{public}d", callId);
        return CALL_ERR_INVALID_CALLID;
    }
    int32_t ret = call->CanSeparateConference();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CanSeparateConference failed");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->SeparateConference(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CombineConference failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::GetMainCallId(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("GetOneCallObject failed! callId:%{public}d", callId);
        return TELEPHONY_ERROR;
    }
    return call->GetMainCallId();
}

std::vector<std::u16string> CallControlManager::GetSubCallIdList(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        return std::vector<std::u16string>();
    }
    return call->GetSubCallIdList();
}

std::vector<std::u16string> CallControlManager::GetCallIdListForConference(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        return std::vector<std::u16string>();
    }
    return call->GetCallIdListForConference();
}

int32_t CallControlManager::GetImsConfig(int32_t slotId, ImsConfigItem item)
{
    int32_t ret = CallPolicy::GetImsConfigPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetImsConfig failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->GetImsConfig(slotId, item);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value)
{
    int32_t ret = CallPolicy::SetImsConfigPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsConfig failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetImsConfig(slotId, item, value);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::GetImsFeatureValue(int32_t slotId, FeatureType type)
{
    int32_t ret = CallPolicy::GetImsFeatureValuePolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("GetImsFeatureValue failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->GetImsFeatureValue(slotId, type);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value)
{
    int32_t ret = CallPolicy::SetImsFeatureValuePolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetImsFeatureValue failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetImsFeatureValue(slotId, type, value);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::EnableImsSwitch(int32_t slotId)
{
    int32_t ret = CallPolicy::EnableVoLtePolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("EnableImsSwitch failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->EnableImsSwitch(slotId);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::DisableImsSwitch(int32_t slotId)
{
    int32_t ret = CallPolicy::DisableVoLtePolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("DisableImsSwitch failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->DisableImsSwitch(slotId);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::IsImsSwitchEnabled(int32_t slotId)
{
    int32_t ret = CallPolicy::IsVoLteEnabledPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("IsImsSwitchEnabled failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->IsImsSwitchEnabled(slotId);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::UpdateImsCallMode(int32_t callId, ImsCallMode mode)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = UpdateCallMediaModePolicy(callId, mode);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check prerequisites failed !");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->UpdateImsCallMode(callId, mode);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateImsCallMode failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::StartRtt(int32_t callId, std::u16string &msg)
{
    int32_t ret = CallPolicy::StartRttPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("NO IMS call,can not StartRtt!");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->StartRtt(callId, msg);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StartRtt failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::StopRtt(int32_t callId)
{
    int32_t ret = CallPolicy::StopRttPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("NO IMS call,no need StopRtt!");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->StopRtt(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StopRtt failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::JoinConference(int32_t callId, std::vector<std::u16string> &numberList)
{
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::vector<std::string> phoneNumberList(numberList.size());
    for (size_t index = 0; index < numberList.size(); ++index) {
        phoneNumberList[index] = Str16ToStr8(numberList[index]);
    }
    int32_t ret = CallPolicy::InviteToConferencePolicy(callId, phoneNumberList);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check InviteToConference Policy failed!");
        return ret;
    }
    ret = callRequestHandlerServicePtr_->JoinConference(callId, phoneNumberList);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("JoinConference failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::SetMuted(bool isMute)
{
    sptr<CallBase> call = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    if (call != nullptr) {
        if (call->GetCallType() == CallType::TYPE_IMS
            || call->GetCallType() == CallType::TYPE_CS) {
            TELEPHONY_LOGI("SetMute by ims or cs");
            call->SetMute(call->GetSlotId(), isMute);
        }
    }

    return DelayedSingleton<AudioControlManager>::GetInstance()->SetMute(isMute);
}

int32_t CallControlManager::MuteRinger()
{
    return DelayedSingleton<AudioControlManager>::GetInstance()->MuteRinger();
}

int32_t CallControlManager::SetAudioDevice(AudioDevice deviceType, const std::string &bluetoothAddress)
{
    if (deviceType == AudioDevice::DEVICE_BLUETOOTH_SCO) {
        DelayedSingleton<AudioDeviceManager>::GetInstance()->SetBluetoothAddress(bluetoothAddress);
    }
    return DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioDevice(deviceType);
}

int32_t CallControlManager::ControlCamera(std::u16string cameraId, int32_t callingUid, int32_t callingPid)
{
    return DelayedSingleton<VideoControlManager>::GetInstance()->ControlCamera(cameraId, callingUid, callingPid);
}

int32_t CallControlManager::SetPreviewWindow(VideoWindow &window)
{
    return DelayedSingleton<VideoControlManager>::GetInstance()->SetPreviewWindow(window);
}

int32_t CallControlManager::SetDisplayWindow(VideoWindow &window)
{
    return DelayedSingleton<VideoControlManager>::GetInstance()->SetDisplayWindow(window);
}

int32_t CallControlManager::SetCameraZoom(float zoomRatio)
{
    return DelayedSingleton<VideoControlManager>::GetInstance()->SetCameraZoom(zoomRatio);
}

int32_t CallControlManager::SetPausePicture(std::u16string path)
{
    return DelayedSingleton<VideoControlManager>::GetInstance()->SetPausePicture(path);
}

int32_t CallControlManager::SetDeviceDirection(int32_t rotation)
{
    return DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(rotation);
}

bool CallControlManager::IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, int32_t &errorCode)
{
    if (IsValidSlotId(slotId)) {
        errorCode = CALL_ERR_INVALID_SLOT_ID;
        return false;
    }
    return DelayedSingleton<CallNumberUtils>::GetInstance()->CheckNumberIsEmergency(
        Str16ToStr8(number), slotId, errorCode);
}

int32_t CallControlManager::FormatPhoneNumber(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    std::string tmpStr("");
    int32_t ret = DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumber(
        Str16ToStr8(number), Str16ToStr8(countryCode), tmpStr);
    formatNumber = Str8ToStr16(tmpStr);
    return ret;
}

int32_t CallControlManager::FormatPhoneNumberToE164(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    std::string tmpStr("");
    int32_t ret = DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(
        Str16ToStr8(number), Str16ToStr8(countryCode), tmpStr);
    formatNumber = Str8ToStr16(tmpStr);
    return ret;
}

void CallControlManager::GetDialParaInfo(DialParaInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    info = dialSrcInfo_;
}

void CallControlManager::GetDialParaInfo(DialParaInfo &info, AppExecFwk::PacMap &extras)
{
    std::lock_guard<std::mutex> lock(mutex_);
    info = dialSrcInfo_;
    extras = extras_;
}

void CallControlManager::CallStateObserve()
{
    if (callStateListenerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStateListenerPtr_ is null");
        return;
    }
    std::unique_ptr<RejectCallSms> hangUpSmsPtr = std::make_unique<RejectCallSms>();
    if (hangUpSmsPtr == nullptr) {
        TELEPHONY_LOGE("hangUpSmsPtr is null");
        return;
    }
    std::unique_ptr<CallStateBroadcast> callStateBroadcastPtr = std::make_unique<CallStateBroadcast>();
    if (callStateBroadcastPtr == nullptr) {
        TELEPHONY_LOGE("callStateBroadcastPtr is null");
        return;
    }
    std::unique_ptr<CallStateReportProxy> callStateReportPtr = std::make_unique<CallStateReportProxy>();
    if (callStateReportPtr == nullptr) {
        TELEPHONY_LOGE("CallStateReportProxy is nullptr!");
        return;
    }
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<CallAbilityReportProxy>::GetInstance().get());
    callStateListenerPtr_->AddOneObserver(callStateReportPtr.release());
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<AudioControlManager>::GetInstance().get());
    callStateListenerPtr_->AddOneObserver(hangUpSmsPtr.release());
    callStateListenerPtr_->AddOneObserver(callStateBroadcastPtr.release());
    callStateListenerPtr_->AddOneObserver(missedCallNotification_.release());
    callStateListenerPtr_->AddOneObserver(incomingCallWakeup_.release());
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<CallRecordsManager>::GetInstance().get());
}

int32_t CallControlManager::NumberLegalityCheck(std::string &number)
{
    if (number.empty()) {
        TELEPHONY_LOGE("phone number is NULL!");
        return CALL_ERR_PHONE_NUMBER_EMPTY;
    }
    if (number.length() > kMaxNumberLen) {
        TELEPHONY_LOGE(
            "the number length exceeds limit,len:%{public}zu,maxLen:%{public}d", number.length(), kMaxNumberLen);
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    return TELEPHONY_SUCCESS;
}

CallControlManager::SystemAbilityListener::SystemAbilityListener(std::shared_ptr<CallBroadcastSubscriber> subscriberPtr)
    : subscriberPtr_(subscriberPtr)
{}

void CallControlManager::SystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != COMMON_EVENT_SERVICE_ID) {
        TELEPHONY_LOGE("systemAbilityId is not COMMON_EVENT_SERVICE_ID");
        return;
    }

    if (subscriberPtr_ == nullptr) {
        TELEPHONY_LOGE("CallControlManager::OnAddSystemAbility subscriberPtr is nullptr");
        return;
    }

    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberPtr_);
    TELEPHONY_LOGI("CallControlManager::OnAddSystemAbility subscribeResult = %{public}d", subscribeResult);
}

void CallControlManager::SystemAbilityListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != COMMON_EVENT_SERVICE_ID) {
        TELEPHONY_LOGE("systemAbilityId is not COMMON_EVENT_SERVICE_ID");
        return;
    }

    if (subscriberPtr_ == nullptr) {
        TELEPHONY_LOGE("CallControlManager::OnRemoveSystemAbility subscriberPtr is nullptr");
        return;
    }

    bool subscribeResult = EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriberPtr_);
    TELEPHONY_LOGI("CallControlManager::OnRemoveSystemAbility subscribeResult = %{public}d", subscribeResult);
}

int32_t CallControlManager::BroadcastSubscriber()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(SIM_STATE_UPDATE_ACTION);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<CallBroadcastSubscriber> subscriberPtr = std::make_shared<CallBroadcastSubscriber>(subscriberInfo);
    if (subscriberPtr == nullptr) {
        TELEPHONY_LOGE("CallControlManager::BroadcastSubscriber subscriberPtr is nullptr");
        return TELEPHONY_ERROR;
    }
    // if SubscribeCommonEvent fail, register statusChangeListener to SubscribeCommonEvent again
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        TELEPHONY_LOGE("samgrProxy is nullptr");
        return TELEPHONY_ERROR;
    }

    statusChangeListener_ = new (std::nothrow) SystemAbilityListener(subscriberPtr);
    if (statusChangeListener_ == nullptr) {
        TELEPHONY_LOGE("statusChangeListener_ is nullptr");
        return TELEPHONY_ERROR;
    }

    int32_t ret = samgrProxy->SubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, statusChangeListener_);
    TELEPHONY_LOGI("CallControlManager::BroadcastSubscriber ret: %{public}d", ret);
    return TELEPHONY_SUCCESS;
}

#ifdef ABILITY_MEDIA_SUPPORT
bool CallControlManager::onButtonDealing(HeadsetButtonService::ButtonEvent type)
{
    bool isRingState = false;
    sptr<CallBase> call = nullptr;

    if ((call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING)) != nullptr) {
        isRingState = true;
    } else if (((call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING)) != nullptr) ||
        ((call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE)) != nullptr) ||
        ((call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD)) != nullptr)) {
        isRingState = false;
    } else {
        return false;
    }

    switch (type) {
        case HeadsetButtonService::SHORT_PRESS_EVENT:
            if (isRingState) {
                call->HangUpCall();
            } else {
                call->AnswerCall(0);
            }
            break;
        case HeadsetButtonService::LONG_PRESS_EVENT:
            if (isRingState) {
                std::string str = "";
                call->RejectCall(false, str);
            } else {
                DelayedSingleton<AudioControlManager>::GetInstance()->SetMute(true);
            }
            break;
        default:
            return false;
    }
    return true;
}
#endif
} // namespace Telephony
} // namespace OHOS
