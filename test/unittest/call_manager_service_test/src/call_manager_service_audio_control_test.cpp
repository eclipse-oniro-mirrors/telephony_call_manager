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

#include "audio_control_manager.h"
#include "audio_device_manager.h"
#include "distributed_call_manager.h"

#ifdef SUPPORT_DSOFTBUS
#include "distributed_communication_manager.h"
#endif

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

/**
 * @tc.number   CallManagerService_UpdateDeviceForForegroundCall_NotAnswered
 * @tc.name     test UpdateDeviceForForegroundCall with AnsweredByPhone false
 * @tc.desc     Branch coverage: !foregroundCall->GetAnsweredByPhone() early return path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UpdateDeviceForForegroundCall_NotAnswered, TestSize.Level1)
{
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    ASSERT_NE(audioControlManager, nullptr);
    ASSERT_NE(audioDeviceManager, nullptr);
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_IMS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_ACTIVE;
    dialParaInfo.videoState = VideoStateType::TYPE_VOICE;
    sptr<CallBase> call = new IMSCall(dialParaInfo);
    ASSERT_NE(call, nullptr);
    call->SetIsAnsweredByPhone(false);
    auto distributedCallManager = DelayedSingleton<DistributedCallManager>::GetInstance();
    if (distributedCallManager != nullptr) {
        distributedCallManager->onlineDCallDevices_.clear();
    }
    audioControlManager->audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
    audioDeviceManager->audioDeviceType_ = AudioDeviceType::DEVICE_UNKNOWN;
    AudioDeviceManager::isEarpieceAvailable_ = true;
    audioControlManager->UpdateDeviceForForegroundCall(call);
    EXPECT_EQ(audioDeviceManager->GetCurrentAudioDevice(), AudioDeviceType::DEVICE_UNKNOWN);
}

/**
 * @tc.number   CallManagerService_UpdateDeviceForForegroundCall_CarDeviceOnline
 * @tc.name     test UpdateDeviceForForegroundCall with distributed car device online
 * @tc.desc     Branch coverage: IsDistributedCarDeviceOnline() true early return path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UpdateDeviceForForegroundCall_CarDeviceOnline, TestSize.Level1)
{
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    ASSERT_NE(audioControlManager, nullptr);
    ASSERT_NE(audioDeviceManager, nullptr);
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_IMS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_ACTIVE;
    dialParaInfo.videoState = VideoStateType::TYPE_VOICE;
    sptr<CallBase> call = new IMSCall(dialParaInfo);
    ASSERT_NE(call, nullptr);
    call->SetIsAnsweredByPhone(true);
    auto distributedCallManager = DelayedSingleton<DistributedCallManager>::GetInstance();
    ASSERT_NE(distributedCallManager, nullptr);
    AudioDevice onlineDevice;
    onlineDevice.deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE;
    distributedCallManager->onlineDCallDevices_["test_car_device"] = onlineDevice;
    audioControlManager->audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
    audioDeviceManager->audioDeviceType_ = AudioDeviceType::DEVICE_UNKNOWN;
    AudioDeviceManager::isEarpieceAvailable_ = true;
    audioControlManager->UpdateDeviceForForegroundCall(call);
    EXPECT_EQ(audioDeviceManager->GetCurrentAudioDevice(), AudioDeviceType::DEVICE_UNKNOWN);
    distributedCallManager->onlineDCallDevices_.clear();
}

/**
 * @tc.number   CallManagerService_UpdateDeviceForForegroundCall_SpeakerMode
 * @tc.name     test UpdateDeviceForForegroundCall with speaker mode
 * @tc.desc     Branch coverage: IsSpeakerMode() true -> SetAudioDeviceByAudioMode path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UpdateDeviceForForegroundCall_SpeakerMode, TestSize.Level1)
{
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    ASSERT_NE(audioControlManager, nullptr);
    ASSERT_NE(audioDeviceManager, nullptr);
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_IMS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_ACTIVE;
    dialParaInfo.videoState = VideoStateType::TYPE_VOICE;
    sptr<CallBase> call = new IMSCall(dialParaInfo);
    ASSERT_NE(call, nullptr);
    call->SetIsAnsweredByPhone(true);
    auto distributedCallManager = DelayedSingleton<DistributedCallManager>::GetInstance();
    if (distributedCallManager != nullptr) {
        distributedCallManager->onlineDCallDevices_.clear();
    }
    CallAudioMode speakerMode;
    speakerMode.audioMode = 1;
    speakerMode.audioScene = 0;
    audioDeviceManager->SetCallAudioMode(speakerMode);
    AudioDeviceManager::isSpeakerAvailable_ = true;
    audioControlManager->audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
    AudioDeviceManager::isEarpieceAvailable_ = true;
    audioControlManager->UpdateDeviceForForegroundCall(call);
    EXPECT_TRUE(audioDeviceManager->IsSpeakerMode());
    CallAudioMode normalMode;
    normalMode.audioMode = 0;
    normalMode.audioScene = 0;
    audioDeviceManager->SetCallAudioMode(normalMode);
}

/**
 * @tc.number   CallManagerService_UpdateDeviceForForegroundCall_DeviceTypeChanged
 * @tc.name     test UpdateDeviceForForegroundCall with initDeviceType != currentDeviceType
 * @tc.desc     Branch coverage: initDeviceType != currentDeviceType -> SetAudioDevice path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UpdateDeviceForForegroundCall_DeviceTypeChanged, TestSize.Level1)
{
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    ASSERT_NE(audioControlManager, nullptr);
    ASSERT_NE(audioDeviceManager, nullptr);
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_IMS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_ACTIVE;
    dialParaInfo.videoState = VideoStateType::TYPE_VOICE;
    sptr<CallBase> call = new IMSCall(dialParaInfo);
    ASSERT_NE(call, nullptr);
    call->SetIsAnsweredByPhone(true);
    CallObjectManager::callObjectPtrList_.emplace_back(call);
    auto distributedCallManager = DelayedSingleton<DistributedCallManager>::GetInstance();
    if (distributedCallManager != nullptr) {
        distributedCallManager->onlineDCallDevices_.clear();
    }
    CallAudioMode normalMode;
    normalMode.audioMode = 0;
    normalMode.audioScene = 0;
    audioDeviceManager->SetCallAudioMode(normalMode);
    audioDeviceManager->audioDeviceType_ = AudioDeviceType::DEVICE_EARPIECE;
    AudioDeviceManager::isSpeakerAvailable_ = true;
    AudioDeviceManager::isEarpieceAvailable_ = false;
    AudioDeviceManager::isWiredHeadsetConnected_ = false;
    audioControlManager->audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
    audioControlManager->UpdateDeviceForForegroundCall(call);
    CallObjectManager::callObjectPtrList_.clear();
}

/**
 * @tc.number   CallManagerService_UpdateDeviceForForegroundCall_DeviceTypeSame
 * @tc.name     test UpdateDeviceForForegroundCall with initDeviceType == currentDeviceType
 * @tc.desc     Branch coverage: initDeviceType == currentDeviceType -> no SetAudioDevice path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UpdateDeviceForForegroundCall_DeviceTypeSame, TestSize.Level1)
{
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    ASSERT_NE(audioControlManager, nullptr);
    ASSERT_NE(audioDeviceManager, nullptr);
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_IMS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_ACTIVE;
    dialParaInfo.videoState = VideoStateType::TYPE_VOICE;
    sptr<CallBase> call = new IMSCall(dialParaInfo);
    ASSERT_NE(call, nullptr);
    call->SetIsAnsweredByPhone(true);
    auto distributedCallManager = DelayedSingleton<DistributedCallManager>::GetInstance();
    if (distributedCallManager != nullptr) {
        distributedCallManager->onlineDCallDevices_.clear();
    }
    CallAudioMode normalMode;
    normalMode.audioMode = 0;
    normalMode.audioScene = 0;
    audioDeviceManager->SetCallAudioMode(normalMode);
    audioDeviceManager->audioDeviceType_ = AudioDeviceType::DEVICE_EARPIECE;
    audioControlManager->audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
    AudioDeviceManager::isEarpieceAvailable_ = true;
    audioControlManager->UpdateDeviceForForegroundCall(call);
    EXPECT_EQ(audioDeviceManager->GetCurrentAudioDevice(), AudioDeviceType::DEVICE_EARPIECE);
}

#ifdef SUPPORT_DSOFTBUS
/**
 * @tc.number   CallManagerService_UpdateDeviceForForegroundCall_AudioOnSink
 * @tc.name     test UpdateDeviceForForegroundCall with IsAudioOnSink true
 * @tc.desc     Branch coverage: SUPPORT_DSOFTBUS && IsAudioOnSink() true early return path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UpdateDeviceForForegroundCall_AudioOnSink, TestSize.Level1)
{
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    auto dcMgrInstance = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NE(audioControlManager, nullptr);
    ASSERT_NE(audioDeviceManager, nullptr);
    ASSERT_NE(dcMgrInstance, nullptr);
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_IMS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_ACTIVE;
    dialParaInfo.videoState = VideoStateType::TYPE_VOICE;
    sptr<CallBase> call = new IMSCall(dialParaInfo);
    ASSERT_NE(call, nullptr);
    call->SetIsAnsweredByPhone(true);
    auto distributedCallManager = DelayedSingleton<DistributedCallManager>::GetInstance();
    if (distributedCallManager != nullptr) {
        distributedCallManager->onlineDCallDevices_.clear();
    }
    bool savedIsAudioOnSink = false;
    if (dcMgrInstance->devSwitchController_ != nullptr) {
        savedIsAudioOnSink = dcMgrInstance->devSwitchController_->isAudioOnSink_;
        dcMgrInstance->devSwitchController_->isAudioOnSink_ = true;
    }
    CallAudioMode normalMode;
    normalMode.audioMode = 0;
    normalMode.audioScene = 0;
    audioDeviceManager->SetCallAudioMode(normalMode);
    audioDeviceManager->audioDeviceType_ = AudioDeviceType::DEVICE_UNKNOWN;
    audioControlManager->audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
    AudioDeviceManager::isEarpieceAvailable_ = true;
    audioControlManager->UpdateDeviceForForegroundCall(call);
    EXPECT_EQ(audioDeviceManager->GetCurrentAudioDevice(), AudioDeviceType::DEVICE_UNKNOWN);
    if (dcMgrInstance->devSwitchController_ != nullptr) {
        dcMgrInstance->devSwitchController_->isAudioOnSink_ = savedIsAudioOnSink;
    }
}
#endif

/**
 * @tc.number   CallManagerService_UpdateDeviceForForegroundCall_Integration
 * @tc.name     test UpdateDeviceType with non-video IMS call (answer 2nd call scenario)
 * @tc.desc     Integration: UpdateDeviceType -> UpdateDeviceForForegroundCall path
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UpdateDeviceForForegroundCall_Integration, TestSize.Level1)
{
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    ASSERT_NE(audioControlManager, nullptr);
    ASSERT_NE(audioDeviceManager, nullptr);
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_IMS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_ACTIVE;
    dialParaInfo.videoState = VideoStateType::TYPE_VOICE;
    sptr<CallBase> call = new IMSCall(dialParaInfo);
    ASSERT_NE(call, nullptr);
    call->SetIsAnsweredByPhone(true);
    CallObjectManager::callObjectPtrList_.emplace_back(call);
    auto distributedCallManager = DelayedSingleton<DistributedCallManager>::GetInstance();
    if (distributedCallManager != nullptr) {
        distributedCallManager->onlineDCallDevices_.clear();
    }
    CallAudioMode normalMode;
    normalMode.audioMode = 0;
    normalMode.audioScene = 0;
    audioDeviceManager->SetCallAudioMode(normalMode);
    AudioDeviceManager::isSpeakerAvailable_ = true;
    AudioDeviceManager::isEarpieceAvailable_ = true;
    AudioDeviceManager::isWiredHeadsetConnected_ = false;
    audioControlManager->audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
    audioControlManager->UpdateDeviceType(call);
    CallObjectManager::callObjectPtrList_.clear();
}
} // namespace Telephony
} // namespace OHOS
