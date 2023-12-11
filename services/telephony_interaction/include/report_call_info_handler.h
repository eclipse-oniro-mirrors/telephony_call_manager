/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef REPORT_CALL_INFO_HANDLER_H
#define REPORT_CALL_INFO_HANDLER_H

#include <memory>

#include "call_status_manager.h"
#include "mutex"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
class ReportCallInfoHandler : public std::enable_shared_from_this<ReportCallInfoHandler> {
    DECLARE_DELAYED_SINGLETON(ReportCallInfoHandler)
public:
    void Init();
    int32_t UpdateCallReportInfo(const CallDetailInfo &info);
    int32_t UpdateCallsReportInfo(CallDetailsInfo &info);
    int32_t UpdateDisconnectedCause(const DisconnectedDetails &details);
    int32_t UpdateEventResultInfo(const CellularCallEventInfo &info);
    int32_t UpdateOttEventInfo(const OttCallEventInfo &info);
    int32_t ReceiveImsCallModeRequest(const CallModeReportInfo &response);
    int32_t ReceiveImsCallModeResponse(const CallModeReportInfo &response);

private:
    template<typename Function>
    void Submit(const std::string &taskName, Function &&func);

private:
    std::shared_ptr<CallStatusManager> callStatusManagerPtr_;
    std::mutex mtx;
};
} // namespace Telephony
} // namespace OHOS

#endif // REPORT_CALL_INFO_HANDLER_H
