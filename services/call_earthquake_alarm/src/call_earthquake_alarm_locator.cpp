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

#include "call_earthquake_alarm_locator.h"
#include "ffrt.h"

using namespace std;
using namespace OHOS::Telephony;
using namespace OHOS::Location;
using namespace OHOS::EventFwk;
namespace OHOS {
namespace Telephony {
using namespace AppExecFwk;
const int MyLocationEngine::DISTANCE_INTERVAL = 10000; /** 10 kilometers */
const int MyLocationEngine::TIMER_INTERVAL = 0;
const std::string MyLocationEngine::EMERGENCY_DEVICE_ID = "";
const std::string MyLocationEngine::EMERGENCY_BUNDLE_NAME = "";
const std::string MyLocationEngine::EMERGENCY_ABILITY_NAME = "";
const std::string MyLocationEngine::PARAMETERS_VALUE = "call_manager_earthquake_alarm";
const char* MyLocationEngine::PARAMETERS_KEY = "callerName";
const std::string MyLocationEngine::ALARM_SWITCH_ON = "1";
const std::string MyLocationEngine::ALARM_SWITCH_OFF = "0";
std::string MyLocationEngine::INITIAL_FIRST_VALUE = "invalid";
std::shared_ptr<MyLocationEngine> MyLocationEngine::mylocator = std::make_shared<MyLocationEngine>();
std::shared_ptr<MyLocationEngine> MyLocationEngine::GetInstance()
{
    if (mylocator != nullptr) {
        return mylocator;
    }
    return std::make_shared<MyLocationEngine>();
}

void MyLocationEngine::OnInit()
{
    this->requestConfig = nullptr;
    this->locatorCallback_ = nullptr;
    this->locatorImpl = nullptr;
    this->switchCallback_ = nullptr;
}

MyLocationEngine::MyLocationEngine() {}

MyLocationEngine::~MyLocationEngine()
{
    UnRegisterSwitchCallback();
    UnregisterLocationChange();
    this->OnInit();
}

void MyLocationEngine::SetValue()
{
    if (this->locatorImpl == nullptr) {
        this->locatorImpl = Location::Locator::GetInstance();
        if (this->locatorImpl == nullptr) {
            TELEPHONY_LOGE("locatorCallback_ is null");
            return;
        }
        locationEnabled_ = locatorImpl->IsLocationEnabled();
        TELEPHONY_LOGI("locatorImpl is not nullptr");
    }
    if (this->requestConfig == nullptr) {
        this->requestConfig = std::make_unique<RequestConfig>(Location::SCENE_NO_POWER);
        this->requestConfig->SetTimeInterval(TIMER_INTERVAL);
        this->requestConfig->SetDistanceInterval(DISTANCE_INTERVAL);
        TELEPHONY_LOGI("requestConfig initial");
    }
}

void MyLocationEngine::RegisterLocationChange()
{
    if (locatorImpl == nullptr) {
        TELEPHONY_LOGE("locatorImpl is null");
        return;
    }
    if (locatorCallback_ == nullptr) {
        locatorCallback_ =
            sptr<MyLocationEngine::MyLocationCallBack>(new (std::nothrow) MyLocationEngine::MyLocationCallBack());
        if (locatorCallback_ == nullptr) {
            TELEPHONY_LOGE("locatorCallback_ is null");
            return;
        }
    }
    bool IsLocationEnable;
    locatorImpl->IsLocationEnabledV9(IsLocationEnable);
    if (IsLocationEnable == false) {
        return;
    }
    auto callback = sptr<Location::ILocatorCallback>(locatorCallback_);
    int code = this->locatorImpl->StartLocatingV9(this->requestConfig, callback);
    TELEPHONY_LOGI("start listen location change code = %{public}d.", code);
}

void MyLocationEngine::UnregisterLocationChange()
{
    if (locatorImpl == nullptr || locatorCallback_ == nullptr) {
        return;
    }
    auto callback = sptr<Location::ILocatorCallback>(locatorCallback_);
    auto code = this->locatorImpl->StopLocatingV9(callback);
    TELEPHONY_LOGI("stopListencdode = %{public}d.", code);
}

void MyLocationEngine::RegisterSwitchCallback()
{
    if (locatorImpl == nullptr) {
        TELEPHONY_LOGE("locatorImpl is null.");
        return;
    }
    auto engine = MyLocationEngine::GetInstance();
    if (switchCallback_ == nullptr) {
        switchCallback_ = sptr<MyLocationEngine::MySwitchCallback>(
            new (std::nothrow) MyLocationEngine::MySwitchCallback(engine));
        if (switchCallback_ == nullptr) {
            TELEPHONY_LOGE("callback is null.");
            return;
        }
    }
    auto code = locatorImpl->RegisterSwitchCallbackV9(switchCallback_->AsObject());
    TELEPHONY_LOGI("register switch callback code = %{public}d. success", code);
}

void MyLocationEngine::UnRegisterSwitchCallback()
{
    if (locatorImpl == nullptr) {
        TELEPHONY_LOGE("locatorImpl is null.");
        return;
    }
    if (switchCallback_ != nullptr) {
        TELEPHONY_LOGE("unregister switch callback");
        locatorImpl->UnregisterSwitchCallbackV9(switchCallback_->AsObject());
    }
}

void MyLocationEngine::LocationSwitchChange()
{
    if (locatorImpl == nullptr) {
        TELEPHONY_LOGE("locatorImpl is null.");
        return;
    }
    bool locationEnabled = locationEnabled_;
    locationEnabled_ = locatorImpl->IsLocationEnabled();
    if (!locationEnabled && locationEnabled_) {
        TELEPHONY_LOGI("Enable location.[%{public}d][%{public}d]", locationEnabled, locationEnabled_);
        RegisterLocationChange();
    } else if (locationEnabled && !locationEnabled_) {
        TELEPHONY_LOGI("Disable location.[%{public}d][%{public}d]", locationEnabled, locationEnabled_);
        UnregisterLocationChange();
    } else {
        TELEPHONY_LOGI("Location switch not change[%{public}d]", locationEnabled_);
    }
}

int32_t MyLocationEngine::MyLocationCallBack::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TELEPHONY_LOGE("invalid token.");
        return -1;
    }
    switch (code) {
        case RECEIVE_LOCATION_INFO_EVENT: {
            std::unique_ptr<OHOS::Location::Location> location = OHOS::Location::Location::Unmarshalling(data);
            OnLocationReport(location);
            break;
        }
        case RECEIVE_ERROR_INFO_EVENT: {
            int32_t errorCode = data.ReadInt32();
            OnErrorReport(errorCode);
            break;
        }
        case RECEIVE_LOCATION_STATUS_EVENT: {
            int32_t status = data.ReadInt32();
            OnLocatingStatusChange(status);
            break;
        }
        default: {
            break;
        }
    }
    TELEPHONY_LOGI("locationreport code = %{public}d.", code);
    return 0;
}

