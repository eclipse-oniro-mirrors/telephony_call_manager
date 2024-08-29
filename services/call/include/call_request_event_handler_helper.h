/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef CALL_REQUEST_EVENT_HANDLER_HELPER
#define CALL_REQUEST_EVENT_HANDLER_HELPER

#include "event_handler.h"
#include "refbase.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {

class CallRequestEventHandlerHelper : public std::enable_shared_from_this<CallRequestEventHandlerHelper> {
    DECLARE_DELAYED_SINGLETON(CallRequestEventHandlerHelper)
public:
    int32_t SetDialingCallProcessing();
    void RemoveEventHandlerTask();
    void RestoreDialingFlag(bool isDialingCallProcessing);
    bool IsDialingCallProcessing();
    void SetPendingMo(bool pendingMo, int32_t callId);
    bool HasPendingMo(int32_t callId);
    void SetPendingHangup(bool pendingHangup, int32_t callId);
    bool HasPendingHangp(int32_t callId);

private:
    std::shared_ptr<AppExecFwk::EventHandler> callRequestEventHandler_;
    bool isDialingCallProcessing_ = false;
    bool pendingMo_ = false;
    bool pendingHangup_ = false;
    bool pendingMoCallId_ = false;
    bool pendingHangupCallId_ = false;
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_REQUEST_EVENT_HANDLER_HELPER
