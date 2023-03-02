/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef CALL_SETTING_MANAGER_H
#define CALL_SETTING_MANAGER_H

#include <cstring>
#include <memory>

#include "pac_map.h"

#include "cellular_call_connection.h"

namespace OHOS {
namespace Telephony {
class CallSettingManager {
public:
    CallSettingManager();
    ~CallSettingManager();

    int32_t GetCallWaiting(int32_t slotId);
    int32_t SetCallWaiting(int32_t slotId, bool activate);
    int32_t GetCallRestriction(int32_t slotId, CallRestrictionType type);
    int32_t SetCallRestriction(int32_t slotId, CallRestrictionInfo &info);
    int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type);
    int32_t SetCallTransferInfo(int32_t slotId, CallTransferInfo &info);
    int32_t CanSetCallTransferTime(int32_t slotId, bool &result);
    int32_t SetCallPreferenceMode(int32_t slotId, int32_t mode);
    int32_t GetImsConfig(int32_t slotId, ImsConfigItem item);
    int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value);
    int32_t GetImsFeatureValue(int32_t slotId, FeatureType type);
    int32_t SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value);
    int32_t EnableImsSwitch(int32_t slotId);
    int32_t DisableImsSwitch(int32_t slotId);
    int32_t IsImsSwitchEnabled(int32_t slotId, bool &enabled);
    int32_t CloseUnFinishedUssd(int32_t slotId);

private:
    int32_t CallWaitingPolicy(int32_t slotId);
    int32_t GetCallRestrictionPolicy(int32_t slotId, CallRestrictionType type);
    int32_t SetCallRestrictionPolicy(int32_t slotId, CallRestrictionInfo &info);
    int32_t GetCallTransferInfoPolicy(int32_t slotId, CallTransferType type);
    int32_t SetCallTransferInfoPolicy(int32_t slotId, CallTransferInfo &info);
    int32_t CloseUnFinishedUssdPolicy(int32_t slotId);

private:
    std::shared_ptr<CellularCallConnection> cellularCallConnectionPtr_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_SETTING_MANAGER_H
