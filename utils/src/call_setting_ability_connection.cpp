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

#include "call_setting_ability_connection.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t PARAM_NUM = 3;

void CallSettingAbilityConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    TELEPHONY_LOGI("OnAbilityConnectDone, resultCode = %{public}d", resultCode);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(PARAM_NUM);
    data.WriteString16(u"bundleName");
    data.WriteString16(u"com.callsetting");
    data.WriteString16(u"abilityName");
    data.WriteString16(u"com.callsetting.SatelliteCallDialogAbility");
    data.WriteString16(u"parameters");
    data.WriteString16(Str8ToStr16(commandStr_));
    int32_t sendRequestRet = remoteObject->SendRequest(AAFwk::IAbilityConnection::ON_ABILITY_CONNECT_DONE,
        data, reply, option);
    TELEPHONY_LOGI("OnAbilityConnectDone, sendRequestRet = %{public}d", sendRequestRet);
}

void CallSettingAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element,
    int32_t resultCode)
{
    TELEPHONY_LOGI("OnAbilityDisconnectDone, resultCode = %{public}d", resultCode);
}
} // namespace Telephony
} // namespace OHOS