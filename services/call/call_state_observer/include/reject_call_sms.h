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

#ifndef TELEPHONY_REJECT_CALL_SMS_H
#define TELEPHONY_REJECT_CALL_SMS_H

#include "call_state_listener_base.h"
#ifdef ABILITY_SMS_SUPPORT
#include "sms_service_manager_client.h"
#endif

namespace OHOS {
namespace Telephony {
class RejectCallSms : public CallStateListenerBase {
public:
    RejectCallSms();
    ~RejectCallSms() = default;
    void NewCallCreated(sptr<CallBase> &callObjectPtr) override;
    void CallDestroyed(int32_t cause) override;
    void IncomingCallActivated(sptr<CallBase> &callObjectPtr) override;
    void IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content) override;
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;

private:
    void SendMessage(int32_t slotId, const std::u16string &desAddr, const std::u16string &text);
    std::u16string ConvertToUtf16(const std::string &str);
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_REJECT_CALL_SMS_H