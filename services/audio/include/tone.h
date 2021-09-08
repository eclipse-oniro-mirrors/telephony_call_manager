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
    DTMF_DIGIT_0,
    DTMF_DIGIT_1,
    DTMF_DIGIT_2,
    DTMF_DIGIT_3,
    DTMF_DIGIT_4,
    DTMF_DIGIT_5,
    DTMF_DIGIT_6,
    DTMF_DIGIT_7,
    DTMF_DIGIT_8,
    DTMF_DIGIT_9,
    DTMF_DIGIT_P,
    DTMF_DIGIT_W
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
    explicit Tone(ToneDescriptor type);
    virtual ~Tone();
    void Init();
    int32_t Start();
    int32_t Stop();
    int32_t Release();
    static void SetDurationMs(int32_t duration);
    static int32_t GetDurationMs();
    static ToneDescriptor ConvertDigitToTone(char digit);

private:
    const static int32_t TONE_DURATION = 3000; // default tone play duration , in millisecond
    ToneDescriptor currentToneDescriptor_;
    static int32_t toneDuration_;
    bool isTonePlaying_;
    bool isCreateComplete_; // whether audio resource create complete or not
};
} // namespace Telephony
} // namespace OHOS
#endif // TONE_H