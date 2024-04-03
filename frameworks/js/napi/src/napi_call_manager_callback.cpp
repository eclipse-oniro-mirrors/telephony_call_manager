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
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("UpdateCallStateInfoHandler success! state:%{public}d, videoState:%{public}d", info.callState,
            info.videoState);
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
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("UpdateAudioDeviceInfo success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnReportPostDialDelay(const std::string &str)
{
    int32_t ret = DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UpdatePostDialDelay(str);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("OnReportPostDialDelay failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("OnReportPostDialDelay success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnUpdateImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
{
    TELEPHONY_LOGE("OnUpdateImsCallModeChange, callMode:%{public}d", imsCallModeInfo.callMode);
    int32_t ret =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UpdateImsCallModeChange(imsCallModeInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("OnUpdateImsCallModeReceive failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("OnUpdateImsCallModeReceive success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnCallSessionEventChange(const CallSessionEvent &callSessionEventOptions)
{
    int32_t ret =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->CallSessionEventChange(callSessionEventOptions);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("OnCallSessionEventChange failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("OnCallSessionEventChange success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
{
    int32_t ret =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->PeerDimensionsChange(peerDimensionsDetail);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("OnPeerDimensionsChange failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("OnPeerDimensionsChange success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnCallDataUsageChange(const int64_t dataUsage)
{
    int32_t ret = DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->CallDataUsageChange(dataUsage);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("OnCallDataUsageChange failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("OnCallDataUsageChange success!");
    }
    return ret;
}

int32_t NapiCallManagerCallback::OnUpdateCameraCapabilities(const CameraCapabilities &cameraCapabilities)
{
    int32_t ret =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UpdateCameraCapabilities(cameraCapabilities);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("OnUpdateCameraCapabilities failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("OnUpdateCameraCapabilities success!");
    }
    return ret;
}
} // namespace Telephony
} // namespace OHOS
