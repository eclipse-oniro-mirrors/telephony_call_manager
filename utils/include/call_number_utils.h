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

#ifndef CALL_NUMBER_UTILS_H
#define CALL_NUMBER_UTILS_H

#include "pac_map.h"
#include "singleton.h"
#include "phonenumberutil.h"

#include "call_base.h"
#include "common_type.h"

namespace OHOS {
namespace Telephony {
class CallNumberUtils {
    DECLARE_DELAYED_SINGLETON(CallNumberUtils)
public:
    int32_t FormatPhoneNumber(
        const std::string &phoneNumber, const std::string &countryCode, std::string &formatNumber);
    int32_t FormatPhoneNumberToE164(
        const std::string phoneNumber, const std::string countryCode, std::string &formatNumber);
    int32_t FormatPhoneNumberToNational(
        const std::string phoneNumber, const std::string countryCode, std::string &formatNumber);
    int32_t FormatPhoneNumberToInternational(
        const std::string phoneNumber, const std::string countryCode, std::string &formatNumber);
    int32_t FormatNumberBase(const std::string phoneNumber, std::string countryCode,
        const i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat formatInfo, std::string &formatNumber);
    int32_t FormatPhoneNumberAsYouType(
        const std::string &phoneNumber, const std::string &countryCode, std::string &formatNumber);
    int32_t CheckNumberIsEmergency(const std::string &phoneNumber, const int32_t slotId, bool &enabled);
    bool IsValidSlotId(int32_t slotId) const;
    int32_t IsCarrierVtConfig(const int32_t slotId, bool &enabled);
    std::string RemoveSeparatorsPhoneNumber(const std::string &phoneString);
    std::string RemovePostDialPhoneNumber(const std::string &phoneString);
    bool HasAlphabetInPhoneNum(const std::string &inputValue);
    bool HasBCPhoneNumber(const std::string &phoneNumber);
    bool SelectAccountId(int32_t slotId, AppExecFwk::PacMap &extras);
    int32_t QueryNumberLocationInfo(std::string &numberLocation, std::string accountNumber);
    void NumberLocationUpdate(const sptr<CallBase> &callObjectPtr);
    int32_t QueryYellowPageAndMarkInfo(NumberMarkInfo &numberMarkInfo, std::string accountNumber);
    void YellowPageAndMarkUpdate(const sptr<CallBase> &callObjectPtr);
private:
    void ProcessSpace(std::string &number);

private:
    static const int16_t HAS_A_SLOT = 1;
    static const int16_t HAS_TWO_SLOT = 2;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_NUMBER_UTILS_H