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

#ifndef MMI_CODE_UTILS_H
#define MMI_CODE_UTILS_H

#include <string>

namespace OHOS {
namespace Telephony {
/**
 * 3GPP TS 22.030 V4.0.0 (2001-03)  6.5.2 Structure of the MMI
 * The following sequence of functions shall be used for the control of Supplementary Services:
 *  SELECT:	Entry of the procedure information (may be a digit or a sequence of characters).
 *  SEND:	Transmission of the information to the network.
 *  INDICATION:	Call progress indications.
 */
struct MMIData {
    std::string fullString = "";
    std::string actionString = "";
    std::string serviceCode = "";
    // 3GPP TS 22.030 V4.0.0 (2001-03)  6.5.2 Structure of the MMI
    // This structure consists of the following parts:
    // Service Code, SC( (2 or 3 digits);
    // Supplementary Information, SI (variable length).
    std::string serviceInfoA = "";
    std::string serviceInfoB = "";
    std::string serviceInfoC = "";
    std::string pwdString = "";
    std::string dialString = "";
};

// Man-Machine Interface
class MMICodeUtils {
public:
    /**
     * MMICodeUtils constructor
     */
    MMICodeUtils() = default;

    /**
     * ~MMICodeUtils destructor
     */
    ~MMICodeUtils() = default;

    /**
     * IsMMICode
     *
     * 3GPP TS 22.030 V4.0.0 (2001-03)  6.5.2 Structure of the MMI
     * TS 24.080 [10]
     *
     * @param analyseString
     * @return bool
     */
    bool IsMMICode(std::string &analyseString);

private:
    /**
     * RegexMatchMmi
     *
     * 3GPP TS 22.030 V4.0.0 (2001-03)  6.5.2 Structure of the MMI
     * TS 24.080 [10]
     *
     * Regex Match Mmi Code
     *
     * @param analyseString
     * @return bool
     */
    bool RegexMatchMmi(std::string &analyseString);

    bool IsShortCode(const std::string &analyseString);

    bool SpecialMatch(const MMIData mmiData, std::string &analyseString);

    bool IsShortCodeWithCellularCall(const std::string &analyseString);

    bool IsShortCodeWithoutCellularCall(const std::string &analyseString);

private:
    MMIData mmiData_;
};
} // namespace Telephony
} // namespace OHOS

#endif // MMI_CODE_UTILS_H
