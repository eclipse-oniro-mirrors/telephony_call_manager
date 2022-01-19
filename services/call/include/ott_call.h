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
    OTTCall(DialParaInfo &info);
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
    int32_t GetSlotId() override;
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
    int32_t SendUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t RecieveUpdateCallMediaModeRequest(ImsCallMode mode) override;
    int32_t SendUpdateCallMediaModeResponse(ImsCallMode mode) override;
    int32_t ReceiveUpdateCallMediaModeResponse(CallMediaModeResponse &reponse) override;
    int32_t DispatchUpdateVideoRequest(ImsCallMode mode) override;
    int32_t DispatchUpdateVideoResponse(ImsCallMode mode) override;

private:
    void PackOttCallRequestInfo(OttCallRequestInfo &requestInfo);

private:
    std::unique_ptr<OTTCallConnection> ottCallConnectionPtr_;
};
} // namespace Telephony
} // namespace OHOS

#endif // OTT_CALL_H
