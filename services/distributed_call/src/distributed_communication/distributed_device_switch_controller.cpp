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

#include "distributed_device_switch_controller.h"
#include "cJSON.h"
#include "audio_proxy.h"
#include "telephony_errors.h"
#include "audio_device_manager.h"
#include "telephony_log_wrapper.h"
#include "distributed_communication_manager.h"

namespace OHOS {
namespace Telephony {
void DistributedDeviceSwitchController::OnDeviceOnline(const std::string &devId, const std::string &devName,
    AudioDeviceType devType)
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    if (audioDeviceManager != nullptr) {
        std::string address = GetDevAddress(devId, devName);
        audioDeviceManager->AddAudioDeviceList(address, devType, devName);
    }
}

void DistributedDeviceSwitchController::OnDeviceOffline(const std::string &devId, const std::string &devName,
    AudioDeviceType devType)
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    if (audioDeviceManager != nullptr) {
        std::string address = GetDevAddress(devId, devName);
        audioDeviceManager->RemoveAudioDeviceList(address, devType);
        audioDeviceManager->InitAudioDevice();
    }
}

void DistributedDeviceSwitchController::OnDistributedAudioDeviceChange(const std::string &devId,
    const std::string &devName, AudioDeviceType devType, int32_t devRole)
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    if (audioDeviceManager != nullptr) {
        if (devRole == static_cast<int32_t>(DistributedRole::SINK)) {
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

    std::lock_guard<ffrt::mutex> lock(mutex_);
    isAudioOnSink_ = (devRole == static_cast<int32_t>(DistributedRole::SINK));
    TELEPHONY_LOGI("OnDistributedAudioDeviceChange isAudioOnSink[%{public}d]", isAudioOnSink_);
}

void DistributedDeviceSwitchController::OnRemoveSystemAbility()
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    if (audioDeviceManager != nullptr) {
        audioDeviceManager->ResetDistributedCallDevicesList();
        audioDeviceManager->InitAudioDevice();
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    isAudioOnSink_ = false;
}

bool DistributedDeviceSwitchController::SwitchDevice(const std::string &devId, int32_t direction)
{
    auto distributedMgr = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    if (distributedMgr == nullptr) {
        return false;
    }
    auto ret = distributedMgr->SwitchDevWrapper(devId, direction);
    TELEPHONY_LOGI("switch distributed device result[%{public}d]", ret);
    return ret == TELEPHONY_SUCCESS;
}

bool DistributedDeviceSwitchController::IsAudioOnSink()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return isAudioOnSink_;
}

std::string DistributedDeviceSwitchController::GetDevAddress(const std::string &devId, const std::string &devName)
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

} // namespace Telephony
} // namespace OHOS
