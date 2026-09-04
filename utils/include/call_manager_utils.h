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

#ifndef CALL_MANAGER_UTILS_H
#define CALL_MANAGER_UTILS_H

#include "call_manager_info.h"

#include "call_base.h"
#include "message_parcel.h"
#include "bundle_mgr_proxy.h"

namespace OHOS {
namespace Telephony {

constexpr int32_t PHONE_NUMBER_LEN_DEFAULT = 11;
struct CallAttributeInfo;

class CallManagerUtils {
public:
    static __attribute__((noinline)) void WriteCallAttributeInfo(
        const CallAttributeInfo &info, MessageParcel &messageParcel);
    static __attribute__((noinline)) bool IsBundleInstalled(const std::string &bundleName, int32_t userId);
    static __attribute__((noinline)) std::string GetSystemParameter(
        const std::string &key, const std::string &defaultVal);
    static bool IsWearableDevice();
    static bool IsTransferControlEnable();
    static bool IsPurePhoneNumber(const std::string &s);
    static bool IsTransferCall(CallType type);

    static std::string GetSelfPhoneNumber(uint32_t slotId);

private:
    static bool IsValidLength(const std::string &s);
    static bool IsValidCharBeforeOpenBracket(const std::string &s, size_t i);
    static bool IsValidCharAfterCloseBracket(const std::string &s, size_t i);
    static bool HasMinDigits(size_t digitCount, bool hasLeadingPlus);
    static bool IsForcedReportVoiceCall(const CallAttributeInfo &info);
    static void WriteVoipCallInfo(const CallAttributeInfo &info, MessageParcel &messageParcel);
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_MANAGER_UTILS_H