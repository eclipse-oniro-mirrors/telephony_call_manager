/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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

#include "distributed_sink_switch_controller.h"

#include <chrono>
#include "distributed_communication_manager.h"
#include "audio_device_manager.h"
#include "audio_control_manager.h"
#include "bluetooth_connection.h"
#ifdef ABILITY_BLUETOOTH_SUPPORT
#include "bluetooth_device.h"
#endif

namespace OHOS {
namespace Telephony {
constexpr int32_t DEFAULT_HFP_FLAG_VALUE = -1;
constexpr int32_t WEAR_ACTION = 0;
constexpr int32_t UNWEAR_ACTION = 1;
constexpr int32_t ENABLE_FROM_REMOTE_ACTION = 2;
constexpr int32_t DISABLE_FROM_REMOTE_ACTION = 3;
using namespace std::chrono;
void DistributedSinkSwitchController::OnDeviceOnline(const std::string &devId, const std::string &devName,
    AudioDeviceType devType)
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (hfpListener_ == nullptr) {
        hfpListener_ = std::make_shared<DcCallHfpListener>();
        Bluetooth::HandsFreeAudioGateway::GetProfile()->RegisterObserver(hfpListener_);
    }
#endif
}

void DistributedSinkSwitchController::OnDeviceOffline(const std::string &devId, const std::string &devName,
    AudioDeviceType devType)
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (hfpListener_ != nullptr) {
        Bluetooth::HandsFreeAudioGateway::GetProfile()->DeregisterObserver(hfpListener_);
        hfpListener_ = nullptr;
    }
#endif
}

void DistributedSinkSwitchController::OnDistributedAudioDeviceChange(const std::string &devId,
    const std::string &devName, AudioDeviceType devType, int32_t devRole)
{
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        isAudioOnSink_ = (devRole == static_cast<int32_t>(DistributedRole::SINK));
        TELEPHONY_LOGI("OnDistributedAudioDeviceChange isAudioOnSink[%{public}d]", isAudioOnSink_);
    }
    if (devRole == static_cast<int32_t>(DistributedRole::SINK)) {
        TrySwitchToBtHeadset();
#ifdef ABILITY_BLUETOOTH_SUPPORT
        if (hfpListener_ != nullptr &&
            !DelayedSingleton<BluetoothConnection>::GetInstance()->GetWearBtHeadsetAddress().empty()) {
            hfpListener_->SetPreAction(WEAR_ACTION);
        }
#endif
    }
}

void DistributedSinkSwitchController::OnRemoveSystemAbility()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    isAudioOnSink_ = false;
#ifdef ABILITY_BLUETOOTH_SUPPORT
    if (hfpListener_ != nullptr) {
        Bluetooth::HandsFreeAudioGateway::GetProfile()->DeregisterObserver(hfpListener_);
        hfpListener_ = nullptr;
    }
#endif
}

void DistributedSinkSwitchController::TrySwitchToBtHeadset()
{
#ifdef ABILITY_BLUETOOTH_SUPPORT
    auto address = DelayedSingleton<BluetoothConnection>::GetInstance()->GetWearBtHeadsetAddress();
    if (address.empty()) {
        return;
    }

    AudioDevice audioDevice;
    audioDevice.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
    if (memcpy_s(audioDevice.address, kMaxAddressLen, address.c_str(), address.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s fail");
        return;
    }
    auto ret = DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioDevice(audioDevice);
    TELEPHONY_LOGI("set bt headset ret[%{public}d]", ret);
#endif
}

#ifdef ABILITY_BLUETOOTH_SUPPORT
void DcCallHfpListener::OnHfpStackChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t action)
{
    TELEPHONY_LOGI("dc call hfp stack changed, action[%{public}d], preAction_[%{public}d]", action, preAction_);
    int32_t cod = DEFAULT_HFP_FLAG_VALUE;
    int32_t majorClass = DEFAULT_HFP_FLAG_VALUE;
    int32_t majorMinorClass = DEFAULT_HFP_FLAG_VALUE;
    device.GetDeviceProductType(cod, majorClass, majorMinorClass);
    bool isBtHeadset = (majorClass == Bluetooth::BluetoothDevice::MAJOR_AUDIO_VIDEO &&
                        (majorMinorClass == Bluetooth::BluetoothDevice::AUDIO_VIDEO_HEADPHONES ||
                         majorMinorClass == Bluetooth::BluetoothDevice::AUDIO_VIDEO_WEARABLE_HEADSET));
    if (!isBtHeadset) {
        return;
    }
    if (!DelayedSingleton<DistributedCommunicationManager>::GetInstance()->IsConnected() ||
        !DelayedSingleton<DistributedCommunicationManager>::GetInstance()->IsAudioOnSink()) {
        TELEPHONY_LOGI("dc not connected or audio not on sink");
        preAction_ = action;
        return;
    }
    if ((action == WEAR_ACTION) || (action == DISABLE_FROM_REMOTE_ACTION && preAction_ == WEAR_ACTION)) {
        // Forcibly switch to the bt headset in either of the following situations:
        // case 1: At the moment, the user wears the headset
        // case 2: The headset is preempted by the peer device when the user wears the headset
        SwitchToBtHeadset(device);
    }
    preAction_ = action;
}

void DcCallHfpListener::SetPreAction(int32_t action)
{
    preAction_ = action;
}

void DcCallHfpListener::SwitchToBtHeadset(const Bluetooth::BluetoothRemoteDevice &device)
{
    AudioDevice audioDevice;
    audioDevice.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
    if (memcpy_s(audioDevice.address, kMaxAddressLen, device.GetDeviceAddr().c_str(),
        device.GetDeviceAddr().length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s address fail");
        return;
    }
    auto ret = DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioDevice(audioDevice);
    TELEPHONY_LOGI("switch to bt headset, ret[%{public}d]", ret);
}
#endif

} // namespace Telephony
} // namespace OHOS
