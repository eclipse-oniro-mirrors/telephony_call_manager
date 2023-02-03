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
        ContactInfo contactInfo = {
            .name = "",
            .number = "",
            .isContacterExists = false,
            .ringtonePath = "",
            .isSendToVoicemail = false,
            .isEcc = false,
            .isVoiceMail = false,
        };
        QueryCallerInfo(contactInfo, std::string(info.phoneNumber));

        DataShare::DataShareValuesBucket bucket;
        TELEPHONY_LOGI("callLog Insert begin");
        bucket.Put(CALL_PHONE_NUMBER, std::string(info.phoneNumber));
        bucket.Put(CALL_DISPLAY_NAME, std::string(contactInfo.name));
        bucket.Put(CALL_DIRECTION, static_cast<int32_t>(info.directionType));
        bucket.Put(CALL_VOICEMAIL_URI, std::string(""));
        bucket.Put(CALL_SIM_TYPE, 0);
        bucket.Put(CALL_IS_HD, 0);
        bucket.Put(CALL_IS_READ, 0);
        bucket.Put(CALL_RING_DURATION, static_cast<int32_t>(info.ringDuration));
        bucket.Put(CALL_TALK_DURATION, static_cast<int32_t>(info.callDuration));
        bucket.Put(CALL_FORMAT_NUMBER, std::string(info.formattedPhoneNumber));
        bucket.Put(CALL_QUICKSEARCH_KEY, std::string(""));
        bucket.Put(CALL_NUMBER_TYPE, 0);
        bucket.Put(CALL_NUMBER_TYPE_NAME, std::string(""));
        bucket.Put(CALL_BEGIN_TIME, info.callBeginTime);
        bucket.Put(CALL_END_TIME, info.callEndTime);
        bucket.Put(CALL_ANSWER_STATE, static_cast<int32_t>(info.answerType));
        time_t timeStamp = time(0);
        bucket.Put(CALL_CREATE_TIME, timeStamp);
        bucket.Put(CALL_NUMBER_LOCATION, std::string(""));
        bucket.Put(CALL_PHOTO_ID, 0);
        bucket.Put(CALL_SLOT_ID, info.slotId);
        callDataPtr_->Insert(bucket);
        return;
    }
}

void CallRecordsHandler::QueryCallerInfo(ContactInfo &contactInfo, std::string phoneNumber)
{
    std::shared_ptr<CallDataBaseHelper> callDataPtr = DelayedSingleton<CallDataBaseHelper>::GetInstance();
    if (callDataPtr == nullptr) {
        TELEPHONY_LOGE("callDataPtr is nullptr!");
        return;
    }
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(CALL_DETAIL_INFO, phoneNumber);
    predicates.And();
    predicates.EqualTo(CALL_CONTENT_TYPE, CALL_PHONE);
    bool ret = callDataPtr->Query(contactInfo, predicates);
    if (!ret) {
        TELEPHONY_LOGE("Query contact database fail!");
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
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_ptr<CallRecordInfo> para = std::make_unique<CallRecordInfo>();
    if (para.get() == nullptr) {
        TELEPHONY_LOGE("make_unique CallRecordInfo failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *para = info;
    handler_->SendEvent(HANDLER_ADD_CALL_RECORD_INFO, std::move(para));
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
