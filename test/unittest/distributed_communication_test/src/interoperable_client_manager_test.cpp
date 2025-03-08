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
#include "cJSON.h"
#include "interoperable_client_manager.h"
#include "call_object_manager.h"
#include "transmission_manager.h"
#include "interoperable_data_controller.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
 
class InteroperableClientManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};
 
/**
 * @tc.number   Telephony_InteroperableClientManagerTest_001
 * @tc.name     test create controller call
 * @tc.desc     Function test
 */
HWTEST_F(InteroperableClientManagerTest, Telephony_InteroperableClientManagerTest_001, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<InteroperableClientManager>();
    sptr<CallBase> call = nullptr;
    std::string devId = "";
    ASSERT_NO_THROW(controller->OnCallCreated(call, devId));
}
 
/**
 * @tc.number   Telephony_InteroperableClientManagerTest_002
 * @tc.name     test destroy call
 * @tc.desc     Function test
 */
HWTEST_F(InteroperableClientManagerTest, Telephony_InteroperableClientManagerTest_002, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<InteroperableClientManager>();
    ASSERT_NO_THROW(controller->OnCallDestroyed());
}
 
/**
 * @tc.number   Telephony_InteroperableClientManagerTest_003
 * @tc.name     test controller connect remote
 * @tc.desc     Function test
 */
HWTEST_F(InteroperableClientManagerTest, Telephony_InteroperableClientManagerTest_003, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<InteroperableClientManager>();
    std::string devId = "";
    ASSERT_NO_THROW(controller->ConnectRemote(devId));
    ASSERT_NO_THROW(controller->ConnectRemote(devId)); // check session is not null
}
 
} // namespace Telephony
} // namespace OHOS