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

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "audio_control_manager.h"

#include "audio_system_manager.h"

namespace OHOS {
namespace Telephony {
size_t AudioPlayer::bufferLen = 0;
bool AudioPlayer::isStop_ = false;
bool AudioPlayer::isRingStop_ = false;
bool AudioPlayer::isToneStop_ = false;
bool AudioPlayer::isDtmfStop_ = false;

bool AudioPlayer::Init(const std::unique_ptr<AudioRenderer> &audioRenderer, const wav_hdr &wavHeader)
{
    AudioRendererParams rendererParams;
    rendererParams.sampleFormat = static_cast<AudioSampleFormat>(wavHeader.bitsPerSample);
    rendererParams.sampleRate = static_cast<AudioSamplingRate>(wavHeader.SamplesPerSec);
    rendererParams.channelCount = static_cast<AudioChannel>(wavHeader.NumOfChan);
    rendererParams.encodingType = static_cast<AudioEncodingType>(ENCODING_PCM);
    if (audioRenderer == nullptr) {
        TELEPHONY_LOGE("audio renderer nullptr");
        return false;
    }
    if (audioRenderer->SetParams(rendererParams) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("set params failed");
        return false;
    }
    if (!audioRenderer->Start()) {
        TELEPHONY_LOGE("audio renderer start failed");
        return false;
    }
    if (audioRenderer->GetBufferSize(bufferLen)) {
        TELEPHONY_LOGE("audio renderer get buffer size failed");
        return TELEPHONY_ERR_FAIL;
    }
    uint32_t frameCount;
    if (audioRenderer->GetFrameCount(frameCount)) {
        TELEPHONY_LOGE("audio renderer get frame count failed");
        return false;
    }
    return true;
}

int32_t AudioPlayer::Play(const std::string &path, AudioStreamType audioStreamType, PlayerType playerType)
{
    wav_hdr wavHeader;
    FILE *wavFile = fopen(path.c_str(), "rb");
    if (wavFile == nullptr) {
        TELEPHONY_LOGE("open audio file failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    (void)fread(&wavHeader, READ_SIZE, sizeof(wav_hdr), wavFile);
    std::unique_ptr<AudioRenderer> audioRenderer = AudioRenderer::Create(audioStreamType);
    if (audioRenderer == nullptr) {
        TELEPHONY_LOGE("audio renderer nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!Init(audioRenderer, wavHeader)) {
        TELEPHONY_LOGE("audio renderer init failed");
        return TELEPHONY_ERR_FAIL;
    }
    uint8_t *buffer = (uint8_t *)malloc(bufferLen + bufferLen);
    if (buffer == nullptr) {
        TELEPHONY_LOGE("audio malloc buffer failed");
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
            bytesWritten += audioRenderer->Write(buffer + bytesWritten, bytesToWrite - bytesWritten);
        }
    }
    Release(audioRenderer);
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
        case PlayerType::TYPE_DTMF:
            isDtmfStop_ = state;
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
        case PlayerType::TYPE_DTMF:
            ret = isDtmfStop_;
            break;
        default:
            break;
    }
    return ret;
}

void AudioPlayer::Release(const std::unique_ptr<AudioRenderer> &audioRenderer)
{
    audioRenderer->Flush();
    audioRenderer->Drain();
    audioRenderer->Stop();
    audioRenderer->Release();
}
} // namespace Telephony
} // namespace OHOS