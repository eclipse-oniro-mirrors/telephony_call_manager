/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#include "ohos.telephony.call.proj.hpp"
#include "ohos.telephony.call.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"

#include "call_manager_client.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include <string_ex.h>

using namespace taihe;
using namespace OHOS::Telephony;
namespace {
// To be implemented.

void MakeCallSync(::taihe::string_view phoneNumber)
{
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->MakeCall(std::string(phoneNumber));
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "makeCall return error");
    }
    return;
}

void MakeCallSync2(uintptr_t context, ::taihe::string_view phoneNumber)
{
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->MakeCall(std::string(phoneNumber));
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "makeCall2 return error");
    }
    return;
}

bool HasVoiceCapability()
{
    auto ret = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HasVoiceCapability();
    return static_cast<ani_boolean>(ret);
}

void FormatPhoneNumberSync(::taihe::string_view phoneNumber,
    ::ohos::telephony::call::NumberFormatOptions const& options)
{
    std::u16string phoneNum = OHOS::Str8ToStr16(std::string(phoneNumber));
    std::u16string countryCode = OHOS::Str8ToStr16(std::string(options.countryCode));
    std::u16string formatNum = OHOS::Str8ToStr16("");
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
        phoneNum, countryCode, formatNum);
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "formatPhoneNumber return error");
    }
    return;
}

void FormatPhoneNumberSync2(::taihe::string_view phoneNumber)
{
    std::u16string phoneNum = OHOS::Str8ToStr16(std::string(phoneNumber));
    std::u16string countryCode = OHOS::Str8ToStr16("");
    std::u16string formatNum = OHOS::Str8ToStr16("");
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
        phoneNum, countryCode, formatNum);
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "formatPhoneNumber2 return error");
    }
    return;
}

::ohos::telephony::call::CallState GetCallStateSync()
{
    int32_t callState = static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN);
    callState = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetCallState();
    return static_cast<::ohos::telephony::call::CallState::key_t>(callState);
}

bool HasCallSync()
{
    auto ret = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HasCall();
    return static_cast<ani_boolean>(ret);
}
} //namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_MakeCallSync(MakeCallSync);
TH_EXPORT_CPP_API_MakeCallSync2(MakeCallSync2);
TH_EXPORT_CPP_API_HasVoiceCapability(HasVoiceCapability);
TH_EXPORT_CPP_API_FormatPhoneNumberSync(FormatPhoneNumberSync);
TH_EXPORT_CPP_API_FormatPhoneNumberSync2(FormatPhoneNumberSync2);
TH_EXPORT_CPP_API_GetCallStateSync(GetCallStateSync);
TH_EXPORT_CPP_API_HasCallSync(HasCallSync);
// NOLINTEND