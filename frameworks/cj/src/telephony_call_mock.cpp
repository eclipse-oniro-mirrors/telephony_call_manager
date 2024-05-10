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

#include "cj_common_ffi.h"

extern "C" {
FFI_EXPORT int FfiOHOSTelephonyCallIsRinging = 0;
FFI_EXPORT int FfiOHOSTelephonyCallFormatPhoneNumberToE164 = 0;
FFI_EXPORT int FfiOHOSTelephonyCallInit = 0;
FFI_EXPORT int FfiOHOSTelephonyCallDialCall = 0;
FFI_EXPORT int FfiOHOSTelephonyCallHasVoiceCapability = 0;
FFI_EXPORT int FfiOHOSTelephonyCallFormatPhoneNumber = 0;
FFI_EXPORT int FfiOHOSTelephonyCallGetCallState = 0;
FFI_EXPORT int FfiOHOSTelephonyCallHasCall = 0;
FFI_EXPORT int FfiOHOSTelephonyCallIsEmergencyPhoneNumber = 0;
FFI_EXPORT int FfiOHOSTelephonyCallMakeCall = 0;
}