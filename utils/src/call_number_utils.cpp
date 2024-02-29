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

#include "call_number_utils.h"

#include <regex>

#include "phonenumbers/phonenumber.pb.h"
#include "telephony_log_wrapper.h"
#include "telephony_types.h"
#include "call_manager_errors.h"
#include "cellular_call_connection.h"
#include "core_service_client.h"
#include "cellular_data_client.h"


namespace OHOS {
namespace Telephony {
CallNumberUtils::CallNumberUtils() {}

CallNumberUtils::~CallNumberUtils() {}

int32_t CallNumberUtils::FormatPhoneNumber(
    const std::string &phoneNumber, const std::string &countryCode, std::string &formatNumber)
{
    if (phoneNumber.empty()) {
        TELEPHONY_LOGE("phoneNumber is nullptr!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (phoneNumber.front() == '#' || phoneNumber.front() == '*') {
        formatNumber = phoneNumber;
        return TELEPHONY_SUCCESS;
    }
    i18n::phonenumbers::PhoneNumberUtil *phoneUtils = i18n::phonenumbers::PhoneNumberUtil::GetInstance();
    if (phoneUtils == nullptr) {
        TELEPHONY_LOGE("phoneUtils is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::string tmpCode = countryCode;
    transform(tmpCode.begin(), tmpCode.end(), tmpCode.begin(), ::toupper);
    i18n::phonenumbers::PhoneNumber parseResult;
    phoneUtils->ParseAndKeepRawInput(phoneNumber, tmpCode, &parseResult);
    phoneUtils->FormatInOriginalFormat(parseResult, tmpCode, &formatNumber);
    if (formatNumber.empty() || formatNumber == "0") {
        formatNumber = "";
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallNumberUtils::FormatPhoneNumberToE164(
    const std::string phoneNumber, const std::string countryCode, std::string &formatNumber)
{
    return FormatNumberBase(phoneNumber, countryCode, i18n::phonenumbers::PhoneNumberUtil::E164, formatNumber);
}

int32_t CallNumberUtils::FormatPhoneNumberToNational(
    const std::string phoneNumber, const std::string countryCode, std::string &formatNumber)
{
    int32_t ret = FormatNumberBase(phoneNumber, countryCode,
        i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::NATIONAL, formatNumber);
    ProcessSpace(formatNumber);
    return ret;
}

int32_t CallNumberUtils::FormatPhoneNumberToInternational(
    const std::string phoneNumber, const std::string countryCode, std::string &formatNumber)
{
    int32_t ret = FormatNumberBase(phoneNumber, countryCode,
        i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::INTERNATIONAL, formatNumber);
    ProcessSpace(formatNumber);
    return ret;
}

int32_t CallNumberUtils::FormatNumberBase(const std::string phoneNumber, std::string countryCode,
    const i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat formatInfo, std::string &formatNumber)
{
    if (phoneNumber.empty()) {
        TELEPHONY_LOGE("phoneNumber is nullptr!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    i18n::phonenumbers::PhoneNumberUtil *phoneUtils = i18n::phonenumbers::PhoneNumberUtil::GetInstance();
    if (phoneUtils == nullptr) {
        TELEPHONY_LOGE("phoneUtils is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    transform(countryCode.begin(), countryCode.end(), countryCode.begin(), ::toupper);
    i18n::phonenumbers::PhoneNumber parseResult;
    phoneUtils->Parse(phoneNumber, countryCode, &parseResult);
    if (phoneUtils->IsValidNumber(parseResult) || HasBCPhoneNumber(phoneNumber)) {
        phoneUtils->Format(parseResult, formatInfo, &formatNumber);
    }
    return TELEPHONY_SUCCESS;
}

void CallNumberUtils::ProcessSpace(std::string &number)
{
    std::string word;
    std::stringstream streamNum(number);
    std::string store;
    while (streamNum >> word) {
        store += word;
    }
    number = store;
}

int32_t CallNumberUtils::CheckNumberIsEmergency(const std::string &phoneNumber, const int32_t slotId, bool &enabled)
{
    return DelayedSingleton<CellularCallConnection>::GetInstance()->IsEmergencyPhoneNumber(
        phoneNumber, slotId, enabled);
}

bool CallNumberUtils::IsValidSlotId(int32_t slotId) const
{
    if (SIM_SLOT_COUNT == HAS_A_SLOT) {
        return slotId == SIM_SLOT_0;
    }
    if (SIM_SLOT_COUNT == HAS_TWO_SLOT) {
        if (slotId == SIM_SLOT_0 || slotId == SIM_SLOT_1) {
            return true;
        }
    }
    return false;
}

bool CallNumberUtils::IsMMICode(const std::string &number)
{
    if (number.empty()) {
        TELEPHONY_LOGE("number is empty.");
        return false;
    }
    if (RegexMatchMmi(number)) {
        return true;
    }

    if ((number.front() == '*' || number.front() == '#') && number.back() == '#') {
        TELEPHONY_LOGI("number start with * or # and end with #");
        return true;
    }

    return false;
}

bool CallNumberUtils::RegexMatchMmi(const std::string &number)
{
    std::string symbols =
        "((\\*|#|\\*#|\\*\\*|##)(\\d{2,3})(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*))?)?)?)?#)(.*)";
    std::regex pattern(symbols);
    std::smatch results;
    if (regex_match(number, results, pattern)) {
        TELEPHONY_LOGI("regex_match ture");
        return true;
    }
    return false;
}

std::string CallNumberUtils::RemoveSeparatorsPhoneNumber(const std::string &phoneString)
{
    std::string newString;
    if (phoneString.empty()) {
        TELEPHONY_LOGE("RemoveSeparatorsPhoneNumber return, phoneStr is empty.");
        return newString;
    }
    for (char c : phoneString) {
        if ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+' || c == 'N' || c == ',' || c == ';') {
            newString += c;
        }
    }

    return newString;
}

std::string CallNumberUtils::RemovePostDialPhoneNumber(const std::string &phoneString)
{
    std::string newString = "";
    if (phoneString.empty()) {
        TELEPHONY_LOGE("RemovePostDialPhoneNumber return, phoneStr is empty.");
        return newString;
    }
    for (char c : phoneString) {
        if ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+' || c == 'N') {
            newString += c;
        } else if (c == ',' || c == ';') {
            break;
        }
    }

    return newString;
}

bool CallNumberUtils::HasAlphabetInPhoneNum(const std::string &inputValue)
{
    if (inputValue.empty()) {
        TELEPHONY_LOGE("HasAlphabetInPhoneNum return, input is empty.");
        return true;
    }
    for (char c : inputValue) {
        if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
            TELEPHONY_LOGE("The Phone Number contains letter");
            return true;
        }
    }
    TELEPHONY_LOGI("The Phone Number is valid");
    return false;
}

bool CallNumberUtils::HasBCPhoneNumber(const std::string &phoneNumber)
{
    int32_t phoneNumberStart = 0;
    int32_t phoneNumberStartLength = 3;
    size_t bCNumberLength = 11;
    std::string bCNumberStart = "192";
    if (phoneNumber.length() == bCNumberLength &&
        phoneNumber.substr(phoneNumberStart, phoneNumberStartLength) == bCNumberStart) {
        return true;
    }
    return false;
}

bool CallNumberUtils::SelectAccountId(int32_t slotId, AppExecFwk::PacMap &extras)
{
    if (IsValidSlotId(slotId)) {
        return true;
    }
    int32_t defaultVoiceSlotId = DelayedRefSingleton<CoreServiceClient>::GetInstance().GetDefaultVoiceSlotId();
    if (defaultVoiceSlotId != TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL && IsValidSlotId(defaultVoiceSlotId)) {
        extras.PutIntValue("accountId", defaultVoiceSlotId);
        TELEPHONY_LOGI("select accountId to defaultVoiceSlotId = %{public}d", defaultVoiceSlotId);
        return true;
    }
    int32_t defaultDataSlotId = DelayedRefSingleton<CellularDataClient>::GetInstance().GetDefaultCellularDataSlotId();
    if (defaultDataSlotId != TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL && IsValidSlotId(defaultDataSlotId)) {
        extras.PutIntValue("accountId", defaultDataSlotId);
        TELEPHONY_LOGI("select accountId to defaultDataSlotId = %{public}d", defaultDataSlotId);
        return true;
    }
    return false;
}
} // namespace Telephony
} // namespace OHOS
