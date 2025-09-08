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
#include "alerting_state.h"
#include "audio_proxy.h"
#include "audio_control_manager.h"
#include "bluetooth_device_state.h"
#include "call_ability_callback_stub.h"
#include "call_policy.h"
#include "call_request_process.h"
#include "call_object_manager.h"
#include "call_state_processor.h"
#include "call_status_callback_proxy.h"
#include "cs_call.h"
#include "cs_call_state.h"
#include "dialing_state.h"
#include "earpiece_device_state.h"
#include "holding_state.h"
#include "ims_call.h"
#include "ims_call_state.h"
#include "inactive_device_state.h"
#include "inactive_state.h"
#include "incoming_state.h"
#include "iservice_registry.h"
#include "ott_call_state.h"
#include "ring.h"
#include "speaker_device_state.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"
#include "tone.h"
#include "voip_call_connection.h"
#include "voip_call.h"
#include "wired_headset_device_state.h"
#include "gtest/gtest.h"
#include "audio_scene_processor.h"
#include "core_service_connection.h"
#include "ringtone_player.h"
#include "data_ability_observer_stub.h"
#include "comfort_reminder_data.h"
#include "settings_datashare_helper.h"
#include "datashare_helper.h"
#include "user_status_client.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
constexpr int WAIT_TIME = 3;
constexpr int DEFAULT_SLOT_ID = 0;
constexpr int VALID_CALL_ID = 1;
constexpr const char* NUMBER = "10086";
constexpr const char* NAME = "test";

class CallStateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void CallStateTest::SetUpTestCase() {}

void CallStateTest::TearDownTestCase() {}

void CallStateTest::SetUp() {}

void CallStateTest::TearDown() {}

/**
 * @tc.number   Telephony_CSCallState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_CSCallState_001, TestSize.Level0)
{
    auto csCallState = std::make_shared<CSCallState>();
    csCallState->ProcessEvent(AudioEvent::NO_MORE_ACTIVE_CALL);
    csCallState->ProcessEvent(AudioEvent::NEW_INCOMING_CALL);
    csCallState->ProcessEvent(AudioEvent::CALL_TYPE_CS_CHANGE_IMS);
    ASSERT_FALSE(csCallState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_IMSCallState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_IMSCallState_001, TestSize.Level0)
{
    auto imsCallState = std::make_shared<IMSCallState>();
    imsCallState->ProcessEvent(AudioEvent::NO_MORE_ACTIVE_CALL);
    imsCallState->ProcessEvent(AudioEvent::NEW_INCOMING_CALL);
    imsCallState->ProcessEvent(AudioEvent::CALL_TYPE_IMS_CHANGE_CS);
    ASSERT_FALSE(imsCallState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_OTTCallState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_OTTCallState_001, TestSize.Level0)
{
    auto ottCallState = std::make_shared<OTTCallState>();
    ASSERT_FALSE(ottCallState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_HoldingState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_HoldingState_001, TestSize.Level0)
{
    auto holdingState = std::make_shared<HoldingState>();
    holdingState->ProcessEvent(AudioEvent::NEW_ACTIVE_CS_CALL);
    holdingState->ProcessEvent(AudioEvent::NEW_ACTIVE_IMS_CALL);
    holdingState->ProcessEvent(AudioEvent::NEW_INCOMING_CALL);
    ASSERT_FALSE(holdingState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_AlertingState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_AlertingState_001, TestSize.Level1)
{
    auto alertingState = std::make_shared<AlertingState>();
    alertingState->ProcessEvent(AudioEvent::NO_MORE_ALERTING_CALL);
    alertingState->ProcessEvent(AudioEvent::NEW_ACTIVE_CS_CALL);
    alertingState->ProcessEvent(AudioEvent::NEW_ACTIVE_IMS_CALL);
    ASSERT_FALSE(alertingState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_IncomingState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_IncomingState_001, TestSize.Level0)
{
    auto incomingState = std::make_shared<IncomingState>();
    incomingState->ProcessEvent(AudioEvent::NO_MORE_INCOMING_CALL);
    incomingState->ProcessEvent(AudioEvent::NEW_ACTIVE_CS_CALL);
    incomingState->ProcessEvent(AudioEvent::NEW_ACTIVE_IMS_CALL);
    ASSERT_FALSE(incomingState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_InActiveState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_InActiveState_001, TestSize.Level0)
{
    auto inActiveState = std::make_shared<InActiveState>();
    inActiveState->ProcessEvent(AudioEvent::NEW_DIALING_CALL);
    inActiveState->ProcessEvent(AudioEvent::NEW_ACTIVE_CS_CALL);
    inActiveState->ProcessEvent(AudioEvent::NEW_ACTIVE_IMS_CALL);
    inActiveState->ProcessEvent(AudioEvent::NEW_ALERTING_CALL);
    inActiveState->ProcessEvent(AudioEvent::NEW_INCOMING_CALL);
    ASSERT_FALSE(inActiveState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_DialingState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_DialingState_001, TestSize.Level0)
{
    auto dialingState = std::make_shared<DialingState>();
    dialingState->ProcessEvent(AudioEvent::NO_MORE_DIALING_CALL);
    dialingState->ProcessEvent(AudioEvent::NEW_ACTIVE_CS_CALL);
    dialingState->ProcessEvent(AudioEvent::NEW_ACTIVE_IMS_CALL);
    dialingState->ProcessEvent(AudioEvent::NEW_ALERTING_CALL);
    ASSERT_FALSE(dialingState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_WiredHeadsetDeviceState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_WiredHeadsetDeviceState_001, TestSize.Level0)
{
    auto wiredHeadsetDeviceState = std::make_shared<WiredHeadsetDeviceState>();
    wiredHeadsetDeviceState->ProcessEvent(AudioEvent::WIRED_HEADSET_DISCONNECTED);
    wiredHeadsetDeviceState->ProcessEvent(AudioEvent::BLUETOOTH_SCO_CONNECTED);
    ASSERT_FALSE(wiredHeadsetDeviceState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_BluetoothDeviceState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_BluetoothDeviceState_001, TestSize.Level0)
{
    auto bluetoothDeviceState = std::make_shared<BluetoothDeviceState>();
    bluetoothDeviceState->ProcessEvent(AudioEvent::BLUETOOTH_SCO_CONNECTED);
    bluetoothDeviceState->ProcessEvent(AudioEvent::AUDIO_ACTIVATED);
    bluetoothDeviceState->ProcessEvent(AudioEvent::AUDIO_RINGING);
    bluetoothDeviceState->ProcessEvent(AudioEvent::BLUETOOTH_SCO_DISCONNECTED);
    bluetoothDeviceState->ProcessEvent(AudioEvent::AUDIO_DEACTIVATED);
    ASSERT_FALSE(bluetoothDeviceState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_EarpieceDeviceState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_EarpieceDeviceState_001, TestSize.Level0)
{
    auto earpieceDeviceState = std::make_shared<EarpieceDeviceState>();
    earpieceDeviceState->ProcessEvent(AudioEvent::WIRED_HEADSET_CONNECTED);
    earpieceDeviceState->ProcessEvent(AudioEvent::BLUETOOTH_SCO_CONNECTED);
    ASSERT_FALSE(earpieceDeviceState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_SpeakerDeviceState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_SpeakerDeviceState_001, TestSize.Level0)
{
    auto speakerDeviceState = std::make_shared<SpeakerDeviceState>();
    speakerDeviceState->ProcessEvent(AudioEvent::WIRED_HEADSET_CONNECTED);
    speakerDeviceState->ProcessEvent(AudioEvent::BLUETOOTH_SCO_CONNECTED);
    speakerDeviceState->ProcessEvent(AudioEvent::AUDIO_ACTIVATED);
    ASSERT_FALSE(speakerDeviceState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_InactiveDeviceState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_InactiveDeviceState_001, TestSize.Level0)
{
    auto inactiveDeviceState = std::make_shared<InactiveDeviceState>();
    inactiveDeviceState->ProcessEvent(AudioEvent::AUDIO_ACTIVATED);
    inactiveDeviceState->ProcessEvent(AudioEvent::AUDIO_RINGING);
    inactiveDeviceState->ProcessEvent(AudioEvent::AUDIO_DEACTIVATED);
    ASSERT_FALSE(inactiveDeviceState->ProcessEvent(AudioEvent::UNKNOWN_EVENT));
}

/**
 * @tc.number   Telephony_AudioProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_AudioProxy_001, TestSize.Level0)
{
    auto audioProxy = std::make_shared<AudioProxy>();
    audioProxy->SetAudioDeviceChangeCallback();
    audioProxy->UnsetDeviceChangeCallback();

    audioProxy->SetBluetoothDevActive();
    audioProxy->SetBluetoothDevActive();
    audioProxy->SetSpeakerDevActive(true);
    audioProxy->SetSpeakerDevActive(true);
    audioProxy->SetWiredHeadsetState(true);
    ASSERT_FALSE(audioProxy->SetEarpieceDevActive());
    audioProxy->SetWiredHeadsetState(false);
    audioProxy->SetEarpieceDevActive();
    audioProxy->SetEarpieceDevActive();
    ASSERT_FALSE(audioProxy->SetWiredHeadsetDevActive());
    audioProxy->SetWiredHeadsetState(true);
    audioProxy->SetWiredHeadsetDevActive();
    audioProxy->SetSpeakerDevActive(true);
    audioProxy->SetWiredHeadsetDevActive();
    audioProxy->SetEarpieceDevActive();
    audioProxy->SetWiredHeadsetDevActive();
    audioProxy->SetBluetoothDevActive();
    audioProxy->SetWiredHeadsetDevActive();

    int32_t volume = audioProxy->GetVolume(AudioStandard::AudioStreamType::STREAM_VOICE_CALL);
    audioProxy->SetMaxVolume(AudioStandard::AudioStreamType::STREAM_VOICE_CALL);
    audioProxy->SetVolumeAudible();
    audioProxy->SetVolume(AudioStandard::AudioStreamType::STREAM_VOICE_CALL, volume);
    audioProxy->SetVolumeWithDevice(AudioStandard::AudioStreamType::STREAM_VOICE_CALL, volume,
        AudioStandard::DeviceType::DEVICE_TYPE_SPEAKER);
    audioProxy->IsStreamActive(AudioStandard::AudioStreamType::STREAM_VOICE_CALL);
    audioProxy->IsStreamMute(AudioStandard::AudioStreamType::STREAM_VOICE_CALL);
    audioProxy->GetMaxVolume(AudioStandard::AudioStreamType::STREAM_VOICE_CALL);
    audioProxy->GetMinVolume(AudioStandard::AudioStreamType::STREAM_VOICE_CALL);
    bool isMute = audioProxy->IsMicrophoneMute();
    audioProxy->SetMicrophoneMute(!isMute);
    audioProxy->SetMicrophoneMute(isMute);
    ASSERT_TRUE(audioProxy->SetMicrophoneMute(isMute));
}

/**
 * @tc.number   Telephony_AudioProxy_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_AudioProxy_002, TestSize.Level0)
{
    auto audioProxy = std::make_shared<AudioProxy>();
    audioProxy->GetRingerMode();
    AudioDevice device;
    audioProxy->GetPreferredOutputAudioDevice(device);
    audioProxy->SetAudioPreferDeviceChangeCallback();
    audioProxy->UnsetAudioPreferDeviceChangeCallback();
    ASSERT_FALSE(audioProxy->GetDefaultTonePath().empty());
    ASSERT_FALSE(audioProxy->GetDefaultDtmfPath().empty());
    DialParaInfo info;
    info.voipCallInfo.uid = 1;
    sptr<CallBase> call0 = new CSCall(info);
    call0->SetCallId(0);
    call0->SetCallIndex(0);
    call0->SetSlotId(0);
    call0->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    call0->SetCallType(CallType::TYPE_CS);
    CallObjectManager::AddOneCallObject(call0);
    EXPECT_TRUE(CallObjectManager::GetAudioLiveCall() != nullptr);
    audioProxy->SetDeviceActive(AudioStandard::DeviceType::DEVICE_TYPE_BLUETOOTH_SCO, true);
    call0->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTED);
    sptr<CallBase> call1 = new VoIPCall(info);
    call1->SetCallId(1);
    call1->SetCallType(CallType::TYPE_VOIP);
    call1->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_TRUE(CallObjectManager::GetAudioLiveCall() != nullptr);
    audioProxy->SetDeviceActive(AudioStandard::DeviceType::DEVICE_TYPE_SPEAKER, true);
}

/**
 * @tc.number   Telephony_AudioDeviceManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_AudioDeviceManager_001, TestSize.Level0)
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    audioDeviceManager->AddAudioDeviceList(NAME, AudioDeviceType::DEVICE_SPEAKER, NAME);
    audioDeviceManager->AddAudioDeviceList(NAME, AudioDeviceType::DEVICE_WIRED_HEADSET, NAME);
    audioDeviceManager->AddAudioDeviceList(NAME, AudioDeviceType::DEVICE_BLUETOOTH_SCO, NAME);
    audioDeviceManager->RemoveAudioDeviceList(NAME, AudioDeviceType::DEVICE_WIRED_HEADSET);
    audioDeviceManager->RemoveAudioDeviceList(NAME, AudioDeviceType::DEVICE_BLUETOOTH_SCO);
    audioDeviceManager->ResetBtAudioDevicesList();
    audioDeviceManager->ResetDistributedCallDevicesList();
    audioDeviceManager->ProcessEvent(AudioEvent::WIRED_HEADSET_DISCONNECTED);
    audioDeviceManager->SwitchDevice(AUDIO_DEACTIVATED);
    audioDeviceManager->EnableBtSco();
    audioDeviceManager->GetCurrentAudioDevice();
    audioDeviceManager->IsEarpieceDevEnable();
    audioDeviceManager->SetDeviceAvailable(AudioDeviceType::DEVICE_SPEAKER, false);
    audioDeviceManager->SetDeviceAvailable(AudioDeviceType::DEVICE_EARPIECE, false);
    audioDeviceManager->SetDeviceAvailable(AudioDeviceType::DEVICE_BLUETOOTH_SCO, false);
    audioDeviceManager->SetDeviceAvailable(AudioDeviceType::DEVICE_WIRED_HEADSET, false);
    audioDeviceManager->SetDeviceAvailable(AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE, false);
    audioDeviceManager->SetDeviceAvailable(AudioDeviceType::DEVICE_DISTRIBUTED_PHONE, false);
    audioDeviceManager->SetDeviceAvailable(AudioDeviceType::DEVICE_UNKNOWN, false);
    ASSERT_FALSE(audioDeviceManager->IsWiredHeadsetDevEnable());
}

/**
 * @tc.number   Telephony_AudioDeviceManager_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_AudioDeviceManager_002, TestSize.Level0)
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    audioDeviceManager->AddAudioDeviceList(NAME, AudioDeviceType::DEVICE_SPEAKER, NAME);
    audioDeviceManager->AddAudioDeviceList(NAME, AudioDeviceType::DEVICE_WIRED_HEADSET, NAME);
    audioDeviceManager->AddAudioDeviceList(NAME, AudioDeviceType::DEVICE_BLUETOOTH_SCO, NAME);
    audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_EARPIECE);
    audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_SPEAKER);
    audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_WIRED_HEADSET);
    audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_BLUETOOTH_SCO);
    audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_DISABLE);
    audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_UNKNOWN);
    audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE);
    audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_DISTRIBUTED_PHONE);
    audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_DISTRIBUTED_PAD);
    audioDeviceManager->isWiredHeadsetConnected_ = true;
    audioDeviceManager->SetDeviceAvailable(AudioDeviceType::DEVICE_WIRED_HEADSET, true);
    audioDeviceManager->EnableWiredHeadset();
    audioDeviceManager->SetDeviceAvailable(AudioDeviceType::DEVICE_DISTRIBUTED_PAD, true);
    audioDeviceManager->IsBtScoDevEnable();
    audioDeviceManager->IsDCallDevEnable();
    audioDeviceManager->IsSpeakerDevEnable();
    ASSERT_FALSE(audioDeviceManager->IsSpeakerAvailable());
    ASSERT_FALSE(audioDeviceManager->IsBtActived());
}

/**
 * @tc.number   Telephony_AudioPlayer_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_AudioPlayer_001, TestSize.Level0)
{
    auto audioPlayer = DelayedSingleton<AudioPlayer>::GetInstance();
    std::string profilePath = "";
    std::string path = "";
    audioPlayer->GetRealPath(profilePath, path);
    ASSERT_FALSE(audioPlayer->IsStop(TYPE_RING));
    ASSERT_FALSE(audioPlayer->IsStop(TYPE_TONE));
    ASSERT_FALSE(audioPlayer->IsStop(TYPE_DTMF));
    ASSERT_FALSE(audioPlayer->IsStop(TYPE_SOUND));
}

/**
 * @tc.number   Telephony_AudioSceneProcessor_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_AudioSceneProcessor_001, TestSize.Level0)
{
    auto audioSceneProcessor = DelayedSingleton<AudioSceneProcessor>::GetInstance();
    audioSceneProcessor->SwitchState(CallStateType::DIALING_STATE);
    audioSceneProcessor->SwitchState(CallStateType::ALERTING_STATE);
    audioSceneProcessor->SwitchState(CallStateType::INCOMING_STATE);
    audioSceneProcessor->SwitchState(CallStateType::CS_CALL_STATE);
    audioSceneProcessor->SwitchState(CallStateType::IMS_CALL_STATE);
    audioSceneProcessor->SwitchState(CallStateType::HOLDING_STATE);
    audioSceneProcessor->SwitchState(CallStateType::UNKNOWN_STATE);
    audioSceneProcessor->SwitchState(CallStateType::INACTIVE_STATE);
    ASSERT_TRUE(audioSceneProcessor->SwitchIncoming());
}

/**
 * @tc.number   Telephony_Ring_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_Ring_001, TestSize.Level0)
{
    auto ring = std::make_shared<Ring>();
    ring->Play(DEFAULT_SLOT_ID, "", Media::HapticStartupMode::DEFAULT);
    sleep(WAIT_TIME);
    ring->Stop();
    ring->ReleaseRenderer();
    ring->Play(DEFAULT_SLOT_ID, "", Media::HapticStartupMode::DEFAULT);
    sleep(WAIT_TIME);
    ASSERT_NE(ring->Stop(), TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_Ring_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_Ring_002, TestSize.Level0)
{
    #ifdef OHOS_SUBSCRIBE_USER_STATUS_ENABLE
    auto ring = std::make_shared<Ring>();
    auto comfortReminder = std::make_shared<Msdp::UserStatusAwareness::ComfortReminderData>();
    comfortReminder->SetFusionReminderData(0);
    comfortReminder->SetEventType(1);
    ring->isAdaptiveSwitchOn_ = true;
    ring->isEnvMsgRecv_ = true;
    ring->isQuiet_ = true;
    ffrt::submit([=]() {
        ffrt_usleep(100000);
        ring->OnComfortReminderDataChanged(0, comfortReminder);
    });
    ring->Play(DEFAULT_SLOT_ID, "", Media::HapticStartupMode::DEFAULT);
    sleep(WAIT_TIME);
    comfortReminder->SetFusionReminderData(0);
    comfortReminder->SetEventType(0);
    ring->isSwingMsgRecv_ = true;
    ring->isSwing_ = true;
    ring->OnComfortReminderDataChanged(0, comfortReminder);
    sleep(WAIT_TIME);
    ASSERT_NE(ring->Stop(), TELEPHONY_SUCCESS);
    ring->ReleaseRenderer();
    #endif
}

/**
 * @tc.number   Telephony_Tone_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_Tone_001, TestSize.Level0)
{
    auto unknownTone = std::make_shared<Tone>(ToneDescriptor::TONE_UNKNOWN);
    unknownTone->Init();
    unknownTone->Play();
    sleep(WAIT_TIME);
    unknownTone->Stop();
    unknownTone->ReleaseRenderer();

    auto ringbackTone = std::make_shared<Tone>(ToneDescriptor::TONE_RINGBACK);
    ringbackTone->Play();
    sleep(WAIT_TIME);
    ringbackTone->Stop();

    auto tone = std::make_shared<Tone>(ToneDescriptor::TONE_DTMF_CHAR_1);
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_0));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_1));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_2));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_3));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_4));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_5));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_6));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_7));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_8));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_9));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_P));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_DTMF_CHAR_W));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_RINGBACK));
    ASSERT_TRUE(tone->IsUseTonePlayer(ToneDescriptor::TONE_WAITING));
}

/**
 * @tc.number   Telephony_Tone_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_Tone_002, TestSize.Level0)
{
    ASSERT_EQ(Tone::ConvertDigitToTone('0'), ToneDescriptor::TONE_DTMF_CHAR_0);
    ASSERT_EQ(Tone::ConvertDigitToTone('1'), ToneDescriptor::TONE_DTMF_CHAR_1);
    ASSERT_EQ(Tone::ConvertDigitToTone('2'), ToneDescriptor::TONE_DTMF_CHAR_2);
    ASSERT_EQ(Tone::ConvertDigitToTone('3'), ToneDescriptor::TONE_DTMF_CHAR_3);
    ASSERT_EQ(Tone::ConvertDigitToTone('4'), ToneDescriptor::TONE_DTMF_CHAR_4);
    ASSERT_EQ(Tone::ConvertDigitToTone('5'), ToneDescriptor::TONE_DTMF_CHAR_5);
    ASSERT_EQ(Tone::ConvertDigitToTone('6'), ToneDescriptor::TONE_DTMF_CHAR_6);
    ASSERT_EQ(Tone::ConvertDigitToTone('7'), ToneDescriptor::TONE_DTMF_CHAR_7);
    ASSERT_EQ(Tone::ConvertDigitToTone('8'), ToneDescriptor::TONE_DTMF_CHAR_8);
    ASSERT_EQ(Tone::ConvertDigitToTone('9'), ToneDescriptor::TONE_DTMF_CHAR_9);
    ASSERT_EQ(Tone::ConvertDigitToTone('*'), ToneDescriptor::TONE_DTMF_CHAR_P);
    ASSERT_EQ(Tone::ConvertDigitToTone('#'), ToneDescriptor::TONE_DTMF_CHAR_W);
    ASSERT_EQ(Tone::ConvertDigitToTone('a'), ToneDescriptor::TONE_UNKNOWN);
}

/**
 * @tc.number   Telephony_Tone_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_Tone_003, TestSize.Level0)
{
    auto tone = std::make_shared<Tone>();
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_0),
        AudioStandard::ToneType::TONE_TYPE_DIAL_0);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_1),
        AudioStandard::ToneType::TONE_TYPE_DIAL_1);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_2),
        AudioStandard::ToneType::TONE_TYPE_DIAL_2);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_3),
        AudioStandard::ToneType::TONE_TYPE_DIAL_3);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_4),
        AudioStandard::ToneType::TONE_TYPE_DIAL_4);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_5),
        AudioStandard::ToneType::TONE_TYPE_DIAL_5);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_6),
        AudioStandard::ToneType::TONE_TYPE_DIAL_6);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_7),
        AudioStandard::ToneType::TONE_TYPE_DIAL_7);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_8),
        AudioStandard::ToneType::TONE_TYPE_DIAL_8);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_9),
        AudioStandard::ToneType::TONE_TYPE_DIAL_9);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_P),
        AudioStandard::ToneType::TONE_TYPE_DIAL_S);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_DTMF_CHAR_W),
        AudioStandard::ToneType::TONE_TYPE_DIAL_P);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_RINGBACK),
        AudioStandard::ToneType::TONE_TYPE_COMMON_SUPERVISORY_RINGTONE);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_WAITING),
        AudioStandard::ToneType::TONE_TYPE_COMMON_SUPERVISORY_CALL_WAITING);
    ASSERT_EQ(tone->ConvertToneDescriptorToToneType(ToneDescriptor::TONE_UNKNOWN), AudioStandard::ToneType::NUM_TONES);
}

/**
 * @tc.number   Telephony_Tone_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_Tone_004, TestSize.Level0)
{
    auto tone = std::make_shared<Tone>();
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_0),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_1),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_2),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_3),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_4),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_5),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_6),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_7),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_8),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_9),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_P),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_DTMF_CHAR_W),
        AudioStandard::StreamUsage::STREAM_USAGE_DTMF);
    ASSERT_EQ(tone->GetStreamUsageByToneType(ToneDescriptor::TONE_RINGBACK),
        AudioStandard::StreamUsage::STREAM_USAGE_VOICE_MODEM_COMMUNICATION);
    tone->GetStreamUsageByToneType(ToneDescriptor::TONE_WAITING);
}

/**
 * @tc.number   Telephony_CallStatusCallbackProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_CallStatusCallbackProxy_001, TestSize.Level0)
{
    auto callStatusCallbackProxy = std::make_shared<CallStatusCallbackProxy>(nullptr);
    CallReportInfo callReportInfo;
    callStatusCallbackProxy->UpdateCallReportInfo(callReportInfo);
    CallsReportInfo callsReportInfo;
    callStatusCallbackProxy->UpdateCallsReportInfo(callsReportInfo);
    DisconnectedDetails details;
    callStatusCallbackProxy->UpdateDisconnectedCause(details);
    CellularCallEventInfo eventInfo;
    callStatusCallbackProxy->UpdateEventResultInfo(eventInfo);
    RBTPlayInfo playInfo = RBTPlayInfo::LOCAL_ALERTING;
    callStatusCallbackProxy->UpdateRBTPlayInfo(playInfo);
    CallWaitResponse callWaitResponse;
    callStatusCallbackProxy->UpdateGetWaitingResult(callWaitResponse);
    int32_t result = 0;
    callStatusCallbackProxy->UpdateSetWaitingResult(result);
    CallRestrictionResponse callRestrictionResult;
    callStatusCallbackProxy->UpdateGetRestrictionResult(callRestrictionResult);
    callStatusCallbackProxy->UpdateSetRestrictionResult(result);
    callStatusCallbackProxy->UpdateSetRestrictionPasswordResult(result);
    CallTransferResponse callTransferResponse;
    callStatusCallbackProxy->UpdateGetTransferResult(callTransferResponse);
    callStatusCallbackProxy->UpdateSetTransferResult(result);
    ClipResponse clipResponse;
    callStatusCallbackProxy->UpdateGetCallClipResult(clipResponse);
    ClirResponse clirResponse;
    callStatusCallbackProxy->UpdateGetCallClirResult(clirResponse);
    callStatusCallbackProxy->UpdateSetCallClirResult(result);
    callStatusCallbackProxy->StartRttResult(result);
    callStatusCallbackProxy->StopRttResult(result);
    GetImsConfigResponse imsConfigResponse;
    callStatusCallbackProxy->GetImsConfigResult(imsConfigResponse);
    callStatusCallbackProxy->SetImsConfigResult(result);
    GetImsFeatureValueResponse imsFeatureValueResponse;
    callStatusCallbackProxy->GetImsFeatureValueResult(imsFeatureValueResponse);
    callStatusCallbackProxy->SetImsFeatureValueResult(result);
    callStatusCallbackProxy->InviteToConferenceResult(result);
    callStatusCallbackProxy->StartDtmfResult(result);
    callStatusCallbackProxy->StopDtmfResult(result);
    callStatusCallbackProxy->SendUssdResult(result);
    MmiCodeInfo mmiCodeInfo;
    ASSERT_EQ(callStatusCallbackProxy->SendMmiCodeResult(mmiCodeInfo), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    ASSERT_EQ(callStatusCallbackProxy->GetImsCallDataResult(result), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    ASSERT_EQ(callStatusCallbackProxy->CloseUnFinishedUssdResult(result), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_CallStatusCallbackProxy_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_CallStatusCallbackProxy_002, TestSize.Level0)
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        TELEPHONY_LOGE("Telephony_CallStatusCallbackProxy systemAbilityMgr is nullptr");
        return;
    }
    sptr<IRemoteObject> remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (remote == nullptr) {
        TELEPHONY_LOGE("Telephony_CallStatusCallbackProxy remote is nullptr");
        return;
    }
    auto callStatusCallbackProxy = std::make_shared<CallStatusCallbackProxy>(remote);

    CallReportInfo callReportInfo;
    callStatusCallbackProxy->UpdateCallReportInfo(callReportInfo);
    CallsReportInfo callsReportInfo;
    callStatusCallbackProxy->UpdateCallsReportInfo(callsReportInfo);
    DisconnectedDetails details;
    callStatusCallbackProxy->UpdateDisconnectedCause(details);
    CellularCallEventInfo eventInfo;
    callStatusCallbackProxy->UpdateEventResultInfo(eventInfo);
    RBTPlayInfo playInfo = RBTPlayInfo::LOCAL_ALERTING;
    callStatusCallbackProxy->UpdateRBTPlayInfo(playInfo);
    CallWaitResponse callWaitResponse;
    callStatusCallbackProxy->UpdateGetWaitingResult(callWaitResponse);
    CallRestrictionResponse callRestrictionResult;
    callStatusCallbackProxy->UpdateGetRestrictionResult(callRestrictionResult);
    CallTransferResponse callTransferResponse;
    callStatusCallbackProxy->UpdateGetTransferResult(callTransferResponse);
    ClipResponse clipResponse;
    ASSERT_EQ(callStatusCallbackProxy->UpdateGetCallClipResult(clipResponse), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    ClirResponse clirResponse;
    ASSERT_EQ(callStatusCallbackProxy->UpdateGetCallClirResult(clirResponse), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_CallStatusCallbackProxy_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_CallStatusCallbackProxy_003, TestSize.Level0)
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        TELEPHONY_LOGE("Telephony_CallStatusCallbackProxy systemAbilityMgr is nullptr");
        return;
    }
    sptr<IRemoteObject> remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (remote == nullptr) {
        TELEPHONY_LOGE("Telephony_CallStatusCallbackProxy remote is nullptr");
        return;
    }
    auto callStatusCallbackProxy = std::make_shared<CallStatusCallbackProxy>(remote);

    int32_t result = 0;
    callStatusCallbackProxy->UpdateSetWaitingResult(result);
    callStatusCallbackProxy->UpdateSetRestrictionResult(result);
    callStatusCallbackProxy->UpdateSetRestrictionPasswordResult(result);
    callStatusCallbackProxy->UpdateSetTransferResult(result);
    callStatusCallbackProxy->UpdateSetCallClirResult(result);
    callStatusCallbackProxy->StartRttResult(result);
    callStatusCallbackProxy->StopRttResult(result);
    GetImsConfigResponse imsConfigResponse;
    callStatusCallbackProxy->GetImsConfigResult(imsConfigResponse);
    callStatusCallbackProxy->SetImsConfigResult(result);
    GetImsFeatureValueResponse imsFeatureValueResponse;
    callStatusCallbackProxy->GetImsFeatureValueResult(imsFeatureValueResponse);
    callStatusCallbackProxy->SetImsFeatureValueResult(result);
    callStatusCallbackProxy->InviteToConferenceResult(result);
    callStatusCallbackProxy->StartDtmfResult(result);
    callStatusCallbackProxy->StopDtmfResult(result);
    callStatusCallbackProxy->SendUssdResult(result);
    MmiCodeInfo mmiCodeInfo;
    CallModeReportInfo callModeRequestInfo;
    callStatusCallbackProxy->ReceiveUpdateCallMediaModeRequest(callModeRequestInfo);
    CallModeReportInfo callModeResponseInfo;
    callStatusCallbackProxy->ReceiveUpdateCallMediaModeResponse(callModeResponseInfo);
    CallSessionReportInfo sessionReportInfo;
    callStatusCallbackProxy->HandleCallSessionEventChanged(sessionReportInfo);
    PeerDimensionsReportInfo peerDimensionsReportInfo;
    callStatusCallbackProxy->HandlePeerDimensionsChanged(peerDimensionsReportInfo);
    callStatusCallbackProxy->HandleCallDataUsageChanged(static_cast<int64_t>(result));
    CameraCapabilitiesReportInfo cameraCapabilitiesInfo;
    callStatusCallbackProxy->HandleCameraCapabilitiesChanged(cameraCapabilitiesInfo);
    ASSERT_EQ(callStatusCallbackProxy->SendMmiCodeResult(mmiCodeInfo), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    ASSERT_EQ(callStatusCallbackProxy->GetImsCallDataResult(result), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    ASSERT_EQ(callStatusCallbackProxy->CloseUnFinishedUssdResult(result), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_CallRequestProcess_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_CallRequestProcess_002, TestSize.Level0)
{
    std::unique_ptr<CallRequestProcess> callRequestProcess = std::make_unique<CallRequestProcess>();
    DialParaInfo mDialParaInfo;
    mDialParaInfo.accountId = 0;
    sptr<OHOS::Telephony::CallBase> callBase1 = new IMSCall(mDialParaInfo);
    callBase1->IsMuted();
    callBase1->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    callBase1->callId_ = 1;
    mDialParaInfo.accountId = 1;
    sptr<OHOS::Telephony::CallBase> callBase2 = new IMSCall(mDialParaInfo);
    callBase2->callState_ = TelCallState::CALL_STATUS_INCOMING;
    callBase2->callId_ = 2;
    bool enabled = false;
    callRequestProcess->HandleCallWaitingNumZero(callBase2, callBase1, 1, 1, enabled);
    mDialParaInfo.accountId = 0;
    sptr<OHOS::Telephony::CallBase> callBase5 = new IMSCall(mDialParaInfo);
    callBase5->callState_ = TelCallState::CALL_STATUS_HOLDING;
    callBase5->callId_ = 1;
    mDialParaInfo.accountId = 1;
    sptr<OHOS::Telephony::CallBase> callBase6 = new IMSCall(mDialParaInfo);
    callBase6->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    callBase6->callId_ = 2;
    sptr<OHOS::Telephony::CallBase> callBase7 = new IMSCall(mDialParaInfo);
    callBase7->callState_ = TelCallState::CALL_STATUS_INCOMING;
    callBase7->callId_ = 3;
    bool enabled1 = false;
    callRequestProcess->HandleCallWaitingNumOne(callBase7, callBase5, 1, 1, enabled1);
    mDialParaInfo.accountId = 0;
    sptr<OHOS::Telephony::CallBase> callBase8 = new IMSCall(mDialParaInfo);
    callBase8->callState_ = TelCallState::CALL_STATUS_HOLDING;
    callBase8->callId_ = 1;
    sptr<OHOS::Telephony::CallBase> callBase9 = new IMSCall(mDialParaInfo);
    callBase9->callState_ = TelCallState::CALL_STATUS_WAITING;
    callBase9->callId_ = 2;
    mDialParaInfo.accountId = 1;
    sptr<OHOS::Telephony::CallBase> callBase10 = new IMSCall(mDialParaInfo);
    callBase10->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    callBase10->callId_ = 3;
    sptr<OHOS::Telephony::CallBase> callBase11 = new IMSCall(mDialParaInfo);
    callBase11->callState_ = TelCallState::CALL_STATUS_WAITING;
    callBase11->callId_ = 4;
    sptr<OHOS::Telephony::CallBase> callBase12 = new IMSCall(mDialParaInfo);
    callBase12->callState_ = TelCallState::CALL_STATUS_INCOMING;
    callBase12->callId_ = 5;
    bool enabled2 = false;
    callRequestProcess->HandleCallWaitingNumTwo(callBase12, callBase8, 1, 1, enabled2);
    ASSERT_NE(callBase12, nullptr);
}

/**
 * @tc.number   Telephony_CallRequestProcess_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_CallRequestProcess_003, TestSize.Level0)
{
    int32_t defaultCallId = 1;
    std::unique_ptr<CallRequestProcess> callRequestProcess = std::make_unique<CallRequestProcess>();
    DialParaInfo mDialParaInfo;
    mDialParaInfo.accountId = 0;
    sptr<OHOS::Telephony::CallBase> callBase1 = new IMSCall(mDialParaInfo);
    callBase1->callState_ = TelCallState::CALL_STATUS_HOLDING;
    callBase1->callId_ = 1;
    callBase1->callRunningState_ = CallRunningState::CALL_RUNNING_STATE_HOLD;
    callBase1->videoState_ = VideoStateType::TYPE_SEND_ONLY;
    mDialParaInfo.accountId = 1;
    sptr<OHOS::Telephony::CallBase> callBase2 = new IMSCall(mDialParaInfo);
    callBase2->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    callBase2->callId_ = 2;
    callBase2->callRunningState_ = CallRunningState::CALL_RUNNING_STATE_ACTIVE;
    callBase2->videoState_ = VideoStateType::TYPE_SEND_ONLY;
    sptr<OHOS::Telephony::CallBase> callBase3 = new IMSCall(mDialParaInfo);
    callBase3->callState_ = TelCallState::CALL_STATUS_INCOMING;
    callBase3->callId_ = 3;
    callRequestProcess->AddOneCallObject(callBase1);
    callRequestProcess->AddOneCallObject(callBase2);
    callRequestProcess->AddOneCallObject(callBase3);
    bool enabled = false;
    callRequestProcess->HandleCallWaitingNumOneNext(callBase3, callBase2, callBase1, 1, enabled);
    ASSERT_FALSE(callRequestProcess->HasConnectingCall(false));
    ASSERT_TRUE(callRequestProcess->HasActivedCall(false));
    ASSERT_TRUE(callRequestProcess->NeedAnswerVTAndEndActiveVO(3, 1));
    ASSERT_TRUE(callRequestProcess->NeedAnswerVOAndEndActiveVT(defaultCallId, 0));
}

/**
 * @tc.number   Telephony_CallRequestProcess_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_CallRequestProcess_004, TestSize.Level0)
{
    int32_t defaultCallId = 1;
    std::unique_ptr<CallRequestProcess> callRequestProcess = std::make_unique<CallRequestProcess>();
    DialParaInfo mDialParaInfo;
    mDialParaInfo.accountId = 0;
    sptr<OHOS::Telephony::CallBase> call1 = new IMSCall(mDialParaInfo);
    sptr<OHOS::Telephony::CallBase> call2 = new IMSCall(mDialParaInfo);
    sptr<OHOS::Telephony::CallBase> call3 = new IMSCall(mDialParaInfo);
    callRequestProcess->AddOneCallObject(call1);
    callRequestProcess->AddOneCallObject(call2);
    callRequestProcess->AddOneCallObject(call3);
    sptr<OHOS::Telephony::CallBase> callBase = new IMSCall(mDialParaInfo);
    callRequestProcess->AnswerRequestForDsda(callBase, 1, 0);
    sptr<OHOS::Telephony::CallBase> callBase1 = new IMSCall(mDialParaInfo);
    sptr<OHOS::Telephony::CallBase> incomingCall = new IMSCall(mDialParaInfo);
    callRequestProcess->HandleDsdaIncomingCall(callBase1, 0, DEFAULT_SLOT_ID, 0, incomingCall);
    sptr<OHOS::Telephony::CallBase> callBase2 = new IMSCall(mDialParaInfo);
    callRequestProcess->HangUpForDsdaRequest(callBase2);
    CellularCallInfo callInfo;
    callRequestProcess->HandleStartDial(false, mDialParaInfo);
    ASSERT_EQ(callRequestProcess->EccDialPolicy(), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_VoipCallConnection_001
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_VoipCallConnection_001, TestSize.Level0)
{
    std::shared_ptr<VoipCallConnection> voipCallConnection = std::make_shared<VoipCallConnection>();
    int32_t systemAbilityId = 1;
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IVoipCallManagerService> voipCallManagerInterfacePtr = nullptr;
    if (managerPtr != nullptr) {
        sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
        if (iRemoteObjectPtr != nullptr) {
            voipCallManagerInterfacePtr = iface_cast<IVoipCallManagerService>(iRemoteObjectPtr);
        }
    }
    voipCallConnection->Init(systemAbilityId);
    voipCallConnection->UnInit();
    voipCallConnection->GetCallManagerProxy();
    VoipCallEventInfo voipCallEventInfo;
    voipCallEventInfo.voipCallId = "123";
    voipCallConnection->AnswerCall(voipCallEventInfo, static_cast<int32_t>(VideoStateType::TYPE_VOICE));
    voipCallConnection->HangUpCall(voipCallEventInfo);
    voipCallConnection->RejectCall(voipCallEventInfo);
    sptr<ICallStatusCallback> callStatusCallback = nullptr;
    voipCallConnection->RegisterCallManagerCallBack(callStatusCallback);
    voipCallConnection->ClearVoipCall();
    ASSERT_NE(voipCallConnection->UnRegisterCallManagerCallBack(), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStateProcessor_001
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_CallStateProcessor_001, TestSize.Level0)
{
    auto callStateProcessor = DelayedSingleton<CallStateProcessor>::GetInstance();
    TelCallState state = TelCallState::CALL_STATUS_ACTIVE;
    callStateProcessor->AddCall(1, state);
    bool result = callStateProcessor->UpdateCurrentCallState();
    callStateProcessor->ShouldStopSoundtone();
    callStateProcessor->GetCurrentActiveCall();
    callStateProcessor->GetCallNumber(state);
    callStateProcessor->ShouldSwitchState(state);
    callStateProcessor->GetAudioForegroundLiveCall();
    ASSERT_EQ(result, false);
}

/**
 * @tc.number   Telephony_CoreServiceConnection_001
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallStateTest, Telephony_CoreServiceConnection_001, TestSize.Level0)
{
    CoreServiceConnection coreServiceConnection;
    coreServiceConnection.GetFdnNumberList(DEFAULT_SLOT_ID);
    bool res = coreServiceConnection.IsFdnEnabled(DEFAULT_SLOT_ID);
    ASSERT_FALSE(res);
}

} // namespace Telephony
} // namespace OHOS
