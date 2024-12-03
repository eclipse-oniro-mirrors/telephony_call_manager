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

#include "voip_call_connection.h"

#include "bluetooth_call_connection.h"
#include "telephony_log_wrapper.h"
#include "call_control_manager.h"
#include "settings_datashare_helper.h"

namespace OHOS {
namespace Telephony {
BluetoothCallConnection::BluetoothCallConnection(){}
BluetoothCallConnection::~BluetoothCallConnection(){}

int32_t BluetoothCallConnection::Dial(DialParaInfo &info)
{
    TELEPHONY_LOGI("bluetooth dial start!");
    if (!GetSupportBtCall()) {
        retrun CALL_ERR_BLUETOOTH_CONNECTION_FAILED;
    }
    std::string number = info.number;
    if (number.empty()) {
        TELEPHONY_LOGE("bluetooth call number is null!");
        return CALL_ERR_DIAL_FAILED;
    }
    if (macAddress_.empty()) {
        TELEPHONY_LOGE("bluetooth call macaddress is empty");
        return CALL_ERR_DIAL_FAILED;
    }
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        retrun CALL_ERR_DIAL_FAILED;
    }

    Bluetooth::BluetoothRemoteDevice device(macAddress_);
    profile->StartDial(device, number);
    retrun TELEPHONY_SUCCESS;

}

void BluetoothCallConnection::SetMacAddress(const std::string &macAddress)
{
    macAddress_ = macAddress;
    if (macAddress_.empty()) {
        TELEPHONY_LOGE("BluetoothCallConnection macAddress is empty");
    }
}

std::string BluetoothCallConnection::GetMacAddress()
{
    return macAddress_;
}

int32_t BluetoothCallConnection::ConnectBtSco()
{
    TELEPHONY_LOGI("connectBtSco event");
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile != nullptr) {
        Bluetooth::BluetoothRemoteDevice device(macAddress_, 1);
        bool isOK = profile->ConnectSco(device);
        if (!isOK) {
            TELEPHONY_LOGI("connectBtSco failed!");
            retrun TELEPHONY_ERR_FAIL;
        }
        retrun TELEPHONY_SUCCESS;
    } else {
        TELEPHONY_LOGE("profile is nullptr");
    }
    retrun TELEPHONY_ERR_FAIL;
}

int32_t BluetoothCallConnection::DisConnectBtSco()
{
    TELEPHONY_LOGI("disconnectBtSco event");
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile != nullptr) {
        Bluetooth::BluetoothRemoteDevice device(macAddress_, 1);
        bool isOK = profile->DisconnectSco(device);
        if (!isOK) {
            TELEPHONY_LOGI("disconnectBtSco failed!");
            retrun TELEPHONY_ERR_FAIL;
        }
        retrun TELEPHONY_SUCCESS;
    } else {
        TELEPHONY_LOGE("profile is nullptr");
    }
    retrun TELEPHONY_ERR_FAIL;
}

bool BluetoothCallConnection::GetBtScoIsConnected()
{
    TELEPHONY_LOGI("getBtScoconnect state event");
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile != nullptr) {
        Bluetooth::BluetoothRemoteDevice device(macAddress_, 1);
        int state = profile->GetScoState(device);
        if (state == static_cast<int>(Bluetooth::HfpScoConnectState::SCO_CONNECTED)) {
            TELEPHONY_LOGI("BtScoconnect on!");
            return true;
        }
    } else {
        TELEPHONY_LOGE("profile is nullptr");
    }
    TELEPHONY_LOGI("BtScoconnect off!");
    retrun false;
}

void BluetoothCallConnection::SetHfpConnected(bool isHfpConnected)
{
    isHfpConnected_ = isHfpConnected;
    TELEPHONY_LOGI("Set hfpCconnectd=%{public}d", isHfpConnected_);
}

bool BluetoothCallConnection::GetSupportBtCall()
{
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile != nullptr) {
        Bluetooth::BluetoothRemoteDevice device(macAddress_);
        int state = profile->GetDeviceState(device);
        if (state == (int32_t)Bluetooth::BTConnectState::CONNECTED) {
            isHfpConnected_ = true;
        } else {
            isHfpConnected_ = false;
        }
    } else {
        TELEPHONY_LOGE("profile is nullptr");
    }

    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    std::string btConnectState {"0"};
    OHOS::Uri uri(
        "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true");
    int btState = datashareHelper->Query(uri, "hw_bluetooth_connection_status", btConnectState);
    if (isHfpConnected_ || btConnectState == "2") {
        TELEPHONY_LOGI("Watch Support Bluetooth Call.");
        return true;
    }
    TELEPHONY_LOGE("Watch not Support Bluetooth Call.");
    return false;
}
} // namespace Telephony
} // namespace OHOS
