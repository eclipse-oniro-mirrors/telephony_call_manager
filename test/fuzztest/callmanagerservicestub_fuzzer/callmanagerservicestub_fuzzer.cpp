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
constexpr int32_t VEDIO_STATE_NUM = 2;
constexpr int32_t DIAL_SCENE_NUM = 3;
constexpr int32_t DIAL_TYPE_NUM = 3;
constexpr int32_t CALL_TYPE_NUM = 3;
constexpr int32_t CALL_ID_NUM = 10;
constexpr int32_t AUDIO_DEVICE_NUM = 6;

void OnRegisterVoipCallManagerCallback(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    std::unique_ptr<CallAbilityCallback> callbackWrap = std::make_unique<CallAbilityCallback>();
    if (callbackWrap == nullptr) {
        return;
    }
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterVoipCallManagerCallback(messageParcel, reply);
}

void OnUnRegisterVoipCallManagerCallback(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnUnRegisterVoipCallManagerCallback(messageParcel, reply);
}

int32_t GetProxyObjectPtr(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t type = static_cast<int32_t>(*data);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(type);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetProxyObjectPtr(messageParcel, reply);
}

int32_t SetVoIPCallState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t state = static_cast<int32_t>(*data % SLOT_NUM);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(state);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetVoIPCallState(messageParcel, reply);
}

int32_t ReportAudioDeviceInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return  DelayedSingleton<CallManagerService>::GetInstance()->OnReportAudioDeviceInfo(messageParcel, reply);
}

int32_t PostDialProceed(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel reply;
    int32_t callId = static_cast<int32_t>(*data % CALL_ID_NUM);
    int32_t activate = static_cast<int32_t>(*data % ACTIVE_NUM);
    messageParcel.WriteInt32(callId);
    messageParcel.WriteBool(activate);
    return DelayedSingleton<CallManagerService>::GetInstance()->OnPostDialProceed(messageParcel, reply);
}

int32_t GetVoIPCallState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetVoIPCallState(messageParcel, reply);
}

int32_t CanSetCallTransferTime(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel messageParcel;
    CallTransferInfo info;
    (void)memcpy_s(info.transferNum, kMaxNumberLen, reinterpret_cast<const char *>(data), size);
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    int32_t activate = static_cast<int32_t>(size % ACTIVE_NUM);
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteBool(activate);
    messageParcel.WriteRawData((const void *)&info, sizeof(CallTransferInfo));
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnCanSetCallTransferTime(messageParcel, reply);
}

int32_t ObserverOnCallDetailsChange(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }

    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnObserverOnCallDetailsChange(messageParcel, reply);
}

int32_t SetVoNRState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = static_cast<int32_t>(*data % SLOT_NUM);
    int32_t state = static_cast<int32_t>(*data % SLOT_NUM);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(state);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetVoNRState(messageParcel, reply);
}

int32_t KickOutFromConference(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = static_cast<int32_t>(*data);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnKickOutFromConference(messageParcel, reply);
}

int32_t SetCallRestrictionPassword(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(static_cast<int32_t>(size));
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallRestrictionPassword(messageParcel, reply);
}

void DoFuzzCallManagerService(const uint8_t *data, size_t size)
{
    auto callManagerService = DelayedSingleton<CallManagerService>::GetInstance();
    callManagerService->OnStart();
    FuzzedDataProvider fdp(data, size);
    uint32_t code = fdp.ConsumeIntegralInRange<uint32_t>(0, 72);
    if (fdp.remaining_bytes() == 0) {
        return;
    }
    std::u16string service_token = u"OHOS.Telephony.ICallManagerService";
    MessageOption option;
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    messageParcel.WriteInterfaceToken(service_token);
    messageParcel.WriteBuffer(subData.data(), subData.size());
    callManagerService->OnRemoteRequest(code, messageParcel, replyParcel, option);
}

int32_t RegisterBluetoothCallManagerCallbackPtr(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    std::string str = "";
    MessageParcel messageParcel;
    messageParcel.WriteString(str);
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterBluetoothCallManagerCallbackPtr(
        messageParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    OnRegisterVoipCallManagerCallback(data, size);
    CanSetCallTransferTime(data, size);
    ObserverOnCallDetailsChange(data, size);
    SetVoNRState(data, size);
    KickOutFromConference(data, size);
    SetCallRestrictionPassword(data, size);
    GetVoIPCallState(data, size);
    SetVoIPCallState(data, size);
    GetProxyObjectPtr(data, size);
    ReportAudioDeviceInfo(data, size);
    PostDialProceed(data, size);
    OnUnRegisterVoipCallManagerCallback(data, size);
    DoFuzzCallManagerService(data, size);
    RegisterBluetoothCallManagerCallbackPtr(data, size);
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
