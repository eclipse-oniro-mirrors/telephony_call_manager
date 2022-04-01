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

#include "common_event_subscriber_test.h"

#include "common_event_support.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr int16_t DEFAULT_MISSED_CALL_NOTIFICATION_ID = 0;

CommonEventSubscriberTest::CommonEventSubscriberTest(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{}

void CommonEventSubscriberTest::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    OHOS::EventFwk::Want want = data.GetWant();
    std::string action = want.GetAction();
    if (action == COMMON_EVENT_INCOMING_CALL_MISSED) {
        TELEPHONY_LOGI("receive action : INCOMING_CALL_MISSED");
        int32_t id = want.GetIntParam("notificationId", DEFAULT_MISSED_CALL_NOTIFICATION_ID);
        TELEPHONY_LOGI("missed call notification id : %{public}d", id);
    }
}
} // namespace Telephony
} // namespace OHOS