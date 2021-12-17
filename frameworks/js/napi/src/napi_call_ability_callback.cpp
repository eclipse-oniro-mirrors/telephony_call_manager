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

#include "napi_call_ability_callback.h"

#include <securec.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
NapiCallAbilityCallback::NapiCallAbilityCallback()
{
    (void)memset_s(&stateCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    (void)memset_s(&eventCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    (void)memset_s(&getWaitingCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    (void)memset_s(&setWaitingCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    (void)memset_s(&getRestrictionCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    (void)memset_s(&setRestrictionCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    (void)memset_s(&getTransferCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    (void)memset_s(&setTransferCallback_, sizeof(EventListener), 0, sizeof(EventListener));
}

NapiCallAbilityCallback::~NapiCallAbilityCallback() {}

void NapiCallAbilityCallback::RegisterCallStateCallback(EventListener stateCallback)
{
    stateCallback_ = stateCallback;
}

void NapiCallAbilityCallback::UnRegisterCallStateCallback()
{
    if (stateCallback_.callbackRef) {
        (void)memset_s(&stateCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    }
}

void NapiCallAbilityCallback::RegisterCallEventCallback(EventListener eventCallback)
{
    eventCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterCallEventCallback()
{
    if (eventCallback_.callbackRef) {
        (void)memset_s(&eventCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    }
}

int32_t NapiCallAbilityCallback::RegisterGetWaitingCallback(EventListener callback)
{
    if (getWaitingCallback_.thisVar) {
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    getWaitingCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterGetWaitingCallback()
{
    (void)memset_s(&getWaitingCallback_, sizeof(EventListener), 0, sizeof(EventListener));
}

int32_t NapiCallAbilityCallback::RegisterSetWaitingCallback(EventListener callback)
{
    if (setWaitingCallback_.thisVar) {
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    setWaitingCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterSetWaitingCallback()
{
    (void)memset_s(&setWaitingCallback_, sizeof(EventListener), 0, sizeof(EventListener));
}

int32_t NapiCallAbilityCallback::RegisterGetRestrictionCallback(EventListener callback)
{
    if (getRestrictionCallback_.thisVar) {
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    getRestrictionCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterGetRestrictionCallback()
{
    (void)memset_s(&getRestrictionCallback_, sizeof(EventListener), 0, sizeof(EventListener));
}

int32_t NapiCallAbilityCallback::RegisterSetRestrictionCallback(EventListener callback)
{
    if (setRestrictionCallback_.thisVar) {
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    setRestrictionCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterSetRestrictionCallback()
{
    (void)memset_s(&setRestrictionCallback_, sizeof(EventListener), 0, sizeof(EventListener));
}

int32_t NapiCallAbilityCallback::RegisterGetTransferCallback(EventListener callback)
{
    if (getTransferCallback_.thisVar) {
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    getTransferCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterGetTransferCallback()
{
    (void)memset_s(&getTransferCallback_, sizeof(EventListener), 0, sizeof(EventListener));
}

int32_t NapiCallAbilityCallback::RegisterSetTransferCallback(EventListener callback)
{
    if (setTransferCallback_.thisVar) {
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    setTransferCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterSetTransferCallback()
{
    (void)memset_s(&setTransferCallback_, sizeof(EventListener), 0, sizeof(EventListener));
}

int32_t NapiCallAbilityCallback::UpdateCallStateInfo(const CallAttributeInfo &info)
{
    if (stateCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("stateCallback is null!");
        return TELEPHONY_ERR_FAIL;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(stateCallback_.env, &loop);
#endif
    CallStateWorker *dataWorker = new (std::nothrow) CallStateWorker();
    if (!dataWorker) {
        return TELEPHONY_ERR_FAIL;
    }
    dataWorker->info = info;
    dataWorker->callback = stateCallback_;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (!work) {
        return TELEPHONY_ERR_FAIL;
    }
    work->data = (void *)dataWorker;

    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportCallStateWork);

    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallStateWork(uv_work_t *work, int status)
{
    CallStateWorker *dataWorkerData = (CallStateWorker *)work->data;
    if (dataWorkerData == nullptr) {
        return;
    }
    int32_t ret = ReportCallState(dataWorkerData->info, dataWorkerData->callback);
    TELEPHONY_LOGI("%{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportCallState(CallAttributeInfo &info, EventListener stateCallback)
{
    napi_value callbackFunc = nullptr;
    napi_env env = stateCallback.env;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = {0};
    callbackValues[ARRAY_INDEX_FIRST] = CreateUndefined(env);
    napi_create_object(env, &callbackValues[ARRAY_INDEX_SECOND]);
    SetPropertyStringUtf8(env, callbackValues[ARRAY_INDEX_SECOND], "accountNumber", info.accountNumber);
    SetPropertyInt32(env, callbackValues[ARRAY_INDEX_SECOND], "accountId", info.accountId);
    SetPropertyInt32(env, callbackValues[ARRAY_INDEX_SECOND], "videoState", info.videoState);
    SetPropertyInt32(env, callbackValues[ARRAY_INDEX_SECOND], "callType", info.callType);
    SetPropertyInt32(env, callbackValues[ARRAY_INDEX_SECOND], "callId", info.callId);
    SetPropertyInt32(env, callbackValues[ARRAY_INDEX_SECOND], "callState", info.callState);
    SetPropertyInt32(env, callbackValues[ARRAY_INDEX_SECOND], "conferenceState", info.conferenceState);
    SetPropertyInt32(env, callbackValues[ARRAY_INDEX_SECOND], "startTime", info.startTime);
    napi_get_reference_value(env, stateCallback.callbackRef, &callbackFunc);
    napi_value callbackResult = nullptr;
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        return TELEPHONY_ERR_FAIL;
    }
    napi_call_function(env, stateCallback.thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateCallEvent(const CallEventInfo &info)
{
    if (eventCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("eventCallback is null!");
        return TELEPHONY_ERR_FAIL;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(eventCallback_.env, &loop);
#endif
    CallEventWorker *dataWorker = new (std::nothrow) CallEventWorker();
    if (!dataWorker) {
        return TELEPHONY_ERR_FAIL;
    }
    dataWorker->info = info;
    dataWorker->callback = eventCallback_;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (!work) {
        return TELEPHONY_ERR_FAIL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportCallEventWork);
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallEventWork(uv_work_t *work, int status)
{
    CallEventWorker *dataWorkerData = (CallEventWorker *)work->data;
    if (dataWorkerData == nullptr) {
        return;
    }
    int32_t ret = ReportCallEvent(dataWorkerData->info, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportCallEvent results %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportCallEvent(CallEventInfo &info, EventListener eventCallback)
{
    napi_value callbackFunc = nullptr;
    napi_env env = eventCallback.env;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = {0};
    callbackValues[ARRAY_INDEX_FIRST] = CreateUndefined(env);
    napi_create_object(env, &callbackValues[ARRAY_INDEX_SECOND]);
    SetPropertyInt32(env, callbackValues[ARRAY_INDEX_SECOND], "eventId", info.eventId);
    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    napi_value callbackResult = nullptr;
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        return TELEPHONY_ERR_FAIL;
    }
    napi_call_function(env, eventCallback.thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateSupplementInfo(
    const CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    switch (reportId) {
        case CallResultReportId::GET_CALL_WAITING_REPORT_ID:
            ReportGetWaitingInfo(resultInfo);
            break;
        case CallResultReportId::SET_CALL_WAITING_REPORT_ID:
            ReportSetWaitingInfo(resultInfo);
            break;
        case CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID:
            ReportGetRestrictionInfo(resultInfo);
            break;
        case CallResultReportId::SET_CALL_RESTRICTION_REPORT_ID:
            ReportSetRestrictionInfo(resultInfo);
            break;
        case CallResultReportId::GET_CALL_TRANSFER_REPORT_ID:
            ReportGetTransferInfo(resultInfo);
            break;
        case CallResultReportId::SET_CALL_TRANSFER_REPORT_ID:
            ReportSetTransferInfo(resultInfo);
            break;
        default:
            break;
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportGetWaitingInfo(AppExecFwk::PacMap &resultInfo)
{
    if (getWaitingCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("getWaitingCallback is null!");
        return TELEPHONY_ERR_FAIL;
    }

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(getWaitingCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = new (std::nothrow) CallSupplementWorker();
    if (!dataWorker) {
        return TELEPHONY_ERR_FAIL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = getWaitingCallback_;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (!work) {
        return TELEPHONY_ERR_FAIL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportWaitAndLimitInfoWork);
    if (getWaitingCallback_.thisVar) {
        (void)memset_s(&getWaitingCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportSetWaitingInfo(AppExecFwk::PacMap &resultInfo)
{
    if (setWaitingCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("setWaitingCallback is null!");
        return TELEPHONY_ERR_FAIL;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(setWaitingCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = new (std::nothrow) CallSupplementWorker();
    if (!dataWorker) {
        return TELEPHONY_ERR_FAIL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = setWaitingCallback_;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (!work) {
        return TELEPHONY_ERR_FAIL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportSettingInfoWork);
    if (setWaitingCallback_.thisVar) {
        (void)memset_s(&setWaitingCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportGetRestrictionInfo(AppExecFwk::PacMap &resultInfo)
{
    if (getRestrictionCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("getRestrictionCallback is null!");
        return TELEPHONY_ERR_FAIL;
    }

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(getRestrictionCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = new (std::nothrow) CallSupplementWorker();
    if (!dataWorker) {
        return TELEPHONY_ERR_FAIL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = getRestrictionCallback_;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (!work) {
        return TELEPHONY_ERR_FAIL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportWaitAndLimitInfoWork);
    if (getRestrictionCallback_.thisVar) {
        (void)memset_s(&getRestrictionCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportSetRestrictionInfo(AppExecFwk::PacMap &resultInfo)
{
    if (setRestrictionCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("setRestrictionCallback is null!");
        return TELEPHONY_ERR_FAIL;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(setRestrictionCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = new (std::nothrow) CallSupplementWorker();
    if (!dataWorker) {
        return TELEPHONY_ERR_FAIL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = setRestrictionCallback_;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (!work) {
        return TELEPHONY_ERR_FAIL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportSettingInfoWork);
    if (setRestrictionCallback_.thisVar) {
        (void)memset_s(&setRestrictionCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportGetTransferInfo(AppExecFwk::PacMap &resultInfo)
{
    if (getTransferCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("getTransferCallback is null!");
        return TELEPHONY_ERR_FAIL;
    }

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(getTransferCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = new (std::nothrow) CallSupplementWorker();
    if (!dataWorker) {
        return TELEPHONY_ERR_FAIL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = getTransferCallback_;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (!work) {
        return TELEPHONY_ERR_FAIL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportSupplementInfoWork);
    if (getTransferCallback_.thisVar) {
        (void)memset_s(&getTransferCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportSetTransferInfo(AppExecFwk::PacMap &resultInfo)
{
    if (setTransferCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("setTransferCallback is null!");
        return TELEPHONY_ERR_FAIL;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(setTransferCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = new (std::nothrow) CallSupplementWorker();
    if (!dataWorker) {
        return TELEPHONY_ERR_FAIL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = setTransferCallback_;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (!work) {
        return TELEPHONY_ERR_FAIL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportSettingInfoWork);
    if (setTransferCallback_.thisVar) {
        (void)memset_s(&setTransferCallback_, sizeof(EventListener), 0, sizeof(EventListener));
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportWaitAndLimitInfoWork(uv_work_t *work, int status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        return;
    }
    ReportWaitAndLimitInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportWaitAndLimitInfo(AppExecFwk::PacMap &resultInfo, EventListener supplementInfo)
{
    napi_env env = supplementInfo.env;
    int32_t result = resultInfo.GetIntValue("result");
    int32_t status = resultInfo.GetIntValue("status");

    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = {0};
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = CreateUndefined(env);
            napi_create_int32(env, status, &callbackValues[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(result);
            callbackValues[ARRAY_INDEX_FIRST] = CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value callbackResult = nullptr;
        napi_call_function(
            env, supplementInfo.thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_int32(env, status, &promiseValue);
            napi_resolve_deferred(env, supplementInfo.deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(result);
            napi_reject_deferred(env, supplementInfo.deferred, CreateErrorMessage(env, errTip));
        }
    }
}

void NapiCallAbilityCallback::ReportSupplementInfoWork(uv_work_t *work, int status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        return;
    }
    ReportSupplementInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportSupplementInfo(AppExecFwk::PacMap &resultInfo, EventListener supplementInfo)
{
    napi_env env = supplementInfo.env;
    napi_value callbackValue = nullptr;
    napi_create_object(env, &callbackValue);
    SetPropertyInt32(env, callbackValue, "status", resultInfo.GetIntValue("status"));
    SetPropertyStringUtf8(env, callbackValue, "number", resultInfo.GetStringValue("number"));
    int32_t result = resultInfo.GetIntValue("result");

    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = {0};
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = CreateUndefined(env);
            callbackValues[ARRAY_INDEX_SECOND] = callbackValue;
        } else {
            std::string errTip = std::to_string(result);
            callbackValues[ARRAY_INDEX_FIRST] = CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value callbackResult = nullptr;
        napi_call_function(
            env, supplementInfo.thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_resolve_deferred(env, supplementInfo.deferred, callbackValue);
        } else {
            std::string errTip = std::to_string(result);
            napi_reject_deferred(env, supplementInfo.deferred, CreateErrorMessage(env, errTip));
        }
    }
}

void NapiCallAbilityCallback::ReportSettingInfoWork(uv_work_t *work, int status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        return;
    }
    ReportSettingInfo(dataWorkerData->callback, dataWorkerData->info);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportSettingInfo(EventListener &settingInfo, AppExecFwk::PacMap &resultInfo)
{
    napi_env env = settingInfo.env;
    napi_value callbackValue = nullptr;
    napi_create_object(env, &callbackValue);
    int32_t result = resultInfo.GetIntValue("result");

    if (settingInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = {0};
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = CreateUndefined(env);
            napi_get_null(env, &callbackValues[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(result);
            callbackValues[ARRAY_INDEX_FIRST] = CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, settingInfo.callbackRef, &callbackFunc);
        napi_value callbackResult = nullptr;
        napi_call_function(
            env, settingInfo.thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, settingInfo.callbackRef);
    } else if (settingInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_resolve_deferred(env, settingInfo.deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(result);
            napi_reject_deferred(env, settingInfo.deferred, CreateErrorMessage(env, errTip));
        }
    }
}

napi_value NapiCallAbilityCallback::CreateUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiCallAbilityCallback::CreateErrorMessage(napi_env env, std::string msg)
{
    napi_value result = nullptr;
    napi_value message = nullptr;
    napi_create_string_utf8(env, (char *)msg.data(), msg.size(), &message);
    napi_create_error(env, nullptr, message, &result);
    return result;
}

void NapiCallAbilityCallback::SetPropertyInt32(napi_env env, napi_value object, std::string name, int32_t value)
{
    napi_value peopertyValue = nullptr;
    napi_create_int32(env, value, &peopertyValue);
    napi_set_named_property(env, object, name.c_str(), peopertyValue);
}

void NapiCallAbilityCallback::SetPropertyStringUtf8(
    napi_env env, napi_value object, std::string name, std::string value)
{
    napi_value peopertyValue = nullptr;
    char *valueChars = (char *)value.c_str();
    napi_create_string_utf8(env, valueChars, std::strlen(valueChars), &peopertyValue);
    napi_set_named_property(env, object, name.c_str(), peopertyValue);
}
} // namespace Telephony
} // namespace OHOS