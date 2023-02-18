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

#include "ims_call.h"

#include "call_control_manager.h"
#include "call_object_manager.h"
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "ims_conference.h"

#include "cellular_call_connection.h"

namespace OHOS {
namespace Telephony {
IMSCall::IMSCall(DialParaInfo &info) : CarrierCall(info), videoCallState_(nullptr), isInitialized_(false) {}

IMSCall::IMSCall(DialParaInfo &info, AppExecFwk::PacMap &extras)
    : CarrierCall(info, extras), videoCallState_(nullptr), isInitialized_(false)
{}

IMSCall::~IMSCall() {}

int32_t IMSCall::InitVideoCall()
{
    if (isInitialized_) {
        TELEPHONY_LOGI("video call initialize ok!");
        return TELEPHONY_SUCCESS;
    }
    sptr<VideoCallState> state = new (std::nothrow) AudioOnlyState(this);
    if (state == nullptr) {
        TELEPHONY_LOGE("null pointer");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    videoStateMap_[ImsCallMode::CALL_MODE_AUDIO_ONLY] = state;
    videoCallState_ = state;
    state = (std::make_unique<VideoSendState>(this)).release();
    if (state == nullptr) {
        TELEPHONY_LOGE("null pointer");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    videoStateMap_[ImsCallMode::CALL_MODE_SEND_ONLY] = state;
    state = (std::make_unique<VideoReceiveState>(this)).release();
    if (state == nullptr) {
        TELEPHONY_LOGE("null pointer");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    videoStateMap_[ImsCallMode::CALL_MODE_RECEIVE_ONLY] = state;
    state = (std::make_unique<VideoSendReceiveState>(this)).release();
    if (state == nullptr) {
        TELEPHONY_LOGE("null pointer");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    videoStateMap_[ImsCallMode::CALL_MODE_SEND_RECEIVE] = state;
    state = (std::make_unique<VideoPauseState>(this)).release();
    if (state == nullptr) {
        TELEPHONY_LOGE("null pointer");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    videoStateMap_[ImsCallMode::CALL_MODE_VIDEO_PAUSED] = state;
    isInitialized_ = true;
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::DialingProcess()
{
    return CarrierDialingProcess();
}

int32_t IMSCall::AnswerCall(int32_t videoState)
{
    return CarrierAnswerCall(videoState);
}

int32_t IMSCall::RejectCall()
{
    return CarrierRejectCall();
}

int32_t IMSCall::HangUpCall()
{
    return CarrierHangUpCall();
}

int32_t IMSCall::HoldCall()
{
    return CarrierHoldCall();
}

int32_t IMSCall::UnHoldCall()
{
    return CarrierUnHoldCall();
}

int32_t IMSCall::SwitchCall()
{
    return CarrierSwitchCall();
}

int32_t IMSCall::StartRtt(std::u16string &msg)
{
    CellularCallInfo callInfo;
    int32_t ret = PackCellularCallInfo(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("PackCellularCallInfo failed!");
        return ret;
    }
    ret = DelayedSingleton<CellularCallConnection>::GetInstance()->StartRtt(callInfo, msg);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StartRtt failed!");
        return CALL_ERR_STARTRTT_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::StopRtt()
{
    CellularCallInfo callInfo;
    int32_t ret = PackCellularCallInfo(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("PackCellularCallInfo failed!");
        return ret;
    }
    ret = DelayedSingleton<CellularCallConnection>::GetInstance()->StopRtt(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StopRtt failed!");
        return CALL_ERR_STOPRTT_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::SetMute(int32_t mute, int32_t slotId)
{
    return CarrierSetMute(mute, slotId);
}

void IMSCall::GetCallAttributeInfo(CallAttributeInfo &info)
{
    GetCallAttributeCarrierInfo(info);
}

int32_t IMSCall::CombineConference()
{
    int32_t ret = DelayedSingleton<ImsConference>::GetInstance()->SetMainCall(GetCallID());
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetMainCall failed,  error%{public}d", ret);
        return ret;
    }
    return CarrierCombineConference();
}

int32_t IMSCall::SeparateConference()
{
    return CarrierSeparateConference();
}

int32_t IMSCall::CanCombineConference()
{
    int32_t ret = IsSupportConferenceable();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("call unsupported conference,  error%{public}d", ret);
        return ret;
    }
    return DelayedSingleton<ImsConference>::GetInstance()->CanCombineConference();
}

int32_t IMSCall::CanSeparateConference()
{
    return DelayedSingleton<ImsConference>::GetInstance()->CanSeparateConference();
}

int32_t IMSCall::GetMainCallId(int32_t &mainCallId)
{
    mainCallId = DelayedSingleton<ImsConference>::GetInstance()->GetMainCall();
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::LaunchConference()
{
    int32_t ret = DelayedSingleton<ImsConference>::GetInstance()->JoinToConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
    }
    return ret;
}

int32_t IMSCall::ExitConference()
{
    return DelayedSingleton<ImsConference>::GetInstance()->LeaveFromConference(GetCallID());
}

int32_t IMSCall::HoldConference()
{
    int32_t ret = DelayedSingleton<ImsConference>::GetInstance()->HoldConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_HOLDING);
    }
    return ret;
}

int32_t IMSCall::GetSubCallIdList(std::vector<std::u16string> &callIdList)
{
    return DelayedSingleton<ImsConference>::GetInstance()->GetSubCallIdList(GetCallID(), callIdList);
}

int32_t IMSCall::GetCallIdListForConference(std::vector<std::u16string> &callIdList)
{
    return DelayedSingleton<ImsConference>::GetInstance()->GetCallIdListForConference(GetCallID(), callIdList);
}

int32_t IMSCall::IsSupportConferenceable()
{
#ifdef ABILIT_CONFIG_SUPPORT
    bool carrierSupport = GetCarrierConfig(IMS_SUPPORT_CONFERENCE);
    if (!carrierSupport) {
        return TELEPHONY_CONFERENCE_CARRIER_NOT_SUPPORT;
    }
    if (isVideoCall()) {
        carrierSupport = GetCarrierConfig(IMS_VIDEO_SUPPORT_CONFERENCE)
    }
    if (!carrierSupport) {
        return TELEPHONY_CONFERENCE_VIDEO_CALL_NOT_SUPPORT;
    }
#endif
    return CarrierCall::IsSupportConferenceable();
}

int32_t IMSCall::AcceptVideoCall()
{
    TELEPHONY_LOGI("always accept video call request.");
    return SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE);
}

int32_t IMSCall::RefuseVideoCall()
{
    TELEPHONY_LOGI("always refuse video call request.");
    return SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY);
}

int32_t IMSCall::SendUpdateCallMediaModeRequest(ImsCallMode mode)
{
    std::lock_guard<std::mutex> lock(videoUpdateMutex_);
    int32_t ret = InitVideoCall();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("video call initialize failed");
        return ret;
    }
    if (GetTelCallState() != TelCallState::CALL_STATUS_ACTIVE) {
        TELEPHONY_LOGE("call state is not active");
        return CALL_ERR_CALL_STATE;
    }
    if (videoCallState_ == nullptr) {
        TELEPHONY_LOGE("unexpected null pointer");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return videoCallState_->SendUpdateCallMediaModeRequest(mode);
}

int32_t IMSCall::SendUpdateCallMediaModeResponse(ImsCallMode mode)
{
    std::lock_guard<std::mutex> lock(videoUpdateMutex_);
    int32_t ret = InitVideoCall();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("video call initialize failed");
        return ret;
    }

    if (GetTelCallState() != TelCallState::CALL_STATUS_ACTIVE) {
        TELEPHONY_LOGE("call state is not active");
        return CALL_ERR_CALL_STATE;
    }
    return videoCallState_->SendUpdateCallMediaModeResponse(mode);
}

int32_t IMSCall::RecieveUpdateCallMediaModeRequest(ImsCallMode mode)
{
    std::lock_guard<std::mutex> lock(videoUpdateMutex_);
    int32_t ret = InitVideoCall();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("video call initialize failed");
        return ret;
    }
    if (GetTelCallState() != TelCallState::CALL_STATUS_ACTIVE) {
        TELEPHONY_LOGE("call state is not active");
        return CALL_ERR_CALL_STATE;
    }
    return videoCallState_->RecieveUpdateCallMediaModeRequest(mode);
}

int32_t IMSCall::ReceiveUpdateCallMediaModeResponse(CallMediaModeResponse &response)
{
    std::lock_guard<std::mutex> lock(videoUpdateMutex_);
    int32_t ret = InitVideoCall();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("video call initialize failed");
        return ret;
    }
    if (GetTelCallState() != TelCallState::CALL_STATUS_ACTIVE) {
        TELEPHONY_LOGE("call state is not active");
        return CALL_ERR_CALL_STATE;
    }
    if (response.result == TELEPHONY_SUCCESS) {
        return videoCallState_->ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE);
    }
    return videoCallState_->ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY);
}

