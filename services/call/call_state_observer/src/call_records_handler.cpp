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

#include "call_manager_base.h"
#include "call_manager_errors.h"
#include "call_manager_inner_type.h"
#include "ffrt.h"
#include "call_number_utils.h"

namespace OHOS {
namespace Telephony {
CallRecordsHandler::CallRecordsHandler() : callDataPtr_(nullptr)
{
    callDataPtr_ = DelayedSingleton<CallDataBaseHelper>::GetInstance();
    if (callDataPtr_ == nullptr) {
        TELEPHONY_LOGE("callDataPtr_ is nullptr!");
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

int32_t CallRecordsHandler::AddCallLogInfo(const CallRecordInfo &info)
{
    if (callDataPtr_ == nullptr) {
        TELEPHONY_LOGE("callDataPtr is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::string numberLocation = CheckNumberLocationInfo(info);
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
    std::string displayName = "";
    if (std::string(contactInfo.name) != "") {
        displayName = std::string(contactInfo.name);
    } else if (info.numberMarkInfo.markType == MarkType::MARK_TYPE_YELLOW_PAGE) {
        displayName = std::string(info.numberMarkInfo.markContent);
    }

    DataShare::DataShareValuesBucket bucket;
    TELEPHONY_LOGI("callLog Insert begin");
    MakeCallLogInsertBucket(bucket, info, displayName, numberLocation);
    bool ret = callDataPtr_->Insert(bucket);
    if (!ret) {
        TELEPHONY_LOGE("Add call log database fail!");
        return TELEPHONY_ERR_DATABASE_WRITE_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

void CallRecordsHandler::MakeCallLogInsertBucket(DataShare::DataShareValuesBucket &bucket,
    const CallRecordInfo &info, std::string displayName, std::string numberLocation)
{
    bucket.Put(CALL_PHONE_NUMBER, std::string(info.phoneNumber));
    bucket.Put(CALL_DISPLAY_NAME, displayName);
    bucket.Put(CALL_DIRECTION, static_cast<int32_t>(info.directionType));
    bucket.Put(CALL_VOICEMAIL_URI, std::string(""));
    bucket.Put(CALL_SIM_TYPE, 0);
    bucket.Put(CALL_IS_HD, static_cast<int32_t>(info.callType));
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
    if (timeStamp < 0) {
        TELEPHONY_LOGE("call log timeStamp less than 0");
        timeStamp = 0;
    }
    bucket.Put(CALL_CREATE_TIME, timeStamp);
    bucket.Put(CALL_NUMBER_LOCATION, numberLocation);
    bucket.Put(CALL_PHOTO_ID, 0);
    bucket.Put(CALL_SLOT_ID, info.slotId);
    bucket.Put(CALL_FEATURES, info.features);
    bucket.Put(CALL_MARK_TYPE, static_cast<int32_t>(info.numberMarkInfo.markType));
    bucket.Put(CALL_MARK_CONTENT, std::string(info.numberMarkInfo.markContent));
    bucket.Put(CALL_IS_CLOUD_MARK, info.numberMarkInfo.isCloud);
    bucket.Put(CALL_MARK_COUNT, info.numberMarkInfo.markCount);
    bucket.Put(CALL_BLOCK_REASON, info.blockReason);
}

std::string CallRecordsHandler::CheckNumberLocationInfo(const CallRecordInfo &info)
{
    std::string str(info.numberLocation);
    if (str == "default") {
        TELEPHONY_LOGI("AddCallLogInfo, number location is default");
        str = "";
        DelayedSingleton<CallNumberUtils>::GetInstance()->QueryNumberLocationInfo(str, std::string(info.phoneNumber));
    }
    if (str == "") {
        str = "N";
    }
    return str;
}

int32_t CallRecordsHandler::QueryAndNotifyUnReadMissedCall()
{
    if (callDataPtr_ == nullptr) {
        TELEPHONY_LOGE("callDataPtr is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    missedCallNotification_ = std::make_shared<MissedCallNotification>();
    if (missedCallNotification_ == nullptr) {
        TELEPHONY_LOGE("missedCallNotification_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    DataShare::DataSharePredicates predicates;
    std::map<std::string, int32_t> phoneNumAndUnreadCountMap;
    predicates.EqualTo(CALL_IS_READ, static_cast<int32_t>(CallLogReadState::CALL_IS_UNREAD));
    predicates.And();
    predicates.EqualTo(CALL_DIRECTION, static_cast<int32_t>(CallDirection::CALL_DIRECTION_IN));
    predicates.And();
    predicates.EqualTo(CALL_ANSWER_STATE, static_cast<int32_t>(CallAnswerType::CALL_ANSWER_MISSED));
    bool ret = callDataPtr_->QueryCallLog(phoneNumAndUnreadCountMap, predicates);
    if (phoneNumAndUnreadCountMap.empty() || !ret) {
        TELEPHONY_LOGE("Don't have unread missed call in call log!");
        return TELEPHONY_ERR_DATABASE_READ_FAIL;
    }
    int32_t result = missedCallNotification_->NotifyUnReadMissedCall(phoneNumAndUnreadCountMap);
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Notify unread missed call error!");
        return TELEPHONY_ERR_PUBLISH_BROADCAST_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

CallRecordsHandlerService::CallRecordsHandlerService() : handler_(nullptr) {}

CallRecordsHandlerService::~CallRecordsHandlerService() {}

void CallRecordsHandlerService::Start()
{
    handler_ = std::make_shared<CallRecordsHandler>();
    return;
}

int32_t CallRecordsHandlerService::StoreCallRecord(const CallRecordInfo &info)
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { handler_->AddCallLogInfo(info); });
    return TELEPHONY_SUCCESS;
}

int32_t CallRecordsHandlerService::RemoveMissedIncomingCallNotification()
{
    std::shared_ptr<CallDataBaseHelper> callDataPtr = DelayedSingleton<CallDataBaseHelper>::GetInstance();
    if (callDataPtr == nullptr) {
        TELEPHONY_LOGE("callDataPtr is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    DataShare::DataSharePredicates predicates;
    DataShare::DataShareValuesBucket bucket;
    bucket.Put(CALL_IS_READ, static_cast<int32_t>(CallLogReadState::CALL_IS_READ));
    predicates.EqualTo(CALL_IS_READ, static_cast<int32_t>(CallLogReadState::CALL_IS_UNREAD));
    predicates.And();
    predicates.EqualTo(CALL_DIRECTION, static_cast<int32_t>(CallDirection::CALL_DIRECTION_IN));
    predicates.And();
    predicates.EqualTo(CALL_ANSWER_STATE, static_cast<int32_t>(CallAnswerType::CALL_ANSWER_MISSED));
    bool ret = callDataPtr->Update(predicates, bucket);
    if (ret) {
        TELEPHONY_LOGE("Update call log database fail!");
        return TELEPHONY_ERR_DATABASE_WRITE_FAIL;
    }
    TELEPHONY_LOGI("Update call log database success!");
    return TELEPHONY_SUCCESS;
}

int32_t CallRecordsHandlerService::QueryUnReadMissedCallLog()
{
    if (handler_.get() == nullptr) {
        TELEPHONY_LOGE("handler_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    ffrt::submit([=]() { handler_->QueryAndNotifyUnReadMissedCall(); });
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
