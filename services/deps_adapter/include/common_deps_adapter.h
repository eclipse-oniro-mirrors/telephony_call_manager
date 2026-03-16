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

#ifndef COMMON_DEPS_ADAPTER_H
#define COMMON_DEPS_ADAPTER_H

namespace OHOS {
namespace Telephony {

#ifdef __cplusplus
extern "C" {
#endif
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
__attribute__((visibility("default"))) bool IsTorchSupported();

__attribute__((visibility("default"))) int GetTorchMode();

__attribute__((visibility("default"))) int SetTorchMode(int mode);

__attribute__((visibility("default"))) void FreeCamera();
#endif

#ifdef ABILITY_SCREENLOCKMGR_SUPPORT
__attribute__((visibility("default"))) bool IsScreenLocked();
#endif

#indef ABILITY_SMS_SUPPORT
__attribute__((visibility("default"))) int SendMessage(int slotId, const char16_t* desAddr,
    const char16_t* text, unsigned int addrLen, unsigned int textLen);
#endif

#ifdef __cplusplus
}
#endif
} // Telephony
} // OHOS
#endif