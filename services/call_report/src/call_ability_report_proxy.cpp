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

#include "call_ability_report_proxy.h"

#include <string_ex.h>

#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "call_manager_errors.h"
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
    sptr<ICallAbilityCallback> callAbilityCallbackPtr, std::string &bundleName)
{
    std::string tmpName = "";
    if (callAbilityCallbackPtr == nullptr) {
        TELEPHONY_LOGE("callAbilityCallbackPtr is null");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callAbilityCallbackPtr->SetBundleName(bundleName);
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); it++) {
        if ((*it)) {
            (*it)->GetBundleName(tmpName);
            if (tmpName == bundleName) {
                (*it).clear();
                (*it) = callAbilityCallbackPtr;
                TELEPHONY_LOGI("%{public}s RegisterCallBack success", bundleName.c_str());
                return TELEPHONY_SUCCESS;
            }
        }
    }
    callbackPtrList_.emplace_back(callAbilityCallbackPtr);
    TELEPHONY_LOGI("%{public}s successfully registered the callback for the first time!", bundleName.c_str());
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityReportProxy::UnRegisterCallBack(std::string &bundleName)
{
    if (callbackPtrList_.empty()) {
        TELEPHONY_LOGE("callbackPtrList_ is null!");
        return TELEPHONY_ERROR;
    }
    std::string tmpName = "";
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); it++) {
        (*it)->GetBundleName(tmpName);
        if (tmpName == bundleName) {
            callbackPtrList_.erase(it);
            return TELEPHONY_SUCCESS;
        }
    }
    TELEPHONY_LOGE("UnRegisterCallBack failure!");
    return TELEPHONY_ERROR;
}

int32_t CallAbilityReportProxy::ReportCallStateInfo(const CallAttributeInfo &info)
{
    int ret = TELEPHONY_ERR_FAIL;
    std::string bundleName = "";
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); it++) {
        if ((*it)) {
            ret = (*it)->OnCallDetailsChange(info);
            if (ret != TELEPHONY_SUCCESS) {
                (*it)->GetBundleName(bundleName);
                TELEPHONY_LOGW("OnCallDetailsChange failed, errcode:%{public}d, bundleName:%{public}s", ret,
                    bundleName.c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("report call state[%{public}d] conferenceState[%{public}d] info success", info.callState,
        info.conferenceState);
    return ret;
}

int32_t CallAbilityReportProxy::ReportCallEvent(const CallEventInfo &info)
{
    int ret = TELEPHONY_ERR_FAIL;
    std::string bundleName = "";
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); it++) {
        if ((*it)) {
            ret = (*it)->OnCallEventChange(info);
            if (ret != TELEPHONY_SUCCESS) {
                (*it)->GetBundleName(bundleName);
                TELEPHONY_LOGW("OnCallEventChange failed, errcode:%{public}d, bundleName:%{public}s", ret,
                    bundleName.c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("report call event[%{public}d] info success", info.eventId);
    return ret;
}

int32_t CallAbilityReportProxy::ReportSupplementResult(
    const CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    int ret = TELEPHONY_ERR_FAIL;
    std::string bundleName = "";
    std::lock_guard<std::mutex> lock(mutex_);
    std::list<sptr<ICallAbilityCallback>>::iterator it = callbackPtrList_.begin();
    for (; it != callbackPtrList_.end(); it++) {
        if ((*it)) {
            ret = (*it)->OnSupplementResult(reportId, resultInfo);
            if (ret != TELEPHONY_SUCCESS) {
                (*it)->GetBundleName(bundleName);
                TELEPHONY_LOGW("ReportSupplementResult failed, errcode:%{public}d, bundleName:%{public}s", ret,
                    bundleName.c_str());
                continue;
            }
        }
    }
    TELEPHONY_LOGI("ReportSupplementResult success, reportId:%{public}d", reportId);
    return ret;
}
} // namespace Telephony
} // namespace OHOS