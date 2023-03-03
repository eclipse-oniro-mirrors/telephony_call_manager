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

#include "call_request_handler.h"

#include <securec.h>
#include <string_ex.h>

#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallRequestHandler::CallRequestHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : AppExecFwk::EventHandler(runner), callRequestProcessPtr_(nullptr)
{
    memberFuncMap_[CallRequestHandlerService::HANDLER_DIAL_CALL_REQUEST] = &CallRequestHandler::DialCallEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_ANSWER_CALL_REQUEST] = &CallRequestHandler::AcceptCallEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_REJECT_CALL_REQUEST] = &CallRequestHandler::RejectCallEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_HANGUP_CALL_REQUEST] = &CallRequestHandler::HangUpCallEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_HOLD_CALL_REQUEST] = &CallRequestHandler::HoldCallEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_UNHOLD_CALL_REQUEST] = &CallRequestHandler::UnHoldCallEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_SWAP_CALL_REQUEST] = &CallRequestHandler::SwitchCallEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_COMBINE_CONFERENCE_REQUEST] =
        &CallRequestHandler::CombineConferenceEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_SEPARATE_CONFERENCE_REQUEST] =
        &CallRequestHandler::SeparateConferenceEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_UPDATE_CALL_MEDIA_MODE_REQUEST] =
        &CallRequestHandler::UpdateCallMediaModeEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_STARTRTT_REQUEST] = &CallRequestHandler::StartRttEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_STOPRTT_REQUEST] = &CallRequestHandler::StopRttEvent;
    memberFuncMap_[CallRequestHandlerService::HANDLER_INVITE_TO_CONFERENCE] =
        &CallRequestHandler::JoinConferenceEvent;
}

CallRequestHandler::~CallRequestHandler()
{
    callRequestProcessPtr_ = nullptr;
}

void CallRequestHandler::Init()
{
    callRequestProcessPtr_ = std::make_unique<CallRequestProcess>();
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is null");
        return;
    }
}

void CallRequestHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    TELEPHONY_LOGI("CallRequestHandler inner event id obtained: %{public}u.", event->GetInnerEventId());
    auto itFunc = memberFuncMap_.find(event->GetInnerEventId());
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(event);
        }
    }
}

void CallRequestHandler::DialCallEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->DialRequest();
}

void CallRequestHandler::AcceptCallEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<AnswerCallPara>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    AnswerCallPara acceptPara = *object;
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->AnswerRequest(acceptPara.callId, acceptPara.videoState);
}

void CallRequestHandler::RejectCallEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<RejectCallPara>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    RejectCallPara rejectPara = *object;
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    std::string content(rejectPara.content);
    callRequestProcessPtr_->RejectRequest(rejectPara.callId, rejectPara.isSendSms, content);
}

void CallRequestHandler::HangUpCallEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<int32_t>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    int32_t callId = *object;
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->HangUpRequest(callId);
}

void CallRequestHandler::HoldCallEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<int32_t>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    int32_t callId = *object;
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->HoldRequest(callId);
}

void CallRequestHandler::UnHoldCallEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<int32_t>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    int32_t callId = *object;
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->UnHoldRequest(callId);
}

void CallRequestHandler::SwitchCallEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<int32_t>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    int32_t callId = *object;
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->SwitchRequest(callId);
}

void CallRequestHandler::CombineConferenceEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<int32_t>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    int32_t mainCallId = *object;
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->CombineConferenceRequest(mainCallId);
}

void CallRequestHandler::SeparateConferenceEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<int32_t>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    int32_t callId = *object;
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->SeparateConferenceRequest(callId);
}

void CallRequestHandler::UpdateCallMediaModeEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<CallMediaUpdatePara>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->UpdateCallMediaModeRequest(object->callId, object->mode);
}

void CallRequestHandler::StartRttEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<StartRttPara>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    StartRttPara rttPara = *object;
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->StartRttRequest(rttPara.callId, rttPara.msg);
}

void CallRequestHandler::StopRttEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::ProcessEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<int32_t>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    int32_t callId = *object;
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->StopRttRequest(callId);
}

void CallRequestHandler::JoinConferenceEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRequestHandler::JoinConferenceEvent parameter error");
        return;
    }
    auto object = event->GetUniqueObject<JoinConferencePara>();
    if (object == nullptr) {
        TELEPHONY_LOGE("object is nullptr!");
        return;
    }
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return;
    }
    callRequestProcessPtr_->JoinConference(object->callId, object->numberList);
}

CallRequestHandlerService::CallRequestHandlerService() : eventLoop_(nullptr), handler_(nullptr) {}

CallRequestHandlerService::~CallRequestHandlerService()
{
    if (eventLoop_ != nullptr) {
        eventLoop_->Stop();
        eventLoop_ = nullptr;
    }
    handler_ = nullptr;
}

void CallRequestHandlerService::Start()
{
    eventLoop_ = AppExecFwk::EventRunner::Create("CallRequestHandler");
    if (eventLoop_.get() == nullptr) {
        TELEPHONY_LOGE("failed to create EventRunner");
        return;
    }
    handler_ = std::make_shared<CallRequestHandler>(eventLoop_);
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("failed to create CallRequestHandler");
        return;
    }
    handler_->Init();
    eventLoop_->Run();
    return;
}

