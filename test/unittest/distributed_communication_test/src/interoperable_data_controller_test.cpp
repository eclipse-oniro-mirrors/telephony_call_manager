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
#include "ims_call.h"
#include "server_session.h"
#include "call_object_manager.h"
#include "interoperable_server_manager.h"
#include "interoperable_data_controller.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
 
class SessionCallbackForTest : public ISessionCallback {
public:
    void OnConnected() override {}
    void OnReceiveMsg(const char* data, uint32_t dataLen) override {}
};

class InteroperableClientManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};
 
/**
 * @tc.number   Telephony_InteroperableDataControllerTest_001
 * @tc.name     test send requisites data 2 client
 * @tc.desc     Function test
 */
HWTEST_F(InteroperableClientManagerTest, Telephony_InteroperableDataControllerTest_001,
Function | MediumTest | Level1)
{
    int32_t slotId = 0;
    std::string phoneNum = "123";
    std::shared_ptr<ISessionCallback> callback = std::make_shared<SessionCallbackForTest>();
    auto session = std::make_shared<ServerSession>(callback);
    std::shared_ptr<InteroperableDataController> dataController = std::make_shared<InteroperableServerManager>();
    EXPECT_NO_THROW(dataController->SendRequisiteDataToPeer(slotId, phoneNum)); // session nullptr

    dataController->session_ = session;
    EXPECT_NO_THROW(dataController->SendRequisiteDataToPeer(slotId, phoneNum));
}

/**
 * @tc.number   Telephony_InteroperableDataControllerTest_002
 * @tc.name     test create msg data
 * @tc.desc     Function test
 */
HWTEST_F(InteroperableClientManagerTest, Telephony_InteroperableDataControllerTest_002,
Function | MediumTest | Level1)
{
    int32_t slotId = 0;
    std::string phoneNum = "123";
    InteroperableMsgType msgType = InteroperableMsgType::DATA_TYPE_UNKNOWN;
    std::shared_ptr<ISessionCallback> callback = std::make_shared<SessionCallbackForTest>();
    auto session = std::make_shared<ServerSession>(callback);
    std::shared_ptr<InteroperableDataController> dataController = std::make_shared<InteroperableServerManager>();
    cJSON *msg = nullptr;
    EXPECT_NO_THROW(dataController->HandleMuted(msg));
    msg = cJSON_Parse("{ \"mute\": true }");
    EXPECT_NO_THROW(dataController->HandleMuted(msg));
    EXPECT_NO_THROW(dataController->MuteRinger());
    EXPECT_NO_THROW(dataController->SetMuted(true));

    dataController->session_ = session;
    EXPECT_NO_THROW(dataController->MuteRinger());
    EXPECT_NO_THROW(dataController->SetMuted(true));
    EXPECT_NE(dataController->CreateRequisitesDataMsg(msgType, slotId, phoneNum), "");
    EXPECT_NE(dataController->CreateQueryRequisitesDataMsg(msgType, phoneNum), "");
    EXPECT_NE(dataController->CreateMuteRingerMsg(msgType), "");
    EXPECT_NE(dataController->CreateMuteMsg(msgType, true), "");
    cJSON_Delete(msg);
}

/**
 * @tc.number   Telephony_InteroperableDataControllerTest_003
 * @tc.name     test parse data
 * @tc.desc     Function test
 */
HWTEST_F(InteroperableClientManagerTest, Telephony_InteroperableDataControllerTest_003,
Function | MediumTest | Level1)
{
    std::shared_ptr<InteroperableDataController> dataController = std::make_shared<InteroperableServerManager>();
    cJSON *msg = cJSON_Parse("{ \"test\": 0 }");
    std::string name = "test";
    std::string stringValue = "";
    int32_t intValue = 0;
    EXPECT_TRUE(dataController->GetInt32Value(msg, name, intValue));
    EXPECT_FALSE(dataController->GetStringValue(msg, name, stringValue));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"test\": \"hello\" }");
    EXPECT_FALSE(dataController->GetInt32Value(msg, name, intValue));
    EXPECT_TRUE(dataController->GetStringValue(msg, name, stringValue));
    bool boolValue = false;
    EXPECT_FALSE(dataController->GetBoolValue(msg, name, boolValue));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"test\": true }");
    EXPECT_TRUE(dataController->GetBoolValue(msg, name, boolValue));
    cJSON_Delete(msg);
}

/**
 * @tc.number   Telephony_InteroperableDataControllerTest_004
 * @tc.name     test ercv requisites data
 * @tc.desc     Function test
 */
HWTEST_F(InteroperableClientManagerTest, Telephony_InteroperableDataControllerTest_004,
Function | MediumTest | Level1)
{
    std::shared_ptr<InteroperableDataController> dataController = std::make_shared<InteroperableServerManager>();
    cJSON *msg = cJSON_Parse("{ \"test\": 0 }");
    EXPECT_NO_THROW(dataController->HandleRequisitesData(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"phoneNumber\": \"123\" }");
    EXPECT_NO_THROW(dataController->HandleRequisitesData(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"phoneNumber\": \"123\", \"slotId\": 0 }");
    EXPECT_NO_THROW(dataController->HandleRequisitesData(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"phoneNumber\": \"123\", \"slotId\": 0, \"callType\": true }");
    EXPECT_NO_THROW(dataController->HandleRequisitesData(msg));

    DialParaInfo info;
    sptr<CallBase> call = new IMSCall(info);
    call->SetAccountNumber("123");
    CallObjectManager::callObjectPtrList_.push_back(call);
    EXPECT_NO_THROW(dataController->HandleRequisitesData(msg));
    CallObjectManager::callObjectPtrList_.clear();
    cJSON_Delete(msg);
}
} // namespace Telephony
} // namespace OHOS