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
    return CarrierAcceptCall(videoState);
}

int32_t CSCall::RejectCall(bool isSendSms, std::string &content)
{
    return CarrierRejectCall(isSendSms, content);
}

int32_t CSCall::HangUpCall()
{
    return CarrierHangUpCall();
}

int32_t CSCall::HoldCall()
{
    return CarrierHoldCall();
}

int32_t CSCall::UnHoldCall()
{
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

int32_t CSCall::CombineConference()
{
    int32_t ret = DelayedSingleton<CsConferenceBase>::GetInstance()->SetMainCall(GetCallID());
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return CarrierCombineConference();
}

int32_t CSCall::SeparateConference()
{
    return CarrierSeparateConference();
}

int32_t CSCall::CanCombineConference()
{
    int32_t ret = IsSupportConferenceable();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("call unsupport conference,  error%{public}d", ret);
        return ret;
    }
    return DelayedSingleton<CsConferenceBase>::GetInstance()->CanCombineConference();
}

int32_t CSCall::CanSeparateConference()
{
    return DelayedSingleton<CsConferenceBase>::GetInstance()->CanSeparateConference();
}

int32_t CSCall::LunchConference()
{
    int32_t ret = DelayedSingleton<CsConferenceBase>::GetInstance()->JoinToConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
    }
    return ret;
}

int32_t CSCall::ExitConference()
{
    return DelayedSingleton<CsConferenceBase>::GetInstance()->LeaveFromConference(GetCallID());
}

int32_t CSCall::GetMainCallId()
{
    return DelayedSingleton<CsConferenceBase>::GetInstance()->GetMainCall();
}

std::vector<std::u16string> CSCall::GetSubCallIdList()
{
    return DelayedSingleton<CsConferenceBase>::GetInstance()->GetSubCallIdList(GetCallID());
}

std::vector<std::u16string> CSCall::GetCallIdListForConference()
{
    return DelayedSingleton<CsConferenceBase>::GetInstance()->GetCallIdListForConference(GetCallID());
}

int32_t CSCall::IsSupportConferenceable()
{
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
