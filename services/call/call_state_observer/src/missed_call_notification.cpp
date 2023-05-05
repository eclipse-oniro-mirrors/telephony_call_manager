/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "missed_call_notification.h"

#include "call_manager_errors.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "notification_content.h"
#include "notification_helper.h"
#include "notification_normal_content.h"
#include "notification_request.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
MissedCallNotification::MissedCallNotification() : isIncomingCallMissed_(true), incomingCallNumber_("") {}

void MissedCallNotification::NewCallCreated(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr != nullptr && callObjectPtr->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING &&
        !callObjectPtr->GetAccountNumber().empty()) {
        incomingCallNumber_ = callObjectPtr->GetAccountNumber();
    } else {
        incomingCallNumber_ = "";
    }
    isIncomingCallMissed_ = true;
}

void MissedCallNotification::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr != nullptr && nextState == TelCallState::CALL_STATUS_DISCONNECTED &&
        callObjectPtr->GetAccountNumber() == incomingCallNumber_ && isIncomingCallMissed_) {
        PublishMissedCallEvent(callObjectPtr);
        PublishMissedCallNotification(callObjectPtr);
    }
}

void MissedCallNotification::PublishMissedCallEvent(sptr<CallBase> &callObjectPtr)
{
    AAFwk::Want want;
    want.SetParam("callId", callObjectPtr->GetCallID());
    want.SetParam("notificationId", INCOMING_CALL_MISSED_ID);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_INCOMING_CALL_MISSED);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    data.SetCode(INCOMING_CALL_MISSED_CODE);
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool result = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGI("publish missed call event result : %{public}d", result);

    AAFwk::Want wantWithNumber = want;
    wantWithNumber.SetParam("phoneNumber", callObjectPtr->GetAccountNumber());
    EventFwk::CommonEventData dataWithNumber;
    dataWithNumber.SetWant(wantWithNumber);
    dataWithNumber.SetCode(INCOMING_CALL_MISSED_CODE);
    dataWithNumber.SetData(callObjectPtr->GetAccountNumber());
    std::vector<std::string> callPermissions;
    callPermissions.emplace_back(Permission::GET_TELEPHONY_STATE);
    publishInfo.SetSubscriberPermissions(callPermissions);
    bool resultWithNumber = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGI("publish missed call event with number result : %{public}d", resultWithNumber);
}

void MissedCallNotification::PublishMissedCallNotification(sptr<CallBase> &callObjectPtr)
{
    std::shared_ptr<Notification::NotificationNormalContent> normalContent =
        std::make_shared<Notification::NotificationNormalContent>();
    if (normalContent == nullptr) {
        TELEPHONY_LOGE("notification normal content nullptr");
        return;
    }
    normalContent->SetTitle(INCOMING_CALL_MISSED_TITLE);
    normalContent->SetText(callObjectPtr->GetAccountNumber());
    std::shared_ptr<Notification::NotificationContent> content =
        std::make_shared<Notification::NotificationContent>(normalContent);
    if (content == nullptr) {
        TELEPHONY_LOGE("notification content nullptr");
        return;
    }
    Notification::NotificationRequest request;
    request.SetContent(content);
    request.SetNotificationId(INCOMING_CALL_MISSED_ID);
    int32_t result = Notification::NotificationHelper::PublishNotification(request);
    TELEPHONY_LOGI("publish missed call notification result : %{public}d", result);
}

int32_t MissedCallNotification::CancelMissedCallsNotification(int32_t id)
{
#ifdef ABILITY_NOTIFICATION_SUPPORT
    return NotificationHelper::CancelNotification(id);
#endif
    return TELEPHONY_SUCCESS;
}

int32_t MissedCallNotification::NotifyUnReadMissedCall(std::map<std::string, int32_t> &phoneNumAndUnreadCountMap)
{
    AAFwk::Want want;
    want.SetParam("notificationId", INCOMING_CALL_MISSED_ID);
    std::vector<std::string> phoneNumberList;
    std::vector<int32_t> countList;
    for (auto it = phoneNumAndUnreadCountMap.begin(); it != phoneNumAndUnreadCountMap.end(); it++) {
        phoneNumberList.push_back(it->first);
        countList.push_back(it->second);
    }
    want.SetParam("phoneNumberList", phoneNumberList);
    want.SetParam("countList", countList);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_INCOMING_CALL_MISSED);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    EventFwk::CommonEventPublishInfo publishInfo;
    std::vector<std::string> callPermissions;
    callPermissions.emplace_back(Permission::GET_TELEPHONY_STATE);
    publishInfo.SetSubscriberPermissions(callPermissions);
    bool resultWithNumber = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGI("publish unread missed call event with number result : %{public}d", resultWithNumber);
    if (!resultWithNumber) {
        TELEPHONY_LOGE("publish unread missed call event with number error");
        return TELEPHONY_ERR_PUBLISH_BROADCAST_FAIL;
    }
    return TELEPHONY_ERR_SUCCESS;
}

void MissedCallNotification::IncomingCallActivated(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr != nullptr && callObjectPtr->GetAccountNumber() == incomingCallNumber_) {
        isIncomingCallMissed_ = false;
    }
}

void MissedCallNotification::IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{
    if (callObjectPtr != nullptr && callObjectPtr->GetAccountNumber() == incomingCallNumber_) {
        isIncomingCallMissed_ = false;
    }
}

void MissedCallNotification::CallDestroyed(const DisconnectedDetails &details) {}
} // namespace Telephony
} // namespace OHOS
