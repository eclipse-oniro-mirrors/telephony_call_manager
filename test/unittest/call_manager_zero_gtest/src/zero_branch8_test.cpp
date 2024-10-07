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
#include "bluetooth_call_client.h"
#include "bluetooth_call_manager.h"
#include "bluetooth_call_service.h"
#include "bluetooth_connection.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "gtest/gtest.h"
#include "ims_call.h"
#include "voip_call.h"
#include "audio_control_manager.h"
#include "call_state_processor.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
constexpr int WAIT_TIME = 3;
constexpr int DEFAULT_SLOT_ID = 0;
constexpr int VALID_CALL_ID = 1;
constexpr const char* NUMBER = "10086";
constexpr const char* NAME = "test";

class ZeroBranch7Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void ZeroBranch7Test::SetUpTestCase() {}

void ZeroBranch7Test::TearDownTestCase() {}

void ZeroBranch7Test::SetUp() {
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioMicStateChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioDeviceChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioPreferDeviceChangeCallback();
}

void ZeroBranch7Test::TearDown() {
    sleep(1);
}

/**
 * @tc.number   Telephony_AudioControlManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_AudioControlManager_001, Function | MediumTest | Level3)
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
    audioControl->MuteNetWorkRingTone();
    audioControl->IsBtOrWireHeadPlugin();
    ASSERT_TRUE(audioControl->IsVideoCall(VideoStateType::TYPE_RECEIVE_ONLY));
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
}

/**
 * @tc.number   Telephony_AudioControlManager_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_AudioControlManager_002, Function | MediumTest | Level3)
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
    audioControl->PlayRingtone();
    audioControl->PlaySoundtone();
    audioControl->StopRingtone();
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
    ASSERT_TRUE(audioControl->IsAudioActivated());
}

/**
 * @tc.number   Telephony_AudioControlManager_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_AudioControlManager_003, Function | MediumTest | Level3)
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
}

/**
 * @tc.number   Telephony_AudioControlManager_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_AudioControlManager_004, Function | MediumTest | Level3)
{
    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    audioControl->isCrsVibrating_ = true;
    audioControl->MuteNetWorkRingTone();
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
}

/**
 * @tc.number   Telephony_CallBroadcastSubscriber_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallBroadcastSubscriber_001, Function | MediumTest | Level3)
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SIM_STATE_CHANGED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    CallBroadcastSubscriber subscriber(subscriberInfo);
    EventFwk::CommonEventData eventData;
    subscriber.OnReceiveEvent(eventData);
    subscriber.UnknownBroadcast(eventData);
    subscriber.SimStateBroadcast(eventData);
    subscriber.ConnectCallUiServiceBroadcast(eventData);
    subscriber.HighTempLevelChangedBroadcast(eventData);
    subscriber.ConnectCallUiSuperPrivacyModeBroadcast(eventData);
    ASSERT_NE(sizeof(eventData), 0);
}

/**
 * @tc.number   Telephony_CallStatusCallbackStub_008
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallStatusCallbackStub_001, Function | MediumTest | Level3)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel dataParcel;
    dataParcel.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    dataParcel.WriteInt32(0);
    dataParcel.WriteString("hello");
    MessageParcel reply;
    ASSERT_EQ(callStatusCallback->OnUpdateDisconnectedCause(dataParcel, reply), TELEPHONY_SUCCESS);
    MessageParcel dataParcel2;
    dataParcel2.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    dataParcel2.WriteInt32(0);
    callStatusCallback->OnUpdateRBTPlayInfo(dataParcel2, reply);
    callStatusCallback->OnUpdateSetWaitingResult(dataParcel2, reply);
    callStatusCallback->OnUpdateSetRestrictionResult(dataParcel2, reply);
    callStatusCallback->OnUpdateSetRestrictionPasswordResult(dataParcel2, reply);
    callStatusCallback->OnUpdateSetTransferResult(dataParcel2, reply);
    callStatusCallback->OnUpdateSetCallClirResult(dataParcel2, reply);
    callStatusCallback->OnStartRttResult(dataParcel2, reply);
    callStatusCallback->OnStopRttResult(dataParcel2, reply);
    callStatusCallback->OnSetImsConfigResult(dataParcel2, reply);
    callStatusCallback->OnSetImsFeatureValueResult(dataParcel2, reply);
    callStatusCallback->OnInviteToConferenceResult(dataParcel2, reply);
    callStatusCallback->OnStartDtmfResult(dataParcel2, reply);
    callStatusCallback->OnStopDtmfResult(dataParcel2, reply);
    callStatusCallback->OnSendUssdResult(dataParcel2, reply);
    callStatusCallback->OnGetImsCallDataResult(dataParcel2, reply);
    ASSERT_EQ(callStatusCallback->OnCloseUnFinishedUssdResult(dataParcel2, reply), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStatusCallbackStub_009
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallStatusCallbackStub_002, Function | MediumTest | Level3)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel dataParce3;
    dataParce3.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    int32_t length = sizeof(ClirResponse);
    dataParce3.WriteInt32(length);
    ClirResponse clirResponse;
    dataParce3.WriteRawData((const void *)&clirResponse, length);
    dataParce3.RewindRead(0);
    MessageParcel reply;
    callStatusCallback->OnUpdateGetCallClirResult(dataParce3, reply);

    MessageParcel dataParce4;
    dataParce4.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    length = sizeof(CallModeReportInfo);
    dataParce4.WriteInt32(length);
    CallModeReportInfo callModeReportInfo;
    dataParce4.WriteRawData((const void *)&callModeReportInfo, length);
    dataParce4.RewindRead(0);
    callStatusCallback->OnReceiveImsCallModeResponse(dataParce4, reply);
    MessageParcel dataParce5;
    dataParce5.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    dataParce5.WriteString("a");
    callStatusCallback->OnPostDialNextChar(dataParce5, reply);
    callStatusCallback->OnReportPostDialDelay(dataParce5, reply);

    MessageParcel dataParce6;
    dataParce6.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    dataParce6.WriteString("123");
    dataParce6.WriteString("abc");
    dataParce6.WriteInt32(0);
    dataParce6.WriteInt32(0);
    ASSERT_EQ(callStatusCallback->OnUpdateVoipEventInfo(dataParce6, reply), TELEPHONY_SUCCESS);
}
} // namespace Telephony
} // namespace OHOS
