/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "bluetoothdialcall_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "bluetooth_call_service.h"
#include "call_manager_service.h"
#include "message_parcel.h"
#include "system_ability_definition.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t SLOT_NUM = 3;

bool IsServiceInited()
{
    if (!g_isInited) {
        DelayedSingleton<CallManagerService>::GetInstance()->OnStart();
        if (DelayedSingleton<CallManagerService>::GetInstance()->GetServiceRunningState() ==
            static_cast<int32_t>(CallManagerService::ServiceRunningState::STATE_RUNNING)) {
            g_isInited = true;
        }
    }
    return g_isInited;
}

void AnswerCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnAnswerCall(dataParcel, reply);
}

void RejectCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnRejectCall(dataParcel, reply);
}

void HangUpCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnHangUpCall(dataParcel, reply);
}

void GetBtCallState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnGetBtCallState(dataParcel, reply);
}

void HoldCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnHoldCall(dataParcel, reply);
}

void UnHoldCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnUnHoldCall(dataParcel, reply);
}

void SwitchCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnSwitchCall(dataParcel, reply);
}

void CombineConference(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnCombineConference(dataParcel, reply);
}

void SeparateConference(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnSeparateConference(dataParcel, reply);
}

void StartDtmf(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteInt8(*data);
    dataParcel.WriteBuffer(data + sizeof(char), size - sizeof(char));
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnStartDtmf(dataParcel, reply);
}

void StopDtmf(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnStopDtmf(dataParcel, reply);
}

void GetCurrentCallList(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(slotId);
    size_t dataSize = size - sizeof(int32_t);
    dataParcel.WriteBuffer(data + sizeof(int32_t), dataSize);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnGetCurrentCallList(dataParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    AnswerCall(data, size);
    RejectCall(data, size);
    HangUpCall(data, size);
    GetBtCallState(data, size);
    HoldCall(data, size);
    UnHoldCall(data, size);
    SwitchCall(data, size);
    CombineConference(data, size);
    SeparateConference(data, size);
    StartDtmf(data, size);
    StopDtmf(data, size);
    GetCurrentCallList(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
