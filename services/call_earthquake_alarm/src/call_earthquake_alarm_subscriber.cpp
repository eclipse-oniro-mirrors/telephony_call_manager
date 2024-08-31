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
        subscribeInfo.SetPermission("ohos.permission.ATTEST_KEY");
        LocationSubscriber::subscriber_ = std::make_shared<LocationSubscriber>(subscribeInfo);
        EventFwk::CommonEventManager::SubscribeCommonEvent(LocationSubscriber::subscriber_);
    }
    TELEPHONY_LOGI("create subscribe commonEvent");
    return true;
}

void LocationSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &eventData)
{
    const AAFwk::Want &want = eventData.GetWant();
    std::string action = want.GetAction();
    std::string switchState = want.GetStringParam(LocationSubscriber::SWITCH_STATE);

    if (action == LocationSubscriber::SWITCH_STATE_CHANGE_EVENTS) {
        if (switchState == LocationSubscriber::SWITCH_STATE_START) {
            TELEPHONY_LOGI("start location listen");
            MyLocationEngine::GetInstance()->SetValue();
            MyLocationEngine::GetInstance()->RegisterLocationChange();
            MyLocationEngine::GetInstance()->RegisterSwitchCallback();
        };
        if (switchState == LocationSubscriber::SWITCH_STATE_STOP) {
            TELEPHONY_LOGI("stop location listen");
            MyLocationEngine::GetInstance()->UnregisterLocationChange();
            MyLocationEngine::GetInstance()->UnRegisterSwitchCallback();
            MyLocationEngine::GetInstance()->OnInit();
        };
    };
}

DataShareSwitchState::DataShareSwitchState()
{
    datashareHelper_ = CreateDataShareHelper(OHOS::TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
}

std::shared_ptr<DataShare::DataShareHelper> DataShareSwitchState::CreateDataShareHelper(int systemAbilityId)
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("get SA manager failed");
        return nullptr;
    }
    sptr<IRemoteObject> remote = saManager->GetSystemAbility(systemAbilityId);
    if (remote == nullptr) {
        TELEPHONY_LOGE("get SA failed");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remote, SETTINGS_DATASHARE_URI, SETTINGS_DATASHARE_EXT_URI);
}

int32_t DataShareSwitchState::QueryData(Uri& uri, const std::string& key, std::string& value)
{
    if (datashareHelper_ == nullptr) {
        TELEPHONY_LOGE("query error, datashareHelper_ is nullptr");
        return INVALID_VALUE;
    }
    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    auto result = datashareHelper_->Query(uri, predicates, columns);
    datashareHelper_->Release();
    if (result == nullptr) {
        TELEPHONY_LOGE("query error, result is nullptr");
        return INVALID_VALUE;
    }
    int rowCount = 0;
    result->GetRowCount(rowCount);
    if (rowCount == 0) {
        TELEPHONY_LOGI("query success, but rowCount is 0");
        result->Close();
        return INVALID_VALUE;
    }
    if (result->GoToFirstRow() != DataShare::E_OK) {
        TELEPHONY_LOGE("query error, go to first row error");
        result->Close();
        return INVALID_VALUE;
    }
    int columnIndex = 0;
    result->GetColumnIndex(SETTINGS_DATA_COLUMN_VALUE, columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    TELEPHONY_LOGI("query success");
    return TELEPHONY_SUCCESS;
}

bool DataShareSwitchState::RegisterListenSettingsKey(std::string key, bool isReg,
    sptr<AAFwk::IDataAbilityObserver> callback)
{
    if (datashareHelper_ == nullptr) {
        TELEPHONY_LOGE("datashareHelper is nullptr");
        return false;
    }
    if (callback == nullptr) {
        TELEPHONY_LOGE("callback is nullptr");
        return false;
    }
    std::string uri = DEFAULT_URI + key;
    if (isReg) {
        TELEPHONY_LOGI("register listen %{public}s finish", key.c_str());
        datashareHelper_->RegisterObserver(OHOS::Uri(uri), callback);
    } else {
        TELEPHONY_LOGI("unregister listen %{public}s finish", key.c_str());
        datashareHelper_->UnregisterObserver(OHOS::Uri(uri), callback);
    }
    datashareHelper_->Release();
    return true;
}

std::map<int32_t, bool> LocationSystemAbilityListener::systemAbilityStatus = {
    {OHOS::DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID, false},
    {OHOS::LOCATION_LOCATOR_SA_ID, false},
    {OHOS::LOCATION_NOPOWER_LOCATING_SA_ID, false}
};
void LocationSystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (startService) {
        TELEPHONY_LOGE("service alredy started");
        return;
    }
    if (systemAbilityId != OHOS::DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID &&
        systemAbilityId != OHOS::LOCATION_LOCATOR_SA_ID &&
        systemAbilityId != OHOS::LOCATION_NOPOWER_LOCATING_SA_ID) {
        TELEPHONY_LOGE("add other SA failed, id = %{public}d", systemAbilityId);
        return;
    };
    TELEPHONY_LOGI("added SA success, id = %{public}d", systemAbilityId);
    systemAbilityStatus[systemAbilityId] = GetSystemAbility(systemAbilityId);
    for (auto& systemId : systemAbilityStatus) {
        if (!systemId.second) {
            TELEPHONY_LOGE("SA id %{public}d, status is false", systemId.first);
            return;
        }
    }
    startService = true;
    std::string stateValue = MyLocationEngine::INITIAL_FIRST_VALUE;
    auto alarmSwitchState = MyLocationEngine::IsSwitchOn(LocationSubscriber::SWITCH_STATE_KEY, stateValue);
    if (stateValue == MyLocationEngine::INITIAL_FIRST_VALUE) {
        TELEPHONY_LOGI("start OOBE complete");
        MyLocationEngine::OOBEComplete();
        return;
    }
    TELEPHONY_LOGI("start boot complete");
    MyLocationEngine::BootComplete(alarmSwitchState);
    OnRemoveSystemAbility(systemAbilityId, deviceId);
}

