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

#include "call_records_manager.h"

#include "securec.h"
#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t DEFAULT_COUNTRY_CODE = 0;
constexpr int32_t DEFAULT_TIME = 0;
CallRecordsManager::CallRecordsManager() : callRecordsHandlerServerPtr_(nullptr) {}

CallRecordsManager::~CallRecordsManager() {}

void CallRecordsManager::Init()
{
    callRecordsHandlerServerPtr_ = DelayedSingleton<CallRecordsHandlerService>::GetInstance();
    if (callRecordsHandlerServerPtr_ == nullptr) {
        TELEPHONY_LOGE("call record manager init failure.");
        return;
    }
    callRecordsHandlerServerPtr_->Start();
}

void CallRecordsManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    CallAttributeInfo info;
    if (nextState != CALL_STATUS_DISCONNECTED) {
        TELEPHONY_LOGE("nextState not CALL_STATUS_DISCONNECTED");
        return;
    }
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object is nullptr");
        return;
    }
    (void)memset_s(&info, sizeof(CallAttributeInfo), 0, sizeof(CallAttributeInfo));
    callObjectPtr->GetCallAttributeBaseInfo(info);
    AddOneCallRecord(info);
}

void CallRecordsManager::AddOneCallRecord(sptr<CallBase> call, CallAnswerType answerType)
{
    CallAttributeInfo info;
    (void)memset_s(&info, sizeof(CallAttributeInfo), 0, sizeof(CallAttributeInfo));
    call->GetCallAttributeBaseInfo(info);
    AddOneCallRecord(info);
}

void CallRecordsManager::AddOneCallRecord(CallAttributeInfo &info)
{
    CallRecordInfo data;
    (void)memset_s(&data, sizeof(CallRecordInfo), 0, sizeof(CallRecordInfo));
    if (callRecordsHandlerServerPtr_ == nullptr) {
        TELEPHONY_LOGE("callRecordsHandlerServerPtr_ is nullptr");
        return;
    }
    if (memcpy_s(data.phoneNumber, kMaxNumberLen, info.accountNumber, strlen(info.accountNumber)) != 0) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return;
    }
    if ((info.callBeginTime == DEFAULT_TIME) || (info.callEndTime == DEFAULT_TIME)) {
        data.callDuration = DEFAULT_TIME;
    } else {
        data.callDuration = difftime(info.callEndTime, info.callBeginTime);
    }
    if ((info.ringBeginTime == DEFAULT_TIME) || (info.ringEndTime == DEFAULT_TIME)) {
        data.ringDuration = DEFAULT_TIME;
    } else {
        data.ringDuration = difftime(info.ringEndTime, info.ringBeginTime);
    }
    data.callId = info.callId;
    data.callBeginTime = info.callBeginTime;
    data.callEndTime = info.callEndTime;
    data.directionType = info.callDirection;
    data.answerType = info.answerType;
    data.countryCode = DEFAULT_COUNTRY_CODE;
    callRecordsHandlerServerPtr_->StoreCallRecord(data);
}
} // namespace Telephony
} // namespace OHOS