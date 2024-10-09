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

#include "call_data_base_helper.h"

#include "ability_context.h"
#include "call_manager_errors.h"
#include "call_number_utils.h"
#include "iservice_registry.h"
#include "phonenumbers/phonenumber.pb.h"
#include "phonenumberutil.h"
#include "telephony_log_wrapper.h"
#include "tel_cust_manager.h"

namespace OHOS {
namespace Telephony {
class AbsSharedResultSet;
static constexpr const char *CALLLOG_URI = "datashare:///com.ohos.calllogability";
static constexpr const char *CALL_SUBSECTION = "datashare:///com.ohos.calllogability/calls/calllog";
static constexpr const char *CONTACT_URI = "datashare:///com.ohos.contactsdataability";
static constexpr const char *CALL_BLOCK = "datashare:///com.ohos.contactsdataability/contacts/contact_blocklist";
static constexpr const char *CONTACT_DATA = "datashare:///com.ohos.contactsdataability/contacts/contact_data";
static constexpr const char *ISO_COUNTRY_CODE = "CN";
static constexpr const char *SETTINGS_DATA_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
static constexpr const char *SETTINGS_DATA_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
static constexpr const char *SETTINGS_AIRPLANE_MODE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=airplane_mode";
static constexpr const char *SETTINGS_AIRPLANE_MODE = "settings.telephony.airplanemode";
static constexpr const int32_t MAX_WAITIME_TIME = 10;
constexpr int32_t E_OK = 0;

CallDataRdbObserver::CallDataRdbObserver(std::vector<std::string> *phones)
{
    if (phones == nullptr) {
        TELEPHONY_LOGE("phones is nullptr");
    }
    this->phones = phones;
}

CallDataRdbObserver::~CallDataRdbObserver() {}

void CallDataRdbObserver::OnChange()
{
    std::shared_ptr<CallDataBaseHelper> callDataPtr = DelayedSingleton<CallDataBaseHelper>::GetInstance();
    if (callDataPtr == nullptr) {
        TELEPHONY_LOGE("callDataPtr is nullptr!");
        return;
    }

    DataShare::DataSharePredicates predicates;
    predicates.NotEqualTo("phone_number", std::string(""));
    this->phones->clear();
    callDataPtr->Query(this->phones, predicates);
}

CallDataBaseHelper::CallDataBaseHelper() {}

CallDataBaseHelper::~CallDataBaseHelper() {}

std::shared_ptr<DataShare::DataShareHelper> CallDataBaseHelper::CreateDataShareHelper(std::string uri)
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
    if (uri == SETTINGS_DATA_URI) {
        return DataShare::DataShareHelper::Creator(remoteObj, uri, SETTINGS_DATA_EXT_URI);
    }
    return DataShare::DataShareHelper::Creator(remoteObj, uri, "", MAX_WAITIME_TIME);
}

void CallDataBaseHelper::RegisterObserver(std::vector<std::string> *phones)
{
    if (phones == nullptr) {
        TELEPHONY_LOGE("phones is nullptr");
        return;
    }
    callDataRdbObserverPtr_ = new (std::nothrow) CallDataRdbObserver(phones);
    if (callDataRdbObserverPtr_ == nullptr) {
        TELEPHONY_LOGE("callDataRdbObserverPtr_ is null");
        return;
    }
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CONTACT_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is null");
        return;
    }
    Uri uri(CALL_BLOCK);
    helper->RegisterObserver(uri, callDataRdbObserverPtr_);
    helper->Release();
}

void CallDataBaseHelper::UnRegisterObserver()
{
    if (callDataRdbObserverPtr_ == nullptr) {
        TELEPHONY_LOGE("callDataRdbObserverPtr_ is null");
        return;
    }
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CONTACT_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper_ is null");
        return;
    }
    Uri uri(CALL_BLOCK);
    helper->UnregisterObserver(uri, callDataRdbObserverPtr_);
    helper->Release();
}

bool CallDataBaseHelper::Insert(DataShare::DataShareValuesBucket &values)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CALLLOG_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr!");
        return false;
    }
    Uri uri(CALL_SUBSECTION);
    bool result = (helper->Insert(uri, values) > 0);
    helper->Release();
    return result;
}

