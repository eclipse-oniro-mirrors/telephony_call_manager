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

#ifndef TELEPHONY_MISSED_CALL_NOTIFICATION_H
#define TELEPHONY_MISSED_CALL_NOTIFICATION_H

#include <cstdint>

#include "call_state_listener_base.h"

namespace OHOS {
namespace Telephony {
class MissedCallNotification : public CallStateListenerBase {
public:
    MissedCallNotification();
    ~MissedCallNotification() = default;
    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(const DisconnectedDetails &details) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    int32_t CancelMissedCallsNotification(int32_t id);
    int32_t NotifyUnReadMissedCall(std::map<std::string, int32_t> &phoneNumAndUnreadCountMap);

private:
    bool isIncomingCallMissed_;
    std::string incomingCallNumber_;
    static constexpr int16_t INCOMING_CALL_MISSED_ID = 0;
    static constexpr int16_t INCOMING_CALL_MISSED_CODE = 0;
    const std::string INCOMING_CALL_MISSED_TITLE = "Missed Call";
    void PublishMissedCallEvent(sptr<CallBase> &callObjectPtr);
    void PublishMissedCallNotification(sptr<CallBase> &callObjectPtr);
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_MISSED_CALL_NOTIFICATION_H