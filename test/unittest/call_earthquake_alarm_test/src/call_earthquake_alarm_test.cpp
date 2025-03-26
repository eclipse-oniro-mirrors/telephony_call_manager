/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define private public
#define protected public
#include "ability_connect_callback_interface.h"
#include "ability_manager_client.h"
#include "call_ability_connect_callback.h"
#include "call_ability_report_proxy.h"
#include "call_connect_ability.h"
#include "call_earthquake_alarm_locator.h"
#include "call_earthquake_alarm_subscriber.h"
#include "call_manager_inner_type.h"
#include "call_number_utils.h"
#include "call_object_manager.h"
#include "common_event_manager.h"
#include "common_event_subscribe_info.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "gtest/gtest.h"
#include "i_locator_callback.h"
#include "int_wrapper.h"
#include "iremote_stub.h"
#include "iservice_registry.h"
#include "location_log.h"
#include "location.h"
#include "locator_impl.h"
#include "locator.h"
#include "matching_skills.h"
#include "os_account_manager_wrapper.h"
#include "os_account_manager.h"
#include "rdb_errno.h"
#include "request_config.h"
#include "securec.h"
#include "singleton.h"
#include "string_wrapper.h"
#include "system_ability_definition.h"
#include "system_ability_status_change_stub.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "uri.h"
#include "want.h"
#include "call_manager_base.h"
#include "call_base.h"
#include <iostream>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
using namespace std;

class LocationEngineTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    virtual void SetUp() {};
    virtual void TearDown() {};
};

