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

#include "audio_player.h"

#include "audio_control_manager.h"
#include "audio_system_manager.h"
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
bool AudioPlayer::InitRenderer(const wav_hdr &wavHeader, AudioStandard::AudioStreamType streamType)
{
    AudioStandard::AudioRendererParams rendererParams;
    rendererParams.sampleFormat = AudioStandard::AudioSampleFormat::SAMPLE_S16LE;
    rendererParams.sampleRate = static_cast<AudioStandard::AudioSamplingRate>(wavHeader.SamplesPerSec);
    rendererParams.channelCount = static_cast<AudioStandard::AudioChannel>(wavHeader.NumOfChan);
    rendererParams.encodingType = static_cast<AudioStandard::AudioEncodingType>(AudioStandard::ENCODING_PCM);
    audioRenderer_ = AudioStandard::AudioRenderer::Create(streamType);
    if (audioRenderer_ == nullptr) {
        TELEPHONY_LOGE("audio renderer create failed");
        return false;
    }
    audioRenderer_->SetInterruptMode(AudioStandard::INDEPENDENT_MODE);
    if (audioRenderer_->SetParams(rendererParams) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("audio renderer set params failed");
        return false;
    }
    callback_ = std::make_shared<CallAudioRendererCallback>();
    if (callback_ == nullptr) {
        TELEPHONY_LOGE("callback_ is nullptr");
        return false;
    }
    audioRenderer_->SetRendererCallback(callback_);
    if (!audioRenderer_->Start()) {
        TELEPHONY_LOGE("audio renderer start failed");
        return false;
    }
    if (audioRenderer_->GetBufferSize(bufferLen)) {
        TELEPHONY_LOGE("audio renderer get buffer size failed");
        return false;
    }
    uint32_t frameCount;
    if (audioRenderer_->GetFrameCount(frameCount)) {
        TELEPHONY_LOGE("audio renderer get frame count failed");
        return false;
    }
    isRenderInitialized_ = true;
    return true;
}

bool AudioPlayer::InitRenderer()
{
    AudioStandard::AudioRendererOptions rendererOptions;
    rendererOptions.streamInfo.samplingRate = AudioStandard::AudioSamplingRate::SAMPLE_RATE_96000;
    rendererOptions.streamInfo.encoding = AudioStandard::AudioEncodingType::ENCODING_PCM;
    rendererOptions.streamInfo.format = AudioStandard::AudioSampleFormat::SAMPLE_U8;
    rendererOptions.streamInfo.channels = AudioStandard::AudioChannel::MONO;
    rendererOptions.rendererInfo.contentType = AudioStandard::ContentType::CONTENT_TYPE_SPEECH;
    rendererOptions.rendererInfo.streamUsage = AudioStandard::StreamUsage::STREAM_USAGE_VOICE_MODEM_COMMUNICATION;
    rendererOptions.rendererInfo.rendererFlags = RENDERER_FLAG;
    audioRenderer_ = AudioStandard::AudioRenderer::Create(rendererOptions);
    if (audioRenderer_ == nullptr) {
        TELEPHONY_LOGE("audio renderer create failed");
        return false;
    }
    audioRenderer_->SetInterruptMode(AudioStandard::INDEPENDENT_MODE);
    if (!audioRenderer_->Start()) {
        TELEPHONY_LOGE("audio renderer start failed");
        return false;
    }
    uint32_t frameCount;
    if (audioRenderer_->GetFrameCount(frameCount)) {
        TELEPHONY_LOGE("audio renderer get frame count failed");
        return false;
    }
    isRenderInitialized_ = true;
    return true;
}

