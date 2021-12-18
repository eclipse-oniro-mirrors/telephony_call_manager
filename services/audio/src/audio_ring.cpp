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

#include "audio_ring.h"

#include <thread>

#include "telephony_log_wrapper.h"

#include "audio_player.h"

namespace OHOS {
namespace Telephony {
AudioRing::AudioRing() : isVibrating_(false), shouldRing_(false), shouldVibrate_(false), ringtonePath_("")
{
    Init(DelayedSingleton<AudioProxy>::GetInstance()->GetDefaultRingPath());
}

AudioRing::AudioRing(const std::string &path)
    : isVibrating_(false), shouldRing_(false), shouldVibrate_(false), ringtonePath_("")
{
    Init(path);
}

AudioRing::~AudioRing() {}

void AudioRing::Init(const std::string &ringtonePath)
{
    if (ringtonePath.empty()) {
        TELEPHONY_LOGE("ringtone path empty");
        return;
    }
    if (AudioRingerMode::RINGER_MODE_NORMAL == DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode()) {
        shouldRing_ = true;
        shouldVibrate_ = true;
    } else if (AudioRingerMode::RINGER_MODE_VIBRATE ==
        DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode()) {
        shouldRing_ = false;
        shouldVibrate_ = true;
    } else if (AudioRingerMode::RINGER_MODE_SILENT == DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode()) {
        shouldRing_ = false;
        shouldVibrate_ = false;
    }
    ringtonePath_ = ringtonePath;
}

int32_t AudioRing::Play()
{
    if (!shouldRing_ || ringtonePath_.empty()) {
        TELEPHONY_LOGE("should not ring or ringtonePath empty");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t result = TELEPHONY_SUCCESS;
    std::thread play(AudioPlayer::Play, ringtonePath_, AudioStreamType::STREAM_RING, PlayerType::TYPE_RING);
    play.detach();
    if (shouldVibrate_) {
        result = StartVibrate();
    }
    return result;
}

int32_t AudioRing::Stop()
{
    if (!shouldRing_ || ringtonePath_.empty()) {
        TELEPHONY_LOGE("should not ring or ringtonePath empty");
        return TELEPHONY_ERR_FAIL;
    }
    int32_t result = TELEPHONY_SUCCESS;
    AudioPlayer::SetStop(PlayerType::TYPE_RING, true);
    if (isVibrating_) {
        result = CancelVibrate();
    }
    return result;
}

int32_t AudioRing::StartVibrate()
{
    if (DelayedSingleton<AudioProxy>::GetInstance()->StartVibrate() == TELEPHONY_SUCCESS) {
        isVibrating_ = true;
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

int32_t AudioRing::CancelVibrate()
{
    if (DelayedSingleton<AudioProxy>::GetInstance()->CancelVibrate() == TELEPHONY_SUCCESS) {
        isVibrating_ = false;
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

bool AudioRing::ShouldVibrate()
{
    return DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode() != AudioRingerMode::RINGER_MODE_SILENT;
}
} // namespace Telephony
} // namespace OHOS