MyLocationEngine::MyLocationCallBack::MyLocationCallBack() {}

void MyLocationEngine::MyLocationCallBack::OnLocatingStatusChange(const int status)
{
    TELEPHONY_LOGI("listen status = %{public}d", status);
}

void MyLocationEngine::MyLocationCallBack::OnErrorReport(const int errorCode) {}

void MyLocationEngine::MyLocationCallBack::OnLocationReport(const std::unique_ptr<Location::Location>& location)
{
    TELEPHONY_LOGI("location report");
    MyLocationEngine::ConnectAbility(MyLocationEngine::PARAMETERS_VALUE);
}

void MyLocationEngine::BootComplete(bool switchState)
{
    if (!switchState) {
        TELEPHONY_LOGE("the alarm switch is close");
        return;
    }
    TELEPHONY_LOGI("the alarm switch is open");
    MyLocationEngine::GetInstance()->SetValue();
    MyLocationEngine::GetInstance()->RegisterLocationChange();
    MyLocationEngine::GetInstance()->RegisterSwitchCallback();
}

MyLocationEngine::MySwitchCallback::MySwitchCallback(std::shared_ptr<MyLocationEngine> locationUpdate)
    : locationUpdate_(locationUpdate)
{}

void MyLocationEngine::MySwitchCallback::OnSwitchChange(const int state)
{
    if (locationUpdate_ == nullptr) {
        return;
    }
    locationUpdate_->LocationSwitchChange();
}

