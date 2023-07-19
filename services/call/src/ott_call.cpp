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

#include "ott_call.h"

#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "telephony_log_wrapper.h"

#include "ott_call_connection.h"
#include "ott_conference.h"

namespace OHOS {
namespace Telephony {
OTTCall::OTTCall(DialParaInfo &info) : CallBase(info), ottCallConnectionPtr_(std::make_unique<OTTCallConnection>())
{}

OTTCall::OTTCall(DialParaInfo &info, AppExecFwk::PacMap &extras)
    : CallBase(info, extras), ottCallConnectionPtr_(std::make_unique<OTTCallConnection>())
{}

OTTCall::~OTTCall() {}

int32_t OTTCall::DialingProcess()
{
    int32_t ret = DialCallBase();
    if (ret != TELEPHONY_SUCCESS) {
        HangUpCall();
    }
    return ret;
}

int32_t OTTCall::AnswerCall(int32_t videoState)
{
    int32_t ret = AnswerCallBase();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("answer call failed!");
        CallManagerHisysevent::WriteAnswerCallFaultEvent(
            INVALID_PARAMETER, INVALID_PARAMETER, videoState, ret, "the device is currently not ringing");
        return CALL_ERR_ANSWER_FAILED;
    }
    OttCallRequestInfo requestInfo;
    ret = PackOttCallRequestInfo(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PackOttCallRequestInfo failed, error%{public}d", ret);
        CallManagerHisysevent::WriteAnswerCallFaultEvent(
            INVALID_PARAMETER, INVALID_PARAMETER, videoState, ret, "PackOttCallRequestInfo failed");
        return CALL_ERR_ANSWER_FAILED;
    }
    if (ottCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("ottCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ret = ottCallConnectionPtr_->Answer(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("answer call failed!");
        return CALL_ERR_ANSWER_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t OTTCall::RejectCall()
{
    int32_t ret = RejectCallBase();
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    OttCallRequestInfo requestInfo;
    ret = PackOttCallRequestInfo(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PackOttCallRequestInfo failed, error%{public}d", ret);
        CallManagerHisysevent::WriteHangUpFaultEvent(
            INVALID_PARAMETER, INVALID_PARAMETER, ret, "Reject PackOttCallRequestInfo failed");
        return CALL_ERR_REJECT_FAILED;
    }
    ret = ottCallConnectionPtr_->Reject(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("reject call failed!");
        return CALL_ERR_REJECT_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t OTTCall::HangUpCall()
{
    OttCallRequestInfo requestInfo;
    int32_t ret = PackOttCallRequestInfo(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PackOttCallRequestInfo failed, error%{public}d", ret);
        CallManagerHisysevent::WriteHangUpFaultEvent(
            INVALID_PARAMETER, INVALID_PARAMETER, ret, "HangUp PackOttCallRequestInfo failed");
        return CALL_ERR_HANGUP_FAILED;
    }
    ret = ottCallConnectionPtr_->HangUp(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("hangUp call failed!");
        return CALL_ERR_HANGUP_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t OTTCall::HoldCall()
{
    OttCallRequestInfo requestInfo;
    int32_t ret = PackOttCallRequestInfo(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PackOttCallRequestInfo failed, error%{public}d", ret);
        return CALL_ERR_HOLD_FAILED;
    }
    ret = ottCallConnectionPtr_->HoldCall(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("holdCall call failed!");
        return CALL_ERR_HOLD_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t OTTCall::UnHoldCall()
{
    OttCallRequestInfo requestInfo;
    int32_t ret = PackOttCallRequestInfo(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PackOttCallRequestInfo failed, error%{public}d", ret);
        return CALL_ERR_UNHOLD_FAILED;
    }
    ret = ottCallConnectionPtr_->UnHoldCall(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("unHoldCall call failed!");
        return CALL_ERR_UNHOLD_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t OTTCall::SwitchCall()
{
    OttCallRequestInfo requestInfo;
    int32_t ret = PackOttCallRequestInfo(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PackOttCallRequestInfo failed, error%{public}d", ret);
        return CALL_ERR_UNHOLD_FAILED;
    }
    ret = ottCallConnectionPtr_->SwitchCall(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("switchCall call failed!");
        return CALL_ERR_UNHOLD_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

void OTTCall::GetCallAttributeInfo(CallAttributeInfo &info)
{
    GetCallAttributeBaseInfo(info);
}

bool OTTCall::GetEmergencyState()
{
    return false;
}

int32_t OTTCall::StartDtmf(char str)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::StopDtmf()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::PostDialProceed(bool proceed)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::GetSlotId()
{
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t OTTCall::CombineConference()
{
    int32_t ret = DelayedSingleton<OttConference>::GetInstance()->SetMainCall(GetCallID());
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetMainCall failed,  error%{public}d", ret);
        return ret;
    }
    OttCallRequestInfo requestInfo;
    ret = PackOttCallRequestInfo(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PackOttCallRequestInfo failed, error%{public}d", ret);
        return ret;
    }
    if (ottCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("ottCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return ottCallConnectionPtr_->CombineConference(requestInfo);
}

int32_t OTTCall::CanCombineConference()
{
    int32_t ret = IsSupportConferenceable();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("call unsupported conference, error%{public}d", ret);
        return ret;
    }
    return DelayedSingleton<OttConference>::GetInstance()->CanCombineConference();
}

int32_t OTTCall::SeparateConference()
{
    OttCallRequestInfo requestInfo;
    int32_t ret = PackOttCallRequestInfo(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PackOttCallRequestInfo failed, error%{public}d", ret);
        return ret;
    }
    if (ottCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("ottCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return ottCallConnectionPtr_->SeparateConference(requestInfo);
}

int32_t OTTCall::KickOutFromConference()
{
    OttCallRequestInfo requestInfo;
    int32_t ret = PackOttCallRequestInfo(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("PackOttCallRequestInfo failed, error%{public}d", ret);
        return ret;
    }
    if (ottCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("ottCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return ottCallConnectionPtr_->KickOutFromConference(requestInfo);
}

int32_t OTTCall::CanSeparateConference()
{
    return DelayedSingleton<OttConference>::GetInstance()->CanSeparateConference();
}

int32_t OTTCall::CanKickOutFromConference()
{
    return DelayedSingleton<OttConference>::GetInstance()->CanKickOutFromConference();
}

int32_t OTTCall::LaunchConference()
{
    int32_t ret = DelayedSingleton<OttConference>::GetInstance()->JoinToConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
    }
    return ret;
}

int32_t OTTCall::ExitConference()
{
    int32_t ret = DelayedSingleton<OttConference>::GetInstance()->LeaveFromConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_IDLE);
    }
    return ret;
}

int32_t OTTCall::HoldConference()
{
    int32_t ret = DelayedSingleton<OttConference>::GetInstance()->HoldConference(GetCallID());
    if (ret == TELEPHONY_SUCCESS) {
        SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_HOLDING);
    }
    return ret;
}

int32_t OTTCall::GetMainCallId(int32_t &mainCallId)
{
    mainCallId = DelayedSingleton<OttConference>::GetInstance()->GetMainCall();
    return TELEPHONY_SUCCESS;
}

int32_t OTTCall::GetSubCallIdList(std::vector<std::u16string> &callIdList)
{
    return DelayedSingleton<OttConference>::GetInstance()->GetSubCallIdList(GetCallID(), callIdList);
}

int32_t OTTCall::GetCallIdListForConference(std::vector<std::u16string> &callIdList)
{
    return DelayedSingleton<OttConference>::GetInstance()->GetCallIdListForConference(GetCallID(), callIdList);
}

int32_t OTTCall::IsSupportConferenceable()
{
#ifdef ABILIT_CONFIG_SUPPORT
    bool ottSupport = GetOttConfig(OTT_SUPPORT_CONFERENCE);
    if (!ottSupport) {
        return TELEPHONY_CONFERENCE_OTT_NOT_SUPPORT;
    }
    if (isVideoCall()) {
        ottSupport = GetOTTConfig(OTT_VIDEO_SUPPORT_CONFERENCE)
    }
    if (!ottSupport) {
        return TELEPHONY_CONFERENCE_VIDEO_CALL_NOT_SUPPORT;
    }
#endif
    return TELEPHONY_SUCCESS;
}

int32_t OTTCall::SendUpdateCallMediaModeRequest(ImsCallMode mode)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::RecieveUpdateCallMediaModeRequest(ImsCallMode mode)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::SendUpdateCallMediaModeResponse(ImsCallMode mode)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::ReceiveUpdateCallMediaModeResponse(CallMediaModeResponse &response)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::DispatchUpdateVideoRequest(ImsCallMode mode)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::DispatchUpdateVideoResponse(ImsCallMode mode)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::SetMute(int32_t mute, int32_t slotId)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::PackOttCallRequestInfo(OttCallRequestInfo &requestInfo)
{
    if (accountNumber_.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    if (memcpy_s(requestInfo.phoneNum, kMaxNumberLen, accountNumber_.c_str(), accountNumber_.length()) != EOK) {
        TELEPHONY_LOGW("memset_s failed!");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    if (bundleName_.length() > static_cast<size_t>(kMaxBundleNameLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    if (memcpy_s(requestInfo.bundleName, kMaxBundleNameLen, bundleName_.c_str(), bundleName_.length()) != EOK) {
        TELEPHONY_LOGW("memset_s failed!");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    requestInfo.videoState = videoState_;
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
