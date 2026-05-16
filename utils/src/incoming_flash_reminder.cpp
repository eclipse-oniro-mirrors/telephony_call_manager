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

#include "incoming_flash_reminder.h"

#include "os_account_manager.h"
#include "settings_datashare_helper.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
constexpr const char* LIB_TEL_CM_ADAPTER_PATH = "libtel_cm_deps_adapter.z.so";
constexpr int64_t DELAY_SET_TORCH_MODE_TIME = 300;
using IsTorchSupportedFunc = bool(*)();
using GetTorchModeFunc = int (*)();
using SetTorchModeFunc = int (*)(int mode);
using FreeCameraFunc = int (*)();
enum class TelTorchMode {
    TORCH_MODE_UNKNOWN = -1,
    TORCH_MODE_OFF = 0,
    TORCH_MODE_ON = 1,
    TORCH_MODE_AUTO = 2
};
#endif
#ifdef ABILITY_SCREENLOCKMGR_SUPPORT
    using IsScreenLockedFunc = bool (*)();
#endif
constexpr uint32_t DELAY_SET_TORCH_EVENT = 1000000;
constexpr uint32_t STOP_FLASH_REMIND_EVENT = 1000001;
constexpr uint32_t START_FLASH_REMIND_EVENT = 1000002;
const std::string FLASH_REMINDER_SWITCH_SUBSTRING = "INCOMING_CALL";
IncomingFlashReminder::IncomingFlashReminder(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    std::function<void()> stopFlashRemindDone)
    : AppExecFwk::EventHandler(runner), stopFlashRemindDone_(std::move(stopFlashRemindDone)) {}

IncomingFlashReminder::~IncomingFlashReminder()
{
    if (!isFlashRemindUsed_) {
        TELEPHONY_LOGI("no need to stop");
        return;
    }
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    if (libAdapterHandler_ == nullptr) {
        TELEPHONY_LOGE("deps adapter is nullptr");
        return;
    }
    SetTorchModeFunc setTorchMode = reinterpret_cast<SetTorchModeFunc>
        (dlsym(libAdapterHandler_, "SetTorchMode"));
    if (setTorchMode == nullptr) {
        return;
    }
    setTorchMode(static_cast<int>(TelTorchMode::TORCH_MODE_OFF));
    ReleaseDepsAdapter();
#endif
}

void IncomingFlashReminder::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    switch (event->GetInnerEventId()) {
        case DELAY_SET_TORCH_EVENT:
            HandleSetTorchMode();
            break;
        case STOP_FLASH_REMIND_EVENT:
            HandleStopFlashRemind();
            break;
        case START_FLASH_REMIND_EVENT:
            HandleStartFlashRemind();
            break;
        default:
            TELEPHONY_LOGE("receive unknown event %{public}u", event->GetInnerEventId());
            break;
    }
}

bool IncomingFlashReminder::IsFlashRemindNecessary()
{
    return IsScreenStatusSatisfied() && IsTorchReady();
}

bool IncomingFlashReminder::IsScreenStatusSatisfied()
{
#ifdef ABILITY_SCREENLOCKMGR_SUPPORT
    if (libAdapterHandler_ == nullptr) {
        TELEPHONY_LOGE("deps adapter is nullptr");
        return false;
    }
    IsScreenLockedFunc isScreenLocked =
        reinterpret_cast<IsScreenLockedFunc>(dlsym(libAdapterHandler_, "IsScreenLocked"));
    if (isScreenLocked == nullptr) {
        return false;
    }

    return isScreenLocked();
#else
    TELEPHONY_LOGI("screen manager not support");
    return false;
#endif
}

bool IncomingFlashReminder::IsTorchReady()
{
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    if (libAdapterHandler_ == nullptr) {
        TELEPHONY_LOGE("deps adapter is nullptr");
        return false;
    }

    IsTorchSupportedFunc isTorchSupported = reinterpret_cast<IsTorchSupportedFunc>
        (dlsym(libAdapterHandler_, "IsTorchSupported"));
    GetTorchModeFunc getTorchMode = reinterpret_cast<GetTorchModeFunc>
        (dlsym(libAdapterHandler_, "GetTorchMode"));
    if (isTorchSupported == nullptr || getTorchMode == nullptr) {
        return false;
    }
    if (!isTorchSupported()) {
        return false;
    }
    
    TelTorchMode currentMode = static_cast<TelTorchMode>(getTorchMode());
    if (currentMode == TelTorchMode::TORCH_MODE_ON) {
        TELEPHONY_LOGI("torch being used");
        return false;
    }
    return true;
#else
    TELEPHONY_LOGI("camera manager not support");
    return false;
#endif
}

