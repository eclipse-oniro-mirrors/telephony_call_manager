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

#ifndef TELEPHONY_AUDIO_PLAYER_H
#define TELEPHONY_AUDIO_PLAYER_H
#include <cstdint>
#include <string>

#include "audio_capturer.h"
#include "audio_renderer.h"
#include "ringtone_player.h"
#include "system_sound_manager.h"

namespace OHOS {
namespace Telephony {
enum PlayerType {
    TYPE_RING = 0,
    TYPE_TONE,
    TYPE_DTMF,
    TYPE_SOUND,
};

struct wav_hdr {
    /* RIFF Chunk Descriptor */
    uint8_t RIFF[4] = {'R', 'I', 'F', 'F'}; // RIFF Header Magic header
    uint32_t ChunkSize = 0; // RIFF Chunk Size
    uint8_t WAVE[4] = {'W', 'A', 'V', 'E'}; // WAVE Header
    /* "fmt" sub-chunk */
    uint8_t fmt[4] = {'f', 'm', 't', ' '}; // FMT header
    uint32_t Subchunk1Size = 16; // Size of the fmt chunk
    uint16_t AudioFormat = 1; // Audio format 1=PCM
    uint16_t NumOfChan = 2; // Number of channels 1=Mono 2=Stereo
    uint32_t SamplesPerSec = 44100; // Sampling Frequency in Hz
    uint32_t bytesPerSec = 176400; // bytes per second
    uint16_t blockAlign = 4; // 2=16-bit mono, 4=16-bit stereo
    /* "INFO" sub-chunk */
    uint16_t SubchunkInfo1 = 16;
    uint8_t LISTB[5] = {'L', 'I', 'S', 'T', 'B'};
    uint16_t SubchunkInfo2 = 0;
    uint8_t SubchunkInfo3 = 0;
    uint8_t INFOICRD[8] = {'I', 'N', 'F', 'O', 'I', 'C', 'R', 'D'};
    uint32_t SubchunkInfo4 = 11;
    uint8_t TIME[10] = {'2', '0', '2', '0', '-', '0', '9', '-', '0', '4'};
    uint16_t SubchunkInfo5 = 0;
    uint8_t ISFT[4] = {'I', 'S', 'F', 'T'};
    uint32_t SubchunkInfo6 = 14;
    uint8_t Lavf[18] = {'L', 'a', 'v', 'f', '5', '8', ' ', '7', '6', ' ', '1', '0', '0', ' ', 'I', 'T', 'C', 'H'};
    uint32_t SubchunkInfo7 = 12;
    uint8_t Logic[12] = {'L', 'o', 'g', 'i', 'c', ' ', 'P', 'r', 'o', ' ', 'X', ' '};
    /* "data" sub-chunk */
    uint8_t Subchunk2ID[4] = {'d', 'a', 't', 'a'}; // "data"  string
    uint32_t Subchunk2Size = 0; // Sampled data length
};

class AudioPlayer {
public:
    AudioPlayer() = default;
    ~AudioPlayer() = default;
    bool InitRenderer(const wav_hdr &wavHeader, AudioStandard::AudioStreamType streamType);
    bool InitRenderer();
    bool InitCapturer();
    int32_t Play(const std::string &path, AudioStandard::AudioStreamType streamType, PlayerType playerType);
    int32_t Play(PlayerType playerType);
    void ReleaseRenderer();
    void ReleaseCapturer();
    void SetStop(PlayerType playerType, bool state);
    int32_t SetMute();
    void RegisterRingCallback(std::shared_ptr<Media::RingtonePlayer> &RingtonePlayer);

private:
    class CallAudioRendererCallback : public AudioStandard::AudioRendererCallback {
    public:
        void OnInterrupt(const AudioStandard::InterruptEvent &interruptEvent) override;
        void OnStateChange(const AudioStandard::RendererState state,
            const AudioStandard::StateChangeCmdType cmdType) override {}
    };
    class RingCallback : public Media::RingtonePlayerInterruptCallback {
    public:
        void OnInterrupt(const AudioStandard::InterruptEvent &interruptEvent) override;
    };

private:
    static constexpr uint16_t READ_SIZE = 1;
    static constexpr uint16_t MIN_BYTES = 4;
    static constexpr uint16_t RENDERER_FLAG = 0;
    static constexpr uint16_t CAPTURER_FLAG = 0;
    size_t bufferLen = 0;
    bool isStop_ = false;
    bool isRingStop_ = false;
    bool isRenderInitialized_ = false;
    bool isCapturerInitialized_ = false;
    bool isToneStop_ = false;
    bool isSoundStop_ = false;
    ffrt::ffrt mutex_;
    std::shared_ptr<AudioStandard::AudioRendererCallback> callback_ = nullptr;
    std::shared_ptr<Media::RingtonePlayerInterruptCallback> ringCallback_;
    bool IsStop(PlayerType playerType);
    std::unique_ptr<AudioStandard::AudioRenderer> audioRenderer_ = nullptr;
    std::unique_ptr<AudioStandard::AudioCapturer> audioCapturer_ = nullptr;
    bool GetRealPath(const std::string &profilePath, std::string &realPath);
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_AUDIO_PLAYER_H
