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
 * @tc.number   Telephony_CallManagerService_StartDtmf_0100
 * @tc.name     test StartDtmf with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_StartDtmf_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->StartDtmf(1, '1');
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_StartDtmf_0200
 * @tc.name     test StartDtmf with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_StartDtmf_0200, TestSize.Level1)
{
    int32_t ret = service_->StartDtmf(1, '1');
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_StopDtmf_0100
 * @tc.name     test StopDtmf with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_StopDtmf_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->StopDtmf(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_StopDtmf_0200
 * @tc.name     test StopDtmf with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_StopDtmf_0200, TestSize.Level1)
{
    int32_t ret = service_->StopDtmf(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_PostDialProceed_0100
 * @tc.name     test PostDialProceed with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_PostDialProceed_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->PostDialProceed(1, true);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_PostDialProceed_0200
 * @tc.name     test PostDialProceed with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_PostDialProceed_0200, TestSize.Level1)
{
    int32_t ret = service_->PostDialProceed(1, true);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

} // namespace Telephony
} // namespace OHOS
