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
#include "parameter.h"
#include "securec.h"
#include "call_earthquake_alarm_subscriber.h"
#include <regex>
#include "telephony_cust_wrapper.h"
#include "settings_datashare_helper.h"
#include "call_manager_utils.h"
#ifdef SUPPORT_MUTE_BY_DATABASE
#include "interoperable_settings_handler.h"
#endif
#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
constexpr const char *PROP_NETWORK_COUNTRY_ISO = "telephony.operator.iso-country";
constexpr const char *DEFAULT_NETWORK_COUNTRY = "CN";
constexpr int16_t DEFAULT_COUNTRY_CODE = 0;
constexpr int16_t DEFAULT_TIME = 0;
const int32_t ACTIVE_USER_ID = 100;
const uint32_t FEATURES_VIDEO = 1 << 0;
const int32_t PROP_SYSPARA_SIZE = 128;
const char *FORMAT_PATTERN = ",|;";
const char *MARK_SOURCE_OF_ANTIFRAUT_CENTER = "5";
const char *MARK_SOURCE_OF_OTHERS = "3";
const std::string SETTINGS_ANTIFRAUD_CENTER_SWITCH = "";
const std::string SETTINGS_ANTIFRAUD_BESTMIND_SWITCH = "";
const std::string ANTIFRAUD_CENTER_BUNDLE_NAME = "";
const std::string BEST_MIND_BUNDLE_NAME = "";
const std::string TELEPHONY_IDENTITY_SWITCH = "";
const std::string SETTINGS_DB_VALUE_EXIST = "1";
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
    if (dataShareReadySubscriber_ != nullptr) {
        bool subRet = CommonEventManager::UnSubscribeCommonEvent(dataShareReadySubscriber_);
        if (!subRet) {
            TELEPHONY_LOGE("UnSubscribe data share ready event failed!");
        }
        dataShareReadySubscriber_ = nullptr;
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
    RegisterDataShareReadySubscriber();
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

void CallRecordsManager::RegisterDataShareReadySubscriber()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_DATA_SHARE_READY);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    subscriberInfo.SetThreadMode(EventFwk::CommonEventSubscribeInfo::COMMON);
    dataShareReadySubscriber_ = std::make_shared<DataShareReadyEventSubscriber>(subscriberInfo);
    bool subRet = CommonEventManager::SubscribeCommonEvent(dataShareReadySubscriber_);
    if (!subRet) {
        TELEPHONY_LOGE("Subscribe data share ready event failed!");
    }
}

void CallRecordsManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr != nullptr && callObjectPtr->GetCallType() == CallType::TYPE_VOIP) {
        TELEPHONY_LOGI("Voip call should not save cellular call records");
        return;
    }
    CallAttributeInfo info;
    if (nextState != TelCallState::CALL_STATUS_DISCONNECTED) {
        return;
    }
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object is nullptr");
        return;
    }
    (void)memset_s(&info, sizeof(CallAttributeInfo), 0, sizeof(CallAttributeInfo));
    callObjectPtr->GetCallAttributeBaseInfo(info);
    if (info.callType == CallType::TYPE_BLUETOOTH && callObjectPtr->GetBtCallSlotId() == BT_CALL_INVALID_SLOT) {
        info.accountId = BT_CALL_INVALID_SLOT;
    }
    ContactInfo contactInfo = callObjectPtr->GetCallerInfo();
    if (contactInfo.name.empty() && !info.name.empty()) {
        info.namePresentation = 1;
    } else {
        info.namePresentation = 0;
    }
    if (info.numberMarkInfo.markType == MarkType::MARK_TYPE_DEFAULT) {
        TELEPHONY_LOGI("markType is default.");
        info.numberMarkInfo.markType = MarkType::MARK_TYPE_NONE;
    }

    if (nextState == TelCallState::CALL_STATUS_DISCONNECTED &&
        callObjectPtr->GetCallDirection() == CallDirection::CALL_DIRECTION_IN && 
        callObjectPtr->IsAiAutoAnswer()) {
        info.answerType = CallAnswerType::CALL_ANSWER_MISSED;
    }
    if (callObjectPtr->GetNewCallUseBox()) {
        info.newCallUseBox = 1;
    } else {
        info.newCallUseBox = 0;
    }
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
    if (TELEPHONY_CUST_WRAPPER.isHideViolenceOrEmcNumbersInCallLog_ != nullptr
        && TELEPHONY_CUST_WRAPPER.isHideViolenceOrEmcNumbersInCallLog_(info)) {
        TELEPHONY_LOGE("hide violence or emergency nums in call log!");
        return;
    }
    errno_t result = memcpy_s(data.phoneNumber, kMaxNumberLen, info.accountNumber, strlen(info.accountNumber));
    if (result != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return;
    }
    if (strlen(info.numberLocation) > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Location out of limit!");
        return;
    }
    result = memcpy_s(data.numberLocation, kMaxNumberLen, info.numberLocation, strlen(info.numberLocation));
    if (result != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return;
    }
    if (memcpy_s(data.detectDetails, sizeof(data.detectDetails), info.detectDetails, strlen(info.detectDetails))
        != EOK) {
        TELEPHONY_LOGE("memcpy_s detectDetails failed!");
        return;
    }
    CopyCallInfoToRecord(info, data);
    std::string countryIso = GetCountryIso();
    int32_t formatRet = CopyFormatNumberToRecord(countryIso, data);
    if (formatRet != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CopyFormatNumberToRecord failed!");
        return;
    }
    int32_t formatToE164Ret = CopyFormatNumberToE164ToRecord(countryIso, data);
    if (formatToE164Ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CopyFormatNumberToE164ToRecord failed!");
        return;
    }
    callRecordsHandlerServerPtr_->StoreCallRecord(data);
}

