/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include "message_parcel.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 3;

void AnswerCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnAnswerCall(messageParcel, reply);
}

void RejectCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnRejectCall(messageParcel, reply);
}

void HangUpCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnHangUpCall(messageParcel, reply);
}

void GetBtCallState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnGetBtCallState(messageParcel, reply);
}

void HoldCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnHoldCall(messageParcel, reply);
}

void UnHoldCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnUnHoldCall(messageParcel, reply);
}

void SwitchCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnSwitchCall(messageParcel, reply);
}

void CombineConference(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnCombineConference(messageParcel, reply);
}

void SeparateConference(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnSeparateConference(messageParcel, reply);
}

void KickOutFromConference(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnKickOutFromConference(messageParcel, reply);
}

void StartDtmf(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteInt8(*data);
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnStartDtmf(messageParcel, reply);
}

void StopDtmf(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnStopDtmf(messageParcel, reply);
}

void GetCurrentCallList(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnGetCurrentCallList(messageParcel, reply);
}

void AddAudioDeviceList(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    std::string str(reinterpret_cast<const char *>(data), size);
    messageParcel.WriteString(str);
    messageParcel.WriteInt32(static_cast<int32_t>(data[0]));
    messageParcel.WriteString(str);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnAddAudioDeviceList(messageParcel, reply);
}

void RemoveAudioDeviceList(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    std::string str(reinterpret_cast<const char *>(data), size);
    messageParcel.WriteString(str);
    messageParcel.WriteInt32(static_cast<int32_t>(data[0]));
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnRemoveAudioDeviceList(messageParcel, reply);
}

void ResetNearlinkDeviceList(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnResetNearlinkDeviceList(messageParcel, reply);
}

void ResetBtHearingAidDeviceList(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnResetBtHearingAidDeviceList(messageParcel, reply);
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
    KickOutFromConference(data, size);
    StartDtmf(data, size);
    StopDtmf(data, size);
    GetCurrentCallList(data, size);
    AddAudioDeviceList(data, size);
    RemoveAudioDeviceList(data, size);
    ResetNearlinkDeviceList(data, size);
    ResetBtHearingAidDeviceList(data, size);
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
