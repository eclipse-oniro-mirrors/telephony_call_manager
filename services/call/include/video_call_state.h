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

#ifndef VIDEO_CALL_STATE_H
#define VIDEO_CALL_STATE_H

#include <map>
#include <memory>
#include <mutex>

#include "refbase.h"
#include "net_call_base.h"
#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
enum VideoUpdateStatus {
    STATUS_NONE,
    STATUS_SEND_REQUEST,
    STATUS_RECV_REQUEST,
};

class VideoCallState : public virtual RefBase {
public:
    VideoCallState(sptr<NetCallBase> callPtr);
    virtual ~VideoCallState() = default;
    bool IsCallSupportVideoCall();
    void SetVideoUpdateStatus(VideoUpdateStatus status);
    VideoUpdateStatus GetVideoUpdateStatus();
    int32_t SwitchCallVideoState(ImsCallMode mode);
    int32_t DispatchUpdateVideoRequest(ImsCallMode mode);
    int32_t DispatchUpdateVideoResponse(ImsCallMode mode);
    virtual int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) = 0;
    virtual int32_t RecieveUpdateCallMediaModeRequest(ImsCallMode mode) = 0;
    virtual int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) = 0;
    virtual int32_t ReceiveUpdateCallMediaModeResponse(ImsCallMode mode) = 0;
    sptr<VideoCallState> GetCallVideoState(ImsCallMode mode);

protected:
    sptr<NetCallBase> call_;
    VideoUpdateStatus updateStatus_;
};

class AudioOnlyState : public VideoCallState {
public:
    AudioOnlyState(sptr<NetCallBase> callPtr);
    ~AudioOnlyState() = default;
    int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t RecieveUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) override;
    int32_t ReceiveUpdateCallMediaModeResponse(ImsCallMode mode) override;
};

class VideoSendState : public VideoCallState {
public:
    VideoSendState(sptr<NetCallBase> callPtr);
    ~VideoSendState() = default;
    int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t RecieveUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) override;
    int32_t ReceiveUpdateCallMediaModeResponse(ImsCallMode mode) override;
};

class VideoReceiveState : public VideoCallState {
public:
    VideoReceiveState(sptr<NetCallBase> callPtr);
    ~VideoReceiveState() = default;
    int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t RecieveUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) override;
    int32_t ReceiveUpdateCallMediaModeResponse(ImsCallMode mode) override;
};

class VideoSendReceiveState : public VideoCallState {
public:
    VideoSendReceiveState(sptr<NetCallBase> callPtr);
    ~VideoSendReceiveState() = default;
    int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t RecieveUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) override;
    int32_t ReceiveUpdateCallMediaModeResponse(ImsCallMode mode) override;
};

class VideoPauseState : public VideoCallState {
public:
    VideoPauseState(sptr<NetCallBase> callPtr);
    ~VideoPauseState() = default;
    int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t RecieveUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) override;
    int32_t ReceiveUpdateCallMediaModeResponse(ImsCallMode mode) override;
};
} // namespace Telephony
} // namespace OHOS
#endif