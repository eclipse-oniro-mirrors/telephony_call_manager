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

#ifndef TELEPHONY_CALL_RECORDS_MANAGER_H
#define TELEPHONY_CALL_RECORDS_MANAGER_H

#include <mutex>
#include <set>

#include "singleton.h"

#include "call_state_listener_base.h"
#include "call_records_handler.h"

namespace OHOS {
namespace Telephony {
/**
 * @class CallRecordsManager
 * Responsible for recording calls. Logging operations will be performed in a background thread
 * to avoid blocking the main thread.
 */
class CallRecordsManager : public CallStateListenerBase {
    DECLARE_DELAYED_SINGLETON(CallRecordsManager)
public:
    void Init();
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    void AddOneCallRecord(CallAttributeInfo &info);
    void AddOneCallRecord(sptr<CallBase> call, CallAnswerType answerType);

private:
    std::shared_ptr<CallRecordsHandlerService> callRecordsHandlerServerPtr_;
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_CALL_RECORDS_MANAGER_H