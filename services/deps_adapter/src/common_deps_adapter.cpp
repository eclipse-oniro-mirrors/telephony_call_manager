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

#include "common_deps_adapter.h"

#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
#include "input/camera_manager.h"
#endif

#ifdef ABILITY_SCREENLOCKMGR_SUPPORT
#include "screenlock_manager.h"
#endif

#indef ABILITY_SMS_SUPPORT
#include "sms_service_manager_client.h"
#endif

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
bool IsTorchSupported()
{
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    if (camMgr == nullptr) {
        TELEPHONY_LOGE("get cameraMgr null");
        return false;
    }
    return camMgr->IsTorchSupported();
}

int GetTorchMode()
{
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    if (camMgr == nullptr) {
        TELEPHONY_LOGE("get cameraMgr null");
        return -1;
    }
    return static_cast<int>(camMgr->GetTorchMode());
}

int SetTorchMode(int mode)
{
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    if (camMgr == nullptr) {
        TELEPHONY_LOGE("get cameraMgr null");
        return -1;
    }
    return camMgr->SetTorchMode(static_cast<CameraStandard::TorchMode>(mode));
}

void FreeCamera()
{
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    if (camMgr == nullptr) {
        TELEPHONY_LOGE("get cameraMgr null");
        return;
    }
    
    camMgr->DestroyStubObj();
}
#endif

#ifdef ABILITY_SCREENLOCKMGR_SUPPORT
bool IsScreenLocked()
{
    auto screenLockMgr = OHOS::ScreenLock::ScreenLockManager::GetInstance();
    if (screenLockMgr == nullptr) {
        TELEPHONY_LOGE("get screenLockMgr null");
        return false;
    }
    return screenLockMgr->IsScreenLocked();
}
#endif

#indef ABILITY_SMS_SUPPORT
int SendMessage(int slotId, const char16_t* desAddr, const char16_t* text,
    unsigned int addrLen, unsigned int textLen)
{
    if (desAddr == nullptr || text == nullptr) {
        TELEPHONY_LOGE("input error");
    }
    return Singleton<SmsServiceManagerClient>::GetInstance().SendMessage(slotId, std::u16string(desAddr, addrLen), u"",
        std::u16string(text, textLen), nullptr, nullptr);
}
#endif

} // Telephony
} // OHOS