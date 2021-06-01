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

#ifndef RING_H
#define RING_H
#include <memory>

#include "audio_proxy.h"

namespace OHOS {
namespace TelephonyCallManager {
/**
 * @class Ring
 * plays the default or specific ringtone.
 */
class Ring {
public:
    Ring();
    explicit Ring(const std::string &path);
    virtual ~Ring();
    void Init();
    int32_t Start();
    int32_t Stop();
    int32_t Release();
    int32_t Release(int id);
    int32_t StartVibrate();
    int32_t CancelVibrate();
    static void SetVolume(int32_t v);
    static int32_t GetVolume();
    static void SetLoop(int32_t num);
    static int32_t GetLoop();
    static void SetSpeed(double s);
    static double GetSpeed();

private:
    static constexpr int32_t RING_LOOP_NUM = 10; // default loop time
    static constexpr double RING_SPEED = 1.0f; // default play speed
    int32_t ringId_;
    bool isRinging_;
    bool isVibrating_;
    bool shouldRing_;
    bool shouldVibrate_;
    bool isCreateComplete_; // whether audio resource create complete or not
    static int volume;
    static int loopNum;
    static double speed;
    bool ShouldVibrate();
};
} // namespace TelephonyCallManager
} // namespace OHOS
#endif // !RNG_H