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

#include "bluetooth_call_client.h"

#include <memory>

#include "system_ability_definition.h"

#include "telephony_errors.h"

#include "bluetooth_call_proxy.h"
#include "call_manager_proxy.h"

namespace OHOS {
namespace Telephony {
static std::shared_ptr<CallManagerProxy> g_callManagerProxyPtr = nullptr;
static sptr<IBluetoothCall> g_bluetoothCallProxyPtr = nullptr;

BluetoothCallClient::BluetoothCallClient() {}

BluetoothCallClient::~BluetoothCallClient() {}

void BluetoothCallClient::Init()
{
    TELEPHONY_LOGI("BluetoothCallClient init:");
    if (g_callManagerProxyPtr == nullptr) {
        g_callManagerProxyPtr = DelayedSingleton<CallManagerProxy>::GetInstance();
        if (g_callManagerProxyPtr == nullptr) {
            TELEPHONY_LOGE("g_callManagerProxyPtr is nullptr");
            return;
        }
        g_callManagerProxyPtr->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    }
    sptr<IRemoteObject> iRemoteObjectPtr = g_callManagerProxyPtr->GetProxyObjectPtr(PROXY_BLUETOOTH_CALL);
    if (iRemoteObjectPtr == nullptr) {
        TELEPHONY_LOGE("GetProxyObjectPtr failed!");
        return;
    }
    g_bluetoothCallProxyPtr = iface_cast<IBluetoothCall>(iRemoteObjectPtr);
    if (g_bluetoothCallProxyPtr == nullptr) {
        TELEPHONY_LOGE("iface_cast<ICallManagerService> failed!");
        return;
    }
    TELEPHONY_LOGI("BluetoothCallClient init success!");
}

void BluetoothCallClient::UnInit()
{
    if (g_callManagerProxyPtr != nullptr) {
        g_callManagerProxyPtr->UnInit();
    } else {
        TELEPHONY_LOGE("init first please!");
    }
}

int32_t BluetoothCallClient::RegisterCallBack(std::unique_ptr<CallManagerCallback> callback)
{
    if (g_callManagerProxyPtr != nullptr) {
        return g_callManagerProxyPtr->RegisterCallBack(std::move(callback));
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::UnRegisterCallBack()
{
    if (g_callManagerProxyPtr != nullptr) {
        return g_callManagerProxyPtr->UnRegisterCallBack();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::DialCall(std::u16string number, AppExecFwk::PacMap &extras)
{
    if (g_callManagerProxyPtr != nullptr) {
        extras.PutBooleanValue("btSlotIdUnknown", true);
        return g_callManagerProxyPtr->DialCall(number, extras);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::AnswerCall()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->AnswerCall();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::RejectCall()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->RejectCall();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::HangUpCall()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->HangUpCall();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::GetCallState()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->GetCallState();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::HoldCall()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->HoldCall();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::UnHoldCall()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->UnHoldCall();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::SwitchCall()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->SwitchCall();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::CombineConference()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->CombineConference();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::SeparateConference()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->SeparateConference();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::KickOutFromConference()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->KickOutFromConference();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::StartDtmf(char str)
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->StartDtmf(str);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::StopDtmf()
{
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->StopDtmf();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::IsRinging(bool &enabled)
{
    if (g_callManagerProxyPtr != nullptr) {
        return g_callManagerProxyPtr->IsRinging(enabled);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

bool BluetoothCallClient::HasCall()
{
    if (g_callManagerProxyPtr != nullptr) {
        return g_callManagerProxyPtr->HasCall();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::IsNewCallAllowed(bool &enabled)
{
    if (g_callManagerProxyPtr != nullptr) {
        return g_callManagerProxyPtr->IsNewCallAllowed(enabled);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::IsInEmergencyCall(bool &enabled)
{
    if (g_callManagerProxyPtr != nullptr) {
        return g_callManagerProxyPtr->IsInEmergencyCall(enabled);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::SetMuted(bool isMute)
{
    if (g_callManagerProxyPtr != nullptr) {
        return g_callManagerProxyPtr->SetMuted(isMute);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::MuteRinger()
{
    if (g_callManagerProxyPtr != nullptr) {
        return g_callManagerProxyPtr->MuteRinger();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t BluetoothCallClient::SetAudioDevice(AudioDeviceType deviceType, const std::string &bluetoothAddress)
{
    AudioDevice device;
    if (memset_s(&device, sizeof(AudioDevice), 0, sizeof(AudioDevice)) != EOK) {
        TELEPHONY_LOGE("memset_s fail");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    device.deviceType = static_cast<AudioDeviceType>(deviceType);
    if (bluetoothAddress.length() > kMaxAddressLen) {
        TELEPHONY_LOGE("address is not too long");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (memcpy_s(device.address, kMaxAddressLen, bluetoothAddress.c_str(), bluetoothAddress.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s address fail");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }

    if (g_callManagerProxyPtr != nullptr) {
        return g_callManagerProxyPtr->SetAudioDevice(device);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

std::vector<CallAttributeInfo> BluetoothCallClient::GetCurrentCallList(int32_t slotId)
{
    std::vector<CallAttributeInfo> callVec;
    callVec.clear();
    if (g_bluetoothCallProxyPtr != nullptr) {
        return g_bluetoothCallProxyPtr->GetCurrentCallList(slotId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return callVec;
    }
}
} // namespace Telephony
} // namespace OHOS