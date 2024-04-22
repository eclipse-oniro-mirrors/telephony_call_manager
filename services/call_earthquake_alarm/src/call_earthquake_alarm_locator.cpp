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

using namespace std;
using namespace OHOS::Telephony;
using namespace OHOS::Location;
using namespace OHOS::EventFwk;
namespace OHOS {
namespace Telephony {
using namespace AppExecFwk;
OHOS::Uri MyLocationEngine::uri_q("datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true"
    + "&key=auto_send_earthquake_alarm_switch");
const int MyLocationEngine::DISTANCE_INTERVAL = 10000; /** 10 kilometers */
const int MyLocationEngine::TIMER_INTERVAL = 0;
const std::string MyLocationEngine::EMERGENCY_DEVICE_ID = "";
const std::string MyLocationEngine::EMERGENCY_BUNDLE_NAME = "com.huawei.hmos.emergencycommunication";
const std::string MyLocationEngine::EMERGENCEY_ABILITY_NAME =
    "com.huawei.hmos.emergencycommunication.ServiceExtAbility";
const std::string MyLocationEngine::PARAMETERS_VALUE = "call_manager_earthquake_alarm";
const char* MyLocationEngine::PARAMETERS_KEY = "callerName";
const std::string MyLocationEngine::ALARM_SWITCH_ON = "1";
const std::string MyLocationEngine::ALARM_SWITCH_OFF = "0";
std::string MyLocationEngine::INITIAL_FIRST_VALUE = "FIRST_NO_VALUE";
const int EmergencyCallConnectCallback::CONNECT_SUCCESS = 0;
std::shared_ptr<MyLocationEngine> MyLocationEngine::mylocator = std::make_shared<MyLocationEngine>();
std::shared_ptr<MyLocationEngine> MyLocationEngine::GetInstance()
{
    if (MyLocationEngine::mylocator != nullptr) {
        std::shared_ptr<MyLocationEngine> instance_ = MyLocationEngine::mylocator;
        return instance_;
    }
    return nullptr;
}

void MyLocationEngine::OnInit()
{
    if (this->requestConfig != nullptr)     this->requestConfig = nullptr;
    if (this->locatorCallback_ != nullptr)  this->locatorCallback_ = nullptr;
    if (this->locatorImpl != nullptr)       this->locatorImpl = nullptr;
    if (this->switchCallback_ != nullptr)   this->switchCallback_ = nullptr;
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
            TELEPHONY_LOGI("MyLocationEngine locatorCallback_ is null");
            return;
        }
        locationEnable_ = locatorImpl->IsLocationEnabled();
        TELEPHONY_LOGI("MyLocationEngine locatorImpl is not nullptr");
    }
    if (this->requestConfig == nullptr) {
    this->requestConfig = std::make_unique<RequestConfig>(Location::SCENE_NO_POWER);
    this->requestConfig->SetTimeInterval(TIMER_INTERVAL);
    this->requestConfig->SetDistanceInterval(DISTANCE_INTERVAL);
    TELEPHONY_LOGI("MyLocationEngine requestConfig initial");
    }
}

void MyLocationEngine::RegisterLocationChange()
{
    if (locatorCallback_ == nullptr) {
        locatorCallback_ =
            sptr<MyLocationEngine::MyLocationCallBack>(new (std::nothrowe) MyLocationEngine::MyLocationCallBack());
        if (locatorCallback_ == nullptr) {
            TELEPHONY_LOGI("MyLocationEngine locatorCallback_ is null");
            return;
        }
    }
    bool IsLocationEnable;
    auto codeable = locatorImpl->IsLocationEnabledV9(IsLocationEnable);
    if (IsLocationEnable == false) {
        return;
    }
    auto callback = sptr<Location::ILocationCallback>(locatorCallback_);
    int code = this->locatorImpl->StartLocatingV9(this->requestConfig, callback);
    TELEPHONY_LOGI("MyLocationEngine startListencode = %{public}d.", code);
}

void MyLocationEngine::UnregisterLocationChange()
{
    auto callback = sptr<Location::ILocationCallback>(locatorCallback_);
    auto code = this->locatorImpl->StopLocatingV9(callback);
    TELEPHONY_LOGI("MyLocationEngine stopListencdode = %{public}d.", code);
}

void MyLocationEngine::RegisterSwitchCallback()
{
    if (locatorImpl == nullptr) {
        TELEPHONY_LOGI("MyLocationEnginek locatorImpl is null.");
        return;
    }
    auto engine = MyLocationEngine::GetInstance();
    if (switchCallback_ == nullptr) {
        switchCallback_ = sptr<MyLocationEngine::MySwitchCallback>(
            new (std::nothrow) MyLocationEngine::MySwitchCallback(engine));
        if (switchCallback_ == nullptr) {
            TELEPHONY_LOGI("MyLocationEngine callback is null.");
            return;
        }
    }
    auto code = locatorImpl->RegisterSwitchCallbackV9(switchCallback_->AsObject());
    TELEPHONY_LOGI("MyLocationEngine code = %{public}d. success", code);
}

void MyLocationEngine::UnRegisterSwitchCallback()
{
    if (locatorImpl == nullptr) {
        TELEPHONY_LOGI("MyLocationEngine locatorImpl is null.");
        return;
    }
    if (switchCallback_ != nullptr) {
        TELEPHONY_LOGI("MyLocationEngine lUnregisterSwitchCallback");
        locatorImpl->UnRegisterSwitchCallbackV9(switchCallback_->AsObject());
    }
}

