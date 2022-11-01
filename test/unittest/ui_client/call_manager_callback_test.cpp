/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
constexpr int16_t RETURN_SUCCESS = 0;

int32_t CallManagerCallbackTest::OnCallDetailsChange(const CallAttributeInfo &info)
{
    std::cout << "----------OnCallDetailsChange--------" << std::endl
              << "callId:" << info.callId << std::endl
              << "callType:" << (int32_t)info.callType << std::endl
              << "callState:" << (int32_t)info.callState << std::endl
              << "conferenceState:" << (int32_t)info.conferenceState << std::endl
              << "accountNumber:" << info.accountNumber << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnCallEventChange(const CallEventInfo &info)
{
    std::cout << "----------OnCallEventChange--------" << std::endl
              << "eventId:" << (int32_t)info.eventId << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnCallDisconnectedCause(const DisconnectedDetails &details)
{
    std::cout << "----------OnCallDisconnectedCause--------" << std::endl
              << "cause:" << (int32_t)(details.reason) << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    std::cout << "----------OnReportAsyncResults--------" << std::endl
              << "reportId:" << (int32_t)reportId << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    std::cout << "----------OnOttCallRequest--------" << std::endl
              << "requestId:" << (int32_t)requestId << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnReportMmiCodeResult(const MmiCodeInfo &info)
{
    std::cout << "----------OnReportMmiCodeResult--------" << std::endl
              << "result:" << info.result << std::endl
              << "message:" << info.message << std::endl;
    return RETURN_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
