/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "call_dialog.h"

#include "extension_manager_client.h"
#include "ipc_skeleton.h"
#include "nlohmann/json.hpp"
#include "telephony_log_wrapper.h"
#include <securec.h>
#include "display_manager.h"
#include "fold_status_manager.h"
#include "call_object_manager.h"
#include "call_superprivacy_control_manager.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t DEFAULT_USER_ID = -1;
const int32_t SCB_SYS_DIALOG_ZORDER_UNDER_LOCK = 1;
const int32_t SOURCE_SCREENLOCKED = 2;

bool CallDialog::DialogConnectExtension(const std::string &dialogReason)
{
    return DialogConnectExtension(dialogReason, -1);
}

bool CallDialog::DialogConnectExtension(const std::string &dialogReason, int32_t slotId)
{
    std::string commandStr = BuildStartCommand(dialogReason, slotId);
    AAFwk::Want want;
    std::string bundleName = "com.ohos.sceneboard";
    std::string abilityName = "com.ohos.sceneboard.systemdialog";
    want.SetElementName(bundleName, abilityName);
    bool connectResult = false;
    if (dialogReason.find("SATELLITE") != std::string::npos) {
        connectResult = CallSettingDialogConnectExtensionAbility(want, commandStr);
    } else {
        connectResult = DialogConnectExtensionAbility(want, commandStr);
    }
    if (!connectResult) {
        TELEPHONY_LOGE("DialogConnectExtensionAbility failed!");
        return false;
    }
    return true;
}

bool CallDialog::DialogConnectExtensionAbility(const AAFwk::Want &want, const std::string commandStr)
{
    TELEPHONY_LOGI("DialogConnectExtensionAbility start");
    sptr<CallAbilityConnection> connection =
        sptr<CallAbilityConnection> (new (std::nothrow) CallAbilityConnection(commandStr));
    if (connection == nullptr) {
        TELEPHONY_LOGE("connection is nullptr");
        return false;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto connectResult = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want,
        connection, nullptr, DEFAULT_USER_ID);
    IPCSkeleton::SetCallingIdentity(identity);
    if (connectResult != 0) {
        TELEPHONY_LOGE("ConnectServiceExtensionAbility Failed!");
        return false;
    }
    return true;
}

bool CallDialog::CallSettingDialogConnectExtensionAbility(const AAFwk::Want &want,
    const std::string commandStr)
{
    TELEPHONY_LOGI("CallSettingDialogConnectExtensionAbility start");
    sptr<CallSettingAbilityConnection> callSettingConnection = sptr<CallSettingAbilityConnection> (new (std::nothrow)
        CallSettingAbilityConnection(commandStr));
    if (callSettingConnection == nullptr) {
        TELEPHONY_LOGE("connection is nullptr");
        return false;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto connectResult = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want,
        callSettingConnection, nullptr, DEFAULT_USER_ID);
    IPCSkeleton::SetCallingIdentity(identity);
    if (connectResult != 0) {
        TELEPHONY_LOGE("ConnectServiceExtensionAbility Failed!");
        return false;
    }
    return true;
}

std::string CallDialog::BuildStartCommand(const std::string &dialogReason, int32_t slotId)
{
    nlohmann::json root;
    std::string uiExtensionType = "sysDialog/common";
    root["ability.want.params.uiExtensionType"] = uiExtensionType;
    root["sysDialogZOrder"] = SOURCE_SCREENLOCKED;
    root["dialogReason"] = dialogReason;
    root["slotId"] = slotId;
    std::string startCommand = root.dump();
    TELEPHONY_LOGI("startCommand is: %{public}s", startCommand.c_str());
    return startCommand;
}

bool CallDialog::DialogConnectPrivpacyModeExtension(const std::string &dialogReason, std::u16string &number,
    int32_t &accountId, int32_t &videoState, int32_t &dialType, int32_t &dialScene, int32_t &callType, bool isVideo)
{
    std::string commandStr = BuildStartPrivpacyModeCommand(dialogReason, number,
    accountId, videoState, dialType, dialScene, callType, isVideo);
    AAFwk::Want want;
    std::string bundleName = "com.ohos.sceneboard";
    std::string abilityName = "com.ohos.sceneboard.systemdialog";
    want.SetElementName(bundleName, abilityName);
    bool connectResult = CallSettingDialogConnectExtensionAbility(want, commandStr);
    if (!connectResult) {
        TELEPHONY_LOGE("CallSettingDialogConnectExtensionAbility failed!");
        return false;
    }
    return true;
}

bool CallDialog::DialogConnectAnswerPrivpacyModeExtension(const std::string &dialogReason,
    int32_t &callId, int32_t &videoState, bool isVideo)
{
    std::string commandStr = BuildStartAnswerPrivpacyModeCommand(dialogReason, callId, videoState, isVideo);
    AAFwk::Want want;
    std::string bundleName = "com.ohos.sceneboard";
    std::string abilityName = "com.ohos.sceneboard.systemdialog";
    want.SetElementName(bundleName, abilityName);
    bool connectResult = CallSettingDialogConnectExtensionAbility(want, commandStr);
    if (!connectResult) {
        TELEPHONY_LOGE("CallSettingDialogConnectExtensionAbility failed!");
        return false;
    }
    return true;
}

