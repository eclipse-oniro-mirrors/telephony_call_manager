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

#include "callmanagerservicestub_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_manager_service_stub.h"
#include "call_ability_callback.h"
#include "surface_utils.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t ACTIVE_NUM = 2;
constexpr int32_t CALL_ID_NUM = 10;
constexpr int32_t WAIT_TIME = 2;

void OnRegisterVoipCallManagerCallback(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    std::unique_ptr<CallAbilityCallback> callbackWrap = std::make_unique<CallAbilityCallback>();
    if (callbackWrap == nullptr) {
        return;
    }
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterVoipCallManagerCallback(messageParcel, reply);
}

void OnUnRegisterVoipCallManagerCallback(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnUnRegisterVoipCallManagerCallback(messageParcel, reply);
}

void GetProxyObjectPtr(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    int32_t type = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(type);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetProxyObjectPtr(messageParcel, reply);
}

void SetVoIPCallState(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    int32_t state = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(state);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetVoIPCallState(messageParcel, reply);
}

void ReportAudioDeviceInfo(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnReportAudioDeviceInfo(messageParcel, reply);
}

void PostDialProceed(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    MessageParcel reply;
    int32_t callId = provider.ConsumeIntegral<int32_t>() % CALL_ID_NUM;
    int32_t activate = provider.ConsumeIntegral<int32_t>() % ACTIVE_NUM;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteBool(activate);
    DelayedSingleton<CallManagerService>::GetInstance()->OnPostDialProceed(messageParcel, reply);
}

void GetVoIPCallState(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetVoIPCallState(messageParcel, reply);
}

void CanSetCallTransferTime(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    bool activate = provider.ConsumeBool();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteBool(activate);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnCanSetCallTransferTime(messageParcel, reply);
}

void ObserverOnCallDetailsChange(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnObserverOnCallDetailsChange(messageParcel, reply);
}

void SetVoNRState(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t state = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(state);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetVoNRState(messageParcel, reply);
}

void KickOutFromConference(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnKickOutFromConference(messageParcel, reply);
}

void SetCallRestrictionPassword(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t fac = provider.ConsumeIntegral<int32_t>();
    std::string oldPassword = provider.ConsumeRandomLengthString(kMaxNumberLen);
    std::string newPassword = provider.ConsumeRandomLengthString(kMaxNumberLen);
    std::string passwordBuf = oldPassword;
    passwordBuf.push_back('\0');
    passwordBuf += newPassword;
    passwordBuf.push_back('\0');
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(fac);
    messageParcel.WriteBuffer(passwordBuf.c_str(), passwordBuf.size());
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallRestrictionPassword(messageParcel, reply);
}

void DoFuzzCallManagerService(FuzzedDataProvider& provider)
{
    auto callManagerService = DelayedSingleton<CallManagerService>::GetInstance();
    callManagerService->OnStart();
    uint32_t code = provider.ConsumeIntegralInRange<uint32_t>(0, 99);
    if (provider.remaining_bytes() == 0) {
        return;
    }
    std::u16string service_token = u"OHOS.Telephony.ICallManagerService";
    MessageOption option;
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    std::vector<uint8_t> subData =
        provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, provider.remaining_bytes()));
    messageParcel.WriteInterfaceToken(service_token);
    messageParcel.WriteBuffer(subData.data(), subData.size());
    callManagerService->OnRemoteRequest(code, messageParcel, replyParcel, option);
}

void RegisterBluetoothCallManagerCallbackPtr(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    int32_t phoneIndex = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    std::string macAddress = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(phoneIndex);
    messageParcel.WriteString(macAddress);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterBluetoothCallManagerCallbackPtr(
        messageParcel, reply);
}

using FuzzApiHandler = void (*)(FuzzedDataProvider&);
constexpr FuzzApiHandler STUB_API_HANDLERS[] = {
    OnRegisterVoipCallManagerCallback,
    CanSetCallTransferTime,
    ObserverOnCallDetailsChange,
    SetVoNRState,
    KickOutFromConference,
    SetCallRestrictionPassword,
    GetVoIPCallState,
    SetVoIPCallState,
    GetProxyObjectPtr,
    ReportAudioDeviceInfo,
    PostDialProceed,
    OnUnRegisterVoipCallManagerCallback,
    DoFuzzCallManagerService,
    RegisterBluetoothCallManagerCallbackPtr,
};
constexpr uint32_t STUB_API_TARGET_COUNT =
    sizeof(STUB_API_HANDLERS) / sizeof(STUB_API_HANDLERS[0]);

void DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    FuzzedDataProvider provider(data, size);
    uint32_t targetIndex = provider.ConsumeIntegralInRange<uint32_t>(0, STUB_API_TARGET_COUNT - 1);
    STUB_API_HANDLERS[targetIndex](provider);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