bool AudioPlayer::InitCapturer()
{
    AudioStandard::AudioCapturerOptions capturerOptions;
    capturerOptions.streamInfo.samplingRate = AudioStandard::AudioSamplingRate::SAMPLE_RATE_96000;
    capturerOptions.streamInfo.encoding = AudioStandard::AudioEncodingType::ENCODING_PCM;
    capturerOptions.streamInfo.format = AudioStandard::AudioSampleFormat::SAMPLE_U8;
    capturerOptions.streamInfo.channels = AudioStandard::AudioChannel::MONO;
    capturerOptions.capturerInfo.sourceType = AudioStandard::SourceType::SOURCE_TYPE_VOICE_MODEM_COMMUNICATION;
    capturerOptions.capturerInfo.capturerFlags = CAPTURER_FLAG;
    audioCapturer_ = AudioStandard::AudioCapturer::Create(capturerOptions);
    if (audioCapturer_ == nullptr) {
        TELEPHONY_LOGE("audio capturer create failed");
        return false;
    }
    if (!audioCapturer_->Start()) {
        TELEPHONY_LOGE("audio capturer start failed");
        return false;
    }
    uint32_t frameCount;
    if (audioCapturer_->GetFrameCount(frameCount)) {
        TELEPHONY_LOGE("audio capturer get frame count failed");
        return false;
    }
    isCapturerInitialized_ = true;
    return true;
}

