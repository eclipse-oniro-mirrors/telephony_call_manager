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

#ifndef AUDIO_RING_H
#define AUDIO_RING_H

#include <memory>

#include "audio_renderer.h"

#include "audio_player.h"
#include "audio_proxy.h"

namespace OHOS {
namespace Telephony {
struct RingtoneStream {
    std::string ringtonePath;
    uint32_t playedPosition;
};
enum RingState {
    RINGING = 0,
    STOPPED,
};
/**
 * @class AudioRing
 * plays the default or specific ringtone.
 */
class AudioRing {
public:
    AudioRing();
    explicit AudioRing(const std::string &path);
    virtual ~AudioRing();
    void Init(const std::string &ringtonePath);
    int32_t Play();
    int32_t Stop();
    int32_t Resume();
    int32_t Pause();
    int32_t Release();
    int32_t StartVibrate();
    int32_t CancelVibrate();

private:
    bool isVibrating_;
    bool shouldRing_;
    bool shouldVibrate_;
    bool ShouldVibrate();
    std::string ringtonePath_;
};
} // namespace Telephony
} // namespace OHOS
#endif // AUDIO_RING_H