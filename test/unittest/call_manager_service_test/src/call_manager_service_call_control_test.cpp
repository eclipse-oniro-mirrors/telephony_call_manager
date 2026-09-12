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
namespace {
using namespace Security::AccessToken;

HapInfoParams rejectInfoParams = {
    .bundleName = "tel_call_manager_service_gtest",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true,
};

HapInfoParams rejectInfoParamsFalse = {
    .bundleName = "tel_call_manager_service_gtest",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = false,
};

PermissionDef testRejectPlaceCallDef = {
    .permission = "ohos.permission.PLACE_CALL",
    .bundlename = "tel_call_manager_service_gtest",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

PermissionStateFull testRejectPlaceCallState = {
    .grantFlags = { 2 },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.PLACE_CALL",
    .resDeviceID = { "local" },
};

HapPolicyParams rejectPolicyParams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testRejectPlaceCallDef },
    .permStateList = { testRejectPlaceCallState },
};

class NonSystemAppToken {
public:
    NonSystemAppToken()
    {
        currentID_ = GetSelfTokenID();
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(rejectInfoParamsFalse, rejectPolicyParams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenIdEx.tokenIDEx);
    }
    ~NonSystemAppToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }

private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};

class SystemAppNoRejectPermToken {
public:
    SystemAppNoRejectPermToken()
    {
        currentID_ = GetSelfTokenID();
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(rejectInfoParams, rejectPolicyParams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenIdEx.tokenIDEx);
    }
    ~SystemAppNoRejectPermToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }

private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};
} // namespace

/**
 * @tc.number   CallManagerService_DialCall_0100
 * @tc.name     test DialCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_DialCall_0100, TestSize.Level1)
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
 * @tc.number   CallManagerService_DialCall_0200
 * @tc.name     test DialCall with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_DialCall_0200, TestSize.Level1)
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
 * @tc.number   CallManagerService_DialCall_0300
 * @tc.name     test DialCall with btSlotIdUnknown flag
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_DialCall_0300, TestSize.Level1)
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
 * @tc.number   CallManagerService_AnswerCall_0100
 * @tc.name     test AnswerCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_AnswerCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->AnswerCall(1, 0, false);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_AnswerCall_0200
 * @tc.name     test AnswerCall with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_AnswerCall_0200, TestSize.Level1)
{
    int32_t ret = service_->AnswerCall(1, 0, false);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_AnswerCall_0300
 * @tc.name     test AnswerCall without callId
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_AnswerCall_0300, TestSize.Level1)
{
    int32_t ret = service_->AnswerCall();
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_RejectCall_0100
 * @tc.name     test RejectCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_RejectCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    std::u16string textMessage = Str8ToStr16("test");
    int32_t ret = service_->RejectCall(1, true, textMessage);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_RejectCall_0200
 * @tc.name     test RejectCall without parameters
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_RejectCall_0200, TestSize.Level1)
{
    int32_t ret = service_->RejectCall();
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_RejectCall_0300
 * @tc.name     test RejectCall with normal reject type
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_RejectCall_0300, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->RejectCall(RejectType::CALL_REJECT_NORMAL);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_HangUpCall_0100
 * @tc.name     test HangUpCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HangUpCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->HangUpCall(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_HangUpCall_0200
 * @tc.name     test HangUpCall without callId
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HangUpCall_0200, TestSize.Level1)
{
    int32_t ret = service_->HangUpCall();
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_EndCall_0100
 * @tc.name     test EndCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_EndCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    bool ret = service_->EndCall();
    EXPECT_FALSE(ret);
}

/**
 * @tc.number   CallManagerService_EndCall_0200
 * @tc.name     test EndCall with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_EndCall_0200, TestSize.Level1)
{
    bool ret = service_->EndCall();
    EXPECT_FALSE(ret);
}

/**
 * @tc.number   CallManagerService_MakeCall_0100
 * @tc.name     test MakeCall
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_MakeCall_0100, TestSize.Level1)
{
    int32_t ret = service_->MakeCall("10086");
    EXPECT_NE(ret, TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
}

/**
 * @tc.number   CallManagerService_RejectCall_0400
 * @tc.name     test RejectCall with reject type by non-system app
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_RejectCall_0400, TestSize.Level1)
{
    NonSystemAppToken token;
    int32_t ret = service_->RejectCall(RejectType::CALL_REJECT_NORMAL);
    EXPECT_EQ(ret, TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
}

/**
 * @tc.number   CallManagerService_RejectCall_0500
 * @tc.name     test RejectCall with reject type without reject permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_RejectCall_0500, TestSize.Level1)
{
    SystemAppNoRejectPermToken token;
    int32_t ret = service_->RejectCall(RejectType::CALL_REJECT_MISSED_CALL);
    EXPECT_EQ(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

} // namespace Telephony
} // namespace OHOS
