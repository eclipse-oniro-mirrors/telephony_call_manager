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
#include "telephony_log_wrapper.h"

#include "common_type.h"
#include "cs_call.h"
#include "ims_call.h"
#include "ott_call.h"
#include "report_call_state_handler.h"
#include "audio_control_manager.h"
#include "video_control_manager.h"
#include "bluetooth_call_manager.h"
#include "call_ability_handler.h"
#include "cellular_call_ipc_interface_proxy.h"
#include "call_broadcast_subscriber.h"
#include "call_records_manager.h"
#include "call_number_utils.h"

namespace OHOS {
namespace Telephony {
CallControlManager::CallControlManager() : callStateListenerPtr_(nullptr), callRequestHandlerServicePtr_(nullptr)
{
    dialSrcInfo_.callId = ERR_ID;
    dialSrcInfo_.number = "";
    extras_.Clear();
    dialSrcInfo_.isDialing = false;
}

CallControlManager::~CallControlManager() {}

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
    reportCallStateHandlerPtr_ = (std::make_unique<ReportCallStateHandlerService>()).release();
    if (reportCallStateHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("reportCallStateHandlerPtr_ is null");
        return false;
    }
    reportCallStateHandlerPtr_->Start();
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
    DelayedSingleton<BluetoothCallManager>::GetInstance()->Init();
    callStateListenerPtr_->AddOneObserver(reportCallStateHandlerPtr_);
    hangUpSms_ = (std::make_unique<HangUpSms>()).release();
    missedCallNotification_ = (std::make_unique<MissedCallNotification>()).release();
    callStateListenerPtr_->AddOneObserver(hangUpSms_);
    callStateListenerPtr_->AddOneObserver(missedCallNotification_);
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<AudioControlManager>::GetInstance().get());
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<CallAbilityHandlerService>::GetInstance().get());
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<CallRecordsManager>::GetInstance().get());
    return true;
}

int32_t CallControlManager::DialCall(std::u16string &number, AppExecFwk::PacMap &extras)
{
    int32_t errorCode = TELEPHONY_ERROR;
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
        TELEPHONY_LOGE("dial policy result：%{public}d", ret);
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
    return (int32_t)callState;
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
        CallEventInfo info;
        info.eventId = EVENT_DISCONNECTED_UNKNOW;
        callStateListenerPtr_->CallEventUpdated(info);
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
        TELEPHONY_LOGE("call is null!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    if (!call->IsAliveState()) {
        TELEPHONY_LOGE("call not activated!");
        return CALL_ERR_CALL_STATE_MISMATCH_OPERATION;
    }

    int32_t ret = call->StartDtmf(str);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StartDtmf failed, return:%{public}d", ret);
    }
    return ret;
}

int32_t CallControlManager::SendDtmf(int32_t callId, char str)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("call is null!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    if (!call->IsAliveState()) {
        TELEPHONY_LOGE("call not activated!");
        return CALL_ERR_CALL_STATE_MISMATCH_OPERATION;
    }
    std::string phoneNum = call->GetAccountNumber();
    int32_t ret = call->SendDtmf(phoneNum, str);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SendDtmf failed, return:%{public}d", ret);
    }
    return ret;
}

int32_t CallControlManager::SendBurstDtmf(int32_t callId, std::u16string str, int32_t on, int32_t off)
{
    std::string strCode = Str16ToStr8(str);
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("call is null!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    if (!((on == DtmfPlaytime::DTMF_PLAY_TONE_MSEC_0) || (on == DtmfPlaytime::DTMF_PLAY_TONE_MSEC_1) ||
        (on == DtmfPlaytime::DTMF_PLAY_TONE_MSEC_2) || (on == DtmfPlaytime::DTMF_PLAY_TONE_MSEC_3) ||
        (on == DtmfPlaytime::DTMF_PLAY_TONE_MSEC_4) || (on == DtmfPlaytime::DTMF_PLAY_TONE_MSEC_5) ||
        (on == DtmfPlaytime::DTMF_PLAY_TONE_MSEC_6) || (on == DtmfPlaytime::DTMF_PLAY_TONE_MSEC_7))) {
        return CALL_ERR_DTMF_PARAMETER_INVALID;
    }
    if ((off < DtmfPlayIntervalTime::DTMF_PLAY_TONE_MIN_INTERVAL_MSEC) ||
        (off > DtmfPlayIntervalTime::DTMF_PLAY_TONE_MAX_INTERVAL_MSEC)) {
        off = DtmfPlayIntervalTime::DTMF_PLAY_TONE_DEFAULT_INTERVAL_MSEC;
    }
    if (on == DtmfPlaytime::DTMF_PLAY_TONE_MSEC_1) {
        on = DtmfPlaytime::DTMF_PLAY_TONE_DEFAULT_MSEC;
    }
    if (strCode.empty()) {
        return CALL_ERR_SEND_DTMF_INPUT_IS_EMPTY;
    }
    if (!call->IsAliveState()) {
        TELEPHONY_LOGE("call not activated!");
        return CALL_ERR_CALL_STATE_MISMATCH_OPERATION;
    }
    std::string phoneNum = call->GetAccountNumber();
    int32_t ret = call->SendBurstDtmf(phoneNum, strCode, on, off);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SendBurstDtmf failed, return:%{public}d", ret);
    }
    return ret;
}

