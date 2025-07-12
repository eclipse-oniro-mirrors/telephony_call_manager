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
#include "ims_call.h"

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
 
/**

 * @tc.number   Telephony_InteroperableClientManagerTest_004
 * @tc.name     test on connected
 * @tc.desc     Function test
 */
HWTEST_F(InteroperableClientManagerTest, Telephony_InteroperableClientManagerTest_004, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<InteroperableClientManager>();
    DialParaInfo info;
    std::string accountNumber = "12345678";
    sptr<CallBase> call = new IMSCall(info);
    EXPECT_NO_THROW(controller->OnConnected());

    controller->phoneNum_ = accountNumber;
    call->SetAccountNumber(accountNumber);
    CallObjectManager::callObjectPtrList_.push_back(call);
    EXPECT_NO_THROW(controller->OnConnected());

    call->SetCallDirection(CallDirection::CALL_DIRECTION_OUT);
    call->SetPhoneOrWatchDial(static_cast<int32_t>(PhoneOrWatchDial::WATCH_DIAL));
    EXPECT_NO_THROW(controller->OnConnected());
}

/**

 * @tc.number   Telephony_InteroperableClientManagerTest_005
 * @tc.name     test call created
 * @tc.desc     Function test
 */
HWTEST_F(InteroperableClientManagerTest, Telephony_InteroperableClientManagerTest_005, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<InteroperableClientManager>();
    DialParaInfo info;
    std::string networkId = "1";
    std::string accountNumber = "123";
    sptr<CallBase> call = new IMSCall(info);
    call->SetAccountNumber(accountNumber);
    call->callState_ = TelCallState::CALL_STATUS_WAITING;
    EXPECT_NO_THROW(controller->CallCreated(call, networkId));
}
} // namespace Telephony
} // namespace OHOS