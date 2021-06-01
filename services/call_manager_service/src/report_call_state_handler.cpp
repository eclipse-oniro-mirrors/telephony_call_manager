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

#include "report_call_state_handler.h"

#include <string_ex.h>

#include "call_manager_errors.h"
#include "call_manager_log.h"

namespace OHOS {
namespace TelephonyCallManager {
ReportCallStateHandler::ReportCallStateHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : AppExecFwk::EventHandler(runner),
      reportCallStateClientPtr_(DelayedSingleton<CallStateReportProxy>::GetInstance())
{
    CALLMANAGER_DEBUG_LOG("ReportCallStateHandler::ReportCallStateHandler constructed.");
}

ReportCallStateHandler::~ReportCallStateHandler()
{
    if (reportCallStateClientPtr_ != nullptr) {
        reportCallStateClientPtr_ = nullptr;
    }
}

void ReportCallStateHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        CALLMANAGER_ERR_LOG("ReportCallStateHandler::ProcessEvent parameter error");
        return;
    }
    CALLMANAGER_DEBUG_LOG("ProcessEvent inner event id obtained: %{public}u.", event->GetInnerEventId());
    switch (event->GetInnerEventId()) {
        case ReportCallStateHandlerService::HANDLER_REPORT_CALL_STATE: {
            auto object = event->GetUniqueObject<CallInfo>();
            if (object == nullptr) {
                CALLMANAGER_ERR_LOG("object is nullptr!");
                return;
            }
            int32_t callState = (int32_t)(*object).state;
            std::string str((*object).phoneNum, strlen((*object).phoneNum));
            std::u16string phoneNumber = Str8ToStr16(str);
            if (reportCallStateClientPtr_ == nullptr) {
                CALLMANAGER_ERR_LOG("reportCallStateClientPtr_ is nullptr!");
                return;
            }
            CALLMANAGER_DEBUG_LOG("ReportCallState");
            reportCallStateClientPtr_->ReportCallState(callState, phoneNumber);
            break;
        }
        case ReportCallStateHandlerService::HANDLER_REPORT_CALL_STATE_FOR_CALLID: {
            auto object = event->GetUniqueObject<CallInfo>();
            if (object == nullptr) {
                CALLMANAGER_ERR_LOG("object is nullptr!");
                return;
            }
            int32_t subId = (int32_t)(*object).accountId;
            int32_t callId = (int32_t)(*object).accountId;
            int32_t callState = (int32_t)(*object).state;
            std::string str((*object).phoneNum, strlen((*object).phoneNum));
            std::u16string incomingNumber = Str8ToStr16(str);
            if (reportCallStateClientPtr_ == nullptr) {
                CALLMANAGER_ERR_LOG("reportCallStateClientPtr_ is nullptr!");
                return;
            }
            CALLMANAGER_DEBUG_LOG("ReportCallStateForCallId");
            reportCallStateClientPtr_->ReportCallStateForCallId(subId, callId, callState, incomingNumber);
            break;
        }
        default: {
            CALLMANAGER_ERR_LOG("unknown data!");
            break;
        }
    }
}

ReportCallStateHandlerService::ReportCallStateHandlerService() : eventLoop_(nullptr), handler_(nullptr) {}

ReportCallStateHandlerService::~ReportCallStateHandlerService()
{
    if (eventLoop_ != nullptr) {
        eventLoop_->Stop();
        eventLoop_ = nullptr;
    }
    if (handler_ != nullptr) {
        handler_ = nullptr;
    }
}

void ReportCallStateHandlerService::Start()
{
    eventLoop_ = AppExecFwk::EventRunner::Create("ReportCallStateHandlerService");
    if (eventLoop_.get() == nullptr) {
        CALLMANAGER_ERR_LOG("failed to create EventRunner");
        return;
    }
    handler_ = std::make_shared<ReportCallStateHandler>(eventLoop_);
    if (handler_.get() == nullptr) {
        CALLMANAGER_ERR_LOG("failed to create ReportCallStateHandler");
        return;
    }
    eventLoop_->Run();
    return;
}

void ReportCallStateHandlerService::NewCallCreated(sptr<CallBase> &callObjectPtr) {}

void ReportCallStateHandlerService::CallDestroyed(sptr<CallBase> &callObjectPtr) {}

void ReportCallStateHandlerService::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallStates priorState, TelCallStates nextState)
{
    if (callObjectPtr == nullptr) {
        CALLMANAGER_ERR_LOG("callObjectPtr is nullptr!");
        return;
    }
    CallInfo info;
    callObjectPtr->GetBaseCallInfo(info);
    CALLMANAGER_DEBUG_LOG("phoneNum:%s,state:%{public}d", info.phoneNum, info.state);
    if (info.state == TelCallStates::CALL_STATUS_INCOMING) {
        ReportCallStateForCallId(info);
    } else {
        ReportCallState(info);
    }
}

void ReportCallStateHandlerService::IncomingCallHungUp(
    sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{}

void ReportCallStateHandlerService::IncomingCallActivated(sptr<CallBase> &callObjectPtr) {}

int32_t ReportCallStateHandlerService::ReportCallState(CallInfo &info)
{
    if (handler_.get() == nullptr) {
        CALLMANAGER_ERR_LOG("failed to create EventRunner");
        return TELEPHONY_FAIL;
    }
    std::unique_ptr<CallInfo> para = std::make_unique<CallInfo>();
    if (para.get() == nullptr) {
        CALLMANAGER_ERR_LOG("make_unique CallInfo failed!");
        return TELEPHONY_FAIL;
    }
    *para = info;
    std::lock_guard<std::mutex> lock(mutex_);
    handler_->SendEvent(HANDLER_REPORT_CALL_STATE, std::move(para));
    return TELEPHONY_NO_ERROR;
}

int32_t ReportCallStateHandlerService::ReportCallStateForCallId(CallInfo &info)
{
    if (handler_.get() == nullptr) {
        CALLMANAGER_ERR_LOG("failed to create EventRunner");
        return TELEPHONY_FAIL;
    }
    std::unique_ptr<CallInfo> para = std::make_unique<CallInfo>();
    if (para.get() == nullptr) {
        CALLMANAGER_ERR_LOG("make_unique CallInfo failed!");
        return TELEPHONY_FAIL;
    }
    *para = info;
    std::lock_guard<std::mutex> lock(mutex_);
    handler_->SendEvent(HANDLER_REPORT_CALL_STATE_FOR_CALLID, std::move(para));
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS
