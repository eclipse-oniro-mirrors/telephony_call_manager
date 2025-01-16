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

#include "call_ability_callback.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallAbilityCallback::CallAbilityCallback() : callbackPtr_(nullptr) {}

CallAbilityCallback::~CallAbilityCallback() {}

int32_t CallAbilityCallback::SetProcessCallback(std::unique_ptr<CallManagerCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callback == nullptr) {
        callbackPtr_ = nullptr;
        return TELEPHONY_SUCCESS;
    }
    callbackPtr_ = std::move(callback);
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnCallDetailsChange(const CallAttributeInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnCallDetailsChange(info);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnMeeTimeDetailsChange(const CallAttributeInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnMeeTimeDetailsChange(info);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnCallEventChange(const CallEventInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnCallEventChange(info);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnCallDisconnectedCause(const DisconnectedDetails &details)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnCallDisconnectedCause(details);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnReportAsyncResults(reportId, resultInfo);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnReportMmiCodeResult(const MmiCodeInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnReportMmiCodeResult(info);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnOttCallRequest(requestId, info);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnReportAudioDeviceChange(const AudioDeviceInfo &info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnReportAudioDeviceChange(info);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnReportPostDialDelay(const std::string &str)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnReportPostDialDelay(str);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnReportImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnUpdateImsCallModeChange(imsCallModeInfo);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnReportCallSessionEventChange(const CallSessionEvent &callSessionEventOptions)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnCallSessionEventChange(callSessionEventOptions);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnReportPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnPeerDimensionsChange(peerDimensionsDetail);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnReportCallDataUsageChange(const int64_t dataUsage)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnCallDataUsageChange(dataUsage);
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallback::OnReportCameraCapabilities(const CameraCapabilities &cameraCapabilities)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtr_ != nullptr) {
        return callbackPtr_->OnUpdateCameraCapabilities(cameraCapabilities);
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
