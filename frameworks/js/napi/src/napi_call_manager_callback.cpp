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

#include "napi_call_manager_callback.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "napi_call_ability_callback.h"

namespace OHOS {
namespace Telephony {
int32_t NapiCallManagerCallback::OnCallDetailsChange(const CallAttributeInfo &info)
{
    int32_t ret = DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UpdateCallStateInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallStateInfoHandler failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateCallStateInfoHandler success! state:%{public}d", info.callState);
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnCallEventChange(const CallEventInfo &info)
{
    int32_t ret = DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UpdateCallEvent(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallEvent failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateCallEvent success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnCallDisconnectedCause(const DisconnectedDetails &details)
{
    int32_t ret = DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UpdateCallDisconnectedCause(details);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallDisconnectedCause failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateCallDisconnectedCause success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    int32_t ret =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UpdateAsyncResultsInfo(reportId, resultInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateAsyncInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateAsyncInfo success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnReportMmiCodeResult(const MmiCodeInfo &info)
{
    int32_t ret = DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UpdateMmiCodeResultsInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateMmiCodeInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateMmiCodeInfo success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    int32_t ret = DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->OttCallRequest(requestId, info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ott call request failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("ott call request success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnReportAudioDeviceChange(const AudioDeviceInfo &info)
{
    int32_t ret = DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UpdateAudioDeviceInfo(info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateAudioDeviceInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateAudioDeviceInfo success!");
    }
    return ret;
}
} // namespace Telephony
} // namespace OHOS
