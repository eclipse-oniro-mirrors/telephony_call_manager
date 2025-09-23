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

#ifndef IMS_CALL_H
#define IMS_CALL_H

#include "carrier_call.h"
#include "net_call_base.h"
#include "video_call_state.h"
#include "ffrt.h"

namespace OHOS {
namespace Telephony {
class IMSCall : public CarrierCall, public NetCallBase {
public:
    IMSCall(DialParaInfo &info);
    IMSCall(DialParaInfo &info, AppExecFwk::PacMap &extras);
    ~IMSCall();
    int32_t InitVideoCall() override;
    int32_t DialingProcess() override;
    int32_t AnswerCall(int32_t videoState) override;
    int32_t RejectCall() override;
    int32_t HangUpCall() override;
    int32_t HoldCall() override;
    int32_t UnHoldCall() override;
    int32_t SwitchCall() override;
    void GetCallAttributeInfo(CallAttributeInfo &info) override;
    int32_t CombineConference() override;
    void HandleCombineConferenceFailEvent() override;
    int32_t SeparateConference() override;
    int32_t KickOutFromConference() override;
    int32_t CanCombineConference() override;
    int32_t CanSeparateConference() override;
    int32_t CanKickOutFromConference() override;
    int32_t LaunchConference() override;
    int32_t ExitConference() override;
    int32_t HoldConference() override;
    int32_t GetMainCallId(int32_t &mainCallId) override;
    int32_t GetSubCallIdList(std::vector<std::u16string> &callIdList) override;
    int32_t GetCallIdListForConference(std::vector<std::u16string> &callIdList) override;
    int32_t IsSupportConferenceable() override;
    int32_t StartRtt(std::u16string &msg);
    int32_t StopRtt();
    int32_t SetMute(int32_t mute, int32_t slotId) override;
    int32_t UpdateImsCallMode(ImsCallMode mode) override;
    int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t RecieveUpdateCallMediaModeRequest(CallModeReportInfo &response) override;
    int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) override;
    int32_t ReceiveUpdateCallMediaModeResponse(CallModeReportInfo &response) override;
    int32_t ReportImsCallModeInfo(CallMediaModeInfo &imsCallModeInfo) override;
    void SwitchVideoState(ImsCallMode mode);
    bool IsSupportVideoCall();
    sptr<VideoCallState> GetCallVideoState(ImsCallMode mode);
    int32_t ControlCamera(std::string &cameraId, int32_t callingUid, int32_t callingPid) override;
    int32_t SetPreviewWindow(std::string &surfaceId, sptr<Surface> surface) override;
    int32_t SetDisplayWindow(std::string &surfaceId, sptr<Surface> surface) override;
    int32_t SetPausePicture(std::string &path) override;
    int32_t SetDeviceDirection(int32_t rotation) override;
    int32_t CancelCallUpgrade() override;
    int32_t RequestCameraCapabilities() override;
    bool IsVoiceModifyToVideo();

private:
    void AssignVideoCallState(VideoStateType videoStateType);

private:
    ffrt::mutex videoUpdateMutex_;
    sptr<VideoCallState> videoCallState_;
    bool isInitialized_;
    std::map<ImsCallMode, sptr<VideoCallState>> videoStateMap_;
};
} // namespace Telephony
} // namespace OHOS

#endif // IMS_CALL_H
