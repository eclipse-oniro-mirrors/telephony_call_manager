/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "bluetooth_call_state.h"
#include "telephony_log_wrapper.h"
#include "audio_device_manager.h"
#include "bluetooth_call_connection.h"

namespace OHOS {
namespace Telephony {

BluetoothCallState::BluetoothCallState() {}
BluetoothCallState::~BluetoothCallState() {}

void BluetoothCallState::OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device,
    int32_t state, int32_t cause)
{
    switch (state) {
        case (int32_t)Bluetooth::BTConnectState::CONNECTED:
            DelayedSingleton<BluetoothCallConnection>::GetInstance()->SetHfpConnected(true);
            break;
        default:
            DelayedSingleton<BluetoothCallConnection>::GetInstance()->SetHfpConnected(false);
            break;
    }
}

void BluetoothCallState::OnScoStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int state)
{
    TELEPHONY_LOGI("BluetoothCallState OnScoStateChanged, state = %{public}d", state);
    if (state == static_cast<int>(Bluetooth::HfpScoConnectState::SCO_CONNECTED) ||
        state == static_cast<int>(Bluetooth::HfpScoConnectState::SCO_DISCONNECTED)) {
        DelayedSingleton<AudioDeviceManager>::GetInstance()->ReportAudioDeviceInfo();
    }
}
} // namespace Telephony
} // namespace OHOS
