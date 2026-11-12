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

#include "bluetoothcallconnectiontwoauto_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "addcalltoken_fuzzer.h"
#include "bluetooth_call.h"
#include "call_manager_base.h"
#include "call_manager_info.h"
#include "iremote_stub.h"
#include "i_transfer_control_callback.h"
#include "message_parcel.h"
#include "securec.h"
#include "singleton.h"
#include "telephony_errors.h"
#include "fuzzer/FuzzedDataProvider.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t SLOT_ID_MAX = 2;
constexpr int32_t CALL_ID_MAX = 100;
constexpr int32_t CALL_INDEX_MAX = 100;
constexpr size_t MAX_MAC_LENGTH = 64;
constexpr size_t MAX_PHONE_NUMBER_LENGTH = 256;
constexpr size_t MAX_CONTACT_NAME_LENGTH = 256;

const uint8_t BLUETOOTH_CACHE_ANCS_CONTACT_NAME = 5;
const uint8_t BLUETOOTH_NOTIFY_TRANSFER_CALL = 7;
const uint8_t BLUETOOTH_UPDATE_TRANSFER_CALL = 8;
const uint8_t BLUETOOTH_START_HFP_WAIT_CONTACT_TASK = 9;
const uint8_t BLUETOOTH_NOTIFY_ANCS_CONTACT_ARRIVED = 10;
const uint8_t BLUETOOTH_HANDLE_TRANSFER_CALL_SWITCH_BACK = 11;
const uint8_t TARGET_SIZE = 12;

class TransferControlCallbackStubImpl : public IRemoteStub<ITransferControlCallback> {
public:
    TransferControlCallbackStubImpl() = default;
    ~TransferControlCallbackStubImpl() override = default;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        (void)code;
        (void)data;
        (void)reply;
        (void)option;
        return 0;
    }

    int32_t OnGetTransferResult(const TransferCallInfo &info, TransferResultInfo &result) override
    {
        (void)info;
        result.deviceShowName = "fuzzTransferDevice";
        result.secPolicy = 0;
        return 0;
    }

    int32_t OnUpdateTransferCall(const TransferCallInfo &info) override
    {
        (void)info;
        return 0;
    }

    int32_t OnDisconnectSco(const std::string &devMac) override
    {
        (void)devMac;
        return 0;
    }
};

static std::shared_ptr<BluetoothCallConnection> GetBluetoothCallConnection()
{
    return DelayedSingleton<BluetoothCallConnection>::GetInstance();
}

static sptr<BluetoothCall> CreateBluetoothCall(FuzzedDataProvider &provider)
{
    DialParaInfo dialPara;
    dialPara.callId = provider.ConsumeIntegralInRange<int32_t>(1, CALL_ID_MAX);
    dialPara.index = provider.ConsumeIntegralInRange<int32_t>(1, CALL_INDEX_MAX);
    dialPara.accountId = provider.ConsumeIntegralInRange<int32_t>(0, SLOT_ID_MAX);
    dialPara.phoneIndex = provider.ConsumeIntegralInRange<int32_t>(0, 1);
    dialPara.number = provider.ConsumeRandomLengthString(MAX_PHONE_NUMBER_LENGTH);
    dialPara.callState = static_cast<TelCallState>(provider.ConsumeIntegral<uint8_t>());
    dialPara.callType = static_cast<CallType>(provider.ConsumeIntegral<uint8_t>());
    std::string macAddress = provider.ConsumeRandomLengthString(MAX_MAC_LENGTH);
    return new BluetoothCall(dialPara, macAddress, dialPara.phoneIndex);
}

static void DoCacheAncsContactName(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    std::string contactName = provider.ConsumeRandomLengthString(MAX_CONTACT_NAME_LENGTH);
    connection->CacheAncsContactName(contactName);
    (void)connection->ConsumeAncsContactName();
}

static void DoNotifyTransferCall(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    sptr<BluetoothCall> call = CreateBluetoothCall(provider);
    if (call == nullptr) {
        return;
    }
    connection->NotifyTransferCall(call);
}

static void DoUpdateTransferCall(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    sptr<BluetoothCall> call = CreateBluetoothCall(provider);
    if (call == nullptr) {
        return;
    }
    connection->UpdateTransferCall(call);
}

static void DoStartHfpWaitContactTask(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    sptr<BluetoothCall> call = CreateBluetoothCall(provider);
    if (call == nullptr) {
        return;
    }
    connection->StartHfpWaitContactTask(call);
}

static void DoNotifyAncsContactArrived(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    std::string contactName = provider.ConsumeRandomLengthString(MAX_CONTACT_NAME_LENGTH);
    sptr<BluetoothCall> call = CreateBluetoothCall(provider);
    if (call == nullptr) {
        return;
    }
    connection->NotifyAncsContactArrived(contactName, call);
}

static void DoHandleTransferCallSwitchBack(FuzzedDataProvider &provider)
{
    auto connection = GetBluetoothCallConnection();
    if (connection == nullptr) {
        return;
    }
    sptr<BluetoothCall> call = CreateBluetoothCall(provider);
    if (call == nullptr) {
        return;
    }
    connection->HandleTransferCallSwitchBack(call);
}

void DoSomethingInterestingWithTwoAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    FuzzedDataProvider provider(data, size);
    uint8_t action = provider.ConsumeIntegralInRange<uint8_t>(0, TARGET_SIZE - 1);
    switch (action) {
        case BLUETOOTH_CACHE_ANCS_CONTACT_NAME:
            DoCacheAncsContactName(provider);
            break;
        case BLUETOOTH_NOTIFY_TRANSFER_CALL:
            DoNotifyTransferCall(provider);
            break;
        case BLUETOOTH_UPDATE_TRANSFER_CALL:
            DoUpdateTransferCall(provider);
            break;
        case BLUETOOTH_START_HFP_WAIT_CONTACT_TASK:
            DoStartHfpWaitContactTask(provider);
            break;
        case BLUETOOTH_NOTIFY_ANCS_CONTACT_ARRIVED:
            DoNotifyAncsContactArrived(provider);
            break;
        case BLUETOOTH_HANDLE_TRANSFER_CALL_SWITCH_BACK:
            DoHandleTransferCallSwitchBack(provider);
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
    OHOS::Telephony::DoSomethingInterestingWithTwoAPI(data, size);
    return 0;
}
