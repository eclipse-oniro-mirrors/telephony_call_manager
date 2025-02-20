/*
 * Copyright (C) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "call_manager_info.h"
#include "call_manager_errors.h"
#include <memory>
#define PRIVATE public
#define PROTECTED public
#include "gtest/gtest.h"
#include "spam_call_adapter.h"
#include "callback_stub_helper.h"
#include "reminder_callback_stub_helper.h"
#include "spam_call_connection.h"
#include "spam_call_stub.h"
#include "time_wait_helper.h"
#include "spam_call_proxy.h"
#include "call_ability_connection.h"
#include "call_setting_ability_connection.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class SpamCallTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void SpamCallTest::SetUpTestCase() {}

void SpamCallTest::TearDownTestCase() {}

void SpamCallTest::SetUp() {}

void SpamCallTest::TearDown() {}

/**
 * @tc.number   Telephony_SpamCallAdapter_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_SpamCallAdapter_001, Function | MediumTest | Level1)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter_ = std::make_shared<SpamCallAdapter>();
    int32_t errCode = -1;
    std::string result = "";
    spamCallAdapter_->GetDetectResult(errCode, result);
    ASSERT_EQ(errCode, -1);
    ASSERT_EQ(result, "");
}

/**
 * @tc.number   Telephony_SpamCallAdapter_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_SpamCallAdapter_002, Function | MediumTest | Level1)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter_ = std::make_shared<SpamCallAdapter>();
    const std::string phoneNumber = "12345678900";
    const int32_t slotId = 0;
    ASSERT_FALSE(spamCallAdapter_->DetectSpamCall(phoneNumber, slotId));
}

/**
 * @tc.number   Telephony_SpamCallAdapter_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_SpamCallAdapter_003, Function | MediumTest | Level1)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter_ = std::make_shared<SpamCallAdapter>();
    std::string jsonData = "{\"detectResult\":1,\"decisionReason\":1}";
    NumberMarkInfo info = {
        .markType = MarkType::MARK_TYPE_NONE,
        .markContent = "",
        .markCount = 0,
        .markSource = "",
        .isCloud = false,
    };
    bool isBlock = false;
    int32_t blockReason = 0;
    spamCallAdapter_->ParseDetectResult(jsonData, isBlock, info, blockReason);
    ASSERT_TRUE(isBlock);
    ASSERT_EQ(blockReason, 1);
}

/**
 * @tc.number   Telephony_CallbackStubHelper_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_CallbackStubHelper_001, Function | MediumTest | Level1)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter = std::make_shared<SpamCallAdapter>();

    CallbackStubHelper callbackStubHelper(spamCallAdapter);
    int32_t errCode = 0;
    std::string result = "{\"detectResult\":0,\"decisionReason\":1002,\"markType\":0}";
    ASSERT_EQ(callbackStubHelper.OnResult(errCode, result), TELEPHONY_SUCCESS);
    ReminderCallbackStubHelper reminderCallbackStubHelper(spamCallAdapter);
    result = "{\"reminderResult\":false,\"slotId\":0,\"reminderTime\":1736428340229,\"remindedTimes\":0}";
    ASSERT_EQ(reminderCallbackStubHelper.OnResult(errCode, result), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallbackStubHelper_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_CallbackStubHelper_002, Function | MediumTest | Level1)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter;

    CallbackStubHelper callbackStubHelper(spamCallAdapter);
    int32_t errCode = 0;
    std::string result;
    ASSERT_NE(callbackStubHelper.OnResult(errCode, result), 0);
    ReminderCallbackStubHelper reminderCallbackStubHelper(spamCallAdapter);
    ASSERT_EQ(reminderCallbackStubHelper.OnResult(errCode, result), 0);
}

/**
 * @tc.number   Telephony_CallbackStubHelper_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_CallbackStubHelper_003, Function | MediumTest | Level1)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter = std::make_shared<SpamCallAdapter>();

    CallbackStubHelper callbackStubHelper(spamCallAdapter);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t errCode = 0;
    std::string result;
    int32_t res = 0;
    res = callbackStubHelper.OnRemoteRequest(errCode, data, reply, option);
    ASSERT_NE(res, ERR_NONE);
}

/**
 * @tc.number   Telephony_SpamCallConnection_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_SpamCallConnection_001, Function | MediumTest | Level1)
{
    std::string phoneNumber = "123456789012";
    int32_t slotId = 0;
    std::shared_ptr<SpamCallAdapter> spamCallAdapter = std::make_shared<SpamCallAdapter>();
    ASSERT_NE(spamCallAdapter, nullptr);
    SpamCallConnection spamCallConnection(phoneNumber, slotId, spamCallAdapter);
    std::string bundle = "111";
    std::string ability = "222";
    AppExecFwk::ElementName element("", bundle, ability);
    sptr<IRemoteObject> remoteObject;
    int resultCode = 0;
    spamCallConnection.OnAbilityConnectDone(element, remoteObject, resultCode);
    ASSERT_EQ(resultCode, 0);
    spamCallConnection.OnAbilityDisconnectDone(element, resultCode);
    ASSERT_EQ(resultCode, 0);
}

/**
 * @tc.number   Telephony_TimeWaitHelper_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_TimeWaitHelper_001, Function | MediumTest | Level1)
{
    TimeWaitHelper timeWaitHelper(10);
    timeWaitHelper.NotifyAll();
    ASSERT_NE(timeWaitHelper.WaitForResult(), true);
}

/**
 * @tc.number   Telephony_SpamCallProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_SpamCallProxy_001, Function | MediumTest | Level1)
{
    sptr<IRemoteObject> remoteObject;
    SpamCallProxy spamCallProxy(remoteObject);
    std::string phoneNumber = "123456789012";
    int32_t slotId = 0;
    std::shared_ptr<SpamCallAdapter> spamCallAdapter = std::make_shared<SpamCallAdapter>();
    ASSERT_NE(spamCallProxy.DetectSpamCall(phoneNumber, slotId, spamCallAdapter), 0);
    ASSERT_NE(spamCallProxy.RequireCallReminder(slotId, spamCallAdapter), 0);
}

/**
 * @tc.number   Telephony_CallAbilityConnection_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_CallAbilityConnection_001, Function | MediumTest | Level1)
{
    std::string commandStr = "11111";
    CallAbilityConnection callAbilityConnection(commandStr);
    std::string bundle = "111";
    std::string ability = "222";
    AppExecFwk::ElementName element("", bundle, ability);
    int resultCode = 0;
    callAbilityConnection.OnAbilityDisconnectDone(element, resultCode);
    ASSERT_EQ(resultCode, 0);
}

/**
 * @tc.number   Telephony_CallSettingAbilityConnection_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_CallSettingAbilityConnection_001, Function | MediumTest | Level1)
{
    std::string commandStr = "11111";
    CallSettingAbilityConnection callSettingAbilityConnection(commandStr);
    std::string bundle = "111";
    std::string ability = "222";
    AppExecFwk::ElementName element("", bundle, ability);
    int resultCode = 0;
    callSettingAbilityConnection.OnAbilityDisconnectDone(element, resultCode);
    ASSERT_EQ(resultCode, 0);
}

}
}
