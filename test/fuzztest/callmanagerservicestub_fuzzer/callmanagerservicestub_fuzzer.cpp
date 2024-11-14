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
    MessageParcel dataMessageParcel;
    std::unique_ptr<CallAbilityCallback> callbackWrap = std::make_unique<CallAbilityCallback>();
    if (callbackWrap == nullptr) {
        return;
    }
    dataMessageParcel.WriteRemoteObject(callbackWrap.release()->AsObject().GetRefPtr());
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterVoipCallManagerCallback(dataMessageParcel, reply);
}

void OnUnRegisterVoipCallManagerCallback(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel dataMessageParcel;
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnUnRegisterVoipCallManagerCallback(dataMessageParcel, reply);
}

int32_t GetProxyObjectPtr(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t type = static_cast<int32_t>(*data);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(type);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetProxyObjectPtr(dataParcel, reply);
}

int32_t SetVoIPCallState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t state = static_cast<int32_t>(*data % SLOT_NUM);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(state);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetVoIPCallState(dataParcel, reply);
}

int32_t ReportAudioDeviceInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    return  DelayedSingleton<CallManagerService>::GetInstance()->OnReportAudioDeviceInfo(dataParcel, reply);
}

int32_t PostDialProceed(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    int32_t callId = static_cast<int32_t>(*data % CALL_ID_NUM);
    int32_t activate = static_cast<int32_t>(*data % ACTIVE_NUM);
    dataParcel.WriteInt32(callId);
    dataParcel.WriteBool(activate);
    return DelayedSingleton<CallManagerService>::GetInstance()->OnPostDialProceed(dataParcel, reply);
}

int32_t GetVoIPCallState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetVoIPCallState(dataParcel, reply);
}

int32_t CanSetCallTransferTime(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataMessageParcel;
    CallTransferInfo info;
    (void)memcpy_s(info.transferNum, kMaxNumberLen, reinterpret_cast<const char *>(data), size);
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    int32_t activate = static_cast<int32_t>(size % ACTIVE_NUM);
    dataMessageParcel.WriteInt32(slotId);
    dataMessageParcel.WriteBool(activate);
    dataMessageParcel.WriteRawData((const void *)&info, sizeof(CallTransferInfo));
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnCanSetCallTransferTime(dataMessageParcel, reply);
}

int32_t ObserverOnCallDetailsChange(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }

    MessageParcel dataMessageParcel;
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnObserverOnCallDetailsChange(dataMessageParcel, reply);
}

int32_t SetVoNRState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = static_cast<int32_t>(*data % SLOT_NUM);
    int32_t state = static_cast<int32_t>(*data % SLOT_NUM);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(state);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetVoNRState(dataParcel, reply);
}

int32_t KickOutFromConference(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = static_cast<int32_t>(*data);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(callId);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnKickOutFromConference(dataParcel, reply);
}

int32_t SetCallRestrictionPassword(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(static_cast<int32_t>(size));
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallRestrictionPassword(dataParcel, reply);
}

void doFuzzCallManagerService(const uint8_t *data, size_t size)
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
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(service_token);
    dataParcel.WriteBuffer(subData.data(), subData.size());
    callManagerService->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

int32_t RegisterBluetoothCallManagerCallbackPtr(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    std::string str = "";
    MessageParcel dataParcel;
    dataParcel.WriteString(str);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterBluetoothCallManagerCallbackPtr(
        dataParcel, reply);
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
    doFuzzCallManagerService(data, size);
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
