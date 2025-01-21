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

#ifndef TELEPHONY_AUDIO_RING_H
#define TELEPHONY_AUDIO_RING_H

#include <memory>

#include "audio_renderer.h"

#include "audio_player.h"
#include "audio_proxy.h"
#include "ringtone_player.h"
#include "system_sound_manager.h"

namespace OHOS {
namespace Telephony {
enum class RingState {
    RINGING = 0,
    STOPPED,
};

static constexpr const char* RING_PLAY_THREAD = "ringPlayThread";

/**
 * @class Ring
 * plays the default or specific ringtone.
 */
class Ring {
public:
    Ring();
    virtual ~Ring();
    void Init();
    int32_t Play(int32_t slotId, std::string ringtonePath);
    int32_t Stop();
    void ReleaseRenderer();
    int32_t SetMute();

private:
    std::mutex mutex_;
    AudioPlayer *audioPlayer_ = nullptr;
    std::shared_ptr<Media::SystemSoundManager> SystemSoundManager_ = nullptr;
    std::shared_ptr<Media::RingtonePlayer> RingtonePlayer_ = nullptr;
    int32_t defaultVolume_ = 1;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_RING_H