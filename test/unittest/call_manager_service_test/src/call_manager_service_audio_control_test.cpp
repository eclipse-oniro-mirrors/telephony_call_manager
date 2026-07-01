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
 * @tc.number   CallManagerService_SetAudioDevice_0100
 * @tc.name     test SetAudioDevice with nall callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_  == nullprt path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SetAudioDevice_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    AudioDevice audioDevice;
    audioDevice.deviceType = AudioDeviceType::DEVICE_SPEAKER;
    int32_t ret = service_->SetAudioDevice(audioDevice);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_SetAudioDevice_0200
 * @tc.name     test SetAudioDevice with valid callControlManagerPtr_
 * @tc.desc     Function test for SetAudioDevice
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SetAudioDevice_0200, TestSize.Level1)
{
    AudioDevice audioDevice;
    audioDevice.deviceType = AudioDeviceType::DEVICE_SPEAKER;
    int32_t ret = service_->SetAudioDevice(audioDevice);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_IsRinging_0100
 * @tc.name     test IsRinging with null callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_  == nullprt path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_IsRinging_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    bool enabled = false;
    int32_t ret = service_->IsRinging(enabled);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_IsRinging_0200
 * @tc.name     test IsRinging with valid callControlManagerPtr_
 * @tc.desc     Function test for IsRinging
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_IsRinging_0200, TestSize.Level1)
{
    bool enabled = false;
    int32_t ret = service_->IsRinging(enabled);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_IsInEmergenc`yCall_0100
 * @tc.name     test IsInEmergencyCall with null callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_  == nullprt path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_IsInEmergencyCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    bool enabled = false;
    int32_t ret = service_->IsInEmergencyCall(enabled);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_IsInEmergencyCall_0200
 * @tc.name     test IsInEmergencyCall with valid callControlManagerPtr_
 * @tc.desc     Function test for IsInEmergencyCall
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_IsInEmergencyCall_0200, TestSize.Level1)
{
    bool enabled = false;
    int32_t ret = service_->IsInEmergencyCall(enabled);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_GetCallState_0100
 * @tc.name     test GetCallState with null callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_  == nullprt path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_GetCallState_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->GetCallState();
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_HasCall_0100
 * @tc.name     test HasCall with isInCludeVoipCall true
 * @tc.desc     Branch coverage: isInCludeVoipCall  == true path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HasCall_0100, TestSize.Level1)
{
    int32_t ret = service_->HasCall(true);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number   CallManagerService_HasCall_0200
 * @tc.name     test HasCall with isInCludeVoipCall false
 * @tc.desc     Branch coverage: isInCludeVoipCall  == false path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HasCall_0200, TestSize.Level1)
{
    int32_t ret = service_->HasCall(false);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number   CallManagerService_HasCall_0300
 * @tc.name     test HasCall with callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_  == nullprt path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HasCall_0300, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->HasCall();
    EXPECT_FALSE(ret);
}

/**
 * @tc.number   CallManagerService_IsNewCallAllowed_0100
 * @tc.name     test IsNewCallAllowed with valid callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_  == nullprt path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_IsNewCallAllowed_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    bool enabled = false;
    int32_t ret = service_->IsNewCallAllowed(enabled);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_HoldCall_0100
 * @tc.name     test HoldCall with null callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_  == nullprt path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HoldCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->HoldCall(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_HoldCall_0200
 * @tc.name     test HoldCall with valid callControlManagerPtr_
 * @tc.desc     Function test for HoldCall
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HoldCall_0200, TestSize.Level1)
{
    int32_t ret = service_->HoldCall(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_UnHoldCall_0100
 * @tc.name     test UnHoldCall with null callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_  == nullprt path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UnHoldCall_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->UnHoldCall(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_UnHoldCall_0200
 * @tc.name     test UnHoldCall with valid callControlManagerPtr_
 * @tc.desc     Function test for UnHoldCall
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UnHoldCall_0200, TestSize.Level1)
{
    int32_t ret = service_->UnHoldCall(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_SwitchCall_0300
 * @tc.name     test SwitchCall with callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_  == nullprt path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SwitchCall_0300, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->SwitchCall(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_SwitchCall_0400
 * @tc.name     test SwitchCall with valid callControlManagerPtr_
 * @tc.desc     Function test for SwitchCall
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SwitchCall_0400, TestSize.Level1)
{
    int32_t ret = service_->SwitchCall(1);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}
} // namespace Telephony
} // namespace OHOS
