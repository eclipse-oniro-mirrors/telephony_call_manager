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

#include "settings_datashare_helper.h"

#include "call_dialog.h"
#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "iservice_registry.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "uri.h"
#include "singleton.h"
#include "os_account_manager.h"

namespace OHOS {
namespace Telephony {
const std::string SettingsDataShareHelper::SETTINGS_DATASHARE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
const std::string SettingsDataShareHelper::SETTINGS_DATASHARE_SECURE_URI_BASE =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_SECURE_";
const std::string SETTINGS_DATASHARE_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
constexpr const char *SETTINGS_DATA_COLUMN_KEYWORD = "KEYWORD";
constexpr const char *SETTINGS_DATA_COLUMN_VALUE = "VALUE";
const std::string SettingsDataShareHelper::QUERY_SATELLITE_MODE_KEY = "satellite_mode_switch";
const std::string SettingsDataShareHelper::QUERY_SATELLITE_CONNECTED_KEY = "satellite_connected";
const std::string SettingsDataShareHelper::QUERY_MOTION_FLIP_MUTE_KEY = "motion_flip_mute_call";
const std::string SettingsDataShareHelper::QUERY_MOTION_CLOSE_TO_EAR_KEY = "motion_quick_answer";
const std::string SettingsDataShareHelper::QUERY_MOTION_PICKUP_REDUCE_KEY = "motion_pickup_reduce_call";

SettingsDataShareHelper::SettingsDataShareHelper() = default;

SettingsDataShareHelper::~SettingsDataShareHelper() = default;

std::shared_ptr<DataShare::DataShareHelper> SettingsDataShareHelper::CreateDataShareHelper(int systemAbilityId)
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("GetSystemAbilityManager failed.");
        return nullptr;
    }
    sptr<IRemoteObject> remote = saManager->GetSystemAbility(systemAbilityId);
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSystemAbility Service Failed.");
        return nullptr;
    }
    TELEPHONY_LOGI("systemAbilityId = %{public}d", systemAbilityId);
    return DataShare::DataShareHelper::Creator(remote, SETTINGS_DATASHARE_URI, SETTINGS_DATASHARE_EXT_URI);
}

std::shared_ptr<DataShare::DataShareHelper> SettingsDataShareHelper::CreateDataShareSecureHelper(
    int systemAbilityId)
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("GetSystemAbilityManager failed.");
        return nullptr;
    }
    sptr<IRemoteObject> remote = saManager->GetSystemAbility(systemAbilityId);
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSystemAbility Service Failed.");
        return nullptr;
    }
    TELEPHONY_LOGI("systemAbilityId = %{public}d", systemAbilityId);
    int32_t userId = 0;
    AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(userId);
    std::string strUri = SETTINGS_DATASHARE_SECURE_URI_BASE + std::to_string(userId) + "?Proxy=true";
    return DataShare::DataShareHelper::Creator(remote, strUri, SETTINGS_DATASHARE_EXT_URI);
}

int32_t SettingsDataShareHelper::Query(Uri& uri, const std::string& key, std::string& value)
{
    TELEPHONY_LOGI("start Query");
    std::shared_ptr<DataShare::DataShareHelper> settingHelper =
        CreateDataShareHelper(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (settingHelper == nullptr) {
        TELEPHONY_LOGE("query error, datashareHelper_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    auto result = settingHelper->Query(uri, predicates, columns);
    settingHelper->Release();
    if (result == nullptr) {
        TELEPHONY_LOGE("query error, result is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int rowCount = 0;
    result->GetRowCount(rowCount);
    if (rowCount == 0) {
        TELEPHONY_LOGW("query success, but rowCount is 0");
        return TELEPHONY_ERROR;
    }

    if (result->GoToFirstRow() != DataShare::E_OK) {
        TELEPHONY_LOGE("query error, go to first row error");
        result->Close();
        return TELEPHONY_ERR_DATABASE_READ_FAIL;
    }

    int columnIndex = 0;
    result->GetColumnIndex(SETTINGS_DATA_COLUMN_VALUE, columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    TELEPHONY_LOGI("SettingUtils: query success");
    return TELEPHONY_SUCCESS;
}

int32_t SettingsDataShareHelper::QuerySecure(Uri& uri, const std::string& key, std::string& value)
{
    TELEPHONY_LOGI("start Query");
    std::shared_ptr<DataShare::DataShareHelper> settingHelper =
        CreateDataShareSecureHelper(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (settingHelper == nullptr) {
        TELEPHONY_LOGE("query error, settingHelper is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    auto result = settingHelper->Query(uri, predicates, columns);
    settingHelper->Release();
    if (result == nullptr) {
        TELEPHONY_LOGE("query error, result is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int rowCount = 0;
    result->GetRowCount(rowCount);
    if (rowCount == 0) {
        TELEPHONY_LOGW("query success, but rowCount is 0");
        return TELEPHONY_ERROR;
    }

    if (result->GoToFirstRow() != DataShare::E_OK) {
        TELEPHONY_LOGE("query error, go to first row error");
        result->Close();
        return TELEPHONY_ERR_DATABASE_READ_FAIL;
    }

    int columnIndex = 0;
    result->GetColumnIndex(SETTINGS_DATA_COLUMN_VALUE, columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    TELEPHONY_LOGI("SettingUtils: query success");
    return TELEPHONY_SUCCESS;
}

int32_t SettingsDataShareHelper::UpdateSecure(Uri &uri, const std::string &key, const std::string &value)
{
    std::shared_ptr<DataShare::DataShareHelper> settingHelper =
        CreateDataShareSecureHelper(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (settingHelper == nullptr) {
        TELEPHONY_LOGE("query error, settingHelper is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    DataShare::DataShareValueObject valueObj(value);
    DataShare::DataShareValuesBucket valueBucket;
    valueBucket.Put(SETTINGS_DATA_COLUMN_VALUE, valueObj);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    int32_t ret = settingHelper->Update(uri, predicates, valueBucket);
    if (ret <= 0) {
        TELEPHONY_LOGE("update failed, retCode:%{public}d", ret);
        settingHelper->Release();
        return TELEPHONY_ERR_DATABASE_WRITE_FAIL;
    }
    settingHelper->NotifyChange(uri);
    settingHelper->Release();
    TELEPHONY_LOGI("SettingUtils: update success");
    return TELEPHONY_SUCCESS;
}

int32_t SettingsDataShareHelper::Insert(Uri &uri, const std::string &key, const std::string &value)
{
    std::shared_ptr<DataShare::DataShareHelper> settingHelper =
        CreateDataShareHelper(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (settingHelper == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    DataShare::DataShareValueObject keyObj(key);
    DataShare::DataShareValueObject valueObj(value);
    DataShare::DataShareValuesBucket valueBucket;
    valueBucket.Put(SETTINGS_DATA_COLUMN_KEYWORD, keyObj);
    valueBucket.Put(SETTINGS_DATA_COLUMN_VALUE, valueObj);
    int32_t ret = settingHelper->Insert(uri, valueBucket);
    if (ret <= 0) {
        TELEPHONY_LOGE("DataShareHelper insert failed, retCode:%{public}d", ret);
        settingHelper->Release();
        return TELEPHONY_ERROR;
    }
    settingHelper->NotifyChange(uri);
    settingHelper->Release();
    return TELEPHONY_SUCCESS;
}
 
int32_t SettingsDataShareHelper::Update(Uri &uri, const std::string &key, const std::string &value)
{
    std::string queryValue = "";
    if (Query(uri, key, queryValue) != 0) {
        return Insert(uri, key, value);
    }
    std::shared_ptr<DataShare::DataShareHelper> settingHelper =
        CreateDataShareHelper(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (settingHelper == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    DataShare::DataShareValueObject valueObj(value);
    DataShare::DataShareValuesBucket valueBucket;
    valueBucket.Put(SETTINGS_DATA_COLUMN_VALUE, valueObj);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    int32_t ret = settingHelper->Update(uri, predicates, valueBucket);
    if (ret <= 0) {
        TELEPHONY_LOGE("DataShareHelper update failed, retCode:%{public}d", ret);
        settingHelper->Release();
        return TELEPHONY_ERROR;
    }
    settingHelper->NotifyChange(uri);
    settingHelper->Release();
    return TELEPHONY_SUCCESS;
}
 
bool SettingsDataShareHelper::RegisterToDataShare(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &observer)
{
    std::shared_ptr<DataShare::DataShareHelper> settingHelper =
        CreateDataShareHelper(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (settingHelper == nullptr) {
        TELEPHONY_LOGE("settingHelper is null");
        return false;
    }
    settingHelper->RegisterObserver(uri, observer);
    settingHelper->Release();
    TELEPHONY_LOGI("SettingsDataShareHelper:Register observer success");
    return true;
}

bool SettingsDataShareHelper::UnRegisterToDataShare(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &observer)
{
    std::shared_ptr<DataShare::DataShareHelper> settingHelper =
        CreateDataShareHelper(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (settingHelper == nullptr) {
        TELEPHONY_LOGE("settingHelper is null");
        return false;
    }
    settingHelper->UnregisterObserver(uri, observer);
    settingHelper->Release();
    TELEPHONY_LOGI("SettingsDataShareHelper:UnRegister observer success");
    return true;
}
} // namespace Telephony
} // namespace OHOS