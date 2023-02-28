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

#ifndef TELEPHONY_AUDIO_DEVICE_MANAGER_H
#define TELEPHONY_AUDIO_DEVICE_MANAGER_H

#include <map>

#include "audio_base.h"
#include "call_manager_inner_type.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
/**
 * @class AudioDeviceManager
 * describes the available devices of a call.
 */
class AudioDeviceManager : public std::enable_shared_from_this<AudioDeviceManager> {
    DECLARE_DELAYED_SINGLETON(AudioDeviceManager)
public:
    void Init();
    bool InitAudioDevice();
    bool ProcessEvent(AudioEvent event);
    static bool IsEarpieceAvailable();
    static bool IsSpeakerAvailable();
    static bool IsBtScoConnected();
    static bool IsWiredHeadsetConnected();
    static void SetDeviceAvailable(AudioDeviceType deviceType, bool available);
    bool ConnectBtScoWithAddress(const std::string &bluetoothAddress);
    bool SwitchDevice(AudioDeviceType device);
    void AddAudioDeviceList(const std::string &address, AudioDeviceType deviceType);
    void RemoveAudioDeviceList(const std::string &address, AudioDeviceType deviceType);
    int32_t ReportAudioDeviceChange();

private:
    std::mutex mutex_;
    AudioDeviceType audioDeviceType_;
    static bool isBtScoDevEnable_;
    bool isWiredHeadsetDevEnable_ = false;
    bool isSpeakerDevEnable_ = false;
    bool isEarpieceDevEnable_ = false;
    std::unique_ptr<AudioBase> currentAudioDevice_;
    static bool isEarpieceAvailable_;
    static bool isSpeakerAvailable_;
    static bool isWiredHeadsetConnected_;
    static bool isBtScoConnected_;
    bool isAudioActivated_;
    using AudioDeviceManagerFunc = bool (AudioDeviceManager::*)();
    std::map<uint32_t, AudioDeviceManagerFunc> memberFuncMap_;
    AudioDeviceInfo info_;
    bool SwitchDevice(AudioEvent event);
    bool EnableBtSco();
    bool EnableWiredHeadset();
    bool EnableSpeaker();
    bool EnableEarpiece();
    bool DisableAll();
    bool IsBtScoDevEnable();
    bool IsSpeakerDevEnable();
    bool IsEarpieceDevEnable();
    bool IsWiredHeadsetDevEnable();
    AudioDeviceType GetCurrentAudioDevice();
    void SetCurrentAudioDevice(AudioDeviceType deviceType);
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_DEVICE_MANAGER_H