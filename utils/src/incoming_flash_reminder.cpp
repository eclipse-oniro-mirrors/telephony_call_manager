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
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
#include "input/camera_manager.h"
#endif
#ifdef ABILITY_SCREENLOCKMGR_SUPPORT
#include "screenlock_manager.h"
#endif

namespace OHOS {
namespace Telephony {
constexpr int64_t DELAY_SET_TORCH_MODE_TIME = 300;
constexpr uint32_t DELAY_SET_TORCH_EVENT = 1000000;
constexpr uint32_t STOP_FLASH_REMIND_EVENT = 1000001;
constexpr uint32_t START_FLASH_REMIND_EVENT = 1000002;
const std::string FLASH_REMINDER_SWITCH_SUBSTRING = "INCOMING_CALL";
IncomingFlashReminder::IncomingFlashReminder(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    std::function<void()> startFlashRemindDone, std::function<void()> stopFlashRemindDone)
    : AppExecFwk::EventHandler(runner), startFlashRemindDone_(std::move(startFlashRemindDone)),
    stopFlashRemindDone_(std::move(stopFlashRemindDone)) {}

IncomingFlashReminder::~IncomingFlashReminder()
{
    if (!isFlashRemindUsed_) {
        TELEPHONY_LOGI("no need to stop");
        return;
    }
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    int32_t result = camMgr->SetTorchMode(CameraStandard::TORCH_MODE_OFF);
    TELEPHONY_LOGI("set torch mode result: %{public}d", result);
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
    if (!IsFlashReminderSwitchOn()) {
        TELEPHONY_LOGI("flash remind switch off");
        return false;
    }

    return IsScreenStatusSatisfied() && IsTorchReady();
}

bool IncomingFlashReminder::IsScreenStatusSatisfied()
{
#ifdef ABILITY_SCREENLOCKMGR_SUPPORT
    if (!OHOS::ScreenLock::ScreenLockManager::GetInstance()->IsScreenLocked()) {
        TELEPHONY_LOGI("screen is unlocked");
        return false;
    }
    return true;
#else
    TELEPHONY_LOGI("screen manager not support");
    return false;
#endif
}

bool IncomingFlashReminder::IsTorchReady()
{
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    if (!camMgr->IsTorchSupported()) {
        TELEPHONY_LOGI("torch not support");
        return false;
    }
    CameraStandard::TorchMode currentMode = camMgr->GetTorchMode();
    if (currentMode == CameraStandard::TORCH_MODE_ON) {
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
    if (result != TELEPHONY_SUCCESS || value != "1") {
        TELEPHONY_LOGI("off");
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
    isFlashRemindUsed_ = true;
    SendEvent(AppExecFwk::InnerEvent::Get(DELAY_SET_TORCH_EVENT, 0));
    if (startFlashRemindDone_ != nullptr) {
        startFlashRemindDone_();
    }
}

void IncomingFlashReminder::HandleSetTorchMode()
{
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    CameraStandard::TorchMode currentMode = camMgr->GetTorchMode();
    CameraStandard::TorchMode nextMode = (currentMode == CameraStandard::TORCH_MODE_ON?
        CameraStandard::TORCH_MODE_OFF : CameraStandard::TORCH_MODE_ON);
    int32_t result = camMgr->SetTorchMode(nextMode);
    TELEPHONY_LOGI("set torch mode result: %{public}d", result);
    SendEvent(AppExecFwk::InnerEvent::Get(DELAY_SET_TORCH_EVENT, 0), DELAY_SET_TORCH_MODE_TIME);
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
    isFlashRemindUsed_ = false;
    RemoveEvent(DELAY_SET_TORCH_EVENT);
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    sptr<CameraStandard::CameraManager> camMgr = CameraStandard::CameraManager::GetInstance();
    int32_t result = camMgr->SetTorchMode(CameraStandard::TORCH_MODE_OFF);
    TELEPHONY_LOGI("set torch mode result: %{public}d", result);
#endif
    if (stopFlashRemindDone_ != nullptr) {
        stopFlashRemindDone_();
    }
}
}
}