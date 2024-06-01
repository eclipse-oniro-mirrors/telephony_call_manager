/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "call_superprivacy_control_manager.h"
#include "call_number_utils.h"
#include "super_privacy_kit.h"
#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
using namespace AppSecurityPrivacy::SecurityPrivacyServer::SuperPrivacy;

bool CallSuperPrivacyControlManager::GetIsChangeSuperPrivacyMode()
{
    return isChangeSuperPrivacyMode;
}

void CallSuperPrivacyControlManager::SetIsChangeSuperPrivacyMode(bool isChangeSuperPrivacy)
{
    isChangeSuperPrivacyMode = isChangeSuperPrivacy;
}

void CallSuperPrivacyControlManager::SetOldSuperPrivacyMode(int32_t superPrivacyMode)
{
    oldSuperPrivacyMode = superPrivacyMode;
}

int32_t CallSuperPrivacyControlManager::GetOldSuperPrivacyMode()
{
    return oldSuperPrivacyMode;
}

int32_t CallSuperPrivacyControlManager::CloseSuperPrivacyMode(
std::u16string &phoneNumber, int32_t &accountId, int32_t &videoState, int32_t &dialType, int32_t &dialScene, int32_t &callType)
{
    int32_t privacy = SuperPrivacyKit::SetSuperPrivacyMode(SuperPrivacyMode::OFF, Source::CALL);
    TELEPHONY_LOGE("CallSuperPrivacyControlManager CloseSuperPrivacyMode privacy:%{public}d", privacy);
    DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->SetIsChangeSuperPrivacyMode(true);
    if (privacy == 0) {
        TELEPHONY_LOGE("CallSuperPrivacyControlManager CloseSuperPrivacyMode is true");
        AppExecFwk::PacMap dialInfo;
        dialInfo.PutIntValue("accountId", accountId);
        dialInfo.PutIntValue("videoState", videoState);
        dialInfo.PutIntValue("dialType", dialType);
        dialInfo.PutIntValue("dialScene", dialScene);
        dialInfo.PutIntValue("callType", callType);
        int32_t ret = DelayedSingleton<CallControlManager>::GetInstance()->DialCall(phoneNumber, dialInfo);
        if (ret == TELEPHONY_SUCCESS) {
            return TELEPHONY_SUCCESS;
        }
    }
    return CALL_ERR_DIAL_FAILED;
}

int32_t CallSuperPrivacyControlManager::CloseAnswerSuperPrivacyMode(int32_t callId, int32_t videoState)
{
    int32_t privacy = SuperPrivacyKit::SetSuperPrivacyMode(SuperPrivacyMode::OFF, Source::CALL);
    TELEPHONY_LOGE("CallSuperPrivacyControlManager CloseAnswerSuperPrivacyMode privacy:%{public}d", privacy);
    DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->SetIsChangeSuperPrivacyMode(true);
    if (privacy == 0) {
        TELEPHONY_LOGE("CallSuperPrivacyControlManager CloseSuperPrivacyMode is true");
        int32_t ret = DelayedSingleton<CallControlManager>::GetInstance()->AnswerCall(callId, videoState);
        if (ret == TELEPHONY_SUCCESS) {
            return TELEPHONY_SUCCESS;
        }
    }
    return CALL_ERR_DIAL_FAILED;
}

void CallSuperPrivacyControlManager::restoreSuperPrivacyMode()
{
    bool isChangeSuperPrivacyMode = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->GetIsChangeSuperPrivacyMode();
    if (!isChangeSuperPrivacyMode) {
        return;
    }
    int32_t privpacyMode;
    int32_t privpacy = SuperPrivacyKit::GetSuperPrivacyMode(privpacyMode);
    int32_t oldPrivpacy = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->GetOldSuperPrivacyMode();
    if (privpacy == TELEPHONY_SUCCESS && privpacyMode != oldPrivpacy) {
        int32_t privacy = SuperPrivacyKit::SetSuperPrivacyMode(SuperPrivacyMode::ALWAYS_ON, Source::CALL);
        DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->SetIsChangeSuperPrivacyMode(false);
    }
}

bool CallSuperPrivacyControlManager::GetCurrentIsSuperPrivacyMode()
{
    int32_t privpacyMode;
    int32_t privpacy = SuperPrivacyKit::GetSuperPrivacyMode(privpacyMode);
    if (privpacy == TELEPHONY_SUCCESS && privpacyMode == static_cast<int32_t>(CallSuperPrivacyModeType::ALWAYS_ON)) {
        return true;
    }
    return false;
}
} // namespace Telephony
} // namespace OHOS