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

#include "taihe_call_manager_callback.h"

#include <cinttypes>
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "taihe_call_ability_callback.h"

namespace OHOS {
namespace Telephony {

TaiheCallManagerCallback::TaiheCallManagerCallback()
{
}

TaiheCallManagerCallback::~TaiheCallManagerCallback()
{
}

int32_t TaiheCallManagerCallback::OnCallDetailsChange(const CallAttributeInfo &info)
{
    int32_t ret = TaiheCallAbilityCallback::GetInstance().UpdateCallStateInfo(info);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("UpdateCallStateInfoHandler success! state:%{public}d, videoState:%{public}d, "
            "index:%{public}d, callType:%{public}d", info.callState, info.videoState, info.index, info.callType);
    } else {
        TELEPHONY_LOGE("UpdateCallStateInfoHandler failed! ret:%{public}d", ret);
    }
    return ret;
}

int32_t TaiheCallManagerCallback::OnCallEventChange(const CallEventInfo &info)
{
    int32_t ret = TaiheCallAbilityCallback::GetInstance().UpdateCallEventInfo(info);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("OnCallEventChange success! eventId:%{public}d, phoneNum:%{public}s, "
            "bundleName:%{public}s", info.eventId, info.phoneNum, info.bundleName);
    } else {
        TELEPHONY_LOGE("OnCallEventChange failed! ret:%{public}d", ret);
    }
    return ret;
}

int32_t TaiheCallManagerCallback::OnCallDisconnectedCause(const DisconnectedDetails &details)
{
    int32_t ret = TaiheCallAbilityCallback::GetInstance().UpdateDisconnectedCause(details);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("OnCallDisconnectedCause success! reason:%{public}d, message:%{public}s",
            details.reason, details.message.c_str());
    } else {
        TELEPHONY_LOGE("OnCallDisconnectedCause failed! ret:%{public}d", ret);
    }
    return ret;
}

int32_t TaiheCallManagerCallback::OnReportMmiCodeResult(const MmiCodeInfo &info)
{
    int32_t ret = TaiheCallAbilityCallback::GetInstance().UpdateMmiCodeResult(info);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("OnReportMmiCodeResult success! result:%{public}d, message:%{public}s, "
            "mmiCodeType:%{public}d", info.result, info.message, info.mmiCodeType);
    } else {
        TELEPHONY_LOGE("OnReportMmiCodeResult failed! ret:%{public}d", ret);
    }
    return ret;
}

int32_t TaiheCallManagerCallback::OnReportAudioDeviceChange(const AudioDeviceInfo &info)
{
    int32_t ret = TaiheCallAbilityCallback::GetInstance().UpdateAudioDeviceInfo(info);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("OnReportAudioDeviceChange success! currentDevice:%{public}d, isMuted:%{public}d, "
            "callId:%{public}d, deviceCount:%{public}zu",
            info.currentAudioDevice.deviceType, info.isMuted, info.callId, info.audioDeviceList.size());
    } else {
        TELEPHONY_LOGE("OnReportAudioDeviceChange failed! ret:%{public}d", ret);
    }
    return ret;
}

int32_t TaiheCallManagerCallback::OnReportPostDialDelay(const std::string &str)
{
    int32_t ret = TaiheCallAbilityCallback::GetInstance().UpdatePostDialDelay(str);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("OnReportPostDialDelay success! str:%{public}s", str.c_str());
    } else {
        TELEPHONY_LOGE("OnReportPostDialDelay failed! ret:%{public}d", ret);
    }
    return ret;
}

int32_t TaiheCallManagerCallback::OnUpdateImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
{
    int32_t ret = TaiheCallAbilityCallback::GetInstance().UpdateImsCallModeInfo(imsCallModeInfo);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("OnUpdateImsCallModeChange success! callId:%{public}d, isRequestInfo:%{public}d, "
            "result:%{public}d, callMode:%{public}d",
            imsCallModeInfo.callId, imsCallModeInfo.isRequestInfo,
            imsCallModeInfo.result, imsCallModeInfo.callMode);
    } else {
        TELEPHONY_LOGE("OnUpdateImsCallModeChange failed! ret:%{public}d", ret);
    }
    return ret;
}

