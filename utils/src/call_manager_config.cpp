/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "call_manager_config.h"
#include <securec.h>
#include <unistd.h>
#include <fstream>
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "os_account_manager.h"

namespace OHOS {
namespace Telephony {

constexpr const char PATH[] = "/system/etc/telephony/call_manager_config.json";
constexpr const char ITEM_HANG_UP_CELLULAR_CALL_BEFORE_ANSWER_LIST[] = "HangUpCellularCallBeforeAnswer";
constexpr const char ITEM_CONVERT_USERNAME_TO_PHONE_NUM_LIST[] = "ConvertUsernameToPhoneNum";
constexpr int MAX_BYTE_LEN = 10 * 1024;
constexpr int MAX_FEATURE_LIST_SIZE = 100;

std::unordered_set<std::string> CallManagerConfig::hangUpCellularCallBeforeAnswerList_{};
std::unordered_set<std::string> CallManagerConfig::convertUsernameToPhoneNumList_{};
std::atomic<bool> CallManagerConfig::isInit_(false);
ffrt::mutex CallManagerConfig::mutex_;

void CallManagerConfig::Init()
{
    if (isInit_.load(std::memory_order_acquire)) {
        TELEPHONY_LOGI("CallManagerConfig has init");
        return;
    }
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (isInit_.load(std::memory_order_acquire)) {
        return;
    }
    hangUpCellularCallBeforeAnswerList_.clear();
    convertUsernameToPhoneNumList_.clear();
    if (ParserConfigJson() != TELEPHONY_SUCCESS) {
        return;
    }
    TELEPHONY_LOGI("CallManagerConfig init success");
    isInit_.store(true, std::memory_order_release);
}

std::string CallManagerConfig::BuildMatchKey(int32_t uid, const std::string &methodName)
{
    std::string bundleName = "";
    int32_t userId = 0;
    if (!TelephonyPermission::GetBundleNameByUid(uid, bundleName)) {
        TELEPHONY_LOGW("%{public}s: GetBundleNameByUid failed, uid=%{public}d", methodName.c_str(), uid);
        return "";
    }
    if (AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(uid, userId) != 0) {
        TELEPHONY_LOGW("%{public}s: GetOsAccountLocalIdFromUid failed, uid=%{public}d", methodName.c_str(), uid);
        return "";
    }
    std::string appIdentifier = "";
    TelephonyPermission::GetAppIdentifier(bundleName, appIdentifier, userId);
    std::string matchKey = bundleName + ":" + appIdentifier;
    return matchKey;
}

bool CallManagerConfig::ShouldHangUpCellularCallBeforeAnswer(int32_t uid)
{
    Init();
    std::string matchKey = BuildMatchKey(uid, std::string("HangUpCellularCallBeforeAnswer"));
    if (matchKey.empty()) {
        return false;
    }
    return hangUpCellularCallBeforeAnswerList_.find(matchKey) != hangUpCellularCallBeforeAnswerList_.end();
}

bool CallManagerConfig::ShouldConvertUsernameToPhoneNum(int32_t uid)
{
    Init();
    std::string matchKey = BuildMatchKey(uid, std::string("ConvertUsernameToPhoneNum"));
    if (matchKey.empty()) {
        return false;
    }
    return convertUsernameToPhoneNumList_.find(matchKey) != convertUsernameToPhoneNumList_.end();
}

int32_t CallManagerConfig::ParserConfigJson()
{
    std::unique_ptr<char[]> content;
    int32_t ret = LoaderJsonFile(content, PATH);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("CallManagerConfig: load fail");
        return ret;
    }
    if (content == nullptr) {
        TELEPHONY_LOGW("CallManagerConfig: content is nullptr");
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }

    cJSON *root = cJSON_Parse(content.get());
    if (root == nullptr) {
        TELEPHONY_LOGW("CallManagerConfig: json root is error");
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }

    ParseFeatureList(root, ITEM_HANG_UP_CELLULAR_CALL_BEFORE_ANSWER_LIST, hangUpCellularCallBeforeAnswerList_);
    ParseFeatureList(root, ITEM_CONVERT_USERNAME_TO_PHONE_NUM_LIST, convertUsernameToPhoneNumList_);

    cJSON_Delete(root);
    root = nullptr;

    return TELEPHONY_SUCCESS;
}

void CallManagerConfig::ParseFeatureList(const cJSON *root, const std::string &itemName,
    std::unordered_set<std::string> &featureList)
{
    cJSON *featureListItem = cJSON_GetObjectItem(root, itemName.c_str());
    if (featureListItem != nullptr && cJSON_IsArray(featureListItem)) {
        int size = cJSON_GetArraySize(featureListItem);
        if (size <= 0 || size > MAX_FEATURE_LIST_SIZE) {
            TELEPHONY_LOGW("CallManagerConfig: %{public}s size invalid, size=%{public}d", itemName.c_str(), size);
            return;
        }
        for (int i = 0; i < size; i++) {
            cJSON *item = cJSON_GetArrayItem(featureListItem, i);
            std::string bundleName = ParseString(item);
            if (!bundleName.empty()) {
                featureList.insert(bundleName);
            }
        }
        TELEPHONY_LOGI("CallManagerConfig loaded %{public}s with %{public}zu items",
            itemName.c_str(), featureList.size());
    } else {
        TELEPHONY_LOGW("CallManagerConfig: %{public}s is invalid", itemName.c_str());
    }
}

int32_t CallManagerConfig::LoaderJsonFile(std::unique_ptr<char[]> &content, const std::string &path)
{
    char realPath[PATH_MAX] = { 0x00 };
    if (realpath(path.c_str(), realPath) == nullptr) {
        TELEPHONY_LOGW("CallManagerConfig: realpath fail");
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }
    std::ifstream file(realPath, std::ios::ate);
    if (!file.is_open()) {
        TELEPHONY_LOGW("CallManagerConfig: file open fail");
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }
    std::streamsize len = file.tellg();
    if (len <= 0 || len > static_cast<std::streamsize>(MAX_BYTE_LEN)) {
        TELEPHONY_LOGW("CallManagerConfig: invalid file size");
        file.close();
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }
    file.seekg(0, std::ios::beg);
    content = std::make_unique<char[]>(len + 1);
    if (content == nullptr) {
        TELEPHONY_LOGW("CallManagerConfig: malloc fail");
        file.close();
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }
    if (memset_s(content.get(), len + 1, 0, len + 1) != EOK) {
        TELEPHONY_LOGW("CallManagerConfig: memset_s fail");
        content.reset();
        file.close();
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }
    if (!file.read(content.get(), len)) {
        TELEPHONY_LOGW("CallManagerConfig: file read fail");
        content.reset();
        file.close();
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }
    file.close();
    return TELEPHONY_SUCCESS;
}

std::string CallManagerConfig::ParseString(const cJSON *value)
{
    if (value != nullptr && value->type == cJSON_String && value->valuestring != nullptr) {
        return value->valuestring;
    }
    return "";
}

} // namespace Telephony
} // namespace OHOS