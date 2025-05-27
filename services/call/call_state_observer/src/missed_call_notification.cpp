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
#include "os_account_manager.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
MissedCallNotification::MissedCallNotification() {}

void MissedCallNotification::NewCallCreated(sptr<CallBase> &callObjectPtr) {}

void MissedCallNotification::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr != nullptr && callObjectPtr->GetCallType() == CallType::TYPE_VOIP) {
        TELEPHONY_LOGI("Voip call should not save  missed notification");
        return;
    }
    if (callObjectPtr != nullptr && nextState == TelCallState::CALL_STATUS_DISCONNECTED &&
        callObjectPtr->GetCallDirection() == CallDirection::CALL_DIRECTION_IN &&
        ((callObjectPtr->GetAnswerType() == CallAnswerType::CALL_ANSWER_MISSED) ||
        (callObjectPtr->GetAnswerType() == CallAnswerType::CALL_ANSWER_ACTIVED &&
        callObjectPtr->IsAiAutoAnswer()))) {
        int32_t userId = 0;
        bool isUserUnlocked = false;
        AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(userId);
        AccountSA::OsAccountManager::IsOsAccountVerified(userId, isUserUnlocked);
        TELEPHONY_LOGI("isUserUnlocked: %{public}d", isUserUnlocked);
        if (isUserUnlocked) {
            PublishMissedCallEvent(callObjectPtr);
        }
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
    TELEPHONY_LOGW("publish missed call event result : %{public}d", result);

    AAFwk::Want wantWithNumber = want;
    wantWithNumber.SetParam("phoneNumber", callObjectPtr->GetAccountNumber());
    EventFwk::CommonEventData dataWithNumber;
    dataWithNumber.SetWant(wantWithNumber);
    dataWithNumber.SetCode(INCOMING_CALL_MISSED_CODE);
    dataWithNumber.SetData(callObjectPtr->GetAccountNumber());
    std::vector<std::string> callPermissions;
    callPermissions.emplace_back(Permission::GET_TELEPHONY_STATE);
    publishInfo.SetSubscriberPermissions(callPermissions);
    bool resultWithNumber = EventFwk::CommonEventManager::PublishCommonEvent(dataWithNumber, publishInfo, nullptr);
    TELEPHONY_LOGW("publish missed call event with number result : %{public}d", resultWithNumber);
}

void MissedCallNotification::PublishBlockedCallEvent(sptr<CallBase> &callObjectPtr)
{
    AAFwk::Want want;
    want.SetParam("callId", callObjectPtr->GetCallID());
    want.SetParam("notificationId", INCOMING_CALL_MISSED_ID);
    want.SetParam("phoneNumber", callObjectPtr->GetAccountNumber());
    want.SetParam("isBlocked", true);
    std::string detectDetails = callObjectPtr->GetDetectDetails();
    want.SetParam("detectDetails", detectDetails);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_INCOMING_CALL_MISSED);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    data.SetCode(INCOMING_CALL_MISSED_CODE);
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    std::vector<std::string> callPermissions;
    callPermissions.emplace_back(Permission::GET_TELEPHONY_STATE);
    publishInfo.SetSubscriberPermissions(callPermissions);
    bool result = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGW("publish blocked call event result: %{public}d, detectDetails length: %{public}zu",
        result, detectDetails.length());
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

void MissedCallNotification::IncomingCallActivated(sptr<CallBase> &callObjectPtr) {}

void MissedCallNotification::IncomingCallHungUp(sptr<CallBase> &callObjectPtr,
    bool isSendSms, std::string content) {}

void MissedCallNotification::CallDestroyed(const DisconnectedDetails &details) {}
} // namespace Telephony
} // namespace OHOS
