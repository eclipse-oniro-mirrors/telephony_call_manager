/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "call_ability_callback_death_recipient.h"

#include "telephony_log_wrapper.h"
#include "i_call_ability_callback.h"
#include "call_ability_report_proxy.h"

namespace OHOS {
namespace Telephony {
CallAbilityCallbackDeathRecipient::CallAbilityCallbackDeathRecipient()
{
    TELEPHONY_LOGI("create CallAbilityCallbackDeathRecipient instance");
}

CallAbilityCallbackDeathRecipient::~CallAbilityCallbackDeathRecipient()
{
    TELEPHONY_LOGI("destory CallAbilityCallbackDeathRecipient instance");
}

void CallAbilityCallbackDeathRecipient::OnRemoteDied(const OHOS::wptr<OHOS::IRemoteObject> &object)
{
    TELEPHONY_LOGI("callback service died, remove the proxy object");
    sptr<ICallAbilityCallback> callback = iface_cast<ICallAbilityCallback>(object.promote());
    if (callback == nullptr) {
        TELEPHONY_LOGE("callback is nullptr");
        return;
    }
    auto report = DelayedSingleton<CallAbilityReportProxy>::GetInstance();
    if (report == nullptr) {
        TELEPHONY_LOGE("report is nullptr");
        return;
    }
    report->UnRegisterCallBack(object.promote());
}
} // namespace Telephony
} // namespace OHOS