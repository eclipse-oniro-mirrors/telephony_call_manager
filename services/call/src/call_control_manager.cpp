/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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
#include "cpp/task_ext.h"
#include <securec.h>
#include "csignal"
#include <string_ex.h>

#include "audio_control_manager.h"
#include "bluetooth_call_manager.h"
#include "call_ability_report_proxy.h"
#include "call_connect_ability.h"
#include "call_dialog.h"
#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "call_number_utils.h"
#include "call_records_manager.h"
#include "call_state_report_proxy.h"
#include "cellular_call_connection.h"
#include "common_type.h"
#include "ims_call.h"
#include "iservice_registry.h"
#include "reject_call_sms.h"
#include "report_call_info_handler.h"
#include "satellite_call_control.h"
#include "telephony_log_wrapper.h"
#include "video_control_manager.h"
#include "audio_device_manager.h"
#include "distributed_call_manager.h"
#include "call_superprivacy_control_manager.h"

namespace OHOS {
namespace Telephony {
bool CallControlManager::alarmSeted = false;
const uint64_t DISCONNECT_DELAY_TIME = 1000000;
static const int32_t SATCOMM_UID = 1096;
using namespace OHOS::EventFwk;
CallControlManager::CallControlManager()
    : callStateListenerPtr_(nullptr), CallRequestHandlerPtr_(nullptr), incomingCallWakeup_(nullptr),
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
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy != nullptr) {
            samgrProxy->UnSubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, statusChangeListener_);
            samgrProxy->UnSubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, pageStateChangeListener_);
            samgrProxy->UnSubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, satcommEventListener_);
            samgrProxy->UnSubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, superPrivacyEventListener_);
            pageStateChangeListener_ = nullptr;
            statusChangeListener_ = nullptr;
            satcommEventListener_ = nullptr;
            superPrivacyEventListener_ = nullptr;
        }
    }
}

bool CallControlManager::Init()
{
    callStateListenerPtr_ = std::make_unique<CallStateListener>();
    if (callStateListenerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStateListenerPtr_ is null");
        return false;
    }
    CallRequestHandlerPtr_ = std::make_unique<CallRequestHandler>();
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is null");
        return false;
    }
    CallRequestHandlerPtr_->Init();
    incomingCallWakeup_ = std::make_shared<IncomingCallWakeup>();
    if (incomingCallWakeup_ == nullptr) {
        TELEPHONY_LOGE("incomingCallWakeup_ is null");
        return false;
    }
    missedCallNotification_ = std::make_shared<MissedCallNotification>();
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
    DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->RegisterSuperPrivacyMode();
    DelayedSingleton<CallStateReportProxy>::GetInstance()->UpdateCallStateForVoIPOrRestart();
    return true;
}

int32_t CallControlManager::DialCall(std::u16string &number, AppExecFwk::PacMap &extras)
{
    sptr<CallBase> callObjectPtr = nullptr;
    std::string accountNumber(Str16ToStr8(number));
    int32_t ret = NumberLegalityCheck(accountNumber);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid number!");
        return ret;
    }
    bool isEcc = false;
    DelayedSingleton<CallNumberUtils>::GetInstance()->CheckNumberIsEmergency(
        accountNumber, extras.GetIntValue("accountId"), isEcc);
    if (isEcc) {
        extras.PutIntValue("dialScene", (int32_t)DialScene::CALL_EMERGENCY);
    }
    ret = DelayedSingleton<SatelliteCallControl>::GetInstance()->IsSatelliteSwitchEnable();
    if (ret == TELEPHONY_SUCCESS) {
        ret = DelayedSingleton<SatelliteCallControl>::GetInstance()->IsAllowedSatelliteDialCall();
        if (ret != TELEPHONY_SUCCESS) {
            return ret;
        } else {
            extras.PutIntValue("callType", (int32_t)CallType::TYPE_SATELLITE);
        }
    }
    ret = CanDial(number, extras, isEcc);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("can dial policy result:%{public}d", ret);
        return ret;
    }
    if (!IsSupportVideoCall(extras)) {
        extras.PutIntValue("videoState", (int32_t)VideoStateType::TYPE_VOICE);
    }
    VideoStateType videoState = (VideoStateType)extras.GetIntValue("videoState");
    if (videoState == VideoStateType::TYPE_VIDEO) {
        extras.PutIntValue("callType", (int32_t)CallType::TYPE_IMS);
    }
    // temporarily save dial information
    PackageDialInformation(extras, accountNumber, isEcc);
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = CallRequestHandlerPtr_->DialCall();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("DialCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::CanDial(std::u16string &number, AppExecFwk::PacMap &extras, bool isEcc)
{
    int32_t ret = DialPolicy(number, extras, isEcc);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("dial policy result:%{public}d", ret);
        return ret;
    }
    ret = CanDialMulityCall(extras, isEcc);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("dial policy result:%{public}d", ret);
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

void CallControlManager::PackageDialInformation(AppExecFwk::PacMap &extras, std::string accountNumber, bool isEcc)
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
    dialSrcInfo_.originalCallType = (int32_t)extras.GetIntValue("videoState");
    dialSrcInfo_.bundleName = extras.GetStringValue("bundleName");
    extras_.Clear();
    extras_ = extras;
}