bool IncomingFlashReminder::IsFlashReminderSwitchOn()
{
    std::vector<int> activedOsAccountIds;
    OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(activedOsAccountIds);
    if (activedOsAccountIds.empty()) {
        TELEPHONY_LOGW("activedOsAccountIds is empty");
        return false;
    }
    int userId = activedOsAccountIds[0];
    OHOS::Uri uri(
        "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_SECURE_"
        + std::to_string(userId) + "?Proxy=true");
    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    std::string value;
    int32_t result = datashareHelper->Query(uri, "", value);
    bool isSwitchOn = (result == TELEPHONY_SUCCESS && value == "1");
    if (!isSwitchOn) {
        TELEPHONY_LOGI("switch off");
        return false;
    }
    result = datashareHelper->Query(uri, "", value);
    TELEPHONY_LOGI("query reminder switch, result: %{public}d", result);
    return (result == TELEPHONY_SUCCESS && value.find(FLASH_REMINDER_SWITCH_SUBSTRING) != std::string::npos);
}

void IncomingFlashReminder::StartFlashRemind()
{
    SendEvent(AppExecFwk::InnerEvent::Get(START_FLASH_REMIND_EVENT, 0));
}

void IncomingFlashReminder::HandleStartFlashRemind()
{
    if (isFlashRemindUsed_) {
        return;
    }
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    if (!IsFlashReminderSwitchOn()) {
        TELEPHONY_LOGI("flash remind switch off");
        return;
    }
    libAdapterHandler_ = dlopen(LIB_TEL_CM_ADAPTER_PATH, RTLD_LAZY);
    if (libAdapterHandler_ == nullptr) {
        TELEPHONY_LOGE("deps adapter dlopen failed : %{public}s", dlerror());
        return;
    }
    if (!IsFlashRemindNecessary()) {
        ReleaseDepsAdapter();
        return;
    }
#endif
    isFlashRemindUsed_ = true;
    SendEvent(AppExecFwk::InnerEvent::Get(DELAY_SET_TORCH_EVENT, 0));
}

void IncomingFlashReminder::HandleSetTorchMode()
{
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    TelTorchMode result = TelTorchMode::TORCH_MODE_UNKNOWN;
    if (libAdapterHandler_ == nullptr) {
        TELEPHONY_LOGE("deps adapter is nullptr");
        return;
    }

    GetTorchModeFunc getTorchMode = reinterpret_cast<GetTorchModeFunc>(dlsym(libAdapterHandler_, "GetTorchMode"));
    SetTorchModeFunc setTorchMode = reinterpret_cast<SetTorchModeFunc>(dlsym(libAdapterHandler_, "SetTorchMode"));
    if (getTorchMode == nullptr || setTorchMode == nullptr) {
        return;
    }

    TelTorchMode currentMode = static_cast<TelTorchMode>(getTorchMode());
    TelTorchMode nextMode = (currentMode == TelTorchMode::TORCH_MODE_ON?
        TelTorchMode::TORCH_MODE_OFF : TelTorchMode::TORCH_MODE_ON);
    result = static_cast<TelTorchMode>(setTorchMode(static_cast<int>(nextMode)));

    SendEvent(AppExecFwk::InnerEvent::Get(DELAY_SET_TORCH_EVENT, 0), DELAY_SET_TORCH_MODE_TIME);
    TELEPHONY_LOGI("set torch mode result: %{public}d", result);
#endif
}

void IncomingFlashReminder::StopFlashRemind()
{
    SendEvent(AppExecFwk::InnerEvent::Get(STOP_FLASH_REMIND_EVENT, 0));
}

void IncomingFlashReminder::HandleStopFlashRemind()
{
    if (!isFlashRemindUsed_) {
        TELEPHONY_LOGI("no need to stop");
        if (stopFlashRemindDone_ != nullptr) {
            stopFlashRemindDone_();
        }
        return;
    }
    RemoveEvent(DELAY_SET_TORCH_EVENT);
    isFlashRemindUsed_ = false;
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    if (libAdapterHandler_ == nullptr) {
        TELEPHONY_LOGE("deps adapter is nullptr");
        return;
    }

    SetTorchModeFunc setTorchMode = reinterpret_cast<SetTorchModeFunc>(dlsym(libAdapterHandler_, "SetTorchMode"));
    if (setTorchMode == nullptr) {
        return;
    }

    setTorchMode(static_cast<int>(TelTorchMode::TORCH_MODE_OFF));
    ReleaseDepsAdapter();
#endif
    if (stopFlashRemindDone_ != nullptr) {
        stopFlashRemindDone_();
    }
}

void IncomingFlashReminder::ReleaseDepsAdapter()
{
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    if (libAdapterHandler_ == nullptr) {
        return;
    }
    FreeCameraFunc freeCameraFunc = reinterpret_cast<FreeCameraFunc>(dlsym(libAdapterHandler_, "FreeCamera"));
    if (freeCameraFunc == nullptr) {
        return;
    }
    int32_t result = freeCameraFunc();
    if (result == TELEPHONY_SUCCESS) {
        dlclose(libAdapterHandler_);
        libAdapterHandler_ = nullptr;
    }
#endif
}
}
}