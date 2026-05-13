/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "audiodevice_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "audio_control_manager.h"
#include "cs_call.h"
#include "fuzzer/FuzzedDataProvider.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t BOOL_NUM = 2;
constexpr int32_t CALL_TYPE_NUM = 4;
constexpr int32_t RING_STATE_NUM = 2;
constexpr int32_t DIAL_TYPE = 3;
constexpr int32_t VIDIO_TYPE_NUM = 2;
constexpr int32_t TEL_CALL_STATE_NUM = 9;
constexpr int32_t AUDIO_DEVICE_NUM = 6;
constexpr int32_t AUDIO_EVENT = 30;
constexpr int32_t AUDIO_SCENE_NUM = 4;
constexpr int32_t AUDIO_VOLUME_TYPE_NUM = 13;

void AudioControlManagerFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<AudioControlManager> audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(
        provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    paraInfo.callType = static_cast<CallType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    paraInfo.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    paraInfo.callState = static_cast<TelCallState>(provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    std::string message = provider.ConsumeRadomLengthString();
    DisconnectedDetails details;
    bool isMute = provider.ConsumeBool();
    RingState ringState = static_cast<RingState>(provider.ConsumeIntegralInRange<int32_t>(0, RING_STATE_NUM));
    AudioDevice audioDevice = {
        .deviceType = static_cast<AudioDeviceType>(provider.ConsumeIntegralInRange<int32_t>(0, AUDIO_DEVICE_NUM)),
        .address = { 0 },
    };

    audioControlManager->SetAudioDevice(audioDevice);
    audioControlManager->CallDestroyed(details);
    audioControlManager->NewCallCreated(callObjectPtr);
    audioControlManager->IncomingCallActivated(callObjectPtr);
    audioControlManager->IncomingCallHungUp(callObjectPtr, true, message);
    audioControlManager->HandleNewActiveCall(callObjectPtr);
    audioControlManager->StopRingtone();
    audioControlManager->GetInitAudioDeviceType();
    audioControlManager->SetMute(isMute);
    audioControlManager->MuteRinger();
    audioControlManager->GetCallList();
    audioControlManager->GetCurrentActiveCall();
    audioControlManager->IsEmergencyCallExists();
    audioControlManager->GetAudioInterruptState();
    audioControlManager->SetVolumeAudible();
    audioControlManager->SetRingState(ringState);
    audioControlManager->SetLocalRingbackNeeded(isMute);
    audioControlManager->IsAudioActivated();
    audioControlManager->StopCallTone();
    audioControlManager->IsCurrentRinging();
    audioControlManager->StopRingback();
    audioControlManager->StopWaitingTone();
}

void AudioDeviceManagerFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<AudioDeviceManager> audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    AudioDeviceType deviceType = static_cast<AudioDeviceType>(
        provider.ConsumeIntegralInRange<int32_t>(0, AUDIO_DEVICE_NUM));
    AudioEvent event = static_cast<AudioEvent>(provider.ConsumeIntegralInRange<int32_t>(0, AUDIO_EVENT));

    audioDeviceManager->Init();
    audioDeviceManager->InitAudioDevice();
    audioDeviceManager->ProcessEvent(event);
    audioDeviceManager->SwitchDevice(deviceType);
    audioDeviceManager->EnableBtSco();
    audioDeviceManager->EnableWiredHeadset(false);
    audioDeviceManager->EnableSpeaker(false);
    audioDeviceManager->EnableEarpiece(false);
    audioDeviceManager->DisableAll();
    audioDeviceManager->IsBtScoDevEnable();
    audioDeviceManager->IsSpeakerDevEnable();
    audioDeviceManager->IsEarpieceDevEnable();
    audioDeviceManager->IsWiredHeadsetDevEnable();
    audioDeviceManager->GetCurrentAudioDevice();
    audioDeviceManager->IsEarpieceAvailable();
    audioDeviceManager->IsSpeakerAvailable();
    audioDeviceManager->IsBtScoConnected();
    audioDeviceManager->IsBtActived();
    audioDeviceManager->IsWiredHeadsetConnected();
    audioDeviceManager->SetCurrentAudioDevice(deviceType);
}

void AudioProxyFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<AudioProxy> audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    std::shared_ptr<AudioControlManager> audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    AudioStandard::AudioScene audioScene = static_cast<AudioStandard::AudioScene>(
        provider.ConsumeIntegralInRange<int32_t>(0, AUDIO_SCENE_NUM));
    AudioStandard::AudioVolumeType audioVolumeType = static_cast<AudioStandard::AudioVolumeType>(
        provider.ConsumeIntegralInRange<int32_t>(0, AUDIO_VOLUME_TYPE_NUM));
    int32_t volume = provider.ConsumeIntegral<int32_t>();
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    bool isMute = provider.ConsumeBool();

    audioControlManager->GetCallBase(callId);
    audioProxy->SetVolumeAudible();
    audioProxy->IsMicrophoneMute();
    audioProxy->SetMicrophoneMute(isMute);
    audioProxy->SetEarpieceDevActive();
    audioProxy->SetSpeakerDevActive(true);
    audioProxy->SetBluetoothDevActive();
    audioProxy->SetWiredHeadsetDevActive();
    audioProxy->GetRingerMode();
    audioProxy->GetVolume(audioVolumeType);
    audioProxy->SetVolume(audioVolumeType, volume);
    audioProxy->SetVolumeWithDevice(audioVolumeType, volume,
        AudioStandard::DeviceType::DEVICE_TYPE_SPEAKER);
    audioProxy->SetMaxVolume(audioVolumeType);
    audioProxy->IsStreamMute(audioVolumeType);
    audioProxy->GetMaxVolume(audioVolumeType);
    audioProxy->GetMinVolume(audioVolumeType);
    audioProxy->SetAudioDeviceChangeCallback();
}

void AudioSceneProcessorFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<AudioSceneProcessor> audioSceneProcessor = DelayedSingleton<AudioSceneProcessor>::GetInstance();
    std::shared_ptr<AudioControlManager> audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    std::string phoneNum = provider.ConsumeRadomLengthString();

    audioControlManager->IsNumberAllowed(phoneNum);
    audioSceneProcessor->Init();
    audioSceneProcessor->SwitchDialing();
    audioSceneProcessor->SwitchCS();
    audioSceneProcessor->SwitchIMS();
    audioSceneProcessor->SwitchOTT();
    audioSceneProcessor->SwitchHolding();
    audioSceneProcessor->SwitchInactive();
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    FuzzedDataProvider provider(data, size);
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioMicStateChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioDeviceChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioPreferDeviceChangeCallback();
    AudioControlManagerFunc(provider);
    AudioDeviceManagerFunc(provider);
    AudioProxyFunc(provider);
    AudioSceneProcessorFunc(provider);
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
