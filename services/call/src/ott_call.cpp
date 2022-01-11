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
#include "telephony_log_wrapper.h"

#include "ott_call_connection.h"

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
        return CALL_ERR_ANSWER_FAILED;
    }
    OttCallRequestInfo requestInfo;
    PackOttCallRequestInfo(requestInfo);
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
    PackOttCallRequestInfo(requestInfo);
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
    PackOttCallRequestInfo(requestInfo);
    int32_t ret = ottCallConnectionPtr_->HangUp(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("hangUp call failed!");
        return CALL_ERR_HANGUP_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t OTTCall::HoldCall()
{
    OttCallRequestInfo requestInfo;
    PackOttCallRequestInfo(requestInfo);
    int32_t ret = ottCallConnectionPtr_->HoldCall(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("holdCall call failed!");
        return CALL_ERR_HOLD_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t OTTCall::UnHoldCall()
{
    OttCallRequestInfo requestInfo;
    PackOttCallRequestInfo(requestInfo);
    int32_t ret = ottCallConnectionPtr_->UnHoldCall(requestInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("unHoldCall call failed!");
        return CALL_ERR_UNHOLD_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t OTTCall::SwitchCall()
{
    OttCallRequestInfo requestInfo;
    PackOttCallRequestInfo(requestInfo);
    int32_t ret = ottCallConnectionPtr_->SwitchCall(requestInfo);
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

int32_t OTTCall::GetSlotId()
{
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t OTTCall::CombineConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::CanCombineConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}
int32_t OTTCall::SeparateConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::CanSeparateConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::LaunchConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::ExitConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::HoldConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::GetMainCallId()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

std::vector<std::u16string> OTTCall::GetSubCallIdList()
{
    std::vector<std::u16string> vec;
    return vec;
}

std::vector<std::u16string> OTTCall::GetCallIdListForConference()
{
    std::vector<std::u16string> vec;
    return vec;
}

int32_t OTTCall::IsSupportConferenceable()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::SendUpdateCallMediaModeRequest(CallMediaMode mode)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::RecieveUpdateCallMediaModeRequest(CallMediaMode mode)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::SendUpdateCallMediaModeResponse(CallMediaMode mode)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::ReceiveUpdateCallMediaModeResponse(CallMediaModeResponse &response)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

void OTTCall::PackOttCallRequestInfo(OttCallRequestInfo &requestInfo)
{
    (void)memcpy_s(requestInfo.phoneNum, kMaxNumberLen, accountNumber_.c_str(), accountNumber_.length());
    (void)memcpy_s(requestInfo.bundleName, kMaxBundleNameLen, bundleName_.c_str(), bundleName_.length());
    requestInfo.videoState = videoState_;
}
} // namespace Telephony
} // namespace OHOS