void CallDialog::DialogCallingPrivacyModeExtension(Rosen::FoldStatus foldStatus)
{
    if (foldStatus != Rosen::FoldStatus::FOLDED
        || !DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->GetCurrentIsSuperPrivacyMode()) {
        return;
    }
    sptr<CallBase> foregroundCall = CallObjectManager::GetForegroundCall(false);
    if (foregroundCall == nullptr || foregroundCall->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING) {
        return;
    }
    int32_t videoState = static_cast<int32_t>(foregroundCall->GetVideoStateType());
    std::string commandStr = BuildCallingPrivacyModeCommand(videoState);
    AAFwk::Want want;
    std::string bundleName = "com.ohos.sceneboard";
    std::string abilityName = "com.ohos.sceneboard.systemdialog";
    want.SetElementName(bundleName, abilityName);
    bool connectResult = CallSettingDialogConnectExtensionAbility(want, commandStr);
    if (!connectResult) {
        TELEPHONY_LOGE("DialogCallingPrivacyModeExtension failed!");
    }
}

std::string CallDialog::BuildStartPrivpacyModeCommand(const std::string &dialogReason, std::u16string &number,
    int32_t &accountId, int32_t &videoState, int32_t &dialType, int32_t &dialScene, int32_t &callType, bool isVideo)
{
    nlohmann::json root;
    std::string uiExtensionType = "sysDialog/common";
    root["ability.want.params.uiExtensionType"] = uiExtensionType;
    root["sysDialogZOrder"] = SOURCE_SCREENLOCKED;
    root["dialogReason"] = dialogReason;
    root["number"] = Str16ToStr8(number);
    root["accountId"] = accountId;
    root["videoState"] = videoState;
    root["dialType"] = dialType;
    root["dialScene"] = dialScene;
    root["callType"] = callType;
    root["isAnswer"] = false;
    root["isVideo"] = isVideo;
    root["isFold"] = FoldStatusManager::IsSmallFoldDevice()
        && Rosen::DisplayManager::GetInstance().GetFoldStatus() == Rosen::FoldStatus::FOLDED;
    std::string startCommand = root.dump();
    TELEPHONY_LOGI("startCommand is: %{public}s", startCommand.c_str());
    return startCommand;
}

std::string CallDialog::BuildStartAnswerPrivpacyModeCommand(const std::string &dialogReason,
    int32_t &callId, int32_t &videoState, bool isVideo)
{
    nlohmann::json root;
    std::string uiExtensionType = "sysDialog/common";
    root["ability.want.params.uiExtensionType"] = uiExtensionType;
    root["sysDialogZOrder"] = SOURCE_SCREENLOCKED;
    root["dialogReason"] = dialogReason;
    root["callId"] = callId;
    root["videoState"] = videoState;
    root["isAnswer"] = true;
    root["isVideo"] = isVideo;
    root["isFold"] = FoldStatusManager::IsSmallFoldDevice()
        && Rosen::DisplayManager::GetInstance().GetFoldStatus() == Rosen::FoldStatus::FOLDED;
    std::string startCommand = root.dump();
    TELEPHONY_LOGI("startCommand is: %{public}s", startCommand.c_str());
    return startCommand;
}

std::string CallDialog::BuildCallingPrivacyModeCommand(int32_t &videoState)
{
    nlohmann::json root;
    std::string uiExtensionType = "sysDialog/common";
    std::string dialogReason = "SUPER_PRIVACY_MODE";
    root["ability.want.params.uiExtensionType"] = uiExtensionType;
    root["sysDialogZOrder"] = SOURCE_SCREENLOCKED;
    root["dialogReason"] = dialogReason;
    root["videoState"] = videoState;
    root["isInCall"] = true;
    root["isAnswer"] = false;
    root["isFold"] = true;
    std::string startCommand = root.dump();
    TELEPHONY_LOGI("startCommand is: %{public}s", startCommand.c_str());
    return startCommand;
}

void CallDialog::DialogProcessMMICodeExtension()
{
    std::string commandStr = BuildProcessMMICodeCommand();
    AAFwk::Want want;
    std::string bundleName = "com.ohos.sceneboard";
    std::string abilityName = "com.ohos.sceneboard.systemdialog";
    want.SetElementName(bundleName, abilityName);
    bool connectResult = CallSettingDialogConnectExtensionAbility(want, commandStr);
    if (!connectResult) {
        TELEPHONY_LOGE("CallSettingDialogConnectExtensionAbility failed!");
    }
}

std::string CallDialog::BuildProcessMMICodeCommand()
{
    TELEPHONY_LOGI("BuildProcessMMICodeCommand enter");
    nlohmann::json root;
    std::string uiExtensionType = "sysDialog/common";
    std::string dialogReason = "PROCESS_MMI_CODE";
    root["ability.want.params.uiExtensionType"] = uiExtensionType;
    root["sysDialogZOrder"] = SCB_SYS_DIALOG_ZORDER_UNDER_LOCK;
    root["dialogReason"] = dialogReason;
    std::string startCommand = root.dump();
    TELEPHONY_LOGI("startCommand is: %{public}s", startCommand.c_str());
    return startCommand;
}
} // namespace Telephony
} // namespace OHOS