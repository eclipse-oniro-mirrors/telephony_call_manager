/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Telephony {
constexpr int32_t DEFAULT_USER_ID = -1;
constexpr char DETECT_RESULT[] = "detectResult";
constexpr char DECISION_REASON[] = "decisionReason";
constexpr char MARK_TYPE[] = "markType";
constexpr char MARK_COUNT[] = "markCount";
constexpr char MARK_SOURCE[] = "markSource";
constexpr char MARK_CONTENT[] = "markContent";
constexpr char IS_CLOUD[] = "isCloud";
sptr<SpamCallConnection> connection_ = nullptr;

SpamCallAdapter::SpamCallAdapter()
{
    timeWaitHelper_ = std::make_unique<TimeWaitHelper>(WAIT_TIME_FIVE_SECOND);
}

SpamCallAdapter::~SpamCallAdapter()
{
    TELEPHONY_LOGI("~SpamCallAdapter");
}

bool SpamCallAdapter::DetectSpamCall(const std::string &phoneNumber, const int32_t &slotId)
{
    TELEPHONY_LOGI("DetectSpamCall start");
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
    TELEPHONY_LOGI("ConnectSpamCallAbility start");
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
    TELEPHONY_LOGI("DisconnectSpamCallAbility start");
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
            if (key == "detectResult" || key == "decisionReason") {
                return false;
            }
        }
        out = static_cast<int32_t>(cJSON_GetNumberValue(cursor));
    } while (0);
    return true;
}

bool SpamCallAdapter::JsonGetStringValue(cJSON *json, const std::string key, std::string &out)
{
    do {
        cJSON *cursor = cJSON_GetObjectItem(json, key.c_str());
        if (!cJSON_IsString(cursor)) {
            TELEPHONY_LOGE("ParseStringValue failed to get %{public}s", key.c_str());
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
    TELEPHONY_LOGI("ParseBoolValue %{public}s: %{public}d", key.c_str(), value);
    return value;
}

void SpamCallAdapter::ParseDetectResult(const std::string &jsonData, bool &isBlock,
    NumberMarkInfo &info, int32_t &blockReason)
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
    cJSON *jsonObj;
    int32_t numberValue = 0;
    std::string stringValue = "";
    if (!JsonGetNumberValue(root, DETECT_RESULT, numberValue)) {
        TELEPHONY_LOGE("ParseDetectResult no detectResult");
        cJSON_Delete(root);
        return;
    }
    TELEPHONY_LOGI("detectResult: %{public}d", numberValue);
    isBlock = numberValue == 1;
    if (!JsonGetNumberValue(root, DECISION_REASON, numberValue)) {
        TELEPHONY_LOGE("ParseDetectResult no decisionReason");
        cJSON_Delete(root);
        return;
    }
    TELEPHONY_LOGI("decisionReason: %{public}d", numberValue);
    blockReason = numberValue;
    JsonGetNumberValue(root, MARK_TYPE, numberValue);
    TELEPHONY_LOGI("markType: %{public}d", numberValue);
    info.markType = static_cast<MarkType>(numberValue);
    JsonGetNumberValue(root, MARK_COUNT, numberValue);
    info.markCount = numberValue;
    JsonGetStringValue(root, MARK_SOURCE, stringValue);
    if (strcpy_s(info.markSource, sizeof(info.markSource), stringValue.c_str()) != EOK) {
        TELEPHONY_LOGE("strcpy_s markSource fail.");
    }
    JsonGetStringValue(root, MARK_CONTENT, stringValue);
    if (strcpy_s(info.markContent, sizeof(info.markContent), stringValue.c_str()) != EOK) {
        TELEPHONY_LOGE("strcpy_s markContent fail.");
    }
    info.isCloud = JsonGetBoolValue(root, IS_CLOUD);
    TELEPHONY_LOGI("ParseDetectResult end");
    cJSON_Delete(root);
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
    if (!timeWaitHelper_->WaitForResult()) {
        DisconnectSpamCallAbility();
        return false;
    }
    DisconnectSpamCallAbility();
    return true;
}
} // namespace Telephony
} // namespace OHOS
