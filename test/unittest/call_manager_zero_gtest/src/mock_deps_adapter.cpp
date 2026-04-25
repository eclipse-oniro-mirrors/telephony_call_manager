/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include <stdbool.h>
#include <cstdint>

namespace OHOS {
namespace Telephony {

static bool g_isTorchSupported = true;
static int g_torchMode = 0;
static bool g_isScreenLocked = true;
static int g_setTorchModeResult = 0;
static int g_freeCameraResult = 0;

void Mock_Reset()
{
    g_isTorchSupported = true;
    g_torchMode = 0;
    g_isScreenLocked = true;
    g_setTorchModeResult = 0;
    g_freeCameraResult = 0;
}

void Mock_SetIsTorchSupported(bool value)
{
    g_isTorchSupported = value;
}

void Mock_SetTorchMode(int mode)
{
    g_torchMode = mode;
}

void Mock_SetIsScreenLocked(bool value)
{
    g_isScreenLocked = value;
}

void Mock_SetSetTorchModeResult(int result)
{
    g_setTorchModeResult = result;
}

void Mock_SetFreeCameraResult(int result)
{
    g_freeCameraResult = result;
}

int Mock_GetTorchMode()
{
    return g_torchMode;
}

} // namespace Telephony
} // namespace OHOS

extern "C" {
bool IsTorchSupported()
{
    return OHOS::Telephony::g_isTorchSupported;
}

int GetTorchMode()
{
    return OHOS::Telephony::g_torchMode;
}

int32_t SetTorchMode(int mode)
{
    OHOS::Telephony::g_torchMode = mode;
    return OHOS::Telephony::g_setTorchModeResult;
}

bool IsScreenLocked()
{
    return OHOS::Telephony::g_isScreenLocked;
}

int FreeCamera()
{
    return OHOS::Telephony::g_freeCameraResult;
}
}