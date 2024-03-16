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

#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "iservice_registry.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "uri.h"

namespace OHOS {
namespace Telephony {
namespace {
constexpr const char *SETTINGS_DATASHARE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
constexpr const char *SETTINGS_DATA_COLUMN_KEYWORD = "KEYWORD";
constexpr const char *SETTINGS_DATA_COLUMN_VALUE = "VALUE";
}

SettingsDataShareHelper::SettingsDataShareHelper()
{
    datashareHelper_ = CreateDataShareHelper(DEVICE_STANDBY_SERVICE_SYSTEM_ABILITY_ID);
}

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
    return DataShare::DataShareHelper::Creator(remote, SETTINGS_DATASHARE_URI);
}

int32_t SettingsDataShareHelper::Query(Uri& uri, const std::string& key, std::string& value)
{
    TELEPHONY_LOGI("start Query");
    if (datashareHelper_ == nullptr) {
        TELEPHONY_LOGE("query error, datashareHelper_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    auto result = datashareHelper_->Query(uri, predicates, columns);
    if (result == nullptr) {
        TELEPHONY_LOGE("query error, result is nullptr");
        datashareHelper_->Release();
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    if (result->GoToFirstRow() != DataShare::E_OK) {
        TELEPHONY_LOGE("query error, go to first row error");
        result->Close();
        datashareHelper_->Release();
        return TELEPHONY_ERR_DATABASE_READ_FAIL;
    }

    int columnIndex = 0;
    result->GetColumnIndex(SETTINGS_DATA_COLUMN_VALUE, columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    datashareHelper_->Release();
    TELEPHONY_LOGI("SettingUtils: query success");
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS