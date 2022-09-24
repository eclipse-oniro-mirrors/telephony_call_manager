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
size_t AudioPlayer::bufferLen = 0;
bool AudioPlayer::isStop_ = false;
bool AudioPlayer::isRingStop_ = false;
bool AudioPlayer::isToneStop_ = false;
std::unique_ptr<AudioStandard::AudioRenderer> AudioPlayer::audioRenderer_ = nullptr;

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
    if (audioRenderer_->SetParams(rendererParams) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("audio renderer set params failed");
        return false;
    }
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
    if (!InitRenderer(wavHeader, streamType)) {
        TELEPHONY_LOGE("audio renderer init failed");
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
    SetStop(playerType, false);
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
        default:
            break;
    }
    return ret;
}

void AudioPlayer::ReleaseRenderer()
{
    audioRenderer_->Flush();
    audioRenderer_->Drain();
    audioRenderer_->Stop();
    audioRenderer_->Release();
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
} // namespace Telephony
} // namespace OHOS
