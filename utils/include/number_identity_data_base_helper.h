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

#ifndef TELEPHONY_NUMBER_IDENTITY_DATA_BASE_HELPER_H
#define TELEPHONY_NUMBER_IDENTITY_DATA_BASE_HELPER_H

#include "ability_manager_interface.h"
#include "common_type.h"
#include "data_ability_observer_stub.h"
#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "if_system_ability_manager.h"
#include "refbase.h"
#include "singleton.h"
#include "system_ability.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
class NumberIdentityDataBaseHelper {
    DECLARE_DELAYED_SINGLETON(NumberIdentityDataBaseHelper)
public:
    bool Query(std::string &numberLocation, DataShare::DataSharePredicates &predicates);
    bool QueryYellowPageAndMark(NumberMarkInfo &numberMarkInfo, DataShare::DataSharePredicates &predicates);
    bool SetMarkInfoValues(std::shared_ptr<DataShare::DataShareResultSet> &resultSet, NumberMarkInfo &numberMarkInfo);
    static constexpr const char *NUMBER_IDENTITY_URI = "datashare:///numberlocationability";
private:
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(std::string uri);
};
} // namespace Telephony
} // namespace OHOS
#endif
