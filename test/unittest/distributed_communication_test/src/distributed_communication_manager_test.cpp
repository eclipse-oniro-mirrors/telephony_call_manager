/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "cJSON.h"
#include "cs_call.h"
#include "common_type.h"
#include "telephony_errors.h"
#include "call_manager_base.h"
#include "distributed_data_controller.h"
#include "distributed_device_observer.h"
#include "distributed_communication_manager.h"
#include "call_manager_disconnected_details.h"
#include "distributed_data_sink_controller.h"
#include "distributed_sink_switch_controller.h"
#include "distributed_data_source_controller.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class DistributedCommunicationManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

/**
 * @tc.number   Telephony_DcManagerTest_001
 * @tc.name     test normal branch
 * @tc.desc     normal branch test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManagerTest_001, Function | MediumTest | Level1)
{
    int32_t direction = 1;
    DialParaInfo mDialParaInfo;
    DisconnectedDetails details;
    sptr<CallBase> csCall = nullptr;
    std::string devId = "UnitTestDeviceId";
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    std::shared_ptr<IDistributedDeviceCallback> deviceListener = std::make_shared<DistributedDeviceCallback>();

    ASSERT_NO_THROW(dcManager->NewCallCreated(csCall));

    auto deviceObserver = dcManager->GetDistributedDeviceObserver();
    ASSERT_TRUE(deviceObserver == nullptr);

    ASSERT_NO_THROW(dcManager->Init());
    csCall = new CSCall(mDialParaInfo);

    int32_t ret = dcManager->RegDevCallbackWrapper(deviceListener);
    ASSERT_TRUE(ret == TELEPHONY_ERROR);

    ret = dcManager->UnRegDevCallbackWrapper();
    ASSERT_TRUE(ret == TELEPHONY_ERROR);

    ret = dcManager->SwitchDevWrapper(devId, direction);
    ASSERT_TRUE(ret == TELEPHONY_ERROR);

    ASSERT_NO_THROW(dcManager->InitExtWrapper());
    ASSERT_NO_THROW(dcManager->InitExtWrapper()); // extWrapperHandler_ != nullptr case

    ret = dcManager->RegDevCallbackWrapper(deviceListener);
    ASSERT_TRUE(ret != TELEPHONY_ERROR);

    ret = dcManager->UnRegDevCallbackWrapper();
    ASSERT_TRUE(ret != TELEPHONY_ERROR);

    ret = dcManager->SwitchDevWrapper(devId, direction);
    ASSERT_TRUE(ret != TELEPHONY_ERROR);

    ASSERT_FALSE(dcManager->IsDistributedDev(devId));

    deviceObserver = dcManager->GetDistributedDeviceObserver();
    ASSERT_TRUE(deviceObserver != nullptr);
    ASSERT_NO_THROW(dcManager->SetMuted(true));
    ASSERT_NO_THROW(dcManager->MuteRinger());
    ASSERT_NO_THROW(dcManager->ProcessCallInfo(csCall, DistributedDataType::LOCATION));
    ASSERT_NO_THROW(dcManager->NewCallCreated(csCall));
    ASSERT_NO_THROW(dcManager->CallDestroyed(details));
    ASSERT_NO_THROW(dcManager->DeInitExtWrapper());
    ASSERT_NO_THROW(dcManager->dataController_ = std::make_shared<DistributedDataSinkController>());
    ASSERT_NO_THROW(dcManager->SetMuted(true));
    ASSERT_NO_THROW(dcManager->MuteRinger());
    ASSERT_NO_THROW(dcManager->ProcessCallInfo(csCall, DistributedDataType::LOCATION));
    csCall->SetCallType(CallType::TYPE_CS);
    dcManager->role_ = DistributedRole::SINK;
    ASSERT_NO_THROW(dcManager->NewCallCreated(csCall));
    dcManager->peerDevices_.push_back("device");
    ASSERT_NO_THROW(dcManager->NewCallCreated(csCall));
    ASSERT_NO_THROW(dcManager->CallDestroyed(details));
    ASSERT_NO_THROW(dcManager->peerDevices_.clear());
}

/**
 * @tc.number   Telephony_DcManager_DeviceOnline_001
 * @tc.name     test dc device online
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_DeviceOnline_001, Function | MediumTest | Level1)
{
    int32_t devRole = 0; // sink
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;

    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    dcManager->devObserver_ = nullptr;
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    ASSERT_FALSE(dcManager->IsConnected());

    ASSERT_NO_THROW(dcManager->Init());

    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    ASSERT_TRUE(dcManager->IsConnected());
    ASSERT_TRUE(dcManager->IsSinkRole());

    devRole = 1; // source
    deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PAD;
    dcManager->dataController_ = nullptr;
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    ASSERT_FALSE(dcManager->IsSinkRole());
}

/**
 * @tc.number   Telephony_DcManager_DeviceOnline_NoControllers
 * @tc.name     test dc device online with null controllers
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_DeviceOnline_NoControllers, Level1)
{
    int32_t devRole = 0; // sink
    std::string devId = "UnitTestDeviceId_002";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->Init());
    dcManager->dataController_ = nullptr;
    dcManager->devSwitchController_ = nullptr;
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    EXPECT_TRUE(dcManager->IsConnected());
    EXPECT_TRUE(dcManager->dataController_ != nullptr);
    EXPECT_TRUE(dcManager->devSwitchController_ != nullptr);
}

/**
 * @tc.number   Telephony_DcManager_DeviceOnline_WithControllers
 * @tc.name     test dc device online with existing controllers
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_DeviceOnline_WithControllers, Level1)
{
    int32_t devRole = 0; // sink
    std::string devId = "UnitTestDeviceId_003";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->Init());
    // Pre-set controlles to non-null to exercise local copy branch
    dcManager->dataController_ = std::make_shared<DistributedDataSinkController>();
    dcManager->devSwitchController_ = std::make_shared<DistributedSinkSwitchController>();
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    EXPECT_TRUE(dcManager->IsConnected());
    EXPECT_TRUE(dcManager->dataController_ != nullptr);
    EXPECT_TRUE(dcManager->devSwitchController_ != nullptr);
    dcManager->peerDevices_.clear();
    dcManager->dataController_ = nullptr;
    dcManager->devSwitchController_ = nullptr;
}

/**
 * @tc.number   Telephony_DcManager_DeviceOffline_NoControllers
 * @tc.name     test dc device offline with null controllers
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_DeviceOffline_NoControllers, Level1)
{
    int32_t devRole = 0;
    std::string devId = "UnitTestDeviceId_002";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->Init());
    dcManager->dataController_ = nullptr;
    dcManager->devSwitchController_ = nullptr;
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(devId, devName, deviceType, devRole));
    EXPECT_TRUE(dcManager->dataController_ == nullptr);
    EXPECT_TRUE(dcManager->devSwitchController_ == nullptr);
}

/**
 * @tc.number   Telephony_DcManager_DeviceOffline_WithControllers
 * @tc.name     test dc device offline with null controllers
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_DeviceOffline_WithControllers, Level1)
{
    int32_t devRole = 0;
    std::string devId = "UnitTestDeviceId_003";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->Init());
    // Online first to create controllers
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    EXPECT_TRUE(dcManager->IsConnected());
    EXPECT_TRUE(dcManager->dataController_ != nullptr);
    EXPECT_TRUE(dcManager->devSwitchController_ != nullptr);
    // Offline with controllers non-null (local copy unregister branch)
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(devId, devName, deviceType, devRole));
    EXPECT_FALSE(dcManager->IsConnected());
    EXPECT_TRUE(dcManager->dataController_ == nullptr);
    EXPECT_TRUE(dcManager->devSwitchController_ == nullptr);
}

/**
 * @tc.number   Telephony_DcManager_DcManager_OnRemoveSystemAbility
 * @tc.name     test dc manager on remove system ability
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_DcManager_OnRemoveSystemAbility, Level1)
{
    int32_t devRole = 0;
    std::string devId = "UnitTestDeviceId_004";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->Init());
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    EXPECT_TRUE(dcManager->IsConnected());
    // OnRemoveSystemAbility clears everything
    ASSERT_NO_THROW(dcManager->OnRemoveSystemAbility());
    EXPECT_FALSE(dcManager->IsConnected());
    EXPECT_TRUE(dcManager->dataController_ == nullptr);
    EXPECT_TRUE(dcManager->devSwitchController_ == nullptr);
    EXPECT_TRUE(dcManager->peerDevices_.empty());
}


/**
 * @tc.number   Telephony_DcManager_ParseDevIdFromAudioDevice
 * @tc.name     test parse dev id from audio device
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_ParseDevIdFromAudioDevice, Level1)
{
    AudioDevice device;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    // empty address
    EXPECT_EQ(dcManager->ParseDevIdFromAudioDevice(device), "");

    // invalid json string
    std::string deviceId = "not Json string";
    EXPECT_EQ(memcpy_s(device.address, kMaxAddressLen + 1, deviceId.c_str(), deviceId.size()), EOK);
    EXPECT_EQ(dcManager->ParseDevIdFromAudioDevice(device), "");

    // valid devId
    deviceId = "{ \"devId\": \"dev_001\" }";
    EXPECT_EQ(memcpy_s(device.address, kMaxAddressLen + 1, deviceId.c_str(), deviceId.size()), EOK);
    EXPECT_EQ(dcManager->ParseDevIdFromAudioDevice(device), "dev_001");

    // devId is not string type
    deviceId = "{ \"devId\": 123 }";
    EXPECT_EQ(memcpy_s(device.address, kMaxAddressLen + 1, deviceId.c_str(), deviceId.size()), EOK);
    EXPECT_EQ(dcManager->ParseDevIdFromAudioDevice(device), "");

    // json without devId field
    deviceId = "{ \"other\": \"value\" }";
    EXPECT_EQ(memcpy_s(device.address, kMaxAddressLen + 1, deviceId.c_str(), deviceId.size()), EOK);
    EXPECT_EQ(dcManager->ParseDevIdFromAudioDevice(device), "");
}

/**
 * @tc.number   Telephony_DcManager_DeviceOffline_001
 * @tc.name     test dc device offline
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_DeviceOffline_001, Function | MediumTest | Level1)
{
    int32_t devRole = 0;
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;

    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(devId, devName, deviceType, devRole));
    ASSERT_NO_THROW(dcManager->IsConnected());

    ASSERT_NO_THROW(dcManager->Init());
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    ASSERT_TRUE(dcManager->IsConnected());

    ASSERT_NO_THROW(dcManager->OnDeviceOffline(devId, devName, deviceType, devRole));
    ASSERT_FALSE(dcManager->IsConnected());

    dcManager->devObserver_ = nullptr;
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(devId, devName, deviceType, devRole));

    dcManager->devObserver_ = std::make_shared<DistributedDeviceObserver>();
    dcManager->peerDevices_.push_back("hello");
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(devId, devName, deviceType, devRole));
    dcManager->peerDevices_.clear();
}

/**
 * @tc.number   Telephony_DcManager_SwitchDevice_001
 * @tc.name     test switch device
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_SwitchDevice_001, Function | MediumTest | Level1)
{
    int32_t devRole = 1;
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PAD;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->SwitchToSourceDevice());
    ASSERT_NO_THROW(dcManager->Init());
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    ASSERT_TRUE(dcManager->SwitchToSourceDevice());
    dcManager->devSwitchController_->isAudioOnSink_ = true;
    ASSERT_NO_THROW(dcManager->SwitchToSourceDevice());
    dcManager->devSwitchController_->isAudioOnSink_ = false;
}

/**
 * @tc.number   Telephony_DcManager_SwitchToSinkDevice
 * @tc.name     test switch to sink device
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_SwitchToSinkDevice, Function | MediumTest | Level1)
{
    AudioDevice device;
    std::string deviceId = "{ \"devId\": \"101\" }";
    std::string deviceName = "deviceName";
    device.deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PAD;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_FALSE(dcManager->SwitchToSinkDevice(device)); // deviceId is empty
    ASSERT_EQ(memcpy_s(device.address, kMaxAddressLen + 1, deviceId.c_str(), deviceId.size()), EOK);
    ASSERT_EQ(memcpy_s(device.deviceName, kMaxDeviceNameLen + 1, deviceName.c_str(), deviceName.size()), EOK);
    ASSERT_FALSE(dcManager->SwitchToSinkDevice(device)); // not dc device
    dcManager->peerDevices_.push_back("101");
    ASSERT_FALSE(dcManager->SwitchToSinkDevice(device));
    dcManager->devSwitchController_ = nullptr;
    ASSERT_FALSE(dcManager->SwitchToSinkDevice(device));
    dcManager->devSwitchController_ = std::make_shared<DistributedSinkSwitchController>();
    dcManager->devSwitchController_->isAudioOnSink_ = true;
    ASSERT_TRUE(dcManager->SwitchToSinkDevice(device));
    dcManager->devSwitchController_->isAudioOnSink_ = false;
    dcManager->peerDevices_.clear();
    deviceId = "not Json string";
    ASSERT_EQ(memcpy_s(device.address, kMaxAddressLen + 1, deviceId.c_str(), deviceId.size()), EOK);
    ASSERT_EQ(dcManager->ParseDevIdFromAudioDevice(device), "");
}

/**
 * @tc.number   Telephony_DcManager_CallStateUpdatedTest
 * @tc.name     test switch to sink device
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_CallStateUpdatedTest, Function | MediumTest | Level1)
{
    DialParaInfo mDialParaInfo;
    sptr<CallBase> callObjectPtr = nullptr;
    TelCallState priorState = TelCallState::CALL_STATUS_INCOMING;
    TelCallState nextState = TelCallState::CALL_STATUS_ACTIVE;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    EXPECT_NO_THROW(dcManager->CallStateUpdated(callObjectPtr, priorState, nextState));
    callObjectPtr = new CSCall(mDialParaInfo);
    nextState = TelCallState::CALL_STATUS_DISCONNECTING;
    ASSERT_NO_THROW(dcManager->dataController_ = std::make_shared<DistributedDataSourceController>());
    EXPECT_NO_THROW(dcManager->CallStateUpdated(callObjectPtr, priorState, nextState));
}

/**
 * @tc.number   Telephony_DcManager_IsAudioOnSink
 * @tc.name     test IsAudioOnSink with null and valid devSwitchController
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_IsAudioOnSink, Function | Level1)
{
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    dcManager->devSwitchController_ = nullptr;
    EXPECT_FALSE(dcManager->IsAudioOnSink());
    dcManager->devSwitchController_ = std::make_shared<DistributedSinkSwitchController>();
    EXPECT_FALSE(dcManager->IsAudioOnSink());
    dcManager->devSwitchController_->isAudioOnSink_ = true;
    EXPECT_TRUE(dcManager->IsAudioOnSink());
    dcManager->devSwitchController_->isAudioOnSink_ = false;
    dcManager->devSwitchController_ = nullptr;
}

/**
 * @tc.number   Telephony_DcManager_IsDistributedDevAudioDevice
 * @tc.name     test IsDistributedDevAudioDevice with AudioDevice overload
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_IsDistributedDevAudioDevice, Function | Level1)
{
    AudioDevice device;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    EXPECT_FALSE(dcManager->IsDistributedDev(device));
    std::string deviceId = "{ \"devId\": \"dev_002\" }";
    ASSERT_EQ(memcpy_s(device.address, kMaxAddressLen + 1, deviceId.c_str(), deviceId.size()), EOK);
    EXPECT_FALSE(dcManager->IsDistributedDev(device));
    dcManager->peerDevices_.push_back("dev_002");
    EXPECT_TRUE(dcManager->IsDistributedDev(device));
    dcManager->peerDevices_.clear();
}

/**
 * @tc.number   Telephony_DcManager_CallStateUpdated_Disconnected
 * @tc.name     test CallStateUpdated_Disconnected with CALL_STATUS_DISCONNECTED state
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_CallStateUpdated_Disconnected, Function | Level1)
{
    DialParaInfo mDialParaInfo;
    sptr<CallBase> callObjectPtr = new CSCall(mDialParaInfo);
    TelCallState priorState = TelCallState::CALL_STATUS_ACTIVE;
    TelCallState nextState = TelCallState::CALL_STATUS_DISCONNECTED;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    dcManager->dataController_ = nullptr;
    EXPECT_NO_THROW(dcManager->CallStateUpdated(callObjectPtr, priorState, nextState));
    dcManager->dataController_ = std::make_shared<DistributedDataSinkController>();
    EXPECT_NO_THROW(dcManager->CallStateUpdated(callObjectPtr, priorState, nextState));
    dcManager->dataController_ = nullptr;
}

/**
 * @tc.number   Telephony_DcManager_DeviceOnline_SourceRole
 * @tc.name     test device online with source role and existing controller
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_DeviceOnline_SourceRole, Function | Level1)
{
    int32_t devRole = 1; // source
    std::string devId = "UnitTestDeviceId_src";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PAD;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->Init());
    dcManager->dataController_ = nullptr;
    dcManager->devSwitchController_ = nullptr;
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    EXPECT_FALSE(dcManager->IsSinkRole());
    EXPECT_TRUE(dcManager->IsConnected());
    EXPECT_TRUE(dcManager->dataController_ != nullptr);
    EXPECT_TRUE(dcManager->devSwitchController_ != nullptr);
    // Online again with existing controllers
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(devId, devName, deviceType, devRole));
    // Clean up
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(devId, devName, deviceType, devRole));
    dcManager->peerDevices_.clear();
    dcManager->dataController_ = nullptr;
    dcManager->devSwitchController_ = nullptr;
}

/**
 * @tc.number   Telephony_DcManager_DeviceOffline_DevObserverNull
 * @tc.name     test device offline with null devObserver
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_DeviceOffline_DevObserverNull, Function | Level1)
{
    int32_t devRole = 0;
    std::string devId = "UnitTestDeviceId_off";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    dcManager->devObserver_ = nullptr;
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(devId, devName, deviceType, devRole));
    EXPECT_FALSE(dcManager->IsConnected());
}

/**
 * @tc.number   Telephony_DcManager_NewCallCreated_NonCSType
 * @tc.name     test NewCallCreated witn non IMS/CS call type
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_NewCallCreated_NonCSType, Function | Level1)
{
    DialParaInfo mDialParaInfo;
    sptr<CallBase> csCall = new CSCall(mDialParaInfo);
    csCall->SetCallType(CallType::TYPE_OTT);
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->NewCallCreated(csCall));
}

/**
 * @tc.number   Telephony_DcManager_DeInitExtWrapper
 * @tc.name     test DeInitExtWrapper with null handler
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_DeInitExtWrapper, Function | Level1)
{
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->DeInitExtWrapper());
}

/**
 * @tc.number   Telephony_DcManager_SwitchToSourceDevice_Null
 * @tc.name     test SwitchToSourceDevice with null devSwitchController
 * @tc.desc     Function test
 */
HWTEST_F(DistributedCommunicationManagerTest, Telephony_DcManager_SwitchToSourceDevice_Null, Function | Level1)
{
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    dcManager->devSwitchController_ = nullptr;
    EXPECT_FALSE(dcManager->SwitchToSourceDevice());
}

} // namespace Telephony
} // namespace OHOS
