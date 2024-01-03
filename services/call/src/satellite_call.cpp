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

#include "satellite_call.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
SatelliteCall::SatelliteCall(DialParaInfo &info) : CarrierCall(info) {}

SatelliteCall::SatelliteCall(DialParaInfo &info, AppExecFwk::PacMap &extras) : CarrierCall(info, extras) {}

SatelliteCall::~SatelliteCall() {}

int32_t SatelliteCall::DialingProcess()
{
    return CarrierDialingProcess();
}

int32_t SatelliteCall::AnswerCall(int32_t videoState)
{
    return CarrierAnswerCall(videoState);
}

int32_t SatelliteCall::RejectCall()
{
    return CarrierRejectCall();
}

int32_t SatelliteCall::HangUpCall()
{
    return CarrierHangUpCall();
}

int32_t SatelliteCall::HoldCall()
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::UnHoldCall()
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::SwitchCall()
{
    return TELEPHONY_ERROR;
}

void SatelliteCall::GetCallAttributeInfo(CallAttributeInfo &info)
{
    GetCallAttributeCarrierInfo(info);
}

int32_t SatelliteCall::SetMute(int32_t mute, int32_t slotId)
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::CombineConference()
{
    return TELEPHONY_ERROR;
}

void SatelliteCall::HandleCombineConferenceFailEvent() {}

int32_t SatelliteCall::SeparateConference()
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::KickOutFromConference()
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::CanCombineConference()
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::CanSeparateConference()
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::CanKickOutFromConference()
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::LaunchConference()
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::ExitConference()
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::HoldConference()
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::GetMainCallId(int32_t &mainCallId)
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::GetSubCallIdList(std::vector<std::u16string> &callIdList)
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::GetCallIdListForConference(std::vector<std::u16string> &callIdList)
{
    return TELEPHONY_ERROR;
}

int32_t SatelliteCall::IsSupportConferenceable()
{
    return TELEPHONY_ERROR;
}
} // namespace Telephony
} // namespace OHOS