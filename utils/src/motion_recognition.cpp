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

#include "audio_control_manager.h"
#include "audio_device_manager.h"
#include "audio_proxy.h"
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

/**
* @brief 翻转方向
*
 */
typedef enum FilpDirection {
    FLIP_DOWN = 3,  /**向上翻转 */
    FLIP_UP = 4,  /**向下翻转 */
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
    MotionPickupSubscriber::SubscribePickupMotion();
#endif
}

void MotionRecogntion::UnsubscribePickupSensor()
{
#ifdef OHOS_SUBSCRIBE_MOTION_ENABLE
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

void MotionRecogntion::SubscribeCloseToEarSensor()
{
#ifdef  OHOS_SUBSCRIBE_MOTION_ENABLE
    MotionCloseToEarSubscriber::SubscribeCloseToEarMotion();
#endif
}

void MotionRecogntion::UnsubscribeCloseToEarSensor()
{
#ifdef  OHOS_SUBSCRIBE_MOTION_ENABLE
    MotionCloseToEarSubscriber::UnsubscribeCloseToEarMotion();
#endif
}

#ifdef  OHOS_SUBSCRIBE_MOTION_ENABLE
void FlipMotionEventCallback(const Rosen::MotionSensorEvent &motionData)
{
    TELEPHONY_LOGI("type = %{public}d, status = %{public}d", motionData.type, motionData.status);
    auto controlManager = DelayedSingleton<CallControlManager>::GetInstance();

    switch (motionData.type) {
        case MOTION_TYPE_FLIP:
            if (motionData.status == FilpDirection::FLIP_UP && controlManager != nullptr) {
                sptr<CallBase> ringCall = controlManager->
                    GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
                if (ringCall != nullptr) {
                    controlManager->MuteRinger();
                    TELEPHONY_LOGI("flip motion muteRinger");
                }
                controlManager->StopFlashRemind();
            }
            break;
        case MOTION_TYPE_PICKUP:
            if (motionData.status != 0) {
                break;
            }
            MotionRecogntion::UnsubscribePickupSensor();
            if (controlManager != nullptr) {
                controlManager->StopFlashRemind();
            }
            ffrt::submit([=]() {
                MotionRecogntion::ReduceRingToneVolume();
            });
            break;
        default:
            break;
    }
}

void MotionRecogntion::ReduceRingToneVolume()
{
    int32_t count = 0;
    int32_t currentVolume = DelayedSingleton<AudioProxy>::GetInstance()->GetVolume(
        AudioStandard::AudioVolumeType::STREAM_RING);
    if (currentVolume <= 1) {
        TELEPHONY_LOGE("currentVolume = %{public}d, return it", currentVolume);
        return;
    }
    float beginVolumeDb = DelayedSingleton<AudioProxy>::GetInstance()->GetSystemRingVolumeInDb(currentVolume);
    if (beginVolumeDb == 0.0f) {
        TELEPHONY_LOGE("GetSystemRingVolumeInDb fail");
        return;
    }
    int32_t reduceCount = currentVolume - 1;
    while (count < reduceCount) {
        count++;
        float endVolumeDb = DelayedSingleton<AudioProxy>::GetInstance()->GetSystemRingVolumeInDb(currentVolume - count);
        if (endVolumeDb == 0.0f || beginVolumeDb <= endVolumeDb) {
            TELEPHONY_LOGE("GetSystemRingVolumeInDb fail or beginVolumeDb unexpected");
            return;
        }
        DelayedSingleton<AudioControlManager>::GetInstance()->SetRingToneVolume(endVolumeDb / beginVolumeDb);
        ffrt_usleep(REDUCE_RING_TOTAL_LENGTH / reduceCount);
    }
}

void CloseToEarMotionEventCallback(const Rosen::MotionSensorEvent &motionData)
{
    TELEPHONY_LOGI("type = %{public}d, status = %{public}d", motionData.type, motionData.status);
    auto controlManager = DelayedSingleton<CallControlManager>::GetInstance();
    if (controlManager == nullptr) {
        return;
    }
    sptr<CallBase> ringCall = controlManager->GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    sptr<CallBase> dialingCall = controlManager->GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING);
    sptr<CallBase> activeCall = controlManager->GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    sptr<CallBase> holdingCall = controlManager->GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
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
            if (dialingCall == nullptr && activeCall == nullptr && holdingCall == nullptr && ringCall != nullptr) {
                controlManager->AnswerCall(ringCall->GetCallID(), static_cast<int32_t>(VideoStateType::TYPE_VOICE));
                TELEPHONY_LOGI("close to ear: AnswerCall");
            };
            if (dialingCall != nullptr || activeCall != nullptr) {
                if (deviceType == AudioDeviceType::DEVICE_SPEAKER ||
                    deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO ||
                    deviceType == AudioDeviceType::DEVICE_NEARLINK) {
                        TELEPHONY_LOGI("current deviceType = %{public}d, det audioDevice to earpiece",
                            static_cast<int32_t>(deviceType));
                        controlManager->SetAudioDevice(device);
                }
            }
            controlManager->StopFlashRemind();
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

    if (!UnsubscribeCallback(MOTION_TYPE_PICKUP, FlipMotionEventCallback)) {
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
    if (!SubscribeCallback(MOTION_TYPE_FLIP, FlipMotionEventCallback)) {
        TELEPHONY_LOGI("flip motion subscribe failed");
        return;
    }
    isMotionFlipSubscribed_ = true;
}

void MotionFlipSubscriber::UnsubscribeFlipMotion()
{
    if (!isMotionFlipSubscribed_)  {
        TELEPHONY_LOGI("Unsubscribe flip motion");
        return;
    }

    if (!UnsubscribeCallback(MOTION_TYPE_FLIP, FlipMotionEventCallback)) {
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
    if (!SubscribeCallback(MOTION_TYPE_CLOSE_TO_EAR, CloseToEarMotionEventCallback)) {
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
    if (!UnsubscribeCallback(MOTION_TYPE_CLOSE_TO_EAR, CloseToEarMotionEventCallback)) {
        TELEPHONY_LOGI("close to ear motion unsubscribe failed");
        return;
    }
    isMotionCloseToEarSubscribed_ = false;
}
#endif
} // namespace Telephony
} // namespace OHOS