void IMSCall::SwitchVideoState(ImsCallMode mode)
{
    // save old state.
    TELEPHONY_LOGI("SwitchVideoState call %{public}d switch to state %{public}d", GetCallID(), mode);
    if (videoStateMap_.find(mode) != videoStateMap_.end()) {
        videoCallState_ = videoStateMap_[mode];
    } else {
        videoCallState_ = videoStateMap_[ImsCallMode::CALL_MODE_AUDIO_ONLY];
    }
    return;
}

bool IMSCall::IsSupportVideoCall()
{
    bool isSupportVideoCall = true;
#ifdef ABILITY_CONFIG_SUPPORT
    isSupportVideoCall = GetCarrierConfig(ITEM_VIDEO_CALL);
#endif
    if (GetTelCallState() == TelCallState::CALL_STATUS_INCOMING) {
        TELEPHONY_LOGW("incoming call not support video upgrade");
        isSupportVideoCall = false;
    }
    if (GetEmergencyState()) {
        TELEPHONY_LOGW("emergency call not support video upgrade");
        isSupportVideoCall = false;
    }
    return isSupportVideoCall;
}

int32_t IMSCall::DispatchUpdateVideoRequest(ImsCallMode mode)
{
    CellularCallInfo callInfo;
    int32_t ret = PackCellularCallInfo(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("PackCellularCallInfo failed!");
        return ret;
    }
    ret = DelayedSingleton<CellularCallConnection>::GetInstance()->SendUpdateCallMediaModeRequest(callInfo, mode);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("send update media failed, errno:%{public}d!", ret);
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::DispatchUpdateVideoResponse(ImsCallMode mode)
{
    return TELEPHONY_SUCCESS;
}

sptr<VideoCallState> IMSCall::GetCallVideoState(ImsCallMode mode)
{
    TELEPHONY_LOGI("get call video state %{public}d", mode);
    if (videoStateMap_.find(mode) != videoStateMap_.end()) {
        return videoStateMap_[mode];
    } else {
        return nullptr;
    }
}
} // namespace Telephony
} // namespace OHOS
