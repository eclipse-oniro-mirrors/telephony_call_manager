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

namespace OHOS {
namespace Telephony {
class IMSCall : public CarrierCall, public NetCallBase {
public:
    enum VideoUpgradeState {
        VIDEO_UPGRADE_NONE = 0,
        VIDEO_UPGRADE_SEND_ONLY,
        VIDEO_UPGRADE_RECV_ONLY,
        VIDEO_UPGRADE_SEND_RECV,
    };

public:
    IMSCall(DialParaInfo &info);
    IMSCall(DialParaInfo &info, AppExecFwk::PacMap &extras);
    ~IMSCall();
    int32_t DialingProcess() override;
    int32_t AnswerCall(int32_t videoState) override;
    int32_t RejectCall() override;
    int32_t HangUpCall() override;
    int32_t HoldCall() override;
    int32_t UnHoldCall() override;
    int32_t SwitchCall() override;
    void GetCallAttributeInfo(CallAttributeInfo &info) override;
    int32_t CombineConference() override;
    int32_t SeparateConference() override;
    int32_t CanCombineConference() override;
    int32_t CanSeparateConference() override;
    int32_t LaunchConference() override;
    int32_t ExitConference() override;
    int32_t HoldConference() override;
    int32_t GetMainCallId() override;
    std::vector<std::u16string> GetSubCallIdList() override;
    std::vector<std::u16string> GetCallIdListForConference() override;
    int32_t IsSupportConferenceable() override;
    int32_t StartRtt();
    int32_t StopRtt();
    int32_t SetMute(int32_t mute, int32_t slotId);
    int32_t SendUpdateCallMediaModeRequest(CallMediaMode mode) override;
    int32_t RecieveUpdateCallMediaModeRequest(CallMediaMode mode) override;
    int32_t SendUpdateCallMediaModeResponse(CallMediaMode mode) override;
    int32_t ReceiveUpdateCallMediaModeResponse(CallMediaModeResponse &reponse) override;

protected:
    VideoUpgradeState GetVideoUpdateState();
    void SetVideoUpdateState(VideoUpgradeState state);
    int32_t HandleUpgradeVideoRequest(CallMediaMode mode);
    int32_t HandleDowngradeVideoRequest(CallMediaMode mode);
    int32_t SendUpgradeVideoRequest(CallMediaMode mode);
    int32_t NotifyCallMediaModeUpdate(VideoUpgradeState state);
    bool IsSupportVideoCall();

private:
    // video call
    VideoUpgradeState videoUpgradeState_;
    std::mutex videoUpdateMutex_;
};
} // namespace Telephony
} // namespace OHOS

#endif // IMS_CALL_H
