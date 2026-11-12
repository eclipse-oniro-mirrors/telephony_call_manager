/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "bluetoothcallconnectionauto_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "addcalltoken_fuzzer.h"
#include "bluetooth_hfp_hf.h"
#include "singleton.h"
#include "telephony_errors.h"
#include "fuzzer/FuzzedDataProvider.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t PHONE_INDEX_MIN = -2;
constexpr int32_t PHONE_INDEX_MAX = 3;
constexpr int32_t SLOT_ID_MAX = 2;
constexpr size_t MAX_MAC_LENGTH = 64;
constexpr size_t MAX_PHONE_NUMBER_LENGTH = 256;
constexpr size_t MAX_CONTACT_NAME_LENGTH = 256;

const uint8_t BLUETOOTH_CALL_DIAL = 0;
const uint8_t BLUETOOTH_SET_MAC_ADDRESS = 1;
const uint8_t BLUETOOTH_GET_MAC_ADDRESS = 2;
const uint8_t BLUETOOTH_GET_DEVICE_NAME = 3;
const uint8_t BLUETOOTH_SET_HFP_CONNECTED = 4;
const uint8_t BLUETOOTH_HFP_DISCONNECTED_END_BT_CALL = 6;
const uint8_t BLUETOOTH_SET_HFP_CONTACT_NAME = 7;
const uint8_t BLUETOOTH_GET_HFP_CONTACT_NAME = 8;
const uint8_t BLUETOOTH_HANDLE_BT_CONNECTION_STATE_CHANGE = 12;
const uint8_t TARGET_SIZE = 13;

static std::shared_ptr<BluetoothCallConnection> GetBluetoothCallConnection()
{
    return DelayedSingleton<BluetoothCallConnection>::GetInstance();
}

static void DoDial(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    int32_t phoneIndex = provider.ConsumeIntegralInRange<int32_t>(PHONE_INDEX_MIN, PHONE_INDEX_MAX);
    std::string macAddress = provider.ConsumeRandomLengthString(MAX_MAC_LENGTH);
    connection->SetMacAddress(phoneIndex, macAddress);
    DialParaInfo dialPara;
    dialPara.phoneIndex = phoneIndex;
    dialPara.accountId = provider.ConsumeIntegralInRange<int32_t>(0, SLOT_ID_MAX);
    dialPara.number = provider.ConsumeRandomLengthString(MAX_PHONE_NUMBER_LENGTH);
    (void)connection->ConnectBtSco();
    (void)connection->Dial(dialPara);
    (void)connection->DisConnectBtSco();
}

static void DoSetMacAddress(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    int32_t phoneIndex = provider.ConsumeIntegralInRange<int32_t>(PHONE_INDEX_MIN, PHONE_INDEX_MAX);
    std::string macAddress = provider.ConsumeRandomLengthString(MAX_MAC_LENGTH);
    connection->SetMacAddress(phoneIndex, macAddress);
}

static void DoGetMacAddress(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    int32_t phoneIndex = provider.ConsumeIntegralInRange<int32_t>(PHONE_INDEX_MIN, PHONE_INDEX_MAX);
    (void)connection->GetMacAddress(phoneIndex);
}

static void DoGetDeviceName(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    int32_t phoneIndex = provider.ConsumeIntegralInRange<int32_t>(PHONE_INDEX_MIN, PHONE_INDEX_MAX);
    (void)connection->GetDeviceName(phoneIndex);
}

static void DoSetHfpConnected(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    bool isHfpConnected = provider.ConsumeBool();
    connection->SetHfpConnected(isHfpConnected);
}

static void DoHfpDisConnectedEndBtCall(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    bool isSystemAbilityRemoved = provider.ConsumeBool();
    connection->HfpDisConnectedEndBtCall(isSystemAbilityRemoved);
}

static void DoSetHfpContactName(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    std::string hfpPhoneNumber = provider.ConsumeRandomLengthString(MAX_PHONE_NUMBER_LENGTH);
    std::string hfpContactName = provider.ConsumeRandomLengthString(MAX_CONTACT_NAME_LENGTH);
    connection->SetHfpContactName(hfpPhoneNumber, hfpContactName);
}

static void DoGetHfpContactName(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    std::string hfpPhoneNumber = provider.ConsumeRandomLengthString(MAX_PHONE_NUMBER_LENGTH);
    (void)connection->GetHfpContactName(hfpPhoneNumber);
}

static void DoHandleBtConnectionStateChange(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    std::string macAddress = provider.ConsumeRandomLengthString(MAX_MAC_LENGTH);
    Bluetooth::BluetoothRemoteDevice device(macAddress);
    int32_t state = static_cast<int32_t>(provider.ConsumeIntegral<uint8_t>());
    connection->HandleBtConnectionStateChange(device, state);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    FuzzedDataProvider provider(data, size);
    uint8_t action = provider.ConsumeIntegralInRange<uint8_t>(0, TARGET_SIZE - 1);
    switch (action) {
        case BLUETOOTH_CALL_DIAL:
            DoDial(provider);
            break;
        case BLUETOOTH_SET_MAC_ADDRESS:
            DoSetMacAddress(provider);
            break;
        case BLUETOOTH_GET_MAC_ADDRESS:
            DoGetMacAddress(provider);
            break;
        case BLUETOOTH_GET_DEVICE_NAME:
            DoGetDeviceName(provider);
            break;
        case BLUETOOTH_SET_HFP_CONNECTED:
            DoSetHfpConnected(provider);
            break;
        case BLUETOOTH_HFP_DISCONNECTED_END_BT_CALL:
            DoHfpDisConnectedEndBtCall(provider);
            break;
        case BLUETOOTH_SET_HFP_CONTACT_NAME:
            DoSetHfpContactName(provider);
            break;
        case BLUETOOTH_GET_HFP_CONTACT_NAME:
            DoGetHfpContactName(provider);
            break;
        case BLUETOOTH_HANDLE_BT_CONNECTION_STATE_CHANGE:
            DoHandleBtConnectionStateChange(provider);
            break;
        default:
            break;
    }
}
} // namespace Telephony
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::Telephony::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