void MyLocationEngine::LocationSwitchChange()
{
    if (locatorImpl == nullptr) {
        TELEPHONY_LOGI("MyLocationEngine locatorImpl is null.");
        return;
    }
    bool locationEnabled = locationEnable_;
    locationEnabled = locatorImpl->IsLocationEnabled();
    if (!locationEnabled && locationEnabled_) {
        TELEPHONY_LOGI("MyLocationEngine Enable location.[%{public}d][%{public}d]", locationEnabled, locationEnabled_);
        RegisterLocationChange();
    } else if (locationEnabled && !locationEnabled_) {
        TELEPHONY_LOGI("MyLocationEngine Disable location.[%{public}d][%{public}d]",
            locationEnabled, locationEnabled_);
        UnregisterLocationChange();
    } else {
        TELEPHONY_LOGI("MyLocationEngine Location switch not change[%{public}d]", locationEnabled_);
    }
}

int32_t MyLocationEngine::MyLocationCallBack::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadOnterfaceToken() != GetDescriptor()) {
        TELEPHONY_LOGI("MyLocationEngine invalid token.");
        return -1;
    }
    switch (code) {
        case REVEIVE_LOCATION_INFO_EVENT: {
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
    TELEPHONY_LOGI("MyLocationEngine locationreport code = %{public}d.", code);
    return 0;
}

MyLocationEngine::MyLocationCallBack::MyLocationCallBack() {}

void MyLocationEngine::MyLocationCallBack::OnLocatingStatusChange(const int status)
{
    TELEPHONY_LOGI("MyLocationEngine ListenStatus = %{public}d.", status);
}

void MyLocationEngine::MyLocationCallBack::OnErrorReport(const int errorCode) {}

void MyLocationEngine::MyLocationCallBack::OnLocationReport(const std::unique_ptr<Location::Location>& location)
{
    TELEPHONY_LOGI("MyLocationEngine ListenLocation is success");
    MyLocationEngine::ConnectAbility();
}

void MyLocationEngine::BootComplete()
{
    if (!MyLocationEngine::IsSwitchOn()) {
        TELEPHONY_LOGI("the switchstate is close MyLocationEngine");
        return;
    }
    TELEPHONY_LOGI("the switchstate is open MyLocationEngine");
    auto engine = MyLocationEngine::GetInstance();
    if (engine == nullptr) {
        TELEPHONY_LOGI("engine == nullptr MyLocationEngine");
        return;
    }
    engine->SetValue();
    engine->RegisterLocationChange();
    engine->RegisterSwitchCallback();
}

MyLocationEngine::MySwitchCallback::MySwitchCallback(std::shared_ptr<MyLocationEngine> locationUpdate)
    : locationUpdate_(locationUpdate)
{}

void MyLocationEngine::MySwitchCallback::OnSwitchChange(const int state)
{
    locationUpdate_->LocationSwitchChange();
}

int MyLocationEngine::MySwitchCallback::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadOnterfaceToken != GetDescriptor()) {
        TELEPHONY_LOGI("MyLocationEngine SwitchCallback invalid token.");
        return -1;
    }
    switch (code) {
        case RECEIVE_SWITCH_STATE_EVENT: {
            int32_t status = data.ReadInt32();
            OnSwitchChange(status);
            break;
        }
        default: {
            TELEPHONY_LOGI("MyLocationEngine  receive error code:%{public}u", code);
            break;
        }
    }
    return 0;
}

bool MyLocationEngine::IsSwitchOn()
{
    auto datasharedHelper = std::make_shared<DataShareSwitchState>();
    std::string switch_state = INITIAL_FIRST_VALUE;
    int resp = datashareHelper->QueryData(MyLocationEngine::uri_q, LocationSubscriber::SWITCH_STATE_KEY, switch_state);
    TELEPHONY_LOGI("MyLocationEngine switch_state = %{public}s ", switch_state.c_str());
    if (resp == DataShareSwitchState::TELEPHONY_SUCCESS && switch_state == ALARM_SWITCH_ON) {
        return true;
    }
    if (resp == DataShareSwitchState::TELEPHONY_SUCCESS && switch_state == ALARM_SWITCH_OFF) {
        return false;
    }
    return false;
}

sptr<AAFwk::IAbilityConnection> EmergencyCallConnectCallback::contentCallback_ = nullptr;
void MyLocationEngine::ConnectAbility()
{
    AAFwk::Want want;
    AppExecFwk::ElementName element(EMERGENCY_DEVICE_ID, EMERGENCY_BUNDLE_NAME, EMERGENCEY_ABILITY_NAME);
    want.SetElement(element);
    want.SetParam(PARAMETERS_KEY, PARAMETERS_VALUE);
    if (EmergencyCallConnectCallback::connectCallback_ == nullptr) {
        EmergencyCallConnectCallback::connectCallback_ = new EmergencyCallConnectCallback();
    }
    int32_t userId = -1;
    AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, EmergencyCallConnectCallback::connectCallback_,
        userId);
    TELEPHONY_LOGI("connect emergencycommunication ability MyLocationEngine");
}

void EmergencyCallConnectCallback::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    TELEPHONY_LOGI("connect callui result code: %{public}d", resultCode);
    if (resultCode == CONNECT_SUCCESS) {
        TELEPHONY_LOGI("connect emergencycommunication result success");
    }
}

void EmergencyCallConnectCallback::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    TELEPHONY_LOGI("disconnect emergencycommunication result code: %{public}d", resultCode);
}
} // namespace Telephony
}; // namespace OHOS