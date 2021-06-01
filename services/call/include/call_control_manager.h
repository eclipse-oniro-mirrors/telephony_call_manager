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

#ifndef CALL_MANAGER_H
#define CALL_MANAGER_H
#include <cstring>
#include <list>
#include <memory>

#include "pac_map.h"
#include "singleton.h"

#include "call_number_utils.h"
#include "call_policy.h"
#include "call_state_listener.h"
#include "report_call_state_handler.h"

namespace OHOS {
namespace TelephonyCallManager {
class CallControlManager : public CallNumberUtils, public CallPolicy {
    DECLARE_DELAYED_SINGLETON(CallControlManager)
public:
    bool Init();

    int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras, int32_t &callId);
    int32_t AcceptCall(int32_t callId, int32_t videoState);
    int32_t RejectCall(int32_t callId, bool isSendSms, std::u16string content);
    int32_t HangUpCall(int32_t callId);
    int32_t GetCallState();
    int32_t InitCallInfo(std::u16string number, CallInfo &callInfo, AppExecFwk::PacMap &extras);
    int32_t CreateCallObject(const CallType &callType, sptr<CallBase> &call, CallInfo &info);
    bool NotifyNewCallCreated(sptr<CallBase> &callObjectPtr);
    bool NotifyCallDestroyed(sptr<CallBase> &callObjectPtr);
    bool NotifyCallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallStates priorState, TelCallStates nextState);
    bool NotifyIncomingCallAnswered(sptr<CallBase> &callObjectPtr);
    bool NotifyIncomingCallRejected(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content);

    int32_t GetSimNumber();
    int32_t GetID();

private:
    std::unique_ptr<CallStateListener> callStateListenerPtr_;
    sptr<ReportCallStateHandlerService> reportCallStateHandlerPtr_;
    SIMCardInfo simInfo_;
};
} // namespace TelephonyCallManager
} // namespace OHOS

#endif // CALL_MANAGER_H