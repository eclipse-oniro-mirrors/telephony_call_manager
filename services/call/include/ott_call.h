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

#ifndef OTT_CALL_H
#define OTT_CALL_H

#include "call_base.h"
#include "net_call_base.h"
#include "ott_call_connection.h"

namespace OHOS {
namespace Telephony {
class OTTCall : public CallBase, public NetCallBase {
public:
    explicit OTTCall(DialParaInfo &info);
    OTTCall(DialParaInfo &info, AppExecFwk::PacMap &extras);
    ~OTTCall();
    int32_t DialingProcess() override;
    int32_t AnswerCall(int32_t videoState) override;
    int32_t RejectCall() override;
    int32_t HangUpCall() override;
    int32_t HoldCall() override;
    int32_t UnHoldCall() override;
    int32_t SwitchCall() override;
    void GetCallAttributeInfo(CallAttributeInfo &info) override;
    bool GetEmergencyState() override;
    int32_t StartDtmf(char str) override;
    int32_t StopDtmf() override;
    int32_t PostDialProceed(bool proceed) override;
    int32_t GetSlotId() override;
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
    int32_t InitVideoCall() override;
    int32_t UpdateImsCallMode(ImsCallMode mode) override;
    int32_t ReportImsCallModeInfo(CallMediaModeInfo &response) override;
    int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t RecieveUpdateCallMediaModeRequest(CallModeReportInfo &imsCallModeInfo) override;
    int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) override;
    int32_t ReceiveUpdateCallMediaModeResponse(CallModeReportInfo &reponse) override;
    int32_t SetMute(int32_t mute, int32_t slotId) override;
    int32_t ControlCamera(std::string &cameraId, int32_t callingUid, int32_t callingPid) override;
    int32_t SetPreviewWindow(std::string &surfaceId, sptr<Surface> surface) override;
    int32_t SetDisplayWindow(std::string &surfaceId, sptr<Surface> surface) override;
    int32_t SetPausePicture(std::string &path) override;
    int32_t SetDeviceDirection(int32_t rotation) override;
    int32_t CancelCallUpgrade() override;
    int32_t RequestCameraCapabilities() override;

private:
    int32_t PackOttCallRequestInfo(OttCallRequestInfo &requestInfo);

private:
    std::unique_ptr<OTTCallConnection> ottCallConnectionPtr_;
};
} // namespace Telephony
} // namespace OHOS

#endif // OTT_CALL_H
