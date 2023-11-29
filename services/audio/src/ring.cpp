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

#include "ring.h"

#include <thread>

#include "telephony_log_wrapper.h"

#include "audio_player.h"

namespace OHOS {
namespace Telephony {
using AudioPlay = int32_t (AudioPlayer::*)(const std::string &, AudioStandard::AudioStreamType, PlayerType);

Ring::Ring() : isVibrating_(false), shouldRing_(false), shouldVibrate_(false), ringtonePath_(""),
    audioPlayer_(new (std::nothrow) AudioPlayer())
{
    Init(DelayedSingleton<AudioProxy>::GetInstance()->GetDefaultRingPath());
}

Ring::Ring(const std::string &path)
    : isVibrating_(false), shouldRing_(false), shouldVibrate_(false), ringtonePath_("")
{
    Init(path);
}

Ring::~Ring()
{
    if (audioPlayer_ != nullptr) {
        delete audioPlayer_;
        audioPlayer_ = nullptr;
    }
}

void Ring::Init(const std::string &ringtonePath)
{
    if (ringtonePath.empty()) {
        TELEPHONY_LOGE("ringtone path empty");
        return;
    }
    if (AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL ==
        DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode()) {
        shouldRing_ = true;
        shouldVibrate_ = true;
    } else if (AudioStandard::AudioRingerMode::RINGER_MODE_VIBRATE ==
        DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode()) {
        shouldRing_ = false;
        shouldVibrate_ = true;
    } else if (AudioStandard::AudioRingerMode::RINGER_MODE_SILENT ==
        DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode()) {
        shouldRing_ = false;
        shouldVibrate_ = false;
    }
    ringtonePath_ = ringtonePath;

    SystemSoundManager_ = Media::SystemSoundManagerFactory::CreateSystemSoundManager();
    if (SystemSoundManager_ == nullptr) {
        TELEPHONY_LOGE("get systemSoundManager failed");
        return;
    }
}

int32_t Ring::Play(int32_t slotId)
{
    const std::shared_ptr<AbilityRuntime::Context> context;
    RingtonePlayer_ = SystemSoundManager_->GetRingtonePlayer(context, static_cast<Media::RingtoneType>(slotId));
    if (RingtonePlayer_ == nullptr) {
        TELEPHONY_LOGE("get RingtonePlayer failed");
    }
    audioPlayer_->RegisterRingCallback(RingtonePlayer_);
    int32_t result = RingtonePlayer_->Configure(defaultVolume_, true);
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("configure failed");
    }
    audioPlayer_->SetStop(PlayerType::TYPE_RING, false);
    return RingtonePlayer_->Start();
}

int32_t Ring::Stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (audioPlayer_ == nullptr) {
        TELEPHONY_LOGE("audioPlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t result = TELEPHONY_SUCCESS;
    audioPlayer_->SetStop(PlayerType::TYPE_RING, true);
    result = RingtonePlayer_->Stop();
    return result;
}

int32_t Ring::StartVibrate()
{
    if (DelayedSingleton<AudioProxy>::GetInstance()->StartVibrate() == TELEPHONY_SUCCESS) {
        isVibrating_ = true;
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("start vibrate failed");
    return CALL_ERR_AUDIO_START_VIBRATE_FAILED;
}

int32_t Ring::CancelVibrate()
{
    if (DelayedSingleton<AudioProxy>::GetInstance()->CancelVibrate() == TELEPHONY_SUCCESS) {
        isVibrating_ = false;
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("cancel vibrate failed");
    return CALL_ERR_AUDIO_CANCEL_VIBRATE_FAILED;
}

bool Ring::ShouldVibrate()
{
    return DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode() !=
        AudioStandard::AudioRingerMode::RINGER_MODE_SILENT;
}

void Ring::ReleaseRenderer()
{
    if (RingtonePlayer_ == nullptr) {
        TELEPHONY_LOGE("RingtonePlayer_ is nullptr");
        return;
    }
    RingtonePlayer_->Release();
}

int32_t Ring::SetMute()
{
    if (RingtonePlayer_ == nullptr) {
        TELEPHONY_LOGE("RingtonePlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return RingtonePlayer_->Stop();
}
} // namespace Telephony
} // namespace OHOS