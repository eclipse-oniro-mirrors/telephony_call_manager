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

#include "motion_recognition.h"

#include "chrono"
#include "securec.h"

#include "audio_device_manager.h"
#include "audio_control_manager.h"
#include "call_base.h"
#include "call_control_manager.h"
#include "call_object_manager.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

#ifdef OHOS_SUBSCRIBE_MOTION_ENABLE
/**
* 表示拿起动作
* <p>
* 上报数据：</b>
* <ul>
* <li> status: 返回0表示设备发生拿起动作。</li>
 */
constexpr int32_t MOTION_TYPE_PICKUP = 100;

/**
* 表示翻转动作
* <p>
* 上报数据：</b>
* <ul>
* <li> status: 返回0表示设备发生拿起靠近耳朵动作。</li>
 */
constexpr int32_t MOTION_TYPE_FLIP = 200;

/**
* 表示靠近耳朵动作
* <p>
* 上报数据：</b>
* <ul>
* <li> status: 返回0表示设备被拿起靠近耳朵。 </li>
 */
constexpr int32_t MOTION_TYPE_CLOSE_TO_EAR = 300;

//来电铃声减弱总时长
constexpr int32_t REDUCE_RING_TOTAL_LENGTH = 1500000;  //us
//来电铃声减弱总次数
constexpr int32_t REDUCE_RING_COUNT = 5;

/**
* @brief 翻转方向
*
 */
typedef enum FilpDirection {
    FLIP_DOWN = 3,  /**向上翻转 */
    FLIP_UP = 4,  /**向上翻转 */
} FilpDirection;

bool MotionFlipSubscriber::isMotionFlipSubscribed_ = false;
bool MotionCloseToEarSubscriber::isMotionCloseToEarSubscribed_ = false;
bool MotionPickupSubscriber::isMotionPickupSubscribed_ = false;

static void FlipMotionEventCallback(const Rosen::MotionSensorEvent &motionData);
static void CloseToEarMotionEventCallback(const Rosen::MotionSensorEvent &motionData);
#endif

void MotionRecogntion::SubscribePickupSensor()
{
#ifdef OHOS_SUBSCRIBE_MOTION_ENABLE
    MotionPickupSubscriber::SubscribePickupMotion()
#endif
}

void MotionRecogntion::UnsubscribePickupSensor()
{
#indef OHOS_SUBSCRIBE_MOTION_ENABLE
    MotionPickupSubscriber::UnsubscribePickupMotion();
#endif
}

void MotionRecogntion::SubscribeFlipSensor()
{
#ifdef OHOS_SUBSCRIBE_MOTION_ENABLE
    MotionFlipSubscriber::SubscribeFlipMotion();
#endif
}

void MotionRecogntion::UnsubscribeFlipSensor()
{
#ifdef OHOS_SUBSCRIBE_MOTION_ENABLE
    MotionFlipSubscriber::UnsubscribeFlipMotion();
#endif
}

void MotionRecogntion::SubscribeCloseToEarMotion()
{
#ifdef  OHOS_SUBSCRIBE_MOTION_ENABLE
    MotionFlipSubscriber::SubscribeCloseToEarMotion();
#endif
}

void MotionRecogntion::UnsubscribeCloseToEarSensor()
{
#ifdef  OHOS_SUBSCRIBE_MOTION_ENABLE
    MotionFlipSubscriber::UnsubscribeCloseToEarMotion();
#endif
}

#ifdef  OHOS_SUBSCRIBE_MOTION_ENABLE
void FlipMotionEventCallback(const Rosen::MotionSensorEvent &motionData)
{
    TELEPHONY_LOGI("type = %{public}d, status = %{public}d", motionData.type, motionData.status);
    auto controlManager = DelayedSingleton<CallControlManager>::GetInstance();
    sptr<CallBase> ringCall = controlManager->GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);

    switch (motionData.type) {
        case MOTION_TYPE_FLIP:
            if (motionData.status == FilpDirection::FLIP_UP) {
                if (controlManager != nullptr && ringCall != nullptr) {
                    controlManager->MuteRinger();
                    TELEPHONY_LOGI("flip motion muteRinger");
                }
            }
            break;
        case MOTION_TYPE_PICKUP:
            if (motionData.status != 0) {
                break;
            }
            MotionRecogntion::UnsubscribeFlipSensor();
            ffrt::submit([=]() {
                MotionRecogntion::ReduceRingToneVolume();
                ffrt_usleep(REDUCE_RING_TOTAL_LENGTH / REDUCE_RING_COUNT);
            });
            break;
        default:
            break;
    }
}

