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

#ifndef TONE_H
#define TONE_H

#include <memory>

#include "audio_renderer.h"

#include "audio_player.h"
#include "audio_proxy.h"

namespace OHOS {
namespace Telephony {
enum ToneDescriptor {
    TONE_UNKNOWN = 0,
    TONE_ENGAGED,
    TONE_FINISHED,
    TONE_WAITING,
    TONE_RING_BACK,
    TONE_NO_SERVICE,
    TONE_INVALID_NUMBER,
    TONE_CALL_RECORDING,
    TONE_DTMF_CHAR_0,
    TONE_DTMF_CHAR_1,
    TONE_DTMF_CHAR_2,
    TONE_DTMF_CHAR_3,
    TONE_DTMF_CHAR_4,
    TONE_DTMF_CHAR_5,
    TONE_DTMF_CHAR_6,
    TONE_DTMF_CHAR_7,
    TONE_DTMF_CHAR_8,
    TONE_DTMF_CHAR_9,
    TONE_DTMF_CHAR_P,
    TONE_DTMF_CHAR_W
};
struct ToneStream {
    ToneDescriptor toneDescriptor;
    int32_t duration;
    int32_t volume;
};

/**
 * @class Tone
 * plays the specific tone.
 */
class Tone {
public:
    Tone();
    Tone(ToneDescriptor tone);
    virtual ~Tone();
    void Init();
    int32_t Play();
    int32_t Stop();
    int32_t Release();
    static void SetDurationMs(int32_t duration);
    static int32_t GetDurationMs();
    static ToneDescriptor ConvertDigitToTone(char digit);

private:
    ToneDescriptor currentToneDescriptor_ = ToneDescriptor::TONE_UNKNOWN;
    std::string GetToneDescriptorPath(ToneDescriptor tone);
    bool IsDtmf(ToneDescriptor tone);
};
} // namespace Telephony
} // namespace OHOS
#endif // TONE_H