int32_t CallControlManager::StopDtmf(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("call is null!");
        return CALL_ERR_CALL_OBJECT_IS_NULL;
    }
    if (!call->IsAliveState()) {
        TELEPHONY_LOGE("call not activated!");
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
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->GetCallWaiting(slotId);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetCallWaiting(int32_t slotId, bool activate)
{
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetCallWaiting(slotId, activate);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->GetCallRestriction(slotId, type);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetCallRestriction(slotId, info);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->GetCallTransferInfo(slotId, type);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetCallTransferInfo(slotId, info);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetCallPreferenceMode(slotId, mode);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::CombineConference(int32_t mainCallId)
{
    sptr<CallBase> mainCall = GetOneCallObject(mainCallId);
    if (mainCall == nullptr) {
        TELEPHONY_LOGE("GetOneCallObject failed, mainCallId:%{public}d", mainCallId);
        return CALL_ERR_CALL_OBJECT_IS_NULL;
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
    RETURN_FAILURE_IF_NULLPTR(call, "call is NULL!", CALL_ERR_CALL_OBJECT_IS_NULL);
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
    RETURN_FAILURE_IF_NULLPTR(call, "call is NULL!", TELEPHONY_ERROR);
    return call->GetMainCallId();
}

std::vector<std::u16string> CallControlManager::GetSubCallIdList(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        std::vector<std::u16string> vec;
        vec.clear();
        return vec;
    }
    return call->GetSubCallIdList();
}

std::vector<std::u16string> CallControlManager::GetCallIdListForConference(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        std::vector<std::u16string> vec;
        vec.clear();
        return vec;
    }
    return call->GetCallIdListForConference();
}

int32_t CallControlManager::UpgradeCall(int32_t callId)
{
    int32_t ret = CallPolicy::UpgradeCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("upgrade call failed !");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->UpgradeCall(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("upgrade call failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::DowngradeCall(int32_t callId)
{
    int32_t ret = CallPolicy::DowngradeCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("downgrade call failed !");
        return ret;
    }
    if (callRequestHandlerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestHandlerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = callRequestHandlerServicePtr_->DowngradeCall(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("downgrade call failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::SetMuted(bool isMute)
{
    return DelayedSingleton<AudioControlManager>::GetInstance()->SetMute(isMute);
}

int32_t CallControlManager::MuteRinger()
{
    return DelayedSingleton<AudioControlManager>::GetInstance()->MuteRinger();
}

int32_t CallControlManager::SetAudioDevice(AudioDevice deviceType)
{
    if (DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioDevice(deviceType)) {
        return TELEPHONY_SUCCESS;
    } else {
        return TELEPHONY_ERR_FAIL;
    }
}

int32_t CallControlManager::ControlCamera(
    std::u16string cameraId, std::u16string callingPackage, int32_t callingUid, int32_t callingPid)
{
    return DelayedSingleton<VideoControlManager>::GetInstance()->ControlCamera(
        cameraId, callingPackage, callingUid, callingPid);
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

int32_t CallControlManager::BroadcastSubscriber()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(SIM_STATE_UPDATE_ACTION);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<CallBroadcastSubscriber> subscriberPtr =
        std::make_shared<CallBroadcastSubscriber>(subscriberInfo);
    if (subscriberPtr != nullptr) {
        bool subscribeResult = CommonEventManager::SubscribeCommonEvent(subscriberPtr);
        if (!subscribeResult) {
            TELEPHONY_LOGW("SubscribeCommonEvent failed");
            return TELEPHONY_ERR_SUBSCRIBE_BROADCAST_FAIL;
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::CancelMissedCallsNotification(int32_t id)
{
    if (missedCallNotification_ == nullptr) {
        TELEPHONY_LOGE("missed call notification nullptr");
        return TELEPHONY_ERR_FAIL;
    }
    return missedCallNotification_->CancelMissedCallsNotification(id);
}

#ifdef ABILITY_MEDIA_SUPPORT
bool CallControlManager::onButtonDealing(HeadsetButtonService::ButtonEvent type)
{
    bool isRingState = false;
    sptr<CallBase> call = nullptr;
    if (call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING) != nullptr) {
        isRingState = true;
    } else if (call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING) != nullptr ||
        call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE) != nullptr ||
        call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD) != nullptr) {
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
