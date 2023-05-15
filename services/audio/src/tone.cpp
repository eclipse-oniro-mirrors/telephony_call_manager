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

#include <thread>

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using AudioPlay = int32_t (AudioPlayer::*)(const std::string &, AudioStandard::AudioStreamType, PlayerType);

Tone::Tone() : audioPlayer_(new (std::nothrow) AudioPlayer()) {}

Tone::Tone(ToneDescriptor tone)
{
    currentToneDescriptor_ = tone;
}

Tone::~Tone()
{
    if (audioPlayer_ != nullptr) {
        delete audioPlayer_;
        audioPlayer_ = nullptr;
    }
}

void Tone::Init() {}

int32_t Tone::Play()
{
    if (currentToneDescriptor_ == TONE_UNKNOWN) {
        TELEPHONY_LOGE("tone descriptor unknown");
        return CALL_ERR_AUDIO_UNKNOWN_TONE;
    }
    PlayerType playerType = PlayerType::TYPE_TONE;
    if (IsDtmf(currentToneDescriptor_)) {
        playerType = PlayerType::TYPE_DTMF;
    }
    AudioPlay audioPlay = &AudioPlayer::Play;
    if (audioPlayer_ == nullptr) {
        TELEPHONY_LOGE("audioPlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::thread play(audioPlay, audioPlayer_,
        GetToneDescriptorPath(currentToneDescriptor_), AudioStandard::AudioStreamType::STREAM_MUSIC, playerType);
    play.detach();
    return TELEPHONY_SUCCESS;
}

int32_t Tone::Stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (currentToneDescriptor_ == TONE_UNKNOWN) {
        TELEPHONY_LOGE("tone descriptor unknown");
        return CALL_ERR_AUDIO_UNKNOWN_TONE;
    }
    PlayerType playerType = PlayerType::TYPE_TONE;
    if (IsDtmf(currentToneDescriptor_)) {
        playerType = PlayerType::TYPE_DTMF;
    }
    if (audioPlayer_ == nullptr) {
        TELEPHONY_LOGE("audioPlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    audioPlayer_->SetStop(playerType, true);
    return TELEPHONY_SUCCESS;
}

ToneDescriptor Tone::ConvertDigitToTone(char digit)
{
    ToneDescriptor dtmf = ToneDescriptor::TONE_UNKNOWN;
    switch (digit) {
        case '0':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_0;
            break;
        case '1':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_1;
            break;
        case '2':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_2;
            break;
        case '3':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_3;
            break;
        case '4':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_4;
            break;
        case '5':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_5;
            break;
        case '6':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_6;
            break;
        case '7':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_7;
            break;
        case '8':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_8;
            break;
        case '9':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_9;
            break;
        case 'p':
        case 'P':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_P;
            break;
        case 'w':
        case 'W':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_W;
            break;
        default:
            break;
    }
    return dtmf;
}

bool Tone::IsDtmf(ToneDescriptor tone)
{
    bool ret = false;
    switch (tone) {
        case ToneDescriptor::TONE_DTMF_CHAR_0:
        case ToneDescriptor::TONE_DTMF_CHAR_1:
        case ToneDescriptor::TONE_DTMF_CHAR_2:
        case ToneDescriptor::TONE_DTMF_CHAR_3:
        case ToneDescriptor::TONE_DTMF_CHAR_4:
        case ToneDescriptor::TONE_DTMF_CHAR_5:
        case ToneDescriptor::TONE_DTMF_CHAR_6:
        case ToneDescriptor::TONE_DTMF_CHAR_7:
        case ToneDescriptor::TONE_DTMF_CHAR_8:
        case ToneDescriptor::TONE_DTMF_CHAR_9:
        case ToneDescriptor::TONE_DTMF_CHAR_P:
        case ToneDescriptor::TONE_DTMF_CHAR_W:
            ret = true;
            break;
        default:
            break;
    }
    return ret;
}

std::string Tone::GetToneDescriptorPath(ToneDescriptor tone)
{
#ifdef ABILITY_AUDIO_SUPPORT
    return DelayedSingleton<AudioProxy>::GetInstance()->GetToneDescriptorPath(tone);
#endif
    return DelayedSingleton<AudioProxy>::GetInstance()->GetDefaultTonePath();
}

void Tone::ReleaseRenderer()
{
    if (audioPlayer_ == nullptr) {
        TELEPHONY_LOGE("audioPlayer_ is nullptr");
        return;
    }
    audioPlayer_->ReleaseRenderer();
}
} // namespace Telephony
} // namespace OHOS