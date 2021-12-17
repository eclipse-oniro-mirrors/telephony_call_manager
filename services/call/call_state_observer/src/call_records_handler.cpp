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

#include "call_records_handler.h"

#include "call_manager_errors.h"

namespace OHOS {
namespace Telephony {
CallRecordsHandler::CallRecordsHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : AppExecFwk::EventHandler(runner), callDataPtr_(nullptr)
{
    callDataPtr_ = DelayedSingleton<CallDataBaseHelper>::GetInstance();
    if (callDataPtr_ == nullptr) {
        TELEPHONY_LOGE("callDataPtr_ is nullptr!");
    }
}

void CallRecordsHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("CallRecordsHandler::ProcessEvent parameter error");
        return;
    }
    if (event->GetInnerEventId() == CallRecordsHandlerService::HANDLER_ADD_CALL_RECORD_INFO) {
        auto object = event->GetUniqueObject<CallRecordInfo>();
        if (object == nullptr) {
            TELEPHONY_LOGE("object is nullptr!");
            return;
        }
        CallRecordInfo info = *object;
        if (callDataPtr_ == nullptr) {
            TELEPHONY_LOGE("callDataPtr_ is nullptr!");
            return;
        }
        NativeRdb::ValuesBucket bucket;
        bucket.PutString(CALL_PHONE_NUMBER, std::string(info.phoneNumber));
        bucket.PutString(CALL_DISPLAY_NAME, std::string(""));
        bucket.PutInt(CALL_DIRECTION, info.directionType);
        bucket.PutString(CALL_VOICEMAIL_URI, std::string(""));
        bucket.PutInt(CALL_SIM_TYPE, 0);
        bucket.PutInt(CALL_IS_HD, 0);
        bucket.PutInt(CALL_IS_READ, 0);
        bucket.PutInt(CALL_RING_DURATION, info.ringDuration);
        bucket.PutInt(CALL_TALK_DURATION, info.callDuration);
        bucket.PutString(CALL_FORMAT_NUMBER, std::string(""));
        bucket.PutString(CALL_QUICKSEARCH_KEY, std::string(""));
        bucket.PutInt(CALL_NUMBER_TYPE, 0);
        bucket.PutString(CALL_NUMBER_TYPE_NAME, std::string(""));
        bucket.PutInt(CALL_BEGIN_TIME, info.callBeginTime);
        bucket.PutInt(CALL_END_TIME, info.callEndTime);
        bucket.PutInt(CALL_ANSWER_STATE, info.answerType);
        uint64_t timeStamp = time(0);
        bucket.PutInt(CALL_CREATE_TIME, timeStamp);
        bucket.PutString(CALL_NUMBER_LOCATION, std::string(""));
        bucket.PutInt(CALL_PHOTO_ID, 0);
        callDataPtr_->Insert(bucket);
        return;
    }
}

CallRecordsHandlerService::CallRecordsHandlerService() : eventLoop_(nullptr), handler_(nullptr) {}

CallRecordsHandlerService::~CallRecordsHandlerService() {}

void CallRecordsHandlerService::Start()
{
    eventLoop_ = AppExecFwk::EventRunner::Create("CallRecordsHandlerService");
    if (eventLoop_.get() == nullptr) {
        TELEPHONY_LOGE("failed to create EventRunner");
        return;
    }
    handler_ = std::make_shared<CallRecordsHandler>(eventLoop_);
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("failed to create CallRecordsHandler");
        return;
    }
    eventLoop_->Run();
    return;
}

int32_t CallRecordsHandlerService::StoreCallRecord(const CallRecordInfo &info)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERROR;
    }
    std::unique_ptr<CallRecordInfo> para = std::make_unique<CallRecordInfo>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique CallRecordInfo failed!");
        return TELEPHONY_ERROR;
    }
    *para = info;
    handler_->SendEvent(HANDLER_ADD_CALL_RECORD_INFO, std::move(para));
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS