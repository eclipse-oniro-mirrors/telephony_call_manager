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

#ifndef NET_CALL_BASE_H
#define NET_CALL_BASE_H

#include <cstdio>
#include <cstdlib>
#include <stdint.h>

#include "refbase.h"
#include "call_manager_inner_type.h"
#include "surface.h"

namespace OHOS {
namespace Telephony {
class NetCallBase : public virtual RefBase {
public:
    NetCallBase();
    virtual ~NetCallBase() = default;

    int32_t StartConference();
    int32_t JoinConference();
    int32_t KickOutConference();
    int32_t LeaveConference();

    int32_t GetNetCallType();
    int32_t ChangeNetCallType();
    virtual int32_t InitVideoCall() = 0;
    virtual int32_t UpdateImsCallMode(ImsCallMode mode) = 0;
    virtual int32_t ReportImsCallModeInfo(CallMediaModeInfo &imsCallModeInfo) = 0;
    virtual int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) = 0;
    // receive call media mode modify request from peer
    virtual int32_t RecieveUpdateCallMediaModeRequest(CallModeReportInfo &response) = 0;
    // send call media mode modify response to peer
    virtual int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) = 0;
    // receive call media mode modify response to peer
    virtual int32_t ReceiveUpdateCallMediaModeResponse(CallModeReportInfo &response) = 0;
    virtual int32_t ControlCamera(std::string &cameraId, int32_t callingUid, int32_t callingPid) = 0;
    virtual int32_t SetPreviewWindow(std::string &surfaceId, sptr<Surface> surface) = 0;
    virtual int32_t SetDisplayWindow(std::string &surfaceId, sptr<Surface> surface) = 0;
    virtual int32_t SetPausePicture(std::string &path) = 0;
    virtual int32_t SetDeviceDirection(int32_t rotation) = 0;
    virtual int32_t CancelCallUpgrade() = 0;
    virtual int32_t RequestCameraCapabilities() = 0;
};
} // namespace Telephony
} // namespace OHOS

#endif // NET_CALL_BASE_H