bool CallDataBaseHelper::Query(std::vector<std::string> *phones, DataShare::DataSharePredicates &predicates)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CONTACT_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr");
        return false;
    }
    Uri uri(CALL_BLOCK);
    std::vector<std::string> columns;
    columns.push_back("phone_number");
    auto resultSet = helper->Query(uri, predicates, columns);
    if (resultSet == nullptr) {
        helper->Release();
        return false;
    }
    int32_t resultSetNum = resultSet->GoToFirstRow();
    while (resultSetNum == 0) {
        std::string phone;
        int32_t columnIndex;
        resultSet->GetColumnIndex("phone_number", columnIndex);
        int32_t ret = resultSet->GetString(columnIndex, phone);
        if (ret == 0 && (!phone.empty())) {
            phones->push_back(phone);
        }
        resultSetNum = resultSet->GoToNextRow();
    }
    resultSet->Close();
    helper->Release();
    TELEPHONY_LOGI("Query end");
    return true;
}

bool CallDataBaseHelper::Query(ContactInfo &contactInfo, DataShare::DataSharePredicates &predicates)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CONTACT_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr");
        return false;
    }
    Uri uri(CONTACT_DATA);
    std::vector<std::string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    if (!CheckResultSet(resultSet)) {
        helper->Release();
        return false;
    }
    int32_t ret = 0;
    if (contactInfo.number.length() >= static_cast<size_t>(QUERY_CONTACT_LEN)) {
        int resultId = TelCustManager::GetInstance().GetCallerIndex(resultSet, contactInfo.number);
        TELEPHONY_LOGI("QueryCallerInfo use enhanced query, index: %{public}d", resultId);
        ret = resultSet->GoToRow(resultId);
    } else {
        TELEPHONY_LOGI("QueryCallerInfo use normal query");
        ret = resultSet->GoToFirstRow();
    }
    if (ret != E_OK) {
        TELEPHONY_LOGE("GoToRow failed");
        resultSet->Close();
        helper->Release();
        return false;
    }
    int32_t columnIndex;
    resultSet->GetColumnIndex(CALL_DISPLAY_NAME, columnIndex);
    resultSet->GetString(columnIndex, contactInfo.name);
    resultSet->Close();
    helper->Release();
    TELEPHONY_LOGI("Query end, contactName length: %{public}zu", contactInfo.name.length());
    return true;
}

bool CallDataBaseHelper::QueryCallLog(
    std::map<std::string, int32_t> &phoneNumAndUnreadCountMap, DataShare::DataSharePredicates &predicates)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CALLLOG_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr!");
        return false;
    }
    Uri uri(CALL_SUBSECTION);
    std::vector<std::string> columns;
    columns.push_back(CALL_PHONE_NUMBER);
    auto resultSet = helper->Query(uri, predicates, columns);
    if (resultSet == nullptr) {
        helper->Release();
        return false;
    }
    int32_t operationResult = resultSet->GoToFirstRow();
    while (operationResult == TELEPHONY_SUCCESS) {
        std::string phoneNumber = "";
        int32_t columnIndex = 0;
        resultSet->GetColumnIndex(CALL_PHONE_NUMBER, columnIndex);
        operationResult = resultSet->GetString(columnIndex, phoneNumber);
        if (operationResult == TELEPHONY_SUCCESS && (!phoneNumber.empty())) {
            auto iter = phoneNumAndUnreadCountMap.find(phoneNumber);
            if (iter != phoneNumAndUnreadCountMap.end()) {
                iter->second++;
            } else {
                phoneNumAndUnreadCountMap.insert(
                    std::map<std::string, int32_t>::value_type(phoneNumber, CALL_LOG_DEFAULT_COUNT));
            }
        }
        operationResult = resultSet->GoToNextRow();
    }
    resultSet->Close();
    helper->Release();
    TELEPHONY_LOGI("QueryCallLog end");
    return true;
}

bool CallDataBaseHelper::QueryIdsNeedToDelete(std::vector<int32_t> &needDeleteIds,
    DataShare::DataSharePredicates &predicates)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CALLLOG_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr!");
        return false;
    }
    Uri uri(CALL_SUBSECTION);
    std::vector<std::string> columns;
    columns.push_back(CALL_ID);
    auto resultSet = helper->Query(uri, predicates, columns);
    if (resultSet == nullptr) {
        helper->Release();
        return false;
    }
    int32_t operationResult = resultSet->GoToFirstRow();
    while (operationResult == TELEPHONY_SUCCESS) {
        int32_t id = 0;
        int32_t columnIndex = 0;
        resultSet->GetColumnIndex(CALL_ID, columnIndex);
        operationResult = resultSet->GetInt(columnIndex, id);
        if (operationResult == TELEPHONY_SUCCESS) {
            TELEPHONY_LOGI("need delete call log id: %{public}d", id);
            needDeleteIds.push_back(id);
        }
        operationResult = resultSet->GoToNextRow();
    }
    resultSet->Close();
    helper->Release();
    TELEPHONY_LOGI("QueryIdsNeedToDelete end");
    return true;
}

