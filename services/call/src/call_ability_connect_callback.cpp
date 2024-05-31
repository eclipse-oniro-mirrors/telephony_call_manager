/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "call_ability_connect_callback.h"
#include "call_ability_report_proxy.h"
#include "call_connect_ability.h"
#include "call_object_manager.h"
#include "telephony_log_wrapper.h"
#include "call_superprivacy_control_manager.h"

namespace OHOS {
namespace Telephony {
const int32_t CONNECT_ABILITY_SUCCESS = 0;
const int32_t UNEXPECT_DISCONNECT_CODE = -1;

void CallAbilityConnectCallback::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    TELEPHONY_LOGI("connect callui result code: %{public}d", resultCode);
    DelayedSingleton<CallConnectAbility>::GetInstance()->NotifyAll();
    DelayedSingleton<CallConnectAbility>::GetInstance()->SetConnectFlag(true);
    if (resultCode == CONNECT_ABILITY_SUCCESS) {
        CallEventInfo eventInfo;
        (void)memset_s(&eventInfo, sizeof(CallEventInfo), 0, sizeof(CallEventInfo));
        bool isSuperPrivacyMode = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->GetCurrentIsSuperPrivacyMode();
        TELEPHONY_LOGI("OnAbilityConnectDone SuperPrivacyMode:%{public}d", isSuperPrivacyMode);
        if(isSuperPrivacyMode){
            eventInfo.eventId = CallAbilityEventId::EVENT_IS_SUPER_PRIVACY_MODE_ON;
        } else {
            eventInfo.eventId = CallAbilityEventId::EVENT_IS_SUPER_PRIVACY_MODE_OFF;
        }
        DelayedSingleton<CallAbilityReportProxy>::GetInstance()->CallEventUpdated(eventInfo);
    }
}

void CallAbilityConnectCallback::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    TELEPHONY_LOGI("disconnect callui result code: %{public}d", resultCode);
    DelayedSingleton<CallConnectAbility>::GetInstance()->SetConnectFlag(false);
    if (resultCode == UNEXPECT_DISCONNECT_CODE) {
        ReConnectAbility();
    }
}

void CallAbilityConnectCallback::ReConnectAbility()
{
    if (!CallObjectManager::HasCallExist()) {
        TELEPHONY_LOGE("callObjectPtrList_ is empty, no need to report");
        return;
    }
    bool connectFlag = false;
    std::vector<CallAttributeInfo> callAttributeInfo = CallObjectManager::GetAllCallInfoList();
    std::vector<CallAttributeInfo>::iterator it = callAttributeInfo.begin();
    while (it != callAttributeInfo.end()) {
        CallAttributeInfo info = (*it);
        TelCallState callState = info.callState;
        it++;
        if (callState == TelCallState::CALL_STATUS_DISCONNECTING ||
            callState == TelCallState::CALL_STATUS_DISCONNECTED ||
            callState == TelCallState::CALL_STATUS_UNKNOWN || callState == TelCallState::CALL_STATUS_IDLE) {
            TELEPHONY_LOGE("no need to report");
            continue;
        }
        if (!connectFlag) {
            DelayedSingleton<CallConnectAbility>::GetInstance()->ConnectAbility();
            connectFlag = true;
            continue;
        }
        if (!DelayedSingleton<CallConnectAbility>::GetInstance()->WaitForConnectResult()) {
            return;
        }
        DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportCallStateInfo(info);
    }
}
} // namespace Telephony
} // namespace OHOS
