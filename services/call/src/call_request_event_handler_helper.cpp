/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "call_request_event_handler_helper.h"

#include <thread>

#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
namespace OHOS {
namespace Telephony {

const std::string TASK_ID = "handler_restore_dialing_flag";
const int32_t DELAY_TIME = 3000;

CallRequestEventHandlerHelper::CallRequestEventHandlerHelper()
{
    auto runner = AppExecFwk::EventRunner::Create(TASK_ID);
    if (callRequestEventHandler_ == nullptr) {
        callRequestEventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }
    if (callRequestEventHandler_ == nullptr) {
        TELEPHONY_LOGE("init call request event handler failed");
    }
}

CallRequestEventHandlerHelper::~CallRequestEventHandlerHelper() {}

int32_t CallRequestEventHandlerHelper::SetDialingCallProcessing()
{
    TELEPHONY_LOGI("start restore dialing flag task");
    if (IsDialingCallProcessing()) {
        auto task = [this]() {
            RestoreDialingFlag(false);
        };
        callRequestEventHandler_->RemoveTask(TASK_ID);
        bool ret = callRequestEventHandler_->PostTask(task, TASK_ID, DELAY_TIME);
        if (ret) {
            TELEPHONY_LOGE("restore dialing flag task failed");
            return TELEPHONY_ERROR;
        }
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERROR;
}

void CallRequestEventHandlerHelper::RemoveEventHandlerTask()
{
    if (callRequestEventHandler_ != nullptr) {
        callRequestEventHandler_->RemoveTask(TASK_ID);
    }
}

void CallRequestEventHandlerHelper::RestoreDialingFlag(bool isDialingCallProcessing)
{
    if (isDialingCallProcessing != isDialingCallProcessing_) {
        isDialingCallProcessing_ = isDialingCallProcessing;
    }
    TELEPHONY_LOGI("isDialingCallProcessing = %{public}d.", isDialingCallProcessing_);
}

bool CallRequestEventHandlerHelper::IsDialingCallProcessing()
{
    return isDialingCallProcessing_;
}

void CallRequestEventHandlerHelper::SetPendingMo(bool pendingMo, int32_t callId)
{
    pendingMo_ = pendingMo;
    pendingMoCallId_ = callId;
}

bool CallRequestEventHandlerHelper::HasPendingMo(int32_t callId)
{
    return pendingMo_ && (pendingMoCallId_ == callId);
}

void CallRequestEventHandlerHelper::SetPendingHangup(bool pendingHangup, int32_t callId)
{
    pendingHangup_ = pendingHangup;
    pendingHangupCallId_ = callId;
}

bool CallRequestEventHandlerHelper::HasPendingHangp(int32_t callId)
{
    return pendingHangup_ && (pendingHangupCallId_ == callId);
}

} // namespace Telephony
} // namespace OHOS