bool CallDataBaseHelper::Update(DataShare::DataSharePredicates &predicates, DataShare::DataShareValuesBucket &values)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CALLLOG_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr");
        return true;
    }
    Uri uri(CALL_SUBSECTION);
    bool result = (helper->Update(uri, predicates, values) > 0);
    helper->Release();
    return result;
}

bool CallDataBaseHelper::Delete(DataShare::DataSharePredicates &predicates)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CALLLOG_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr!");
        return false;
    }
    Uri uri(CALL_SUBSECTION);
    bool result = (helper->Delete(uri, predicates) > 0);
    TELEPHONY_LOGI("delete result: %{public}d", result);
    helper->Release();
    return result;
}

int32_t CallDataBaseHelper::QueryIsBlockPhoneNumber(const std::string &phoneNum, bool &result)
{
    result = false;
    std::shared_ptr<DataShare::DataShareHelper> callDataHelper = CreateDataShareHelper(CALLLOG_URI);
    if (callDataHelper == nullptr) {
        TELEPHONY_LOGE("callDataHelper is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    Uri uri(CALL_BLOCK);
    DataShare::DataSharePredicates predicates;
    std::vector<std::string> columns;
    std::string internationalNumber;
    std::string nationalNumber;
    int32_t ret = DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToNational(
        phoneNum, ISO_COUNTRY_CODE, nationalNumber);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Format phone number failed.");
        nationalNumber = phoneNum;
    }
    ret = DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToInternational(
        phoneNum, ISO_COUNTRY_CODE, internationalNumber);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Format phone number failed.");
        internationalNumber = phoneNum;
    }
    predicates.EqualTo(CALL_PHONE_NUMBER, nationalNumber)->Or()->EqualTo(CALL_PHONE_NUMBER, internationalNumber);
    auto resultSet = callDataHelper->Query(uri, predicates, columns);
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("Query Result Set nullptr Failed.");
        callDataHelper->Release();
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t count = 0;
    if (resultSet->GetRowCount(count) == E_OK && count != 0) {
        result = true;
    }
    TELEPHONY_LOGI("count: %{public}d", count);
    resultSet->Close();
    callDataHelper->Release();
    return TELEPHONY_SUCCESS;
}

int32_t CallDataBaseHelper::GetAirplaneMode(bool &isAirplaneModeOn)
{
    std::shared_ptr<DataShare::DataShareHelper> callDataHelper = CreateDataShareHelper(SETTINGS_DATA_URI);
    if (callDataHelper == nullptr) {
        TELEPHONY_LOGE("callDataHelper is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    Uri uri(SETTINGS_AIRPLANE_MODE_URI);
    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTING_KEY, SETTINGS_AIRPLANE_MODE);
    auto result = callDataHelper->Query(uri, predicates, columns);
    if (result == nullptr) {
        TELEPHONY_LOGE("CallDataBaseHelper: query error, result is null");
        callDataHelper->Release();
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (result->GoToFirstRow() != DataShare::E_OK) {
        TELEPHONY_LOGE("CallDataBaseHelper: query error, go to first row error");
        result->Close();
        callDataHelper->Release();
        return TELEPHONY_ERR_DATABASE_READ_FAIL;
    }
    int32_t columnindex = 0;
    std::string value = "";
    result->GetColumnIndex(SETTING_VALUE, columnindex);
    result->GetString(columnindex, value);
    result->Close();
    callDataHelper->Release();
    isAirplaneModeOn = value == "1";
    TELEPHONY_LOGI("Get airplane mode:%{public}d", isAirplaneModeOn);
    return TELEPHONY_SUCCESS;
}

bool CallDataBaseHelper::CheckResultSet(std::shared_ptr<DataShare::DataShareResultSet> resultSet) {
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("resultSet is nullptr");
        return false;
    }
    int rowCount = 0;
    resultSet->GetRowCount(rowCount);
    if (rowCount == 0) {
        TELEPHONY_LOGE("query success, but rowCount is 0");
        resultSet->Close();
        return false;
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS
