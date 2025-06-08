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
#include "bluetooth_call_proxy.h"
#include "call_manager_proxy.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t DEVICE_NEARLINK = 10;
static std::shared_ptr<CallManagerProxy> g_callManagerProxyPtr = nullptr;
static sptr<IBluetoothCall> g_bluetoothCallProxyPtr = nullptr;

NearlinkCallClient::NearlinkCallClient() {}

NearlinkCallClient::~NearlinkCallClient() {}

void NearlinkCallClient::Init()
{
    TELEPHONY_LOGI("NearlinkCallClient init:");
    if (g_callManagerProxyPtr == nullptr) {
        g_callManagerProxyPtr = DelayedSingleton<CallManagerProxy>::GetInstance();
    }
    g_callManagerProxyPtr->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    sptr<IRemoteObject> iRemoteObjectPtr = g_callManagerProxyPtr->GetProxyObjectPtr(PROXY_BLUETOOTH_CALL);
    if (iRemoteObjectPtr == nullptr) {
        TELEPHONY_LOGE("GetProxyObjectPtr failed!");
        return;
    }
    g_bluetoothCallProxyPtr = iface_cast<IBluetoothCall>(iRemoteObjectPtr);
    if (g_bluetoothCallProxyPtr == nullptr) {
        TELEPHONY_LOGE("iface_cast<IBluetoothCall> failed!");
        return;
    }
    TELEPHONY_LOGI("NearlinkCallClient init success!");
}

void NearlinkCallClient::UnInit()
{
    if (g_callManagerProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return;
    }

    g_callManagerProxyPtr->UnInit();
}

int32_t NearlinkCallClient::RegisterCallBack(std::unique_ptr<CallManagerCallback> callback)
{
    if (g_callManagerProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
    
    return g_callManagerProxyPtr->RegisterCallBack(std::move(callback));
}

int32_t NearlinkCallClient::UnRegisterCallBack()
{
    if (g_callManagerProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return g_callManagerProxyPtr->UnRegisterCallBack();
}

int32_t NearlinkCallClient::AnswerCall()
{
    if (g_bluetoothCallProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return g_bluetoothCallProxyPtr->AnswerCall();
}

int32_t NearlinkCallClient::RejectCall()
{
    if (g_bluetoothCallProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return g_bluetoothCallProxyPtr->RejectCall();
}

int32_t NearlinkCallClient::HangUpCall()
{
    if (g_bluetoothCallProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return g_bluetoothCallProxyPtr->HangUpCall();
}

int32_t NearlinkCallClient::GetCallState()
{
    if (g_bluetoothCallProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return g_bluetoothCallProxyPtr->GetCallState();
}

std::vector<CallAttributeInfo> NearlinkCallClient::GetCurrentCallList(int32_t slotId)
{
    if (g_bluetoothCallProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return std::vector<CallAttributeInfo>();
    }

    return g_bluetoothCallProxyPtr->GetCurrentCallList(slotId);
}

int32_t NearlinkCallClient::AddAudioDevice(const std::string &address, const std::string &name)
{
    if (g_bluetoothCallProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return g_bluetoothCallProxyPtr->AddAudioDeviceList(address, DEVICE_NEARLINK, name);
}

int32_t NearlinkCallClient::RemoveAudioDevice(const std::string &address)
{
    if (g_bluetoothCallProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return g_bluetoothCallProxyPtr->RemoveAudioDeviceList(address, DEVICE_NEARLINK);
}

int32_t NearlinkCallClient::ResetNearlinkDeviceList()
{
    if (g_bluetoothCallProxyPtr == nullptr) {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }

    return g_bluetoothCallProxyPtr->ResetNearlinkDeviceList();
}
} // namespace Telephony
} // namespace OHOS