int32_t AudioPlayer::Play(const std::string &path, AudioStandard::AudioStreamType streamType, PlayerType playerType)
{
    wav_hdr wavHeader;
    std::string realPath = "";
    if (!GetRealPath(path, realPath) || realPath.empty()) {
        TELEPHONY_LOGE("path or realPath is NULL");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    FILE *wavFile = fopen(realPath.c_str(), "rb");
    if (wavFile == nullptr) {
        TELEPHONY_LOGE("open audio file failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    (void)fread(&wavHeader, READ_SIZE, sizeof(wav_hdr), wavFile);
    SetStop(playerType, false);
    if (!InitRenderer(wavHeader, streamType)) {
        TELEPHONY_LOGE("audio renderer and capturer init failed");
        (void)fclose(wavFile);
        return TELEPHONY_ERR_UNINIT;
    }
    uint8_t *buffer = (uint8_t *)malloc(bufferLen + bufferLen);
    if (buffer == nullptr) {
        TELEPHONY_LOGE("audio malloc buffer failed");
        (void)fclose(wavFile);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    size_t bytesToWrite = 0, bytesWritten = 0;
    TELEPHONY_LOGI("start audio rendering");
    while (!isStop_) {
        if (IsStop(playerType)) {
            break;
        } else if (feof(wavFile)) {
            fseek(wavFile, 0, SEEK_SET); // jump back to the beginning of the file
            fread(&wavHeader, READ_SIZE, sizeof(wav_hdr), wavFile); // skip the wav header
        }
        bytesToWrite = fread(buffer, READ_SIZE, bufferLen, wavFile);
        bytesWritten = 0;
        while ((bytesWritten < bytesToWrite) && ((bytesToWrite - bytesWritten) > MIN_BYTES)) {
            if (IsStop(playerType)) {
                break;
            }
            bytesWritten += static_cast<size_t>(
                    audioRenderer_->Write(buffer + bytesWritten, bytesToWrite - bytesWritten));
        }
    }
    free(buffer);
    (void)fclose(wavFile);
    TELEPHONY_LOGI("audio renderer playback done");
    return TELEPHONY_SUCCESS;
}

int32_t AudioPlayer::Play(PlayerType playerType)
{
    SetStop(playerType, false);
    if (!InitRenderer() || !InitCapturer()) {
        TELEPHONY_LOGE("audio renderer and capturer init failed");
        return TELEPHONY_ERR_UNINIT;
    }
    return TELEPHONY_SUCCESS;
}

void AudioPlayer::SetStop(PlayerType playerType, bool state)
{
    switch (playerType) {
        case PlayerType::TYPE_RING:
            isRingStop_ = state;
            if (isRingStop_) {
                DelayedSingleton<AudioControlManager>::GetInstance()->SetRingState(RingState::STOPPED);
            } else {
                DelayedSingleton<AudioControlManager>::GetInstance()->SetRingState(RingState::RINGING);
            }
            break;
        case PlayerType::TYPE_TONE:
            isToneStop_ = state;
            if (isToneStop_) {
                DelayedSingleton<AudioControlManager>::GetInstance()->SetToneState(ToneState::STOPPED);
            } else {
                DelayedSingleton<AudioControlManager>::GetInstance()->SetToneState(ToneState::TONEING);
            }
            break;
        case PlayerType::TYPE_SOUND:
            isSoundStop_ = state;
            if (isSoundStop_) {
                DelayedSingleton<AudioControlManager>::GetInstance()->SetSoundState(SoundState::STOPPED);
            } else {
                DelayedSingleton<AudioControlManager>::GetInstance()->SetSoundState(SoundState::SOUNDING);
            }
            break;
        default:
            break;
    }
}

bool AudioPlayer::IsStop(PlayerType playerType)
{
    bool ret = false;
    switch (playerType) {
        case PlayerType::TYPE_RING:
            ret = isRingStop_;
            break;
        case PlayerType::TYPE_TONE:
            ret = isToneStop_;
            break;
        case PlayerType::TYPE_SOUND:
            ret = isSoundStop_;
            break;
        default:
            break;
    }
    return ret;
}

void AudioPlayer::ReleaseRenderer()
{
    if (!isRenderInitialized_) {
        return;
    }
    audioRenderer_->Flush();
    audioRenderer_->Drain();
    audioRenderer_->Stop();
    audioRenderer_->Release();
}

void AudioPlayer::ReleaseCapturer()
{
    if (!isCapturerInitialized_) {
        return;
    }
    audioCapturer_->Flush();
    audioCapturer_->Stop();
    audioCapturer_->Release();
}

bool AudioPlayer::GetRealPath(const std::string &profilePath, std::string &realPath)
{
    if (profilePath.empty()) {
        TELEPHONY_LOGE("profilePath is empty");
        return false;
    }

    char path[PATH_MAX] = { '\0' };
    if (realpath(profilePath.c_str(), path) == nullptr) {
        TELEPHONY_LOGE("get real path fail");
        return false;
    }
    realPath = path;
    return true;
}

void AudioPlayer::CallAudioRendererCallback::OnInterrupt(const AudioStandard::InterruptEvent &interruptEvent)
{
    if (interruptEvent.forceType == AudioStandard::INTERRUPT_FORCE) {
        switch (interruptEvent.hintType) {
            case AudioStandard::INTERRUPT_HINT_PAUSE:
                break;
            case AudioStandard::INTERRUPT_HINT_STOP:
                if (DelayedSingleton<AudioControlManager>::GetInstance()->IsCurrentRinging() &&
                    DelayedSingleton<AudioControlManager>::GetInstance()->IsSoundPlaying()) {
                    DelayedSingleton<AudioControlManager>::GetInstance()->StopRingtone();
                }
                break;
            default:
                break;
        }
    }
}

void AudioPlayer::RingCallback::OnInterrupt(const AudioStandard::InterruptEvent &interruptEvent)
{
    if (interruptEvent.forceType == AudioStandard::INTERRUPT_FORCE) {
        switch (interruptEvent.hintType) {
            case AudioStandard::INTERRUPT_HINT_PAUSE:
                break;
            case AudioStandard::INTERRUPT_HINT_STOP:
                if (DelayedSingleton<AudioControlManager>::GetInstance()->IsCurrentRinging() &&
                    DelayedSingleton<AudioControlManager>::GetInstance()->IsSoundPlaying()) {
                    DelayedSingleton<AudioControlManager>::GetInstance()->StopRingtone();
                }
                break;
            default:
                break;
        }
    }
}

void AudioPlayer::RegisterRingCallback(std::shared_ptr<Media::RingtonePlayer> &RingtonePlayer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ringCallback_ = std::make_shared<RingCallback>();
    if (ringCallback_ == nullptr) {
        TELEPHONY_LOGE("ringCallback_ is nullptr");
        return;
    }
    if (RingtonePlayer == nullptr) {
        TELEPHONY_LOGE("RingtonePlayer is nullptr");
        return;
    }
    RingtonePlayer->SetRingtonePlayerInterruptCallback(ringCallback_);
}

int32_t AudioPlayer::SetMute()
{
    if (audioRenderer_ == nullptr) {
        TELEPHONY_LOGE("audioRenderer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    float volume = 0.0f;
    return audioRenderer_->SetVolume(volume);
}
} // namespace Telephony
} // namespace OHOS
