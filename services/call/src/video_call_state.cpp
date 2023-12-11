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

#include "video_call_state.h"

#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "call_manager_errors.h"
#include "ims_call.h"

namespace OHOS {
namespace Telephony {
VideoCallState::VideoCallState(sptr<NetCallBase> callPtr)
    : call_(callPtr), updateStatus_(VideoUpdateStatus::STATUS_NONE)
{}

bool VideoCallState::IsCallSupportVideoCall()
{
    if (call_ == nullptr) {
        TELEPHONY_LOGE("unexpected null pointer.");
        return false;
    }
    sptr<IMSCall> netCall = static_cast<IMSCall *>(call_.GetRefPtr());
    return netCall->IsSupportVideoCall();
}

void VideoCallState::SetVideoUpdateStatus(VideoUpdateStatus status)
{
    updateStatus_ = status;
}

VideoUpdateStatus VideoCallState::GetVideoUpdateStatus()
{
    return updateStatus_;
}

int32_t VideoCallState::SwitchCallVideoState(ImsCallMode mode)
{
    if (call_ == nullptr) {
        TELEPHONY_LOGE("unexpected null pointer.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    sptr<IMSCall> netCall = static_cast<IMSCall *>(call_.GetRefPtr());
    netCall->SwitchVideoState(mode);
    return TELEPHONY_SUCCESS;
}

int32_t VideoCallState::DispatchUpdateVideoRequest(ImsCallMode mode)
{
    if (call_ == nullptr) {
        TELEPHONY_LOGE("unexpected null pointer.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return call_->SendUpdateCallMediaModeRequest(mode);
}

int32_t VideoCallState::DispatchUpdateVideoResponse(ImsCallMode mode)
{
    if (call_ == nullptr) {
        TELEPHONY_LOGE("unexpected null pointer.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return call_->SendUpdateCallMediaModeResponse(mode);
}

int32_t VideoCallState::DispatchReportVideoCallInfo(CallMediaModeInfo &imsCallModeInfo)
{
    if (call_ == nullptr) {
        TELEPHONY_LOGE("unexpected null pointer.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return call_->ReportImsCallModeInfo(imsCallModeInfo);
}

sptr<VideoCallState> VideoCallState::GetCallVideoState(ImsCallMode mode)
{
    if (call_ == nullptr) {
        TELEPHONY_LOGE("unexpected null pointer.");
        return nullptr;
    }
    sptr<IMSCall> netCall = static_cast<IMSCall *>(call_.GetRefPtr());
    return netCall->GetCallVideoState(mode);
}

AudioOnlyState::AudioOnlyState(sptr<NetCallBase> callPtr) : VideoCallState(callPtr) {}

int32_t AudioOnlyState::SendUpdateCallMediaModeRequest(ImsCallMode mode)
{
    int ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI("AudioOnlyState send update video request. mode:%{public}d, status %{public}d", mode, status);
    switch (mode) {
        case ImsCallMode::CALL_MODE_SEND_ONLY:
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            break;
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            TELEPHONY_LOGE("illegal media type.");
            ret = CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            break;
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            if (!IsCallSupportVideoCall()) {
                TELEPHONY_LOGE("call not support video, end.");
                return CALL_ERR_VIDEO_NOT_SUPPORTED;
            }
            if (status != VideoUpdateStatus::STATUS_NONE) {
                TELEPHONY_LOGE("update is in progress. no need");
                return CALL_ERR_VIDEO_IN_PROGRESS;
            }
            ret = DispatchUpdateVideoRequest(mode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occur when send updateRequest to call");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_SEND_REQUEST);
            break;
        default:
            TELEPHONY_LOGE("unknown call media type.");
            return CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
    }
    return ret;
}

int32_t AudioOnlyState::RecieveUpdateCallMediaModeRequest(CallMediaModeInfo &imsCallModeInfo)
{
    int32_t ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI("AudioOnlyState receive update video request. mode:%{public}d, status %{public}d",
        imsCallModeInfo.callMode, status);
    switch (imsCallModeInfo.callMode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            TELEPHONY_LOGI("invalid media state request.");
            ret = CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            break;
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            if (status != VideoUpdateStatus::STATUS_NONE) {
                TELEPHONY_LOGE("video update in progress");
                return CALL_ERR_VIDEO_IN_PROGRESS;
            }
            // notify app to accept or refuse, assume always accept here
            (void)DispatchReportVideoCallInfo(imsCallModeInfo);
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_RECV_REQUEST);
            break;
        case ImsCallMode::CALL_MODE_SEND_ONLY:
            TELEPHONY_LOGI("receive request.");
            break;
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            if (!IsCallSupportVideoCall()) {
                TELEPHONY_LOGE("not support video, refuse");
                (void)DispatchUpdateVideoResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY);
                return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            }
            if (status != VideoUpdateStatus::STATUS_NONE) {
                TELEPHONY_LOGE("video update in progress");
                return CALL_ERR_VIDEO_IN_PROGRESS;
            }
            // notify app to accept or refuse, assume always accept here
            (void)DispatchReportVideoCallInfo(imsCallModeInfo);
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_RECV_REQUEST);
            break;
        default:
            TELEPHONY_LOGE("unknown call media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t AudioOnlyState::SendUpdateCallMediaModeResponse(ImsCallMode mode)
{
    int32_t ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI("AudioOnlyState send update video response. mode:%{public}d, status %{public}d", mode, status);
    switch (mode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
            if (!IsCallSupportVideoCall()) {
                TELEPHONY_LOGE("not support video, refuse");
                (void)DispatchUpdateVideoResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY);
                return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            }
            if (status != VideoUpdateStatus::STATUS_RECV_REQUEST) {
                TELEPHONY_LOGE("video update in progress");
                return CALL_ERR_VIDEO_IN_PROGRESS;
            }
            // notify app to accept or refuse, assume always accept here
            (void)DispatchUpdateVideoResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY);
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            TELEPHONY_LOGI("invalid media state request.");
            ret = CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            break;
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
        case ImsCallMode::CALL_MODE_SEND_ONLY:
            TELEPHONY_LOGI("receive request.");
            break;
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            if (!IsCallSupportVideoCall()) {
                TELEPHONY_LOGE("not support video, refuse");
                (void)DispatchUpdateVideoResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY);
                return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            }
            if (status != VideoUpdateStatus::STATUS_RECV_REQUEST) {
                TELEPHONY_LOGE("video update in progress");
                return CALL_ERR_VIDEO_IN_PROGRESS;
            }
            // notify app to accept or refuse, assume always accept here
            (void)DispatchUpdateVideoResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE);
            (void)SwitchCallVideoState(ImsCallMode::CALL_MODE_SEND_RECEIVE);
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        default:
            TELEPHONY_LOGE("unknown call media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t AudioOnlyState::ReceiveUpdateCallMediaModeResponse(CallMediaModeInfo &imsCallModeInfo)
{
    int32_t ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI(
        "AudioOnlyState receive response, mode %{public}d, status %{public}d", imsCallModeInfo.callMode, status);
    switch (imsCallModeInfo.callMode) {
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            if (status != VideoUpdateStatus::STATUS_SEND_REQUEST) {
                return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            }
            (void)SwitchCallVideoState(ImsCallMode::CALL_MODE_SEND_RECEIVE);
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            // notify app to accept or refuse, assume always accept here
            DispatchReportVideoCallInfo(imsCallModeInfo);
            break;
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            // notify app to accept or refuse, assume always accept here
            DispatchReportVideoCallInfo(imsCallModeInfo);
            break;
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            if (status != VideoUpdateStatus::STATUS_SEND_REQUEST) {
                return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            }
            (void)SwitchCallVideoState(ImsCallMode::CALL_MODE_RECEIVE_ONLY);
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            // notify app to accept or refuse, assume always accept here
            DispatchReportVideoCallInfo(imsCallModeInfo);
            break;
        default:
            TELEPHONY_LOGI("other media type, ignored");
            break;
    }
    return ret;
}

VideoSendState::VideoSendState(sptr<NetCallBase> callPtr) : VideoCallState(callPtr) {}

int32_t VideoSendState::SendUpdateCallMediaModeRequest(ImsCallMode mode)
{
    TELEPHONY_LOGI("VideoSendState send update video request. mode:%{public}d", mode);
    int32_t ret = TELEPHONY_SUCCESS;
    switch (mode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            ret = DispatchUpdateVideoRequest(mode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when dispatch request");
                return ret;
            }
            ret = SwitchCallVideoState(mode); // switch call video state
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when switch call state");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
        case ImsCallMode::CALL_MODE_SEND_ONLY:
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            TELEPHONY_LOGE("already in progress.");
            ret = CALL_ERR_VIDEO_IN_PROGRESS;
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t VideoSendState::RecieveUpdateCallMediaModeRequest(CallMediaModeInfo &imsCallModeInfo)
{
    int32_t ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI("VideoSendState receive update video request. mode:%{public}d, status:%{public}d",
        imsCallModeInfo.callMode, status);
    switch (imsCallModeInfo.callMode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            ret = SwitchCallVideoState(imsCallModeInfo.callMode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when switch call state.");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            TELEPHONY_LOGE("invalid media state request.");
            ret = CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            break;
        case ImsCallMode::CALL_MODE_SEND_ONLY:
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            if (status != VideoUpdateStatus::STATUS_NONE) {
                TELEPHONY_LOGE("already in progress.");
                ret = CALL_ERR_VIDEO_IN_PROGRESS;
            }
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t VideoSendState::SendUpdateCallMediaModeResponse(ImsCallMode mode)
{
    TELEPHONY_LOGI("VideoSendState send response ignored. mode %{public}d", mode);
    return TELEPHONY_SUCCESS;
}

int32_t VideoSendState::ReceiveUpdateCallMediaModeResponse(CallMediaModeInfo &imsCallModeInfo)
{
    int32_t ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI("VideoSendState receive update video response. mode:%{public}d, status:%{public}d",
        imsCallModeInfo.callMode, status);
    switch (imsCallModeInfo.callMode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            ret = SwitchCallVideoState(imsCallModeInfo.callMode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when switch call state");
                return ret;
            }
            // set previous state none
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_SEND_ONLY:
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            if (status != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("already in progress.");
                ret = CALL_ERR_VIDEO_IN_PROGRESS;
            }
            break;
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            ret = DispatchUpdateVideoResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE);
            if (ret != TELEPHONY_SUCCESS) {
                return ret;
            }
            ret = SwitchCallVideoState(imsCallModeInfo.callMode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when switch call");
                return ret;
            }
            GetCallVideoState(ImsCallMode::CALL_MODE_AUDIO_ONLY)->SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

VideoReceiveState::VideoReceiveState(sptr<NetCallBase> callPtr) : VideoCallState(callPtr) {}

int32_t VideoReceiveState::SendUpdateCallMediaModeRequest(ImsCallMode mode)
{
    int32_t ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI("VideoReceiveState send update video request. mode:%{public}d, status:%{public}d", mode, status);
    switch (mode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            ret = DispatchUpdateVideoRequest(mode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when dispatch request");
                return ret;
            }
            ret = SwitchCallVideoState(mode);
            if (ret) {
                TELEPHONY_LOGE("error occur when switch call state");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_SEND_ONLY:
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            if (status != VideoUpdateStatus::STATUS_NONE) {
                TELEPHONY_LOGE("already in progress.");
                ret = CALL_ERR_VIDEO_IN_PROGRESS;
            }
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t VideoReceiveState::RecieveUpdateCallMediaModeRequest(CallMediaModeInfo &imsCallModeInfo)
{
    int32_t ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI("VideoReceiveState receive update video request. mode:%{public}d, status:%{public}d",
        imsCallModeInfo.callMode, status);
    switch (imsCallModeInfo.callMode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            ret = SwitchCallVideoState(imsCallModeInfo.callMode);
            if (ret != TELEPHONY_SUCCESS) {
                return ret;
            }
            DispatchReportVideoCallInfo(imsCallModeInfo);
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_SEND_ONLY:
            break;
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            if (status != VideoUpdateStatus::STATUS_NONE) {
                TELEPHONY_LOGE("already in progress.");
                ret = CALL_ERR_VIDEO_IN_PROGRESS;
            }
            DispatchReportVideoCallInfo(imsCallModeInfo);
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_RECV_REQUEST);
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t VideoReceiveState::SendUpdateCallMediaModeResponse(ImsCallMode mode)
{
    int32_t ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI("VideoReceiveState send update video response. mode:%{public}d, status:%{public}d", mode, status);
    switch (mode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            ret = SwitchCallVideoState(mode);
            if (ret == TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occur when switch call state");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_SEND_ONLY:
            break;
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            if (status != VideoUpdateStatus::STATUS_RECV_REQUEST) {
                return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            }
            ret = DispatchUpdateVideoRequest(mode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when dispatch request");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            if (status != VideoUpdateStatus::STATUS_RECV_REQUEST) {
                return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            }
            ret = DispatchUpdateVideoRequest(mode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when dispatch request");
                return ret;
            }
            ret = SwitchCallVideoState(mode);
            if (ret) {
                TELEPHONY_LOGE("error occur when switch call state");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t VideoReceiveState::ReceiveUpdateCallMediaModeResponse(CallMediaModeInfo &imsCallModeInfo)
{
    TELEPHONY_LOGI("VideoReceiveState receive update video response. mode:%{public}d", imsCallModeInfo.callMode);
    int32_t ret = TELEPHONY_SUCCESS;
    switch (imsCallModeInfo.callMode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            ret = SwitchCallVideoState(imsCallModeInfo.callMode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occur when switch call state");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_SEND_ONLY:
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            ret = DispatchReportVideoCallInfo(imsCallModeInfo);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occur when switch call state");
                return ret;
            }
            if (GetVideoUpdateStatus() != VideoUpdateStatus::STATUS_NONE) {
                SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            }
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

VideoSendReceiveState::VideoSendReceiveState(sptr<NetCallBase> callPtr) : VideoCallState(callPtr) {}

int32_t VideoSendReceiveState::SendUpdateCallMediaModeRequest(ImsCallMode mode)
{
    TELEPHONY_LOGI("VideoSendReceiveState send update video request. mode:%{public}d", mode);
    int32_t ret = TELEPHONY_SUCCESS;
    switch (mode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            ret = DispatchUpdateVideoRequest(mode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occur when send updateRequest to call");
                return ret;
            }
            ret = SwitchCallVideoState(mode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occur when switch call state");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_SEND_ONLY:
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            ret = DispatchUpdateVideoRequest(mode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occur when send updateRequest to call");
                return ret;
            }
            break;
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            TELEPHONY_LOGE("illegal media type.");
            return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t VideoSendReceiveState::RecieveUpdateCallMediaModeRequest(CallMediaModeInfo &imsCallModeInfo)
{
    int32_t ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI("VideoSendReceiveState receive update video request. mode:%{public}d, status:%{public}d",
        imsCallModeInfo.callMode, status);
    switch (imsCallModeInfo.callMode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
            ret = SwitchCallVideoState(imsCallModeInfo.callMode); // support send downgrade & pause video request
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when switch call state");
                return ret;
            }
            (void)DispatchReportVideoCallInfo(imsCallModeInfo);
            if (status != VideoUpdateStatus::STATUS_NONE) {
                SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            }
            break;
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
        case ImsCallMode::CALL_MODE_SEND_ONLY:
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            TELEPHONY_LOGI("receive update video request");
            break;
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            (void)DispatchReportVideoCallInfo(imsCallModeInfo);
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_RECV_REQUEST);
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t VideoSendReceiveState::SendUpdateCallMediaModeResponse(ImsCallMode mode)
{
    TELEPHONY_LOGI("VideoSendReceiveState ignore response");
    return TELEPHONY_SUCCESS;
}

int32_t VideoSendReceiveState::ReceiveUpdateCallMediaModeResponse(CallMediaModeInfo &imsCallModeInfo)
{
    int32_t ret = TELEPHONY_SUCCESS;
    VideoUpdateStatus status = GetVideoUpdateStatus();
    TELEPHONY_LOGI("VideoSendReceiveState receive update video response. mode:%{public}d, status:%{public}d",
        imsCallModeInfo.callMode, status);
    switch (imsCallModeInfo.callMode) {
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            TELEPHONY_LOGI("receive update video request");
            if (status != VideoUpdateStatus::STATUS_RECV_REQUEST) {
                return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            }
            ret = DispatchReportVideoCallInfo(imsCallModeInfo);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when switch call state");
                return ret;
            }
            ret = SwitchCallVideoState(imsCallModeInfo.callMode); // support send downgrade & pause video request
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occurs when switch call state");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            if (status != VideoUpdateStatus::STATUS_RECV_REQUEST) {
                return CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            }
            ret = DispatchReportVideoCallInfo(imsCallModeInfo);
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

VideoPauseState::VideoPauseState(sptr<NetCallBase> callPtr) : VideoCallState(callPtr) {}

int32_t VideoPauseState::SendUpdateCallMediaModeRequest(ImsCallMode mode)
{
    TELEPHONY_LOGI("VideoPauseState send update video request. mode:%{public}d", mode);
    int32_t ret = TELEPHONY_SUCCESS;
    switch (mode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            ret = DispatchUpdateVideoRequest(mode);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occur when send updateRequest to call");
                return ret;
            }
            ret = SwitchCallVideoState(mode);
            if (ret == TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occur when switch call state");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_SEND_ONLY:
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            TELEPHONY_LOGI("send update video state");
            ret = DispatchUpdateVideoRequest(mode);
            break;
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            TELEPHONY_LOGE("illegal media type.");
            ret = CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t VideoPauseState::RecieveUpdateCallMediaModeRequest(CallMediaModeInfo &imsCallModeInfo)
{
    TELEPHONY_LOGI("VideoPauseState receive update video request. mode:%{public}d", imsCallModeInfo.callMode);
    int32_t ret = TELEPHONY_SUCCESS;
    switch (imsCallModeInfo.callMode) {
        case ImsCallMode::CALL_MODE_AUDIO_ONLY:
        case ImsCallMode::CALL_MODE_SEND_RECEIVE:
            ret = SwitchCallVideoState(imsCallModeInfo.callMode);
            if (ret == TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("error occur when switch call state");
                return ret;
            }
            SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
            break;
        case ImsCallMode::CALL_MODE_SEND_ONLY:
        case ImsCallMode::CALL_MODE_RECEIVE_ONLY:
            TELEPHONY_LOGI("receive update call media request");
            break;
        case ImsCallMode::CALL_MODE_VIDEO_PAUSED:
            TELEPHONY_LOGE("illegal media type.");
            ret = CALL_ERR_VIDEO_ILLEAGAL_SCENARIO;
            break;
        default:
            TELEPHONY_LOGE("unknown media type.");
            ret = CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE;
            break;
    }
    return ret;
}

int32_t VideoPauseState::SendUpdateCallMediaModeResponse(ImsCallMode mode)
{
    TELEPHONY_LOGI("VideoPauseState send update video response. mode:%{public}d", mode);
    return TELEPHONY_SUCCESS;
}

int32_t VideoPauseState::ReceiveUpdateCallMediaModeResponse(CallMediaModeInfo &imsCallModeInfo)
{
    TELEPHONY_LOGI("VideoPauseState receive update video response. mode:%{public}d", imsCallModeInfo.callMode);
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
