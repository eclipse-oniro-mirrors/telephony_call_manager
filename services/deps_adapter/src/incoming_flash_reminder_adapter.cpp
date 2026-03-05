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

#include "incoming_flash_reminder_adapter.h"
#include "input/camera_manager.h"
#include "screenlock_manager.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
#ifdef __cplusplus
extern "C" {
#endif

bool IsTorchSupported()
{
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    if (camMgr == nullptr) {
        TELEPHONY_LOGE("get cameraMgr null");
        return false;
    }
    return camMgr->IsTorchSupported();
}

TelTorchMode GetTorchMode()
{
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    if (camMgr == nullptr) {
        TELEPHONY_LOGE("get cameraMgr null");
        return TelTorchMode::TORCH_MODE_UNKNOWN;
    }
    return static_cast<TelTorchMode>(camMgr->GetTorchMode());
}

int SetTorchMode(TelTorchMode mode)
{
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    if (camMgr == nullptr) {
        TELEPHONY_LOGE("get cameraMgr null");
        return -1;
    }
    return camMgr->SetTorchMode(static_cast<CameraStandard::TorchMode>(mode));
}

bool IsScreenLocked()
{
    auto screenLockMgr = OHOS::ScreenLock::ScreenLockManager::GetInstance();
    if (screenLockMgr == nullptr) {
        TELEPHONY_LOGE("get screenLockMgr null");
        return false;
    }
    return screenLockMgr->IsScreenLocked();
}

#ifdef __cplusplus
}
#endif
}
}