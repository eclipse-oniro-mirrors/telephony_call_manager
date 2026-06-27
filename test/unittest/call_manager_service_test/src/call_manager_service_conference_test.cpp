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
 * @tc.number   Telephony_CallManagerService_CombineConference_0100
 * @tc.name     test CombineConference with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_CombineConference_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->CombineConference(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_CombineConference_0200
 * @tc.name     test UnHoldCall with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_CombineConference_0200, TestSize.Level1)
{
    int32_t ret = service_->CombineConference(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_SeparateConference_0100
 * @tc.name     test SeparateConference with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SeparateConference_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->SeparateConference(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_SeparateConference_0200
 * @tc.name     test SeparateConference with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SeparateConference_0200, TestSize.Level1)
{
    int32_t ret = service_->SeparateConference(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_KickOutConference_0100
 * @tc.name     test KickOutConference with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_KickOutConference_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->KickOutConference(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_KickOutConference_0200
 * @tc.name     test KickOutConference with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_KickOutConference_0200, TestSize.Level1)
{
    int32_t ret = service_->KickOutConference(1);   
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_JoinConference_0100
 * @tc.name     test JoinConference with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_JoinConference_0100, TestSize.Level1)
{
    std::vector<std::u16string> numberList;
    numberList.push_back(Str8ToStr16("10086"));
    int32_t ret = service_->JoinConference(1, numberList);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_JoinConference_0200
 * @tc.name     test JoinConference with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_JoinConference_0200, TestSize.Level1)
{
    std::vector<std::u16string> numberList;
    numberList.push_back(Str8ToStr16("10086"));
    int32_t ret = service_->JoinConference(1, numberList);
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
 * @tc.number   Telephony_CallManagerService_GetMainCallId_0200
 * @tc.name     test GetMainCallId with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetMainCallId_0200, TestSize.Level1)
{
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
    std::vector<int32_t> callIdList;
    int32_t ret = service_->GetSubCallIdList(1, callIdList);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_GetCallIdListForConference_0100
 * @tc.name     test GetCallIdListForConference with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetCallIdListForConference_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    std::vector<int32_t> callIdList;
    int32_t ret = service_->GetCallIdListForConference(1, callIdList);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

} // namespace Telephony
} // namespace OHOS