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

#include "incoming_call_notification.h"

#include "notification_normal_content.h"
#include "notification_helper.h"
#include "notification_content.h"
#include "notification_request.h"
#include "common_event_support.h"
#include "common_event_manager.h"
#include "common_event.h"
#include "want.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
IncomingCallNotification::IncomingCallNotification() : incomingCallNumber_("") {}

void IncomingCallNotification::NewCallCreated(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr != nullptr && callObjectPtr->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING &&
        !callObjectPtr->GetAccountNumber().empty()) {
        incomingCallNumber_ = callObjectPtr->GetAccountNumber();
        PublishIncomingCallNotification(callObjectPtr);
    } else {
        incomingCallNumber_ = "";
    }
}

void IncomingCallNotification::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr != nullptr && priorState == TelCallState::CALL_STATUS_INCOMING &&
        callObjectPtr->GetAccountNumber() == incomingCallNumber_) {
        CancelIncomingCallNotification();
    }
}

void IncomingCallNotification::IncomingCallActivated(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr != nullptr && callObjectPtr->GetAccountNumber() == incomingCallNumber_) {
        CancelIncomingCallNotification();
    }
}

void IncomingCallNotification::IncomingCallHungUp(
    sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{
    if (callObjectPtr != nullptr && callObjectPtr->GetAccountNumber() == incomingCallNumber_) {
        CancelIncomingCallNotification();
    }
}

void IncomingCallNotification::CallDestroyed(sptr<CallBase> &callObjectPtr) {}

void IncomingCallNotification::PublishIncomingCallNotification(sptr<CallBase> &callObjectPtr)
{
    // show the incoming call notification while full screen
    if (!IsFullScreen()) {
        TELEPHONY_LOGE("not full screen");
        return;
    }
    std::shared_ptr<Notification::NotificationNormalContent> normalContent =
        std::make_shared<Notification::NotificationNormalContent>();
    if (normalContent == nullptr) {
        TELEPHONY_LOGE("notification normal content nullptr");
        return;
    }
    normalContent->SetTitle(INCOMING_CALL_NOTIFICATION_TITLE);
    normalContent->SetText(callObjectPtr->GetAccountNumber());
    std::shared_ptr<Notification::NotificationContent> content =
        std::make_shared<Notification::NotificationContent>(normalContent);
    if (content == nullptr) {
        TELEPHONY_LOGE("notification content nullptr");
        return;
    }
    Notification::NotificationRequest request;
    request.SetContent(content);
    request.SetNotificationId(INCOMING_CALL_NOTIFICATION_ID);
    int32_t result = Notification::NotificationHelper::PublishNotification(request);
    TELEPHONY_LOGI("publish incoming call notification result : %{public}d", result);
}

int32_t IncomingCallNotification::CancelIncomingCallNotification()
{
#ifdef ABILITY_NOTIFICATION_SUPPORT
    return NotificationHelper::CancelNotification(INCOMING_CALL_NOTIFICATION_ID);
#endif
    return TELEPHONY_SUCCESS;
}

bool IncomingCallNotification::IsFullScreen()
{
#ifdef ABILITY_DISPLAY_SUPPORT
    return DisplayManager::IsFullScreen();
#endif
    return false;
}
} // namespace Telephony
} // namespace OHOS