int32_t TaiheCallManagerCallback::OnCallSessionEventChange(const CallSessionEvent &callSessionEventOptions)
{
    int32_t ret = TaiheCallAbilityCallback::GetInstance().UpdateCallSessionEvent(callSessionEventOptions);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("OnCallSessionEventChange success! callId:%{public}d, eventId:%{public}d",
            callSessionEventOptions.callId, callSessionEventOptions.eventId);
    } else {
        TELEPHONY_LOGE("OnCallSessionEventChange failed! ret:%{public}d", ret);
    }
    return ret;
}

int32_t TaiheCallManagerCallback::OnPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
{
    int32_t ret = TaiheCallAbilityCallback::GetInstance().UpdatePeerDimensions(peerDimensionsDetail);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("OnPeerDimensionsChange success! callId:%{public}d, width:%{public}d, height:%{public}d",
            peerDimensionsDetail.callId, peerDimensionsDetail.width, peerDimensionsDetail.height);
    } else {
        TELEPHONY_LOGE("OnPeerDimensionsChange failed! ret:%{public}d", ret);
    }
    return ret;
}

int32_t TaiheCallManagerCallback::OnUpdateCameraCapabilities(const CameraCapabilities &cameraCapabilities)
{
    int32_t ret = TaiheCallAbilityCallback::GetInstance().UpdateCameraCapabilities(cameraCapabilities);
    if (ret == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("OnUpdateCameraCapabilities success! callId:%{public}d, width:%{public}d, height:%{public}d",
            cameraCapabilities.callId, cameraCapabilities.width, cameraCapabilities.height);
    } else {
        TELEPHONY_LOGE("OnUpdateCameraCapabilities failed! ret:%{public}d", ret);
    }
    return ret;
}

int32_t TaiheCallManagerCallback::OnMeeTimeDetailsChange(const CallAttributeInfo &info)
{
    TELEPHONY_LOGI("OnMeeTimeDetailsChange");
    // 这里可以根据具体需求处理
    // 目前暂时返回成功，后续可以根据业务需求进行扩展
    return TELEPHONY_SUCCESS;
}

int32_t TaiheCallManagerCallback::OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    TELEPHONY_LOGI("OnReportAsyncResults reportId:%{public}d", reportId);
    // 这里可以根据具体需求处理异步结果
    // 目前暂时返回成功，后续可以根据业务需求进行扩展
    return TELEPHONY_SUCCESS;
}

int32_t TaiheCallManagerCallback::OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    TELEPHONY_LOGI("OnOttCallRequest requestId:%{public}d", requestId);
    // 这里可以根据具体需求处理OTT通话请求
    // 目前暂时返回成功，后续可以根据业务需求进行扩展
    return TELEPHONY_SUCCESS;
}

int32_t TaiheCallManagerCallback::OnCallDataUsageChange(const int64_t dataUsage)
{
    TELEPHONY_LOGI("OnCallDataUsageChange dataUsage:%{public}" PRId64, dataUsage);
    // 这里可以根据具体需求处理数据使用量变化
    // 目前暂时返回成功，后续可以根据业务需求进行扩展
    return TELEPHONY_SUCCESS;
}

int32_t TaiheCallManagerCallback::OnPhoneStateChange(int32_t numActive, int32_t numHeld, int32_t callState,
    const std::string &number)
{
    TELEPHONY_LOGI("OnPhoneStateChange numActive:%{public}d, numHeld:%{public}d, "
        "callState:%{public}d, number:%{public}s", numActive, numHeld, callState, number.c_str());
    // 这里可以根据具体需求处理电话状态变化
    // 目前暂时返回成功，后续可以根据业务需求进行扩展
    return TELEPHONY_SUCCESS;
}

} // namespace Telephony
} // namespace OHOS