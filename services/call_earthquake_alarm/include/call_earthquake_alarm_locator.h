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

#ifndef CALL_EARTHQUAKE_ALARM_LOCATOR_H
#define CALL_EARTHQUAKE_ALARM_LOCATOR_H

#include <iostream>
#include "request_config.h"
#include "location.h"
#include "locator.h"
#include "i_locator_callback.h"
#include "locator_impl.h"
#include "location_log.h"
#include "iremote_stub.h"
#include <memory>
#include "call_earthquake_alarm_subscriber.h"
#include "ability_connect_callback_interface.h"
#include "telephony_permission.h"
#include "os_account_manager.h"
#include "telephony_log_wrapper.h"
#include "call_connect_ability.h"
#include "ability_manager_client.h"
#include "call_ability_connect_callback.h"
#include "call_number_utils.h"
#include "int_wrapper.h"
#include "string_wrapper.h"
#include "call_ability_report_proxy.h"
#include "call_object_manager.h"

namespace OHOS {
namespace Telephony {
class MyLocationEngine {
public:
    MyLocationEngine();
    virtual ~MyLocationEngine();
    void OnInit();
    void SetValue();
    void RegisterLocationChange();
    void UnregisterLocationChange();
    void RegisterSwitchCallback();
    void UnRegisterSwitchCallback();
    void LocationSwitchChange();
    static void BootComplete();
    static bool IsSwitchOn();
    static std::shared_ptr<MyLocationEngine> GetInstance();
    static void ConnectAbility();
    static OHOS::Uri uri_q;
private:
    class MyLocationCallBack : public IRemoteStub<Location::ILocatorCallback> {
    public:
        MyLocationCallBack();
        virtual void OnLocationReport(const std::unique_ptr<Location::Location>& location) override;
        virtual void OnLocatingStatusChange(const int status) override;
        virtual void OnErrorReport(const int errorCode) override;
        int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
            MessageOption &option) override;
    };
    //class MyLoctionCallBack

    class MySwitchCallback : public IRemoteStub<Location::ISwitchCallback> {
    public:
        explicit MySwitchCallback(std::shared_ptr<MyLocationEngine> locationUpdate);
        virtual int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
           MessageOption &option) override;
        void OnSwitchChange(const int state) override;
    private:
        std::shared_ptr<MyLocationEngine> locationUpdate_ = nullptr;
    };
private:
    std::unique_ptr<Location::RequestConfig> requestConfig = nullptr;
    std::shared_ptr<Location::LocatorImpl> locatorImpl = nullptr;
    sptr<MyLocationCallBack> locatorCallback_ = nullptr;
    sptr<MySwitchCallback> switchCallback_ = nullptr;
    bool locationEnabled_ = false;
    static std::shared_ptr<MyLocationEngine> mylocator;
    static const int DISTANCE_INTERVAL;
    static const int TIMER_INTERVAL;
    static const std::string EMERGENCY_DEVICE_ID;
    static const std::string EMERGENCY_BUNDLE_NAME;
    static const std::string EMERGENCEY_ABILITY_NAME;
    static const std::string PARAMETERS_VALUE;
    static const char* PARAMETERS_KEY;
    static const std::string ALARM_SWITCH_ON;
    static const std::string ALARM_SWITCH_OFF;
    static std::string INITIAL_FIRST_VALUE;
};
//class MyLocationCallBack

class EmergencyCallConnectCallback : public IRemoteStub<AAFwk::IAbilityConnection> {
public:
    EmergencyCallConnectCallback() = default;
    ~EmergencyCallConnectCallback() = default;
    void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
        int resultCode);
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode);
    static sptr<AAFwk::IAbilityConnection> connectCallback_;
    static const int CONNECT_SUCCESS;
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_MANAGER_SERVICE_LOCATOR_H