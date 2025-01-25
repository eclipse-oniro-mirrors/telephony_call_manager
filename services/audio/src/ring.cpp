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

#include "audio_player.h"
#include "call_base.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr int32_t DEFAULT_SIM_SLOT_ID = 0;

Ring::Ring() : audioPlayer_(new (std::nothrow) AudioPlayer())
{
    Init();
}

Ring::~Ring()
{
    if (audioPlayer_ != nullptr) {
        delete audioPlayer_;
        audioPlayer_ = nullptr;
    }
}

void Ring::Init()
{
    SystemSoundManager_ = Media::SystemSoundManagerFactory::CreateSystemSoundManager();
    if (SystemSoundManager_ == nullptr) {
        TELEPHONY_LOGE("get systemSoundManager failed");
        return;
    }
}

int32_t Ring::Play(int32_t slotId)
{
    if (SystemSoundManager_ == nullptr || audioPlayer_ == nullptr) {
        TELEPHONY_LOGE("SystemSoundManager_ or audioPlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    const std::shared_ptr<AbilityRuntime::Context> context;
    Media::RingtoneType type = slotId == DEFAULT_SIM_SLOT_ID ? Media::RingtoneType::RINGTONE_TYPE_SIM_CARD_0 :
        Media::RingtoneType::RINGTONE_TYPE_SIM_CARD_1;
    RingtonePlayer_ = SystemSoundManager_->GetRingtonePlayer(context, type);
    if (RingtonePlayer_ == nullptr) {
        TELEPHONY_LOGE("get RingtonePlayer failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
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
    if (RingtonePlayer_ == nullptr) {
        TELEPHONY_LOGE("RingtonePlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    result = RingtonePlayer_->Stop();
    return result;
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
    return RingtonePlayer_->Configure(0, true);
}

int32_t Ring::SetRingToneVolume(float volume)
{
    TELEPHONY_LOGI("SetRingToneVolume volume = %{public}f", volume);
    if (RingtonePlayer_ == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (volume >= 0.0f && volume <= 1.0f) {
        return RingtonePlayer_->Configure(volume, true);
    } else {
        TELEPHONY_LOGE("volume is valid");
        return TELEPHONY_ERR_FAIL;
    }
}
} // namespace Telephony
} // namespace OHOS