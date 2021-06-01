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

#ifndef REPORT_CALL_STATE_HANDLER_H
#define REPORT_CALL_STATE_HANDLER_H
#include <memory>
#include <mutex>

#include "event_handler.h"
#include "event_runner.h"
#include "singleton.h"

#include "call_state_listener_base.h"
#include "common_type.h"
#include "call_state_report_proxy.h"

namespace OHOS {
namespace TelephonyCallManager {
class ReportCallStateHandler : public AppExecFwk::EventHandler {
public:
    ReportCallStateHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
    virtual ~ReportCallStateHandler();

    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event);

private:
    std::shared_ptr<CallStateReportProxy> reportCallStateClientPtr_;
};

class ReportCallStateHandlerService : public CallStateListenerBase {
public:
    ReportCallStateHandlerService();
    ~ReportCallStateHandlerService();
    void Start();

    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(sptr<CallBase> &callObjectPtr) override;
    void CallStateUpdated(
        sptr<CallBase> &callObjectPtr, TelCallStates priorState, TelCallStates nextState) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;
    int32_t ReportCallState(CallInfo &info);
    int32_t ReportCallStateForCallId(CallInfo &info);

    enum {
        HANDLER_REPORT_CALL_STATE = 0,
        HANDLER_REPORT_CALL_STATE_FOR_CALLID,
    };

private:
    std::shared_ptr<AppExecFwk::EventRunner> eventLoop_;
    std::shared_ptr<ReportCallStateHandler> handler_;
    std::mutex mutex_;
};
} // namespace TelephonyCallManager
} // namespace OHOS
#endif // CALL_MANAGER_REPORT_CALL_STATE_HANDLER_H
