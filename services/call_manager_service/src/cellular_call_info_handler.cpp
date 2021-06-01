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

#include "cellular_call_info_handler.h"

#include "call_manager_errors.h"
#include "call_manager_log.h"

namespace OHOS {
namespace TelephonyCallManager {
CellularCallInfoHandler::CellularCallInfoHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : AppExecFwk::EventHandler(runner)
{
    CALLMANAGER_DEBUG_LOG("CellularCallInfoHandler constructed.");
}

CellularCallInfoHandler::~CellularCallInfoHandler()
{
    if (callStatusManagerPtr_) {
        callStatusManagerPtr_ = nullptr;
    }
}

void CellularCallInfoHandler::Init()
{
    callStatusManagerPtr_ = std::make_unique<CallStatusManager>();
    if (callStatusManagerPtr_ == nullptr) {
        CALLMANAGER_ERR_LOG("callStatusManagerPtr_ is null");
        return;
    }
    callStatusManagerPtr_->Init();
}

void CellularCallInfoHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        CALLMANAGER_ERR_LOG("CellularCallInfoHandler::ProcessEvent parameter error");
        return;
    }

    CALLMANAGER_DEBUG_LOG(
        "CellularCallInfoHandler::ProcessEvent inner event id obtained: %{public}u.", event->GetInnerEventId());
    switch (event->GetInnerEventId()) {
        case CellularCallInfoHandlerService::HANDLER_UPDATE_CELLULAR_CALL_INFO: {
            auto object = event->GetUniqueObject<CallReportInfo>();
            if (object == nullptr) {
                CALLMANAGER_ERR_LOG("object is nullptr!");
                break;
            }
            CallReportInfo info = *object;
            if (callStatusManagerPtr_ == nullptr) {
                CALLMANAGER_ERR_LOG("callStatusManagerPtr_ is nullprt");
                return;
            }
            callStatusManagerPtr_->HandleCallReportInfo(info);
            break;
        }
        case CellularCallInfoHandlerService::HANDLER_UPDATE_CELLULAR_CS_CALL_INFO: {
            auto object = event->GetUniqueObject<CallsReportInfo>();
            if (object == nullptr) {
                CALLMANAGER_ERR_LOG("object is nullptr!");
                break;
            }
            CallsReportInfo info = *object;
            if (callStatusManagerPtr_ == nullptr) {
                CALLMANAGER_ERR_LOG("callStatusManagerPtr_ is nullprt");
                return;
            }
            callStatusManagerPtr_->HandleCallsReportInfo(info);
            break;
        }
        default: {
            break;
        }
    }
}

CellularCallInfoHandlerService::CellularCallInfoHandlerService() : eventLoop_(nullptr), handler_(nullptr) {}

CellularCallInfoHandlerService::~CellularCallInfoHandlerService()
{
    if (eventLoop_ != nullptr) {
        eventLoop_->Stop();
        eventLoop_ = nullptr;
    }
    if (handler_ != nullptr) {
        handler_ = nullptr;
    }
}

void CellularCallInfoHandlerService::Start()
{
    eventLoop_ = AppExecFwk::EventRunner::Create("CellularCallInfoHandlerService");
    if (eventLoop_.get() == nullptr) {
        CALLMANAGER_ERR_LOG("failed to create EventRunner");
        return;
    }
    handler_ = std::make_shared<CellularCallInfoHandler>(eventLoop_);
    if (handler_.get() == nullptr) {
        CALLMANAGER_ERR_LOG("failed to create CellularCallInfoHandler");
        return;
    }
    handler_->Init();
    eventLoop_->Run();
    return;
}

int32_t CellularCallInfoHandlerService::UpdateCallReportInfo(const CallReportInfo &info)
{
    if (handler_.get() == nullptr) {
        CALLMANAGER_ERR_LOG("handler_ is nullptr");
        return TELEPHONY_FAIL;
    }
    std::unique_ptr<CallReportInfo> para = std::make_unique<CallReportInfo>();
    if (para.get() == nullptr) {
        CALLMANAGER_ERR_LOG("make_unique CallReportInfo failed!");
        return TELEPHONY_FAIL;
    }
    *para = info;
    std::lock_guard<std::mutex> lock(mutex_);
    handler_->SendEvent(HANDLER_UPDATE_CELLULAR_CALL_INFO, std::move(para));
    return TELEPHONY_NO_ERROR;
}

int32_t CellularCallInfoHandlerService::UpdateCallsReportInfo(const CallsReportInfo &info)
{
    if (handler_.get() == nullptr) {
        CALLMANAGER_ERR_LOG("handler_ is nullptr");
        return TELEPHONY_FAIL;
    }
    std::unique_ptr<CallsReportInfo> para = std::make_unique<CallsReportInfo>();
    if (para.get() == nullptr) {
        CALLMANAGER_ERR_LOG("make_unique CallsReportInfo failed!");
        return TELEPHONY_FAIL;
    }
    *para = info;
    std::lock_guard<std::mutex> lock(mutex_);
    handler_->SendEvent(HANDLER_UPDATE_CELLULAR_CS_CALL_INFO, std::move(para));
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS
