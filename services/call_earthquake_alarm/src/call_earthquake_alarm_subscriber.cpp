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

#include "call_earthquake_alarm_subscriber.h"

using namespace OHOS::AAFwk;
namespace OHOS {
namespace Telephony {
const std::string LocationSubscriber::SWITCH_STATE_CHANGE_EVENTS = "usual.event.EMERGENCY_WARNINGS_SWITCH_STATE_CHANGE";
const std::string LocationSubscriber::SWITCH_STATE_START = "true";
const std::string LocationSubscriber::SWITCH_STATE_STOP = "false";
const std::string LocationSubscriber::SWITCH_STATE = "switchState";
const std::string LocationSubscriber::SWITCH_STATE_KEY = "auto_send_earthquake_alarm_switch";
std::shared_ptr<LocationSubscriber> LocationSubscriber::subscriber_ = nullptr;
sptr<ISystemAbilityStatusChange> LocationSystemAbilityListener::statusChangeListener_ = nullptr;
const int DataShareSwitchState::INVALID_VALUE = -1;
const int DataShareSwitchState::TELEPHONY_SUCCESS = 0;
const int DataShareSwitchState::TELEPHONY_ERR_LOCAL_PTR_NULL = 1;
const int DataShareSwitchState::TELEPHONY_ERR_DATABASE_READ_FAIL = 2;
const int DataShareSwitchState::TELEPHONY_ERR_DATABASE_WRITE_FAIL = 3;
const int DataShareSwitchState::COW_COUNT_NULL = 4;
constexpr const char *SETTINGS_DATASHARE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
constexpr const char *SETTINGS_DATASHARE_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
constexpr const char *SETTINGS_DATA_COLUMN_KEYWORD = "KEYWORD";
constexpr const char *SETTINGS_DATA_COLUMN_VALUE = "VALUE";

LocationSubscriber::LocationSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : EventFwk::CommonEventSubscriber(subscriberInfo) {}

bool LocationSubscriber::Subscriber(void)
{
    if (LocationSubscriber::subscriber_ == nullptr) {
        EventFwk::MatchingSkills matchingSkills;
        matchingSkills.AddEvent(LocationSubscriber::SWITCH_STATE_CHANGE_EVENTS);
        EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
        LocationSubscriber::subscriber_ = std::make_shared<LocationSubscriber>(subscribeInfo);
        EventFwk::CommonEventManager::SubscribeCommonEvent(LocationSubscriber::subscriber_);
    }
    TELEPHONY_LOGI("create SubscribeCommonEvent LocationSubscriber");
    return true;
}

void LocationSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &eventData)
{
    const AAFwk::Want &want = eventData.GetWant();
    std::string action = want.GetAction();
    std::string switchState = want.GetStringParam(LocationSubscriber::SWITCH_STATE);

    if (action == LocationSubscriber::SWITCH_STATE_CHANGE_EVENTS) {
        auto engine = MyLocationEngine::GetInstance();
        if (engine == nullptr) {
            TELEPHONY_LOGI("engine == nullptr LocationSubscriber");
            return;
        }
        if (switchState == LocationSubscriber::SWITCH_STATE_START) {
            TELEPHONY_LOGI("start location LocationSubscriber");
            engine->SetValue();
            engine->RegisterLocationChange();
            engine->RegisterSwitchCallback();
        };
        if (switchState == LocationSubscriber::SWITCH_STATE_STOP) {
            TELEPHONY_LOGI("stop location LocationSubscriber");
            engine->UnregisterLocationChange();
            engine->UnRegisterSwitchCallback();
            engine->OnInit();
        };
    };
}

DataShareSwitchState::DataShareSwitchState()
{
    datashareHelper_ = CreateDataShareHelper(OHOS::DEVICE_STANDBY_SERVICE_SYSTEM_ABILITY_ID);
}

std::shared_ptr<DataShare::DataShareHelper> DataShareSwitchState::CreateDataShareHelper(int systemAbilityId)
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("DataShareSwitchState failed.");
        return nullptr;
    }
    sptr<IRemoteObject> remote = saManager->GetSystemAbility(systemAbilityId);
    if (remote == nullptr) {
        TELEPHONY_LOGE("DataShareSwitchState Service Failed.");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remote, SETTINGS_DATASHARE_URI, SETTINGS_DATASHARE_EXT_URI);
}

int32_t DataShareSwitchState::QueryData(Uri& uri, const std::string& key, std::string& value)
{
    if (datashareHelper_ == nullptr) {
        TELEPHONY_LOGE("DataShareSwitchState query error, datashareHelper_ is nullptr");
        return DataShareSwitchState::TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    auto result = datashareHelper_->Query(uri, predicates, columns);
    if (result == nullptr) {
        TELEPHONY_LOGE("DataShareSwitchState query error, result is nullptr");
        return DataShareSwitchState::TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int rowCount = 0;
    result->GetRowCount(rowCount);
    if (rowCount == 0) {
        TELEPHONY_LOGI("DataShareSwitchState query success, but rowCount is 0");
        return DataShareSwitchState::COW_COUNT_NULL;
    }

    if (result->GoToFirstRow() != DataShare::E_OK) {
        TELEPHONY_LOGE("DataShareSwitchState query error, go to first row error");
        result->Close();
        return DataShareSwitchState::TELEPHONY_ERR_DATABASE_READ_FAIL;
    }
    int columnIndex = 0;
    result->GetColumnIndex(SETTINGS_DATA_COLUMN_VALUE, columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    datashareHelper_->Release();
    TELEPHONY_LOGI("DataShareSwitchState query success");
    return DataShareSwitchState::TELEPHONY_SUCCESS;
}

void LocationSystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    static int firstboot = 0;
    TELEPHONY_LOGI("location service are add LocationSubscriber systemID = %{public}d", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("added SA is invalid! LocationSubscriber");
        return;
    }
    if (systemAbilityId != OHOS::DEVICE_STANDBY_SERVICE_SYSTEM_ABILITY_ID &&
        systemAbilityId != OHOS::LOCATION_LOCATOR_SA_ID &&
        systemAbilityId != OHOS::LOCATION_NOPOWER_LOCATING_SA_ID) {
        TELEPHONY_LOGI("location service are add LocationSubscriber systemID = %{public}d", systemAbilityId);
        return;
    }
    if (!GetSystemAbility(OHOS::DEVICE_STANDBY_SERVICE_SYSTEM_ABILITY_ID))  return;
    if (!GetSystemAbility(OHOS::LOCATION_LOCATOR_SA_ID))  return;
    if (!GetSystemAbility(OHOS::LOCATION_NOPOWER_LOCATING_SA_ID))  return;
    if (firstboot == 0) {
        MyLocationEngine::BootComplete();
        firstboot++;
        TELEPHONY_LOGI("first boot LocationSubscriber");
    }
}

bool LocationSystemAbilityListener::GetSystemAbility(int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> sysAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sysAbilityMgr == nullptr) {
        TELEPHONY_LOGE("system ability manager is nullptr. LocationSubscriber");
        return false;
    }
    sptr<IRemoteObject> remote = sysAbilityMgr->GetSystemAbility(systemAbilityId);
    if (remote == nullptr) {
        TELEPHONY_LOGE("check systemAbilityId return is nullptr. LocationSubscriber %{public}d", systemAbilityId);
        return false;
    }
    TELEPHONY_LOGE("check systemAbilityId return is not nullptr. LocationSubscriber %{public}d", systemAbilityId);
    return true;
}

void LocationSystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("location service are remove LocationSubscriber systemID = %{public}d", systemAbilityId);
    GetSystemAbility(OHOS::DEVICE_STANDBY_SERVICE_SYSTEM_ABILITY_ID);
    GetSystemAbility(OHOS::LOCATION_LOCATOR_SA_ID);
    GetSystemAbility(OHOS::LOCATION_NOPOWER_LOCATING_SA_ID);
}

bool LocationSystemAbilityListener::SystemAbilitySubscriber()
{
    statusChangeListener_ = new (std::nothrow) LocationSystemAbilityListener();
    if (statusChangeListener_ == nullptr) {
        TELEPHONY_LOGI("failed to create statusChangeListener LocationSubscriber");
        return false;
    }
    auto managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("get system ability manager error LocationSubscriber");
        return false;
    }
    int32_t ret1 = managerPtr->SubscribeSystemAbility(OHOS::DEVICE_STANDBY_SERVICE_SYSTEM_ABILITY_ID,
        statusChangeListener_);
        if (ret1 != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("failed to subscribe 1914 service SA!");
        }
    TELEPHONY_LOGE("success to subscribe 1914 service SA! LocationSubscriber");
    int32_t ret2 = managerPtr->SubscribeSystemAbility(OHOS::LOCATION_LOCATOR_SA_ID, statusChangeListener_);
        if (ret2 != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("failed to subscribe 2802 service SA!");
        }
    TELEPHONY_LOGE("success to subscribe 2802 service SA! LocationSubscriber");
    int32_t ret3 = managerPtr->SubscribeSystemAbility(OHOS::LOCATION_NOPOWER_LOCATING_SA_ID, statusChangeListener_);
        if (ret3 != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("failed to subscribe 2805 service SA!");
        }
    TELEPHONY_LOGE("success to subscribe 2805 service SA! LocationSubscriber");
    return true;
}
} // namespace Telephony
} // namespace OHOS