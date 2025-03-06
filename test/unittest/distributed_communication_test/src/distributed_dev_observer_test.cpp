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
#include "telephony_errors.h"
#include "distributed_device_observer.h"
#include "distributed_sink_switch_controller.h"
#include "distributed_source_switch_controller.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class DistributedDevObserverTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

/**
 * @tc.number   Telephony_DcDeviceObserver_001
 * @tc.name     test normal branch
 * @tc.desc     normal branch test
 */
HWTEST_F(DistributedDevObserverTest, Telephony_DcDeviceObserver_001, Function | MediumTest | Level1)
{
    int32_t devRole = 0;
    int32_t devType = -1;
    int32_t dcPhone = 0;
    int32_t dcPad = 1;
    int32_t dcPc = 2;
    int32_t dcCallSaId = 66198;
    int32_t otherSaId = -1;
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    auto deviceObserver = std::make_shared<DistributedDeviceObserver>();
    auto deviceCallback = std::make_shared<DistributedDeviceCallback>();
    auto saListener = sptr<DistributedSystemAbilityListener>::MakeSptr();
    std::shared_ptr<IDistributedDeviceStateCallback> callback = nullptr;
    ASSERT_NO_THROW(deviceObserver->Init());
    ASSERT_NO_THROW(deviceObserver->RegisterDevStatusCallback(callback));
    ASSERT_NO_THROW(deviceObserver->RegisterDevStatusCallback(callback)); // duplicate register
    ASSERT_NO_THROW(deviceObserver->UnRegisterDevCallback());

    callback = std::make_shared<DistributedSourceSwitchController>();
    ASSERT_NO_THROW(deviceObserver->RegisterDevStatusCallback(callback));
    ASSERT_NO_THROW(deviceObserver->Init());
    ASSERT_NO_THROW(deviceObserver->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(deviceObserver->OnDeviceOffline(devId, devName, deviceType));
    ASSERT_NO_THROW(deviceObserver->OnRemoveSystemAbility());
    deviceObserver->callbacks_.push_back(callback);
    ASSERT_NO_THROW(deviceObserver->OnDistributedAudioDeviceChange(devId, devName,
        static_cast<AudioDeviceType>(devType), devRole));
    ASSERT_NO_THROW(saListener->OnAddSystemAbility(otherSaId, devId));
    ASSERT_NO_THROW(saListener->OnAddSystemAbility(dcCallSaId, devId));
    ASSERT_NO_THROW(saListener->OnRemoveSystemAbility(otherSaId, devId));
    ASSERT_NO_THROW(saListener->OnRemoveSystemAbility(dcCallSaId, devId));
    int32_t ret = deviceCallback->OnDistributedAudioDeviceChange(devId, devName, devType, devRole);
    ASSERT_TRUE(ret == TELEPHONY_ERROR);

    devType = 1;
    ret = deviceCallback->OnDistributedAudioDeviceChange(devId, devName, devType, devRole);
    ASSERT_TRUE(ret == TELEPHONY_SUCCESS);

    AudioDeviceType convertRes = deviceCallback->ConvertDeviceType(-1);
    EXPECT_TRUE(convertRes == AudioDeviceType::DEVICE_UNKNOWN);
    convertRes = deviceCallback->ConvertDeviceType(dcPhone);
    EXPECT_TRUE(convertRes == AudioDeviceType::DEVICE_DISTRIBUTED_PHONE);
    convertRes = deviceCallback->ConvertDeviceType(dcPad);
    EXPECT_TRUE(convertRes == AudioDeviceType::DEVICE_DISTRIBUTED_PAD);
    convertRes = deviceCallback->ConvertDeviceType(dcPc);
    EXPECT_TRUE(convertRes == AudioDeviceType::DEVICE_DISTRIBUTED_PC);
}

/**
 * @tc.number   Telephony_DcDeviceObserver_002
 * @tc.name     test normal branch
 * @tc.desc     normal branch test
 */
HWTEST_F(DistributedDevObserverTest, Telephony_DcDeviceObserver_002, Function | MediumTest | Level1)
{
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    auto deviceObserver = std::make_shared<DistributedDeviceObserver>();
    std::shared_ptr<IDistributedDeviceStateCallback> callback = nullptr;
    ASSERT_NO_THROW(deviceObserver->Init());
    ASSERT_NO_THROW(deviceObserver->RegisterDevStatusCallback(callback));
    ASSERT_NO_THROW(deviceObserver->RegisterDevStatusCallback(callback)); // duplicate register
    ASSERT_NO_THROW(deviceObserver->UnRegisterDevCallback());

    callback = std::make_shared<DistributedSinkSwitchController>();
    ASSERT_NO_THROW(deviceObserver->RegisterDevStatusCallback(callback));
    ASSERT_NO_THROW(deviceObserver->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(deviceObserver->OnDeviceOffline(devId, devName, deviceType));
    ASSERT_NO_THROW(deviceObserver->OnRemoveSystemAbility());
}

/**
 * @tc.number   Telephony_DcDeviceObserver_DeviceOnlineOffline_001
 * @tc.name     test device online and device offline
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDevObserverTest, Telephony_DcDeviceObserver_DeviceOnlineOffline_001, Function | MediumTest | Level1)
{
    int32_t devRole = 0;
    int32_t devType = -1;
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    auto deviceCallback = std::make_shared<DistributedDeviceCallback>();

    int32_t ret = deviceCallback->OnDistributedDeviceOnline(devId, devName, devType, devRole);
    ASSERT_TRUE(ret == TELEPHONY_ERROR);

    ret = deviceCallback->OnDistributedDeviceOffline(devId, devName, devType, devRole);
    ASSERT_TRUE(ret == TELEPHONY_ERROR);

    devType = 1;
    ret = deviceCallback->OnDistributedDeviceOnline(devId, devName, devType, devRole);
    ASSERT_TRUE(ret == TELEPHONY_SUCCESS);

    ret = deviceCallback->OnDistributedDeviceOffline(devId, devName, devType, devRole);
    ASSERT_TRUE(ret == TELEPHONY_SUCCESS);
}

} // namespace Telephony
} // namespace OHOS
