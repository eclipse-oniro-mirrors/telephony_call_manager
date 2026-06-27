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
 * @tc.number   Telephony_CallManagerService_SetVoIPCallState_0100
 * @tc.name     test SetVoIPCallState with null callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_ == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetVoIPCallState_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->SetVoIPCallState(0);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_SetVoIPCallState_0200
 * @tc.name     test SetVoIPCallState with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetVoIPCallState_0200, TestSize.Level1)
{
    int32_t ret = service_->SetVoIPCallState(0);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_GetVoipCallState_0100
 * @tc.name     test GetVoipCallState with null callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_ == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetVoipCallState_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t state = 0;
    int32_t ret = service_->GetVoipCallState(state);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_GetVoipCallState_0200
 * @tc.name     test GetVoipCallState with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetVoipCallState_0200, TestSize.Level1)
{
    int32_t state = 0;
    int32_t ret = service_->GetVoipCallState(state);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_SetVoIPCallInfo_0100
 * @tc.name     test SetVoIPCallInfo with null callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_ == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetVoIPCallInfo_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->SetVoIPCallInfo(0);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_SetVoIPCallInfo_0200
 * @tc.name     test SetVoIPCallInfo with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetVoIPCallInfo_0200, TestSize.Level1)
{
    int32_t ret = service_->SetVoIPCallInfo(0);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_GetVoIPCallInfo_0100
 * @tc.name     test GetVoIPCallInfo with null callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_ == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetVoIPCallInfo_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t callId = 0;
    int32_t state = 0;
    std::u16string phoneNumber;
    int32_t ret = service_->GetVoIPCallInfo(callId, state, phoneNumber);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_GetVoIPCallInfo_0200
 * @tc.name     test GetVoIPCallInfo with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_GetVoIPCallInfo_0200, TestSize.Level1)
{
    int32_t callId = 0;
    int32_t state = 0;
    std::u16string phoneNumber;
    int32_t ret = service_->GetVoIPCallInfo(callId, state, phoneNumber);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_PreloadCallUi_0100
 * @tc.name     test PrereloadCallUi with valid callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_ == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_PreloadCallUi_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->PreloadCallUi(true);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_NotifyVoIPAudioStreamStart_0100
 * @tc.name     test NotifyVoIPAudioStreamStart with invalid caller UID
 * @tc.desc     Branch coverage: IPCSkeleton::GetCallingUid() == AUDIO_UID_INVALID
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_NotifyVoIPAudioStreamStart_0100, TestSize.Level1)
{
    int32_t testdata = 9999;
    int32_t ret = service_->NotifyVoIPAudioStreamStart(testdata);
    EXPECT_EQ(ret, TELEPHONY_ERR_FAIL);
}

/**
 * @tc.number   Telephony_CallManagerService_CancelCallUpgrade_0100
 * @tc.name     test CancelCallUpgrade with null videoControlManager
 * @tc.desc     Branch coverage: videoControlManager == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_CancelCallUpgrade_0100, TestSize.Level1)
{
    int32_t ret = service_->CancelCallUpgrade(1);
    EXPECT_NE(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_RequestCameraCapabilities_0100
 * @tc.name     test RequestCameraCapabilities with valid videoControlManager
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_RequestCameraCapabilities_0100, TestSize.Level1)
{
    int32_t ret = service_->RequestCameraCapabilities(1);
    EXPECT_NE(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_InputDialerSpecialCode_0100
 * @tc.name     test InputDialerSpecialCode with supported code
 * @tc.desc     Function test for InputDialerSpecialCode
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_InputDialerSpecialCode_0100, TestSize.Level1)
{
    std::string specialCode = "*#2846#*";
    int32_t ret = service_->InputDialerSpecialCode(specialCode);
    EXPECT_NE(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_InputDialerSpecialCode_0200
 * @tc.name     test InputDialerSpecialCode with unsupported code
 * @tc.desc     Branch coverage: specialCode is supportSpecialCode_ list
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_InputDialerSpecialCode_0200, TestSize.Level1)
{
    std::string specialCode = "invalid_code";
    int32_t ret = service_->InputDialerSpecialCode(specialCode);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_RemoveMissedIncomingCallNotfication_0100
 * @tc.name     test RemoveMissedIncomingCallNotfication with valid callControlManagerPtr_
 * @tc.desc     Branch coverage: callControlManagerPtr_ == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_RemoveMissedIncomingCallNotfication_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->RemoveMissedIncomingCallNotfication();
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}
} // namespace Telephony
} // namespace OHOS
