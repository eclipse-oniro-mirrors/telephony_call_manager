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

#include "cs_call.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CSCall::CSCall(DialParaInfo &info) : CarrierCall(info) {}

CSCall::CSCall(DialParaInfo &info, AppExecFwk::PacMap &extras) : CarrierCall(info, extras) {}

CSCall::~CSCall() {}

int32_t CSCall::DialingProcess()
{
    return CarrierDialingProcess();
}

int32_t CSCall::AnswerCall(int32_t videoState)
{
    return CarrierAnswerCall(videoState);
}

int32_t CSCall::RejectCall()
{
    return CarrierRejectCall();
}

int32_t CSCall::HangUpCall()
{
    return CarrierHangUpCall();
}

int32_t CSCall::HoldCall()
{
    ConferenceState conferenceState = DelayedSingleton<CsConference>::GetInstance()->GetConferenceState();
    if (conferenceState == CONFERENCE_STATE_ACTIVE) {
        DelayedSingleton<CsConference>::GetInstance()->SetConferenceState(CONFERENCE_STATE_HOLDING);
    }
    return CarrierHoldCall();
}

int32_t CSCall::UnHoldCall()
{
    ConferenceState conferenceState = DelayedSingleton<CsConference>::GetInstance()->GetConferenceState();
    if (conferenceState == CONFERENCE_STATE_HOLDING) {
        DelayedSingleton<CsConference>::GetInstance()->SetConferenceState(CONFERENCE_STATE_ACTIVE);
    }
    return CarrierUnHoldCall();
}

int32_t CSCall::SwitchCall()
{
    return CarrierSwitchCall();
}

void CSCall::GetCallAttributeInfo(CallAttributeInfo &info)
{
    GetCallAttributeCarrierInfo(info);
}

int32_t CSCall::SetMute(int32_t mute, int32_t slotId)
{
    return CarrierSetMute(mute, slotId);
}

int32_t CSCall::CombineConference()
{
    int32_t ret = DelayedSingleton<CsConference>::GetInstance()->SetMainCall(GetCallID());
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return CarrierCombineConference();
}

int32_t CSCall::SeparateConference()
{
    return CarrierSeparateConference();
}

int32_t CSCall::KickOutFromConference()
{
    return CarrierKickOutFromConference();
}

int32_t CSCall::CanCombineConference()
{
    int32_t ret = IsSupportConferenceable();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("call unsupported conference, error%{public}d", ret);
        return ret;
    }
    return DelayedSingleton<CsConference>::GetInstance()->CanCombineConference();
}

int32_t CSCall::CanSeparateConference()
{
    return DelayedSingleton<CsConference>::GetInstance()->CanSeparateConference();
}

int32_t CSCall::CanKickOutFromConference()
{
    return DelayedSingleton<CsConference>::GetInstance()->CanKickOutFromConference();
}

int32_t CSCall::LaunchConference()
{
    int32_t ret = DelayedSingleton<CsConference>::GetInstance()->JoinToConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
    }
    return ret;
}

int32_t CSCall::ExitConference()
{
    int32_t ret = DelayedSingleton<CsConference>::GetInstance()->LeaveFromConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_IDLE);
    }
    return ret;
}

int32_t CSCall::HoldConference()
{
    int32_t ret = DelayedSingleton<CsConference>::GetInstance()->HoldConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_HOLDING);
    }
    return ret;
}

int32_t CSCall::GetMainCallId(int32_t &mainCallId)
{
    mainCallId = DelayedSingleton<CsConference>::GetInstance()->GetMainCall();
    return TELEPHONY_SUCCESS;
}

int32_t CSCall::GetSubCallIdList(std::vector<std::u16string> &callIdList)
{
    return DelayedSingleton<CsConference>::GetInstance()->GetSubCallIdList(GetCallID(), callIdList);
}

int32_t CSCall::GetCallIdListForConference(std::vector<std::u16string> &callIdList)
{
    return DelayedSingleton<CsConference>::GetInstance()->GetCallIdListForConference(GetCallID(), callIdList);
}

int32_t CSCall::IsSupportConferenceable()
{
    // get from configure file
#ifdef ABILIT_CONFIG_SUPPORT
    bool carrierSupport = GetCarrierConfig(CS_SUPPORT_CONFERENCE);
    if (!carrierSupport) {
        return TELEPHONY_CONFERENCE_CARRIER_NOT_SUPPORT;
    }
#endif
    return CarrierCall::IsSupportConferenceable();
}
} // namespace Telephony
} // namespace OHOS
