/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "telephony_call_ffi.h"
#include "telephony_call_impl.h"

using namespace OHOS::FFI;

namespace OHOS {
namespace Telephony {
extern "C" {
    bool FfiOHOSTelephonyCallIsRinging(int32_t *errCode)
    {
        return TelephonyCallImpl::IsRinging(errCode);
    }

    char* FfiOHOSTelephonyCallFormatPhoneNumberToE164(char *cPhoneNumber, char *cCountryCode, int32_t *errCode)
    {
        return TelephonyCallImpl::FormatPhoneNumberToE164(cPhoneNumber, cCountryCode, errCode);
    }

    int32_t FfiOHOSTelephonyCallMakeCall(char* number)
    {
        return TelephonyCallImpl::MakeCall(number);
    }

    bool FfiOHOSTelephonyCallIsEmergencyPhoneNumber(char* phoneNumber, int32_t slotId, int32_t* errCode)
    {
        return TelephonyCallImpl::IsEmergencyPhoneNumber(phoneNumber, slotId, errCode);
    }

    bool FfiOHOSTelephonyCallHasCall()
    {
        return TelephonyCallImpl::HasCall();
    }

    int32_t FfiOHOSTelephonyCallGetCallState()
    {
        return TelephonyCallImpl::GetCallState();
    }

    void FfiOHOSTelephonyCallInit()
    {
        return TelephonyCallImpl::Init();
    }

    int32_t FfiOHOSTelephonyCallDialCall(char *phoneNumber, DialCallOptions options)
    {
        return TelephonyCallImpl::DialCall(phoneNumber, options);
    }

    bool FfiOHOSTelephonyCallHasVoiceCapability()
    {
        return TelephonyCallImpl::HasVoiceCapability();
    }

    char* FfiOHOSTelephonyCallFormatPhoneNumber(char *phoneNumber, char *countryCode, int32_t *errCode)
    {
        return TelephonyCallImpl::FormatPhoneNumber(phoneNumber, countryCode, errCode);
    }
}
}  // namespace Telephony
}  // namespace OHOS