int32_t CallControlManager::AnswerCall(int32_t callId, int32_t videoState)
{
    sptr<CallBase> call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (call == nullptr) {
        TELEPHONY_LOGE("call is nullptr");
        CallManagerHisysevent::WriteAnswerCallFaultEvent(
            INVALID_PARAMETER, callId, videoState, TELEPHONY_ERR_LOCAL_PTR_NULL, "call is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (callId == INVALID_CALLID) {
        callId = call->GetCallID();
    }
    call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("call is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (CurrentIsSuperPrivacyMode(callId, videoState)) {
        return TELEPHONY_SUCCESS;
    }
    AnswerHandlerForSatelliteOrVideoCall(call, videoState);
    TELEPHONY_LOGI("report answered state");
    NotifyCallStateUpdated(call, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_ANSWERED);
    CarrierAndVoipConflictProcess(callId, TelCallState::CALL_STATUS_ANSWERED);
    if (VoIPCallState_ != CallStateToApp::CALL_STATE_IDLE) {
            TELEPHONY_LOGW("VoIP call is active, waiting for VoIP to disconnect");
            AnsweredCallQueue_.hasCall = true;
            AnsweredCallQueue_.callId = callId;
            AnsweredCallQueue_.videoState = videoState;
            return TELEPHONY_SUCCESS;
    }
    int32_t ret = AnswerCallPolicy(callId, videoState);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("AnswerCallPolicy failed!");
        CallManagerHisysevent::WriteAnswerCallFaultEvent(
            INVALID_PARAMETER, callId, videoState, ret, "AnswerCallPolicy failed");
        return ret;
    }
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = CallRequestHandlerPtr_->AnswerCall(callId, videoState);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("AnswerCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

bool CallControlManager::CurrentIsSuperPrivacyMode(int32_t callId, int32_t videoState)
{
    bool currentIsSuperPrivacyMode = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->
        GetCurrentIsSuperPrivacyMode();
    TELEPHONY_LOGI("call policy answer currentIsSuperPrivacyMode:%{public}d", currentIsSuperPrivacyMode);
    if (currentIsSuperPrivacyMode) {
        DelayedSingleton<CallDialog>::GetInstance()->DialogConnectAnswerPrivpacyModeExtension("SUPER_PRIVACY_MODE",
            callId, videoState, true);
        return true;
    }
    return false;
}

void CallControlManager::AnswerHandlerForSatelliteOrVideoCall(sptr<CallBase> &call, int32_t videoState)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("call is nullptr");
        return;
    }
    if (call->GetCallType() == CallType::TYPE_IMS && videoState != static_cast<int32_t>(call->GetVideoStateType())) {
        call->SetVideoStateType(static_cast<VideoStateType>(videoState));
        sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(call.GetRefPtr());
        imsCall->InitVideoCall();
        TELEPHONY_LOGI("videoState has changed");
    }
    if (call->GetCallType() == CallType::TYPE_SATELLITE) {
        sptr<CallBase> foregroundSatelliteCall =
            CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
        if (foregroundSatelliteCall != nullptr) {
            CallRequestHandlerPtr_->HangUpCall(foregroundSatelliteCall->GetCallID());
        }
    }
}

