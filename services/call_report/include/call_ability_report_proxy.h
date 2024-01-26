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

#ifndef CALL_ABILITY_REPORT_PROXY_H
#define CALL_ABILITY_REPORT_PROXY_H

#include <list>

#include "singleton.h"

#include "i_call_ability_callback.h"

#include "call_state_listener_base.h"
#include "app_state_observer.h"

namespace OHOS {
namespace Telephony {
class CallAbilityReportProxy : public CallStateListenerBase,
                               public std::enable_shared_from_this<CallAbilityReportProxy> {
    DECLARE_DELAYED_SINGLETON(CallAbilityReportProxy)
public:
    int32_t RegisterCallBack(sptr<ICallAbilityCallback> callback, const std::string &bundleName);
    int32_t UnRegisterCallBack(const std::string &bundleName);
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    void CallEventUpdated(CallEventInfo &info) override;
    void CallDestroyed(const DisconnectedDetails &details) override;
    int32_t ReportAsyncResults(const CallResultReportId reportId, AppExecFwk::PacMap &resultInfo);
    int32_t OttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info);
    int32_t ReportMmiCodeResult(const MmiCodeInfo &info);
    int32_t ReportAudioDeviceChange(const AudioDeviceInfo &info);
    int32_t ReportCallStateInfo(const CallAttributeInfo &info);
    int32_t ReportCallStateInfo(const CallAttributeInfo &info, std::string bundleName);
    int32_t ReportPostDialDelay(const std::string &str);
    int32_t ReportImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo);
    int32_t ReportCallSessionEventChange(const CallSessionEvent &callSessionEventOptions);
    int32_t ReportPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail);
    int32_t ReportCallDataUsageChange(const int64_t dataUsage);
    int32_t ReportCameraCapabilities(const CameraCapabilities &cameraCapabilities);

private:
    int32_t ReportCallEvent(const CallEventInfo &info);

private:
    std::list<sptr<ICallAbilityCallback>> callbackPtrList_;
    sptr<ICallAbilityCallback> callAbilityCallbackPtr_;
    sptr<ApplicationStateObserver> appStateObserver;
    sptr<AppExecFwk::IAppMgr> appMgrProxy = nullptr;
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_ABILITY_REPORT_IPC_PROXY_H
