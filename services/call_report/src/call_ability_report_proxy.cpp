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

#include "call_ability_report_proxy.h"

#include <string_ex.h>

#include "app_mgr_interface.h"
#include "app_state_observer.h"
#include "bluetooth_call_manager.h"
#include "call_ability_callback_death_recipient.h"
#include "call_manager_errors.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallAbilityReportProxy::CallAbilityReportProxy()
{
    callbackPtrList_.clear();
}

CallAbilityReportProxy::~CallAbilityReportProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    while (it != callbackPtrList_.end()) {
        if ((*it)) {
            (*it).clear();
            (*it) = nullptr;
        }
        callbackPtrList_.erase(it++);
    }
}

int32_t CallAbilityReportProxy::RegisterCallBack(
    sptr<ICallAbilityCallback> callAbilityCallbackPtr, const std::string &bundleInfo)
{
    if (callAbilityCallbackPtr == nullptr) {
        TELEPHONY_LOGE("callAbilityCallbackPtr is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callAbilityCallbackPtr->SetBundleInfo(bundleInfo);
    std::lock_guard<std::mutex> lock(mutex_);
    callbackPtrList_.emplace_back(callAbilityCallbackPtr);
    TELEPHONY_LOGI("%{public}s successfully registered the callback!", bundleInfo.c_str());
    if (callAbilityCallbackPtr->AsObject() != nullptr) {
        sptr<CallAbilityCallbackDeathRecipient> deathRecipient =
            new (std::nothrow) CallAbilityCallbackDeathRecipient();
        if (deathRecipient == nullptr) {
            TELEPHONY_LOGW("deathRecipient is nullptr");
            return false;
        }
        callAbilityCallbackPtr->AsObject()->AddDeathRecipient();
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityReportProxy::UnRegisterCallBack(const std::string &bundleInfo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtrList_.empty()) {
        TELEPHONY_LOGE("callbackPtrList_ is null! %{public}s UnRegisterCallBack failed", bundleInfo.c_str());
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)->GetBundleInfo() == bundleInfo) {
            callbackPtrList_.erase(it);
            TELEPHONY_LOGI("%{public}s UnRegisterCallBack success", bundleInfo.c_str());
            break;
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityReportProxy::UnRegisterCallBack(sptr<IRemoteObject> object)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackPtrList_.empty()) {
        TELEPHONY_LOGE("callbackPtrList_ is null! %{public}s UnRegisterCallBack failed", bundleInfo.c_str());
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)->AsObject() == object) {
            callbackPtrList_.erase(it);
            TELEPHONY_LOGI("%{public}s UnRegisterCallBack success", bundleInfo.c_str());
            break;
        }
    }
    return TELEPHONY_SUCCESS;
}

void CallAbilityReportProxy::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("callObjectPtr is nullptr!");
        return;
    }
    CallAttributeInfo info;
    callObjectPtr->GetCallAttributeInfo(info);
    size_t accountLen = strlen(info.accountNumber);
    if (accountLen > static_cast<size_t>(kMaxNumberLen)) {
        accountLen = kMaxNumberLen;
    }
    for (size_t i = 0; i < accountLen; i++) {
        if (info.accountNumber[i] == ',' || info.accountNumber[i] == ';') {
            info.accountNumber[i] = '\0';
            break;
        }
    }
    if (nextState == TelCallState::CALL_STATUS_ANSWERED) {
        TELEPHONY_LOGI("report answered state");
        info.callState = TelCallState::CALL_STATUS_ANSWERED;
    }
    ReportCallStateInfo(info);
}

void CallAbilityReportProxy::CallEventUpdated(CallEventInfo &info)
{
    ReportCallEvent(info);
}

void CallAbilityReportProxy::CallDestroyed(const DisconnectedDetails &details)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnCallDisconnectedCause(details);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("OnCallDisconnectedCause failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("report call disconnected cause[%{public}d] success", details.reason);
}

int32_t CallAbilityReportProxy::ReportCallStateInfo(const CallAttributeInfo &info)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::string bundleInfo = "";
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            bundleInfo = (*it)->GetBundleInfo();
            ret = (*it)->OnCallDetailsChange(info);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW(
                    "OnCallDetailsChange failed, errcode:%{public}d, bundleInfo:%{public}s", ret, bundleInfo.c_str());
                continue;
            }
        }
    }
    DelayedSingleton<BluetoothCallManager>::GetInstance()->SendCallDetailsChange(static_cast<int32_t>(info.callId),
        static_cast<int32_t>(info.callState));
    TELEPHONY_LOGI("report call state info success, callId[%{public}d] state[%{public}d] conferenceState[%{public}d] "
                   "videoState[%{public}d]",
        info.callId, info.callState, info.conferenceState, info.videoState);
    return ret;
}