int32_t CallControlManager::CarrierAndVoipConflictProcess(int32_t callId, TelCallState callState)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("CarrierAndVoipConflictProcess, call is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (call->GetCallType() == CallType::TYPE_CS ||
        call->GetCallType() == CallType::TYPE_IMS ||
        call->GetCallType() == CallType::TYPE_SATELLITE) {
        std::list<int32_t> voipCallIdList;
        int32_t ret = GetVoipCallList(voipCallIdList);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("GetVoipCallList failed!");
            return ret;
        }
        for (auto voipCallId : voipCallIdList) {
            sptr<CallBase> voipCall = GetOneCallObject(voipCallId);
            if (voipCall->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING ||
                voipCall->GetTelCallState() == TelCallState::CALL_STATUS_WAITING) {
                ret = RejectCall(voipCallId, true, u"CarrierAndVoipConflictProcess");
            } else if (callState == TelCallState::CALL_STATUS_ANSWERED) {
                ret = HangUpCall(voipCallId);
            }
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("CarrierAndVoipConflictProcess failed!");
                return ret;
            }
        }
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::RejectCall(int32_t callId, bool rejectWithMessage, std::u16string textMessage)
{
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    if (callId == INVALID_CALLID) {
        sptr<CallBase> call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
        if (call == nullptr) {
            TELEPHONY_LOGE("call is nullptr");
            CallManagerHisysevent::WriteHangUpFaultEvent(
                INVALID_PARAMETER, callId, TELEPHONY_ERR_LOCAL_PTR_NULL, "Reject call is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        callId = call->GetCallID();
    }

    int32_t ret = RejectCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RejectCallPolicy failed!");
        CallManagerHisysevent::WriteHangUpFaultEvent(INVALID_PARAMETER, callId, ret, "Reject RejectCallPolicy failed");
        return ret;
    }
    std::string messageStr(Str16ToStr8(textMessage));
    ret = CallRequestHandlerPtr_->RejectCall(callId, rejectWithMessage, messageStr);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RejectCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::HangUpCall(int32_t callId)
{
    if (callId == INVALID_CALLID) {
        std::vector<CallRunningState> callRunningStateVec;
        callRunningStateVec.push_back(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
        callRunningStateVec.push_back(CallRunningState::CALL_RUNNING_STATE_DIALING);
        callRunningStateVec.push_back(CallRunningState::CALL_RUNNING_STATE_CONNECTING);

        for (auto &state : callRunningStateVec) {
            sptr<CallBase> call = GetOneCallObject(state);
            if (call != nullptr) {
                callId = call->GetCallID();
                break;
            }
        }

        if (callId == INVALID_CALLID) {
            TELEPHONY_LOGE("callId is INVALID_CALLID!");
            CallManagerHisysevent::WriteHangUpFaultEvent(
                INVALID_PARAMETER, callId, TELEPHONY_ERR_LOCAL_PTR_NULL, "HangUp callId is INVALID_CALLID");
            return TELEPHONY_ERR_ARGUMENT_INVALID;
        }
    }

    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = HangUpPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HangUpPolicy failed!");
        CallManagerHisysevent::WriteHangUpFaultEvent(INVALID_PARAMETER, callId, ret, "HangUp HangUpPolicy failed");
        return ret;
    }
    ret = CallRequestHandlerPtr_->HangUpCall(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HangUpCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::GetCallState()
{
    CallStateToApp callState = CallStateToApp::CALL_STATE_UNKNOWN;
    if (!HasCellularCallExist()) {
        callState = CallStateToApp::CALL_STATE_IDLE;
    } else {
        callState = CallStateToApp::CALL_STATE_OFFHOOK;
        bool hasRingingCall = false;
        if ((HasRingingCall(hasRingingCall) == TELEPHONY_SUCCESS) && hasRingingCall) {
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
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = CallRequestHandlerPtr_->HoldCall(callId);
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
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = CallRequestHandlerPtr_->UnHoldCall(callId);
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
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = CallRequestHandlerPtr_->SwitchCall(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SwitchCall failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

bool CallControlManager::HasCall()
{
    return HasCellularCallExist();
}

int32_t CallControlManager::IsNewCallAllowed(bool &enabled)
{
    return IsNewCallAllowedCreate(enabled);
}

int32_t CallControlManager::IsRinging(bool &enabled)
{
    return HasRingingCall(enabled);
}

int32_t CallControlManager::HasEmergency(bool &enabled)
{
    return HasEmergencyCall(enabled);
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

bool CallControlManager::NotifyCallDestroyed(const DisconnectedDetails &details)
{
    if (callStateListenerPtr_ != nullptr) {
        callStateListenerPtr_->CallDestroyed(details);
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
        TELEPHONY_LOGI("NotifyCallStateUpdated priorState:%{public}d,nextState:%{public}d", priorState, nextState);
        callStateListenerPtr_->CallStateUpdated(callObjectPtr, priorState, nextState);
        if (callObjectPtr->GetCallType() == CallType::TYPE_VOIP) {
            return true;
        }
        if (priorState == TelCallState::CALL_STATUS_DIALING &&
            (nextState == TelCallState::CALL_STATUS_ALERTING || nextState == TelCallState::CALL_STATUS_ACTIVE)) {
            TELEPHONY_LOGI("call is actived, now check and switch call to distributed audio device");
            DelayedSingleton<AudioDeviceManager>::GetInstance()->CheckAndSwitchDistributedAudioDevice();
        } else if ((priorState == TelCallState::CALL_STATUS_ACTIVE &&
            nextState == TelCallState::CALL_STATUS_DISCONNECTED) ||
            (priorState == TelCallState::CALL_STATUS_DISCONNECTING &&
            nextState == TelCallState::CALL_STATUS_DISCONNECTED)) {
            bool hasHoldCall = false;
            if ((HasHoldCall(hasHoldCall) == TELEPHONY_SUCCESS) && !hasHoldCall) {
                TELEPHONY_LOGI("call is disconnected, clear distributed call state");
                DelayedSingleton<AudioDeviceManager>::GetInstance()->OnActivedCallDisconnected();
            }
        } else if (priorState == TelCallState::CALL_STATUS_WAITING &&
            nextState == TelCallState::CALL_STATUS_ACTIVE) {
            if (DelayedSingleton<DistributedCallManager>::GetInstance()->IsDCallDeviceSwitchedOn()) {
                TELEPHONY_LOGI("answer multi-line call, need switch again.");
                DelayedSingleton<AudioDeviceManager>::GetInstance()->CheckAndSwitchDistributedAudioDevice();
            }
        }
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
        TELEPHONY_LOGI("call is answered, now check and switch call to distributed audio device");
        DelayedSingleton<AudioDeviceManager>::GetInstance()->CheckAndSwitchDistributedAudioDevice();
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
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!call->IsAliveState()) {
        return CALL_ERR_CALL_STATE_MISMATCH_OPERATION;
    }

    int32_t ret = call->StartDtmf(str);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StartDtmf failed, return:%{public}d", ret);
    }
    ffrt::submit([str]() {
        DelayedSingleton<AudioControlManager>::GetInstance()->PlayDtmfTone(str);
    });
    return ret;
}

int32_t CallControlManager::StopDtmf(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!call->IsAliveState()) {
        return CALL_ERR_CALL_STATE_MISMATCH_OPERATION;
    }

    int32_t ret = call->StopDtmf();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StopDtmf failed, return:%{public}d", ret);
    }
    DelayedSingleton<AudioControlManager>::GetInstance()->StopDtmfTone();
    return ret;
}

int32_t CallControlManager::PostDialProceed(int32_t callId, bool proceed)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!call->IsAliveState()) {
        return CALL_ERR_CALL_STATE_MISMATCH_OPERATION;
    }

    int32_t ret = call->PostDialProceed(proceed);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PostDialProceed failed, return:%{public}d", ret);
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

int32_t CallControlManager::SetCallRestrictionPassword(
    int32_t slotId, CallRestrictionType fac, const char *oldPassword, const char *newPassword)
{
    int32_t ret = CallPolicy::SetCallRestrictionPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("policy check failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetCallRestrictionPassword(slotId, fac, oldPassword, newPassword);
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

int32_t CallControlManager::CanSetCallTransferTime(int32_t slotId, bool &result)
{
    int32_t ret = CallPolicy::SetCallTransferInfoPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("[slot%{public}d] failed!", slotId);
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->CanSetCallTransferTime(slotId, result);
    } else {
        TELEPHONY_LOGE("[slot%{public}d]  callSettingManagerPtr_ is nullptr!", slotId);
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
        return TELEPHONY_ERR_ARGUMENT_INVALID;
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
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = CallRequestHandlerPtr_->CombineConference(mainCallId);
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
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    int32_t ret = call->CanSeparateConference();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CanSeparateConference failed");
        return ret;
    }
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = CallRequestHandlerPtr_->SeparateConference(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SeparateConference failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::KickOutFromConference(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("GetOneCallObject failed, callId:%{public}d", callId);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    int32_t ret = call->CanKickOutFromConference();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CanKickOutFromConference failed");
        return ret;
    }
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = CallRequestHandlerPtr_->KickOutFromConference(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("KickOutFromConference failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::GetMainCallId(int32_t callId, int32_t &mainCallId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("GetMainCallId failed! callId:%{public}d", callId);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    return call->GetMainCallId(mainCallId);
}

int32_t CallControlManager::GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("GetSubCallIdList failed! callId:%{public}d", callId);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    return call->GetSubCallIdList(callIdList);
}

int32_t CallControlManager::GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("GetCallIdListForConference failed! callId:%{public}d", callId);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    return call->GetCallIdListForConference(callIdList);
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

int32_t CallControlManager::IsImsSwitchEnabled(int32_t slotId, bool &enabled)
{
    int32_t ret = CallPolicy::IsVoLteEnabledPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("IsImsSwitchEnabled failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->IsImsSwitchEnabled(slotId, enabled);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::SetVoNRState(int32_t slotId, int32_t state)
{
    int32_t ret = CallPolicy::VoNRStatePolicy(slotId, state);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetVoNRState failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->SetVoNRState(slotId, state);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::GetVoNRState(int32_t slotId, int32_t &state)
{
    int32_t ret = CallPolicy::IsValidSlotId(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetVoNRState failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        ret = callSettingManagerPtr_->GetVoNRState(slotId, state);
        if (ret == TELEPHONY_SUCCESS) {
            ret = CallPolicy::VoNRStatePolicy(slotId, state);
        }
        return ret;
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallControlManager::UpdateImsCallMode(int32_t callId, ImsCallMode mode)
{
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::StartRtt(int32_t callId, std::u16string &msg)
{
    int32_t ret = CallPolicy::StartRttPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("NO IMS call,can not StartRtt!");
        return ret;
    }
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = CallRequestHandlerPtr_->StartRtt(callId, msg);
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
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = CallRequestHandlerPtr_->StopRtt(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StopRtt failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::JoinConference(int32_t callId, std::vector<std::u16string> &numberList)
{
    if (CallRequestHandlerPtr_ == nullptr) {
        TELEPHONY_LOGE("CallRequestHandlerPtr_ is nullptr!");
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
    ret = CallRequestHandlerPtr_->JoinConference(callId, phoneNumberList);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("JoinConference failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::SetMuted(bool isMute)
{
    sptr<CallBase> call = CallObjectManager::GetForegroundLiveCall();
    if (call == nullptr) {
        return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
    }

    if (call->GetCallType() == CallType::TYPE_VOIP) {
        TELEPHONY_LOGI("SetMute by voip");
        return call->SetMute(isMute, call->GetSlotId());
    }
    if (call->GetCallType() == CallType::TYPE_IMS
        || call->GetCallType() == CallType::TYPE_CS) {
        TELEPHONY_LOGI("SetMute by ims or cs,isMute = %{public}d", isMute);
        call->SetMute(isMute, call->GetSlotId());
    }

    return DelayedSingleton<AudioControlManager>::GetInstance()->SetMute(isMute);
}

int32_t CallControlManager::MuteRinger()
{
    return DelayedSingleton<AudioControlManager>::GetInstance()->MuteRinger();
}

int32_t CallControlManager::SetAudioDevice(const AudioDevice &audioDevice)
{
    return DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioDevice(audioDevice, true);
}

int32_t CallControlManager::ControlCamera(std::u16string cameraId, int32_t callingUid, int32_t callingPid)
{
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::SetPreviewWindow(VideoWindow &window)
{
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::SetDisplayWindow(VideoWindow &window)
{
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::SetCameraZoom(float zoomRatio)
{
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::SetPausePicture(std::u16string path)
{
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::SetDeviceDirection(int32_t rotation)
{
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, bool &enabled)
{
    if (IsValidSlotId(slotId)) {
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return DelayedSingleton<CallNumberUtils>::GetInstance()->CheckNumberIsEmergency(
        Str16ToStr8(number), slotId, enabled);
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

int32_t CallControlManager::CloseUnFinishedUssd(int32_t slotId)
{
    int32_t ret = CallPolicy::CloseUnFinishedUssdPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CloseUnFinishedUssd failed!");
        return ret;
    }
    if (callSettingManagerPtr_ != nullptr) {
        return callSettingManagerPtr_->CloseUnFinishedUssd(slotId);
    } else {
        TELEPHONY_LOGE("callSettingManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
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

void CallControlManager::handler()
{
    alarmSeted = false;
    TELEPHONY_LOGE("handle DisconnectAbility");
    if (!CallObjectManager::HasCallExist()) {
        DelayedSingleton<CallConnectAbility>::GetInstance()->DisconnectAbility();
    }
}

bool CallControlManager::cancel(ffrt::task_handle &handle)
{
    if (handle != nullptr) {
        TELEPHONY_LOGI("skip disconnect ability task");
        int ret = ffrt::skip(handle);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("skip task failed, ret = %{public}d", ret);
            return false;
        }
    }
    return true;
}

void CallControlManager::ConnectCallUiService(bool shouldConnect)
{
    if (shouldConnect) {
        if (alarmSeted) {
            if (!cancel(disconnectHandle)) {
                return;
            }
            alarmSeted = false;
        }
        DelayedSingleton<CallConnectAbility>::GetInstance()->ConnectAbility();
        shouldDisconnect = false;
    } else {
        shouldDisconnect = true;
        if (!alarmSeted) {
            TELEPHONY_LOGI("submit delay disconnect ability");
            disconnectHandle = ffrt::submit_h([&]() {
                handler();
            }, {}, {}, ffrt::task_attr().delay(DISCONNECT_DELAY_TIME));
            alarmSeted = true;
        }  else {
            if (!cancel(disconnectHandle)) {
                return;
            }
            TELEPHONY_LOGI("submit delay disconnect ability");
            disconnectHandle = ffrt::submit_h([&]() {
                handler();
            }, {}, {}, ffrt::task_attr().delay(DISCONNECT_DELAY_TIME));
        }
    }
}

bool CallControlManager::ShouldDisconnectService()
{
    return shouldDisconnect;
}

int32_t CallControlManager::RemoveMissedIncomingCallNotification()
{
    int32_t ret = DelayedSingleton<CallRecordsManager>::GetInstance()->RemoveMissedIncomingCallNotification();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RemoveMissedIncomingCallNotification failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::SetVoIPCallState(int32_t state)
{
    TELEPHONY_LOGI("VoIP state is %{public}d", state);
    VoIPCallState_ = (CallStateToApp)state;
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    DelayedSingleton<CallStateReportProxy>::GetInstance()->UpdateCallStateForVoIPOrRestart();
    IPCSkeleton::SetCallingIdentity(identity);
    if (VoIPCallState_ == CallStateToApp::CALL_STATE_ANSWERED) {
        TELEPHONY_LOGI("VoIP answered the call, should hangup all calls");
        std::list<sptr<CallBase>> allCallList = CallObjectManager::GetAllCallList();
        for (auto call : allCallList) {
            int32_t ret = HangUpCall(call->GetCallID());
            if (ret != TELEPHONY_SUCCESS) {
                return ret;
            }
        }
    }
    if (VoIPCallState_ == CallStateToApp::CALL_STATE_OFFHOOK) {
        HangUpVoipCall();
    }
    if (VoIPCallState_ == CallStateToApp::CALL_STATE_IDLE) {
            TELEPHONY_LOGI("VoIP call state is not active");
            if (AnsweredCallQueue_.hasCall) {
                AnsweredCallQueue_.hasCall = false;
                return AnswerCall(AnsweredCallQueue_.callId, AnsweredCallQueue_.videoState);
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::HangUpVoipCall()
{
    std::list<sptr<CallBase>> allCallList = CallObjectManager::GetAllCallList();
    for (auto call : allCallList) {
        if (call != nullptr && call->GetCallType() == CallType::TYPE_VOIP) {
            TELEPHONY_LOGI("HangUp VoipCall callId %{public}d", call->GetCallID());
            int32_t ret = HangUpCall(call->GetCallID());
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("hangup voip call %{public}d failed!", call->GetCallID());
                return ret;
            }
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::GetVoIPCallState(int32_t &state)
{
    state = (int32_t)VoIPCallState_;
    return TELEPHONY_SUCCESS;
}

void CallControlManager::CallStateObserve()
{
    if (callStateListenerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStateListenerPtr_ is null");
        return;
    }
    std::shared_ptr<RejectCallSms> hangUpSmsPtr = std::make_shared<RejectCallSms>();
    if (hangUpSmsPtr == nullptr) {
        TELEPHONY_LOGE("hangUpSmsPtr is null");
        return;
    }
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<CallAbilityReportProxy>::GetInstance());
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<CallStateReportProxy>::GetInstance());
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<AudioControlManager>::GetInstance());
    callStateListenerPtr_->AddOneObserver(hangUpSmsPtr);
    callStateListenerPtr_->AddOneObserver(missedCallNotification_);
    callStateListenerPtr_->AddOneObserver(incomingCallWakeup_);
    callStateListenerPtr_->AddOneObserver(DelayedSingleton<CallRecordsManager>::GetInstance());
}

int32_t CallControlManager::AddCallLogAndNotification(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("callObjectPtr is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callObjectPtr->SetAnswerType(CallAnswerType::CALL_ANSWER_MISSED);
    DelayedSingleton<CallRecordsManager>::GetInstance()
        ->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_DISCONNECTED);
    if (missedCallNotification_ == nullptr) {
        TELEPHONY_LOGE("missedCallNotification is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    missedCallNotification_->PublishMissedCallEvent(callObjectPtr);
    missedCallNotification_->PublishMissedCallNotification(callObjectPtr);
    return TELEPHONY_SUCCESS;
}

int32_t CallControlManager::AddBlockLogAndNotification(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("callObjectPtr is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callObjectPtr->SetAnswerType(CallAnswerType::CALL_ANSWER_BLOCKED);
    DelayedSingleton<CallRecordsManager>::GetInstance()
        ->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_DISCONNECTED);
    if (missedCallNotification_ == nullptr) {
        TELEPHONY_LOGE("missedCallNotification is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    missedCallNotification_->PublishBlockedCallEvent(callObjectPtr);
    return TELEPHONY_SUCCESS;
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

void CallControlManager::AcquireIncomingLock()
{
    if (incomingCallWakeup_ == nullptr) {
        return;
    }
    incomingCallWakeup_->AcquireIncomingLock();
}

void CallControlManager::ReleaseIncomingLock()
{
    if (incomingCallWakeup_ == nullptr) {
        return;
    }
    incomingCallWakeup_->ReleaseIncomingLock();
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
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SIM_STATE_CHANGED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_NAME_UPDATE);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    subscriberInfo.SetThreadMode(EventFwk::CommonEventSubscribeInfo::COMMON);
    std::shared_ptr<CallBroadcastSubscriber> subscriberPtr = std::make_shared<CallBroadcastSubscriber>(subscriberInfo);
    if (subscriberPtr == nullptr) {
        TELEPHONY_LOGE("CallControlManager::BroadcastSubscriber subscriberPtr is nullptr");
        return TELEPHONY_ERROR;
    }

    EventFwk::MatchingSkills matchingSkill;
    matchingSkill.AddEvent("event.custom.contacts.PAGE_STATE_CHANGE");
    EventFwk::CommonEventSubscribeInfo subscriberInfomation(matchingSkill);
    subscriberInfomation.SetThreadMode(EventFwk::CommonEventSubscribeInfo::COMMON);
    subscriberInfomation.SetPermission("ohos.permission.SET_TELEPHONY_STATE");
    std::shared_ptr<CallBroadcastSubscriber> subscriber =
        std::make_shared<CallBroadcastSubscriber>(subscriberInfomation);
    if (subscriber == nullptr) {
        TELEPHONY_LOGE("CallControlManager::BroadcastSubscriber subscriber is nullptr");
        return TELEPHONY_ERROR;
    }

    EventFwk::MatchingSkills matchingSkills_;
    matchingSkills.AddEvent("usual.event.thermal.satcomm.HIGH_TEMP_LEVEL");
    EventFwk::CommonEventSubscribeInfo subscriberInfo_(matchingSkills_);
    subscriberInfo_.SetThreadMode(EventFwk::CommonEventSubscribeInfo::COMMON);
    subscriberInfo_.SetPublisherUid(SATCOMM_UID);
    std::shared_ptr<CallBroadcastSubscriber> subscriberPtr_ =
        std::make_shared<CallBroadcastSubscriber>(subscriberInfo_);
    if (subscriberPtr_ == nullptr) {
        TELEPHONY_LOGE("CallControlManager::BroadcastSubscriber subscriberPtr_ is nullptr");
        return TELEPHONY_ERROR;
    }

    EventFwk::MatchingSkills matchingSkill_;
    matchingSkills.AddEvent("usual.event.SUPER_PRIVACY_MODE");
    EventFwk::CommonEventSubscribeInfo subscriberInfos_(matchingSkill_);
    subscriberInfos_.SetPublisherBundleName("com.settings");
    subscriberInfos_.SetPermission("ohos.permission.SET_TELEPHONY_STATE");
    subscriberInfos_.SetThreadMode(EventFwk::CommonEventSubscribeInfo::COMMON);
    std::shared_ptr<CallBroadcastSubscriber> subscriberPtrs_ =
        std::make_shared<CallBroadcastSubscriber>(subscriberInfos_);
    if (subscriberPtrs_ == nullptr) {
        TELEPHONY_LOGE("CallControlManager::BroadcastSubscriber subscriberPtrs_ is nullptr");
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
    pageStateChangeListener_ = new (std::nothrow) SystemAbilityListener(subscriber);
    if (pageStateChangeListener_ == nullptr) {
        TELEPHONY_LOGE("pageStateChangeListener_ is nullptr");
        return TELEPHONY_ERROR;
    }
    satcommEventListener_ =
        new (std::nothrow) SystemAbilityListener(subscriberPtr_);
    if (satcommEventListener_ == nullptr) {
        TELEPHONY_LOGE("satcommEventListener_ is nullptr");
        return TELEPHONY_ERROR;
    }
    superPrivacyEventListener_ =
        new (std::nothrow) SystemAbilityListener(subscriberPtrs_);
    if (superPrivacyEventListener_ == nullptr) {
        TELEPHONY_LOGE("superPrivacyEventListener_ is nullptr");
        return TELEPHONY_ERROR;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, statusChangeListener_);
    TELEPHONY_LOGI("CallControlManager::BroadcastSubscriber statusChangeListener_ ret: %{public}d", ret);
    ret = samgrProxy->SubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, pageStateChangeListener_);
    TELEPHONY_LOGI("CallControlManager::BroadcastSubscriber pageStateChangeListener_ ret: %{public}d", ret);
    ret = samgrProxy->SubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, satcommEventListener_);
    TELEPHONY_LOGI("CallControlManager::BroadcastSubscriber satcommEventListener_ ret: %{public}d", ret);
    ret = samgrProxy->SubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, superPrivacyEventListener_);
    TELEPHONY_LOGI("CallControlManager::BroadcastSubscriber superPrivacyEventListener_ ret: %{public}d", ret);
    return TELEPHONY_SUCCESS;
}

#ifdef ABILITY_MEDIA_SUPPORT
bool CallControlManager::onButtonDealing(HeadsetButtonService::ButtonEvent type)
{
    bool isRingState = false;
    sptr<CallBase> call = nullptr;

    if (GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING) != nullptr) {
        call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
        isRingState = true;
    } else if (GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING) != nullptr) {
        call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING);
    } else if (GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE) != nullptr) {
        call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    } else if (GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD) != nullptr) {
        call = GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
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
