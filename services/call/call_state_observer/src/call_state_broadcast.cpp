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

#include "common_event.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
void CallStateBroadcast::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object ptr nullptr");
        return;
    }
    PublishCallStateEvent(callObjectPtr, static_cast<int32_t>(priorState), static_cast<int32_t>(nextState));
}

void CallStateBroadcast::PublishCallStateEvent(sptr<CallBase> &callObjectPtr, int32_t priorState, int32_t nextState)
{
    AAFwk::Want want;
    want.SetParam("callId", callObjectPtr->GetCallID());
    want.SetParam("priorState", priorState);
    want.SetParam("nextState", nextState);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_CALL_STATE_UPDATED);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    data.SetCode(CALL_STATE_UPDATED_CODE);
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool result = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGI("publish call state event result : %{public}d", result);

    AAFwk::Want wantWithNumber = want;
    wantWithNumber.SetParam("phoneNumber", callObjectPtr->GetAccountNumber());
    EventFwk::CommonEventData dataWithNumber;
    dataWithNumber.SetWant(wantWithNumber);
    dataWithNumber.SetCode(CALL_STATE_UPDATED_CODE);
    std::vector<std::string> callPermissions;
    callPermissions.emplace_back(Permission::GET_TELEPHONY_STATE);
    publishInfo.SetSubscriberPermissions(callPermissions);
    bool resultWithNumber = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGI("publish call state event with number result : %{public}d", resultWithNumber);
}

void CallStateBroadcast::NewCallCreated(sptr<CallBase> &callObjectPtr) {}

void CallStateBroadcast::CallDestroyed(int32_t cause) {}

void CallStateBroadcast::IncomingCallActivated(sptr<CallBase> &callObjectPtr) {}

void CallStateBroadcast::IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) {}
} // namespace Telephony
} // namespace OHOS