int32_t CallRequestHandlerService::DialCall()
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!handler_->SendEvent(HANDLER_DIAL_CALL_REQUEST)) {
        TELEPHONY_LOGE("send dial event failed!");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::AnswerCall(int32_t callId, int32_t videoState)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<AnswerCallPara> para = std::make_unique<AnswerCallPara>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique AnswerCallPara failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    para->callId = callId;
    para->videoState = videoState;
    if (!handler_->SendEvent(HANDLER_ANSWER_CALL_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send accept event failed!");
        CallManagerHisysevent::WriteAnswerCallFaultEvent(INVALID_PARAMETER, callId, videoState,
            CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE, "send HANDLER_ANSWER_CALL_REQUEST event failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::RejectCall(int32_t callId, bool isSendSms, std::string &content)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<RejectCallPara> para = std::make_unique<RejectCallPara>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique RejectCallPara failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    para->callId = callId;
    para->isSendSms = isSendSms;
    (void)memset_s(para->content, REJECT_CALL_MSG_MAX_LEN + 1, 0, REJECT_CALL_MSG_MAX_LEN + 1);
    errno_t result = memcpy_s(para->content, REJECT_CALL_MSG_MAX_LEN, content.c_str(), content.length());
    if (para->isSendSms && result != EOK) {
        TELEPHONY_LOGE("memcpy_s rejectCall content failed!");
        CallManagerHisysevent::WriteHangUpFaultEvent(
            INVALID_PARAMETER, callId, TELEPHONY_ERR_MEMCPY_FAIL, "Reject memcpy_s rejectCall content failed");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    if (!handler_->SendEvent(HANDLER_REJECT_CALL_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send reject event failed!");
        CallManagerHisysevent::WriteHangUpFaultEvent(INVALID_PARAMETER, callId, CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE,
            "Reject send HANDLER_REJECT_CALL_REQUEST event failed");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::HangUpCall(int32_t callId)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<int32_t> para = std::make_unique<int32_t>(callId);
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique callId failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!handler_->SendEvent(HANDLER_HANGUP_CALL_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send hung up event failed!");
        CallManagerHisysevent::WriteHangUpFaultEvent(INVALID_PARAMETER, callId,
            CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE, "HangUp send HANDLER_HANGUP_CALL_REQUEST event failed");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::HoldCall(int32_t callId)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<int32_t> para = std::make_unique<int32_t>(callId);
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique callId failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!handler_->SendEvent(HANDLER_HOLD_CALL_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send hold event failed!");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::UnHoldCall(int32_t callId)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<int32_t> para = std::make_unique<int32_t>(callId);
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique callId failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!handler_->SendEvent(HANDLER_UNHOLD_CALL_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send unHold event failed!");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::SwitchCall(int32_t callId)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<int32_t> para = std::make_unique<int32_t>(callId);
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique callId failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!handler_->SendEvent(HANDLER_SWAP_CALL_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send swap event failed!");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::StartRtt(int32_t callId, std::u16string &msg)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<StartRttPara> para = std::make_unique<StartRttPara>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique StartRttPara failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    para->callId = callId;
    para->msg = msg;
    if (!handler_->SendEvent(HANDLER_STARTRTT_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send StartRtt event failed!");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::StopRtt(int32_t callId)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<int32_t> para = std::make_unique<int32_t>(callId);
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique callId failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!handler_->SendEvent(HANDLER_STOPRTT_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send StopRtt event failed!");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::JoinConference(int32_t callId, std::vector<std::string> &numberList)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<JoinConferencePara> para = std::make_unique<JoinConferencePara>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique JoinConferencePara failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    para->callId = callId;
    para->numberList = numberList;
    if (!handler_->SendEvent(HANDLER_INVITE_TO_CONFERENCE, std::move(para))) {
        TELEPHONY_LOGE("send JoinConference event failed!");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::CombineConference(int32_t mainCallId)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<int32_t> para = std::make_unique<int32_t>(mainCallId);
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique mainCallId failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!handler_->SendEvent(HANDLER_COMBINE_CONFERENCE_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send CombineConference event failed!");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::SeparateConference(int32_t callId)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<int32_t> para = std::make_unique<int32_t>(callId);
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique callId failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!handler_->SendEvent(HANDLER_SEPARATE_CONFERENCE_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send SeparateConference event failed!");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandlerService::UpdateImsCallMode(int32_t callId, ImsCallMode mode)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<CallMediaUpdatePara> para = std::make_unique<CallMediaUpdatePara>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique CallMediaUpdatePara failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    para->callId = callId;
    para->mode = mode;
    if (!handler_->SendEvent(HANDLER_UPDATE_CALL_MEDIA_MODE_REQUEST, std::move(para))) {
        TELEPHONY_LOGE("send UpdateImsCallMode event failed!");
        return CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
