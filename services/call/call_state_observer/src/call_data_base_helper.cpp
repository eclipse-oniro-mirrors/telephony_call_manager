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

#include "iservice_registry.h"

#include "ability_context.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
class AbsSharedResultSet;
static constexpr const char *CALLLOG_URI = "datashare:///com.ohos.calllogability";
static constexpr const char *CALL_SUBSECTION = "datashare:///com.ohos.calllogability/calls/calllog";
static constexpr const char *CONTACT_URI = "datashare:///com.ohos.contactsdataability";
static constexpr const char *CALL_BLOCK = "datashare:///com.ohos.contactsdataability/contacts/contact_blocklist";
static constexpr const char *CONTACT_DATA = "datashare:///com.ohos.contactsdataability/contacts/contact_data";

CallDataRdbObserver::CallDataRdbObserver(std::vector<std::string> *phones)
{
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
    return DataShare::DataShareHelper::Creator(remoteObj, uri);
}

void CallDataBaseHelper::RegisterObserver(std::vector<std::string> *phones)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CONTACT_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper_ is null");
        return;
    }
    Uri uri(CALL_BLOCK);
    callDataRdbObserverPtr_ = new (std::nothrow) CallDataRdbObserver(phones);
    if (callDataRdbObserverPtr_ == nullptr) {
        TELEPHONY_LOGE("callDataRdbObserverPtr_ is null");
        return;
    }
    helper->RegisterObserver(uri, callDataRdbObserverPtr_);
}

void CallDataBaseHelper::UnRegisterObserver()
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CONTACT_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper_ is null");
        return;
    }
    Uri uri(CALL_BLOCK);
    if (callDataRdbObserverPtr_ == nullptr) {
        TELEPHONY_LOGE("callDataRdbObserverPtr_ is null");
        return;
    }
    helper->UnregisterObserver(uri, callDataRdbObserverPtr_);
}

bool CallDataBaseHelper::Insert(DataShare::DataShareValuesBucket &values)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CALLLOG_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr!");
        return false;
    }
    Uri uri(CALL_SUBSECTION);
    return helper->Insert(uri, values);
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
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("resultSet is nullptr");
        return false;
    }
    int32_t resultSetNum = resultSet->GoToFirstRow();
    while (resultSetNum == 0) {
        std::string displayName;
        int32_t columnIndex;
        resultSet->GetColumnIndex(CALL_DISPLAY_NAME, columnIndex);
        int32_t ret = resultSet->GetString(columnIndex, displayName);
        if (ret == 0 && (!displayName.empty())) {
            size_t cpyLen = displayName.length() + 1;
            if (displayName.length() > static_cast<size_t>(CONTACT_NAME_LEN)) {
                return false;
            }
            if (strcpy_s(contactInfo.name, cpyLen, displayName.c_str()) != EOK) {
                TELEPHONY_LOGE("strcpy_s fail.");
                return false;
            }
        }
        resultSetNum = resultSet->GoToNextRow();
    }
    resultSet->Close();
    helper->Release();
    TELEPHONY_LOGI("Query end");
    return true;
}

bool CallDataBaseHelper::Delete(DataShare::DataSharePredicates &predicates)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CALLLOG_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr!");
        return false;
    }
    Uri uri(CALL_SUBSECTION);
    return helper->Delete(uri, predicates);
}
} // namespace Telephony
} // namespace OHOS