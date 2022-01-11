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
#include "ims_conference_base.h"

#include "cellular_call_connection.h"

namespace OHOS {
namespace Telephony {
IMSCall::IMSCall(DialParaInfo &info) : CarrierCall(info), videoUpgradeState_(VideoUpgradeState::VIDEO_UPGRADE_NONE)
{}

IMSCall::IMSCall(DialParaInfo &info, AppExecFwk::PacMap &extras) : CarrierCall(info, extras) {}

IMSCall::~IMSCall() {}

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

int32_t IMSCall::StartRtt()
{
    CellularCallInfo callInfo;
    std::u16string msg;
    int32_t ret = DelayedSingleton<CellularCallConnection>::GetInstance()->StartRtt(callInfo, msg);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StartRtt failed!");
        return CALL_ERR_STARTRTT_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::StopRtt()
{
    CellularCallInfo callInfo;
    int32_t ret = DelayedSingleton<CellularCallConnection>::GetInstance()->StopRtt(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("StopRtt failed!");
        return CALL_ERR_STOPRTT_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::SetMute(int32_t mute, int32_t slotId)
{
    return DelayedSingleton<CellularCallConnection>::GetInstance()->SetMute(mute, slotId);
}

void IMSCall::GetCallAttributeInfo(CallAttributeInfo &info)
{
    GetCallAttributeCarrierInfo(info);
}

int32_t IMSCall::CombineConference()
{
    int32_t ret = DelayedSingleton<ImsConferenceBase>::GetInstance()->SetMainCall(GetCallID());
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
        TELEPHONY_LOGE("call unsupport conference,  error%{public}d", ret);
        return ret;
    }
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->CanCombineConference();
}

int32_t IMSCall::CanSeparateConference()
{
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->CanSeparateConference();
}

int32_t IMSCall::GetMainCallId()
{
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->GetMainCall();
}

int32_t IMSCall::LaunchConference()
{
    int32_t ret = DelayedSingleton<ImsConferenceBase>::GetInstance()->JoinToConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
    }
    return ret;
}

int32_t IMSCall::ExitConference()
{
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->LeaveFromConference(GetCallID());
}

int32_t IMSCall::HoldConference()
{
    int32_t ret = DelayedSingleton<ImsConferenceBase>::GetInstance()->HoldConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_HOLDING);
    }
    return ret;
}

std::vector<std::u16string> IMSCall::GetSubCallIdList()
{
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->GetSubCallIdList(GetCallID());
}

std::vector<std::u16string> IMSCall::GetCallIdListForConference()
{
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->GetCallIdListForConference(GetCallID());
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

int32_t IMSCall::SendUpdateCallMediaModeRequest(CallMediaMode mode)
{
    /**
     * need handle request types
     * 1. video-call need set pause
     * 2. video-call dowgrade to audio
     * 3. upgrade to video, regard paused video  resume as upgrade.
     */
    TELEPHONY_LOGI("send update media mode request ,mode: %{public}d", mode);
    CallMediaMode curMode = GetCallMediaMode();
    if (mode == CallMediaMode::CALL_MODE_SEND_RECEIVE) {
        return SendUpgradeVideoRequest(mode);
    } else if (mode == CallMediaMode::CALL_MODE_AUDIO_ONLY) {
        if (curMode == CallMediaMode::CALL_MODE_AUDIO_ONLY) {
            TELEPHONY_LOGE("already in audio-only state,return error");
            return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
        }
        /*
         * no need response from the remote, directly send downgrade request to remote,set
         * media mode audo-only if current mode is SEND_RECIEVE
         */
        CellularCallInfo callInfo;
        int32_t ret = PackCellularCallInfo(callInfo);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGW("PackCellularCallInfo failed!");
        }
        ret = DelayedSingleton<CellularCallConnection>::GetInstance()->SendUpdateCallMediaModeRequest(
            callInfo, mode);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("send update media failed, errorId:%{public}d!", ret);
            return ret;
        }
        // notify control manager to close camara if it is open
        (void)NotifyCallMediaModeUpdate(VideoUpgradeState::VIDEO_UPGRADE_NONE);
        SetVideoUpdateState(VideoUpgradeState::VIDEO_UPGRADE_NONE);
        SetCallMediaMode(mode);
        return ret;
    } else {
        TELEPHONY_LOGE("unexpected media type");
        return CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
    }
}

int32_t IMSCall::RecieveUpdateCallMediaModeRequest(CallMediaMode mode)
{
    TELEPHONY_LOGI("receive update media mode request, mode:%{public}d", mode);
    if (mode == CallMediaMode::CALL_MODE_AUDIO_ONLY) {
        return HandleDowngradeVideoRequest(mode);
    } else if (mode == CallMediaMode::CALL_MODE_SEND_RECEIVE) {
        return HandleUpgradeVideoRequest(mode);
    } else {
        TELEPHONY_LOGE("unexpected media type");
        return CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
    }
}

int32_t IMSCall::SendUpdateCallMediaModeResponse(CallMediaMode mode)
{
    return TELEPHONY_SUCCESS;
}

/*
 * sceniro 1: recieve reject update from peer
 * sceniro 2: timeout
 * sceniro 3: recieve accept from peer
 */
int32_t IMSCall::ReceiveUpdateCallMediaModeResponse(CallMediaModeResponse &response)
{
    TELEPHONY_LOGI("receive update media mode response.");
    VideoUpgradeState state = GetVideoUpdateState();
    TELEPHONY_LOGI("callId %{public}d state:%{public}d", GetCallID(), state);
    int32_t ret = TELEPHONY_ERROR;
    if (state == VideoUpgradeState::VIDEO_UPGRADE_SEND_ONLY) {
        if (response.result == TELEPHONY_SUCCESS) {
            TELEPHONY_LOGI("remote accept. result %{public}d", response.result);
            // notify local to open camara
            ret = NotifyCallMediaModeUpdate(VideoUpgradeState::VIDEO_UPGRADE_SEND_RECV);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occured when notify video mode. %{public}d", ret);
                return ret;
            }
            // send confirm to remote
            int32_t ret = SendUpdateCallMediaModeResponse(CallMediaMode::CALL_MODE_SEND_RECEIVE);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occured ,errono:%{public}d", ret);
            }
            SetVideoUpdateState(VideoUpgradeState::VIDEO_UPGRADE_NONE);
            SetCallMediaMode(CallMediaMode::CALL_MODE_SEND_RECEIVE);
            return ret;
        }
        TELEPHONY_LOGW("remote refused. result %{public}d", response.result);
        SetVideoUpdateState(VideoUpgradeState::VIDEO_UPGRADE_NONE);
        return ret;
    } else if (state == VideoUpgradeState::VIDEO_UPGRADE_RECV_ONLY) {
        // remote recieved upgrade request. negotiation ended.
        (void)NotifyCallMediaModeUpdate(VideoUpgradeState::VIDEO_UPGRADE_SEND_RECV);
        SetVideoUpdateState(VideoUpgradeState::VIDEO_UPGRADE_NONE);
        return TELEPHONY_SUCCESS;
    } else {
        (void)NotifyCallMediaModeUpdate(VideoUpgradeState::VIDEO_UPGRADE_NONE);
        SetVideoUpdateState(VideoUpgradeState::VIDEO_UPGRADE_NONE);
        TELEPHONY_LOGE("illegal media update scenario.");
        return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
    }
}

int32_t IMSCall::HandleUpgradeVideoRequest(CallMediaMode mode)
{
    TELEPHONY_LOGI("handle upgrade video request.");
    if (mode != CallMediaMode::CALL_MODE_SEND_RECEIVE) {
        TELEPHONY_LOGE("invalid media type");
        return CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
    }
    // if  does not support then send response directly refuse it and keep current mode unchange
    if (!IsSupportVideoCall()) {
        TELEPHONY_LOGE("current call not support upgrade to video");
        return SendUpdateCallMediaModeResponse(CallMediaMode::CALL_MODE_AUDIO_ONLY);
    }
    if (GetCallMediaMode() != CallMediaMode::CALL_MODE_AUDIO_ONLY) {
        TELEPHONY_LOGE("illegal scenario, current media type is video");
        return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
    }
    // media update from audio to video
    VideoUpgradeState state = GetVideoUpdateState();
    if (state == VideoUpgradeState::VIDEO_UPGRADE_NONE) {
        // notify call control manager decide to accept/decline it
        int32_t ret = NotifyCallMediaModeUpdate(VideoUpgradeState::VIDEO_UPGRADE_RECV_ONLY);
        if (ret == TELEPHONY_SUCCESS) {
            SetVideoUpdateState(VideoUpgradeState::VIDEO_UPGRADE_RECV_ONLY);
        }
        return ret;
    } else {
        // video update state is recv_only or send_only
        TELEPHONY_LOGW("video update is in progress, please wait!");
        return CALL_ERR_VIDEO_IN_PROGRESS;
    }
}