int MyLocationEngine::MySwitchCallback::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TELEPHONY_LOGE("switchCallback invalid token.");
        return -1;
    }
    switch (code) {
        case RECEIVE_SWITCH_STATE_EVENT: {
            int32_t status = data.ReadInt32();
            OnSwitchChange(status);
            break;
        }
        default: {
            TELEPHONY_LOGI("receive error code:%{public}u", code);
            break;
        }
    }
    return 0;
}

bool MyLocationEngine::IsSwitchOn(std::string key, std::string& value)
{
    auto datashareHelper = std::make_shared<DataShareSwitchState>();
    OHOS::Uri uri(datashareHelper->DEFAULT_URI + key);
    int resp = datashareHelper->QueryData(uri, key, value);
    TELEPHONY_LOGI("query %{public}s is %{public}s", key.c_str(), value.c_str());
    if (resp != DataShareSwitchState::TELEPHONY_SUCCESS || value == ALARM_SWITCH_OFF) {
        return false;
    }
    return true;
}

std::map<std::string, sptr<AAFwk::IDataAbilityObserver>> MyLocationEngine::settingsCallbacks = {};
void MyLocationEngine::OOBEComplete()
{
    std::string stateValue = INITIAL_FIRST_VALUE;
    for (auto& oobeKey : OOBESwitchObserver::keyStatus) {
        oobeKey.second = MyLocationEngine::IsSwitchOn(oobeKey.first, stateValue);
        if (!oobeKey.second) {
            settingsCallbacks[oobeKey.first] = sptr<OOBESwitchObserver>::MakeSptr(oobeKey.first);
            auto datashareHelper = std::make_shared<DataShareSwitchState>();
            datashareHelper->RegisterListenSettingsKey(oobeKey.first, true, settingsCallbacks[oobeKey.first]);
        }
    }
};

std::map<std::string, bool> OOBESwitchObserver::keyStatus = {
    {"device_provisioned", false},
    {"user_setup_complete", false},
    {"is_ota_finished", false}
};

std::mutex OOBESwitchObserver::mutex_;
void OOBESwitchObserver::OnChange()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!keyStatus[mKey]) {
        keyStatus[mKey] = MyLocationEngine::IsSwitchOn(mKey, mValue);
    }
    for (auto& oobeKey : keyStatus) {
        if (!oobeKey.second) {
            TELEPHONY_LOGE("%{public}s is trun off", oobeKey.first.c_str());
            return;
        }
    }
    mValue = MyLocationEngine::INITIAL_FIRST_VALUE;
    if (MyLocationEngine::IsSwitchOn(LocationSubscriber::SWITCH_STATE_KEY, mValue)) {
        TELEPHONY_LOGI("the alarm switch is open");
        MyLocationEngine::ConnectAbility("call_manager_oobe_earthquake_warning_switch_on");
    }
    ffrt::submit([&]() {
        std::string stateValue = INITIAL_FIRST_VALUE;
        for (auto& oobeKey : MyLocationEngine::settingsCallbacks) {
            auto datashareHelper = std::make_shared<DataShareSwitchState>();
            datashareHelper->RegisterListenSettingsKey(oobeKey.first, false, oobeKey.second);
            oobeKey.second = nullptr;
        }
        TELEPHONY_LOGI("unregister listen finish");
    });
}

sptr<AAFwk::IAbilityConnection> EmergencyCallConnectCallback::connectCallback_ = nullptr;
void MyLocationEngine::ConnectAbility(std::string value)
{
    AAFwk::Want want;
    AppExecFwk::ElementName element(EMERGENCY_DEVICE_ID, EMERGENCY_BUNDLE_NAME, EMERGENCY_ABILITY_NAME);
    want.SetElement(element);
    want.SetParam(PARAMETERS_KEY, value);
    if (EmergencyCallConnectCallback::connectCallback_ == nullptr) {
        EmergencyCallConnectCallback::connectCallback_ = sptr<EmergencyCallConnectCallback>::MakeSptr();
    }
    int32_t userId = -1;
    auto ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want,
        EmergencyCallConnectCallback::connectCallback_, userId);
    TELEPHONY_LOGI("Connect emergencycall ability %{public}d", ret);
}

void EmergencyCallConnectCallback::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    TELEPHONY_LOGI("connect result code: %{public}d", resultCode);
}

void EmergencyCallConnectCallback::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    TELEPHONY_LOGI("disconnect result code: %{public}d", resultCode);
}
} // namespace Telephony
} // namespace OHOS