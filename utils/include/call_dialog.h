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

#ifndef CALL_DIALOG_H
#define CALL_DIALOG_H

#include <string>

#include "call_ability_connection.h"
#include "call_setting_ability_connection.h"
#include "display_info.h"

namespace OHOS {
namespace Telephony {
class CallDialog {
public:
    bool DialogConnectExtension(const std::string &dialogReason);
    bool DialogConnectExtension(const std::string &dialogReason, int32_t slotId);
    bool DialogConnectPrivpacyModeExtension(const std::string &dialogReason, std::u16string &number, int32_t &accountId,
    int32_t &videoState, int32_t &dialType, int32_t &dialScene, int32_t &callType, bool isVideo);
    bool DialogConnectAnswerPrivpacyModeExtension(const std::string &dialogReason,
        int32_t &callId, int32_t &videoState, bool isVideo);
    void DialogCallingPrivacyModeExtension(Rosen::FoldStatus foldStatus);
    void DialogProcessMMICodeExtension();

private:
    bool DialogConnectExtensionAbility(const AAFwk::Want &want, const std::string commandStr);
    bool CallSettingDialogConnectExtensionAbility(const AAFwk::Want &want, const std::string commandStr);
    std::string BuildStartCommand(const std::string &dialogReason, int32_t slotId);
    std::string BuildStartPrivpacyModeCommand(const std::string &dialogReason, std::u16string &number,
	int32_t &accountId, int32_t &videoState, int32_t &dialType, int32_t &dialScene, int32_t &callType, bool isVideo);
    std::string BuildStartAnswerPrivpacyModeCommand(const std::string &dialogReason,
        int32_t &callId, int32_t &videoState, bool isVideo);
    std::string BuildCallingPrivacyModeCommand(int32_t &videoState);
    std::string BuildProcessMMICodeCommand();
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_DIALOG_H