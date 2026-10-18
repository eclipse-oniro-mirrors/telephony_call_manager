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
#include "distributed_communication_manager.h"
#include "distributed_sink_switch_controller.h"
#include "distributed_source_switch_controller.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class DistributedDevSwitchTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

/**
 * @tc.number   Telephony_DcDevSwitch_001
 * @tc.name     test test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitch_001, Function | MediumTest | Level1)
{
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    DistributedRole devRole = DistributedRole::SINK;
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    auto sourceSwitchController = std::make_shared<DistributedSourceSwitchController>();
    ASSERT_NO_THROW(sourceSwitchController->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(sourceSwitchController->OnDeviceOffline(devId, devName, deviceType));
    ASSERT_NO_THROW(sourceSwitchController->OnDistributedAudioDeviceChange(devId, devName,
        deviceType, static_cast<int32_t>(devRole)));
    ASSERT_TRUE(sourceSwitchController->IsAudioOnSink());
    ASSERT_NO_THROW(sourceSwitchController->OnRemoveSystemAbility());
    ASSERT_FALSE(sourceSwitchController->IsAudioOnSink());
    ASSERT_NO_THROW(sourceSwitchController->SwitchDevice(devId, 1));
    auto sinkSwitchController = std::make_shared<DistributedSinkSwitchController>();
    ASSERT_NO_THROW(sinkSwitchController->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(sinkSwitchController->OnDeviceOffline(devId, devName, deviceType));
    ASSERT_NO_THROW(sinkSwitchController->OnDistributedAudioDeviceChange(devId, devName,
        deviceType, static_cast<int32_t>(devRole)));
    ASSERT_TRUE(sinkSwitchController->IsAudioOnSink());
    ASSERT_NO_THROW(sinkSwitchController->OnRemoveSystemAbility());
    ASSERT_FALSE(sinkSwitchController->IsAudioOnSink());
}

/**
 * @tc.number   Telephony_DcDevSwitch_CheckSource
 * @tc.name     test source switch controller with source role
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitch_CheckSource, Function | MediumTest | Level1)
{
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    DistributedRole devRole = DistributedRole::SOURCE;
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PAD;
    auto sourceSwitchController = std::make_shared<DistributedSourceSwitchController>();
    ASSERT_NO_THROW(sourceSwitchController->OnDistributedAudioDeviceChange(devId, devName,
        deviceType, static_cast<int32_t>(devRole)));
    EXPECT_FALSE(sourceSwitchController->IsAudioOnSink());
    ASSERT_NO_THROW(sourceSwitchController->OnRemoveSystemAbility());
    EXPECT_FALSE(sourceSwitchController->IsAudioOnSink());
}

/**
 * @tc.number   Telephony_DcDevSwitch_GetDevAddress
 * @tc.name     test source switch controller with GetDevAddress
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitch_GetDevAddress, Function | Level1)
{
    auto sourceSwitchController = std::make_shared<DistributedSourceSwitchController>();
    std::string devId = "testDevId";
    std::string devName = "testDevName";
    std::string address = sourceSwitchController->GetDevAddress(devId, devName);
    EXPECT_FALSE(address.empty());
    // empty devId and devName
    address = sourceSwitchController->GetDevAddress("", "");
    EXPECT_FALSE(address.empty());
}

#ifdef ABILITY_BLUETOOTH_SUPPORT
/**
 * @tc.number   Telephony_DcDevSwitch_HfpListenerNull
 * @tc.name     test source switch controller OnDeviceOnline with null hfpListener
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitch_HfpListenerNull, Function | Level1)
{
    auto sourceSwitchController = std::make_shared<DistributedSourceSwitchController>();
    std::string devId = "testDevId";
    std::string devName = "testDevName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    ASSERT_NO_THROW(sourceSwitchController->OnDeviceOnline(devId, devName, deviceType));
    EXPECT_TRUE(sourceSwitchController->hfpListener_ != nullptr);
    sourceSwitchController->hfpListener_ = nullptr;
}

/**
 * @tc.number   Telephony_DcDevSwitch_HfpListenerNotNull
 * @tc.name     test source switch controller OnDeviceOnline with hfpListener
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitch_HfpListenerNotNull, Function | Level1)
{
    auto sourceSwitchController = std::make_shared<DistributedSourceSwitchController>();
    std::string devId = "testDevId";
    std::string devName = "testDevName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    ASSERT_NO_THROW(sourceSwitchController->OnDeviceOnline(devId, devName, deviceType));
    EXPECT_TRUE(sourceSwitchController->hfpListener_ != nullptr);
    ASSERT_NO_THROW(sourceSwitchController->OnDeviceOnline(devId, devName, deviceType));
    sourceSwitchController->hfpListener_ = nullptr;
}

/**
 * @tc.number   Telephony_DcDevSwitchOffline_HfpListenerNull
 * @tc.name     test source switch controller OnDeviceOffline with null hfpListener
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitchOffline_HfpListenerNull, Function | Level1)
{
    auto sourceSwitchController = std::make_shared<DistributedSourceSwitchController>();
    std::string devId = "testDevId";
    std::string devName = "testDevName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    ASSERT_NO_THROW(sourceSwitchController->OnDeviceOffline(devId, devName, deviceType));
    EXPECT_TRUE(sourceSwitchController->hfpListener_ == nullptr);
}

/**
 * @tc.number   Telephony_DcDevSwitchOffline_HfpListenerNotNull
 * @tc.name     test source switch controller OnDeviceOffline with hfpListener
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitchOffline_HfpListenerNotNull, Function | Level1)
{
    auto sourceSwitchController = std::make_shared<DistributedSourceSwitchController>();
    std::string devId = "testDevId";
    std::string devName = "testDevName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    ASSERT_NO_THROW(sourceSwitchController->OnDeviceOnline(devId, devName, deviceType));
    EXPECT_TRUE(sourceSwitchController->hfpListener_ != nullptr);
    ASSERT_NO_THROW(sourceSwitchController->OnDeviceOffline(devId, devName, deviceType));
    EXPECT_TRUE(sourceSwitchController->hfpListener_ == nullptr);
}

/**
 * @tc.number   Telephony_DcDevSwitch_RemoveSa_HfpListenerNull
 * @tc.name     test source switch controller OnRemoveSystemAbility with null hfpListener
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitch_RemoveSa_HfpListenerNull, Function | Level1)
{
    auto sourceSwitchController = std::make_shared<DistributedSourceSwitchController>();
    sourceSwitchController->isAudioOnSink_ = true;
    ASSERT_NO_THROW(sourceSwitchController->OnRemoveSystemAbility());
    EXPECT_FALSE(sourceSwitchController->IsAudioOnSink());
    EXPECT_TRUE(sourceSwitchController->hfpListener_ == nullptr);
}

/**
 * @tc.number   Telephony_DcDevSwitch_RemoveSa_HfpListenerNotNull
 * @tc.name     test source switch controller OnRemoveSystemAbility with hfpListener
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitch_RemoveSa_HfpListenerNotNull, Function | Level1)
{
    auto sourceSwitchController = std::make_shared<DistributedSourceSwitchController>();
    std::string devId = "testDevId";
    std::string devName = "testDevName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    ASSERT_NO_THROW(sourceSwitchController->OnDeviceOnline(devId, devName, deviceType));
    EXPECT_TRUE(sourceSwitchController->hfpListener_ != nullptr);
    sourceSwitchController->isAudioOnSink_ = true;
    ASSERT_NO_THROW(sourceSwitchController->OnRemoveSystemAbility());
    EXPECT_FALSE(sourceSwitchController->IsAudioOnSink());
    EXPECT_TRUE(sourceSwitchController->hfpListener_ == nullptr);
}

/**
 * @tc.number   Telephony_DcDevSwitch_OnHfpStackChanged
 * @tc.name     test DcCallSourceHfpListener with OnHfpStackChanged
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitch_OnHfpStackChanged, Function | Level1)
{
    auto listener = std::make_shared<DcCallSourceHfpListener>();
    Bluetooth::BluetoothRemoteDevice device;
    ASSERT_NO_THROW(listener->OnHfpStackChanged(device, USER_SELECTION_ACTION));
    ASSERT_NO_THROW(listener->OnHfpStackChanged(device, DISABLE_FROM_REMOTE_ACTION));
    ASSERT_NO_THROW(listener->OnHfpStackChanged(device, UNWEAR_ACTION));
    bool result = listener->IsNeedSwitchToSource(device, USER_SELECTION_ACTION);
    EXPECT_FALSE(result);
}

/**
 * @tc.number   Telephony_DcDevSwitch_SwitchSource_AudioOnSink
 * @tc.name     test DcCallSourceHfpListener IsNeedSwitchToSource when audio on sink
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevSwitchTest, Telephony_DcDevSwitch_SwitchSource_AudioOnSink, Function | Level1)
{
    auto dcManager = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NO_THROW(dcManager->Init());
    dcManager->devSwitchController_ = std::make_shared<DistributedSourceSwitchController>();
    dcManager->devSwitchController_->isAudioOnSink_ = true;
    auto listener = std::make_shared<DcCallSourceHfpListener>();
    Bluetooth::BluetoothRemoteDevice device;
    bool result = listener->IsNeedSwitchToSource(device, USER_SELECTION_ACTION);
    EXPECT_FALSE(result);
    result = listener->IsNeedSwitchToSource(device, WEAR_ACTION);
    EXPECT_FALSE(result);
    dcManager->devSwitchController_->isAudioOnSink_ = false;
    dcManager->devSwitchController_ = nullptr;
}
#endif

} // namespace Telephony
} // namespace OHOS