std::string CallRecordsManager::GetCountryIso()
{
    char valueStr[PROP_SYSPARA_SIZE] = {0};
    GetParameter(PROP_NETWORK_COUNTRY_ISO, "", valueStr, PROP_SYSPARA_SIZE);
    std::string countryIso = valueStr;
    if (countryIso == "") {
        TELEPHONY_LOGI("GetParameter is null, default network countryIso cn");
        countryIso = DEFAULT_NETWORK_COUNTRY;
    } else {
        TELEPHONY_LOGI("GetParameter network countryIso is %{public}s", countryIso.c_str());
    }
    return countryIso;
}

int32_t CallRecordsManager::CopyFormatNumberToRecord(std::string &countryIso, CallRecordInfo &data)
{
    std::string tmpStr("");
    if (std::regex_search(std::string(data.phoneNumber), std::regex(FORMAT_PATTERN))) {
        (void)DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberAsYouType(
            std::string(data.phoneNumber), countryIso, tmpStr);
    } else {
        (void)DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumber(
            std::string(data.phoneNumber), countryIso, tmpStr);
    }

    if (tmpStr.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    errno_t result = memcpy_s(data.formattedNumber, kMaxNumberLen, tmpStr.c_str(), tmpStr.length());
    if (result != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return result;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallRecordsManager::CopyFormatNumberToE164ToRecord(std::string &countryIso, CallRecordInfo &data)
{
    std::string tmpStr("");
    (void)DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(
        std::string(data.phoneNumber), countryIso, tmpStr);
    if (tmpStr.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    errno_t result = memcpy_s(data.formattedNumberToE164, kMaxNumberLen, tmpStr.c_str(), tmpStr.length());
    if (result != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return result;
    }
    return TELEPHONY_SUCCESS;
}

void CallRecordsManager::CopyCallInfoToRecord(CallAttributeInfo &info, CallRecordInfo &data)
{
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
    data.callCreateTime = info.callCreateTime;
    data.callEndTime = info.callEndTime;
    data.directionType = info.callDirection;
    data.answerType = info.answerType;
    data.countryCode = DEFAULT_COUNTRY_CODE;
    data.slotId = info.accountId;
    data.callType = info.callType;
    // use original call type for video call record
    int32_t callFeatures = GetCallFeatures(info.originalCallType);
    data.features = callFeatures;
    data.numberMarkInfo = info.numberMarkInfo;
    if (strcmp(data.numberMarkInfo.markSource, MARK_SOURCE_OF_ANTIFRAUT_CENTER) == 0) {
        int32_t userId = 0;
        AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(userId);
        GetNumberMarkSource(userId, data.numberMarkInfo.markSource, kMaxNumberLen + 1);
    }
    data.blockReason = info.blockReason;
    data.celiaCallType = info.celiaCallType;
    data.name = info.name;
    data.namePresentation = info.namePresentation;
    data.newCallUseBox = info.newCallUseBox;
}

void CallRecordsManager::GetNumberMarkSource(int32_t userId, char *source, unsigned int size)
{
    std::string isAntifraudSwitchOn = "0";
    std::string isTelephonyIdentityOn = "0";
    std::string isAntiFraudBestMindSwitchOn = "0";
    auto settingHelper = SettingsDataShareHelper::GetInstance();
    if (settingHelper == nullptr) {
        return;
    }
    if (size <= strlen(MARK_SOURCE_OF_ANTIFRAUT_CENTER) && size <= strlen(MARK_SOURCE_OF_OTHERS)) {
        return;
    }
    OHOS::Uri settingUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI);
    settingHelper->Query(settingUri, SETTINGS_ANTIFRAUD_CENTER_SWITCH, isAntifraudSwitchOn);
    settingHelper->Query(settingUri, SETTINGS_ANTIFRAUD_BESTMIND_SWITCH, isAntiFraudBestMindSwitchOn);
    if (isAntifraudSwitchOn == "0" && isAntiFraudBestMindSwitchOn == "0") {
        strcpy_s(source, size, MARK_SOURCE_OF_OTHERS);
        return;
    }
    settingHelper->Query(settingUri, TELEPHONY_IDENTITY_SWITCH, isTelephonyIdentityOn);
    if (isTelephonyIdentityOn == "0") {
        strcpy_s(source, size, MARK_SOURCE_OF_OTHERS);
        return;
    }
    if (!CallManagerUtils::IsBundleInstalled(ANTIFRAUD_CENTER_BUNDLE_NAME, userId) &&
        !CallManagerUtils::IsBundleInstalled(BEST_MIND_BUNDLE_NAME, userId)) {
        strcpy_s(source, size, MARK_SOURCE_OF_OTHERS);
        return;
    }
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

int32_t CallRecordsManager::GetCallFeatures(int32_t videoState)
{
    uint32_t features = 0;
    if (IsVideoCall(videoState)) {
        features |= FEATURES_VIDEO;
    }
    return static_cast<int32_t>(features);
}

bool CallRecordsManager::IsVideoCall(int32_t videoState)
{
    if (static_cast<VideoStateType>(videoState) == VideoStateType::TYPE_SEND_ONLY ||
        static_cast<VideoStateType>(videoState) == VideoStateType::TYPE_VIDEO) {
        return true;
    }
    return false;
}

void CallRecordsManager::SetDataShareReady(bool isDataShareReady)
{
    isDataShareReady_ = isDataShareReady;
}

void CallRecordsManager::SetSystemAbilityAdd(bool isSystemAbilityAdd)
{
    isSystemAbilityAdd_ = isSystemAbilityAdd;
}

void CallRecordsManager::QueryUnReadMissedCallLog(int32_t userId)
{
    if (!isDataShareReady_ || !isSystemAbilityAdd_ || isUnReadMissedCallLogQuery_) {
        return;
    }
    TELEPHONY_LOGI("the user id is :%{public}d", userId);
    if (userId == ACTIVE_USER_ID && IsBasicStatementAgree()) {
        int32_t ret = DelayedSingleton<CallRecordsHandlerService>::GetInstance()->QueryUnReadMissedCallLog();
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("Query unread missed call log failed!");
            isUnReadMissedCallLogQuery_ = false;
        } else {
            isUnReadMissedCallLogQuery_ = true;
        }
    }
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
    DelayedSingleton<CallRecordsManager>::GetInstance()->SetSystemAbilityAdd(true);
    std::vector<int32_t> activeList = { 0 };
    DelayedSingleton<AppExecFwk::OsAccountManagerWrapper>::GetInstance()->QueryActiveOsAccountIds(activeList);
    TELEPHONY_LOGI("current active user id is :%{public}d", activeList[0]);
    bool isBasicStatementAgree = DelayedSingleton<CallRecordsManager>::GetInstance()->IsBasicStatementAgree();
    if (activeList[0] == ACTIVE_USER_ID && isBasicStatementAgree) {
        DelayedSingleton<CallRecordsManager>::GetInstance()->QueryUnReadMissedCallLog(activeList[0]);
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
    int32_t userId = data.GetCode();
    TELEPHONY_LOGI("action = %{public}s, current active user id is :%{public}d", action.c_str(), userId);
    bool isBasicStatementAgree = DelayedSingleton<CallRecordsManager>::GetInstance()->IsBasicStatementAgree();
    if (action == CommonEventSupport::COMMON_EVENT_USER_SWITCHED && userId == ACTIVE_USER_ID && isBasicStatementAgree) {
        DelayedSingleton<CallRecordsManager>::GetInstance()->QueryUnReadMissedCallLog(userId);
    }
}

void DataShareReadyEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    OHOS::EventFwk::Want want = data.GetWant();
    std::string action = data.GetWant().GetAction();
    TELEPHONY_LOGI("action = %{public}s", action.c_str());
    if (action == CommonEventSupport::COMMON_EVENT_DATA_SHARE_READY) {
        DelayedSingleton<CallRecordsManager>::GetInstance()->SetDataShareReady(true);
        std::vector<int32_t> activeList = { 0 };
        DelayedSingleton<AppExecFwk::OsAccountManagerWrapper>::GetInstance()->QueryActiveOsAccountIds(activeList);
        bool isBasicStatementAgree = DelayedSingleton<CallRecordsManager>::GetInstance()->IsBasicStatementAgree();
        if (activeList[0] == ACTIVE_USER_ID && isBasicStatementAgree) {
            DelayedSingleton<CallRecordsManager>::GetInstance()->QueryUnReadMissedCallLog(activeList[0]);
        }
        LocationSystemAbilityListener::SystemAbilitySubscriber();
        DelayedSingleton<CallControlManager>::GetInstance()->RegisterObserver();
#ifdef SUPPORT_MUTE_BY_DATABASE
        InteroperableSettingsHandler::RegisterObserver();
#endif
        CallStatusManager::RegisterObserver();
    }
}

bool CallRecordsManager::IsBasicStatementAgree()
{
    std::string result = "";
    auto settingHelper = SettingsDataShareHelper::GetInstance();
    if (settingHelper != nullptr) {
        OHOS::Uri settingUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI);
        settingHelper->Query(settingUri, "basic_statement_agreed", result);
    }
    TELEPHONY_LOGI("query basic statement agree result: %{public}s", result.c_str());
    if (result != SETTINGS_DB_VALUE_EXIST) {
        return false;
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS