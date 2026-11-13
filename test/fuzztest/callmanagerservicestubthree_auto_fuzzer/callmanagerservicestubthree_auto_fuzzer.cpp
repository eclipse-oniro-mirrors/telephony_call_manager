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

#include "callmanagerservicestubthree_auto_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_manager_service_stub.h"
#include "call_ability_callback.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t AUDIO_DEVICE_NUM = 8;
constexpr int32_t CONFERENCE_LIST_NUM = 3;

static void DoOnCombineConference(FuzzedDataProvider &provider)
{
    int32_t mainCallId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(mainCallId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnCombineConference(messageParcel, reply);
}

static void DoOnSeparateConference(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSeparateConference(messageParcel, reply);
}

static void DoOnJoinConference(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::vector<std::u16string> numberList;
    for (int32_t i = 0; i < CONFERENCE_LIST_NUM; i++) {
        numberList.push_back(Str8ToStr16(provider.ConsumeRandomLengthString()));
    }
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString16Vector(numberList);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnJoinConference(messageParcel, reply);
}

static void DoOnGetMainCallId(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetMainCallId(messageParcel, reply);
}

static void DoOnGetSubCallIdList(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetSubCallIdList(messageParcel, reply);
}

static void DoOnGetCallIdListForConference(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallIdListForConference(messageParcel, reply);
}

static void DoOnStartDtmf(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    int8_t dtmfCode = provider.ConsumeIntegral<int8_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteInt8(dtmfCode);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnStartDtmf(messageParcel, reply);
}

static void DoOnStopDtmf(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnStopDtmf(messageParcel, reply);
}

static void DoOnSetMute(FuzzedDataProvider &provider)
{
    bool isMute = provider.ConsumeBool();
    MessageParcel messageParcel;
    messageParcel.WriteBool(isMute);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetMute(messageParcel, reply);
}

static void DoOnSetAudioDevice(FuzzedDataProvider &provider)
{
    std::string address = provider.ConsumeRandomLengthString();
    std::string deviceName = provider.ConsumeRandomLengthString();
    AudioDevice audioDevice;
    if (memset_s(&audioDevice, sizeof(AudioDevice), 0, sizeof(AudioDevice)) != EOK) {
        return;
    }
    audioDevice.deviceType = static_cast<AudioDeviceType>(
        provider.ConsumeIntegral<int32_t>() % AUDIO_DEVICE_NUM);
    size_t addressLen = address.length() > static_cast<size_t>(kMaxAddressLen)
        ? static_cast<size_t>(kMaxAddressLen) : address.length();
    if (memcpy_s(audioDevice.address, kMaxAddressLen + 1, address.c_str(), addressLen) != EOK) {
        return;
    }
    size_t nameLen = deviceName.length() > static_cast<size_t>(kMaxDeviceNameLen)
        ? static_cast<size_t>(kMaxDeviceNameLen) : deviceName.length();
    if (memcpy_s(audioDevice.deviceName, kMaxDeviceNameLen + 1, deviceName.c_str(), nameLen) != EOK) {
        return;
    }
    MessageParcel messageParcel;
    messageParcel.WriteRawData((const void *)&audioDevice, sizeof(AudioDevice));
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetAudioDevice(messageParcel, reply);
}

static void DoOnControlCamera(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::string cameraId = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString16(Str8ToStr16(cameraId));
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnControlCamera(messageParcel, reply);
}

static void DoOnSetPreviewWindow(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::string surfaceId = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString(surfaceId);
    if (provider.ConsumeBool()) {
        sptr<CallAbilityCallback> callback = new CallAbilityCallback();
        messageParcel.WriteRemoteObject(callback->AsObject());
    } else {
        messageParcel.WriteRemoteObject(nullptr);
    }
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetPreviewWindow(messageParcel, reply);
}

static void DoOnSetDisplayWindow(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::string surfaceId = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString(surfaceId);
    if (provider.ConsumeBool()) {
        sptr<CallAbilityCallback> callback = new CallAbilityCallback();
        messageParcel.WriteRemoteObject(callback->AsObject());
    } else {
        messageParcel.WriteRemoteObject(nullptr);
    }
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetDisplayWindow(messageParcel, reply);
}

static void DoOnSetCameraZoom(FuzzedDataProvider &provider)
{
    float zoom = provider.ConsumeFloatingPoint<float>();
    MessageParcel messageParcel;
    messageParcel.WriteFloat(zoom);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetCameraZoom(messageParcel, reply);
}

static void DoOnSetPausePicture(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::string path = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString16(Str8ToStr16(path));
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetPausePicture(messageParcel, reply);
}

static void DoOnSetDeviceDirection(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    int32_t rotation = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteInt32(rotation);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetDeviceDirection(messageParcel, reply);
}

static void DoOnCancelCallUpgrade(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnCancelCallUpgrade(messageParcel, reply);
}

static void DoOnRequestCameraCapabilities(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRequestCameraCapabilities(messageParcel, reply);
}

using FuzzApiHandler = void (*)(FuzzedDataProvider &);
constexpr FuzzApiHandler MULTIMEDIA_API_HANDLERS[] = {
    DoOnCombineConference,
    DoOnSeparateConference,
    DoOnJoinConference,
    DoOnGetMainCallId,
    DoOnGetSubCallIdList,
    DoOnGetCallIdListForConference,
    DoOnStartDtmf,
    DoOnStopDtmf,
    DoOnSetMute,
    DoOnSetAudioDevice,
    DoOnControlCamera,
    DoOnSetPreviewWindow,
    DoOnSetDisplayWindow,
    DoOnSetCameraZoom,
    DoOnSetPausePicture,
    DoOnSetDeviceDirection,
    DoOnCancelCallUpgrade,
    DoOnRequestCameraCapabilities,
};
constexpr uint32_t MULTIMEDIA_API_TARGET_COUNT =
    sizeof(MULTIMEDIA_API_HANDLERS) / sizeof(MULTIMEDIA_API_HANDLERS[0]);

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    if (!IsServiceInited()) {
        return;
    }
    FuzzedDataProvider provider(data, size);
    uint32_t targetIndex =
        provider.ConsumeIntegralInRange<uint32_t>(0, MULTIMEDIA_API_TARGET_COUNT - 1);
    MULTIMEDIA_API_HANDLERS[targetIndex](provider);
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
