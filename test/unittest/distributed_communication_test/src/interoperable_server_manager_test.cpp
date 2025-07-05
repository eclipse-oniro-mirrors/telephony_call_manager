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
#include "interoperable_data_controller.h"
#include "interoperable_server_manager.h"
#include "interoperable_device_observer.h"
#include "transmission_manager.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
 
class InteroperableServerManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};
 
/**
 * @tc.number   Telephony_InteroperableServerManager_001
 * @tc.name     test normal branch
 * @tc.desc     normal branch test
 */
HWTEST_F(InteroperableServerManagerTest, Telephony_InteroperableServerManager_001, Function | MediumTest | Level1)
{
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    uint16_t deviceType = 1;
    auto deviceObserver = std::make_shared<InteroperableServerManager>();
    ASSERT_NO_THROW(deviceObserver->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(deviceObserver->OnDeviceOffline(devId, devName, deviceType));
}
 
/**
 * @tc.number   Telephony_InteroperableServerManager_002
 * @tc.name     test recv message
 * @tc.desc     normal branch test
 */
HWTEST_F(InteroperableServerManagerTest, Telephony_InteroperableServerManager_002, Function | MediumTest | Level1)
{
    int32_t msgType = 1;
    cJSON *msg = nullptr;
    auto deviceObserver = std::make_shared<InteroperableServerManager>();
    EXPECT_NO_THROW(deviceObserver->HandleSpecificMsg(msgType, msg));

    msg = cJSON_Parse("{ \"testData\": 101 }");
    msgType = static_cast<int32_t>(InteroperableMsgType::DATA_TYPE_QUERY_REQUISITES_DATA);
    EXPECT_NO_THROW(deviceObserver->HandleSpecificMsg(msgType, msg));
    cJSON_Delete(msg);
}

/**
 * @tc.number   Telephony_InteroperableServerManager_003
 * @tc.name     test recv message
 * @tc.desc     normal branch test
 */
HWTEST_F(InteroperableServerManagerTest, Telephony_InteroperableServerManager_003, Function | MediumTest | Level1)
{
    int32_t msgType = 1;
    cJSON *msg = nullptr;
    auto deviceObserver = std::make_shared<InteroperableServerManager>();
    EXPECT_NO_THROW(deviceObserver->HandleSpecificMsg(msgType, msg));

    msg = cJSON_Parse("{ \"testData\": 101 }");
    msgType = static_cast<int32_t>(InteroperableMsgType::DATA_TYPE_QUERY_REQUISITES_DATA);
    EXPECT_NO_THROW(deviceObserver->HandleSpecificMsg(msgType, msg));
    cJSON_Delete(msg);
}

/**
 * @tc.number   Telephony_InteroperableServerManager_004
 * @tc.name     test recv query requisites data msg
 * @tc.desc     normal branch test
 */
HWTEST_F(InteroperableServerManagerTest, Telephony_InteroperableServerManager_004, Function | MediumTest | Level1)
{
    cJSON *msg = cJSON_Parse("{ \"123\": \"123456\"}");
    auto deviceObserver = std::make_shared<InteroperableServerManager>();
    EXPECT_NO_THROW(deviceObserver->OnQueryRequisitesDataMsgReceived(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"phoneNumber\": \"123456\"}");
    EXPECT_NO_THROW(deviceObserver->OnQueryRequisitesDataMsgReceived(msg));
    cJSON_Delete(msg);
}
} // namespace Telephony
} // namespace OHOS