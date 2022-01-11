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

#include "report_call_info_handler.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "ims_call.h"

namespace OHOS {
namespace Telephony {
ReportCallInfoHandler::ReportCallInfoHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : AppExecFwk::EventHandler(runner)
{
    memberFuncMap_[ReportCallInfoHandlerService::HANDLER_UPDATE_CELLULAR_CALL_INFO] =
        &ReportCallInfoHandler::ReportCallInfo;
    memberFuncMap_[ReportCallInfoHandlerService::HANDLER_UPDATE_CALL_INFO_LIST] =
        &ReportCallInfoHandler::ReportCallsInfo;
    memberFuncMap_[ReportCallInfoHandlerService::HANDLER_UPDATE_DISCONNECTED_CAUSE] =
        &ReportCallInfoHandler::ReportDisconnectedCause;
    memberFuncMap_[ReportCallInfoHandlerService::HANDLER_UPDATE_CELLULAR_EVENT_RESULT_INFO] =
        &ReportCallInfoHandler::ReportEventInfo;
    memberFuncMap_[ReportCallInfoHandlerService::HANDLE_UPDATE_MEDIA_MODE_RESPONSE] =
        &ReportCallInfoHandler::OnUpdateMediaModeResponse;
}

ReportCallInfoHandler::~ReportCallInfoHandler()
{
    if (callStatusManagerPtr_) {
        callStatusManagerPtr_ = nullptr;
    }
}

void ReportCallInfoHandler::Init()
{
    callStatusManagerPtr_ = std::make_unique<CallStatusManager>();
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is null");
        return;
    }
    callStatusManagerPtr_->Init();
}

void ReportCallInfoHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("ReportCallInfoHandler::ProcessEvent parameter error");
        return;
    }
    TELEPHONY_LOGI("ReportCallInfoHandler::ProcessEvent inner event id: %{public}u.", event->GetInnerEventId());
    auto itFunc = memberFuncMap_.find(event->GetInnerEventId());
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(event);
        }
    }
}

void ReportCallInfoHandler::ReportCallInfo(const AppExecFwk::InnerEvent::Pointer &event)
{
    auto object = event->GetUniqueObject<CallDetailInfo>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    CallDetailInfo info = *object;
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is nullptr");
        return;
    }
    int32_t ret = callStatusManagerPtr_->HandleCallReportInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HandleCallReportInfo failed! ret:%{public}d", ret);
        return;
    }
    TELEPHONY_LOGI("HandleCallReportInfo success!");
}

void ReportCallInfoHandler::ReportCallsInfo(const AppExecFwk::InnerEvent::Pointer &event)
{
    auto object = event->GetUniqueObject<CallDetailsInfo>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    CallDetailsInfo info = *object;
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is nullptr");
        return;
    }
    int32_t ret = callStatusManagerPtr_->HandleCallsReportInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HandleCallsReportInfo failed! ret:%{public}d", ret);
        return;
    }
    TELEPHONY_LOGI("HandleCallsReportInfo success!");
}

void ReportCallInfoHandler::ReportDisconnectedCause(const AppExecFwk::InnerEvent::Pointer &event)
{
    auto object = event->GetUniqueObject<int32_t>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    int32_t cause = *object;
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is nullptr");
        return;
    }
    int32_t ret = callStatusManagerPtr_->HandleDisconnectedCause(cause);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HandleEventResultReportInfo failed! ret:%{public}d", ret);
        return;
    }
    TELEPHONY_LOGI("HandleEventResultReportInfo success!");
}

void ReportCallInfoHandler::ReportEventInfo(const AppExecFwk::InnerEvent::Pointer &event)
{
    auto object = event->GetUniqueObject<CellularCallEventInfo>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    CellularCallEventInfo info = *object;
    if (callStatusManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callStatusManagerPtr_ is nullptr");
        return;
    }
    int32_t ret = callStatusManagerPtr_->HandleEventResultReportInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("HandleEventResultReportInfo failed! ret:%{public}d", ret);
        return;
    }
    TELEPHONY_LOGI("HandleEventResultReportInfo success!");
}

