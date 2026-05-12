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

#include "controlcamera_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "surface_utils.h"
#include "fuzzer/FuzzedDataProvider.h"

using namespace OHOS::Telephony;
namespace OHOS {

int32_t ControlCamera(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::string cameraId = provider.ConsumeRadomLengthString();
    auto cameraIdU16 = Str8ToStr16(cameraId);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString16(cameraIdU16);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnControlCamera(messageParcel, reply);
}

int32_t SetPreviewWindow(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::string surfaceId = provider.ConsumeRadomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    int len = static_cast<int>(surfaceId.length());
    std::string subSurfaceId = surfaceId;
    if (len >= 1) {
        subSurfaceId = surfaceId.substr(0, 1);
    }
    if (subSurfaceId.empty() || subSurfaceId[0] < '0' || subSurfaceId[0] > '9') {
        subSurfaceId = "";
        messageParcel.WriteString(subSurfaceId);
    } else {
        messageParcel.WriteString(subSurfaceId);
        uint64_t tmpSurfaceId = std::stoull(subSurfaceId);
        auto surface = SurfaceUtils::GetInstance()->GetSurface(tmpSurfaceId);
        if (surface != nullptr) {
            sptr<IBufferProducer> producer = surface->GetProducer();
            if (producer != nullptr) {
                messageParcel.WriteRemoteObject(producer->AsObject());
            }
        }
    }
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetPreviewWindow(messageParcel, reply);
}

int32_t SetDisplayWindow(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::string surfaceId = provider.ConsumeRadomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    int len = static_cast<int>(surfaceId.length());
    std::string subSurfaceId = surfaceId;
    if (len >= 1) {
        subSurfaceId = surfaceId.substr(0, 1);
    }
    if (subSurfaceId.empty() || subSurfaceId[0] < '0' || subSurfaceId[0] > '9') {
        subSurfaceId = "";
        messageParcel.WriteString(subSurfaceId);
    } else {
        messageParcel.WriteString(subSurfaceId);
        uint64_t tmpSurfaceId = std::stoull(subSurfaceId);
        auto surface = SurfaceUtils::GetInstance()->GetSurface(tmpSurfaceId);
        if (surface != nullptr) {
            sptr<IBufferProducer> producer = surface->GetProducer();
            if (producer != nullptr) {
                messageParcel.WriteRemoteObject(producer->AsObject());
            }
        }
    }
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetDisplayWindow(messageParcel, reply);
}

int32_t SetCameraZoom(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    float zoomRatio = provider.ConsumeIntegral<float>();
    MessageParcel messageParcel;
    messageParcel.WriteFloat(zoomRatio);
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetCameraZoom(messageParcel, reply);
}

int32_t SetPausePicture(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::string path = provider.ConsumeRadomLengthString();
    auto pathU16 = Str8ToStr16(path);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString16(pathU16);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetPausePicture(messageParcel, reply);
}

int32_t SetDeviceDirection(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    int32_t rotation = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteInt32(rotation);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetDeviceDirection(messageParcel, reply);
}

int32_t CancelCallUpgrade(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnCancelCallUpgrade(messageParcel, reply);
}

int32_t RequestCameraCapabilities(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnRequestCameraCapabilities(messageParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    FuzzedDataProvider provider(data, size);
    ControlCamera(provider);
    SetPreviewWindow(provider);
    SetDisplayWindow(provider);
    SetCameraZoom(provider);
    SetPausePicture(provider);
    SetDeviceDirection(provider);
    CancelCallUpgrade(provider);
    RequestCameraCapabilities(provider);
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
