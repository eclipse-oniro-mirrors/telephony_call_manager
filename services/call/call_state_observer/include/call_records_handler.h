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

#ifndef TELEPHONY_CALL_RECORDS_HANDLER_H
#define TELEPHONY_CALL_RECORDS_HANDLER_H

#include <memory>
#include <mutex>

#include "call_data_base_helper.h"
#include "call_status_manager.h"
#include "event_handler.h"
#include "event_runner.h"
#include "missed_call_notification.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
class CallRecordsHandler {
public:
    CallRecordsHandler();
    virtual ~CallRecordsHandler() = default;
    int32_t QueryAndNotifyUnReadMissedCall();
    int32_t AddCallLogInfo(const CallRecordInfo &info);

private:
    void QueryCallerInfo(ContactInfo &contactInfo, std::string phoneNumber);

private:
    std::shared_ptr<CallDataBaseHelper> callDataPtr_;
    std::shared_ptr<MissedCallNotification> missedCallNotification_;
};

class CallRecordsHandlerService : public std::enable_shared_from_this<CallRecordsHandlerService> {
    DECLARE_DELAYED_SINGLETON(CallRecordsHandlerService)
public:
    void Start();
    int32_t StoreCallRecord(const CallRecordInfo &info);
    int32_t CancelMissedIncomingCallNotification();
    int32_t QueryUnReadMissedCallLog();

private:
    std::shared_ptr<CallRecordsHandler> handler_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_CALL_RECORDS_HANDLER_H