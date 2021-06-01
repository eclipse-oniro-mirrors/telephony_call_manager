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

#include "call_control_manager.h"

#include <securec.h>
#include <string_ex.h>

#include "call_manager_errors.h"
#include "call_manager_log.h"

#include "common_type.h"
#include "cs_call.h"
#include "ims_call.h"
#include "ott_call.h"
#include "report_call_state_handler.h"
#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
CallControlManager::CallControlManager() : callStateListenerPtr_(nullptr)
{
    (void)memset_s(&simInfo_, sizeof(SIMCardInfo), 0, sizeof(SIMCardInfo));
}

CallControlManager::~CallControlManager() {}

bool CallControlManager::Init()
{
    callStateListenerPtr_ = std::make_unique<CallStateListener>();
    if (callStateListenerPtr_ == nullptr) {
        CALLMANAGER_ERR_LOG("callStateListenerPtr_ is null");
        return false;
    }
    reportCallStateHandlerPtr_ = (std::make_unique<ReportCallStateHandlerService>()).release();
    if (reportCallStateHandlerPtr_ == nullptr) {
        CALLMANAGER_ERR_LOG("reportCallStateHandlerPtr_ is null");
        return false;
    }
    reportCallStateHandlerPtr_->Start();
    callStateListenerPtr_->AddOneObserver(reportCallStateHandlerPtr_);
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<AudioControlManager>::GetInstance().get());

    return true;
}

int32_t CallControlManager::DialCall(std::u16string number, AppExecFwk::PacMap &extras, int32_t &callId)
{
    int32_t ret = CALL_MANAGER_DIAL_FAILED;
    CallInfo info;
    sptr<CallBase> callObjectPtr = nullptr;
    callId = -1;
    if (number.empty()) {
        CALLMANAGER_ERR_LOG("phone number is NULL!");
        return CALL_MANAGER_PHONENUM_NULL;
    }
    if (!IsPhoneNumberLegal(Str16ToStr8(number))) {
        CALLMANAGER_ERR_LOG("Invalid number!");
        return CALL_MANAGER_PHONENUM_INVALID;
    }
    ret = DialPolicy();
    if (ret != TELEPHONY_NO_ERROR) {
        return ret;
    }
    ret = InitCallInfo(number, info, extras);
    CALLMANAGER_DEBUG_LOG("dialScene:%{public}d", info.dialScene);
    if (info.dialScene != CALL_NORMAL && info.dialScene != CALL_PRIVILEGED && info.dialScene != CALL_EMERGENCY) {
        CALLMANAGER_ERR_LOG("DialScene incorrect!");
        return CALL_MANAGER_DIAL_SCENE_INCORRECT;
    }
    info.callType = CallType::TYPE_CS;
    ret = CreateCallObject(info.callType, callObjectPtr, info);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("creating call object failed!");
        return ret;
    }
    if (callObjectPtr != nullptr) {
        AddOneCallObject(callObjectPtr);
        NotifyNewCallCreated(callObjectPtr);
        callId = info.callId;
    }
    return ret;
}

int32_t CallControlManager::AcceptCall(int32_t callId, int32_t videoState)
{
    int32_t ret = CALL_MANAGER_ACCPET_FAILED;
    sptr<CallBase> call = GetOneCallObject(callId);
    if (AccpetCallPolicy(callId) == TELEPHONY_NO_ERROR && call != nullptr) {
        ret = call->AccpetCallBase();
        if (ret == TELEPHONY_NO_ERROR) {
            NotifyIncomingCallAnswered(call);
        }
    }
    return ret;
}

int32_t CallControlManager::RejectCall(int32_t callId, bool isSendSms, std::u16string content)
{
    int32_t ret = CALL_MANAGER_REJECT_FAILED;
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call != nullptr) {
        ret = call->RejectCallBase();
        if (ret == TELEPHONY_NO_ERROR) {
            NotifyIncomingCallRejected(call, isSendSms, Str16ToStr8(content));
        }
    }
    return ret;
}

