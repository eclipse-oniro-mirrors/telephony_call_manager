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

int32_t GetProxyObjectPtr(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t type = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(type);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetProxyObjectPtr(messageParcel, reply);
}

int32_t SetVoIPCallState(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t state = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(state);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetVoIPCallState(messageParcel, reply);
}

int32_t ReportAudioDeviceInfo(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return  DelayedSingleton<CallManagerService>::GetInstance()->OnReportAudioDeviceInfo(messageParcel, reply);
}

int32_t PostDialProceed(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel reply;
    int32_t callId = provider.ConsumeIntegral<int32_t>() % CALL_ID_NUM;
    int32_t activate = provider.ConsumeIntegral<int32_t>() % ACTIVE_NUM;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteBool(activate);
    return DelayedSingleton<CallManagerService>::GetInstance()->OnPostDialProceed(messageParcel, reply);
}

int32_t GetVoIPCallState(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetVoIPCallState(messageParcel, reply);
}

int32_t CanSetCallTransferTime(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }

    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    bool activate = provider.ConsumeBool();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteBool(activate);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnCanSetCallTransferTime(messageParcel, reply);
}

int32_t ObserverOnCallDetailsChange(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }

    MessageParcel messageParcel;
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnObserverOnCallDetailsChange(messageParcel, reply);
}

int32_t SetVoNRState(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t state = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(state);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetVoNRState(messageParcel, reply);
}

int32_t KickOutFromConference(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnKickOutFromConference(messageParcel, reply);
}

int32_t SetCallRestrictionPassword(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
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
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallRestrictionPassword(messageParcel, reply);
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

int32_t RegisterBluetoothCallManagerCallbackPtr(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t phoneIndex = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    std::string macAddress = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(phoneIndex);
    messageParcel.WriteString(macAddress);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterBluetoothCallManagerCallbackPtr(
        messageParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    FuzzedDataProvider provider(data, size);
    uint32_t choice = provider.ConsumeIntegralInRange<uint32_t>(0, 13);
    switch (choice) {
        case 0:
            OnRegisterVoipCallManagerCallback(provider);
            break;
        case 1:
            CanSetCallTransferTime(provider);
            break;
        case 2:
            ObserverOnCallDetailsChange(provider);
            break;
        case 3:
            SetVoNRState(provider);
            break;
        case 4:
            KickOutFromConference(provider);
            break;
        case 5:
            SetCallRestrictionPassword(provider);
            break;
        case 6:
            GetVoIPCallState(provider);
            break;
        case 7:
            SetVoIPCallState(provider);
            break;
        case 8:
            GetProxyObjectPtr(provider);
            break;
        case 9:
            ReportAudioDeviceInfo(provider);
            break;
        case 10:
            PostDialProceed(provider);
            break;
        case 11:
            OnUnRegisterVoipCallManagerCallback(provider);
            break;
        case 12:
            DoFuzzCallManagerService(provider);
            break;
        default:
            RegisterBluetoothCallManagerCallbackPtr(provider);
            break;
    }
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
