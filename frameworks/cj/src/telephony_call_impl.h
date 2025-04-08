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
#ifndef TELEPHONY_CALL_IMPL_H
#define TELEPHONY_CALL_IMPL_H

#include "telephony_call_utils.h"
#include "ability_context.h"

namespace OHOS {
namespace Telephony {

class TelephonyCallImpl {
public:
    static bool IsRinging(int32_t *errCode);

    static char* FormatPhoneNumberToE164(char *cPhoneNumber, char *cCountryCode, int32_t *errCode);

    static void Init();

    static int32_t DialCall(char *phoneNumber, DialCallOptions options);

    static bool HasVoiceCapability();

    static char* FormatPhoneNumber(char *cPhoneNumber, char *cCountryCode, int32_t *errCode);

    static int32_t GetCallState();

    static bool HasCall();

    static bool IsEmergencyPhoneNumber(char *cPhoneNumber, int32_t slotId, int32_t* errCode);

    static int32_t MakeCall(char* phoneNumber);
};
}
}

#endif // TELEPHONY_CALL_IMPL_H