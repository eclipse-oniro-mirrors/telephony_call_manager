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
    int32_t direction = 0;
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
    auto sinkSwitchController = std::make_shared<DistributedSinkSwitchController>();
    ASSERT_NO_THROW(sinkSwitchController->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(sinkSwitchController->OnDeviceOffline(devId, devName, deviceType));
    ASSERT_NO_THROW(sinkSwitchController->OnDistributedAudioDeviceChange(devId, devName,
        deviceType, static_cast<int32_t>(devRole)));
    ASSERT_TRUE(sinkSwitchController->IsAudioOnSink());
    ASSERT_NO_THROW(sinkSwitchController->OnRemoveSystemAbility());
    ASSERT_FALSE(sinkSwitchController->IsAudioOnSink());
}

} // namespace Telephony
} // namespace OHOS
