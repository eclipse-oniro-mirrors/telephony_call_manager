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

#ifndef TELEPHONY_AUDIO_SOUND_H
#define TELEPHONY_AUDIO_SOUND_H

#include <memory>

#include "audio_renderer.h"

#include "audio_player.h"
#include "audio_proxy.h"

namespace OHOS {
namespace Telephony {
enum class SoundState {
    SOUNDING,
    STOPPED,
};
/**
 * @class Sound
 * plays call sound.
 */
class Sound {
public:
    Sound();
    virtual ~Sound();
    void Init();
    int32_t Play();
    int32_t Stop();
    void ReleaseRenderer();

private:
    AudioPlayer *audioPlayer_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif