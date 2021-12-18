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

#include "call_manager_callback_test.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t RETURN_SUCCESS = 0;

int32_t CallManagerCallbackTest::OnCallDetailsChange(const CallAttributeInfo &info)
{
    std::cout << "----------OnCallDetailsChange--------" << std::endl
              << "callId:" << info.callId << std::endl
              << "callType:" << info.callType << std::endl
              << "callState:" << info.callState << std::endl
              << "conferenceState:" << info.conferenceState << std::endl
              << "accountNumber:" << info.accountNumber << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnCallEventChange(const CallEventInfo &info)
{
    std::cout << "----------OnCallEventChange--------" << std::endl << "eventId:" << info.eventId << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnSupplementResult(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    std::cout << "----------OnSupplementResult--------" << std::endl << "reportId:" << reportId << std::endl;
    return RETURN_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
