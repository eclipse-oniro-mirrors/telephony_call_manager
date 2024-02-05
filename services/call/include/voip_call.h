/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef CALL_BASE_VOIP_CALL_OBJECT_H
#define CALL_BASE_VOIP_CALL_OBJECT_H

#include "carrier_call.h"
#include "voip_call_manager_info.h"

namespace OHOS {
namespace Telephony {
class VoIPCall : public CarrierCall {
public:
    VoIPCall(DialParaInfo &info);
    ~VoIPCall();
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
    int32_t SetMute(int32_t mute, int32_t slotId) override;
    int32_t HangUpCall(const ErrorReason &status);
    std::string GetVoipCallId();

protected:
    int32_t PackVoipCallInfo(VoipCallEventInfo &voipcallInfo);

private:
    std::string voipCallId_ = "";
    std::string userName_ = "";
    std::string userProfilePath_ = "";
    std::string packageProfilePath_ = "";
    std::string extensionId_ = "";
    std::string voipBundleName_ = "";
    std::string abilityName_ = "";
    std::vector<uint8_t> pixelMap_ = {};
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_BASE_VOIP_CALL_OBJECT_H
