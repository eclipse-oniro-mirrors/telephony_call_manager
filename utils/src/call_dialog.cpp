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

namespace OHOS {
namespace Telephony {
constexpr int32_t DEFALUT_VALUE = -1;

bool CallDialog::DialogConnectExtension(const std::string &dialogReason)
{
    std::string commandStr = BuildStartCommand(dialogReason);
    AAFwk::Want want;
    std::string bundleName = "com.ohos.sceneboard";
    std::string abilityName = "com.ohos.sceneboard.systemdialog";
    want.SetElementName(bundleName, abilityName);
    bool connetResult = DialogConnectExtensionAbility(want, commandStr);
    if (!connetResult) {
        TELEPHONY_LOGE("DialogConnectExtensionAbility failed!");
        return false;
    }
    return true;
}

bool CallDialog::DialogConnectExtensionAbility(const AAFwk::Want &want, const std::string commandStr)
{
    TELEPHONY_LOGI("DialogConnectExtensionAbility start");
    connection_ = sptr<CallAbilityConnection> (new (std::nothrow) CallAbilityConnection(commandStr));
    if (connection_ == nullptr) {
        TELEPHONY_LOGE("connection_ is nullptr");
        return false;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto connetResult = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want, connection_,
        nullptr, DEFALUT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    if (connetResult != 0) {
        TELEPHONY_LOGE("ConnectServiceExtensionAbility is not ERR_OK!");
        return false;
    }
    return true;
}

std::string CallDialog::BuildStartCommand(const std::string &dialogReason)
{
    nlohmann::json root;
    std::string uiExtensionType = "sysDialog/common";
    root["ability.want.params.uiExtensionType"] = uiExtensionType;
    root["dialogReason"] = dialogReason;
    std::string startCommand = root.dump();
    TELEPHONY_LOGI("startCommand is: %{public}s", startCommand.c_str());
    return startCommand;
}
} // namespace Telephony
} // namespace OHOS