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

#define TONE_DURATION 3000 // default tone play duration , in millisecond

namespace OHOS {
namespace TelephonyCallManager {
enum ToneDescriptor {
    TONE_UNKNOWN = 0,
    TONE_ENGAGED,
    TONE_FINISHED,
    TONE_WAITING,
    TONE_RING_BACK,
    TONE_NO_SERVICE,
    TONE_INVALID_NUMBER,
};
enum ToneState { STATE_OFF = 0, STATE_ON, STATE_STOPPED };
struct ToneData {
    int toneDescriptor;
    int duration;
    int volume;
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

private:
    ToneDescriptor currentToneDescriptor_;
    static int32_t toneDuration_;
    bool isTonePlaying_;
    bool isCreateComplete_; // whether audio resource create complete or not
};
} // namespace TelephonyCallManager
} // namespace OHOS
#endif // !TONE_H