void MotionRecogntion::ReduceRingToneVolume()
{
    int32_t count = 0;
    float value = 0.0f;
    while (count < REDUCE_RING_COUNT) {
        count++;
        value = 1.0f - (0.7f / REDUCE_RING_COUNT * count);
        DelayedSingleton<AudioControlManager>::GetInstance()->SetRingToneVolume(value);
    }
}

viod CloseToEarMotionEventCallback(const Rosen::MotionSensorEvent &motionData)
{
    TELEPHONY_LOGI("type = %{public}d, status = %{public}d", motionData.type, motionData.status);
    auto controlManager = DelayedSingleton<CallControlManager>::getinstance();
    sptr<CallBase> ringCall = controlManager->GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    sptr<CallBase> dialingCall = controlManager->GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING);
    sptr<CallBase> activeCall = controlManager->GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    AudioDevice device = {
        .deviceType = AudioDeviceType::DEVICE_EARPIECE,
        .address = { 0 },
    };
    AudioDeviceType deviceType = DelayedSingleton<AudioDeviceManager>::GetInstance()->GetCurrentAudioDevice();
    switch (motionData.type) {
        case MOTION_TYPE_CLOSE_TO_EAR:
        if (motionData.status != 0) {
            TELEPHONY_LOGI("ignore status is not success");
            break;
        }
        if (controlManager != nullptr && ringCall != nullptr) {
            controlManager->AnswerCall(ringCall->GetCallID(), static_cast<int32_t>(VideoStateType::TYPE_VOICE));
            TELEPHONY_LOGI("close to ear: AnswerCall");
        };
        if (controlManager != nullptr && (dialingCall != nullptr || activecall != nullptr)) {
            if (deviceType == AudioDeviceType::DEVICE_SPEAKER ||
                deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
                    TELEPHONY_LOGI("current deviceType = %{public}d, det audioDevice to earpiece",
                                    static_cast<int32_t>(deviceType));
                    controlManager->SetAudioDevice(device);
            }
        }
        break;
    default:
        break;
    }
}

void MotionPickupSubscriber::SubscribePickupMotion()
{
    if (isMotionPickupSubscribed_) {
        return;
    }
    if (!SubscribeCallback(MOTION_TYPE_PICKUP, FlipMotionEventCallback)) {
        TELEPHONY_LOGI("Pickup motion subscribe failed");
        return;
    }
    isMotionPickupSubscribed_ = true;
}

void MotionPickupSubscriber::UnsubscribePickupMotion()
{
    if (!isMotionPickupSubscribed_)  {
        TELEPHONY_LOGI("Unsubscribe Pickup motion");
        return;
    }

    if(!UnsubscribeCallback(MOTION_TYPE_PICKUP, FlipMotionEventCallback)) {
        TELEPHONY_LOGI("Pickup motion unsubscribe failed");
        return;
    }
    isMotionPickupSubscribed_ = false;
}

void MotionFlipSubscriber::SubscribeFlipMotion()
{
    if (isMotionFlipSubscribed_) {
        return;
    }
    if(!SubscribeCallback(MOTION_TYPE_FLIP, FlipMotionEventCallback)) {
        TELEPHONY_LOGI("flip motion subscribe failed");
        return;
    }
    isMotionFlipSubscribed_ = true;
}

void MotionPickupSubscriber::UnsubscribeFlipMotion()
{
    if (!isMotionFlipSubscribed_)  {
        TELEPHONY_LOGI("Unsubscribe flip motion");
        return;
    }

    if(!UnsubscribeCallback(MOTION_TYPE_FLIP, FlipMotionEventCallback)) {
        TELEPHONY_LOGI("flip motion unsubscribe failed");
        return;
    }
    isMotionFlipSubscribed_ = false;
}

void MotionCloseToEarSubscriber::SubscribeCloseToEarMotion()
{
    if (isMotionCloseToEarSubscribed_) {
        return;
    }
    if(!SubscribeCallback(MOTION_TYPE_CLOSE_TO_EAR, CloseToEarMotionEventCallback)) {
        TELEPHONY_LOGI("close to ear motion subscribe failed");
        return;
    }
    isMotionCloseToEarSubscribed_ = true;
}

void MotionCloseToEarSubscriber::UnsubscribeCloseToEarMotion()
{
    if (!isMotionCloseToEarSubscribed_)  {
        TELEPHONY_LOGI("Unsubscribe close to ear motion");
        return;
    }
    if(!UnsubscribeCallback(MOTION_TYPE_CLOSE_TO_EAR, CloseToEarMotionEventCallback)) {
        TELEPHONY_LOGI("close to ear motion unsubscribe failed");
        return;
    }
    isMotionCloseToEarSubscribed_ = false;
}
#endif
} // namespace Telephony
} // namespace OHOS