/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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
#include "interoperable_device_observer.h"
#include "interoperable_communication_manager.h"
#include "telephony_permission.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
 
class InteroperableDeviceObserverTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};
 
/**
 * @tc.number   Telephony_InteroperableDeviceObserver_001
 * @tc.name     test normal branch
 * @tc.desc     normal branch test
 */
HWTEST_F(InteroperableDeviceObserverTest, Telephony_InteroperableDeviceObserver_001, Function | MediumTest | Level1)
{
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    uint16_t deviceType = 1;
    auto deviceObserver = std::make_shared<InteroperableDeviceObserver>();
    std::shared_ptr<IInteroperableDeviceStateCallback> callback = nullptr;
    ASSERT_NO_THROW(deviceObserver->Init());
    ASSERT_NO_THROW(deviceObserver->RegisterDevStatusCallback(callback));
    ASSERT_NO_THROW(deviceObserver->RegisterDevStatusCallback(callback)); // duplicate register
    ASSERT_NO_THROW(deviceObserver->UnRegisterDevStatusCallback(callback));
    ASSERT_NO_THROW(deviceObserver->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(deviceObserver->OnDeviceOffline(devId, devName, deviceType));
}
 
} // namespace Telephony
} // namespace OHOS