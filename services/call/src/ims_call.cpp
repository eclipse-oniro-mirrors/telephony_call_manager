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

#include "ims_call.h"

#include "call_object_manager.h"
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "ims_conference_base.h"

namespace OHOS {
namespace Telephony {
IMSCall::IMSCall(DialParaInfo &info) : CarrierCall(info) {}

IMSCall::IMSCall(DialParaInfo &info, AppExecFwk::PacMap &extras) : CarrierCall(info, extras) {}

IMSCall::~IMSCall() {}

int32_t IMSCall::DialingProcess()
{
    return CarrierDialingProcess();
}

int32_t IMSCall::AnswerCall(int32_t videoState)
{
    return CarrierAcceptCall(videoState);
}

int32_t IMSCall::RejectCall(bool isSendSms, std::string &content)
{
    return CarrierRejectCall(isSendSms, content);
}

int32_t IMSCall::HangUpCall()
{
    return CarrierHangUpCall();
}

int32_t IMSCall::HoldCall()
{
    return CarrierHoldCall();
}

int32_t IMSCall::UnHoldCall()
{
    return CarrierUnHoldCall();
}

int32_t IMSCall::SwitchCall()
{
    return CarrierSwitchCall();
}

void IMSCall::GetCallAttributeInfo(CallAttributeInfo &info)
{
    GetCallAttributeCarrierInfo(info);
}

int32_t IMSCall::CombineConference()
{
    int32_t ret = DelayedSingleton<ImsConferenceBase>::GetInstance()->SetMainCall(GetCallID());
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetMainCall failed,  error%{public}d", ret);
        return ret;
    }
    return CarrierCombineConference();
}

int32_t IMSCall::SeparateConference()
{
    return CarrierSeparateConference();
}

int32_t IMSCall::CanCombineConference()
{
    int32_t ret = IsSupportConferenceable();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("call unsupport conference,  error%{public}d", ret);
        return ret;
    }
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->CanCombineConference();
}

int32_t IMSCall::CanSeparateConference()
{
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->CanSeparateConference();
}

int32_t IMSCall::GetMainCallId()
{
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->GetMainCall();
}

int32_t IMSCall::LunchConference()
{
    int32_t ret = DelayedSingleton<ImsConferenceBase>::GetInstance()->JoinToConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
    }
    return ret;
}

int32_t IMSCall::ExitConference()
{
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->LeaveFromConference(GetCallID());
}

std::vector<std::u16string> IMSCall::GetSubCallIdList()
{
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->GetSubCallIdList(GetCallID());
}

std::vector<std::u16string> IMSCall::GetCallIdListForConference()
{
    return DelayedSingleton<ImsConferenceBase>::GetInstance()->GetCallIdListForConference(GetCallID());
}

int32_t IMSCall::IsSupportConferenceable()
{
#ifdef ABILIT_CONFIG_SUPPORT
    bool carrierSupport = GetCarrierConfig(IMS_SUPPORT_CONFERENCE);
    if (!carrierSupport) {
        return TELEPHONY_CONFERENCE_CARRIER_NOT_SUPPORT;
    }
    if (isVideoCall()) {
        carrierSupport = GetCarrierConfig(IMS_VIDEO_SUPPORT_CONFERENCE)
    }
    if (!carrierSupport) {
        return TELEPHONY_CONFERENCE_VIDEO_CALL_NOT_SUPPORT;
    }
#endif
    return CarrierCall::IsSupportConferenceable();
}
} // namespace Telephony
} // namespace OHOS
