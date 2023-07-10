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

#ifndef NAPI_CALL_ABILITY_CALLBACK_H
#define NAPI_CALL_ABILITY_CALLBACK_H

#include <uv.h>

#include "pac_map.h"
#include "singleton.h"

#include "napi_call_manager_types.h"

namespace OHOS {
namespace Telephony {
/**
 * Data sent asynchronously from the CallManager will be notified to JavaScript via NapiCallAbilityCallback.
 */
class NapiCallAbilityCallback {
    DECLARE_DELAYED_SINGLETON(NapiCallAbilityCallback)
public:
    void RegisterCallStateCallback(EventCallback stateCallback);
    void UnRegisterCallStateCallback();
    void RegisterCallEventCallback(EventCallback eventCallback);
    void UnRegisterCallEventCallback();
    void RegisterDisconnectedCauseCallback(EventCallback eventCallback);
    void UnRegisterDisconnectedCauseCallback();
    void RegisterCallOttRequestCallback(EventCallback ottRequestCallback);
    void UnRegisterCallOttRequestCallback();
    int32_t RegisterGetWaitingCallback(EventCallback callback);
    void UnRegisterGetWaitingCallback();
    int32_t RegisterSetWaitingCallback(EventCallback callback);
    void UnRegisterSetWaitingCallback();
    int32_t RegisterGetRestrictionCallback(EventCallback callback);
    void UnRegisterGetRestrictionCallback();
    int32_t RegisterSetRestrictionCallback(EventCallback callback);
    void UnRegisterSetRestrictionCallback();
    int32_t RegisterSetRestrictionPasswordCallback(EventCallback callback);
    void UnRegisterSetRestrictionPasswordCallback();
    int32_t RegisterGetTransferCallback(EventCallback callback);
    void UnRegisterGetTransferCallback();
    int32_t RegisterSetTransferCallback(EventCallback callback);
    void UnRegisterSetTransferCallback();
    int32_t RegisterStartRttCallback(EventCallback callback);
    void UnRegisterStartRttCallback();
    int32_t RegisterStopRttCallback(EventCallback callback);
    void UnRegisterStopRttCallback();
    void RegisterMmiCodeCallback(EventCallback eventCallback);
    void UnRegisterMmiCodeCallback();
    int32_t RegisterCloseUnFinishedUssdCallback(EventCallback callback);
    void UnRegisterCloseUnFinishedUssdCallback();
    void RegisterAudioDeviceCallback(EventCallback eventCallback);
    void UnRegisterAudioDeviceCallback();
    int32_t UpdateCallStateInfo(const CallAttributeInfo &info);
    int32_t UpdateCallEvent(const CallEventInfo &info);
    int32_t UpdateCallDisconnectedCause(const DisconnectedDetails &details);
    int32_t UpdateAsyncResultsInfo(const CallResultReportId reportId, AppExecFwk::PacMap &resultInfo);
    int32_t OttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info);
    int32_t RegisterUpdateCallMediaModeCallback(EventCallback callback);
    void UnRegisterUpdateCallMediaModeCallback();
    int32_t UpdateMmiCodeResultsInfo(const MmiCodeInfo &info);
    int32_t UpdateAudioDeviceInfo(const AudioDeviceInfo &info);

private:
    static void ReportCallStateWork(uv_work_t *work, int32_t status);
    static int32_t ReportCallState(CallAttributeInfo &info, EventCallback stateCallback);
    static void ReportCallEventWork(uv_work_t *work, int32_t status);
    static int32_t ReportCallEvent(CallEventInfo &info, EventCallback stateCallback);
    static void ReportCallDisconnectedCauseWork(uv_work_t *work, int32_t status);
    static int32_t ReportDisconnectedCause(const DisconnectedDetails &details, EventCallback eventCallback);
    int32_t ReportGetWaitingInfo(AppExecFwk::PacMap &resultInfo);
    int32_t ReportSetWaitingInfo(AppExecFwk::PacMap &resultInfo);
    int32_t ReportGetRestrictionInfo(AppExecFwk::PacMap &resultInfo);
    int32_t ReportSetRestrictionInfo(AppExecFwk::PacMap &resultInfo);
    int32_t ReportSetRestrictionPassword(AppExecFwk::PacMap &resultInfo);
    int32_t ReportGetTransferInfo(AppExecFwk::PacMap &resultInfo);
    int32_t ReportSetTransferInfo(AppExecFwk::PacMap &resultInfo);
    static void ReportWaitAndLimitInfoWork(uv_work_t *work, int32_t status);
    static void ReportWaitAndLimitInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo);
    static void ReportSupplementInfoWork(uv_work_t *work, int32_t status);
    static void ReportSupplementInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo);
    static void ReportExecutionResultWork(uv_work_t *work, int32_t status);
    static void ReportExecutionResult(EventCallback &settingInfo, AppExecFwk::PacMap &resultInfo);
    static void ReportStartRttInfoWork(uv_work_t *work, int32_t status);
    static void ReportStartRttInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo);
    static void ReportStopRttInfoWork(uv_work_t *work, int32_t status);
    static void ReportStopRttInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo);
    int32_t ReportStartRttInfo(AppExecFwk::PacMap &resultInfo);
    int32_t ReportStopRttInfo(AppExecFwk::PacMap &resultInfo);
    static void ReportCallOttWork(uv_work_t *work, int32_t status);
    static int32_t ReportCallOtt(
        EventCallback &settingInfo, AppExecFwk::PacMap &resultInfo, OttCallRequestId requestId);
    int32_t ReportCallMediaModeInfo(AppExecFwk::PacMap &resultInfo);
    static void ReportCallMediaModeInfoWork(uv_work_t *work, int32_t status);
    static void ReportCallMediaModeInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo);
    static void ReportMmiCodeWork(uv_work_t *work, int32_t status);
    static int32_t ReportMmiCode(MmiCodeInfo &info, EventCallback eventCallback);
    int32_t ReportCloseUnFinishedUssdInfo(AppExecFwk::PacMap &resultInfo);
    static void ReportAudioDeviceInfoWork(uv_work_t *work, int32_t status);
    static int32_t ReportAudioDeviceInfo(AudioDeviceInfo &info, EventCallback eventCallback);

private:
    EventCallback stateCallback_;
    EventCallback eventCallback_;
    EventCallback callDisconnectCauseCallback_;
    EventCallback ottRequestCallback_;
    EventCallback getWaitingCallback_;
    EventCallback setWaitingCallback_;
    EventCallback getRestrictionCallback_;
    EventCallback setRestrictionCallback_;
    EventCallback setRestrictionPasswordCallback_;
    EventCallback getTransferCallback_;
    EventCallback setTransferCallback_;
    EventCallback startRttCallback_;
    EventCallback stopRttCallback_;
    EventCallback updateCallMediaModeCallback_;
    EventCallback mmiCodeCallback_;
    EventCallback closeUnfinishedUssdCallback_;
    EventCallback audioDeviceCallback_;
    using CallResultReportIdProcessorFunc = int32_t (NapiCallAbilityCallback::*)(AppExecFwk::PacMap &resultInfo);
    std::map<CallResultReportId, CallResultReportIdProcessorFunc> memberFuncMap_;
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS

#endif // NAPI_CALL_ABILITY_CALLBACK_H
