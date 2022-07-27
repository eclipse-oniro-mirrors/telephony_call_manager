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

#ifndef CALL_BROADCAST_SUBSCRIBER_H
#define CALL_BROADCAST_SUBSCRIBER_H

#include <string>
#include <vector>
#include <map>

#include "common_event.h"
#include "common_event_manager.h"
#include "common_event_support.h"

namespace OHOS {
namespace Telephony {
class CallBroadcastSubscriber : public EventFwk::CommonEventSubscriber {
public:
    explicit CallBroadcastSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo);
    ~CallBroadcastSubscriber() = default;
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data);

private:
    enum {
        UNKNOWN_BROADCAST_EVENT = 0,
        SIM_STATE_BROADCAST_EVENT,
    };
    using broadcastSubscriberFunc = void (CallBroadcastSubscriber::*)(const EventFwk::CommonEventData &data);

    void UnknownBroadcast(const EventFwk::CommonEventData &data);
    void SimStateBroadcast(const EventFwk::CommonEventData &data);
    std::map<uint32_t, broadcastSubscriberFunc> memberFuncMap_;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_BROADCAST_SUBSCRIBER_H
