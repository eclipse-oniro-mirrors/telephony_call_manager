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

#ifndef INCOMING_CALL_NOTIFICATION_H
#define INCOMING_CALL_NOTIFICATION_H

#include <cstdint>

#include "call_state_listener_base.h"

namespace OHOS {
namespace Telephony {
class IncomingCallNotification : public CallStateListenerBase {
public:
    IncomingCallNotification();
    ~IncomingCallNotification() = default;
    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(int32_t cause) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;

private:
    std::string incomingCallNumber_;
    static constexpr int16_t INCOMING_CALL_NOTIFICATION_ID = 0;
    static constexpr int16_t INCOMING_CALL_NOTIFICATION_CODE = 0;
    const std::string INCOMING_CALL_NOTIFICATION_TITLE = "Incoming Call";
    void PublishIncomingCallNotification(sptr<CallBase> &callObjectPtr);
    int32_t CancelIncomingCallNotification();
    bool IsFullScreen();
};
} // namespace Telephony
} // namespace OHOS
#endif