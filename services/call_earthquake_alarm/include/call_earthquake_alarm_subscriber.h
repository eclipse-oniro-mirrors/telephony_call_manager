/*
 * Copyright (C) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef CALL_EARTHQUAKE_ALARM_SUBSCRIBER_H
#define CALL_EARTHQUAKE_ALARM_SUBSCRIBER_H

#include <mutex>
#include <unordered_map>
#include <memory>
#include "common_event_manager.h"
#include "common_event_subscribe_info.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "matching_skills.h"
#include "system_ability_definition.h"
#include "uri.h"
#include "iservice_registry.h"
#include "telephony_errors.h"
#include "want.h"
#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "rdb_errno.h"
#include "singleton.h"
#include "system_ability_status_change_stub.h"
#include "call_earthquake_alarm_locator.h"
#include "call_manager_inner_type.h"
#include "os_account_manager_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace Telephony {
class LocationSubscriber : public EventFwk::CommonEventSubscriber {
public:
    LocationSubscriber() = default;
    explicit LocationSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo);
    virtual ~LocationSubscriber() = default;
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &eventData) override;
    static bool Subscriber(void);
public:
    static const std::string SWITCH_STATE_CHANGE_EVENTS;
    static const std::string SWITCH_STATE_START;
    static const std::string SWITCH_STATE_STOP;
    static const std::string SWITCH_STATE;
    static std::shared_ptr<LocationSubscriber> subscriber_;
    static const std::string SWITCH_STATE_KEY;
};

class DataShareSwitchState {
public:
    DataShareSwitchState();
    int32_t QueryData(Uri& uri, const std::string& key, std::string& values);
public:
    static const int TELEPHONY_SUCCESS;
    static const int COW_COUNT_NULL;
    static const int TELEPHONY_ERR_LOCAL_PTR_NULL;
    static const int TELEPHONY_ERR_DATABASE_READ_FAIL;
    static const int TELEPHONY_ERR_DATABASE_WRITE_FAIL;
    static const int INVALID_VALUE;
private:
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int systemAbilityId);
    std::shared_ptr<DataShare::DataShareHelper> datashareHelper_ = nullptr;
};

class LocationSystemAbilityListener : public SystemAbilityStatusChangeStub {
public:
    LocationSystemAbilityListener() = default;
    ~LocationSystemAbilityListener() = default;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    static bool SystemAbilitySubscriber();
    static bool GetSystemAbility(int32_t systemAbilityId);
private:
    static sptr<ISystemAbilityStatusChange> statusChangeListener_;
};
} // namespace Telephony
} // namespace OHOS
#endif //  CALL_MANAGER_SERVICE_SUBSCRIBER_H