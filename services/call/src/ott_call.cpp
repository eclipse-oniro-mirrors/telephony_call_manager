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

namespace OHOS {
namespace Telephony {
OTTCall::OTTCall(DialParaInfo &info) : CallBase(info) {}

OTTCall::OTTCall(DialParaInfo &info, AppExecFwk::PacMap &extras) : CallBase(info, extras) {}

OTTCall::~OTTCall() {}

int32_t OTTCall::DialingProcess()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::AnswerCall(int32_t videoState)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::RejectCall(bool isSendSms, std::string &content)
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::HangUpCall()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::HoldCall()
{
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t OTTCall::UnHoldCall()
{
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t OTTCall::SwitchCall()
{
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
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
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t OTTCall::StopDtmf()
{
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t OTTCall::SendDtmf(std::string &phoneNum, char str)
{
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t OTTCall::SendBurstDtmf(std::string &phoneNum, std::string str, int32_t on, int32_t off)
{
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t OTTCall::GetSlotId()
{
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t OTTCall::CombineConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::SeparateConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::CanCombineConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::CanSeparateConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::LunchConference()
{
    return CALL_ERR_FUNCTION_NOT_SUPPORTED;
}

int32_t OTTCall::ExitConference()
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
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
