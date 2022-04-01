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

namespace OHOS {
namespace Telephony {
class IMSCall : public CarrierCall, public NetCallBase {
public:
    IMSCall(DialParaInfo &info);
    IMSCall(DialParaInfo &info, AppExecFwk::PacMap &extras);
    ~IMSCall();
    int32_t InitVideoCall();
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
    int32_t StartRtt(std::u16string &msg);
    int32_t StopRtt();
    int32_t SetMute(int32_t mute, int32_t slotId);
    int32_t AcceptVideoCall();
    int32_t RefuseVideoCall();
    int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t RecieveUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) override;
    int32_t ReceiveUpdateCallMediaModeResponse(CallMediaModeResponse &response) override;
    void SwitchVideoState(ImsCallMode mode);
    bool IsSupportVideoCall();
    int32_t DispatchUpdateVideoRequest(ImsCallMode mode) override;
    int32_t DispatchUpdateVideoResponse(ImsCallMode mode) override;
    sptr<VideoCallState> GetCallVideoState(ImsCallMode mode);

private:
    std::mutex videoUpdateMutex_;
    sptr<VideoCallState> videoCallState_;
    bool isInitialized_;
    std::map<ImsCallMode, sptr<VideoCallState>> videoStateMap_;
};
} // namespace Telephony
} // namespace OHOS

#endif // IMS_CALL_H
