/*
 * Copyright (C) 2024-2025 Huawei Device Co., Ltd.
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

#include "spam_call_adapter.h"

#include "call_manager_base.h"
#include "call_manager_info.h"
#include "extension_manager_client.h"
#include "ipc_skeleton.h"
#include "nlohmann/json.hpp"
#include "telephony_log_wrapper.h"
#include "cJSON.h"
#include <securec.h>
#include "time_wait_helper.h"
#include "spam_call_connection.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "telephony_permission.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t DEFAULT_USER_ID = -1;
constexpr int16_t INCOMING_CALL_MISSED_CODE = 0;
constexpr char REMINDER_RESULT[] = "reminderResult";
constexpr char SLOT_ID[] = "slotId";
constexpr char DETECT_RESULT[] = "detectResult";
constexpr char DECISION_REASON[] = "decisionReason";
constexpr char MARK_TYPE[] = "markType";
constexpr char MARK_COUNT[] = "markCount";
constexpr char MARK_SOURCE[] = "markSource";
constexpr char MARK_CONTENT[] = "markContent";
constexpr char IS_CLOUD[] = "isCloud";
constexpr char MARK_DETAILS[] = "markDetails";
constexpr char DETECT_DETAILS[] = "detectDetails";
sptr<SpamCallConnection> connection_ = nullptr;

SpamCallAdapter::SpamCallAdapter()
{
    timeWaitHelper_ = std::make_unique<TimeWaitHelper>(WAIT_TIME_FIVE_SECOND);
}

SpamCallAdapter::~SpamCallAdapter()
{
    TELEPHONY_LOGW("~SpamCallAdapter");
}

bool SpamCallAdapter::DetectSpamCall(const std::string &phoneNumber, const int32_t &slotId)
{
    TELEPHONY_LOGW("DetectSpamCall start");
    phoneNumber_ = phoneNumber;
    AAFwk::Want want;
    std::string bundleName = "com.spamshield";
    std::string abilityName = "SpamShieldServiceExtAbility";
    want.SetElementName(bundleName, abilityName);
    bool connectResult = ConnectSpamCallAbility(want, phoneNumber, slotId);
    if (!connectResult) {
        TELEPHONY_LOGE("DetectSpamCall failed!");
        return false;
    }
    return true;
}

bool SpamCallAdapter::ConnectSpamCallAbility(const AAFwk::Want &want, const std::string &phoneNumber,
    const int32_t &slotId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    TELEPHONY_LOGW("ConnectSpamCallAbility start");
    connection_ = new (std::nothrow) SpamCallConnection(phoneNumber, slotId,
        shared_from_this());
    if (connection_ == nullptr) {
        TELEPHONY_LOGE("connection_ is nullptr");
        return false;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto connectResult = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want,
        connection_, nullptr, DEFAULT_USER_ID);
    IPCSkeleton::SetCallingIdentity(identity);
    if (connectResult != 0) {
        TELEPHONY_LOGE("ConnectServiceExtensionAbility Failed!");
        return false;
    }
    return true;
}

void SpamCallAdapter::DisconnectSpamCallAbility()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    TELEPHONY_LOGW("DisconnectSpamCallAbility start");
    if (connection_ == nullptr) {
        TELEPHONY_LOGE("connection_ is nullptr");
        return;
    }
    auto disconnectResult = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(connection_);
    connection_.clear();
    if (disconnectResult != 0) {
        TELEPHONY_LOGE("DisconnectAbility failed! %d", disconnectResult);
    }
}

bool SpamCallAdapter::JsonGetNumberValue(cJSON *json, const std::string key, int32_t &out)
{
    do {
        cJSON *cursor = cJSON_GetObjectItem(json, key.c_str());
        if (!cJSON_IsNumber(cursor)) {
            TELEPHONY_LOGE("ParseNumberValue failed to get %{public}s", key.c_str());
            return false;
        }
        out = static_cast<int32_t>(cJSON_GetNumberValue(cursor));
    } while (0);
    return true;
}

bool SpamCallAdapter::JsonGetStringValue(cJSON *json, const std::string key, std::string &out)
{
    do {
        out = "";
        cJSON *cursor = cJSON_GetObjectItem(json, key.c_str());
        if (!cJSON_IsString(cursor)) {
            TELEPHONY_LOGE("ParseStringValue failed to get %{public}s", key.c_str());
            return false;
        }
        char *value = cJSON_GetStringValue(cursor);
        if (value != nullptr) {
            out = value;
        }
    } while (0);
    return true;
}

bool SpamCallAdapter::JsonGetBoolValue(cJSON *json, const std::string key)
{
    cJSON *cursor = cJSON_GetObjectItem(json, key.c_str());
    bool value = cJSON_IsTrue(cursor);
    TELEPHONY_LOGW("ParseBoolValue %{public}s: %{public}d", key.c_str(), value);
    return value;
}

void SpamCallAdapter::ParseNeedNotifyResult(const std::string &jsonData)
{
    if (jsonData.empty()) {
        return;
    }
    const char *data = jsonData.c_str();
    cJSON *root = cJSON_Parse(data);
    if (root == nullptr) {
        TELEPHONY_LOGE("ParseNeedNotifyResult failed to parse JSON");
        return;
    }
    int32_t slotId = 0;
    if (!JsonGetNumberValue(root, SLOT_ID, slotId)) {
        cJSON_Delete(root);
        return;
    }
    bool result = JsonGetBoolValue(root, REMINDER_RESULT);
    TELEPHONY_LOGI("result: %{public}d, slotId: %{public}d", result, slotId);
    if (result) {
        TELEPHONY_LOGI("send notify to contacts");
        AAFwk::Want want;
        want.SetParam("isHarassmentGuidance", true);
        want.SetParam("slotId", slotId);
        want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_INCOMING_CALL_MISSED);
        EventFwk::CommonEventData eventData;
        eventData.SetWant(want);
        EventFwk::CommonEventPublishInfo publishInfo;
        std::vector<std::string> callPermissions;
        callPermissions.emplace_back(Permission::GET_TELEPHONY_STATE);
        publishInfo.SetSubscriberPermissions(callPermissions);
        if (!EventFwk::CommonEventManager::PublishCommonEvent(eventData, publishInfo, nullptr)) {
            TELEPHONY_LOGE("PublishCommonEvent fail.");
        }
    }
    cJSON_Delete(root);
}

void SpamCallAdapter::ParseDetectResult(const std::string &jsonData, bool &isBlock,
    NumberMarkInfo &info, int32_t &blockReason, std::string &detectDetails)
{
    if (jsonData.empty()) {
        return;
    }
    const char *data = jsonData.c_str();
    cJSON *root = cJSON_Parse(data);
    if (root == nullptr) {
        TELEPHONY_LOGE("ParseDetectResult failed to parse JSON");
        return;
    }
    int32_t numberValue = 0;
    if (!JsonGetNumberValue(root, DETECT_RESULT, numberValue)) {
        cJSON_Delete(root);
        return;
    }
    isBlock = numberValue == 1;
    TELEPHONY_LOGI("DetectSpamCall detectResult: %{public}d", isBlock);
    if (!JsonGetNumberValue(root, DECISION_REASON, numberValue)) {
        cJSON_Delete(root);
        return;
    }
    blockReason = numberValue;
    TELEPHONY_LOGI("DetectSpamCall decisionReason: %{public}d", blockReason);
    ParseMarkResults(info, root, detectDetails, isBlock);
    cJSON_Delete(root);
}

void SpamCallAdapter::ParseMarkResults(NumberMarkInfo &info, cJSON *root, std::string &detectDetails, bool isBlock)
{
    int32_t numberValue = 0;
    std::string stringValue = "";
    if (JsonGetNumberValue(root, MARK_TYPE, numberValue)) {
        info.markType = static_cast<MarkType>(numberValue);
    }
    TELEPHONY_LOGI("DetectSpamCall markType: %{public}d", info.markType);
    if (!isBlock && (info.markType == MarkType::MARK_TYPE_CRANK || info.markType == MarkType::MARK_TYPE_FRAUD ||
        info.markType == MarkType::MARK_TYPE_PROMOTE_SALES || info.markType == MarkType::MARK_TYPE_HOUSE_AGENT)) {
        connection_->RequireCallReminder();
    }
    if (JsonGetNumberValue(root, MARK_COUNT, numberValue)) {
        info.markCount = numberValue;
    }
    JsonGetStringValue(root, MARK_SOURCE, stringValue);
    if (strcpy_s(info.markSource, sizeof(info.markSource), stringValue.c_str()) != EOK) {
        TELEPHONY_LOGE("strcpy_s markSource fail.");
    }
    JsonGetStringValue(root, MARK_CONTENT, stringValue);
    if (strcpy_s(info.markContent, sizeof(info.markContent), stringValue.c_str()) != EOK) {
        TELEPHONY_LOGE("strcpy_s markContent fail.");
    }
    info.isCloud = JsonGetBoolValue(root, IS_CLOUD);
    JsonGetStringValue(root, MARK_DETAILS, stringValue);
    if (strcpy_s(info.markDetails, sizeof(info.markDetails), stringValue.c_str()) != EOK) {
        TELEPHONY_LOGE("strcpy_s markDetails fail.");
    }
    JsonGetStringValue(root, DETECT_DETAILS, detectDetails);
    TELEPHONY_LOGI("DetectSpamCall detectDetails length: %{public}zu", detectDetails.length());
}

void SpamCallAdapter::GetDetectResult(int32_t &errCode, std::string &result)
{
    errCode = errCode_;
    result = result_;
}

void SpamCallAdapter::SetDetectResult(int32_t &errCode, std::string &result)
{
    errCode_ = errCode;
    result_ = result;
}

void SpamCallAdapter::GetParseResult(bool &isBlock, NumberMarkInfo &info,
    int32_t &blockReason, std::string &detectDetails)
{
    isBlock = isBlock_;
    info = info_;
    blockReason = blockReason_;
    detectDetails = detectDetails_;
}

void SpamCallAdapter::SetParseResult(bool &isBlock, NumberMarkInfo &info,
    int32_t &blockReason, std::string &detectDetails)
{
    isBlock_ = isBlock;
    info_ = info;
    blockReason_ = blockReason;
    detectDetails_ = detectDetails;
}

std::string SpamCallAdapter::GetDetectPhoneNum()
{
    return phoneNumber_;
}

void SpamCallAdapter::NotifyAll()
{
    if (timeWaitHelper_ == nullptr) {
        TELEPHONY_LOGE("timeWaitHelper_ is null");
        return;
    }
    timeWaitHelper_->NotifyAll();
}

bool SpamCallAdapter::WaitForDetectResult()
{
    if (timeWaitHelper_ == nullptr) {
        TELEPHONY_LOGE("timeWaitHelper_ is null");
        return false;
    }
    if (!timeWaitHelper_->WaitForResult()) {
        DisconnectSpamCallAbility();
        return false;
    }
    DisconnectSpamCallAbility();
    return true;
}
} // namespace Telephony
} // namespace OHOS
