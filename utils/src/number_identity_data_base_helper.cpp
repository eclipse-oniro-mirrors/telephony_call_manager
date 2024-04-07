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

#include "number_identity_data_base_helper.h"

#include "ability_context.h"
#include "call_manager_errors.h"
#include "call_number_utils.h"
#include "iservice_registry.h"
#include "phonenumbers/phonenumber.pb.h"
#include "phonenumberutil.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr const char *NUMBER_IDENTITY_URI = "datashare:///numberlocationability";
const char *NUMBER_LOCATION = "number_location";

NumberIdentityDataBaseHelper::NumberIdentityDataBaseHelper() {}

NumberIdentityDataBaseHelper::~NumberIdentityDataBaseHelper() {}

std::shared_ptr<DataShare::DataShareHelper> NumberIdentityDataBaseHelper::CreateDataShareHelper(std::string uri)
{
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("Get system ability mgr failed.");
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (remoteObj == nullptr) {
        TELEPHONY_LOGE("GetSystemAbility Service Failed.");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remoteObj, uri);
}

bool NumberIdentityDataBaseHelper::Query(std::string &numberLocation, DataShare::DataSharePredicates &predicates)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(NUMBER_IDENTITY_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr");
        return false;
    }
    Uri uri(NUMBER_IDENTITY_URI);
    std::string isExactMatchStr = "true";
    std::vector<std::string> columns;
    columns.push_back(isExactMatchStr);
    auto resultSet = helper->Query(uri, predicates, columns);
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("resultSet is nullptr");
        helper->Release();
        helper = nullptr;
        return false;
    }
    int rowCount = 0;
    resultSet->GetRowCount(rowCount);
    if (rowCount == 0) {
        TELEPHONY_LOGE("query success, but rowCount is 0");
        helper->Release();
        return TELEPHONY_SUCCESS;
    }
    resultSet->GoToFirstRow();
    int columnIndex = 0;
    resultSet->GetColumnIndex(NUMBER_LOCATION, columnIndex);
    resultSet->GetString(columnIndex, numberLocation);
    resultSet->Close();
    helper->Release();
    helper = nullptr;
    TELEPHONY_LOGI("Query end");
    return true;
}
} // namespace Telephony
} // namespace OHOS