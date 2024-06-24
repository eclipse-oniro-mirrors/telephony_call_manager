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
#include "video_control_manager.h"

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
        VideoStateType videoStateType = GetVideoStateType();
        AssignVideoCallState(videoStateType);
        TELEPHONY_LOGI("video call initialize ok!");
        return TELEPHONY_SUCCESS;
    }
    sptr<VideoCallState> state = (std::make_unique<AudioOnlyState>(this)).release();
    if (state == nullptr) {
        TELEPHONY_LOGE("null pointer");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    videoStateMap_[ImsCallMode::CALL_MODE_AUDIO_ONLY] = state;
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
    VideoStateType videoStateType = GetVideoStateType();
    AssignVideoCallState(videoStateType);
    isInitialized_ = true;
    return TELEPHONY_SUCCESS;
}

void IMSCall::AssignVideoCallState(VideoStateType videoStateType)
{
    switch (videoStateType) {
        case VideoStateType::TYPE_VOICE:
            videoCallState_ = videoStateMap_[ImsCallMode::CALL_MODE_AUDIO_ONLY];
            break;
        case VideoStateType::TYPE_SEND_ONLY:
            videoCallState_ = videoStateMap_[ImsCallMode::CALL_MODE_SEND_ONLY];
            break;
        case VideoStateType::TYPE_RECEIVE_ONLY:
            videoCallState_ = videoStateMap_[ImsCallMode::CALL_MODE_RECEIVE_ONLY];
            break;
        case VideoStateType::TYPE_VIDEO:
            videoCallState_ = videoStateMap_[ImsCallMode::CALL_MODE_SEND_RECEIVE];
            break;
        default:
            videoCallState_ = videoStateMap_[ImsCallMode::CALL_MODE_AUDIO_ONLY];
            break;
    }
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
    ConferenceState currentState = DelayedSingleton<ImsConference>::GetInstance()->GetConferenceState();
    if (currentState == ConferenceState::CONFERENCE_STATE_CREATING) {
        TELEPHONY_LOGE("skip combine, a process of combine already exsists");
        return TELEPHONY_SUCCESS;
    }
    DelayedSingleton<ImsConference>::GetInstance()->SetConferenceState(ConferenceState::CONFERENCE_STATE_CREATING);
    return CarrierCombineConference();
}

void IMSCall::HandleCombineConferenceFailEvent()
{
    std::set<std::int32_t> subCallIdList = DelayedSingleton<ImsConference>::GetInstance()->GetSubCallIdList();
    if (subCallIdList.empty()) {
        DelayedSingleton<ImsConference>::GetInstance()->SetMainCall(ERR_ID);
    } else {
        DelayedSingleton<ImsConference>::GetInstance()->SetMainCall(*subCallIdList.begin());
    }
    ConferenceState oldState = DelayedSingleton<ImsConference>::GetInstance()->GetOldConferenceState();
    DelayedSingleton<ImsConference>::GetInstance()->SetConferenceState(oldState);
}

int32_t IMSCall::SeparateConference()
{
    return CarrierSeparateConference();
}

int32_t IMSCall::KickOutFromConference()
{
    return CarrierKickOutFromConference();
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

int32_t IMSCall::CanKickOutFromConference()
{
    return DelayedSingleton<ImsConference>::GetInstance()->CanKickOutFromConference();
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
    int32_t ret = DelayedSingleton<ImsConference>::GetInstance()->LeaveFromConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_IDLE);
    }
    return ret;
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

