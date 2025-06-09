/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "nearlink_call_client.h"

#include <memory>
#include "system_ability_definition.h"
#include "telephony_errors.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t DEVICE_NEARLINK = 10;

NearlinkCallClient::NearlinkCallClient() {}

NearlinkCallClient::~NearlinkCallClient() {}

void NearlinkCallClient::Init()
{
    TELEPHONY_LOGI("NearlinkCallClient init:");
    if (callManagerProxyPtr_ == nullptr) {
        callManagerProxyPtr_ = DelayedSingleton<CallManagerProxy>::GetInstance();
    }
    callManagerProxyPtr_->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    sptr<IRemoteObject> iRemoteObjectPtr = callManagerProxyPtr_->GetProxyObjectPtr(PROXY_BLUETOOTH_CALL);
    if (iRemoteObjectPtr == nullptr) {
        TELEPHONY_LOGE("GetProxyObjectPtr failed!");
        return;
    }
    bluetoothCallProxyPtr_ = iface_cast<IBluetoothCall>(iRemoteObjectPtr);
    if (bluetoothCallProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("iface_cast<IBluetoothCall> failed!");
        return;
    }
    TELEPHONY_LOGI("NearlinkCallClient init success!");
}

void NearlinkCallClient::UnInit()
{
    if (callManagerProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return;
    }

    callManagerProxyPtr_->UnInit();
}

int32_t NearlinkCallClient::RegisterCallBack(std::unique_ptr<CallManagerCallback> callback)
{
    if (callManagerProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
    
    return callManagerProxyPtr_->RegisterCallBack(std::move(callback));
}

int32_t NearlinkCallClient::UnRegisterCallBack()
{
    if (callManagerProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return callManagerProxyPtr_->UnRegisterCallBack();
}

int32_t NearlinkCallClient::AnswerCall()
{
    if (bluetoothCallProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return bluetoothCallProxyPtr_->AnswerCall();
}

int32_t NearlinkCallClient::RejectCall()
{
    if (bluetoothCallProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return bluetoothCallProxyPtr_->RejectCall();
}

int32_t NearlinkCallClient::HangUpCall()
{
    if (bluetoothCallProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return bluetoothCallProxyPtr_->HangUpCall();
}

int32_t NearlinkCallClient::GetCallState()
{
    if (bluetoothCallProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return bluetoothCallProxyPtr_->GetCallState();
}

std::vector<CallAttributeInfo> NearlinkCallClient::GetCurrentCallList(int32_t slotId)
{
    if (bluetoothCallProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return std::vector<CallAttributeInfo>();
    }

    return bluetoothCallProxyPtr_->GetCurrentCallList(slotId);
}

int32_t NearlinkCallClient::AddAudioDevice(const std::string &address, const std::string &name)
{
    if (bluetoothCallProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return bluetoothCallProxyPtr_->AddAudioDeviceList(address, DEVICE_NEARLINK, name);
}

int32_t NearlinkCallClient::RemoveAudioDevice(const std::string &address)
{
    if (bluetoothCallProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return bluetoothCallProxyPtr_->RemoveAudioDeviceList(address, DEVICE_NEARLINK);
}

int32_t NearlinkCallClient::ResetNearlinkDeviceList()
{
    if (bluetoothCallProxyPtr_ == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return bluetoothCallProxyPtr_->ResetNearlinkDeviceList();
}
} // namespace Telephony
} // namespace OHOS