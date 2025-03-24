/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "mmi_code_utils.h"

#include <regex>

#include "call_object_manager.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

bool MMICodeUtils::IsMMICode(std::string &analyseString)
{
    if (analyseString.empty()) {
        TELEPHONY_LOGE("analyseString is empty.");
        return false;
    }
    if (RegexMatchMmi(analyseString)) {
        return true;
    }
    if ((analyseString.front() == '*' || analyseString.front() == '#') && analyseString.back() == '#') {
        TELEPHONY_LOGI("analyseString start with * or # and end with #");
        mmiData_.fullString = analyseString;
        return true;
    }

    return false;
}

bool MMICodeUtils::RegexMatchMmi(std::string &analyseString)
{
    if (IsShortCode(analyseString)) {
        return true;
    }
    std::string symbols =
        "((\\*|#|\\*#|\\*\\*|##)(\\d{2,3})(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*))?)?)?)?#)(.*)";
    std::regex pattern(symbols);
    std::smatch results;
    if (!regex_match(analyseString, results, pattern)) {
        TELEPHONY_LOGI("regex_match false");
        return false;
    }
    /**
     * The following sequence of functions shall be used for the control of Supplementary Services:
     *  SELECT:	Entry of the procedure information (may be a digit or a sequence of characters).
     *  SEND: Transmission of the information to the network.
     *  INDICATION:	Call progress indications.
     */
    int32_t fullString = 1;
    int32_t action = 2;
    // 3GPP TS 22.030 V4.0.0 (2001-03)  6.5.2 Structure of the MMI
    // This structure consists of the following parts:
    //     Service Code, SC( (2 or 3 digits)
    //     Supplementary Information, SI (variable length).
    int32_t serviceCode = 3;
    int32_t sia = 5;
    int32_t sib = 7;
    int32_t sic = 9;
    int32_t pwdConfirm = 11;
    int32_t dialingNumber = 12;
    mmiData_.fullString = results.str(fullString);
    mmiData_.actionString = results.str(action);
    mmiData_.serviceCode = results.str(serviceCode);
    mmiData_.serviceInfoA = results.str(sia);
    mmiData_.serviceInfoB = results.str(sib);
    mmiData_.serviceInfoC = results.str(sic);
    mmiData_.pwdString = results.str(pwdConfirm);
    mmiData_.dialString = results.str(dialingNumber);
 
    /* 3GPP TS 22.030 V4.0.0 (2001-03)  6.5.2 Structure of the MMI
     * The procedure always starts with *, #, **, ## or *# and is finished by #.
     * Each part within the procedure is separated by *.
     */
    if (analyseString.back() == '#' && !mmiData_.dialString.empty() && mmiData_.dialString.back() == '#') {
        mmiData_.fullString = analyseString;
    }
    return true;
}

MMIData MMICodeUtils::GetMMIData()
{
    return mmiData_;
}

bool MMICodeUtils::SpecialMatch(const MMIData mmiData, std::string &analyseString)
{
    if ((mmiData.actionString == "#" || mmiData.actionString == "*")
        && (!mmiData.serviceCode.empty() && !mmiData.dialString.empty())) {
        TELEPHONY_LOGI("SpecialMatch true");
        analyseString = mmiData.dialString;
        return true;
    }
    return false;
}

bool MMICodeUtils::IsShortCode(const std::string &analyseString)
{
    std::string symbols = "^(?!1\\d)\\d{2}";
    if (CallObjectManager::HasCellularCallExist()) {
        TELEPHONY_LOGI("IsShortCode: HasCellularCallExist true");
        symbols = "\\d{1,2}";
    }
    std::regex shortCodePattern(symbols);
    std::smatch shortCodeResult;
    if (regex_match(analyseString, shortCodeResult, shortCodePattern)) {
        TELEPHONY_LOGI("patterns shortCodeResults true");
        return true;
    }
    return false;
}
} // namespace Telephony
} // namespace OHOS
