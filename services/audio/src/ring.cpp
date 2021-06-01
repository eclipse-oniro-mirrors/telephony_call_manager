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

#include "ring.h"

#include "call_manager_log.h"

#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
int Ring::volume = 0;
int Ring::loopNum = RING_LOOP_NUM;
double Ring::speed = RING_SPEED;

Ring::Ring()
    : ringId_(-1), isRinging_(false), isVibrating_(false), shouldRing_(false), shouldVibrate_(false),
      isCreateComplete_(false)
{
    Init();
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy != nullptr) {
        ringId_ = audioProxy->CreateRing(audioProxy->GetDefaultRingerPath()); // default ring
    }
}

Ring::Ring(const std::string &ringtonePath)
    : ringId_(-1), isRinging_(false), isVibrating_(false), shouldRing_(false), shouldVibrate_(false),
      isCreateComplete_(false)
{
    Init();
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy != nullptr) {
        ringId_ = audioProxy->CreateRing(ringtonePath); // specific ring
    }
}

Ring::~Ring()
{
    Release();
}

void Ring::Init()
{
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return;
    }
    audioProxy->Init();
    if (AudioRingMode::RINGER_MODE_NORMAL == audioProxy->GetRingerMode()) {
        shouldRing_ = true;
        shouldVibrate_ = true;
    } else if (AudioRingMode::RINGER_MODE_VIBRATE == audioProxy->GetRingerMode()) {
        shouldRing_ = false;
        shouldVibrate_ = true;
    } else if (AudioRingMode::RINGER_MODE_SILENT == audioProxy->GetRingerMode()) {
        shouldRing_ = false;
        shouldVibrate_ = false;
    }
    volume = audioProxy->GetVolume(AudioVolumeType::STREAM_RING);
}

int32_t Ring::Start()
{
    if (ringId_ == -1) {
        CALLMANAGER_ERR_LOG("ring id invalid");
        return TELEPHONY_FAIL;
    }
    if (shouldVibrate_) {
        StartVibrate();
    }
    if (shouldRing_) {
        auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
        if (audioProxy == nullptr) {
            CALLMANAGER_ERR_LOG("audio proxy nullptr");
            return TELEPHONY_FAIL;
        }
        if (audioProxy->Play(ringId_, volume, loopNum, speed) == TELEPHONY_NO_ERROR) {
            isRinging_ = true;
            return TELEPHONY_NO_ERROR;
        }
    }
    return TELEPHONY_FAIL;
}

int32_t Ring::Stop()
{
    CALLMANAGER_INFO_LOG("ring stop");
    if (ringId_ == -1) {
        CALLMANAGER_ERR_LOG("ring id invalid");
        return TELEPHONY_FAIL;
    }
    if (isVibrating_) {
        CancelVibrate();
    }
    if (isRinging_) {
        auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
        if (audioProxy == nullptr) {
            CALLMANAGER_ERR_LOG("audio proxy nullptr");
            return TELEPHONY_FAIL;
        }
        if (audioProxy->StopRing(ringId_) == TELEPHONY_NO_ERROR) {
            isRinging_ = false;
            isCreateComplete_ = false;
            return TELEPHONY_NO_ERROR;
        }
    }
    return TELEPHONY_FAIL;
}

int32_t Ring::Release(int id)
{
    CALLMANAGER_INFO_LOG("ring release");
    if (ringId_ == -1) {
        CALLMANAGER_ERR_LOG("ring id invalid");
        return TELEPHONY_FAIL;
    }
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return TELEPHONY_FAIL;
    }
    if (audioProxy->Release(id) == TELEPHONY_NO_ERROR) {
        return TELEPHONY_NO_ERROR;
    }
    return TELEPHONY_FAIL;
}

int32_t Ring::StartVibrate()
{
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return TELEPHONY_FAIL;
    }
    if (audioProxy->StartVibrate() == TELEPHONY_NO_ERROR) {
        isVibrating_ = true;
        return TELEPHONY_NO_ERROR;
    }
    return TELEPHONY_FAIL;
}

int32_t Ring::CancelVibrate()
{
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return TELEPHONY_FAIL;
    }
    if (audioProxy->CancelVibrate() == TELEPHONY_NO_ERROR) {
        return TELEPHONY_NO_ERROR;
    }
    return TELEPHONY_FAIL;
}

int32_t Ring::Release()
{
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return TELEPHONY_FAIL;
    }
    if (audioProxy->Release(ringId_) == TELEPHONY_NO_ERROR) {
        return TELEPHONY_NO_ERROR;
    }
    return TELEPHONY_FAIL;
}

void Ring::SetLoop(int32_t num)
{
    loopNum = num;
}

int32_t Ring::GetLoop()
{
    return loopNum;
}

void Ring::SetVolume(int32_t v)
{
    volume = v;
}

int32_t Ring::GetVolume()
{
    return volume;
}

void Ring::SetSpeed(double s)
{
    speed = s;
}

double Ring::GetSpeed()
{
    return speed;
}

bool Ring::ShouldVibrate()
{
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return false;
    }
    return audioProxy->GetRingerMode() != AudioRingMode::RINGER_MODE_SILENT;
}
} // namespace TelephonyCallManager
} // namespace OHOS