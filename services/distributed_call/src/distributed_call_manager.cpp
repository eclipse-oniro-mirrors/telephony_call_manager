/*
 * Copyright (C) 2023-2024 Huawei Device Co., Ltd.
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

#include <thread>
#include <pthread.h>
#include <dlfcn.h>

#include "distributed_call_manager.h"
#include "audio_control_manager.h"
#include "telephony_log_wrapper.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace OHOS {
namespace Telephony {
namespace {
const size_t INT32_MIN_ID_LENGTH = 3;
const size_t INT32_SHORT_ID_LENGTH = 20;
const size_t INT32_PLAINTEXT_LENGTH = 4;
const int32_t DCALL_SWITCH_DEVICE_TYPE_SOURCE = 0;
const int32_t DCALL_SWITCH_DEVICE_TYPE_SINK = 1;
const int32_t DISTRIBUTED_CALL_SOURCE_SA_ID = 9855;
const int32_t WAIT_DCALL_INIT_100MS = 100 * 1000;
const std::string CALLBACK_NAME = "telephony";
const std::string DISTRIBUTED_AUDIO_DEV_CAR = "dCar";
const std::string DISTRIBUTED_AUDIO_DEV_PHONE = "dPhone";
const std::string DISTRIBUTED_AUDIO_DEV_PAD = "dPad";
const std::string SWITCH_ON_DCALL_THREAD_NAME = "switch on dcall";

const int32_t DISTRIBUTED_COMMUNICATION_CALL_SA_ID = 66198;

std::string GetAnonyString(const std::string &value)
{
    std::string res;
    std::string tmpStr("******");
    size_t strLen = value.length();
    if (strLen < INT32_MIN_ID_LENGTH) {
        return tmpStr;
    }
    if (strLen <= INT32_SHORT_ID_LENGTH) {
        res += value[0];
        res += tmpStr;
        res += value[strLen - 1];
    } else {
        res.append(value, 0, INT32_PLAINTEXT_LENGTH);
        res += tmpStr;
        res.append(value, strLen - INT32_PLAINTEXT_LENGTH, INT32_PLAINTEXT_LENGTH);
    }
    return res;
}

bool IsDistributedAudioDevice(const AudioDevice& device)
{
    if ((device.deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_PHONE) ||
        (device.deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_PAD) ||
        (device.deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE)) {
        return true;
    }
    return false;
}
}

DistributedCallManager::DistributedCallManager()
{
    TELEPHONY_LOGI("DistributedCallManager constructed.");
}

DistributedCallManager::~DistributedCallManager()
{
    TELEPHONY_LOGI("DistributedCallManager destructed.");
}

void DistributedCallManager::Init()
{
    TELEPHONY_LOGI("Init start.");
    statusChangeListener_ = new (std::nothrow) DCallSystemAbilityListener();
    if (statusChangeListener_ == nullptr) {
        TELEPHONY_LOGE("failed to create statusChangeListener");
        return;
    }
    auto managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("get system ability manager error");
        return;
    }
    int32_t ret = managerPtr->SubscribeSystemAbility(DISTRIBUTED_CALL_SOURCE_SA_ID, statusChangeListener_);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("failed to subscribe dcall service SA: %{public}d", DISTRIBUTED_CALL_SOURCE_SA_ID);
        return;
    }
    InitDistributedCommunicationCall();
    TELEPHONY_LOGI("Init end.");
}

bool DistributedCallManager::CreateDAudioDevice(const std::string& devId, AudioDevice& device)
{
    if (!devId.length()) {
        TELEPHONY_LOGE("dcall devId is invalid");
        return false;
    }
    if (dcallProxy_ == nullptr) {
        TELEPHONY_LOGE("dcallProxy_ is nullptr");
        return false;
    }
    OHOS::DistributedHardware::DCallDeviceInfo devInfo;
    int32_t ret = dcallProxy_->GetDCallDeviceInfo(devId, devInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("get dcall device info failed.");
        return false;
    }
    std::string devTypeName;
    std::string devName = devInfo.devName;
    if (devInfo.devType == OHOS::DistributedHardware::DCallDeviceType::DISTRIBUTED_DEVICE_PHONE) {
        devTypeName = DISTRIBUTED_AUDIO_DEV_PHONE;
        device.deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    } else if (devInfo.devType == OHOS::DistributedHardware::DCallDeviceType::DISTRIBUTED_DEVICE_PAD) {
        devTypeName = DISTRIBUTED_AUDIO_DEV_PAD;
        device.deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PAD;
    } else {
        devTypeName = DISTRIBUTED_AUDIO_DEV_CAR;
        device.deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE;
    }
    json addressJson;
    addressJson["devName"] = devName;
    addressJson["devId"] = devId;
    std::string addressStr = addressJson.dump();
    if (memcpy_s(device.address, kMaxAddressLen, addressStr.c_str(), addressStr.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s failed.");
        return false;
    }
    TELEPHONY_LOGI("create distributed audio device succeed.");
    return true;
}

std::string DistributedCallManager::GetDevIdFromAudioDevice(const AudioDevice& device)
{
    std::string devId = "";
    if (!IsDistributedAudioDevice(device)) {
        TELEPHONY_LOGE("not distributed audio device, device type: %{public}d", device.deviceType);
        return devId;
    }
    std::string address = device.address;
    if (!address.length()) {
        TELEPHONY_LOGE("invalid address");
        return devId;
    }
    json addressJson = json::parse(address, nullptr, false);
    if (addressJson.is_null() || addressJson.is_discarded()) {
        TELEPHONY_LOGE("json value is null or discarded.");
        return devId;
    }
    if (!addressJson.contains("devId")) {
        TELEPHONY_LOGE("json not contain devId.");
        return devId;
    }
    if (!addressJson["devId"].is_string()) {
        TELEPHONY_LOGE("json has no devId string.");
        return devId;
    }
    devId = addressJson["devId"];
    TELEPHONY_LOGI("devId: %{public}s", GetAnonyString(devId).c_str());
    return devId;
}

int32_t DistributedCallManager::AddDCallDevice(const std::string& devId)
{
    TELEPHONY_LOGI("add dcall device, devId: %{public}s.", GetAnonyString(devId).c_str());
    std::lock_guard<std::mutex> lock(onlineDeviceMtx_);

    auto iter = onlineDCallDevices_.find(devId);
    if (iter != onlineDCallDevices_.end()) {
        TELEPHONY_LOGW("device is already exist, devId: %{public}s", GetAnonyString(devId).c_str());
        return TELEPHONY_SUCCESS;
    }

    AudioDevice device;
    if (!CreateDAudioDevice(devId, device)) {
        TELEPHONY_LOGE("failed to create distributed audio device, devId: %{public}s", GetAnonyString(devId).c_str());
        return TELEPHONY_ERR_FAIL;
    }

    DelayedSingleton<AudioDeviceManager>::GetInstance()->AddAudioDeviceList(device.address, device.deviceType, "");
    onlineDCallDevices_.emplace(devId, device);

    if (!dCallDeviceSwitchedOn_.load() && isCallActived_.load()) {
        if (device.deviceType == AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE) {
            TELEPHONY_LOGI("switch call to auto motive as it is online");
            SwitchOnDCallDeviceAsync(device);
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t DistributedCallManager::RemoveDCallDevice(const std::string& devId)
{
    TELEPHONY_LOGI("remove dcall device, devId: %{public}s.", GetAnonyString(devId).c_str());
    std::lock_guard<std::mutex> lock(onlineDeviceMtx_);
    auto iter = onlineDCallDevices_.find(devId);
    if (iter != onlineDCallDevices_.end()) {
        std::string devId = GetDevIdFromAudioDevice(iter->second);
        std::string curDevId = GetConnectedDCallDeviceId();
        TELEPHONY_LOGI("removed devId: %{public}s, current devId: %{public}s",
            GetAnonyString(devId).c_str(), GetAnonyString(curDevId).c_str());
        DelayedSingleton<AudioDeviceManager>::GetInstance()->RemoveAudioDeviceList(
            iter->second.address, iter->second.deviceType);
        onlineDCallDevices_.erase(iter);
        if (curDevId == devId) {
            TELEPHONY_LOGI("current dcall device is removed, now reinit audio device.");
            dCallDeviceSwitchedOn_.store(false);
            ClearConnectedDCallDevice();
            DelayedSingleton<AudioDeviceManager>::GetInstance()->InitAudioDevice();
        }
    }
    return TELEPHONY_SUCCESS;
}

void DistributedCallManager::ClearDCallDevices()
{
    TELEPHONY_LOGI("clear dcall device.");
    std::lock_guard<std::mutex> lock(onlineDeviceMtx_);
    onlineDCallDevices_.clear();
}

void DistributedCallManager::NotifyOnlineDCallDevices(std::vector<std::string> devices)
{
    TELEPHONY_LOGI("notify online dcall devices start, size: %{public}d", static_cast<int32_t>(devices.size()));
    for (auto item : devices) {
        AddDCallDevice(item);
        TELEPHONY_LOGI("notify dcall device, devId: %{public}s", GetAnonyString(item).c_str());
    }
    TELEPHONY_LOGI("notify online dcall devices end.");
}

std::string DistributedCallManager::GetConnectedDCallDeviceAddr()
{
    std::lock_guard<std::mutex> lock(connectedDevMtx_);
    std::string addr = connectedAudioDevice_.address;
    return addr;
}

AudioDeviceType DistributedCallManager::GetConnectedDCallDeviceType()
{
    std::lock_guard<std::mutex> lock(connectedDevMtx_);
    AudioDeviceType type = connectedAudioDevice_.deviceType;
    return type;
}

std::string DistributedCallManager::GetConnectedDCallDeviceId()
{
    std::lock_guard<std::mutex> lock(connectedDevMtx_);
    std::string devId = "";
    if (dCallDeviceSwitchedOn_.load()) {
        devId = GetDevIdFromAudioDevice(connectedAudioDevice_);
    }
    return devId;
}

void DistributedCallManager::GetConnectedDCallDevice(AudioDevice& device)
{
    std::lock_guard<std::mutex> lock(connectedDevMtx_);
    device.deviceType = connectedAudioDevice_.deviceType;
    if (memcpy_s(device.address, kMaxAddressLen, connectedAudioDevice_.address, kMaxAddressLen) != EOK) {
        TELEPHONY_LOGE("memcpy_s failed.");
    }
}

void DistributedCallManager::SetConnectedDCallDevice(const AudioDevice& device)
{
    std::lock_guard<std::mutex> lock(connectedDevMtx_);
    connectedAudioDevice_.deviceType = device.deviceType;
    if (memcpy_s(connectedAudioDevice_.address, kMaxAddressLen, device.address, kMaxAddressLen) != EOK) {
        TELEPHONY_LOGE("memcpy_s failed.");
    }
}

void DistributedCallManager::ClearConnectedDCallDevice()
{
    std::lock_guard<std::mutex> lock(connectedDevMtx_);
    connectedAudioDevice_.deviceType = AudioDeviceType::DEVICE_UNKNOWN;
    if (memset_s(connectedAudioDevice_.address, kMaxAddressLen, 0, kMaxAddressLen) != EOK) {
        TELEPHONY_LOGE("memset_s failed.");
    }
}

bool DistributedCallManager::SwitchOnDCallDeviceSync(const AudioDevice& device)
{
    TELEPHONY_LOGI("switch dcall device on sync");
    if (!IsDistributedAudioDevice(device)) {
        TELEPHONY_LOGE("not distributed audio device, device type: %{public}d", device.deviceType);
        return false;
    }
    std::string devId = GetDevIdFromAudioDevice(device);
    if (!devId.length()) {
        TELEPHONY_LOGE("dcall devId is invalid");
        return false;
    }
    TELEPHONY_LOGI("switch dcall device on start, devId: %{public}s", GetAnonyString(devId).c_str());
    if (dcallProxy_ == nullptr) {
        TELEPHONY_LOGE("dcallProxy_ is nullptr");
        return false;
    }
    int32_t ret = dcallProxy_->SwitchDevice(devId, DCALL_SWITCH_DEVICE_TYPE_SINK);
    if (ret == TELEPHONY_SUCCESS) {
        dCallDeviceSwitchedOn_.store(true);
        SetConnectedDCallDevice(device);
        DelayedSingleton<AudioDeviceManager>::GetInstance()->SetCurrentAudioDevice(device.deviceType);
        TELEPHONY_LOGI("switch dcall device on succeed.");
        return true;
    }
    TELEPHONY_LOGI("switch dcall device on failed, ret: %{public}d.", ret);
    return false;
}

void DistributedCallManager::SetCallState(bool isActive)
{
    isCallActived_.store(isActive);
}

void DistributedCallManager::DealDisconnectCall()
{
    dCallDeviceSwitchedOn_.store(false);
    ClearConnectedDCallDevice();
}

void DistributedCallManager::SwitchOnDCallDeviceAsync(const AudioDevice& device)
{
    TELEPHONY_LOGI("switch on dcall device async");
    std::thread switchThread = std::thread([this, device]() { this->SwitchOnDCallDeviceSync(device); });
    pthread_setname_np(switchThread.native_handle(), SWITCH_ON_DCALL_THREAD_NAME.c_str());
    switchThread.detach();
}

void DistributedCallManager::SwitchOffDCallDeviceSync()
{
    TELEPHONY_LOGI("switch dcall device off sync");
    if (!dCallDeviceSwitchedOn_.load()) {
        TELEPHONY_LOGE("distributed audio device not connected.");
        return;
    }
    std::string devId = GetConnectedDCallDeviceId();
    if (!devId.length()) {
        TELEPHONY_LOGE("dcall devId is invalid");
        return;
    }
    TELEPHONY_LOGI("switch dcall device off start, devId: %{public}s", GetAnonyString(devId).c_str());
    if (dcallProxy_ == nullptr) {
        TELEPHONY_LOGE("dcallProxy_ is nullptr");
        return;
    }
    int32_t ret = dcallProxy_->SwitchDevice(devId, DCALL_SWITCH_DEVICE_TYPE_SOURCE);
    if (ret == TELEPHONY_SUCCESS) {
        dCallDeviceSwitchedOn_.store(false);
        ClearConnectedDCallDevice();
        TELEPHONY_LOGI("switch dcall device off succeed.");
    } else {
        TELEPHONY_LOGE("switch dcall device off failed, %{public}d", ret);
    }
}

bool DistributedCallManager::IsDCallDeviceSwitchedOn()
{
    return dCallDeviceSwitchedOn_.load();
}

int32_t DistributedCallManager::OnDCallDeviceOnline(const std::string &devId)
{
    TELEPHONY_LOGI("dcall device is online, devId: %{public}s", GetAnonyString(devId).c_str());
    return AddDCallDevice(devId);
}

int32_t DistributedCallManager::OnDCallDeviceOffline(const std::string &devId)
{
    TELEPHONY_LOGI("dcall device is offline, devId: %{public}s", GetAnonyString(devId).c_str());
    return RemoveDCallDevice(devId);
}

int32_t DistributedCallManager::DistributedCallDeviceListener::OnDCallDeviceOnline(const std::string &devId)
{
    TELEPHONY_LOGI("dcall device is online, devId: %{public}s", GetAnonyString(devId).c_str());
    return DelayedSingleton<DistributedCallManager>::GetInstance()->OnDCallDeviceOnline(devId);
}

int32_t DistributedCallManager::DistributedCallDeviceListener::OnDCallDeviceOffline(const std::string &devId)
{
    TELEPHONY_LOGI("dcall device is offline, devId: %{public}s", GetAnonyString(devId).c_str());
    return DelayedSingleton<DistributedCallManager>::GetInstance()->OnDCallDeviceOffline(devId);
}

void DistributedCallManager::OnDCallSystemAbilityAdded(const std::string &deviceId)
{
    TELEPHONY_LOGI("dcall source service is added, deviceId: %{public}s", GetAnonyString(deviceId).c_str());
    // wait 100ms for dcall-sa to complete init.
    usleep(WAIT_DCALL_INIT_100MS);
    dcallProxy_ = std::make_shared<DistributedCallProxy>();
    if (dcallProxy_ == nullptr) {
        TELEPHONY_LOGE("fail to create dcall proxy obj");
        return;
    }
    if (dcallProxy_->Init() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("init dcall proxy failed");
        return;
    }
    dcallDeviceListener_ = std::make_shared<DistributedCallDeviceListener>();
    if (dcallDeviceListener_ == nullptr) {
        TELEPHONY_LOGE("dcallDeviceListener_ is nullptr");
        return;
    }
    if (dcallProxy_->RegisterDeviceCallback(CALLBACK_NAME, dcallDeviceListener_) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("register dcall callback failed");
        return;
    }
    std::vector<std::string> dcallDevices;
    if (dcallProxy_->GetOnlineDeviceList(dcallDevices) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("get dcall device list failed");
        return;
    }
    if (dcallDevices.size() > 0) {
        NotifyOnlineDCallDevices(dcallDevices);
    }
    TELEPHONY_LOGI("OnDCallSystemAbilityAdded end.");
}

void DistributedCallManager::OnDCallSystemAbilityRemoved(const std::string &deviceId)
{
    TELEPHONY_LOGI("dcall source service is removed, deviceId: %{public}s", GetAnonyString(deviceId).c_str());
    dcallDeviceListener_ = nullptr;
    dcallProxy_ = nullptr;
    dCallDeviceSwitchedOn_.store(false);
    ClearDCallDevices();
    ClearConnectedDCallDevice();
    DelayedSingleton<AudioDeviceManager>::GetInstance()->ResetDistributedCallDevicesList();
    DelayedSingleton<AudioDeviceManager>::GetInstance()->InitAudioDevice();
    TELEPHONY_LOGI("OnDCallSystemAbilityRemoved end.");
}

void DCallSystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("SA: %{public}d is added!", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("added SA is invalid!");
        return;
    }
    if (systemAbilityId != DISTRIBUTED_CALL_SOURCE_SA_ID) {
        TELEPHONY_LOGE("added SA is not dcall source service, ignored.");
        return;
    }
    TELEPHONY_LOGI("notify dcall source service added event to distributed call manager");
    DelayedSingleton<DistributedCallManager>::GetInstance()->OnDCallSystemAbilityAdded(deviceId);
}

void DCallSystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("SA: %{public}d is removed!", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("removed SA is invalid!");
        return;
    }
    if (systemAbilityId != DISTRIBUTED_CALL_SOURCE_SA_ID) {
        TELEPHONY_LOGE("removed SA is not dcall source service, ignored.");
        return;
    }
    TELEPHONY_LOGI("notify dcall source service removed event to distributed call manager");
    DelayedSingleton<DistributedCallManager>::GetInstance()->OnDCallSystemAbilityRemoved(deviceId);
}

void DistributedCallManager::InitDistributedCommunicationCall()
{
    TELEPHONY_LOGI("Init distributed communication call");
    dcCallSaListener_ = new (std::nothrow) DcCallSystemAbilityListener();
    if (dcCallSaListener_ == nullptr) {
        TELEPHONY_LOGE("init dc-call fail, create sa linstener fail");
        return;
    }
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("init dc-call fail, get system ability manager fail");
        return;
    }
    int32_t ret = saManager->SubscribeSystemAbility(DISTRIBUTED_COMMUNICATION_CALL_SA_ID, dcCallSaListener_);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("failed to subscribe dc-call service SA: %{public}d", DISTRIBUTED_COMMUNICATION_CALL_SA_ID);
        return;
    }
}

void DistributedCallManager::OnDcCallSystemAbilityAdded()
{
    TELEPHONY_LOGI("dc-call service added");
    auto handle = dlopen("libtelephony_ext_innerkits.z.so", RTLD_NOW);
    if (handle == nullptr) {
        TELEPHONY_LOGE("open so failed");
        return;
    }
    typedef int32_t (*REGISTER_DC_CALL)();
    auto regFunc = (REGISTER_DC_CALL)dlsym(handle, "RegisterDcCall");
    if (regFunc == nullptr) {
        TELEPHONY_LOGE("get reg function failed");
        dlclose(handle);
        return;
    }
    auto ret = regFunc();
    TELEPHONY_LOGI("reg dc-call service result %{public}d", ret);
    dlclose(handle);
}

void DistributedCallManager::OnDcCallSystemAbilityRemoved()
{
    TELEPHONY_LOGI("dc-call service removed");
}

void DcCallSystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("invalid sa");
        return;
    }
    if (systemAbilityId != DISTRIBUTED_COMMUNICATION_CALL_SA_ID) {
        TELEPHONY_LOGE("added SA is not dc-call service, ignored");
        return;
    }
    DelayedSingleton<DistributedCallManager>::GetInstance()->OnDcCallSystemAbilityAdded();
}

void DcCallSystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("invalid sa");
        return;
    }
    if (systemAbilityId != DISTRIBUTED_COMMUNICATION_CALL_SA_ID) {
        TELEPHONY_LOGE("removed SA is not dc-call service, ignored");
        return;
    }
    DelayedSingleton<DistributedCallManager>::GetInstance()->OnDcCallSystemAbilityRemoved();
}

} // namespace Telephony
} // namespace OHOS