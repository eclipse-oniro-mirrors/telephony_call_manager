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

#include "tone.h"

#include "call_manager_log.h"

#include "audio_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
int32_t Tone::toneDuration_ = TONE_DURATION;

Tone::Tone()
    : currentToneDescriptor_(ToneDescriptor::TONE_UNKNOWN), isTonePlaying_(false), isCreateComplete_(false)
{}

Tone::Tone(ToneDescriptor type) : currentToneDescriptor_(type), isTonePlaying_(false), isCreateComplete_(false) {}

Tone::~Tone()
{
    Release();
}

void Tone::Init() {}

int32_t Tone::Start()
{
    if (currentToneDescriptor_ == TONE_UNKNOWN) {
        CALLMANAGER_INFO_LOG("tone type unknown");
        return TELEPHONY_FAIL;
    }
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return TELEPHONY_FAIL;
    }
    if (audioProxy->Play(currentToneDescriptor_, toneDuration_) == TELEPHONY_NO_ERROR) {
        isTonePlaying_ = true;
        return TELEPHONY_NO_ERROR;
    }
    return TELEPHONY_FAIL;
}

int32_t Tone::Stop()
{
    if (isTonePlaying_ == false) {
        return TELEPHONY_FAIL;
    }
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return TELEPHONY_FAIL;
    }
    if (audioProxy->StopCallTone() == TELEPHONY_NO_ERROR) {
        isTonePlaying_ = false;
        isCreateComplete_ = false;
        return TELEPHONY_NO_ERROR;
    }
    return TELEPHONY_FAIL;
}

int32_t Tone::Release()
{
    CALLMANAGER_INFO_LOG("tone release");
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy == nullptr) {
        CALLMANAGER_ERR_LOG("audio proxy nullptr");
        return TELEPHONY_FAIL;
    }
    if (audioProxy->Release() == TELEPHONY_NO_ERROR) {
        isTonePlaying_ = false;
        isCreateComplete_ = false;
        return TELEPHONY_NO_ERROR;
    }
    return TELEPHONY_FAIL;
}

void Tone::SetDurationMs(int32_t duration)
{
    toneDuration_ = duration;
}

int32_t Tone::GetDurationMs()
{
    return toneDuration_;
}
} // namespace TelephonyCallManager
} // namespace OHOS