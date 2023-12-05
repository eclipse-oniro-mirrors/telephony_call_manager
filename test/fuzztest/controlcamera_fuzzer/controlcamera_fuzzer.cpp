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

using namespace OHOS::Telephony;
namespace OHOS {

int32_t ControlCamera(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = static_cast<int32_t>(size);
    std::string cameraId(reinterpret_cast<const char *>(data), size);
    auto cameraIdU16 = Str8ToStr16(cameraId);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString16(cameraIdU16);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnControlCamera(dataParcel, reply);
}

int32_t SetPreviewWindow(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = static_cast<int32_t>(size);
    std::string surfaceId(reinterpret_cast<const char *>(data), size);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(callId);
    if (surfaceId.empty() || surfaceId[0] < '0' || surfaceId[0] > '9') {
        surfaceId = "";
        dataParcel.WriteString(surfaceId);
    } else {
        int len = static_cast<int>(surfaceId.length());
        std::string subSurfaceId = surfaceId;
        if (len >= 1) {
            subSurfaceId = surfaceId.substr(0, 1);
        }
        dataParcel.WriteString(subSurfaceId);
        uint64_t tmpSurfaceId = std::stoull(subSurfaceId);
        auto surface = SurfaceUtils::GetInstance()->GetSurface(tmpSurfaceId);
        if (surface != nullptr) {
            sptr<IBufferProducer> producer = surface->GetProducer();
            if (producer != nullptr) {
                dataParcel.WriteRemoteObject(producer->AsObject());
            }
        }
    }
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetPreviewWindow(dataParcel, reply);
}

int32_t SetDisplayWindow(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = static_cast<int32_t>(size);
    std::string surfaceId(reinterpret_cast<const char *>(data), size);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(callId);
    if (surfaceId.empty() || surfaceId[0] < '0' || surfaceId[0] > '9') {
        surfaceId = "";
        dataParcel.WriteString(surfaceId);
    } else {
        int len = static_cast<int>(surfaceId.length());
        std::string subSurfaceId = surfaceId;
        if (len >= 1) {
            subSurfaceId = surfaceId.substr(0, 1);
        }
        dataParcel.WriteString(subSurfaceId);
        uint64_t tmpSurfaceId = std::stoull(subSurfaceId);
        auto surface = SurfaceUtils::GetInstance()->GetSurface(tmpSurfaceId);
        if (surface != nullptr) {
            sptr<IBufferProducer> producer = surface->GetProducer();
            if (producer != nullptr) {
                dataParcel.WriteRemoteObject(producer->AsObject());
            }
        }
    }
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetDisplayWindow(dataParcel, reply);
}

int32_t SetCameraZoom(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    float zoomRatio = static_cast<float>(size);
    MessageParcel dataParcel;
    dataParcel.WriteFloat(zoomRatio);
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetCameraZoom(dataParcel, reply);
}

int32_t SetPausePicture(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = static_cast<int32_t>(size);
    std::string path(reinterpret_cast<const char *>(data), size);
    auto pathU16 = Str8ToStr16(path);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString16(pathU16);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetPausePicture(dataParcel, reply);
}

int32_t SetDeviceDirection(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = static_cast<int32_t>(size);
    int32_t rotation = static_cast<int32_t>(size);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(callId);
    dataParcel.WriteInt32(rotation);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetDeviceDirection(dataParcel, reply);
}

int32_t CancelCallUpgrade(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = static_cast<int32_t>(size);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(callId);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnCancelCallUpgrade(dataParcel, reply);
}

int32_t RequestCameraCapabilities(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = static_cast<int32_t>(size);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(callId);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnRequestCameraCapabilities(dataParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    ControlCamera(data, size);
    SetPreviewWindow(data, size);
    SetDisplayWindow(data, size);
    SetCameraZoom(data, size);
    SetPausePicture(data, size);
    SetDeviceDirection(data, size);
    CancelCallUpgrade(data, size);
    RequestCameraCapabilities(data, size);
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
