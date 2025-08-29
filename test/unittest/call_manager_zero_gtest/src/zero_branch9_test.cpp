/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#define private public
#define protected public
#include "audio_control_manager.h"
#include "bluetooth_call.h"
#include "call_state_processor.h"
#include "call_control_manager.h"
#include "common_event_manager.h"
#include "gtest/gtest.h"
#include "ims_call.h"
#include "voip_call.h"
#include "int_wrapper.h"
#include "call_manager_connect.h"

namespace OHOS::Telephony {
using namespace testing::ext;
constexpr int WAIT_TIME = 5;
constexpr int VALID_CALL_ID = 1;
constexpr int32_t CRS_TYPE = 2;
constexpr const char* NUMBER = "10086";
static constexpr const char *SYSTEM_VIDEO_RING = "system_video_ring";
static constexpr const char *STR_MP4 = "123.mp4";
static constexpr const char *STR_NOT_MP4 = "1234567";
class ZeroBranch9Test : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
};
void ZeroBranch9Test::SetUp()
{
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioMicStateChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioDeviceChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioPreferDeviceChangeCallback();
}

void ZeroBranch9Test::TearDown()
{
    DelayedSingleton<AudioProxy>::GetInstance()->UnsetAudioMicStateChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->UnsetDeviceChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->UnsetAudioPreferDeviceChangeCallback();
}

void ZeroBranch9Test::SetUpTestCase() {}
    
void ZeroBranch9Test::TearDownTestCase()
{
    sleep(WAIT_TIME);
}

/**
 * @tc.number   Telephony_AudioControlManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_001, TestSize.Level0)
{
    DialParaInfo mDialParaInfo;
    mDialParaInfo.accountId = 0;
    sptr<OHOS::Telephony::CallBase> callObjectPtr = nullptr;
    DelayedSingleton<CallStateProcessor>::GetInstance()->holdingCalls_.insert(1);
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    audioControl->VideoStateUpdated(callObjectPtr, VideoStateType::TYPE_VOICE, VideoStateType::TYPE_VIDEO);
    callObjectPtr = new IMSCall(mDialParaInfo);
    callObjectPtr->SetCallType(CallType::TYPE_IMS);
    audioControl->VideoStateUpdated(callObjectPtr, VideoStateType::TYPE_VOICE, VideoStateType::TYPE_VIDEO);
    callObjectPtr->SetCrsType(2);
    audioControl->VideoStateUpdated(callObjectPtr, VideoStateType::TYPE_VOICE, VideoStateType::TYPE_VIDEO);
    audioControl->UpdateDeviceTypeForVideoOrSatelliteCall();
    audioControl->MuteNetWorkRingTone(true);
    audioControl->IsBtOrWireHeadPlugin();
    ASSERT_FALSE(audioControl->IsVideoCall(VideoStateType::TYPE_RECEIVE_ONLY));
    sptr<CallBase> call = nullptr;
    audioControl->IncomingCallHungUp(call, false, "");
    audioControl->CallStateUpdated(call, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ALERTING);
    call = new VoIPCall(mDialParaInfo);
    call->SetCallType(CallType::TYPE_VOIP);
    audioControl->CallStateUpdated(call, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ALERTING);
    audioControl->HandleCallStateUpdated(call, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ANSWERED);
    audioControl->HandleNextState(call, TelCallState::CALL_STATUS_ALERTING);
    audioControl->HandleNextState(call, TelCallState::CALL_STATUS_INCOMING);
    audioControl->HandleNextState(call, TelCallState::CALL_STATUS_WAITING);
    audioControl->HandlePriorState(call, TelCallState::CALL_STATUS_ALERTING);
    audioControl->HandlePriorState(call, TelCallState::CALL_STATUS_INCOMING);
    audioControl->HandlePriorState(call, TelCallState::CALL_STATUS_WAITING);
    audioControl->HandlePriorState(call, TelCallState::CALL_STATUS_HOLDING);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    audioControl->HandlePriorState(call, TelCallState::CALL_STATUS_INCOMING);
    audioControl->HandlePriorState(call, TelCallState::CALL_STATUS_WAITING);
    audioControl->UpdateForegroundLiveCall();
    audioControl->ProcessAudioWhenCallActive(call);
    std::string emptyNumber = "";
    call->SetAccountNumber(emptyNumber);
    audioControl->HandleNewActiveCall(call);
    call->SetAccountNumber(NUMBER);
    call->SetCallType(CallType::TYPE_CS);
    audioControl->HandleNewActiveCall(call);
    call->SetCallType(CallType::TYPE_SATELLITE);
    audioControl->HandleNewActiveCall(call);
    call->SetCallType(CallType::TYPE_ERR_CALL);
    audioControl->HandleNewActiveCall(call);
    audioControl->GetCallList().empty();
    ASSERT_TRUE(audioControl->GetCurrentActiveCall() == nullptr);
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_AudioControlManager_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_002, TestSize.Level1)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    AudioDevice device;
    device.deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PAD;
    if (memset_s(device.address, kMaxAddressLen + 1, 0, kMaxAddressLen + 1) != EOK) {
        return;
    }
    audioControl->SetAudioDevice(device);
    device.deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    audioControl->SetAudioDevice(device);
    device.deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE;
    audioControl->SetAudioDevice(device);
    device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
    audioControl->SetAudioDevice(device);
    audioControl->PlaySoundtone();
    audioControl->GetInitAudioDeviceType();
    audioControl->SetMute(false);
    audioControl->MuteRinger();
    audioControl->PlayCallEndedTone(CallEndedType::PHONE_IS_BUSY);
    audioControl->PlayCallEndedTone(CallEndedType::CALL_ENDED_NORMALLY);
    audioControl->PlayCallEndedTone(CallEndedType::UNKNOWN);
    audioControl->PlayCallEndedTone(CallEndedType::INVALID_NUMBER);
    audioControl->PlayCallEndedTone(static_cast<CallEndedType>(5));
    audioControl->GetCallBase(VALID_CALL_ID);
    audioControl->IsEmergencyCallExists();
    audioControl->SetToneState(ToneState::TONEING);
    audioControl->IsNumberAllowed(NUMBER);
    audioControl->audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
    ASSERT_TRUE(audioControl->IsAudioActivated());
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_AudioControlManager_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_003, TestSize.Level0)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    audioControl->IsTonePlaying();
    audioControl->IsCurrentRinging();
    audioControl->PlayRingback();
    audioControl->StopWaitingTone();
    audioControl->PlayDtmfTone('a');
    audioControl->StopDtmfTone();
    audioControl->OnPostDialNextChar('a');
    audioControl->IsSoundPlaying();
    ASSERT_TRUE(audioControl->IsVideoCall(VideoStateType::TYPE_VIDEO));
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_AudioControlManager_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_004, TestSize.Level0)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    audioControl->isCrsVibrating_ = true;
    audioControl->MuteNetWorkRingTone(true);
    DisconnectedDetails details;
    audioControl->CallDestroyed(details);
    audioControl->toneState_ = ToneState::CALLENDED;
    audioControl->StopCallTone();
    audioControl->tone_ = nullptr;
    audioControl->StopCallTone();
    audioControl->GetAudioInterruptState();
    audioControl->SetVolumeAudible();
    audioControl->ringState_ = RingState::RINGING;
    audioControl->MuteRinger();
    ASSERT_TRUE(audioControl->GetCurrentActiveCall() == nullptr);
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_AudioControlManager_005
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_005, TestSize.Level0)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    audioControl->ExcludeBluetoothSco();
    audioControl->UnexcludeBluetoothSco();
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
    audioControl->UpdateDeviceTypeForCrs(deviceType);
    EXPECT_TRUE(audioControl->IsRingingVibrateModeOn());
    auto callControl = DelayedSingleton<CallControlManager>::GetInstance();
    callControl->SetVoIPCallState(1);
    EXPECT_TRUE(audioControl->IsVoIPCallActived());
    callControl->SetVoIPCallState(0);
    EXPECT_FALSE(audioControl->IsVoIPCallActived());
    callControl->SetVoIPCallState(-1);
    EXPECT_FALSE(audioControl->IsVoIPCallActived());
    CallObjectManager::callObjectPtrList_.clear();
    EXPECT_FALSE(audioControl->IsBtCallDisconnected());
    DialParaInfo info;
    sptr<CallBase> call = new BluetoothCall(info, "");
    CallObjectManager::AddOneCallObject(call);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    EXPECT_FALSE(audioControl->IsBtCallDisconnected());
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ENDED);
    EXPECT_FALSE(audioControl->IsBtCallDisconnected());
    call->SetCallType(CallType::TYPE_BLUETOOTH);
    EXPECT_TRUE(audioControl->IsBtCallDisconnected());
    audioControl->SetRingToneVolume(0.0f);
    audioControl->PlaySoundtone();
    audioControl->SetRingToneVolume(0.0f);
    audioControl->SetRingToneVolume(0.5f);
    audioControl->SetRingToneVolume(1.5f);
    audioControl->StopSoundtone();
    audioControl->ringState_ = RingState::RINGING;
    audioControl->StopRingtone();
    audioControl->ring_ = nullptr;
    audioControl->StopRingtone();
    audioControl->PlayCallTone(ToneDescriptor::TONE_ENGAGED);
    EXPECT_EQ(audioControl->StopRingback(), 0);
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_AudioControlManager_006
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_006, TestSize.Level0)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    DialParaInfo info;
    sptr<CallBase> ringingCall = new IMSCall(info);
    ringingCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    audioControl->SaveVoiceVolume(1);
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(ringingCall));
    audioControl->SaveVoiceVolume(-1);
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(ringingCall));
    audioControl->soundState_ = SoundState::SOUNDING;
    audioControl->SetRingState(RingState::RINGING);
    ringingCall->SetCrsType(2);
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    audioDeviceManager->isWiredHeadsetConnected_ = true;
    auto callStateProcessor = DelayedSingleton<CallStateProcessor>::GetInstance();
    callStateProcessor->incomingCalls_.insert(1);
    callStateProcessor->alertingCalls_.clear();
    ringingCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    CallPolicy callPolicy;
    callPolicy.AddOneCallObject(ringingCall);
    audioControl->ringState_ = RingState::STOPPED;
    audioControl->soundState_ = SoundState::STOPPED;
    audioControl->isCrsVibrating_ =true;
    ASSERT_FALSE(audioControl->PlayRingtone());
    ASSERT_FALSE(audioControl->StopForNoRing());
    ASSERT_TRUE(audioControl->PlayForNoRing());
    ASSERT_TRUE(audioControl->StopForNoRing());
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_AudioControlManager_007
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_007, Function | MediumTest | Level3)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    ASSERT_NO_THROW(audioControl->PostProcessRingtone());
    DialParaInfo info;
    sptr<CallBase> ringingCall = new IMSCall(info);
    AAFwk::WantParams params = ringingCall->GetExtraParams();
    params.SetParam("isNeedMuteRing", AAFwk::Integer::Box(1));
    ringingCall->SetExtraParams(params);
    ringingCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    CallObjectManager::AddOneCallObject(ringingCall);
    ASSERT_NO_THROW(audioControl->PostProcessRingtone());
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_AudioControlManager_008
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_008, Function | MediumTest | Level3)
{
    DialParaInfo info;
    sptr<CallBase> call = new IMSCall(info);
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    audioControl->isCrsStartSoundTone_ = false;
    audioControl->HandleCallStateUpdated(call, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NO_THROW(audioControl->UnmuteSoundTone());
    audioControl->HandleNextState(call, TelCallState::CALL_STATUS_ANSWERED);
    audioControl->HandlePriorState(call, TelCallState::CALL_STATUS_INCOMING);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    audioControl->UpdateForegroundLiveCall();
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(call));
    audioControl->isCrsStartSoundTone_ = true;
    ASSERT_NO_THROW(audioControl->UnmuteSoundTone());
    audioControl->HandlePriorState(call, TelCallState::CALL_STATUS_INCOMING);
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(call));
    ASSERT_NO_THROW(audioControl->MuteNetWorkRingTone(false));
    audioControl->HandleCallStateUpdated(call, TelCallState::CALL_STATUS_INCOMING,
        TelCallState::CALL_STATUS_DISCONNECTING);
    ASSERT_NO_THROW(DelayedSingleton<CallStateProcessor>::GetInstance()->DeleteCall(call->GetCallID(),
        TelCallState::CALL_STATUS_ACTIVE));
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number Telephony_AudioControlManager_009
 * @tc.name   test error branch
 * @tc.desc   Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_009, Function | MediumTest | Level3)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    sptr<OHOS::Telephony::CallBase> crsCallObjectPtr = nullptr;
    DialParaInfo mDialParaInfo;
    mDialParaInfo.accountId = 0;
    mDialParaInfo.callId = 0;
    mDialParaInfo.index = 0;
    crsCallObjectPtr = new IMSCall(mDialParaInfo);
    crsCallObjectPtr->SetCallType(CallType::TYPE_IMS);
    CallObjectManager::AddOneCallObject(crsCallObjectPtr);
    ASSERT_NO_THROW(audioControl->HandleNextState(crsCallObjectPtr, TelCallState::CALL_STATUS_DISCONNECTED));
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(crsCallObjectPtr));
    audioControl->isCrsVibrating_ = true;
    ASSERT_NO_THROW(audioControl->HandleNextState(crsCallObjectPtr, TelCallState::CALL_STATUS_DISCONNECTED));
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(crsCallObjectPtr));
    audioControl->isVideoRingVibrating_ = true;
    ASSERT_NO_THROW(audioControl->HandleNextState(crsCallObjectPtr, TelCallState::CALL_STATUS_DISCONNECTED));
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(crsCallObjectPtr));
    crsCallObjectPtr->SetCrsType(2);
    ASSERT_NO_THROW(audioControl->HandleNextState(crsCallObjectPtr, TelCallState::CALL_STATUS_DISCONNECTED));
    crsCallObjectPtr->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    ASSERT_NO_THROW(audioControl->HandleNextState(crsCallObjectPtr, TelCallState::CALL_STATUS_DISCONNECTED));
    sptr<OHOS::Telephony::CallBase> videoRingCallObjectPtr = nullptr;
    mDialParaInfo.callId = 1;
    mDialParaInfo.index = 1;
    videoRingCallObjectPtr = new IMSCall(mDialParaInfo);
    CallObjectManager::AddOneCallObject(videoRingCallObjectPtr);
    ASSERT_NO_THROW(audioControl->HandleNextState(crsCallObjectPtr, TelCallState::CALL_STATUS_DISCONNECTED));
    crsCallObjectPtr->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    ASSERT_NO_THROW(audioControl->HandleNextState(crsCallObjectPtr, TelCallState::CALL_STATUS_DISCONNECTED));
    ContactInfo contactInfo = videoRingCallObjectPtr->GetCallerInfo();
    memcpy_s(contactInfo.ringtonePath, FILE_PATH_MAX_LEN, SYSTEM_VIDEO_RING, strlen(SYSTEM_VIDEO_RING));
    videoRingCallObjectPtr->SetCallerInfo(contactInfo);
    ASSERT_NO_THROW(audioControl->HandleNextState(crsCallObjectPtr, TelCallState::CALL_STATUS_DISCONNECTED));
    ASSERT_NO_THROW(CallObjectManager::DeleteOneCallObject(crsCallObjectPtr->GetCallID()));
    ASSERT_NO_THROW(CallObjectManager::DeleteOneCallObject(videoRingCallObjectPtr->GetCallID()));
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_AudioControlManager_010
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_010, Function | MediumTest | Level3)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    DialParaInfo mDialParaInfo;
    mDialParaInfo.accountId = 0;
    mDialParaInfo.callId = 0;
    mDialParaInfo.index = 0;
    sptr<OHOS::Telephony::CallBase> videoRingCallObjectPtr = nullptr;
    videoRingCallObjectPtr = new IMSCall(mDialParaInfo);
    videoRingCallObjectPtr->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    CallObjectManager::AddOneCallObject(videoRingCallObjectPtr);
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(videoRingCallObjectPtr));
    audioControl->isCrsVibrating_ = true;
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(videoRingCallObjectPtr));
    audioControl->isVideoRingVibrating_ = true;
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(videoRingCallObjectPtr));
    sptr<OHOS::Telephony::CallBase> videoRingCallObjectPtr1 = nullptr;
    mDialParaInfo.callId = 1;
    mDialParaInfo.index = 1;
    videoRingCallObjectPtr1 = new IMSCall(mDialParaInfo);
    videoRingCallObjectPtr1->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    CallObjectManager::AddOneCallObject(videoRingCallObjectPtr1);
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(videoRingCallObjectPtr));
    ContactInfo contactInfo = videoRingCallObjectPtr1->GetCallerInfo();
    memcpy_s(contactInfo.ringtonePath, FILE_PATH_MAX_LEN, SYSTEM_VIDEO_RING, strlen(SYSTEM_VIDEO_RING));
    videoRingCallObjectPtr1->SetCallerInfo(contactInfo);
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(videoRingCallObjectPtr));
    memcpy_s(contactInfo.personalNotificationRingtone, FILE_PATH_MAX_LEN, "abc.mp4", strlen("abc.mp4"));
    videoRingCallObjectPtr1->SetCallerInfo(contactInfo);
    ASSERT_NO_THROW(audioControl->ProcessAudioWhenCallActive(videoRingCallObjectPtr));
    ASSERT_NO_THROW(CallObjectManager::DeleteOneCallObject(videoRingCallObjectPtr->GetCallID()));
    ASSERT_NO_THROW(CallObjectManager::DeleteOneCallObject(videoRingCallObjectPtr1->GetCallID()));
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_DealVideoRingPath_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_DealVideoRingPath_001, TestSize.Level0)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    sptr<CallBase> callObjectPtr = nullptr;
    DialParaInfo dialParaInfo;
    dialParaInfo.index = 0;
    dialParaInfo.accountId = 0;
    dialParaInfo.callType = CallType::TYPE_CS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_INCOMING;
    callObjectPtr = new IMSCall(dialParaInfo);
    ContactInfo contactInfo;
    ASSERT_FALSE(audioControl->NeedPlayVideoRing(contactInfo, callObjectPtr));
    AccessToken token;
    ASSERT_FALSE(audioControl->NeedPlayVideoRing(contactInfo, callObjectPtr));
    memcpy_s(contactInfo.ringtonePath, 3, "123", 3);
    ASSERT_FALSE(audioControl->NeedPlayVideoRing(contactInfo, callObjectPtr));
    memcpy_s(contactInfo.personalNotificationRingtone, strlen(STR_MP4) + 1, STR_MP4, strlen(STR_MP4));
    ASSERT_TRUE(audioControl->NeedPlayVideoRing(contactInfo, callObjectPtr));
    memcpy_s(contactInfo.ringtonePath, strlen(SYSTEM_VIDEO_RING) + 1, SYSTEM_VIDEO_RING, strlen(SYSTEM_VIDEO_RING));
    memcpy_s(contactInfo.personalNotificationRingtone, strlen(STR_NOT_MP4) + 1, STR_NOT_MP4, strlen(STR_NOT_MP4));
    ASSERT_FALSE(audioControl->NeedPlayVideoRing(contactInfo, callObjectPtr));
    auto callControl = DelayedSingleton<CallControlManager>::GetInstance();
    callControl->SetWearState(WEAR_STATUS_OFF);
    ASSERT_FALSE(audioControl->NeedPlayVideoRing(contactInfo, callObjectPtr));
    callObjectPtr->SetCrsType(CRS_TYPE);
    ASSERT_FALSE(audioControl->NeedPlayVideoRing(contactInfo, callObjectPtr));
    ASSERT_NO_THROW(CallObjectManager::DeleteOneCallObject(callObjectPtr->GetCallID()));
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_PlayRingtone_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_PlayRingtone_001, Function | MediumTest | Level3)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    ASSERT_FALSE(audioControl->PlayRingtone());
    DialParaInfo info;
    sptr<CallBase> ringingCall = new IMSCall(info);
    AAFwk::WantParams params = ringingCall->GetExtraParams();
    params.SetParam("isNeedMuteRing", AAFwk::Integer::Box(1));
    ringingCall->SetExtraParams(params);
    ringingCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    CallObjectManager::AddOneCallObject(ringingCall);
    ASSERT_TRUE(audioControl->PlayRingtone());
    ContactInfo contactInfo;
    memcpy_s(contactInfo.personalNotificationRingtone, strlen(STR_MP4) + 1, STR_MP4, strlen(STR_MP4));
    ringingCall->SetCallerInfo(contactInfo);
    audioControl->StopRingtone();
    ASSERT_TRUE(audioControl->PlayRingtone());
    ringingCall->SetCrsType(CRS_TYPE);
    ASSERT_FALSE(audioControl->PlayRingtone());
    ASSERT_NO_THROW(CallObjectManager::DeleteOneCallObject(ringingCall->GetCallID()));
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_SwitchIncoming_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_SwitchIncoming_001, Function | MediumTest | Level3)
{
    auto audioSceneProcessor = DelayedSingleton<AudioSceneProcessor>::GetInstance();
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    DialParaInfo mDialParaInfo;
    mDialParaInfo.callType = CallType::TYPE_IMS;
    mDialParaInfo.accountId = 0;
    mDialParaInfo.callId = 0;
    mDialParaInfo.index = 0;
    sptr<OHOS::Telephony::CallBase> crsCallObjectPtr = nullptr;
    crsCallObjectPtr = new IMSCall(mDialParaInfo);
    crsCallObjectPtr->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    CallObjectManager::AddOneCallObject(crsCallObjectPtr);
    ASSERT_TRUE(audioSceneProcessor->SwitchIncoming());
    crsCallObjectPtr->SetCrsType(CRS_TYPE);
    ASSERT_TRUE(audioSceneProcessor->SwitchIncoming());
    mDialParaInfo.callId = 1;
    mDialParaInfo.index = 1;
    sptr<OHOS::Telephony::CallBase> vidioRingCallObjectPtr = nullptr;
    vidioRingCallObjectPtr = new IMSCall(mDialParaInfo);
    vidioRingCallObjectPtr->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    CallObjectManager::AddOneCallObject(vidioRingCallObjectPtr);
    ASSERT_TRUE(audioSceneProcessor->SwitchIncoming());
    audioControl->SetSoundState(SoundState::SOUNDING);
    ASSERT_TRUE(audioSceneProcessor->SwitchIncoming());
    ASSERT_NO_THROW(CallObjectManager::DeleteOneCallObject(crsCallObjectPtr->GetCallID()));
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}
}