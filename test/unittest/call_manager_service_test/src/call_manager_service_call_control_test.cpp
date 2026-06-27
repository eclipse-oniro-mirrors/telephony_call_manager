/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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

#include "call_manager_service_test_base.h"

namespace OHOS {
namespace Telephony {

/**
 * @tc.number   Telephony_CallManagerService_DialCall_0100
 * @tc.name     test DialCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_DialCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    std::u16string number = Str8ToStr16("10086");
    AppExecFwk::PacMap extras;
    extras.PutIntValue("accountId", 0);
    extras.PutIntValue("callType", 0);
    int32_t ret = service_->DialCall(number, extras);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_DialCall_0200
 * @tc.name     test DialCall with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_DialCall_0200, TestSize.Level1)
{
    std::u16string number = Str8ToStr16("10086");
    AppExecFwk::PacMap extras;
    extras.PutIntValue("accountId", 0);
    extras.PutIntValue("callType", 0);
    extras.PutIntValue("videoState", 0);
    int32_t ret = service_->DialCall(number, extras);
    EXPECT_GE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_DialCall_0300
 * @tc.name     test DialCall with btSlotIdUnknown flag
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_DialCall_0300, TestSize.Level1)
{
    std::u16string number = Str8ToStr16("10086");
    AppExecFwk::PacMap extras;
    extras.PutIntValue("accountId", 0);
    extras.PutIntValue("callType", 0);
    extras.PutBooleanValue("btSlotIdUnknown", true);
    int32_t ret = service_->DialCall(number, extras);
    EXPECT_GE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_AnswerCall_0100
 * @tc.name     test AnswerCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_AnswerCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->AnswerCall(1, 0, false);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_AnswerCall_0200
 * @tc.name     test AnswerCall with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_AnswerCall_0200, TestSize.Level1)
{
    int32_t ret = service_->AnswerCall(1, 0, false);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_AnswerCall_0300
 * @tc.name     test AnswerCall without callId
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_AnswerCall_0300, TestSize.Level1)
{
    int32_t ret = service_->AnswerCall();
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_RejectCall_0100
 * @tc.name     test RejectCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_RejectCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    std::u16string textMessage = Str8ToStr16("test");
    int32_t ret = service_->RejectCall(1, true, textMessage);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_RejectCall_0200
 * @tc.name     test RejectCall without parameters
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_RejectCall_0200, TestSize.Level1)
{
    int32_t ret = service_->RejectCall();
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_HangUpCall_0100
 * @tc.name     test HangUpCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_HangUpCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->HangUpCall(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_HangUpCall_0200
 * @tc.name     test HangUpCall without callId
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_HangUpCall_0200, TestSize.Level1)
{
    int32_t ret = service_->HangUpCall();
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_EndCall_0100
 * @tc.name     test EndCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_EndCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    bool ret = service_->EndCall();
    EXPECT_FALSE(ret);
}

/**
 * @tc.number   Telephony_CallManagerService_EndCall_0200
 * @tc.name     test EndCall with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_EndCall_0200, TestSize.Level1)
{
    bool ret = service_->EndCall();
    EXPECT_FALSE(ret);
}

/**
 * @tc.number   Telephony_CallManagerService_MakeCall_0100
 * @tc.name     test MakeCall
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_MakeCall_0100, TestSize.Level1)
{
    int32_t ret = service_->MakeCall("10086");
    EXPECT_NE(ret, TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
}

} // namespace Telephony
} // namespace OHOS