void ReportCallInfoHandler::OnUpdateMediaModeResponse(const AppExecFwk::InnerEvent::Pointer &event)
{
    auto object = event->GetUniqueObject<CallMediaModeResponse>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    std::string phoneNumber(object->phoneNum);
    sptr<CallBase> call = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    if (call == nullptr) {
        TELEPHONY_LOGE("call not exists");
        return;
    }
    if (call->GetCallType() == CallType::TYPE_IMS) {
        sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(call.GetRefPtr());
        imsCall->ReceiveUpdateCallMediaModeResponse(*object);
    }
    return;
}

ReportCallInfoHandlerService::ReportCallInfoHandlerService() : eventLoop_(nullptr), handler_(nullptr) {}

ReportCallInfoHandlerService::~ReportCallInfoHandlerService()
{
    if (eventLoop_ != nullptr) {
        eventLoop_->Stop();
        eventLoop_ = nullptr;
    }
    if (handler_ != nullptr) {
        handler_ = nullptr;
    }
}

void ReportCallInfoHandlerService::Start()
{
    eventLoop_ = AppExecFwk::EventRunner::Create("ReportCallInfoHandlerService");
    if (eventLoop_.get() == nullptr) {
        TELEPHONY_LOGE("failed to create EventRunner");
        return;
    }
    handler_ = std::make_shared<ReportCallInfoHandler>(eventLoop_);
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("failed to create ReportCallInfoHandler");
        return;
    }
    handler_->Init();
    eventLoop_->Run();
    return;
}

int32_t ReportCallInfoHandlerService::UpdateCallReportInfo(const CallDetailInfo &info)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<CallDetailInfo> para = std::make_unique<CallDetailInfo>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique CallDetailInfo failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *para = info;
    bool ret = handler_->SendEvent(HANDLER_UPDATE_CELLULAR_CALL_INFO, std::move(para));
    if (!ret) {
        TELEPHONY_LOGE("SendEvent failed! status update failed, state:%{public}d", info.state);
    }
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandlerService::UpdateCallsReportInfo(CallDetailsInfo &info)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<CallDetailsInfo> para = std::make_unique<CallDetailsInfo>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique CallDetailsInfo failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *para = info;
    bool ret = handler_->SendEvent(HANDLER_UPDATE_CALL_INFO_LIST, std::move(para));
    if (!ret) {
        TELEPHONY_LOGE("SendEvent failed! status update failed, slotId:%{public}d", info.slotId);
    }
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandlerService::UpdateDisconnectedCause(const DisconnectedDetails &cause)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<int32_t> para = std::make_unique<int32_t>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique int32_t failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *para = static_cast<int32_t>(cause);
    bool ret = handler_->SendEvent(HANDLER_UPDATE_DISCONNECTED_CAUSE, std::move(para));
    if (!ret) {
        TELEPHONY_LOGE("SendEvent failed! DisconnectedDetails:%{public}d", cause);
    }
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandlerService::UpdateEventResultInfo(const CellularCallEventInfo &info)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<CellularCallEventInfo> para = std::make_unique<CellularCallEventInfo>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique CellularCallEventInfo failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *para = info;
    bool ret = handler_->SendEvent(HANDLER_UPDATE_CELLULAR_EVENT_RESULT_INFO, std::move(para));
    if (!ret) {
        TELEPHONY_LOGE("SendEvent failed! eventType:%{public}d, eventId:%{public}d", info.eventType, info.eventId);
    }
    return TELEPHONY_SUCCESS;
}

int32_t ReportCallInfoHandlerService::UpdateMediaModeResponse(const CallMediaModeResponse &response)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<CallMediaModeResponse> para = std::make_unique<CallMediaModeResponse>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique CellularCallEventInfo failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    (void)memcpy_s(para->phoneNum, kMaxNumberLen, 0, kMaxNumberLen);
    para->result = response.result;
    bool ret = handler_->SendEvent(HANDLE_UPDATE_MEDIA_MODE_RESPONSE, std::move(para));
    if (!ret) {
        TELEPHONY_LOGE("SendEvent failed! errno: %{public}d", ret);
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