bool LocationSystemAbilityListener::GetSystemAbility(int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> sysAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sysAbilityMgr == nullptr) {
        TELEPHONY_LOGE("SA manager is nullptr");
        return false;
    }
    sptr<IRemoteObject> remote = sysAbilityMgr->GetSystemAbility(systemAbilityId);
    if (remote == nullptr) {
        TELEPHONY_LOGE("get SA %{public}d failed", systemAbilityId);
        return false;
    }
    TELEPHONY_LOGI("get SA %{public}d success", systemAbilityId);
    return true;
}

void LocationSystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("remove SA %{public}d", systemAbilityId);
    if (!startService) {
        TELEPHONY_LOGE("service is not started");
        return;
    }
    int32_t len = (int32_t)MyLocationEngine::settingsCallbacks.size();
    TELEPHONY_LOGI("callbacks is %{public}d", len);
    if (len != 0) {
        for (auto& oobeKey : OOBESwitchObserver::keyStatus) {
            if (!oobeKey.second) {
                TELEPHONY_LOGE("key %{public}s, status is false", oobeKey.first.c_str());
                return;
            }
        }
        for (auto& oobeKey : MyLocationEngine::settingsCallbacks) {
            auto datashareHelper = std::make_shared<DataShareSwitchState>();
            datashareHelper->RegisterListenSettingsKey(oobeKey.first, false, oobeKey.second);
        }
    }
    MyLocationEngine::settingsCallbacks = {};
    systemAbilityStatus = {};
    OOBESwitchObserver::keyStatus = {};
    SystemAbilitySubscriber();
    statusChangeListener_ = nullptr;
}

bool LocationSystemAbilityListener::SystemAbilitySubscriber()
{
    if (statusChangeListener_ == nullptr) {
        statusChangeListener_ = sptr<LocationSystemAbilityListener>::MakeSptr();
    }
    auto managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("get SA manager error");
        return false;
    }
    int32_t ret = -1;
    for (auto& systemId : systemAbilityStatus) {
        if (!systemId.second) {
            ret = managerPtr->SubscribeSystemAbility(systemId.first, statusChangeListener_);
        } else {
            ret = managerPtr->UnSubscribeSystemAbility(systemId.first, statusChangeListener_);
        }
        std::string isSub = systemId.second ? "unsubscribe" : "subscribe";
        TELEPHONY_LOGI("%{public}s SA %{public}d ret code is %{public}d", isSub.c_str(), systemId.first, ret);
    }
    return true;
}

} // namespace Telephony
} // namespace OHOS