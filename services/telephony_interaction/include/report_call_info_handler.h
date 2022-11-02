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

#ifndef REPORT_CALL_INFO_HANDLER_H
#define REPORT_CALL_INFO_HANDLER_H

#include <memory>
#include <mutex>

#include "event_handler.h"
#include "event_runner.h"
#include "singleton.h"

#include "call_status_manager.h"

namespace OHOS {
namespace Telephony {
class ReportCallInfoHandler : public AppExecFwk::EventHandler {
public:
    ReportCallInfoHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
    virtual ~ReportCallInfoHandler();
    void Init();
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event);

private:
    using CallManagerServiceFunc = void (ReportCallInfoHandler::*)(const AppExecFwk::InnerEvent::Pointer &event);
    void ReportCallInfo(const AppExecFwk::InnerEvent::Pointer &event);
    void ReportCallsInfo(const AppExecFwk::InnerEvent::Pointer &event);
    void ReportDisconnectedCause(const AppExecFwk::InnerEvent::Pointer &event);
    void ReportEventInfo(const AppExecFwk::InnerEvent::Pointer &event);
    void ReportOttEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void OnUpdateMediaModeResponse(const AppExecFwk::InnerEvent::Pointer &event);
    std::map<uint32_t, CallManagerServiceFunc> memberFuncMap_;
    std::unique_ptr<CallStatusManager> callStatusManagerPtr_;
};

class ReportCallInfoHandlerService : public std::enable_shared_from_this<ReportCallInfoHandlerService> {
    DECLARE_DELAYED_SINGLETON(ReportCallInfoHandlerService)
public:
    void Start();
    int32_t UpdateCallReportInfo(const CallDetailInfo &info);
    int32_t UpdateCallsReportInfo(CallDetailsInfo &info);
    int32_t UpdateDisconnectedCause(const DisconnectedDetails &details);
    int32_t UpdateEventResultInfo(const CellularCallEventInfo &info);
    int32_t UpdateOttEventInfo(const OttCallEventInfo &info);
    int32_t UpdateMediaModeResponse(const CallMediaModeResponse &response);

    enum {
        HANDLER_UPDATE_CELLULAR_CALL_INFO = 0,
        HANDLER_UPDATE_CALL_INFO_LIST,
        HANDLER_UPDATE_DISCONNECTED_CAUSE,
        HANDLER_UPDATE_CELLULAR_EVENT_RESULT_INFO,
        HANDLER_UPDATE_OTT_EVENT_RESULT_INFO,
        HANDLE_UPDATE_MEDIA_MODE_RESPONSE,
    };

private:
    std::shared_ptr<AppExecFwk::EventRunner> eventLoop_;
    std::shared_ptr<ReportCallInfoHandler> handler_;
};
} // namespace Telephony
} // namespace OHOS

#endif // REPORT_CALL_INFO_HANDLER_H