int32_t CallControlManager::HangUpCall(int32_t callId)
{
    int32_t ret = CALL_MANAGER_HANGUP_FAILED;
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call != nullptr) {
        ret = call->HangUpBase();
    }
    return ret;
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
    return (int32_t)callState;
}

int32_t CallControlManager::InitCallInfo(std::u16string number, CallInfo &callInfo, AppExecFwk::PacMap &extras)
{
    std::string phoneNumber = Str16ToStr8(number);
    callInfo.speakerphoneOn = false;
    callInfo.dialScene = extras.GetIntValue("dialScene");
    callInfo.isDefault = false;
    callInfo.isEcc = false;
    callInfo.startime = 0;
    callInfo.policyFlags = 0;
    (void)memset_s(callInfo.scheme, kMaxSchemeNumberLen, 0, kMaxSchemeNumberLen);
    callInfo.callId = GetNextCallId();
    CALLMANAGER_DEBUG_LOG("Call id is %{public}d", callInfo.callId);
    callInfo.accountId = extras.GetIntValue("accountId");
    callInfo.videoState = extras.GetIntValue("videoState");
    if (number.size() > kMaxNumberLen) {
        return CALL_MANAGER_PHONE_BEYOND;
    }
    (void)memset_s(callInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen);
    if (memcpy_s(callInfo.phoneNum, kMaxNumberLen, phoneNumber.c_str(), number.size()) != 0) {
        return TELEPHONY_MEMCPY_FAIL;
    }
    callInfo.isEcc = CheckNumberIsEmergency(phoneNumber);
    return TELEPHONY_NO_ERROR;
}

int32_t CallControlManager::CreateCallObject(const CallType &callType, sptr<CallBase> &call, CallInfo &info)
{
    switch (callType) {
        case CallType::TYPE_CS: {
            std::unique_ptr<CSCall> csCall = std::make_unique<CSCall>(info);
            if (csCall != nullptr && csCall->DialCall() == TELEPHONY_NO_ERROR) {
                call = csCall.release();
                return TELEPHONY_NO_ERROR;
            }
            break;
        }
        case CallType::TYPE_IMS: {
            std::unique_ptr<IMSCall> imsCall = std::make_unique<IMSCall>(info);
            if (imsCall != nullptr && imsCall->DialCall() == TELEPHONY_NO_ERROR) {
                call = imsCall.release();
                return TELEPHONY_NO_ERROR;
            }
            break;
        }
        case CallType::TYPE_OTT: {
            std::unique_ptr<OTTCall> ottCall = std::make_unique<OTTCall>(info);
            if (ottCall != nullptr && ottCall->DialCall() == TELEPHONY_NO_ERROR) {
                call = ottCall.release();
                return TELEPHONY_NO_ERROR;
            }
            break;
        }
        default:
            CALLMANAGER_ERR_LOG("Invalid call type!");
            break;
    }
    return CALL_MANAGER_DIAL_FAILED;
}

bool CallControlManager::NotifyNewCallCreated(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("callObjectPtr is null!");
        return false;
    }
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->NewCallCreated(callObjectPtr);
    }
    return true;
}

bool CallControlManager::NotifyCallDestroyed(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("callObjectPtr is null!");
        return false;
    }
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->CallDestroyed(callObjectPtr);
    }
    return true;
}

bool CallControlManager::NotifyCallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallStates priorState, TelCallStates nextState)
{
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("callObjectPtr is null!");
        return false;
    }
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->CallStateUpdated(callObjectPtr, priorState, nextState);
    }
    return true;
}

bool CallControlManager::NotifyIncomingCallAnswered(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("callObjectPtr is null!");
        return false;
    }
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->IncomingCallActivated(callObjectPtr);
    }
    return true;
}

bool CallControlManager::NotifyIncomingCallRejected(
    sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("callObjectPtr is null!");
        return false;
    }
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->IncomingCallHungUp(callObjectPtr, isSendSms, content);
    }
    return true;
}

int32_t CallControlManager::GetSimNumber()
{
    return simInfo_.simId;
}

int32_t CallControlManager::GetID()
{
    return TELEPHONY_NO_ERROR;
}

int32_t GetCallerInfoDate(ContactInfo info)
{
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS
