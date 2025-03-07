/*
 * Copyright (C) 2024-2025 Huawei Device Co., Ltd.
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

#include "spam_call_connection.h"

#include "telephony_log_wrapper.h"
#include "spam_call_proxy.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t PARAM_NUM = 3;

void SpamCallConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    TELEPHONY_LOGI("OnAbilityConnectDone, resultCode = %{public}d", resultCode);
    if (remoteObject == nullptr) {
        TELEPHONY_LOGE("Connected service is invalid!");
        return;
    }
    remoteObject_ = remoteObject;
    SpamCallProxy service(remoteObject);
    int32_t ret = service.DetectSpamCall(phoneNumber_, slotId_, spamCallAdapter_);
    TELEPHONY_LOGI("SpamCallProxy DetectSpamCall ret = %{public}d", ret);
}

void SpamCallConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element,
    int32_t resultCode)
{
    remoteObject_ = nullptr;
    TELEPHONY_LOGI("OnAbilityDisconnectDone, resultCode = %{public}d", resultCode);
}

void SpamCallConnection::RequireCallReminder()
{
    TELEPHONY_LOGI("DetectNeedNotify start.");
    if (remoteObject_ == nullptr) {
        TELEPHONY_LOGE("remoteObject_ is nullptr");
        return;
    }
    SpamCallProxy service(remoteObject_);
    int32_t ret = service.RequireCallReminder(slotId_, spamCallAdapter_);
    TELEPHONY_LOGI("SpamCallProxy DetectNeedNotify ret = %{public}d", ret);
}
} // namespace Telephony
} // namespace OHOS
