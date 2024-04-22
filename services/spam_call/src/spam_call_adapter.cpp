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
std::condition_variable SpamCallAdapter::cv_;

SpamCallAdapter::SpamCallAdapter() {}

SpamCallAdapter::~SpamCallAdapter() {}

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
    TELEPHONY_LOGI("ConnectSpamCallAbility start");
    connection_ = sptr<SpamCallConnection> (new (std::nothrow) SpamCallConnection(phoneNumber, slotId));
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
bool SpamCallAdapter::GetDetectFlag()
{
    return isDetected_;
}

void SpamCallAdapter::SetDetectFlag(bool isDetected)
{
    isDetected_ = isDetected;
}

void SpamCallAdapter::ParseDetectResult(const std::string &jsonData, bool &isBlock, NumberMarkInfo &info, int32_t &blockReason)
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
    auto stringValue = "";
    JSON_GET_NUMBER_VALUE(root, std::string(DETECT_RESULT), jsonObj, numberValue, goto finally);
    isBlock = numberValue == 1;
    JSON_GET_NUMBER_VALUE(root, std::string(DECISION_REASON), jsonObj, numberValue, goto finally);
    blockReason = numberValue;
    JSON_GET_NUMBER_VALUE(root, std::string(MARK_TYPE), jsonObj, numberValue, goto finally);
    info.markType = static_cast<MarkType>(numberValue);
    JSON_GET_NUMBER_VALUE(root, std::string(MARK_COUNT), jsonObj, numberValue, goto finally);
    info.markCount = numberValue;
    JSON_GET_STRING_VALUE(root, std::string(MARK_SOURCE), jsonObj, stringValue, goto finally);
    if (stringValue != nullptr && strcpy_s(info.markContent, sizeof(info.markContent), stringValue) != EOK) {
        TELEPHONY_LOGE("strcpy_s markContent fail.");
    }
    JSON_GET_STRING_VALUE(root, std::string(MARK_CONTENT), jsonObj, stringValue, goto finally);
    if (stringValue != nullptr && strcpy_s(info.markContent, sizeof(info.markContent), stringValue) != EOK) {
        TELEPHONY_LOGE("strcpy_s markContent fail.");
    }
    JSON_GET_NUMBER_VALUE(root, std::string(IS_CLOUD), jsonObj, numberValue, goto finally);
    info.isCloud = numberValue == 1;
finally:
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
    TELEPHONY_LOGI("SpamCallDetect NotifyAll");
    cv_.notify_all();
}

bool SpamCallAdapter::WaitForDetectResult()
{
    // if (!isDetected_) {
        std::unique_lock<std::mutex> lock(mutex_);
        isDetected_ = false;
        while (!isDetected_) {
            if (cv_.wait_for(lock, std::chrono::seconds(WAIT_TIME_TWO_SECOND)) == std::cv_status::timeout) {
                TELEPHONY_LOGE("detect spam call is time out");
                return false;
            }
        }
    // }
    return true;
}
} // namespace Telephony
} // namespace OHOS
