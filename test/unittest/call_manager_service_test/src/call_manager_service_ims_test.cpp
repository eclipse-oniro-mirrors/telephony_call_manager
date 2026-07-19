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
 * @tc.number   CallManagerService_GetImsConfig_0100
 * @tc.name     test GetImsConfig with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetImsConfig_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->GetImsConfig(0, ImsConfigItem::ITEM_VIDEO_QUALITY);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_UpdateImsCallMode_0100
 * @tc.name     test UpdateImsCallMode
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UpdateImsCallMode_0100, TestSize.Level1)
{
    int32_t ret = service_->UpdateImsCallMode(1, ImsCallMode::CALL_MODE_AUDIO_ONLY);
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

/**
 * @tc.number   CallManagerService_SetCallPreferenceMode_0100
 * @tc.name     test SetCallPreferenceMode with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SetCallPreferenceMode_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->SetCallPreferenceMode(0, 0);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_GetCallWaiting_0100
 * @tc.name     test GetCallWaiting with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetCallWaiting_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->GetCallWaiting(0);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_SetCallWaiting_0100
 * @tc.name     test SetCallWaiting with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SetCallWaiting_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->SetCallWaiting(0, true);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_GetCallRestriction_0100
 * @tc.name     test GetCallRestriction with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetCallRestriction_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->GetCallRestriction(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_SetCallRestriction_0100
 * @tc.name     test SetCallRestriction with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SetCallRestriction_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    CallRestrictionInfo info;
    int32_t ret = service_->SetCallRestriction(0, info);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_SetCallRestrictionPassword_0100
 * @tc.name     test SetCallRestrictionPassword with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SetCallRestrictionPassword_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->SetCallRestrictionPassword(0,
        CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING, "old", "new");
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_GetCallTransferInfo_0100
 * @tc.name     test GetCallTransferInfo with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetCallTransferInfo_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->GetCallTransferInfo(0, CallTransferType::TRANSFER_TYPE_UNCONDITIONAL);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_SetCallTransferInfo_0100
 * @tc.name     test SetCallTransferInfo with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SetCallTransferInfo_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    CallTransferInfo info;
    int32_t ret = service_->SetCallTransferInfo(0, info);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_CanSetCallTransferTime_0100
 * @tc.name     test CanSetCallTransferTime with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_CanSetCallTransferTime_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    bool result = false;
    int32_t ret = service_->CanSetCallTransferTime(0, result);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_GetCallTransferInfo_ByNumber_0100
 * @tc.name     test GetCallTransferInfo by number with invalid number
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetCallTransferInfo_ByNumber_0100, TestSize.Level1)
{
    int32_t ret = service_->GetCallTransferInfo("invalid_number", CallTransferType::TRANSFER_TYPE_UNCONDITIONAL);
    EXPECT_EQ(ret, CALL_ERR_INVALID_CALL_NUMBER);
}

/**
 * @tc.number   CallManagerService_GetCallTransferInfo_ByNumber_0200
 * @tc.name     test GetCallTransferInfo by number with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetCallTransferInfo_ByNumber_0200, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->GetCallTransferInfo("10086", CallTransferType::TRANSFER_TYPE_UNCONDITIONAL);
    EXPECT_NE(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

} // namespace Telephony
} // namespace OHOS
