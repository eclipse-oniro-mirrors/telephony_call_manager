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
#include "distributed_device_switch_controller.h"

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
    auto devSwitchController = std::make_shared<DistributedDeviceSwitchController>();
    ASSERT_NO_THROW(devSwitchController->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(devSwitchController->OnDeviceOffline(devId, devName, deviceType));
    ASSERT_NO_THROW(devSwitchController->OnDistributedAudioDeviceChange(devId, devName,
        deviceType, static_cast<int32_t>(devRole)));
    ASSERT_TRUE(devSwitchController->IsAudioOnSink());
    ASSERT_NO_THROW(devSwitchController->OnRemoveSystemAbility());
    ASSERT_FALSE(devSwitchController->IsAudioOnSink());
}

} // namespace Telephony
} // namespace OHOS