int32_t CallAbilityReportProxy::ReportCallStateInfo(const CallAttributeInfo &info, std::string bundleInfo)
{
    int32_t ret = TELEPHONY_ERROR;
    for (auto callback : callbackPtrList_) {
        if (callback->GetBundleInfo() == bundleInfo) {
            ret = callback->OnCallDetailsChange(info);
            break;
        }
    }
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE(
            "OnCallDetailsChange failed, errcode:%{public}d, bundleInfo:%{public}s", ret, bundleInfo.c_str());
    } else {
        TELEPHONY_LOGI("callId[%{public}d] state[%{public}d] conferenceState[%{public}d] "
                       "videoState[%{public}d], report bundleInfo %{public}s success",
            info.callId, info.callState, info.conferenceState, info.videoState, bundleInfo.c_str());
    }
    return ret;
}

int32_t CallAbilityReportProxy::ReportCallEvent(const CallEventInfo &info)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    TELEPHONY_LOGI("report call event, eventId:%{public}d", info.eventId);
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnCallEventChange(info);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("OnCallEventChange failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("report call event[%{public}d] info success", info.eventId);
    return ret;
}

int32_t CallAbilityReportProxy::ReportAsyncResults(
    const CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnReportAsyncResults(reportId, resultInfo);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("ReportAsyncResults failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("ReportAsyncResults success, reportId:%{public}d", reportId);
    return ret;
}

int32_t CallAbilityReportProxy::ReportMmiCodeResult(const MmiCodeInfo &info)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnReportMmiCodeResult(info);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("ReportMmiCodeResult failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("ReportMmiCodeResult success");
    return ret;
}

int32_t CallAbilityReportProxy::OttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        std::string bundleInfo = (*it)->GetBundleInfo();
        if (bundleInfo == "com.ohos.callservice") {
            ret = (*it)->OnOttCallRequest(requestId, info);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW(
                    "OttCallRequest failed, errcode:%{public}d, bundleInfo:%{public}s", ret, bundleInfo.c_str());
                break;
            }
        }
    }
    TELEPHONY_LOGI("OttCallRequest success, requestId:%{public}d", requestId);
    return ret;
}

int32_t CallAbilityReportProxy::ReportAudioDeviceChange(const AudioDeviceInfo &info)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnReportAudioDeviceChange(info);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("ReportAudioDeviceChange failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("ReportAudioDeviceChange success");
    return ret;
}

int32_t CallAbilityReportProxy::ReportPostDialDelay(const std::string &str)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnReportPostDialDelay(str);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("ReportPostDialDelay failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("ReportPostDialDelay success");
    return ret;
}

int32_t CallAbilityReportProxy::ReportImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnReportImsCallModeChange(imsCallModeInfo);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("ReportImsCallModeReceive failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("ReportImsCallModeReceive success");
    return ret;
}

int32_t CallAbilityReportProxy::ReportCallSessionEventChange(
    const CallSessionEvent &callSessionEventOptions)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnReportCallSessionEventChange(callSessionEventOptions);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("ReportCallSessionEventChange failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("ReportCallSessionEventChange success");
    return ret;
}

int32_t CallAbilityReportProxy::ReportPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnReportPeerDimensionsChange(peerDimensionsDetail);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("ReportPeerDimensionsChange failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("ReportPeerDimensionsChange success");
    return ret;
}

int32_t CallAbilityReportProxy::ReportCallDataUsageChange(const int64_t dataUsage)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnReportCallDataUsageChange(dataUsage);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("ReportCallDataUsageChange failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("ReportCallDataUsageChange success");
    return ret;
}

int32_t CallAbilityReportProxy::ReportCameraCapabilities(const CameraCapabilities &cameraCapabilities)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); ++it) {
        if ((*it)) {
            ret = (*it)->OnReportCameraCapabilities(cameraCapabilities);
            if (ret != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGW("ReportCameraCapabilities failed, errcode:%{public}d, bundleInfo:%{public}s", ret,
                    ((*it)->GetBundleInfo()).c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("ReportCameraCapabilities success");
    return ret;
}
} // namespace Telephony
} // namespace OHOS
