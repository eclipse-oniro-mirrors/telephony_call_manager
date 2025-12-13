/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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
#include "call_base.h"
#include "call_manager_inner_type.h"
#include "ffrt.h"
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
    static bool IsBtActived();
    static bool IsNearlinkActived(AudioDevice &device);
#ifdef SUPPORT_HEARING_AID
    static bool IsBtHearingAidActived(AudioDevice &device);
#endif
    static bool IsDistributedCallConnected();
    static bool IsWiredHeadsetConnected();
    static void SetDeviceAvailable(AudioDeviceType deviceType, bool available);
    static bool SetVirtualCall(bool isVirtual);
    static bool GetVirtualCall();
    static bool IsRemoteDevicesConnected();

    bool SwitchDevice(AudioDeviceType device, bool isSetAudioDeviceByUser = false);
    void AddAudioDeviceList(const std::string &address, AudioDeviceType deviceType, const std::string &deviceName);
    void RemoveAudioDeviceList(const std::string &address, AudioDeviceType deviceType);
    void ResetBtAudioDevicesList();
    void ResetDistributedCallDevicesList();
    void ResetNearlinkAudioDevicesList();
#ifdef SUPPORT_HEARING_AID
    void ResetBtHearingAidDeviceList();
#endif
    int32_t ReportAudioDeviceChange(const AudioDevice &device);
    int32_t CopyDeviceData(const std::string &address, const std::string &deviceName);
    bool CheckAndSwitchDistributedAudioDevice();
    int32_t ReportAudioDeviceInfo();
    int32_t ReportAudioDeviceInfo(sptr<CallBase> call);
    void SetCurrentAudioDevice(AudioDeviceType deviceType);
    void SetCurrentAudioDevice(const AudioDevice &device);
    void OnActivedCallDisconnected();
    void SetMuteState(bool isMuted);
    bool GetMuteState();
    std::string ConvertAddress();

    /**
     * UpdateEarpieceDevice
     *
     * @brief update earpiece device
     */
    void UpdateEarpieceDevice();

    /**
     * @brief Update bluetooth device name
     *
     * @param macAddress[in], mac address
     * @param deviceName[in], device name
     */
    void UpdateBluetoothDeviceName(const std::string &macAddress, const std::string &deviceName);

    /**
     * @brief Update nearlink device name
     *
     * @param macAddress[in], mac address
     * @param deviceName[in], device name
     */
    void UpdateNearlinkDeviceName(const std::string &macAddress, const std::string &deviceName);

    AudioDeviceType GetCurrentAudioDevice();

private:
    bool EnableBtSco();
    bool EnableNearlink();
#ifdef SUPPORT_HEARING_AID
    bool EnableBtHearingAid();
#endif
    bool EnableWiredHeadset(bool isSetAudioDeviceByUser);
    bool EnableSpeaker(bool isSetAudioDeviceByUser);
    bool EnableEarpiece(bool isSetAudioDeviceByUser);
    bool DisableAll();
    bool IsBtScoDevEnable();
    bool IsDCallDevEnable();
    bool IsSpeakerDevEnable();
    bool IsEarpieceDevEnable();
    bool IsWiredHeadsetDevEnable();
    bool IsDistributedAudioDeviceType(AudioDeviceType deviceType);
    bool IsSupportEarpiece();
    void AddEarpiece();
    void UpdateBtDevice(std::string &address, std::string &deviceName);
    void UpdateNearlinkDevice(std::string &address, std::string &deviceName);
#ifdef SUPPORT_HEARING_AID
    void UpdateBtHearingAidDevice(std::string &address, std::string &deviceName);
#endif
    bool UpdateDeviceName(const std::string &macAddress, const std::string &deviceName, AudioDeviceType deviceType);
    ffrt::mutex mutex_;
    ffrt::mutex infoMutex_;
    AudioDeviceType audioDeviceType_;
    static bool isBtScoDevEnable_;
    static bool isDCallDevEnable_;
    bool isWiredHeadsetDevEnable_ = false;
    bool isSpeakerDevEnable_ = false;
    bool isEarpieceDevEnable_ = false;
    std::unique_ptr<AudioBase> currentAudioDevice_;
    static bool isEarpieceAvailable_;
    static bool isSpeakerAvailable_;
    static bool isUpdateEarpieceDevice_;
    static bool isWiredHeadsetConnected_;
    static bool isBtScoConnected_;
    static bool isDCallDevConnected_;
    bool isAudioActivated_;
    AudioDeviceInfo info_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_DEVICE_MANAGER_H