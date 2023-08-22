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

#include "call_manager_inner_type.h"
#include "call_number_utils.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "os_account_manager_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace Telephony {
constexpr int16_t DEFAULT_COUNTRY_CODE = 0;
constexpr int16_t DEFAULT_TIME = 0;
const int32_t ACTIVE_USER_ID = 100;
CallRecordsManager::CallRecordsManager() : callRecordsHandlerServerPtr_(nullptr) {}

CallRecordsManager::~CallRecordsManager()
{
    if (statusChangeListener_ != nullptr) {
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy != nullptr) {
            samgrProxy->UnSubscribeSystemAbility(OHOS::SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN, statusChangeListener_);
            statusChangeListener_ = nullptr;
        }
    }
}

void CallRecordsManager::Init()
{
    callRecordsHandlerServerPtr_ = DelayedSingleton<CallRecordsHandlerService>::GetInstance();
    if (callRecordsHandlerServerPtr_ == nullptr) {
        TELEPHONY_LOGE("call record manager init failure.");
        return;
    }
    callRecordsHandlerServerPtr_->Start();
    statusChangeListener_ = new (std::nothrow) AccountSystemAbilityListener();
    if (statusChangeListener_ == nullptr) {
        TELEPHONY_LOGE("failed to create statusChangeListener");
        return;
    }
    auto managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("get system ability manager error");
        return;
    }
    int32_t ret = managerPtr->SubscribeSystemAbility(OHOS::SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN, statusChangeListener_);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("failed to subscribe account manager service SA!");
        return;
    }
}

void CallRecordsManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    CallAttributeInfo info;
    if (nextState != TelCallState::CALL_STATUS_DISCONNECTED) {
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
    if (strlen(info.accountNumber) > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return;
    }
    errno_t result = memcpy_s(data.phoneNumber, kMaxNumberLen, info.accountNumber, strlen(info.accountNumber));
    if (result != EOK) {
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
    data.slotId = info.accountId;
    data.callType = info.callType;
    std::string tmpStr("");
    (void)DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumber(
        std::string(data.phoneNumber), "CN", tmpStr);

    if (tmpStr.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return;
    }
    if (memcpy_s(data.formattedPhoneNumber, kMaxNumberLen, tmpStr.c_str(), tmpStr.length()) != 0) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return;
    }
    callRecordsHandlerServerPtr_->StoreCallRecord(data);
}

int32_t CallRecordsManager::RemoveMissedIncomingCallNotification()
{
    if (callRecordsHandlerServerPtr_ == nullptr) {
        TELEPHONY_LOGE("callRecordsHandlerServerPtr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = callRecordsHandlerServerPtr_->RemoveMissedIncomingCallNotification();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RemoveMissedIncomingCallNotification failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

void AccountSystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("SA:%{public}d is added!", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("added SA is invalid!");
        return;
    }
    if (systemAbilityId != OHOS::SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN) {
        TELEPHONY_LOGE("added SA is not accoubt manager service, ignored.");
        return;
    }
    std::vector<int32_t> activeList = { 0 };
    DelayedSingleton<AppExecFwk::OsAccountManagerWrapper>::GetInstance()->QueryActiveOsAccountIds(activeList);
    TELEPHONY_LOGI("current active user id is :%{public}d", activeList[0]);
    if (activeList[0] == ACTIVE_USER_ID) {
        int32_t ret = DelayedSingleton<CallRecordsHandlerService>::GetInstance()->QueryUnReadMissedCallLog();
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("QueryUnReadMissedCallLog failed!");
        }
    } else {
        MatchingSkills matchingSkills;
        matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
        CommonEventSubscribeInfo subscriberInfo(matchingSkills);
        subscriberInfo.SetThreadMode(EventFwk::CommonEventSubscribeInfo::COMMON);
        userSwitchSubscriber_ = std::make_shared<UserSwitchEventSubscriber>(subscriberInfo);
        bool subRet = CommonEventManager::SubscribeCommonEvent(userSwitchSubscriber_);
        if (!subRet) {
            TELEPHONY_LOGE("Subscribe user switched event failed!");
        }
    }
}

void AccountSystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("SA:%{public}d is removed!", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("removed SA is invalid!");
        return;
    }
    if (systemAbilityId != OHOS::SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN) {
        TELEPHONY_LOGE("removed SA is not account manager service,, ignored.");
        return;
    }
    if (userSwitchSubscriber_ != nullptr) {
        bool subRet = CommonEventManager::UnSubscribeCommonEvent(userSwitchSubscriber_);
        if (!subRet) {
            TELEPHONY_LOGE("UnSubscribe user switched event failed!");
        }
        userSwitchSubscriber_ = nullptr;
    }
}

void UserSwitchEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    OHOS::EventFwk::Want want = data.GetWant();
    std::string action = data.GetWant().GetAction();
    TELEPHONY_LOGI("action = %{public}s", action.c_str());
    if (action == CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        int32_t userId = data.GetCode();
        if (userId == ACTIVE_USER_ID) {
            int32_t ret = DelayedSingleton<CallRecordsHandlerService>::GetInstance()->QueryUnReadMissedCallLog();
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("Query unread missed call log failed!");
            }
        }
    }
}
} // namespace Telephony
} // namespace OHOS