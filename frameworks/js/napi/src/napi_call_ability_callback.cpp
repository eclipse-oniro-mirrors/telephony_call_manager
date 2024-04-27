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

#include "napi_call_ability_callback.h"

#include <securec.h>
#include <ctime>

#include "call_manager_errors.h"
#include "napi_call_manager_utils.h"
#include "napi_util.h"
#include "pixel_map.h"
#include "pixel_map_napi.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
NapiCallAbilityCallback::NapiCallAbilityCallback()
{
    (void)memset_s(&stateCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&eventCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&ottRequestCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&getWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&setWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&getRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&setRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&setRestrictionPasswordCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&getTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&setTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&startRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&stopRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&callDisconnectCauseCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&mmiCodeCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&postDialDelayCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&imsCallModeCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&peerDimensionsCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&callDataUsageCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&cameraCapabilitiesCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&callSessionEventCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    memberFuncMap_[CallResultReportId::GET_CALL_WAITING_REPORT_ID] = &NapiCallAbilityCallback::ReportGetWaitingInfo;
    memberFuncMap_[CallResultReportId::SET_CALL_WAITING_REPORT_ID] = &NapiCallAbilityCallback::ReportSetWaitingInfo;
    memberFuncMap_[CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID] =
        &NapiCallAbilityCallback::ReportGetRestrictionInfo;
    memberFuncMap_[CallResultReportId::SET_CALL_RESTRICTION_REPORT_ID] =
        &NapiCallAbilityCallback::ReportSetRestrictionInfo;
    memberFuncMap_[CallResultReportId::SET_CALL_RESTRICTION_PWD_REPORT_ID] =
        &NapiCallAbilityCallback::ReportSetRestrictionPassword;
    memberFuncMap_[CallResultReportId::GET_CALL_TRANSFER_REPORT_ID] =
        &NapiCallAbilityCallback::ReportGetTransferInfo;
    memberFuncMap_[CallResultReportId::SET_CALL_TRANSFER_REPORT_ID] =
        &NapiCallAbilityCallback::ReportSetTransferInfo;
    memberFuncMap_[CallResultReportId::START_RTT_REPORT_ID] = &NapiCallAbilityCallback::ReportStartRttInfo;
    memberFuncMap_[CallResultReportId::STOP_RTT_REPORT_ID] = &NapiCallAbilityCallback::ReportStopRttInfo;
    memberFuncMap_[CallResultReportId::CLOSE_UNFINISHED_USSD_REPORT_ID] =
        &NapiCallAbilityCallback::ReportCloseUnFinishedUssdInfo;
}

NapiCallAbilityCallback::~NapiCallAbilityCallback() {}

void NapiCallAbilityCallback::RegisterCallStateCallback(EventCallback stateCallback)
{
    stateCallback_ = stateCallback;
}

void NapiCallAbilityCallback::UnRegisterCallStateCallback()
{
    if (stateCallback_.callbackRef) {
        (void)memset_s(&stateCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterMmiCodeCallback(EventCallback eventCallback)
{
    mmiCodeCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterMmiCodeCallback()
{
    if (mmiCodeCallback_.callbackRef) {
        (void)memset_s(&mmiCodeCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterAudioDeviceCallback(EventCallback eventCallback)
{
    audioDeviceCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterAudioDeviceCallback()
{
    if (audioDeviceCallback_.callbackRef) {
        (void)memset_s(&audioDeviceCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterPostDialDelay(EventCallback eventCallback)
{
    postDialDelayCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterPostDialDelayCallback()
{
    if (postDialDelayCallback_.callbackRef) {
        (void)memset_s(&postDialDelayCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterCallEventCallback(EventCallback eventCallback)
{
    eventCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterCallEventCallback()
{
    if (eventCallback_.callbackRef) {
        (void)memset_s(&eventCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterImsCallModeChangeCallback(EventCallback eventCallback)
{
    imsCallModeCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterImsCallModeChangeCallback()
{
    if (eventCallback_.callbackRef) {
        (void)memset_s(&imsCallModeCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterPeerDimensionsChangeCallback(EventCallback eventCallback)
{
    peerDimensionsCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterPeerDimensionsChangeCallback()
{
    if (eventCallback_.callbackRef) {
        (void)memset_s(&peerDimensionsCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterCallDataUsageChangeCallback(EventCallback eventCallback)
{
    callDataUsageCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterCallDataUsageChangeCallback()
{
    if (eventCallback_.callbackRef) {
        (void)memset_s(&callDataUsageCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterCameraCapabilitiesChangeCallback(EventCallback eventCallback)
{
    cameraCapabilitiesCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterCameraCapabilitiesChangeCallback()
{
    if (eventCallback_.callbackRef) {
        (void)memset_s(&cameraCapabilitiesCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterCallSessionEventChangeCallback(EventCallback eventCallback)
{
    callSessionEventCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterCallSessionEventChangeCallback()
{
    if (eventCallback_.callbackRef) {
        (void)memset_s(&callSessionEventCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterDisconnectedCauseCallback(EventCallback eventCallback)
{
    callDisconnectCauseCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterDisconnectedCauseCallback()
{
    if (callDisconnectCauseCallback_.callbackRef) {
        (void)memset_s(&callDisconnectCauseCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterCallOttRequestCallback(EventCallback ottRequestCallback)
{
    ottRequestCallback_ = ottRequestCallback;
}

void NapiCallAbilityCallback::UnRegisterCallOttRequestCallback()
{
    if (ottRequestCallback_.callbackRef) {
        (void)memset_s(&ottRequestCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

int32_t NapiCallAbilityCallback::RegisterGetWaitingCallback(EventCallback callback)
{
    if (getWaitingCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    getWaitingCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterGetWaitingCallback()
{
    (void)memset_s(&getWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterCloseUnFinishedUssdCallback(EventCallback callback)
{
    if (closeUnfinishedUssdCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    closeUnfinishedUssdCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterCloseUnFinishedUssdCallback()
{
    (void)memset_s(&closeUnfinishedUssdCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterSetWaitingCallback(EventCallback callback)
{
    if (setWaitingCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    setWaitingCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterSetWaitingCallback()
{
    (void)memset_s(&setWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterGetRestrictionCallback(EventCallback callback)
{
    if (getRestrictionCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    getRestrictionCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterGetRestrictionCallback()
{
    (void)memset_s(&getRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterSetRestrictionCallback(EventCallback callback)
{
    if (setRestrictionCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    setRestrictionCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterSetRestrictionCallback()
{
    (void)memset_s(&setRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterSetRestrictionPasswordCallback(EventCallback callback)
{
    if (setRestrictionPasswordCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    setRestrictionPasswordCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterSetRestrictionPasswordCallback()
{
    (void)memset_s(&setRestrictionPasswordCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterGetTransferCallback(EventCallback callback, int32_t type)
{
    getTransferCallback_ = callback;
    getCallTransferReason_ = type;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterGetTransferCallback()
{
    (void)memset_s(&getTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterSetTransferCallback(EventCallback callback)
{
    if (setTransferCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    setTransferCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterSetTransferCallback()
{
    (void)memset_s(&setTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterStartRttCallback(EventCallback callback)
{
    if (startRttCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    startRttCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterStartRttCallback()
{
    (void)memset_s(&startRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterStopRttCallback(EventCallback callback)
{
    if (stopRttCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    stopRttCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterStopRttCallback()
{
    (void)memset_s(&stopRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::ReportStartRttInfo(AppExecFwk::PacMap &resultInfo)
{
    if (startRttCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("startRttCallback_ is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(startRttCallback_.env, &loop);
#endif
    CallSupplementWorker *callSupplementWorker = std::make_unique<CallSupplementWorker>().release();
    if (callSupplementWorker == nullptr) {
        TELEPHONY_LOGE("callSupplementWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSupplementWorker->info = resultInfo;
    callSupplementWorker->callback = startRttCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSupplementWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportStartRttInfoWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete callSupplementWorker;
        callSupplementWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    if (startRttCallback_.thisVar) {
        (void)memset_s(&startRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportStopRttInfo(AppExecFwk::PacMap &resultInfo)
{
    if (stopRttCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("startRttCallback_ is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(stopRttCallback_.env, &loop);
#endif
    CallSupplementWorker *callSupplementWorker = std::make_unique<CallSupplementWorker>().release();
    if (callSupplementWorker == nullptr) {
        TELEPHONY_LOGE("callSupplementWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSupplementWorker->info = resultInfo;
    callSupplementWorker->callback = stopRttCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSupplementWorker;
    int32_t resultCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportStopRttInfoWork, uv_qos_default);
    if (resultCode != 0) {
        delete callSupplementWorker;
        callSupplementWorker = nullptr;
        TELEPHONY_LOGE("failed to add uv_queue_work_with_qos, resultCode: %{public}d", resultCode);
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    if (stopRttCallback_.thisVar) {
        (void)memset_s(&stopRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateCallStateInfo(const CallAttributeInfo &info)
{
    if (stateCallback_.thisVar == nullptr) {
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(stateCallback_.env, &loop);
#endif
    CallStateWorker *callStateWorker = std::make_unique<CallStateWorker>().release();
    if (callStateWorker == nullptr) {
        TELEPHONY_LOGE("callStateWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callStateWorker->info = info;
    callStateWorker->callback = stateCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callStateWorker;
    int32_t errCode = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {}, ReportCallStateWork, uv_qos_default);
    if (errCode != 0) {
        delete callStateWorker;
        callStateWorker = nullptr;
        delete work;
        work = nullptr;
        TELEPHONY_LOGE("failed to add uv_queue_work_with_qos, errCode: %{public}d", errCode);
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallStateWork(uv_work_t *work, int32_t status)
{
    CallStateWorker *dataWorkerData = (CallStateWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportCallState(dataWorkerData->info, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportCallState result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

/**
 * To notify an application of a call status change, register a callback with on() first.
 */
int32_t NapiCallAbilityCallback::ReportCallState(CallAttributeInfo &info, EventCallback stateCallback)
{
    napi_env env = stateCallback.env;
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        TELEPHONY_LOGE("scope is nullptr");
        napi_close_handle_scope(env, scope);
        return TELEPHONY_ERROR;
    }
    napi_value callbackFunc = nullptr;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
    napi_create_object(env, &callbackValues[ARRAY_INDEX_FIRST]);
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, callbackValues[ARRAY_INDEX_FIRST], "accountNumber", info.accountNumber);
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValues[ARRAY_INDEX_FIRST], "accountId", info.accountId);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "videoState", static_cast<int32_t>(info.videoState));
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValues[ARRAY_INDEX_FIRST], "startTime", info.startTime);
    NapiCallManagerUtils::SetPropertyBoolean(env, callbackValues[ARRAY_INDEX_FIRST], "isEcc", info.isEcc);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "callType", static_cast<int32_t>(info.callType));
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValues[ARRAY_INDEX_FIRST], "callId", info.callId);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "callState", static_cast<int32_t>(info.callState));
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "conferenceState", static_cast<int32_t>(info.conferenceState));
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "crsType", info.crsType);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "originalCallType", info.originalCallType);
    ReportCallAttribute(env, callbackValues, info);
    napi_get_reference_value(env, stateCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, scope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, stateCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    napi_close_handle_scope(env, scope);
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallAttribute(napi_env &env, napi_value callbackValues[], CallAttributeInfo &info)
{
    std::string str(info.numberLocation);
    if (str == "default") {
        TELEPHONY_LOGE("numberLocation is default");
        (void)memset_s(info.numberLocation, kMaxNumberLen, 0, kMaxNumberLen);
    }
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, callbackValues[ARRAY_INDEX_FIRST], "numberLocation", info.numberLocation);
    TELEPHONY_LOGI("ReportCallState crsType = %{public}d", info.crsType);
    if (info.callType == CallType::TYPE_VOIP) {
        napi_value voipObject = nullptr;
        CreateVoipNapiValue(env, voipObject, info);
        napi_set_named_property(env, callbackValues[ARRAY_INDEX_FIRST], "voipCallAttribute", voipObject);
    }
    napi_value markInfoObject = nullptr;
    CreateMarkInfoNapiValue(env, markInfoObject, info);
    napi_set_named_property(env, callbackValues[ARRAY_INDEX_FIRST], "numberMarkInfo", markInfoObject);
}

void NapiCallAbilityCallback::CreateVoipNapiValue(napi_env &env, napi_value &voipObject, CallAttributeInfo &info)
{
    napi_create_object(env, &voipObject);
    NapiCallManagerUtils::SetPropertyStringUtf8(env, voipObject, "userName", info.voipCallInfo.userName);
    NapiCallManagerUtils::SetPropertyStringUtf8(env, voipObject, "abilityName", info.voipCallInfo.abilityName);
    NapiCallManagerUtils::SetPropertyStringUtf8(env, voipObject, "extensionId", info.voipCallInfo.extensionId);
    NapiCallManagerUtils::SetPropertyStringUtf8(env, voipObject, "voipBundleName", info.voipCallInfo.voipBundleName);
    NapiCallManagerUtils::SetPropertyStringUtf8(env, voipObject, "voipCallId", info.voipCallInfo.voipCallId);
    NapiCallManagerUtils::SetPropertyBoolean(env, voipObject, "showBannerForIncomingCall",
        info.voipCallInfo.showBannerForIncomingCall);
    std::shared_ptr<Media::PixelMap> userProfile =
        std::shared_ptr<Media::PixelMap>(Media::PixelMap::DecodeTlv(info.voipCallInfo.userProfile));
    napi_value pixelMapObject = Media::PixelMapNapi::CreatePixelMap(env, userProfile);
    napi_set_named_property(env, voipObject, "userProfile", pixelMapObject);
}

void NapiCallAbilityCallback::CreateMarkInfoNapiValue(napi_env &env, napi_value &markInfoObject, CallAttributeInfo &info)
{
    napi_create_object(env, &markInfoObject);
    NapiCallManagerUtils::SetPropertyInt32(env, markInfoObject, "markType", info.numberMarkInfo.markType);
    NapiCallManagerUtils::SetPropertyStringUtf8(env, markInfoObject, "markContent", info.numberMarkInfo.markContent);
    NapiCallManagerUtils::SetPropertyInt32(env, markInfoObject, "markCount", info.numberMarkInfo.markCount);
    NapiCallManagerUtils::SetPropertyStringUtf8(env, markInfoObject, "markSource", info.numberMarkInfo.markSource);
    NapiCallManagerUtils::SetPropertyBoolean(env, markInfoObject, "isCloud", info.numberMarkInfo.isCloud);
}

int32_t NapiCallAbilityCallback::UpdateCallEvent(const CallEventInfo &info)
{
    if (eventCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("eventCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(eventCallback_.env, &loop);
#endif
    CallEventWorker *callEventWorker = std::make_unique<CallEventWorker>().release();
    if (callEventWorker == nullptr) {
        TELEPHONY_LOGE("callEventWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callEventWorker->info = info;
    callEventWorker->callback = eventCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callEventWorker;
    int32_t errCode = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {}, ReportCallEventWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete callEventWorker;
        callEventWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallEventWork(uv_work_t *work, int32_t status)
{
    CallEventWorker *dataWorkerData = (CallEventWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportCallEvent(dataWorkerData->info, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportCallEvent results %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportCallEvent(CallEventInfo &info, EventCallback eventCallback)
{
    napi_env env = eventCallback.env;
    napi_handle_scope scopeCallEvent = nullptr;
    napi_open_handle_scope(env, &scopeCallEvent);
    if (scopeCallEvent == nullptr) {
        TELEPHONY_LOGE("scopeCallEvent is nullptr");
        napi_close_handle_scope(env, scopeCallEvent);
        return TELEPHONY_ERROR;
    }
    napi_value callEventCallbackFunc = nullptr;
    napi_value callEventCallbackValues[ARRAY_INDEX_THIRD] = { 0 };
    napi_create_object(env, &callEventCallbackValues[ARRAY_INDEX_FIRST]);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callEventCallbackValues[ARRAY_INDEX_FIRST], "eventId", static_cast<int32_t>(info.eventId));
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, callEventCallbackValues[ARRAY_INDEX_FIRST], "accountNumber", info.phoneNum);
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, callEventCallbackValues[ARRAY_INDEX_FIRST], "bundleName", info.bundleName);
    napi_get_reference_value(env, eventCallback.callbackRef, &callEventCallbackFunc);
    if (callEventCallbackFunc == nullptr) {
        TELEPHONY_LOGE("callEventCallbackFunc is null!");
        napi_close_handle_scope(env, scopeCallEvent);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callEventCallbackFunc, DATA_LENGTH_ONE, callEventCallbackValues, &callbackResult);
    napi_close_handle_scope(env, scopeCallEvent);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateCallDisconnectedCause(const DisconnectedDetails &details)
{
    if (callDisconnectCauseCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("callDisconnectCauseCallback_ is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(callDisconnectCauseCallback_.env, &loop);
#endif
    CallDisconnectedCauseWorker *callDisconnectedCauseWorker =
        std::make_unique<CallDisconnectedCauseWorker>().release();
    if (callDisconnectedCauseWorker == nullptr) {
        TELEPHONY_LOGE("callDisconnectedCauseWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callDisconnectedCauseWorker->details = details;
    callDisconnectedCauseWorker->callback = callDisconnectCauseCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callDisconnectedCauseWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportCallDisconnectedCauseWork, uv_qos_default);
    if (errCode != 0) {
        delete callDisconnectedCauseWorker;
        callDisconnectedCauseWorker = nullptr;
        TELEPHONY_LOGE("failed to add uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    if (callDisconnectCauseCallback_.thisVar) {
        (void)memset_s(&callDisconnectCauseCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallDisconnectedCauseWork(uv_work_t *work, int32_t status)
{
    CallDisconnectedCauseWorker *dataWorkerData = (CallDisconnectedCauseWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportDisconnectedCause(dataWorkerData->details, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportDisconnectedCause results %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportDisconnectedCause(
    const DisconnectedDetails &details, EventCallback eventCallback)
{
    napi_env env = eventCallback.env;
    napi_handle_scope disconnectedScope = nullptr;
    napi_open_handle_scope(env, &disconnectedScope);
    if (disconnectedScope == nullptr) {
        TELEPHONY_LOGE("disconnectedScope is nullptr");
        napi_close_handle_scope(env, disconnectedScope);
        return TELEPHONY_ERROR;
    }
    napi_value callbackFunc = nullptr;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
    napi_create_object(env, &callbackValues[ARRAY_INDEX_SECOND]);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_SECOND], "disconnectedCause", static_cast<int32_t>(details.reason));
    NapiCallManagerUtils::SetPropertyStringUtf8(env, callbackValues[ARRAY_INDEX_SECOND], "message", details.message);
    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, disconnectedScope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    napi_close_handle_scope(env, disconnectedScope);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateAsyncResultsInfo(
    const CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    TELEPHONY_LOGI("UpdateAsyncResultsInfo reportId = %{public}d", reportId);
    auto itFunc = memberFuncMap_.find(reportId);
    if (itFunc != memberFuncMap_.end() && itFunc->second != nullptr) {
        auto memberFunc = itFunc->second;
        result = (this->*memberFunc)(resultInfo);
    }
    return result;
}

int32_t NapiCallAbilityCallback::UpdateMmiCodeResultsInfo(const MmiCodeInfo &info)
{
    if (mmiCodeCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("mmiCodeCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(mmiCodeCallback_.env, &loop);
#endif
    MmiCodeWorker *mmiCodeWorker = std::make_unique<MmiCodeWorker>().release();
    if (mmiCodeWorker == nullptr) {
        TELEPHONY_LOGE("mmiCodeWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    mmiCodeWorker->info = info;
    mmiCodeWorker->callback = mmiCodeCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)mmiCodeWorker;
    int32_t errCode = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {}, ReportMmiCodeWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete mmiCodeWorker;
        mmiCodeWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportMmiCodeWork(uv_work_t *work, int32_t status)
{
    MmiCodeWorker *dataWorkerData = (MmiCodeWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportMmiCode(dataWorkerData->info, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportMmiCode result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

/**
 * To notify an application of MMI code result, register a callback with on() first.
 */
int32_t NapiCallAbilityCallback::ReportMmiCode(MmiCodeInfo &info, EventCallback eventCallback)
{
    napi_env env = eventCallback.env;
    napi_handle_scope mmiCodeScope = nullptr;
    napi_open_handle_scope(env, &mmiCodeScope);
    if (mmiCodeScope == nullptr) {
        TELEPHONY_LOGE("mmiCodeScope is nullptr");
        napi_close_handle_scope(env, mmiCodeScope);
        return TELEPHONY_ERROR;
    }
    napi_value callbackFunc = nullptr;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
    callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
    napi_create_object(env, &callbackValues[ARRAY_INDEX_SECOND]);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_SECOND], "result", static_cast<int32_t>(info.result));
    NapiCallManagerUtils::SetPropertyStringUtf8(env, callbackValues[ARRAY_INDEX_SECOND], "message", info.message);
    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, mmiCodeScope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
    napi_close_handle_scope(env, mmiCodeScope);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateAudioDeviceInfo(const AudioDeviceInfo &info)
{
    if (audioDeviceCallback_.thisVar == nullptr) {
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(audioDeviceCallback_.env, &loop);
#endif
    AudioDeviceWork *audioDeviceWork = std::make_unique<AudioDeviceWork>().release();
    if (audioDeviceWork == nullptr) {
        TELEPHONY_LOGE("audioDeviceWork is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    audioDeviceWork->info = info;
    audioDeviceWork->callback = audioDeviceCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)audioDeviceWork;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportAudioDeviceInfoWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete audioDeviceWork;
        audioDeviceWork = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportAudioDeviceInfoWork(uv_work_t *work, int32_t status)
{
    AudioDeviceWork *dataWorkerData = (AudioDeviceWork *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportAudioDeviceInfo(dataWorkerData->info, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportAudioDeviceInfo result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportAudioDeviceInfo(AudioDeviceInfo &info, EventCallback eventCallback)
{
    napi_env env = eventCallback.env;
    napi_handle_scope AudioDeviceInfoScope = nullptr;
    napi_open_handle_scope(env, &AudioDeviceInfoScope);
    if (AudioDeviceInfoScope == nullptr) {
        TELEPHONY_LOGE("AudioDeviceInfoScope is nullptr");
        napi_close_handle_scope(env, AudioDeviceInfoScope);
        return TELEPHONY_ERROR;
    }
    napi_value callbackFunc = nullptr;
    napi_value callbackValues[ARRAY_INDEX_SECOND] = { 0 };
    napi_create_object(env, &callbackValues[ARRAY_INDEX_FIRST]);

    NapiCallManagerUtils::SetPropertyBoolean(env, callbackValues[ARRAY_INDEX_FIRST], "isMuted", info.isMuted);

    napi_value currentAudioDeviceValue = nullptr;
    napi_create_object(env, &currentAudioDeviceValue);
    NapiCallManagerUtils::SetPropertyInt32(env, currentAudioDeviceValue, "deviceType",
        static_cast<int32_t>(info.currentAudioDevice.deviceType));
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, currentAudioDeviceValue, "address", info.currentAudioDevice.address);
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, currentAudioDeviceValue, "deviceName", info.currentAudioDevice.deviceName);
    napi_set_named_property(env, callbackValues[ARRAY_INDEX_FIRST], "currentAudioDevice", currentAudioDeviceValue);

    napi_value audioDeviceListValue = nullptr;
    napi_create_array(env, &audioDeviceListValue);
    std::vector<AudioDevice>::iterator it = info.audioDeviceList.begin();
    int32_t i = 0;
    for (; it != info.audioDeviceList.end(); ++it) {
        napi_value value = nullptr;
        napi_create_object(env, &value);
        NapiCallManagerUtils::SetPropertyInt32(env, value, "deviceType", static_cast<int32_t>(it->deviceType));
        NapiCallManagerUtils::SetPropertyStringUtf8(env, value, "address", it->address);
        NapiCallManagerUtils::SetPropertyStringUtf8(env, value, "deviceName", it->deviceName);
        napi_set_element(env, audioDeviceListValue, i, value);
        ++i;
    }
    napi_set_named_property(env, callbackValues[ARRAY_INDEX_FIRST], "audioDeviceList", audioDeviceListValue);

    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, AudioDeviceInfoScope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    napi_close_handle_scope(env, AudioDeviceInfoScope);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::OttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    if (ottRequestCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("stateCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(ottRequestCallback_.env, &loop);
#endif
    CallOttWorker *callOttWorker = std::make_unique<CallOttWorker>().release();
    if (callOttWorker == nullptr) {
        TELEPHONY_LOGE("callOttWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callOttWorker->requestId = requestId;
    callOttWorker->info = info;
    callOttWorker->callback = ottRequestCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callOttWorker;
    int32_t errCode = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {}, ReportCallOttWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete callOttWorker;
        callOttWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportGetWaitingInfo(AppExecFwk::PacMap &resultInfo)
{
    if (getWaitingCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("getWaitingCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(getWaitingCallback_.env, &loop);
#endif
    CallSupplementWorker *callSupplementWorkerData = std::make_unique<CallSupplementWorker>().release();
    if (callSupplementWorkerData == nullptr) {
        TELEPHONY_LOGE("callSupplementWorkerData is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSupplementWorkerData->info = resultInfo;
    callSupplementWorkerData->callback = getWaitingCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSupplementWorkerData;
    int32_t errorCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportWaitAndLimitInfoWork, uv_qos_default);
    if (errorCode != 0) {
        delete callSupplementWorkerData;
        callSupplementWorkerData = nullptr;
        delete work;
        work = nullptr;
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errorCode: %{public}d", errorCode);
        return TELEPHONY_ERROR;
    }
    if (getWaitingCallback_.thisVar) {
        (void)memset_s(&getWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportCloseUnFinishedUssdInfo(AppExecFwk::PacMap &resultInfo)
{
    if (closeUnfinishedUssdCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("closeUnfinishedUssdCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(closeUnfinishedUssdCallback_.env, &loop);
#endif
    CallSupplementWorker *callSupplementDataWorker = std::make_unique<CallSupplementWorker>().release();
    if (callSupplementDataWorker == nullptr) {
        TELEPHONY_LOGE("callSupplementDataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSupplementDataWorker->info = resultInfo;
    callSupplementDataWorker->callback = closeUnfinishedUssdCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSupplementDataWorker;
    int32_t errorCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportExecutionResultWork, uv_qos_default);
    if (errorCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errorCode: %{public}d", errorCode);
        delete callSupplementDataWorker;
        callSupplementDataWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    if (closeUnfinishedUssdCallback_.thisVar) {
        (void)memset_s(&closeUnfinishedUssdCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportSetWaitingInfo(AppExecFwk::PacMap &resultInfo)
{
    if (setWaitingCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("setWaitingCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(setWaitingCallback_.env, &loop);
#endif
    CallSupplementWorker *callSupplementWorker = std::make_unique<CallSupplementWorker>().release();
    if (callSupplementWorker == nullptr) {
        TELEPHONY_LOGE("callSupplementWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSupplementWorker->info = resultInfo;
    callSupplementWorker->callback = setWaitingCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSupplementWorker;
    int32_t resultCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportExecutionResultWork, uv_qos_default);
    if (resultCode != 0) {
        delete callSupplementWorker;
        callSupplementWorker = nullptr;
        delete work;
        work = nullptr;
        TELEPHONY_LOGE("failed to add uv_queue_work_with_qos, resultCode: %{public}d", resultCode);
        return TELEPHONY_ERROR;
    }
    if (setWaitingCallback_.thisVar) {
        (void)memset_s(&setWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportGetRestrictionInfo(AppExecFwk::PacMap &resultInfo)
{
    if (getRestrictionCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("getRestrictionCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(getRestrictionCallback_.env, &loop);
#endif
    CallSupplementWorker *callSupplementWorker = std::make_unique<CallSupplementWorker>().release();
    if (callSupplementWorker == nullptr) {
        TELEPHONY_LOGE("callSupplementWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSupplementWorker->info = resultInfo;
    callSupplementWorker->callback = getRestrictionCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSupplementWorker;
    int32_t resultCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportWaitAndLimitInfoWork, uv_qos_default);
    if (resultCode != 0) {
        delete callSupplementWorker;
        callSupplementWorker = nullptr;
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, resultCode: %{public}d", resultCode);
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    if (getRestrictionCallback_.thisVar) {
        (void)memset_s(&getRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportSetRestrictionInfo(AppExecFwk::PacMap &resultInfo)
{
    if (setRestrictionCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("setRestrictionCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(setRestrictionCallback_.env, &loop);
#endif
    CallSupplementWorker *callSupplementWorker = std::make_unique<CallSupplementWorker>().release();
    if (callSupplementWorker == nullptr) {
        TELEPHONY_LOGE("callSupplementWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSupplementWorker->info = resultInfo;
    callSupplementWorker->callback = setRestrictionCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSupplementWorker;
    int32_t resultCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportExecutionResultWork, uv_qos_default);
    if (resultCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, resultCode: %{public}d", resultCode);
        delete callSupplementWorker;
        callSupplementWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    if (setRestrictionCallback_.thisVar) {
        (void)memset_s(&setRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportSetRestrictionPassword(AppExecFwk::PacMap &resultInfo)
{
    if (setRestrictionPasswordCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("setRestrictionPasswordCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(setRestrictionPasswordCallback_.env, &loop);
#endif
    CallSupplementWorker *callSupplementWorker = std::make_unique<CallSupplementWorker>().release();
    if (callSupplementWorker == nullptr) {
        TELEPHONY_LOGE("callSupplementWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSupplementWorker->info = resultInfo;
    callSupplementWorker->callback = setRestrictionPasswordCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSupplementWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportExecutionResultWork, uv_qos_default);
    if (errCode != 0) {
        delete callSupplementWorker;
        callSupplementWorker = nullptr;
        delete work;
        work = nullptr;
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        return TELEPHONY_ERROR;
    }
    if (setRestrictionPasswordCallback_.thisVar) {
        (void)memset_s(&setRestrictionPasswordCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportGetTransferInfo(AppExecFwk::PacMap &resultInfo)
{
    if (getTransferCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("getTransferCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    if (getCallTransferReason_ != resultInfo.GetIntValue("reason")) {
        TELEPHONY_LOGE("Transfer reason is different, require is %{public}d, now is %{public}d", getCallTransferReason_,
            resultInfo.GetIntValue("reason"));
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(getTransferCallback_.env, &loop);
#endif
    CallSupplementWorker *callSupplementWorker = std::make_unique<CallSupplementWorker>().release();
    if (callSupplementWorker == nullptr) {
        TELEPHONY_LOGE("callSupplementWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSupplementWorker->info = resultInfo;
    callSupplementWorker->callback = getTransferCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSupplementWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportSupplementInfoWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed add to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete callSupplementWorker;
        callSupplementWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    if (getTransferCallback_.thisVar) {
        (void)memset_s(&getTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportSetTransferInfo(AppExecFwk::PacMap &resultInfo)
{
    if (setTransferCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("setTransferCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(setTransferCallback_.env, &loop);
#endif
    CallSupplementWorker *callSupplementWorker = std::make_unique<CallSupplementWorker>().release();
    if (callSupplementWorker == nullptr) {
        TELEPHONY_LOGE("callSupplementWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSupplementWorker->info = resultInfo;
    callSupplementWorker->callback = setTransferCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSupplementWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportExecutionResultWork, uv_qos_default);
    if (errCode != 0) {
        delete callSupplementWorker;
        callSupplementWorker = nullptr;
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    if (setTransferCallback_.thisVar) {
        (void)memset_s(&setTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportWaitAndLimitInfoWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportWaitAndLimitInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportWaitAndLimitInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo)
{
    napi_env env = supplementInfo.env;
    int32_t result = resultInfo.GetIntValue("result");
    int32_t status = resultInfo.GetIntValue("status");
    napi_handle_scope limitScope = nullptr;
    napi_open_handle_scope(env, &limitScope);
    if (limitScope == nullptr) {
        TELEPHONY_LOGE("limitScope is nullptr");
        napi_close_handle_scope(env, limitScope);
        return;
    }
    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_int32(env, status, &callbackValues[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, supplementInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
        napi_delete_reference(env, supplementInfo.thisVar);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_int32(env, status, &promiseValue);
            napi_resolve_deferred(env, supplementInfo.deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            napi_reject_deferred(env, supplementInfo.deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    }
    napi_close_handle_scope(env, limitScope);
}

void NapiCallAbilityCallback::ReportSupplementInfoWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportSupplementInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportSupplementInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo)
{
    napi_env env = supplementInfo.env;
    napi_handle_scope supplementScope = nullptr;
    napi_open_handle_scope(env, &supplementScope);
    if (supplementScope == nullptr) {
        TELEPHONY_LOGE("supplementScope is nullptr");
        napi_close_handle_scope(env, supplementScope);
        return;
    }
    napi_value callbackValue = nullptr;
    napi_create_object(env, &callbackValue);
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValue, "status", resultInfo.GetIntValue("status"));
    NapiCallManagerUtils::SetPropertyStringUtf8(env, callbackValue, "number", resultInfo.GetStringValue("number"));
    int32_t result = resultInfo.GetIntValue("result");
    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            callbackValues[ARRAY_INDEX_SECOND] = callbackValue;
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(result);
            callbackValues[ARRAY_INDEX_FIRST] =
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, supplementInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
        napi_delete_reference(env, supplementInfo.thisVar);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_resolve_deferred(env, supplementInfo.deferred, callbackValue);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(result);
            napi_reject_deferred(env, supplementInfo.deferred,
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode));
        }
    }
    napi_close_handle_scope(env, supplementScope);
}

void NapiCallAbilityCallback::ReportExecutionResultWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportExecutionResult(dataWorkerData->callback, dataWorkerData->info);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportExecutionResult(EventCallback &settingInfo, AppExecFwk::PacMap &resultInfo)
{
    napi_env env = settingInfo.env;
    napi_handle_scope executionScope = nullptr;
    napi_open_handle_scope(env, &executionScope);
    if (executionScope == nullptr) {
        TELEPHONY_LOGE("executionScope is nullptr");
        napi_close_handle_scope(env, executionScope);
        return;
    }
    napi_value callbackValue = nullptr;
    napi_create_object(env, &callbackValue);
    int32_t result = resultInfo.GetIntValue("result");
    if (settingInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_null(env, &callbackValues[ARRAY_INDEX_SECOND]);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(result);
            callbackValues[ARRAY_INDEX_FIRST] =
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, settingInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, settingInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, settingInfo.callbackRef);
        napi_delete_reference(env, settingInfo.thisVar);
    } else if (settingInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_resolve_deferred(env, settingInfo.deferred, promiseValue);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(result);
            napi_reject_deferred(env, settingInfo.deferred,
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode));
        }
    }
    napi_close_handle_scope(env, executionScope);
}

void NapiCallAbilityCallback::ReportStartRttInfoWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportStartRttInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportStartRttInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo)
{
    napi_env env = supplementInfo.env;
    int32_t result = resultInfo.GetIntValue("result");
    napi_handle_scope startRttScope = nullptr;
    napi_open_handle_scope(env, &startRttScope);
    if (startRttScope == nullptr) {
        TELEPHONY_LOGE("startRttScope is nullptr");
        napi_close_handle_scope(env, startRttScope);
        return;
    }
    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, supplementInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
        napi_delete_reference(env, supplementInfo.thisVar);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_resolve_deferred(env, supplementInfo.deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            napi_reject_deferred(env, supplementInfo.deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    }
    napi_close_handle_scope(env, startRttScope);
}

void NapiCallAbilityCallback::ReportStopRttInfoWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportStopRttInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportStopRttInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo)
{
    napi_env env = supplementInfo.env;
    int32_t result = resultInfo.GetIntValue("result");
    napi_handle_scope stopRttScope = nullptr;
    napi_open_handle_scope(env, &stopRttScope);
    if (stopRttScope == nullptr) {
        TELEPHONY_LOGE("stopRttScope is nullptr");
        napi_close_handle_scope(env, stopRttScope);
        return;
    }
    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, supplementInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
        napi_delete_reference(env, supplementInfo.thisVar);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_resolve_deferred(env, supplementInfo.deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            napi_reject_deferred(env, supplementInfo.deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    }
    napi_close_handle_scope(env, stopRttScope);
}

void NapiCallAbilityCallback::ReportCallOttWork(uv_work_t *work, int32_t status)
{
    CallOttWorker *dataWorkerData = (CallOttWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportCallOtt(dataWorkerData->callback, dataWorkerData->info, dataWorkerData->requestId);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportCallOtt(
    EventCallback &settingInfo, AppExecFwk::PacMap &resultInfo, OttCallRequestId requestId)
{
    napi_env env = settingInfo.env;
    napi_handle_scope callOttscope = nullptr;
    napi_open_handle_scope(env, &callOttscope);
    if (callOttscope == nullptr) {
        TELEPHONY_LOGE("callOttscope is nullptr");
        napi_close_handle_scope(env, callOttscope);
        return TELEPHONY_ERROR;
    }
    napi_value callbackFunc = nullptr;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
    napi_create_object(env, &callbackValues[ARRAY_INDEX_FIRST]);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "requestId", static_cast<int32_t>(requestId));
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, callbackValues[ARRAY_INDEX_FIRST], "phoneNumber", resultInfo.GetStringValue("phoneNumber").c_str());
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, callbackValues[ARRAY_INDEX_FIRST], "bundleName", resultInfo.GetStringValue("bundleName").c_str());
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "videoState", resultInfo.GetIntValue("videoState"));
    switch (requestId) {
        case OttCallRequestId::OTT_REQUEST_INVITE_TO_CONFERENCE:
            NapiCallManagerUtils::SetPropertyStringUtf8(
                env, callbackValues[ARRAY_INDEX_FIRST], "number", resultInfo.GetStringValue("number").c_str());
            break;
        case OttCallRequestId::OTT_REQUEST_UPDATE_CALL_MEDIA_MODE:
            NapiCallManagerUtils::SetPropertyInt32(
                env, callbackValues[ARRAY_INDEX_FIRST], "callMediaMode", resultInfo.GetIntValue("callMediaMode"));
            break;
        default:
            break;
    }

    napi_get_reference_value(env, settingInfo.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, callOttscope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, settingInfo.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    napi_close_handle_scope(env, callOttscope);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdatePostDialDelay(const std::string str)
{
    if (postDialDelayCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("thisVar is nullptr!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(postDialDelayCallback_.env, &loop);
#endif
    PostDialDelayWorker *postDialDelayWorker = std::make_unique<PostDialDelayWorker>().release();
    if (postDialDelayWorker == nullptr) {
        TELEPHONY_LOGE("postDialDelayWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    postDialDelayWorker->postDialStr = str;
    postDialDelayWorker->callback = postDialDelayCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)postDialDelayWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportPostDialDelayWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete postDialDelayWorker;
        postDialDelayWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportPostDialDelayWork(uv_work_t *work, int32_t status)
{
    PostDialDelayWorker *dataWorkerData = (PostDialDelayWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportPostDialDelay(dataWorkerData->postDialStr, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportPostDialDelay result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportPostDialDelay(std::string postDialStr, EventCallback eventCallback)
{
    napi_value callbackFunc = nullptr;
    napi_env env = eventCallback.env;
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        TELEPHONY_LOGE("scope is nullptr");
        napi_close_handle_scope(env, scope);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    napi_value callbackValues[ARRAY_INDEX_SECOND] = { 0 };
    napi_create_string_utf8(env, postDialStr.c_str(), postDialStr.length(), &callbackValues[ARRAY_INDEX_FIRST]);
    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, scope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    napi_close_handle_scope(env, scope);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
{
    if (imsCallModeCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("thisVar is nullptr!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(imsCallModeCallback_.env, &loop);
#endif
    ImsCallModeInfoWorker *imsCallModeInfoWorker = std::make_unique<ImsCallModeInfoWorker>().release();
    if (imsCallModeInfoWorker == nullptr) {
        TELEPHONY_LOGE("imsCallModeInfoWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    imsCallModeInfoWorker->callModeInfo = imsCallModeInfo;
    imsCallModeInfoWorker->callback = imsCallModeCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)imsCallModeInfoWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportCallMediaModeInfoWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete imsCallModeInfoWorker;
        imsCallModeInfoWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallMediaModeInfoWork(uv_work_t *work, int32_t status)
{
    ImsCallModeInfoWorker *dataWorkerData = (ImsCallModeInfoWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportCallMediaModeInfo(dataWorkerData->callModeInfo, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportPostDialDelay result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportCallMediaModeInfo(
    CallMediaModeInfo &imsCallModeInfo, EventCallback eventCallback)
{
    napi_env env = eventCallback.env;
    napi_handle_scope CallMediaModeInfoScope = nullptr;
    napi_open_handle_scope(env, &CallMediaModeInfoScope);
    if (CallMediaModeInfoScope == nullptr) {
        TELEPHONY_LOGE("CallMediaModeInfoScope is nullptr");
        napi_close_handle_scope(env, CallMediaModeInfoScope);
        return TELEPHONY_ERROR;
    }
    napi_value callbackFunc = nullptr;
    napi_value callbackValues[ARRAY_INDEX_SECOND] = { 0 };

    napi_create_object(env, &callbackValues[ARRAY_INDEX_FIRST]);
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValues[ARRAY_INDEX_FIRST], "callId", imsCallModeInfo.callId);
    NapiCallManagerUtils::SetPropertyBoolean(
        env, callbackValues[ARRAY_INDEX_FIRST], "isRequestInfo", imsCallModeInfo.isRequestInfo);
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValues[ARRAY_INDEX_FIRST], "result", imsCallModeInfo.result);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "imsCallMode", static_cast<int32_t>(imsCallModeInfo.callMode));

    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, CallMediaModeInfoScope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    napi_close_handle_scope(env, CallMediaModeInfoScope);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::CallSessionEventChange(const CallSessionEvent &callSessionEvent)
{
    if (callSessionEventCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("thisVar is nullptr!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(callSessionEventCallback_.env, &loop);
#endif
    CallSessionEventWorker *callSessionEventWorker = std::make_unique<CallSessionEventWorker>().release();
    if (callSessionEventWorker == nullptr) {
        TELEPHONY_LOGE("callSessionEventWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callSessionEventWorker->sessionEvent = callSessionEvent;
    callSessionEventWorker->callback = callSessionEventCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callSessionEventWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportCallSessionEventWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete callSessionEventWorker;
        callSessionEventWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallSessionEventWork(uv_work_t *work, int32_t status)
{
    CallSessionEventWorker *dataWorkerData = (CallSessionEventWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportCallSessionEvent(dataWorkerData->sessionEvent, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportPostDialDelay result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportCallSessionEvent(
    CallSessionEvent &sessionEvent, EventCallback eventCallback)
{
    napi_env env = eventCallback.env;
    napi_handle_scope CallSessionEventScope = nullptr;
    napi_open_handle_scope(env, &CallSessionEventScope);
    if (CallSessionEventScope == nullptr) {
        TELEPHONY_LOGE("CallMediaModeInfoScope is nullptr");
        napi_close_handle_scope(env, CallSessionEventScope);
        return TELEPHONY_ERROR;
    }
    napi_value callbackFunc = nullptr;
    napi_value callbackValues[ARRAY_INDEX_SECOND] = { 0 };

    napi_create_object(env, &callbackValues[ARRAY_INDEX_FIRST]);
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValues[ARRAY_INDEX_FIRST], "callId", sessionEvent.callId);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "eventId", static_cast<int32_t>(sessionEvent.eventId));

    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, CallSessionEventScope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    napi_close_handle_scope(env, CallSessionEventScope);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::PeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
{
    if (peerDimensionsCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("thisVar is nullptr!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(peerDimensionsCallback_.env, &loop);
#endif
    PeerDimensionsWorker *peerDimensionsWorker = std::make_unique<PeerDimensionsWorker>().release();
    if (peerDimensionsWorker == nullptr) {
        TELEPHONY_LOGE("peerDimensionsWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    peerDimensionsWorker->peerDimensionsDetail = peerDimensionsDetail;
    peerDimensionsWorker->callback = peerDimensionsCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)peerDimensionsWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportPeerDimensionsWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete peerDimensionsWorker;
        peerDimensionsWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportPeerDimensionsWork(uv_work_t *work, int32_t status)
{
    PeerDimensionsWorker *dataWorkerData = (PeerDimensionsWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportPeerDimensions(dataWorkerData->peerDimensionsDetail, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportPostDialDelay result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportPeerDimensions(
    PeerDimensionsDetail &peerDimensionsDetail, EventCallback eventCallback)
{
    napi_env env = eventCallback.env;
    napi_handle_scope PeerDimensionsDetailScope = nullptr;
    napi_open_handle_scope(env, &PeerDimensionsDetailScope);
    if (PeerDimensionsDetailScope == nullptr) {
        TELEPHONY_LOGE("CallMediaModeInfoScope is nullptr");
        napi_close_handle_scope(env, PeerDimensionsDetailScope);
        return TELEPHONY_ERROR;
    }
    napi_value callbackFunc = nullptr;
    napi_value callbackValues[ARRAY_INDEX_SECOND] = { 0 };

    napi_create_object(env, &callbackValues[ARRAY_INDEX_FIRST]);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "callId", peerDimensionsDetail.callId);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "width", peerDimensionsDetail.width);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "height", peerDimensionsDetail.height);

    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, PeerDimensionsDetailScope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    napi_close_handle_scope(env, PeerDimensionsDetailScope);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::CallDataUsageChange(const int64_t dataUsage)
{
    if (callDataUsageCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("thisVar is nullptr!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(callDataUsageCallback_.env, &loop);
#endif
    CallDataUsageWorker *callDataUsageWorker = std::make_unique<CallDataUsageWorker>().release();
    if (callDataUsageWorker == nullptr) {
        TELEPHONY_LOGE("callDataUsageWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    callDataUsageWorker->callDataUsage = dataUsage;
    callDataUsageWorker->callback = callDataUsageCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)callDataUsageWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportCallDataUsageWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete callDataUsageWorker;
        callDataUsageWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallDataUsageWork(uv_work_t *work, int32_t status)
{
    CallDataUsageWorker *dataWorkerData = (CallDataUsageWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportCallDataUsage(dataWorkerData->callDataUsage, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportPostDialDelay result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportCallDataUsage(int64_t dataUsage, EventCallback eventCallback)
{
    napi_env env = eventCallback.env;
    napi_handle_scope CallDataUsageScope = nullptr;
    napi_open_handle_scope(env, &CallDataUsageScope);
    if (CallDataUsageScope == nullptr) {
        TELEPHONY_LOGE("CallMediaModeInfoScope is nullptr");
        napi_close_handle_scope(env, CallDataUsageScope);
        return TELEPHONY_ERROR;
    }
    napi_value callbackFunc = nullptr;
    napi_value callbackValues[ARRAY_INDEX_SECOND] = { 0 };
    napi_create_int64(env, dataUsage, &callbackValues[ARRAY_INDEX_FIRST]);

    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, CallDataUsageScope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    napi_close_handle_scope(env, CallDataUsageScope);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateCameraCapabilities(const CameraCapabilities &cameraCapabilities)
{
    if (cameraCapabilitiesCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("thisVar is nullptr!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if defined(NAPI_VERSION) && NAPI_VERSION >= 2
    napi_get_uv_event_loop(cameraCapabilitiesCallback_.env, &loop);
#endif
    CameraCapbilitiesWorker *cameraCapbilitiesWorker = std::make_unique<CameraCapbilitiesWorker>().release();
    if (cameraCapbilitiesWorker == nullptr) {
        TELEPHONY_LOGE("cameraCapbilitiesWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    cameraCapbilitiesWorker->cameraCapabilities = cameraCapabilities;
    cameraCapbilitiesWorker->callback = cameraCapabilitiesCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)cameraCapbilitiesWorker;
    int32_t errCode = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, ReportCameraCapabilitiesInfoWork, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        delete cameraCapbilitiesWorker;
        cameraCapbilitiesWorker = nullptr;
        delete work;
        work = nullptr;
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCameraCapabilitiesInfoWork(uv_work_t *work, int32_t status)
{
    CameraCapbilitiesWorker *dataWorkerData = (CameraCapbilitiesWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportCameraCapabilitiesInfo(dataWorkerData->cameraCapabilities, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportPostDialDelay result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportCameraCapabilitiesInfo(
    CameraCapabilities &cameraCapabilities, EventCallback eventCallback)
{
    napi_env env = eventCallback.env;
    napi_handle_scope cameraCapabilitiesScope = nullptr;
    napi_open_handle_scope(env, &cameraCapabilitiesScope);
    if (cameraCapabilitiesScope == nullptr) {
        TELEPHONY_LOGE("CallMediaModeInfoScope is nullptr");
        napi_close_handle_scope(env, cameraCapabilitiesScope);
        return TELEPHONY_ERROR;
    }
    napi_value callbackFunc = nullptr;
    napi_value callbackValues[ARRAY_INDEX_SECOND] = { 0 };

    napi_create_object(env, &callbackValues[ARRAY_INDEX_FIRST]);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "callId", cameraCapabilities.callId);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "width", cameraCapabilities.width);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "height", cameraCapabilities.height);

    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        napi_close_handle_scope(env, cameraCapabilitiesScope);
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    napi_close_handle_scope(env, cameraCapabilitiesScope);
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
