/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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
#include "ffrt.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallRequestHandler::CallRequestHandler() {}

CallRequestHandler::~CallRequestHandler() {}

void CallRequestHandler::Init()
{
    callRequestProcessPtr_ = std::make_shared<CallRequestProcess>();
    return;
}

int32_t CallRequestHandler::DialCall()
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return callRequestProcessPtr_->DialRequest();
}

int32_t CallRequestHandler::AnswerCall(int32_t callId, int32_t videoState)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { callRequestProcessPtr_->AnswerRequest(callId, videoState); });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::RejectCall(int32_t callId, bool isSendSms, std::string &content)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() {
        std::string mContent = content;
        callRequestProcessPtr_->RejectRequest(callId, isSendSms, mContent);
    });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::HangUpCall(int32_t callId)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { callRequestProcessPtr_->HangUpRequest(callId); });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::HoldCall(int32_t callId)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { callRequestProcessPtr_->HoldRequest(callId); });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::UnHoldCall(int32_t callId)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { callRequestProcessPtr_->UnHoldRequest(callId); });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::SwitchCall(int32_t callId)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { callRequestProcessPtr_->SwitchRequest(callId); });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::StartRtt(int32_t callId, std::u16string &msg)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() {
        std::u16string mMsg = msg;
        callRequestProcessPtr_->StartRttRequest(callId, mMsg);
    });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::StopRtt(int32_t callId)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { callRequestProcessPtr_->StopRttRequest(callId); });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::JoinConference(int32_t callId, std::vector<std::string> &numberList)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() {
        std::vector<std::string> mNumberList(numberList);
        callRequestProcessPtr_->JoinConference(callId, mNumberList);
    });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::CombineConference(int32_t mainCallId)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { callRequestProcessPtr_->CombineConferenceRequest(mainCallId); });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::SeparateConference(int32_t callId)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { callRequestProcessPtr_->SeparateConferenceRequest(callId); });
    return TELEPHONY_SUCCESS;
}

int32_t CallRequestHandler::KickOutFromConference(int32_t callId)
{
    if (callRequestProcessPtr_ == nullptr) {
        TELEPHONY_LOGE("callRequestProcessPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { callRequestProcessPtr_->KickOutFromConferenceRequest(callId); });
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
