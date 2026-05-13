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
#include "fuzzer/FuzzedDataProvider.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 3;

void AnswerCall(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnAnswerCall(messageParcel, reply);
}

void RejectCall(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnRejectCall(messageParcel, reply);
}

void HangUpCall(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnHangUpCall(messageParcel, reply);
}

void GetBtCallState(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnGetBtCallState(messageParcel, reply);
}

void HoldCall(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnHoldCall(messageParcel, reply);
}

void UnHoldCall(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnUnHoldCall(messageParcel, reply);
}

void SwitchCall(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnSwitchCall(messageParcel, reply);
}

void CombineConference(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnCombineConference(messageParcel, reply);
}

void SeparateConference(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnSeparateConference(messageParcel, reply);
}

void KickOutFromConference(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnKickOutFromConference(messageParcel, reply);
}

void StartDtmf(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt8(static_cast<uint8_t>(testData.data()));
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnStartDtmf(messageParcel, reply);
}

void StopDtmf(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnStopDtmf(messageParcel, reply);
}

void GetCurrentCallList(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = provider.ConsumeIntegralInRange<int32_t>(0, SLOT_NUM);
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnGetCurrentCallList(messageParcel, reply);
}
#ifdef SUPPORT_HEARING_AID
void AddAudioDeviceList(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    std::string str = provider.ConsumeRadomLengthString();
    messageParcel.WriteString(str);
    messageParcel.WriteInt32(provider.ConsumeIntegral<int32_t>());
    messageParcel.WriteString(str);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnAddAudioDeviceList(messageParcel, reply);
}

void RemoveAudioDeviceList(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    std::string str = provider.ConsumeRadomLengthString();
    messageParcel.WriteString(str);
    messageParcel.WriteInt32(provider.ConsumeIntegral<int32_t>());
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnRemoveAudioDeviceList(messageParcel, reply);
}

void ResetNearlinkDeviceList(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnResetNearlinkDeviceList(messageParcel, reply);
}

void ResetBtHearingAidDeviceList(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<BluetoothCallService>::GetInstance()->OnResetBtHearingAidDeviceList(messageParcel, reply);
}
#endif
void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    FuzzedDataProvider provider(data, size);
    AnswerCall(provider);
    RejectCall(provider);
    HangUpCall(provider);
    GetBtCallState(provider);
    HoldCall(provider);
    UnHoldCall(provider);
    SwitchCall(provider);
    CombineConference(provider);
    SeparateConference(provider);
    KickOutFromConference(provider);
    StartDtmf(provider);
    StopDtmf(provider);
    GetCurrentCallList(provider);
#ifdef SUPPORT_HEARING_AID
    AddAudioDeviceList(provider);
    RemoveAudioDeviceList(provider);
    ResetNearlinkDeviceList(provider);
    ResetBtHearingAidDeviceList(provider);
#endif
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
