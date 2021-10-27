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
using namespace OHOS::AppExecFwk;
using namespace OHOS::AAFwk;
using namespace NativeRdb;
class AbsSharedResultSet;
CallDataBaseHelper::CallDataBaseHelper() {}

CallDataBaseHelper::~CallDataBaseHelper() {}

std::shared_ptr<AppExecFwk::DataAbilityHelper> CallDataBaseHelper::CreateDataAHelper()
{
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGD("Get system ability mgr failed.");
        return nullptr;
    }

    auto remoteObj = saManager->GetSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (remoteObj == nullptr) {
        TELEPHONY_LOGD("GetSystemAbility Service Failed.");
        return nullptr;
    }

    return DataAbilityHelper::Creator(remoteObj);
}

bool CallDataBaseHelper::Insert(NativeRdb::ValuesBucket &values)
{
    std::shared_ptr<DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        return false;
    }
    std::shared_ptr<Uri> uri = std::make_shared<Uri>(CALL_SUBSECTION);
    return helper->Insert(*uri, values);
}

bool CallDataBaseHelper::Query(NativeRdb::DataAbilityPredicates &predicates)
{
    return true;
}

bool CallDataBaseHelper::Delete(NativeRdb::DataAbilityPredicates &predicates)
{
    std::shared_ptr<DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        return false;
    }
    std::shared_ptr<Uri> uri = std::make_shared<Uri>(CALL_SUBSECTION);
    return helper->Delete(*uri, predicates);
}

void CallDataBaseHelper::ResultSetConvertToIndexer(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{
}
} // namespace Telephony
} // namespace OHOS