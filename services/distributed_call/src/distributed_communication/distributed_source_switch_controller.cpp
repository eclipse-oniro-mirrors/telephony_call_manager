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

#include "distributed_source_switch_controller.h"
#include "cJSON.h"
#ifdef ABILITY_BLUETOOTH_SUPPORT
#include "bluetooth_device.h"
#endif
#include "audio_proxy.h"
#include "telephony_errors.h"
#include "audio_device_manager.h"
#include "telephony_log_wrapper.h"
#include "distributed_communication_manager.h"

namespace OHOS {
namespace Telephony {
void DistributedSourceSwitchController::OnDeviceOnline(const std::string &devId, const std::string &devName,
    AudioDeviceType devType)
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    if (audioDeviceManager != nullptr) {
        std::string address = GetDevAddress(devId, devName);
        audioDeviceManager->AddAudioDeviceList(address, devType, devName);
    }

#ifdef ABILITY_BLUETOOTH_SUPPORT
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (hfpListener_ == nullptr) {
        hfpListener_ = std::make_shared<DcCallSourceHfpListener>();
        Bluetooth::HandsFreeAudioGateway::GetProfile()->RegisterObserver(hfpListener_);
    }
#endif
}

void DistributedSourceSwitchController::OnDeviceOffline(const std::string &devId, const std::string &devName,
    AudioDeviceType devType)
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    if (audioDeviceManager != nullptr) {
        std::string address = GetDevAddress(devId, devName);
        audioDeviceManager->RemoveAudioDeviceList(address, devType);
        audioDeviceManager->InitAudioDevice();
    }

#ifdef ABILITY_BLUETOOTH_SUPPORT
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (hfpListener_ != nullptr) {
        Bluetooth::HandsFreeAudioGateway::GetProfile()->DeregisterObserver(hfpListener_);
        hfpListener_ = nullptr;
    }
#endif
}

void DistributedSourceSwitchController::OnDistributedAudioDeviceChange(const std::string &devId,
    const std::string &devName, AudioDeviceType devType, int32_t devRole)
{
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        isAudioOnSink_ = (devRole == static_cast<int32_t>(DistributedRole::SINK));
        TELEPHONY_LOGI("OnDistributedAudioDeviceChange isAudioOnSink[%{public}d]", isAudioOnSink_);
    }
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    if (audioDeviceManager != nullptr) {
        if (devRole == static_cast<int32_t>(DistributedRole::SINK)) {
            if (AudioStandard::AudioSystemManager::GetInstance()->IsDeviceActive(
                    AudioStandard::DeviceType::DEVICE_TYPE_BLUETOOTH_SCO)) { // deactive bt if switch from bt to sink
                AudioStandard::AudioSystemManager::GetInstance()->SetDeviceActive(
                    AudioStandard::DeviceType::DEVICE_TYPE_BLUETOOTH_SCO, false);
            }
            std::string address = GetDevAddress(devId, devName);
            AudioDevice targetDevice = {
                .deviceType = devType,
            };
            if (memcpy_s(targetDevice.address, kMaxAddressLen, address.c_str(), address.length()) != EOK ||
                memcpy_s(targetDevice.deviceName, kMaxDeviceNameLen, devName.c_str(), devName.length()) != EOK) {
                TELEPHONY_LOGE("memcpy_s address or deviceName failed.");
                return;
            }
            audioDeviceManager->SetCurrentAudioDevice(targetDevice);
        } else {
            AudioDevice audioDevice;
            if (DelayedSingleton<AudioProxy>::GetInstance()->GetPreferredOutputAudioDevice(audioDevice) !=
                TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("get preferred output audio device fail");
                return;
            }
            audioDeviceManager->SetCurrentAudioDevice(audioDevice.deviceType);
        }
    }
}

void DistributedSourceSwitchController::OnRemoveSystemAbility()
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    if (audioDeviceManager != nullptr) {
        audioDeviceManager->ResetDistributedCallDevicesList();
        audioDeviceManager->InitAudioDevice();
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    isAudioOnSink_ = false;
#ifdef ABILITY_BLUETOOTH_SUPPORT
    if (hfpListener_ != nullptr) {
        Bluetooth::HandsFreeAudioGateway::GetProfile()->DeregisterObserver(hfpListener_);
        hfpListener_ = nullptr;
    }
#endif
}

bool DistributedSourceSwitchController::SwitchDevice(const std::string &devId, int32_t direction)
{
    auto distributedMgr = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    if (distributedMgr == nullptr) {
        return false;
    }
    auto ret = distributedMgr->SwitchDevWrapper(devId, direction);
    TELEPHONY_LOGI("switch distributed device result[%{public}d]", ret);
    return ret == TELEPHONY_SUCCESS;
}

std::string DistributedSourceSwitchController::GetDevAddress(const std::string &devId, const std::string &devName)
{
    std::string address = "";
    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return address;
    }
    do {
        if (cJSON_AddStringToObject(root, "devId", devId.c_str()) == nullptr) {
            TELEPHONY_LOGE("add dev id fail");
            break;
        }
        if (cJSON_AddStringToObject(root, "devName", devName.c_str()) == nullptr) {
            TELEPHONY_LOGE("add dev name fail");
            break;
        }
        char *jsonData = cJSON_PrintUnformatted(root);
        if (jsonData != nullptr) {
            address = jsonData;
            free(jsonData);
            jsonData = nullptr;
        }
    } while (false);
    cJSON_Delete(root);
    return address;
}

#ifdef ABILITY_BLUETOOTH_SUPPORT
void DcCallSourceHfpListener::OnHfpStackChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t action)
{
    TELEPHONY_LOGI("source hfp stack changed, action[%{public}d]", action);
    if (IsNeedSwitchToSource(device, action)) {
        (void)DelayedSingleton<DistributedCommunicationManager>::GetInstance()->SwitchToSourceDevice();
    }
}

bool DcCallSourceHfpListener::IsNeedSwitchToSource(const Bluetooth::BluetoothRemoteDevice &device, int32_t action)
{
    if (!DelayedSingleton<DistributedCommunicationManager>::GetInstance()->IsAudioOnSink()) {
        return false;
    }
    int32_t cod = DEFAULT_HFP_FLAG_VALUE;
    int32_t majorClass = DEFAULT_HFP_FLAG_VALUE;
    int32_t majorMinorClass = DEFAULT_HFP_FLAG_VALUE;
    device.GetDeviceProductType(cod, majorClass, majorMinorClass);
    TELEPHONY_LOGI("device major %{public}d, minor %{public}d", majorClass, majorMinorClass);
    if (action == WEAR_ACTION && majorClass == Bluetooth::BluetoothDevice::MAJOR_AUDIO_VIDEO &&
        (majorMinorClass == Bluetooth::BluetoothDevice::AUDIO_VIDEO_HEADPHONES ||
        majorMinorClass == Bluetooth::BluetoothDevice::AUDIO_VIDEO_WEARABLE_HEADSET)) {
        return true;
    }
    if (action == USER_SELECTION_ACTION && majorClass == Bluetooth::BluetoothDevice::MAJOR_WEARABLE &&
        majorMinorClass == Bluetooth::BluetoothDevice::WEARABLE_WRIST_WATCH) {
        return true;
    }
    return false;
}
#endif

} // namespace Telephony
} // namespace OHOS
