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
#ifdef SUPPORT_SUPER_PRIVACY_SERVICE
#include "super_privacy_kit.h"
#endif
#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
#ifdef SUPPORT_SUPER_PRIVACY_SERVICE
using namespace AppSecurityPrivacy::SecurityPrivacyServer::SuperPrivacy;
#endif

void CallSuperPrivacyControlManager::RegisterSuperPrivacyMode()
{
#ifdef SUPPORT_SUPER_PRIVACY_SERVICE
    modeChangeListener = std::make_shared<CallSuperPrivacyListener>();
    int32_t ret = SuperPrivacyKit::RegisterSuperPrivacyModeListener(modeChangeListener);
    TELEPHONY_LOGE("RegisterSuperPrivacyMode ret:%{public}d", ret);
#endif
}

void CallSuperPrivacyControlManager::UnRegisterSuperPrivacyMode()
{
#ifdef SUPPORT_SUPER_PRIVACY_SERVICE
    if (modeChangeListener != nullptr) {
        int32_t ret = SuperPrivacyKit::UnRegisterSuperPrivacyModeListener(modeChangeListener);
        TELEPHONY_LOGE("UnRegisterSuperPrivacyMode ret:%{public}d", ret);
    }
#endif
}

void CallSuperPrivacyListener::OnSuperPrivacyModeChanged(const int32_t &superPrivacyMode)
{
    TELEPHONY_LOGE("OnSuperPrivacyModeChanged superPrivacyMode:%{public}d", superPrivacyMode);
    if (superPrivacyMode == static_cast<int32_t>(CallSuperPrivacyModeType::ALWAYS_ON)) {
        SetIsChangeSuperPrivacyMode(false);
        CloseAllCall();
    } else if (superPrivacyMode == static_cast<int32_t>(CallSuperPrivacyModeType::OFF)) {
        if (!GetIsChangeSuperPrivacyMode()) {
            SetIsChangeSuperPrivacyMode(false);
        }
    }
}

void CloseAllCall()
{
    std::vector<CallAttributeInfo> infos = CallObjectManager::GetAllCallInfoList();
    for (auto &info : infos) {
        if (!info.isEcc) {
            TELEPHONY_LOGE("OnSuperPrivacyModeChanged callState:%{public}d", info.callState);
            if (info.callState == TelCallState::CALL_STATE_INCOMING ||
                info.callState == TelCallState::CALL_STATE_WAITING) {
                DelayedSingleton<CallControlManager>::GetInstance()->RejectCall(info.callId, false,
                u"superPrivacyMode");
            } else {
                DelayedSingleton<CallControlManager>::GetInstance()->HangUpCall(info.callId);
            }
        }
    }
}

bool CallSuperPrivacyControlManager::GetIsChangeSuperPrivacyMode()
{
    return isChangeSuperPrivacyMode;
}

void CallSuperPrivacyControlManager::SetIsChangeSuperPrivacyMode(bool isChangeSuperPrivacy)
{
    isChangeSuperPrivacyMode = isChangeSuperPrivacy;
}

void CallSuperPrivacyControlManager::SetOldSuperPrivacyMode()
{
#ifdef SUPPORT_SUPER_PRIVACY_SERVICE
    int32_t privpacyMode;
    int32_t privpacy = SuperPrivacyKit::GetSuperPrivacyMode(privpacyMode);
    oldSuperPrivacyMode = privpacyMode;
#endif
}

int32_t CallSuperPrivacyControlManager::GetOldSuperPrivacyMode()
{
    return oldSuperPrivacyMode;
}

int32_t CallSuperPrivacyControlManager::CloseSuperPrivacyMode()
#ifdef SUPPORT_SUPER_PRIVACY_SERVICE
    int32_t privacy = SuperPrivacyKit::SetSuperPrivacyMode(SuperPrivacyMode::OFF, Source::CALL);
    TELEPHONY_LOGE("CloseSuperPrivacyMode privacy:%{public}d", privacy);
    return privacy;
