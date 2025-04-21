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

#ifndef SETTINGS_DATASHARE_HELPER_H
#define SETTINGS_DATASHARE_HELPER_H

#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "system_ability_definition.h"
#include "singleton.h"
#include "uri.h"

namespace OHOS {
namespace Telephony {
class SettingsDataShareHelper : public DelayedSingleton<SettingsDataShareHelper> {
    DECLARE_DELAYED_SINGLETON(SettingsDataShareHelper);

public:
    static const std::string QUERY_SATELLITE_MODE_KEY;
    static const std::string QUERY_SATELLITE_CONNECTED_KEY;
    static const std::string SETTINGS_DATASHARE_URI;
    static const std::string QUERY_MOTION_CLOSE_TO_EAR_KEY;
    static const std::string QUERY_MOTION_FLIP_MUTE_KEY;
    static const std::string QUERY_MOTION_PICKUP_REDUCE_KEY;

    ErrCode Query(Uri& uri, const std::string& key, std::string& values);
    int32_t Insert(Uri &uri, const std::string &key, const std::string &value);
    int32_t Update(Uri &uri, const std::string &key, const std::string &value);
    bool RegisterToDataShare(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &observer);
private:
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int systemAbilityId);
};
} // namespace Telephony
} // namespace OHOS
#endif //SETTINGS_DATASHARE_HELPER_H