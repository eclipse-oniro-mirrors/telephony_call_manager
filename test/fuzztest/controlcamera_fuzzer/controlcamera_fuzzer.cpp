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
#include "call_manager_service.h"
#include "system_ability_definition.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;

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

int32_t ControlCamera(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    std::string cameraId(reinterpret_cast<const char *>(data), size);
    auto cameraIdU16 = Str8ToStr16(cameraId);
    MessageParcel dataParcel;
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
    VideoWindow window;
    window.x = static_cast<int32_t>(size);
    window.y = static_cast<int32_t>(size);
    window.z = static_cast<int32_t>(size);
    window.width = static_cast<int32_t>(size);
    window.height = static_cast<int32_t>(size);
    MessageParcel dataParcel;
    MessageParcel reply;
    dataParcel.WriteRawData(static_cast<const void *>(&window), sizeof(VideoWindow));
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetPreviewWindow(dataParcel, reply);
}

int32_t SetDisplayWindow(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    VideoWindow window;
    window.x = static_cast<int32_t>(size);
    window.y = static_cast<int32_t>(size);
    window.z = static_cast<int32_t>(size);
    window.width = static_cast<int32_t>(size);
    window.height = static_cast<int32_t>(size);
    MessageParcel dataParcel;
    MessageParcel reply;
    dataParcel.WriteRawData(static_cast<const void *>(&window), sizeof(VideoWindow));
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

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    ControlCamera(data, size);
    SetPreviewWindow(data, size);
    SetDisplayWindow(data, size);
    SetCameraZoom(data, size);
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
