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
 * @tc.number   Telephony_CallManagerService_GetCallState_0100
 * @tc.name     test GetCallState with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetCallState_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->GetCallState();
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_GetCallState_0200
 * @tc.name     test GetCallState with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetCallState_0200, TestSize.Level1)
{
    int32_t ret = service_->GetCallState();
    EXPECT_GE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_HasCall_0100
 * @tc.name     test HasCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_HasCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    bool ret = service_->HasCall(true);
    EXPECT_FALSE(ret);
    ret = service_->HasCall(false);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number   Telephony_CallManagerService_HasCall_0200
 * @tc.name     test HasCall with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_HasCall_0200, TestSize.Level1)
{
    bool ret = service_->HasCall(true);
    EXPECT_FALSE(ret);
    ret = service_->HasCall(false);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number   Telephony_CallManagerService_IsNewCallAllowed_0100
 * @tc.name     test IsNewCallAllowed with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_IsNewCallAllowed_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    bool enabled = false;
    int32_t ret = service_->IsNewCallAllowed(enabled);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_IsRinging_0100
 * @tc.name     test IsRinging with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_IsRinging_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    bool enabled = false;
    int32_t ret = service_->IsRinging(enabled);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_IsInEmergencyCall_0100
 * @tc.name     test IsInEmergencyCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_IsInEmergencyCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    bool enabled = false;
    int32_t ret = service_->IsInEmergencyCall(enabled);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_IsEmergencyPhoneNumber_0100
 * @tc.name     test IsEmergencyPhoneNumber with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_IsEmergencyPhoneNumber_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    std::u16string number = Str8ToStr16("110");
    bool enabled = false;
    int32_t ret = service_->IsEmergencyPhoneNumber(number, 0, enabled);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_GetMainCallId_0100
 * @tc.name     test GetMainCallId with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetMainCallId_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t mainCallId = 0;
    int32_t ret = service_->GetMainCallId(1, mainCallId);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_GetSubCallIdList_0100
 * @tc.name     test GetSubCallIdList with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetSubCallIdList_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    std::vector<std::u16string> callIdList;
    int32_t ret = service_->GetSubCallIdList(1, callIdList);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_TRUE(callIdList.empty());
}

/**
 * @tc.number   Telephony_CallManagerService_GetCallIdListForConference_0100
 * @tc.name     test GetCallIdListForConference with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetCallIdListForConference_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    std::vector<std::u16string> callIdList;
    int32_t ret = service_->GetCallIdListForConference(1, callIdList);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_TRUE(callIdList.empty());
}

} // namespace Telephony
} // namespace OHOS