int32_t IMSCall::UpdateImsCallMode(ImsCallMode mode)
{
    std::lock_guard<std::mutex> lock(videoUpdateMutex_);
    int32_t ret = TELEPHONY_SUCCESS;
    if (GetTelCallState() != TelCallState::CALL_STATUS_ACTIVE) {
        TELEPHONY_LOGE("call state is not active");
        return CALL_ERR_CALL_STATE;
    }
    if (videoCallState_ == nullptr) {
        TELEPHONY_LOGE("unexpected null pointer");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    VideoUpdateStatus status = videoCallState_->GetVideoUpdateStatus();
    if (VideoUpdateStatus::STATUS_NONE == status) {
        ret = videoCallState_->SendUpdateCallMediaModeRequest(mode);
    } else if (VideoUpdateStatus::STATUS_RECV_REQUEST == status) {
        ret = videoCallState_->SendUpdateCallMediaModeResponse(mode);
    }
    return ret;
}

int32_t IMSCall::ReportImsCallModeInfo(CallMediaModeInfo &imsCallModeInfo)
{
    TELEPHONY_LOGI("callMode:%{public}d", imsCallModeInfo.callMode);
    return DelayedSingleton<VideoControlManager>::GetInstance()->ReportImsCallModeInfo(imsCallModeInfo);
}

int32_t IMSCall::SendUpdateCallMediaModeRequest(ImsCallMode mode)
{
    CellularCallInfo callInfo;
    int32_t ret = PackCellularCallInfo(callInfo);
    TELEPHONY_LOGI("SendUpdateCallMediaModeRequest callMode:%{public}d", mode);
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

int32_t IMSCall::SendUpdateCallMediaModeResponse(ImsCallMode mode)
{
    CellularCallInfo callInfo;
    int32_t ret = PackCellularCallInfo(callInfo);
    TELEPHONY_LOGI("SendUpdateCallMediaModeResponse callMode:%{public}d", mode);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("PackCellularCallInfo failed!");
        return ret;
    }
    ret = DelayedSingleton<CellularCallConnection>::GetInstance()->SendUpdateCallMediaModeResponse(callInfo, mode);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("send update media failed, errno:%{public}d!", ret);
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::RecieveUpdateCallMediaModeRequest(CallModeReportInfo &response)
{
    std::lock_guard<std::mutex> lock(videoUpdateMutex_);
    if (videoCallState_ == nullptr) {
        TELEPHONY_LOGE("unexpected null pointer");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    CallMediaModeInfo callModeInfo;
    callModeInfo.callId = GetCallID();
    callModeInfo.result = response.result;
    callModeInfo.callMode = response.callMode;
    callModeInfo.isRequestInfo = true;
    TELEPHONY_LOGI("RecieveUpdateCallMediaModeRequest callMode:%{public}d", callModeInfo.callMode);
    return videoCallState_->RecieveUpdateCallMediaModeRequest(callModeInfo);
}

int32_t IMSCall::ReceiveUpdateCallMediaModeResponse(CallModeReportInfo &response)
{
    std::lock_guard<std::mutex> lock(videoUpdateMutex_);
    if (videoCallState_ == nullptr) {
        TELEPHONY_LOGE("unexpected null pointer");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    CallMediaModeInfo callModeInfo;
    callModeInfo.callId = GetCallID();
    callModeInfo.result = response.result;
    callModeInfo.callMode = response.callMode;
    callModeInfo.isRequestInfo = false;
    TELEPHONY_LOGI("ReceiveUpdateCallMediaModeResponse callMode:%{public}d", callModeInfo.callMode);
    if (response.result == VideoRequestResultType::TYPE_REQUEST_SUCCESS) {
        return videoCallState_->ReceiveUpdateCallMediaModeResponse(callModeInfo);
    }
    callModeInfo.callMode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    return videoCallState_->ReceiveUpdateCallMediaModeResponse(callModeInfo);
}

int32_t IMSCall::ControlCamera(std::string &cameraId, int32_t callingUid, int32_t callingPid)
{
    TELEPHONY_LOGI("cameraId:%{public}s", cameraId.c_str());
    int32_t ret = DelayedSingleton<CellularCallConnection>::GetInstance()->ControlCamera(
        GetSlotId(), GetCallIndex(), cameraId, callingUid, callingPid);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CancelCallUpgrade failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::SetPreviewWindow(std::string &surfaceId, sptr<Surface> surface)
{
    TELEPHONY_LOGI("surfaceId:%{public}s", surfaceId.c_str());
    int32_t ret = DelayedSingleton<CellularCallConnection>::GetInstance()->SetPreviewWindow(
        GetSlotId(), GetCallIndex(), surfaceId, surface);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CancelCallUpgrade failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::SetDisplayWindow(std::string &surfaceId, sptr<Surface> surface)
{
    TELEPHONY_LOGI("surfaceId:%{public}s", surfaceId.c_str());
    int32_t ret = DelayedSingleton<CellularCallConnection>::GetInstance()->SetDisplayWindow(
        GetSlotId(), GetCallIndex(), surfaceId, surface);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CancelCallUpgrade failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::SetPausePicture(std::string &path)
{
    int32_t ret = DelayedSingleton<CellularCallConnection>::GetInstance()->SetPausePicture(
        GetSlotId(), GetCallIndex(), path);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CancelCallUpgrade failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::SetDeviceDirection(int32_t rotation)
{
    int32_t ret = DelayedSingleton<CellularCallConnection>::GetInstance()->SetDeviceDirection(
        GetSlotId(), GetCallIndex(), rotation);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CancelCallUpgrade failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::RequestCameraCapabilities()
{
    int32_t ret = DelayedSingleton<CellularCallConnection>::GetInstance()->RequestCameraCapabilities(
        GetSlotId(), GetCallIndex());
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CancelCallUpgrade failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::CancelCallUpgrade()
{
    int32_t ret = DelayedSingleton<CellularCallConnection>::GetInstance()->CancelCallUpgrade(
        GetSlotId(), GetCallIndex());
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CancelCallUpgrade failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
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

sptr<VideoCallState> IMSCall::GetCallVideoState(ImsCallMode mode)
{
    TELEPHONY_LOGI("get call video state %{public}d", mode);
    if (videoStateMap_.find(mode) != videoStateMap_.end()) {
        return videoStateMap_[mode];
    } else {
        return nullptr;
    }
}

bool IMSCall::IsVoiceModifyToVideo()
{
    if (videoCallState_ == nullptr) {
        return false;
    }
    switch (videoCallState_->GetVideoUpdateStatus()) {
        case VideoUpdateStatus::STATUS_RECV_REQUEST:
        case VideoUpdateStatus::STATUS_SEND_REQUEST:
            return true;
            break;
        default:
            return false;
            break;
    }
}
} // namespace Telephony
} // namespace OHOS
