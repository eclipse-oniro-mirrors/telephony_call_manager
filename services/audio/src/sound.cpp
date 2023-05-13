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

#include "sound.h"

#include <thread>

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
Sound::Sound() : audioPlayer_(new (std::nothrow) AudioPlayer()) {}

Sound::~Sound()
{
    if (audioPlayer_ != nullptr) {
        delete audioPlayer_;
        audioPlayer_ = nullptr;
    }
}

int32_t Sound::Play()
{
    AudioPlay audioPlay = &AudioPlayer::Play;
    if (audioPlayer_ == nullptr) {
        TELEPHONY_LOGE("audioPlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return audioPlayer_->Play(TYPE_SOUND);
}

int32_t Sound::Stop()
{
    if (audioPlayer_ == nullptr) {
        TELEPHONY_LOGE("audioPlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    audioPlayer_->SetStop(PlayerType::TYPE_SOUND, true);
    return TELEPHONY_SUCCESS;
}

void Sound::ReleaseRenderer()
{
    if (audioPlayer_ == nullptr) {
        TELEPHONY_LOGE("audioPlayer_ is nullptr");
        return;
    }
    audioPlayer_->ReleaseRenderer();
}
} // namespace Telephony
} // namespace OHOS