#endif
}

void CallSuperPrivacyControlManager::CloseCallSuperPrivacyMode(std::u16string &phoneNumber, int32_t &accountId,
    int32_t &videoState, int32_t &dialType, int32_t &dialScene, int32_t &callType)
{
#ifdef SUPPORT_SUPER_PRIVACY_SERVICE
    int32_t privacy = CloseSuperPrivacyMode();
    TELEPHONY_LOGE("CloseCallSuperPrivacyMode privacy:%{public}d", privacy);
    if (privacy == SUPER_PRIVACY_MODE_REQUEST_SUCCESS) {
        AppExecFwk::PacMap dialInfo;
        dialInfo.PutIntValue("accountId", accountId);
        dialInfo.PutIntValue("videoState", videoState);
        dialInfo.PutIntValue("dialType", dialType);
        dialInfo.PutIntValue("dialScene", dialScene);
        dialInfo.PutIntValue("callType", callType);
        int32_t ret = DelayedSingleton<CallControlManager>::GetInstance()->DialCall(phoneNumber, dialInfo);
        if (ret != TELEPHONY_SUCCESS) {
            RestoreSuperPrivacyMode();
        }
    }
#endif
}

void CallSuperPrivacyControlManager::CloseAnswerSuperPrivacyMode(int32_t callId, int32_t videoState)
{
#ifdef SUPPORT_SUPER_PRIVACY_SERVICE
    int32_t privacy = CloseSuperPrivacyMode();
    TELEPHONY_LOGE("CloseAnswerSuperPrivacyMode privacy:%{public}d", privacy);
    if (privacy == SUPER_PRIVACY_MODE_REQUEST_SUCCESS) {
        DelayedSingleton<CallControlManager>::GetInstance()->AnswerCall(callId, videoState);
    }
#endif
}

void CallSuperPrivacyControlManager::RestoreSuperPrivacyMode()
{
    if (!GetIsChangeSuperPrivacyMode()) {
        return;
    }
    int32_t privpacyMode;
#ifdef SUPPORT_SUPER_PRIVACY_SERVICE
    int32_t privpacy = SuperPrivacyKit::GetSuperPrivacyMode(privpacyMode);
    int32_t oldPrivpacy = GetOldSuperPrivacyMode();
    TELEPHONY_LOGE("RestoreSuperPrivacyMode oldPrivpacy:%{public}d", oldPrivpacy);
    if (privpacy == SUPER_PRIVACY_MODE_REQUEST_SUCCESS && privpacyMode != oldPrivpacy) {
        SetIsChangeSuperPrivacyMode(false);
        if (oldPrivpacy == static_cast<int32_t>(CallSuperPrivacyModeType::ALWAYS_ON)) {
            int32_t privacy = SuperPrivacyKit::SetSuperPrivacyMode(SuperPrivacyMode::ALWAYS_ON, Source::CALL);
            TELEPHONY_LOGE("RestoreSuperPrivacyMode ret privacy:%{public}d", privacy);
        }
    }
#endif
}

bool CallSuperPrivacyControlManager::GetCurrentIsSuperPrivacyMode()
{
    int32_t privpacyMode;
#ifdef SUPPORT_SUPER_PRIVACY_SERVICE
    int32_t privpacy = SuperPrivacyKit::GetSuperPrivacyMode(privpacyMode);
    TELEPHONY_LOGE("GetCurrentIsSuperPrivacyMode privpacyMode:%{public}d", privpacyMode);
    if (privpacy == SUPER_PRIVACY_MODE_REQUEST_SUCCESS &&
        privpacyMode ==static_cast<int32_t>(CallSuperPrivacyModeType::ALWAYS_ON)) {
        return true;
    }
#endif
    return false;
}
} // namespace Telephony
} // namespace OHOS