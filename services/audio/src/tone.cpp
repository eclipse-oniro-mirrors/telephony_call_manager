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

Tone::Tone(ToneDescriptor tone) : audioPlayer_(new (std::nothrow) AudioPlayer())
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
    if (IsUseTonePlayer(currentToneDescriptor_)) {
        TELEPHONY_LOGI("currentToneDescriptor = %{public}d", currentToneDescriptor_);
        if (!InitTonePlayer()) {
            return TELEPHONY_ERROR;
        }
        std::thread play([&]() {
            pthread_setname_np(pthread_self(), TONE_PLAY_THREAD);
            tonePlayer_->StartTone();
        });
        play.detach();
    } else {
        AudioPlay audioPlay = &AudioPlayer::Play;
        if (audioPlayer_ == nullptr) {
            TELEPHONY_LOGE("audioPlayer_ is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        std::thread play(audioPlay, audioPlayer_, GetToneDescriptorPath(currentToneDescriptor_),
            AudioStandard::AudioStreamType::STREAM_MUSIC, PlayerType::TYPE_TONE);
        pthread_setname_np(play.native_handle(), TONE_PLAY_THREAD);
        play.detach();
    }
    return TELEPHONY_SUCCESS;
}

int32_t Tone::Stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (currentToneDescriptor_ == TONE_UNKNOWN) {
        TELEPHONY_LOGE("tone descriptor unknown");
        return CALL_ERR_AUDIO_UNKNOWN_TONE;
    }
    if (IsUseTonePlayer(currentToneDescriptor_)) {
        if (tonePlayer_ != nullptr) {
            tonePlayer_->StopTone();
            tonePlayer_->Release();
        }
    } else {
        if (audioPlayer_ == nullptr) {
            TELEPHONY_LOGE("audioPlayer_ is nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        audioPlayer_->SetStop(PlayerType::TYPE_TONE, true);
    }
    return TELEPHONY_SUCCESS;
}

bool Tone::InitTonePlayer()
{
    using namespace OHOS::AudioStandard;
    if (tonePlayer_ == nullptr) {
        StreamUsage streamUsage = GetStreamUsageByToneType(currentToneDescriptor_);
        AudioRendererInfo rendererInfo = {};
        rendererInfo.contentType = ContentType::CONTENT_TYPE_UNKNOWN;
        rendererInfo.streamUsage = streamUsage;
        rendererInfo.rendererFlags = 0;
        tonePlayer_ = TonePlayer::Create(rendererInfo);
        if (tonePlayer_ == nullptr) {
            return false;
        }
        ToneType toneType = ConvertToneDescriptorToToneType(currentToneDescriptor_);
        if (!tonePlayer_->LoadTone(toneType)) {
            return false;
        }
    }
    return true;
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
        case '*':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_P;
            break;
        case '#':
            dtmf = ToneDescriptor::TONE_DTMF_CHAR_W;
            break;
        default:
            break;
    }
    return dtmf;
}

AudioStandard::ToneType Tone::ConvertToneDescriptorToToneType(ToneDescriptor tone)
{
    using namespace OHOS::AudioStandard;
    ToneType tonType = ToneType::NUM_TONES;
    switch (tone) {
        case ToneDescriptor::TONE_DTMF_CHAR_0:
            tonType = ToneType::TONE_TYPE_DIAL_0;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_1:
            tonType = ToneType::TONE_TYPE_DIAL_1;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_2:
            tonType = ToneType::TONE_TYPE_DIAL_2;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_3:
            tonType = ToneType::TONE_TYPE_DIAL_3;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_4:
            tonType = ToneType::TONE_TYPE_DIAL_4;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_5:
            tonType = ToneType::TONE_TYPE_DIAL_5;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_6:
            tonType = ToneType::TONE_TYPE_DIAL_6;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_7:
            tonType = ToneType::TONE_TYPE_DIAL_7;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_8:
            tonType = ToneType::TONE_TYPE_DIAL_8;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_9:
            tonType = ToneType::TONE_TYPE_DIAL_9;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_P:
            tonType = ToneType::TONE_TYPE_DIAL_S;
            break;
        case ToneDescriptor::TONE_DTMF_CHAR_W:
            tonType = ToneType::TONE_TYPE_DIAL_P;
            break;
        default:
            ConvertCallToneDescriptorToToneType(tone);
            break;
    }
    return tonType;
}

AudioStandard::ToneType Tone::ConvertCallToneDescriptorToToneType(ToneDescriptor tone)
{
    using namespace OHOS::AudioStandard;
    ToneType tonType = ToneType::NUM_TONES;
    switch (tone) {
        case ToneDescriptor::TONE_RINGBACK:
            tonType = ToneType::TONE_TYPE_COMMON_SUPERVISORY_RINGTONE;
            break;
        case ToneDescriptor::TONE_WAITING:
            tonType = ToneType::TONE_TYPE_COMMON_SUPERVISORY_CALL_WAITING;
            break;
        default:
            break;
    }
    return tonType;
}

AudioStandard::StreamUsage Tone::GetStreamUsageByToneType(ToneDescriptor descriptor)
{
    AudioStandard::StreamUsage streamUsage = AudioStandard::StreamUsage::STREAM_USAGE_UNKNOWN;
    switch (descriptor) {
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
            streamUsage = AudioStandard::StreamUsage::STREAM_USAGE_DTMF;
            break;
        case ToneDescriptor::TONE_RINGBACK:
        case ToneDescriptor::TONE_WAITING:
            streamUsage = AudioStandard::StreamUsage::STREAM_USAGE_VOICE_COMMUNICATION;
            break;
        default:
            break;
    }
    return streamUsage;
}

bool Tone::IsUseTonePlayer(ToneDescriptor tone)
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
        case ToneDescriptor::TONE_RINGBACK:
        case ToneDescriptor::TONE_WAITING:
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
