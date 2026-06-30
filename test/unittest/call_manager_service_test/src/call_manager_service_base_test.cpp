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
 * @tc.number   CallManagerService_GetServiceRunningState_0100
 * @tc.name     test GetServiceRunningState when stopped
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetServiceRunningState_0100, TestSize.Level1)
{
    EXPECT_EQ(service_->GetServiceRunningState(),
        static_cast<int32_t>(CallManagerService::ServiceRunningState::STATE_STOPPED));
}

/**
 * @tc.number   CallManagerService_GetBindTime_0100
 * @tc.name     test GetBindTime returns non-empty string
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetBindTime_0100, TestSize.Level1)
{
    std::string bindTime = service_->GetBindTime();
    EXPECT_FALSE(bindTime.empty());
}

/**
 * @tc.number   CallManagerService_GetStartServiceSpent_0100
 * @tc.name     test GetStartServiceSpent returns non-empty string
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetStartServiceSpent_0100, TestSize.Level1)
{
    std::string spentTime = service_->GetStartServiceSpent();
    EXPECT_FALSE(spentTime.empty());
}

/**
 * @tc.number   CallManagerService_Dump_0100
 * @tc.name     test Dump with invalid fd
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_Dump_0100, TestSize.Level1)
{
    std::vector<std::u16string> args;
    int32_t ret = service_->Dump(-1, args);
    EXPECT_EQ(ret, TELEPHONY_ERR_ARGUMENT_INVALID);
}

/**
 * @tc.number   CallManagerService_GetBundleInfo_0100
 * @tc.name     test GetBundleInfo returns non-empty string
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetBundleInfo_0100, TestSize.Level1)
{
    std::string bundleInfo = service_->GetBundleInfo();
    EXPECT_FALSE(bundleInfo.empty());
}

/**
 * @tc.number   CallManagerService_OnAddSystemAbility_0100
 * @tc.name     test OnAddSystemAbility with AUDIO_POLICY_SERVICE_ID
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_OnAddSystemAbility_0100, TestSize.Level1)
{
    service_->OnAddSystemAbility(AUDIO_POLICY_SERVICE_ID, "");
}

/**
 * @tc.number   CallManagerService_OnAddSystemAbility_0200
 * @tc.name     test OnAddSystemAbility with DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_OnAddSystemAbility_0200, TestSize.Level1)
{
    service_->OnAddSystemAbility(DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID, "");
}

/**
 * @tc.number   CallManagerService_OnAddSystemAbility_0300
 * @tc.name     test OnAddSystemAbility with unknown id
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_OnAddSystemAbility_0300, TestSize.Level1)
{
    service_->OnAddSystemAbility(9999, "");
}

/**
 * @tc.number   CallManagerService_OnAddSystemAbility_0400
 * @tc.name     test OnAddSystemAbility with various system abilities
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_OnAddSystemAbility_0400, TestSize.Level1)
{
    service_->OnAddSystemAbility(AUDIO_POLICY_SERVICE_ID, "");
    service_->OnAddSystemAbility(DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID, "");
    service_->OnAddSystemAbility(-1, "");
    service_->OnAddSystemAbility(0, "");
    EXPECT_NE(service_, nullptr);
}

/**
 * @tc.number   CallManagerService_HasDistributedCommunicationCapability_0100
 * @tc.name     test HasDistributedCommunicationCapability
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HasDistributedCommunicationCapability_0100, TestSize.Level1)
{
    bool ret = service_->HasDistributedCommunicationCapability();
    EXPECT_FALSE(ret);
}

/**
 * @tc.number   CallManagerService_SetCallPolicyInfo_0100
 * @tc.name     test SetCallPolicyInfo with non-EDM UID
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SetCallPolicyInfo_0100, TestSize.Level1)
{
    std::vector<std::string> dialingList;
    std::vector<std::string> incomingList;
    int32_t ret = service_->SetCallPolicyInfo(true, dialingList, true, incomingList);
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

/**
 * @tc.number   CallManagerService_SetCallStatusManager_0100
 * @tc.name     test SetCallStatusManager
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SetCallStatusManager_0100, TestSize.Level1)
{
    auto callStatusManager = std::make_shared<CallStatusManager>();
    service_->SetCallStatusManager(callStatusManager);
    EXPECT_EQ(service_->callStatusManagerPtr_, callStatusManager);
}

} // namespace Telephony
} // namespace OHOS
