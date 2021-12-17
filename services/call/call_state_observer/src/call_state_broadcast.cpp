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

#include "call_state_broadcast.h"

#include "want.h"
#include "common_event_support.h"
#include "common_event.h"
#include "common_event_manager.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
void CallStateBroadcast::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object ptr nullptr");
        return;
    }
    PublishCallStateEvent(callObjectPtr, (int32_t)priorState, (int32_t)nextState);
}

void CallStateBroadcast::PublishCallStateEvent(sptr<CallBase> &callObjectPtr, int32_t priorState, int32_t nextState)
{
    AAFwk::Want want;
    want.SetParam("callId", callObjectPtr->GetCallID());
    want.SetParam("priorState", priorState);
    want.SetParam("nextState", nextState);
    want.SetParam("phoneNumber", callObjectPtr->GetAccountNumber());
#ifdef ABILITY_NOTIFICATION_SUPPORT
    want.SetAction(CommonEventSupport::COMMON_EVENT_CALL_STATE_UPDATED);
#endif
#ifndef ABILITY_NOTIFICATION_SUPPORT
    want.SetAction(COMMON_EVENT_CALL_STATE_UPDATED);
#endif
    EventFwk::CommonEventData data;
    data.SetWant(want);
    data.SetCode(CALL_STATE_UPDATED_CODE);
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool result = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGI("publish call state event result : %{public}d", result);
}

void CallStateBroadcast::NewCallCreated(sptr<CallBase> &callObjectPtr) {}

void CallStateBroadcast::CallDestroyed(sptr<CallBase> &callObjectPtr) {}

void CallStateBroadcast::IncomingCallActivated(sptr<CallBase> &callObjectPtr) {}

void CallStateBroadcast::IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) {}
} // namespace Telephony
} // namespace OHOS
