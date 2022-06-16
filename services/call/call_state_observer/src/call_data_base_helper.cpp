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

    NativeRdb::DataAbilityPredicates predicates;
    predicates.NotEqualTo("phone_number", std::string(""));
    this->phones->clear();
    callDataPtr->Query(this->phones, predicates);
}

CallDataBaseHelper::CallDataBaseHelper() {}

CallDataBaseHelper::~CallDataBaseHelper() {}

std::shared_ptr<AppExecFwk::DataAbilityHelper> CallDataBaseHelper::CreateDataAHelper()
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
    return AppExecFwk::DataAbilityHelper::Creator(remoteObj);
}

void CallDataBaseHelper::RegisterObserver(std::vector<std::string> *phones)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper_ is null");
        return;
    }
    Uri uri(CALL_BLOCK);
    callDataRdbObserverPtr_ = (std::make_unique<CallDataRdbObserver>(phones)).release();
    if (callDataRdbObserverPtr_ == nullptr) {
        TELEPHONY_LOGE("callDataRdbObserverPtr_ is null");
        return;
    }
    helper->RegisterObserver(uri, callDataRdbObserverPtr_);
}

void CallDataBaseHelper::UnRegisterObserver()
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateDataAHelper();
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

bool CallDataBaseHelper::Insert(NativeRdb::ValuesBucket &values)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr!");
        return false;
    }
    Uri uri(CALL_SUBSECTION);
    return helper->Insert(uri, values);
}

bool CallDataBaseHelper::Query(std::vector<std::string> *phones, NativeRdb::DataAbilityPredicates &predicates)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr");
        return false;
    }
    Uri uri(CALL_BLOCK);
    std::vector<std::string> columns;
    columns.push_back("phone_number");
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = helper->Query(uri, columns, predicates);
    helper->Release();
    if (resultSet == nullptr) {
        return false;
    }
    int32_t resultSetNum = resultSet->GoToFirstRow();
    while (resultSetNum == 0) {
        std::string phone;
        int32_t columnIndex;
        int32_t ret = resultSet->GetColumnIndex("phone_number", columnIndex);
        ret = resultSet->GetString(columnIndex, phone);
        if (ret == 0 && (!phone.empty())) {
            phones->push_back(phone);
        }
        resultSetNum = resultSet->GoToNextRow();
    }
    resultSet->Close();
    TELEPHONY_LOGI("Query end");
    return true;
}

bool CallDataBaseHelper::Query(ContactInfo &contactInfo, NativeRdb::DataAbilityPredicates &predicates)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr");
        return false;
    }
    Uri uri(CONTACT_DATA);
    std::vector<std::string> columns;
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = helper->Query(uri, columns, predicates);
    helper->Release();
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("resultSet is nullptr");
        return false;
    }
    int32_t resultSetNum = resultSet->GoToFirstRow();
    while (resultSetNum == 0) {
        std::string displayName;
        int32_t columnIndex;
        int32_t ret = resultSet->GetColumnIndex(CALL_DISPLAY_NAME, columnIndex);
        ret = resultSet->GetString(columnIndex, displayName);
        if (ret == 0 && (!displayName.empty())) {
            size_t cpyLen = strlen(displayName.c_str()) + 1;
            if (strcpy_s(contactInfo.name, cpyLen, displayName.c_str()) != EOK) {
                TELEPHONY_LOGE("strcpy_s fail.");
                return false;
            }
        }
        resultSetNum = resultSet->GoToNextRow();
    }
    resultSet->Close();
    TELEPHONY_LOGI("Query end");
    return true;
}

bool CallDataBaseHelper::Delete(NativeRdb::DataAbilityPredicates &predicates)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr!");
        return false;
    }
    Uri uri(CALL_SUBSECTION);
    return helper->Delete(uri, predicates);
}

void CallDataBaseHelper::ResultSetConvertToIndexer(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{}
} // namespace Telephony
} // namespace OHOS