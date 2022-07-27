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

#ifndef COMMON_EVENT_SUBSCRIBER_TEST_H
#define COMMON_EVENT_SUBSCRIBER_TEST_H

#include "common_event.h"
#include "common_event_manager.h"

namespace OHOS {
namespace Telephony {
class CommonEventSubscriberTest : public OHOS::EventFwk::CommonEventSubscriber {
public:
    explicit CommonEventSubscriberTest(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo);
    ~CommonEventSubscriberTest() = default;
    virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data);
};
} // namespace Telephony
} // namespace OHOS
#endif
