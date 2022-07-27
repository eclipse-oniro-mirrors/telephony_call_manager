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

#ifndef TELEPHONY_CALL_STATE_BROADCAST_H
#define TELEPHONY_CALL_STATE_BROADCAST_H

#include "call_state_listener_base.h"

namespace OHOS {
namespace Telephony {
class CallStateBroadcast : public CallStateListenerBase {
public:
    CallStateBroadcast() = default;
    ~CallStateBroadcast() = default;
    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(int32_t cause) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;

private:
    constexpr static int16_t CALL_STATE_UPDATED_CODE = 0;
    void PublishCallStateEvent(sptr<CallBase> &callObjectPtr, int32_t priorState, int32_t nextState);
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_CALL_STATE_BROADCAST_H