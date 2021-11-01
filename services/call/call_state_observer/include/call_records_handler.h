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

#ifndef CALL_RECORDS_HANDLER_H
#define CALL_RECORDS_HANDLER_H

#include <memory>
#include <mutex>

#include "event_handler.h"
#include "event_runner.h"
#include "singleton.h"

#include "call_status_manager.h"
#include "call_data_base_helper.h"

namespace OHOS {
namespace Telephony {
const std::string CALL_PHONE_NUMBER = "phone_number";
const std::string CALL_DISPLAY_NAME = "display_name";
const std::string CALL_DIRECTION = "call_direction";
const std::string CALL_VOICEMAIL_URI = "voicemail_uri";
const std::string CALL_SIM_TYPE = "sim_type";
const std::string CALL_IS_HD = "is_hd";
const std::string CALL_IS_READ = "is_read";
const std::string CALL_RING_DURATION = "ring_duration";
const std::string CALL_TALK_DURATION = "talk_duration";
const std::string CALL_FORMAT_NUMBER = "format_number";
const std::string CALL_QUICKSEARCH_KEY = "quicksearch_key";
const std::string CALL_NUMBER_TYPE = "number_type";
const std::string CALL_NUMBER_TYPE_NAME = "number_type_name";
const std::string CALL_BEGIN_TIME = "begin_time";
const std::string CALL_END_TIME = "end_time";
const std::string CALL_ANSWER_STATE = "answer_state";
const std::string CALL_CREATE_TIME = "create_time";
const std::string CALL_NUMBER_LOCATION = "number_location";
const std::string CALL_PHOTO_ID = "photo_id";

class CallRecordsHandler : public AppExecFwk::EventHandler {
public:
    CallRecordsHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
    virtual ~CallRecordsHandler() = default;
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event);
    std::shared_ptr<CallDataBaseHelper> callDataPtr_;
};

class CallRecordsHandlerService : public std::enable_shared_from_this<CallRecordsHandlerService> {
    DECLARE_DELAYED_SINGLETON(CallRecordsHandlerService)
public:
    void Start();
    int32_t StoreCallRecord(const CallRecordInfo &info);
    enum {
        HANDLER_ADD_CALL_RECORD_INFO = 0,
    };

private:
    std::shared_ptr<AppExecFwk::EventRunner> eventLoop_;
    std::shared_ptr<CallRecordsHandler> handler_;
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_RECORDS_HANDLER_H