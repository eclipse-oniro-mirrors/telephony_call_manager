/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef MOTION_RECOGNITION_H
#define MOTION_RECOGNITION_H

#include <refbase.h>
#include <screen_sensor_plugin.h>

namespace OHOS {
namespace Telephony {

class MotionRecogntion : public RefBase {
public:
    MotionRecogntion() = delete;
    MotionRecogntion() = default;

    static void SubscribePickupSensor();
    static void UnsubscribePickupSensor();
    static void SubscribeFlipSensor();
    static void UnsubscribeFlipSensor();
    static void SubscribeCloseToEarSensor();
    static void UnsubscribeCloseToEarSensor();
    static void ReduceRingToneVolume();
};

#ifdef  OHOS_SUBSCRIBE_MOTION_ENABLE
class MotionPickupSubscriber
{
    friend MotionRecogntion;

public:
    MotionPickupSubscriber() = delete;
    ~MotionPickupSubscriber() = default;

private:
    static void SubscribePickupMotion();
    static void UnsubscribePickupMotion();

    static bool IsMotionPickupSubscribed_;
};

class MotionCloseToEarSubscriber
{
    friend MotionRecogntion;

public:
    MotionCloseToEarSubscriber() = delete;
    ~MotionCloseToEarSubscriber() = default;

private:
    static void SubscribeCloseToEarMotion();
    static void UnsubscribeCloseToEarMotion();

    static bool isMotionCloseToEarSubscribed_;
};

class MotionFlipSubscriber{
    friend MotionRecogntion;

public:
    MotionFlipSubscriber() = delete;
    ~MotionFlipSubscriber() = default;

private:
    static void SubscribeFlipMotion();
    static void UnsubscribeFlipMotion();

    static bool isMotionFlipSubscribed_;
};
#endif
} // namespace Telephony
} // namespace OHOS
#endif // MOTION_RECOGNITION_H