int32_t IMSCall::HandleDowngradeVideoRequest(CallMediaMode mode)
{
    if (mode != CallMediaMode::CALL_MODE_AUDIO_ONLY) {
        TELEPHONY_LOGE("invalid media type");
        return CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
    }
    if (GetCallMediaMode() != CallMediaMode::CALL_MODE_SEND_RECEIVE) {
        TELEPHONY_LOGE("illegal scenario, current media type is not video");
        return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
    }
    // recieve downgrade to audio-only no need confirm, notify controlmanager close camara
    (void)NotifyCallMediaModeUpdate(VideoUpgradeState::VIDEO_UPGRADE_NONE);
    SetVideoUpdateState(VideoUpgradeState::VIDEO_UPGRADE_NONE);
    SetCallMediaMode(mode);
    return TELEPHONY_SUCCESS;
}

int32_t IMSCall::SendUpgradeVideoRequest(CallMediaMode mode)
{
    TELEPHONY_LOGI("begin send upgrade video request.");
    if (mode != CallMediaMode::CALL_MODE_SEND_RECEIVE) {
        TELEPHONY_LOGE("invailid media type");
        return CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
    }
    if (!IsSupportVideoCall()) {
        TELEPHONY_LOGE("not support");
        return CALL_ERR_VIDEO_ILLEGAL_CALL_TYPE;
    }
    // prerequisites check
    CallMediaMode curMode = GetCallMediaMode();
    if (curMode == CallMediaMode::CALL_MODE_SEND_RECEIVE) {
        TELEPHONY_LOGE("already in video mode, no need upgrade");
        return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
    }
    /**
     * if current upgrade state is RECEIVE_ONLY or SEND_ONLY, implys it has just sent upgrade video request to remote
     * or recieved request from remote, In this situation, to reduce complexities  of media update,
     * Continuously send  upgrade-to-video request is forbidden.
     */
    VideoUpgradeState state = GetVideoUpdateState();
    if (state == VideoUpgradeState::VIDEO_UPGRADE_SEND_ONLY || state == VideoUpgradeState::VIDEO_UPGRADE_RECV_ONLY) {
        TELEPHONY_LOGE("upgrade video in progress, no need send");
        return CALL_ERR_VIDEO_IN_PROGRESS;
    }
    CellularCallInfo callInfo;
    int32_t ret = PackCellularCallInfo(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("PackCellularCallInfo failed!");
    }
    SetVideoUpdateState(VideoUpgradeState::VIDEO_UPGRADE_SEND_ONLY);
    ret = DelayedSingleton<CellularCallConnection>::GetInstance()->SendUpdateCallMediaModeRequest(callInfo, mode);
    if (ret != TELEPHONY_SUCCESS) {
        SetVideoUpdateState(VideoUpgradeState::VIDEO_UPGRADE_NONE);
        TELEPHONY_LOGE("send update media failed, errno:%{public}d!", ret);
        return ret;
    }
    //  audio-only upgrade to video, we need set state send-only and wait for response from peer.
    TELEPHONY_LOGI("callId %{public}d", GetCallID());
    return ret;
}

int32_t IMSCall::NotifyCallMediaModeUpdate(VideoUpgradeState state)
{
    // notify call control manager
    CallEventInfo callEvent;
    if (DelayedSingleton<CallControlManager>::GetInstance()->NotifyCallEventUpdated(callEvent)) {
        return TELEPHONY_SUCCESS;
    } else {
        TELEPHONY_LOGE("video mode update notify failed");
        return CALL_ERR_VIDEO_MODE_CHANGE_NOTIFY_FAILED;
    }
}

IMSCall::VideoUpgradeState IMSCall::GetVideoUpdateState()
{
    std::lock_guard<std::mutex> lock(videoUpdateMutex_);
    return videoUpgradeState_;
}

void IMSCall::SetVideoUpdateState(VideoUpgradeState state)
{
    std::lock_guard<std::mutex> lock(videoUpdateMutex_);
    videoUpgradeState_ = state;
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
} // namespace Telephony
} // namespace OHOS
