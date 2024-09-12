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
static constexpr const char *NUMBER_MARK_INFO_URI = "datashare:///numbermarkability/number_mark_info";
const char *NUMBER_LOCATION = "number_location";
const char *MARK_TYPE = "markType";
const char *MARK_CONTENT = "markContent";
const char *MARK_COUNT = "markCount";
const char *MARK_SOURCE = "markSource";
const char *IS_CLOUD = "isCloud";
const char *MARK_DETAILS = "markDetails";

NumberIdentityDataBaseHelper::NumberIdentityDataBaseHelper() {}

NumberIdentityDataBaseHelper::~NumberIdentityDataBaseHelper() {}

std::shared_ptr<DataShare::DataShareHelper> NumberIdentityDataBaseHelper::CreateDataShareHelper(std::string uri)
{
    TELEPHONY_LOGI("uri: %{public}s", uri.c_str());
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

bool NumberIdentityDataBaseHelper::QueryYellowPageAndMark(NumberMarkInfo &numberMarkInfo,
    DataShare::DataSharePredicates &predicates)
{
    TELEPHONY_LOGE("query yellow page and mark.");
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(NUMBER_MARK_INFO_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr");
        return false;
    }
    Uri uri(NUMBER_MARK_INFO_URI);
    std::vector<std::string> columns;
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
    SetMarkInfoValues(resultSet, numberMarkInfo);

    resultSet->Close();
    helper->Release();
    helper = nullptr;
    TELEPHONY_LOGI("QueryYellowPageAndMark success.");
    return true;
}

bool NumberIdentityDataBaseHelper::SetMarkInfoValues(std::shared_ptr<DataShare::DataShareResultSet> &resultSet,
    NumberMarkInfo &numberMarkInfo)
{
    resultSet->GoToFirstRow();
    int columnIndex = 0;
    int64_t longValue;
    std::string stringValue = "";

    resultSet->GetColumnIndex(MARK_TYPE, columnIndex);
    resultSet->GetLong(columnIndex, longValue);
    numberMarkInfo.markType = static_cast<MarkType>(longValue);

    resultSet->GetColumnIndex(MARK_CONTENT, columnIndex);
    resultSet->GetString(columnIndex, stringValue);
    if (memcpy_s(numberMarkInfo.markContent, kMaxNumberLen, stringValue.c_str(), stringValue.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return false;
    }

    resultSet->GetColumnIndex(MARK_COUNT, columnIndex);
    resultSet->GetLong(columnIndex, longValue);
    numberMarkInfo.markCount = longValue;

    stringValue = "";
    resultSet->GetColumnIndex(MARK_SOURCE, columnIndex);
    resultSet->GetString(columnIndex, stringValue);
    if (memcpy_s(numberMarkInfo.markSource, kMaxNumberLen, stringValue.c_str(), stringValue.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return false;
    }

    resultSet->GetColumnIndex(IS_CLOUD, columnIndex);
    resultSet->GetLong(columnIndex, longValue);
    if (longValue == 1L) {
        numberMarkInfo.isCloud = true;
    } else {
        numberMarkInfo.isCloud = false;
    }

    stringValue = "";
    resultSet->GetColumnIndex(MARK_DETAILS, columnIndex);
    resultSet->GetString(columnIndex, stringValue);
    if (memcpy_s(numberMarkInfo.markDetails, kMaxNumberLen, stringValue.c_str(), stringValue.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return false;
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS