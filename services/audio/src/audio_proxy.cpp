/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "audio_proxy.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "call_manager_log.h"

namespace OHOS {
namespace TelephonyCallManager {
AudioProxy::AudioProxy() : remoteObject_(nullptr) {}

AudioProxy::~AudioProxy()
{
    if (audioManagerProxy_ != nullptr) {
        audioManagerProxy_ = nullptr;
    }
}

void AudioProxy::Init()
{
    CALLMANAGER_INFO_LOG("audio proxy init");
    std::lock_guard<std::mutex> lock(proxyMutex_);
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        CALLMANAGER_ERR_LOG("system ability manager nullptr");
        return;
    }
    remoteObject_ = sam->GetSystemAbility(AUDIO_DISTRIBUTED_SERVICE_ID); // system audio service
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return;
    }
    audioManagerProxy_ = std::make_unique<AudioStandard::AudioManagerProxy>(remoteObject_);
    if (audioManagerProxy_ == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
    }
}

void AudioProxy::ActivateAudioInterrupt()
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    audioManagerProxy_->ActivateAudioInterrupt();
#endif
}

void AudioProxy::DeactivateAudioInterrupt()
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    audioManagerProxy_->DeactivateAudioInterrupt();
#endif
}

void AudioProxy::UpdateCallState(int state)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    audioManagerProxy_->UpdateCallState(state);
#endif
}

void AudioProxy::SetDeviceActive(int deviceType, bool state)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    audioManagerProxy_->SetDeviceActive(deviceType, state);
#endif
}

bool AudioProxy::IsDeviceActive(int deviceType)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return false;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->IsDeviceActive(deviceType);
#endif
    return false;
}

std::string AudioProxy::GetDefaultRingerPath() const
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return "";
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->GetDefaultRingerPath();
#endif
    return "";
}

void AudioProxy::SetBluetoothDevActive(bool active)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    audioManagerProxy_->SetDeviceActive(AudioDeviceType::DEV_BLUETOOTH, active);
#endif
}

void AudioProxy::SetWiredHeadsetDevActive(bool active)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return;
    }

#ifdef ABILITY_AUDIO_SUPPORT
    audioManagerProxy_->SetDeviceActive(AudioDeviceType::DEV_WIRED_HEADSET, active);
#endif
}

void AudioProxy::SetSpeakerDevActive(bool active)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        CALLMANAGER_INFO_LOG("setting speaker dev active state failed");
        return;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    audioManagerProxy_->SetDeviceActive(AudioDeviceType::DEV_SPEAKERPHONE, active);
    CALLMANAGER_INFO_LOG("speaker dev active state successfully set , state : %{public}d", active);
#endif
}

void AudioProxy::SetEarpieceDevActive(bool active) {}

void AudioProxy::SetAudioDeviceChangeObserver()
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        CALLMANAGER_INFO_LOG("setting audio device change observer failed");
        return;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    audioManagerProxy_->SetAudioDeviceChangeObserver();
#endif
}

void AudioProxy::SetOnCreateCompleteListener() {}

int32_t AudioProxy::GetVolume(int audioVolumeType)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return TELEPHONY_FAIL;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->GetVolume(audioVolumeType);
#endif
    return TELEPHONY_FAIL;
}

void AudioProxy::SetVolume(int audioVolumeType, int volume)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    audioManagerProxy_->SetVolume(audioVolumeType, volume);
#endif
}

int32_t AudioProxy::CreateRing(const std::string &ringPath)
{
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->CreateRing(ringPath);
#endif
    return TELEPHONY_NO_ERROR;
}

int32_t AudioProxy::CreateTone(int toneDescriptor, int durationMs)
{
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->CreateTone(int toneDescriptor, int durationMs);
#endif
    return TELEPHONY_NO_ERROR;
}

int32_t AudioProxy::Play(int id, double volume, int loopNum, double speed)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        CALLMANAGER_INFO_LOG("ring play failed");
        return TELEPHONY_FAIL;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->Play(id, volume, loopNum, speed);
#endif
    return TELEPHONY_NO_ERROR;
}

int32_t AudioProxy::Play(int toneDescriptor, int duration)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        CALLMANAGER_INFO_LOG("tone play failed");
        return TELEPHONY_FAIL;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->Play(toneDescriptor, duration);
#endif
    return TELEPHONY_NO_ERROR;
}

int32_t AudioProxy::Play()
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        CALLMANAGER_INFO_LOG("audio play failed");
        return TELEPHONY_FAIL;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->Play();
#endif
    return TELEPHONY_NO_ERROR;
}

int32_t AudioProxy::StopCallTone()
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        CALLMANAGER_INFO_LOG("tone stop failed");
        return TELEPHONY_FAIL;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->StopCallTone();
#endif
    return TELEPHONY_NO_ERROR;
}

int32_t AudioProxy::StopRing(int id)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        CALLMANAGER_INFO_LOG("ring stop failed");
        return TELEPHONY_FAIL;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->StopCallTone(id);
#endif
    return TELEPHONY_NO_ERROR;
}

int32_t AudioProxy::Release()
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        CALLMANAGER_INFO_LOG("audio release failed");
        return TELEPHONY_FAIL;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->Release();
#endif
    return TELEPHONY_NO_ERROR;
}

int32_t AudioProxy::Release(int id)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        CALLMANAGER_INFO_LOG("audio release failed");
        return TELEPHONY_FAIL;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->Release(id);
#endif
    return TELEPHONY_NO_ERROR;
}

bool AudioProxy::IsMute(int audioVolumeType)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return false;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->IsMute(audioVolumeType);
#endif
    return false;
}

int32_t AudioProxy::GetMaxVolume(int audioVolumeType)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return TELEPHONY_FAIL;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->GetMaxVolume(audioVolumeType);
#endif
    return TELEPHONY_NO_ERROR;
}

int32_t AudioProxy::GetMinVolume(int audioVolumeType)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return TELEPHONY_FAIL;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->GetMinVolume(audioVolumeType);
#endif
    return TELEPHONY_NO_ERROR;
}

bool AudioProxy::IsMicrophoneMute()
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        return false;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->IsMicrophoneMute();
#endif
    return false;
}

void AudioProxy::SetMicrophoneMute(bool mute)
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
        CALLMANAGER_INFO_LOG("set microphone mute failed");
        return;
    }
#ifdef ABILITY_AUDIO_SUPPORT
    audioManagerProxy_->SetMicrophoneMute(mute);
#endif
}

int32_t AudioProxy::StartVibrate()
{
    return TELEPHONY_NO_ERROR; // return default value
}

int32_t AudioProxy::CancelVibrate()
{
    return TELEPHONY_NO_ERROR; // return default value
}

AudioRingMode AudioProxy::GetRingerMode() const
{
    if (remoteObject_ == nullptr) {
        CALLMANAGER_ERR_LOG("remote object : audio service  , nullptr");
    }
#ifdef ABILITY_AUDIO_SUPPORT
    return audioManagerProxy_->GetRingerMode();
#endif
    return AudioRingMode::RINGER_MODE_NORMAL;
}

bool AudioProxy::IsVibrateMode() const
{
    return AudioRingMode::RINGER_MODE_VIBRATE == GetRingerMode();
}
} // namespace TelephonyCallManager
} // namespace OHOS