/**
 * @tc.number   Telephony_MyLocationEngine_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(LocationEngineTest, Telephony_MyLocationEngine_001, Function | MediumTest | Level3)
{
    auto engine = std::make_shared<MyLocationEngine>();
    if (engine == nullptr) {
        TELEPHONY_LOGI("engine is null. MyLocationEngine");
        return;
    }
    engine->mylocator = engine;
    engine = engine->GetInstance();
    engine->SetValue();
    std::string key1 = "testkey";
    std::string value1 = "testvalue";
    engine->IsSwitchOn(key1, value1);
    engine->BootComplete(true);
    engine->BootComplete(false);
    engine->OOBEComplete();
    engine->RegisterLocationChange();
    engine->RegisterSwitchCallback();
    engine->LocationSwitchChange();
    engine->UnregisterLocationChange();
    engine->UnRegisterSwitchCallback();
    engine->OnInit();
    engine->SetValue();
    engine->RegisterLocationChange();
    engine->RegisterSwitchCallback();
    ASSERT_TRUE(engine->GetInstance() != nullptr);
}

/**
 * @tc.number   Telephony_MyLocationEngine_002
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(LocationEngineTest, Telephony_MyLocationEngine_002, Function | MediumTest | Level3)
{
    auto engine1 = std::make_shared<MyLocationEngine>();
    engine1->mylocator = engine1;
    auto engine = engine1->GetInstance();
    engine->SetValue();
    engine->RegisterLocationChange();
    engine->RegisterSwitchCallback();
    if (engine->locatorCallback_ == nullptr) {
        return;
    }
    Parcel parcel;
    std::unique_ptr<Location::Location> location = Location::Location::UnmarshallingMakeUnique(parcel);
    engine->locatorCallback_->OnLocationReport(location);
    uint32_t code = 0;
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    engine->locatorCallback_->OnRemoteRequest(code, data1, reply, option);
    MessageParcel data2;
    data2.WriteInterfaceToken(MyLocationEngine::MyLocationCallBack::GetDescriptor());
    engine->locatorCallback_->OnRemoteRequest(code, data2, reply, option);
    MessageParcel data3;
    data3.WriteInterfaceToken(MyLocationEngine::MyLocationCallBack::GetDescriptor());
    code = Location::ILocatorCallback::RECEIVE_LOCATION_INFO_EVENT;
    engine->locatorCallback_->OnRemoteRequest(code, data3, reply, option);
    MessageParcel data4;
    data4.WriteInterfaceToken(MyLocationEngine::MyLocationCallBack::GetDescriptor());
    code = Location::ILocatorCallback::RECEIVE_ERROR_INFO_EVENT;
    engine->locatorCallback_->OnRemoteRequest(code, data4, reply, option);
    MessageParcel data5;
    data5.WriteInterfaceToken(MyLocationEngine::MyLocationCallBack::GetDescriptor());
    code = Location::ILocatorCallback::RECEIVE_LOCATION_STATUS_EVENT;
    engine->locatorCallback_->OnRemoteRequest(code, data5, reply, option);
    engine->SetValue();
    engine->RegisterLocationChange();
    engine->RegisterSwitchCallback();
    std::string key2 = "testkey2222";
    std::string value2 = "valuetst2222";
    ASSERT_TRUE(engine->IsSwitchOn(key2, value2) == false);
}

/**
 * @tc.number   Telephony_MyLocationEngine_003
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(LocationEngineTest, Telephony_MyLocationEngine_003, Function | MediumTest | Level3)
{
    auto engine1 = std::make_shared<MyLocationEngine>();
    engine1->mylocator = engine1;
    auto engine = engine1->GetInstance();
    engine->SetValue();
    engine->RegisterLocationChange();
    engine->RegisterSwitchCallback();
    if (engine->switchCallback_ == nullptr) {
        return;
    }
    int state = 0;
    engine->switchCallback_->OnSwitchChange(state);
    uint32_t code = 0;
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    engine->switchCallback_->OnRemoteRequest(code, data1, reply, option);
    MessageParcel data2;
    data2.WriteInterfaceToken(MyLocationEngine::MySwitchCallback::GetDescriptor());
    engine->switchCallback_->OnRemoteRequest(code, data2, reply, option);
    MessageParcel data3;
    data3.WriteInterfaceToken(MyLocationEngine::MySwitchCallback::GetDescriptor());
    code = Location::ISwitchCallback::RECEIVE_SWITCH_STATE_EVENT;
    engine->switchCallback_->OnRemoteRequest(code, data3, reply, option);
    engine->SetValue();
    engine->RegisterLocationChange();
    engine->RegisterSwitchCallback();
    std::string key3 = "keytest3333";
    std::string value3 = "valuetest3333";
    ASSERT_TRUE(engine->IsSwitchOn(key3, value3) == false);
}

/**
 * @tc.number   Telephony_EmergencyCallConnectCallback_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(LocationEngineTest, Telephony_EmergencyCallConnectCallback_001, Function | MediumTest | Level3)
{
    auto engine1 = std::make_shared<MyLocationEngine>();
    engine1->mylocator = engine1;
    auto engine = engine1->GetInstance();
    sptr<AAFwk::IAbilityConnection> connectcallback = sptr<EmergencyCallConnectCallback>::MakeSptr();
    CallDetailInfo info;
    AAFwk::Want want;
    engine->ConnectAbility("valuetest1111", connectcallback, want);
    engine->StartEccService(nullptr, info);
    engine->StopEccService(-1);
    std::string bundle = "111";
    std::string ability = "222";
    AppExecFwk::ElementName element("", bundle, ability);
    sptr<IRemoteObject> remoteObject = sptr<EmergencyCallConnectCallback>::MakeSptr();
    int resultCode = 0;
    sptr<AAFwk::IAbilityConnection> connectcallback3 = sptr<EmergencyCallConnectCallback>::MakeSptr();
    connectcallback3->OnAbilityConnectDone(element, remoteObject, resultCode);
    connectcallback3->OnAbilityDisconnectDone(element, resultCode);
    ASSERT_TRUE(connectcallback3 != nullptr);
}

/**
 * @tc.number   Telephony_LocationSubscriber_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(LocationEngineTest, Telephony_LocationSubscriber_001, Function | MediumTest | Level3)
{
    const char* keyword = "switchtest";
    std::string value1 = "true";
    std::string value2 = "false";
    std::string event = "test.test.test";
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(event);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscribe = std::make_shared<LocationSubscriber>(subscribeInfo);
    subscribe->subscriber_ = subscribe;
    if (subscribe->subscriber_ == nullptr) {
        return;
    }
    subscribe->Subscriber();
    EventFwk::CommonEventData eventData;
    AAFwk::Want want;
    want.SetAction(event);
    want.SetParam(keyword, value1);
    eventData.SetWant(want);
    subscribe->subscriber_->OnReceiveEvent(eventData);
    want.SetParam(keyword, value2);
    eventData.SetWant(want);
    subscribe->subscriber_->OnReceiveEvent(eventData);
    ASSERT_TRUE(subscribe->subscriber_ != nullptr);
}

/**
 * @tc.number   Telephony_DataShareSwitchState_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(LocationEngineTest, Telephony_DataShareSwitchState_001, Function | MediumTest | Level3)
{
    auto datashareHelper = std::make_shared<DataShareSwitchState>();
    OHOS::Uri uriTest(string("datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?")
        + string("Proxy=true&key=testKeyWord"));
    const std::string key = "testKeyWord";
    std::string key2 = "testKeyWord2";
    std::string value1 = "test";
    int code = -1;
    auto ret = datashareHelper->QueryData(uriTest, key, value1);
    sptr<AAFwk::IDataAbilityObserver> callback1 = sptr<OOBESwitchObserver>::MakeSptr(key2);
    callback1->OnChange();
    auto datashareHelper2 = std::make_shared<DataShareSwitchState>();
    datashareHelper2->RegisterListenSettingsKey(key2, true, callback1);
    auto datashareHelper3 = std::make_shared<DataShareSwitchState>();
    datashareHelper3->RegisterListenSettingsKey(key2, false, callback1);
    callback1 = nullptr;
    auto datashareHelper4 = std::make_shared<DataShareSwitchState>();
    datashareHelper4->RegisterListenSettingsKey(key2, false, callback1);
    ASSERT_TRUE(ret == code);
    ASSERT_TRUE(value1 == "test");
}

/**
 * @tc.number   Telephony_LocationSystemAbilityListener_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(LocationEngineTest, Telephony_LocationSystemAbilityListener_001, Function | MediumTest | Level3)
{
    int32_t ability1301 = OHOS::DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID;
    int32_t ability2802 = OHOS::LOCATION_LOCATOR_SA_ID;
    int32_t ability2805 = OHOS::LOCATION_NOPOWER_LOCATING_SA_ID;
    const std::string deviceId = "11111111111test";
    sptr<LocationSystemAbilityListener> locationAbility = sptr<LocationSystemAbilityListener>::MakeSptr();
    locationAbility->systemAbilityStatus = {};
    locationAbility->GetSystemAbility(ability1301);
    locationAbility->GetSystemAbility(ability2802);
    locationAbility->GetSystemAbility(ability2805);
    locationAbility->statusChangeListener_ = sptr<LocationSystemAbilityListener>::MakeSptr();
    locationAbility->SystemAbilitySubscriber();
    locationAbility->statusChangeListener_ = nullptr;
    locationAbility->SystemAbilitySubscriber();
    sptr<LocationSystemAbilityListener> callback = sptr<LocationSystemAbilityListener>::MakeSptr();
    callback->OnAddSystemAbility(ability1301, deviceId);
    callback->OnAddSystemAbility(ability2802, deviceId);
    callback->OnAddSystemAbility(ability2805, deviceId);
    callback->OnRemoveSystemAbility(ability1301, deviceId);
    callback->OnRemoveSystemAbility(ability2802, deviceId);
    callback->OnRemoveSystemAbility(ability2805, deviceId);
    ASSERT_TRUE(locationAbility->SystemAbilitySubscriber() == true);
    ASSERT_TRUE(locationAbility->statusChangeListener_ != nullptr);
}
}
// namespace Telephony

}
// namespace OHOS