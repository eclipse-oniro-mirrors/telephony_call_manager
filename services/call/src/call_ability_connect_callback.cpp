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
#include "call_connect_ability.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
const int32_t CONNECT_ABILITY_SUCCESS = 0;
void CallAbilityConnectCallback::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    TELEPHONY_LOGI("connect callui result code: %{public}d", resultCode);
    if (resultCode == CONNECT_ABILITY_SUCCESS) {
        DelayedSingleton<CallConnectAbility>::GetInstance()->SetConnectFlag(true);
    }
}

void CallAbilityConnectCallback::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    TELEPHONY_LOGI("disconnect callui result code: %{public}d", resultCode);
    DelayedSingleton<CallConnectAbility>::GetInstance()->SetConnectFlag(false);
}
} // namespace Telephony
} // namespace OHOS
