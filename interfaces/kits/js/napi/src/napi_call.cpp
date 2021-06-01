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

#include "napi_call.h"

#include <securec.h>
#include <codecvt>
#include <initializer_list>
#include <locale>
#include <memory>
#include <string>
#include <utility>

#include "string_ex.h"
#include "pac_map.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "call_manager.h"
#include "kit_call_manager_hilog_wrapper.h"

namespace OHOS {
namespace TelephonyNapi {
static std::unique_ptr<OHOS::TelephonyApi::CallManager> g_callManager;
static bool InitCallManager()
{
    if (g_callManager == nullptr) {
        g_callManager = std::make_unique<OHOS::TelephonyApi::CallManager>();
    }
    return g_callManager != nullptr;
}

static bool MatchValueType(napi_env env, napi_value value, napi_valuetype targetType)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    return valueType == targetType;
}

static bool MatchParamters(
    napi_env env, const napi_value parameters[], std::initializer_list<napi_valuetype> valueTypes)
{
    int i = INDEX_ZERO;
    for (auto beg = valueTypes.begin(); beg != valueTypes.end(); ++beg) {
        if (!MatchValueType(env, parameters[i], *beg)) {
            return false;
        }
        ++i;
    }
    return true;
}

static napi_value CreateErrorMessage(napi_env env, std::string msg)
{
    napi_value result = nullptr;
    napi_value message = nullptr;
    napi_create_string_utf8(env, (char *)msg.data(), msg.size(), &message);
    napi_create_error(env, nullptr, message, &result);
    return result;
}

static napi_value CreateUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

static napi_value GetPropertyValue(napi_env env, napi_value object, std::string propertyName)
{
    napi_value propertyValue = nullptr;
    bool hasProperty = false;
    napi_has_named_property(env, object, propertyName.data(), &hasProperty);
    if (hasProperty) {
        napi_value propertyNameValue = nullptr;
        napi_create_string_utf8(env, propertyName.data(), NAPI_AUTO_LENGTH, &propertyNameValue);
        napi_get_property(env, object, propertyNameValue, &propertyValue);
    }
    return propertyValue;
}

static void ExecNativeDial(napi_env env, AsyncContext &asyncContext)
{
    std::string phoneNumber(asyncContext.number, asyncContext.numberLen);
    bool isVideoCall = false;
    if (asyncContext.valueLen >= ONE && MatchValueType(env, asyncContext.value[INDEX_ZERO], napi_object)) {
        napi_value propertyNameValue = nullptr;
        napi_create_string_utf8(env, "extras", NAPI_AUTO_LENGTH, &propertyNameValue);
        bool hasExtrasProperty = false;
        napi_has_property(env, asyncContext.value[INDEX_ONE], propertyNameValue, &hasExtrasProperty);
        if (hasExtrasProperty) {
            napi_value extrasValue = nullptr;
            napi_get_property(env, asyncContext.value[INDEX_ONE], propertyNameValue, &extrasValue);
            if (MatchValueType(env, extrasValue, napi_boolean)) {
                napi_get_value_bool(env, extrasValue, &isVideoCall);
            }
        }
    }
    OHOS::AppExecFwk::PacMap dialInfo;
    dialInfo.PutBooleanValue("isVideoCall", isVideoCall);
    dialInfo.PutIntValue("accountId", ZERO);
    dialInfo.PutIntValue("videoState", ZERO);
    dialInfo.PutIntValue("dialScene", ZERO);
    int32_t dialResult = DEFAULT_ERROR;
    int32_t callId = MINUS_ONE;
    if (InitCallManager()) {
        dialResult = g_callManager->DialCall(OHOS::Str8ToStr16(phoneNumber), dialInfo, callId);
    }
    if (dialResult == ZERO) {
        asyncContext.status = RESOLVED;
        bool dialResultBool = dialResult == ZERO;
        napi_value booResultValue;
        napi_get_boolean(env, dialResultBool, &booResultValue);
        asyncContext.value[INDEX_ZERO] = booResultValue;
        asyncContext.valueLen = ONE;
    } else {
        napi_create_int32(env, dialResult, &asyncContext.value[INDEX_ZERO]);
        asyncContext.status = REJECT;
    }
}

static void ExecDialCallback(napi_env env, AsyncContext &asyncContext)
{
    if (asyncContext.deferred != nullptr) {
        if (asyncContext.status == RESOLVED) {
            napi_value promiseValue = asyncContext.value[INDEX_ZERO];
            napi_resolve_deferred(env, asyncContext.deferred, promiseValue);
        } else {
            napi_value undefined = CreateUndefined(env);
            napi_reject_deferred(env, asyncContext.deferred, undefined);
        }
    } else {
        napi_value callbackValue[INDEX_TWO] = {MEMORY_CLEAR};
        if (asyncContext.status == RESOLVED) {
            callbackValue[INDEX_ZERO] = CreateUndefined(env);
            callbackValue[INDEX_ONE] = asyncContext.value[INDEX_ZERO];
        } else {
            int32_t errCode = DEFAULT_ERROR;
            if (asyncContext.valueLen >= ONE) {
                napi_get_value_int32(env, asyncContext.value[INDEX_ZERO], &errCode);
            }
            std::string errTip = "dial error code :" + std::to_string(errCode);
            callbackValue[INDEX_ZERO] = CreateErrorMessage(env, errTip);
            callbackValue[INDEX_ONE] = CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_get_reference_value(env, asyncContext.callbackRef, &callback);
        napi_call_function(env, nullptr, callback, TWO, callbackValue, nullptr);
    }
    napi_delete_reference(env, asyncContext.callbackRef);
    delete &asyncContext;
}

static napi_value Dial(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    NAPI_ASSERT(env, argc == ONE_PARAMETER || argc == TWO_PARAMETER || argc == THREE_PARAMETER, "type mismatch");
    if (argc == ONE_PARAMETER) {
        NAPI_ASSERT(env, MatchParamters(env, argv, {napi_string}), "type mismatch");
    } else if (argc == TWO_PARAMETER) {
        NAPI_ASSERT(env,
            MatchParamters(env, argv, {napi_string, napi_function}) ||
                MatchParamters(env, argv, {napi_string, napi_object}),
            "type mismatch");
    } else if (argc == THREE_PARAMETER) {
        NAPI_ASSERT(env, MatchParamters(env, argv, {napi_string, napi_object, napi_function}), "type mismatch");
    }
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_string_utf8(env, argv[INDEX_ZERO], asyncContext->number, BUF_SIZE, &(asyncContext->numberLen));
    if (argc == TWO) {
        if (MatchValueType(env, argv[INDEX_ONE], napi_function)) {
            napi_create_reference(env, argv[INDEX_ONE], ONE, &(asyncContext->callbackRef));
        } else if (MatchValueType(env, argv[INDEX_ONE], napi_object)) {
            asyncContext->value[INDEX_ZERO] = argv[INDEX_ONE];
            asyncContext->valueLen = ONE;
        }
    } else if (argc == THREE) {
        asyncContext->value[INDEX_ZERO] = argv[INDEX_ONE];
        asyncContext->valueLen = ONE;
        napi_create_reference(env, argv[INDEX_TWO], TWO, &(asyncContext->callbackRef));
    }
    napi_value result = nullptr;
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &(asyncContext->deferred), &result);
    } else {
        napi_get_undefined(env, &result);
    }
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "Dial", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) -> void { ExecNativeDial(env, *(AsyncContext *)data); },
        [](napi_env env, napi_status status, void *data) -> void { ExecDialCallback(env, *(AsyncContext *)data); },
        (void *)(asyncContext), &(asyncContext->work));
    napi_queue_async_work(env, asyncContext->work);
    return result;
}

static void ExecNativeAnswer(napi_env env, AsyncContext &asyncContext)
{
    HILOG_DEBUG("Exec ExecNativeAnswer start");
    int32_t callId = asyncContext.callId;
    HILOG_DEBUG("Exec ExecNativeAnswer callId = %{public}d", callId);
    bool isVideoCall = false;
    if (asyncContext.valueLen >= TWO && MatchValueType(env, asyncContext.value[INDEX_ONE], napi_object)) {
        napi_value isVideoCallValue = GetPropertyValue(env, asyncContext.value[INDEX_ONE], "isVideoCall");
        if (isVideoCallValue != nullptr) {
            napi_get_value_bool(env, isVideoCallValue, &isVideoCall);
        }
    }
    int32_t answerResult = DEFAULT_ERROR;
    if (InitCallManager()) {
        answerResult = g_callManager->AcceptCall(callId, isVideoCall);
        HILOG_DEBUG("Exec ExecNativeAnswer answerResult = %{public}d", answerResult);
    }
    if (answerResult == ZERO) {
        asyncContext.status = RESOLVED;
        bool dialResultBool = answerResult == OHOS::ERR_NONE;
        napi_value booResultValue = nullptr;
        napi_get_boolean(env, dialResultBool, &booResultValue);
        asyncContext.value[INDEX_ZERO] = booResultValue;
        asyncContext.valueLen = ONE;
    } else {
        asyncContext.status = REJECT;
    }
    HILOG_DEBUG("Exec ExecNativeAnswer end");
}

static void ExecAnswerCallback(napi_env env, AsyncContext &asyncContext)
{
    HILOG_DEBUG("Exec ExecAnswerCallback start deferred = %{public}d, status = %{public}d",
        asyncContext.deferred != nullptr, asyncContext.status);
    if (asyncContext.deferred) {
        if (asyncContext.status == RESOLVED) {
            napi_value promiseValue = asyncContext.value[INDEX_ZERO];
            napi_resolve_deferred(env, asyncContext.deferred, promiseValue);
        } else {
            napi_value undefined = CreateUndefined(env);
            napi_reject_deferred(env, asyncContext.deferred, undefined);
        }
    } else {
        napi_value callbackValue[INDEX_TWO] = {MEMORY_CLEAR};
        if (asyncContext.status == RESOLVED) {
            callbackValue[INDEX_ZERO] = CreateUndefined(env);
            callbackValue[INDEX_ONE] = asyncContext.value[INDEX_ZERO];
        } else {
            callbackValue[INDEX_ZERO] = CreateErrorMessage(env, "answer failed");
            callbackValue[INDEX_ONE] = CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_get_reference_value(env, asyncContext.callbackRef, &callback);
        napi_call_function(env, nullptr, callback, TWO, callbackValue, nullptr);
    }
    napi_delete_reference(env, asyncContext.callbackRef);
    delete &asyncContext;
    HILOG_DEBUG("Exec ExecAnswerCallback end");
}

bool MatchAnswerParameters(napi_env env, napi_value parameter[], size_t parameterCount)
{
    switch (parameterCount) {
        case ONE_PARAMETER:
            return MatchParamters(env, parameter, {napi_number});
        case TWO_PARAMETER:
            return MatchParamters(env, parameter, {napi_number, napi_function}) ||
                MatchParamters(env, parameter, {napi_number, napi_object});
        case THREE_PARAMETER:
            return MatchParamters(env, parameter, {napi_number, napi_object, napi_function});
        default:
            return false;
    }
}

static napi_value Answer(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    NAPI_ASSERT(env, MatchAnswerParameters(env, argv, argc), "type mismatch");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[INDEX_ZERO], &asyncContext->callId);
    if (argc == TWO_PARAMETER) {
        if (MatchValueType(env, argv[INDEX_ONE], napi_function)) {
            napi_create_reference(env, argv[INDEX_ONE], ONE, &(asyncContext->callbackRef));
            asyncContext->valueLen = ONE;
        } else if (MatchValueType(env, argv[INDEX_ONE], napi_object)) {
            asyncContext->value[INDEX_ONE] = argv[INDEX_ONE];
            asyncContext->valueLen = TWO;
        }
    } else if (argc == THREE_PARAMETER) {
        asyncContext->value[INDEX_ONE] = argv[INDEX_ONE];
        asyncContext->valueLen = TWO;
        napi_create_reference(env, argv[INDEX_TWO], ZERO, &(asyncContext->callbackRef));
    }
    napi_value result = nullptr;
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &asyncContext->deferred, &result);
    } else {
        napi_get_undefined(env, &result);
    }
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "Answer", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) -> void {
            auto asyncContext = (AsyncContext *)data;
            ExecNativeAnswer(env, *asyncContext);
        },
        [](napi_env env, napi_status status, void *data) -> void {
            auto asyncContext = (AsyncContext *)data;
            ExecAnswerCallback(env, *asyncContext);
        },
        (void *)asyncContext, &(asyncContext->work));
    napi_queue_async_work(env, asyncContext->work);
    return result;
}

static void ExecNativeReject(napi_env env, AsyncContext &asyncContext)
{
    int32_t callId = asyncContext.callId;
    HILOG_DEBUG("Exec ExecNativeReject callId = %{public}d", callId);
    bool isSendSms = false;
    std::string rejectMessage = "";
    if (asyncContext.valueLen >= TWO && MatchValueType(env, asyncContext.value[INDEX_ONE], napi_object)) {
        napi_value rejectWithMessgeValue =
            GetPropertyValue(env, asyncContext.value[INDEX_ONE], "rejectWithMessgeValue");
        napi_value rejectMessageValue = GetPropertyValue(env, asyncContext.value[INDEX_ONE], "rejectMessageValue");
        if ((rejectWithMessgeValue != nullptr) && (rejectMessageValue != nullptr)) {
            napi_get_value_bool(env, rejectWithMessgeValue, &isSendSms);
            size_t messageSize;
            char *rejectMessageChars = nullptr;
            napi_get_value_string_utf8(env, rejectMessageValue, rejectMessageChars, NAPI_AUTO_LENGTH, &messageSize);
            rejectMessage = std::string(rejectMessageChars, messageSize);
        }
    }
    int32_t rejectResult = DEFAULT_ERROR;
    if (InitCallManager()) {
        rejectResult = g_callManager->RejectCall(callId, isSendSms, Str8ToStr16(rejectMessage));
        HILOG_DEBUG("Exec ExecNativeReject rejectResult = %{public}d", rejectResult);
    }
    if (rejectResult == ZERO) {
        asyncContext.status = RESOLVED;
        bool dialResultBool = rejectResult == OHOS::ERR_NONE;
        napi_get_boolean(env, dialResultBool, &asyncContext.value[INDEX_ZERO]);
        asyncContext.valueLen = ONE;
    } else {
        asyncContext.status = REJECT;
    }
}

static void ExecRejectCallback(napi_env env, napi_status status, AsyncContext &asyncContext)
{
    if (asyncContext.deferred) {
        if (asyncContext.status == RESOLVED) {
            napi_value promiseValue = asyncContext.value[INDEX_ZERO];
            napi_resolve_deferred(env, asyncContext.deferred, promiseValue);
        } else {
            napi_value undefined = CreateUndefined(env);
            napi_reject_deferred(env, asyncContext.deferred, undefined);
        }
    } else {
        napi_value callbackValue[INDEX_TWO] = {MEMORY_CLEAR};
        if (asyncContext.status == RESOLVED) {
            callbackValue[INDEX_ZERO] = CreateUndefined(env);
            callbackValue[INDEX_ONE] = asyncContext.value[INDEX_ZERO];
        } else {
            callbackValue[INDEX_ZERO] = CreateErrorMessage(env, "reject failed");
            callbackValue[INDEX_ONE] = CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_get_reference_value(env, asyncContext.callbackRef, &callback);
        napi_call_function(env, nullptr, callback, TWO, callbackValue, nullptr);
    }
    napi_delete_reference(env, asyncContext.callbackRef);
    delete &asyncContext;
}

static bool MatchRejectParameter(napi_env env, size_t parameterCount, napi_value parameters[])
{
    switch (parameterCount) {
        case ONE_PARAMETER:
            return MatchParamters(env, parameters, {napi_number});
        case TWO_PARAMETER:
            return MatchParamters(env, parameters, {napi_number, napi_function}) ||
                MatchParamters(env, parameters, {napi_number, napi_object});
        case THREE_PARAMETER:
            return MatchParamters(env, parameters, {napi_number, napi_object, napi_function});
        default:
            return false;
    }
}

static napi_value Reject(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    NAPI_ASSERT(env, MatchRejectParameter(env, argc, argv), "type mismatch");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[INDEX_ZERO], &asyncContext->callId);
    if (argc == TWO_PARAMETER) {
        if (MatchValueType(env, argv[INDEX_ONE], napi_function)) {
            napi_create_reference(env, argv[INDEX_ONE], ONE, &(asyncContext->callbackRef));
            asyncContext->valueLen = ONE;
        } else if (MatchValueType(env, argv[INDEX_ONE], napi_object)) {
            asyncContext->value[INDEX_ONE] = argv[INDEX_ONE];
            asyncContext->valueLen = TWO;
        }
    } else if (argc == THREE_PARAMETER) {
        asyncContext->value[INDEX_ONE] = argv[INDEX_ONE];
        asyncContext->valueLen = TWO;
        napi_create_reference(env, argv[INDEX_TWO], ONE, &(asyncContext->callbackRef));
    }
    napi_value result = nullptr;
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &asyncContext->deferred, &result);
    } else {
        napi_get_undefined(env, &result);
    }
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "Answer", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) -> void {
            auto asyncContext = (AsyncContext *)data;
            ExecNativeReject(env, *asyncContext);
        },
        [](napi_env env, napi_status status, void *data) -> void {
            auto asyncContext = (AsyncContext *)data;
            ExecRejectCallback(env, status, *asyncContext);
        },
        (void *)asyncContext, &(asyncContext->work));
    napi_queue_async_work(env, asyncContext->work);
    return result;
}

static void ExecNativeHangup(napi_env env, AsyncContext &asyncContext)
{
    int32_t hangUpResult = DEFAULT_ERROR;
    if (InitCallManager()) {
        int32_t callId = asyncContext.callId;
        HILOG_DEBUG("Exec ExecNativeHangup callId = %{public}d", callId);
        hangUpResult = g_callManager->HangUpCall(callId);
        HILOG_DEBUG("Exec ExecNativeHangup hangUpResult = %{public}d", hangUpResult);
    }
    if (hangUpResult == ZERO) {
        asyncContext.status = RESOLVED;
        bool hangUpResultBool = hangUpResult == OHOS::ERR_NONE;
        napi_get_boolean(env, hangUpResultBool, &asyncContext.value[INDEX_ZERO]);
        asyncContext.valueLen = ONE;
    } else {
        asyncContext.status = REJECT;
    }
}

static void ExecHangupCallback(napi_env env, napi_status status, AsyncContext &asyncContext)
{
    HILOG_DEBUG("Exec ExecHangupCallback asyncContext.deferred = %{public}d  status = %{public}d",
        asyncContext.deferred != nullptr, asyncContext.status);
    if (asyncContext.deferred) {
        if (asyncContext.status == RESOLVED) {
            napi_value promiseValue = asyncContext.value[INDEX_ZERO];
            napi_resolve_deferred(env, asyncContext.deferred, promiseValue);
        } else {
            napi_value undefined = CreateUndefined(env);
            napi_reject_deferred(env, asyncContext.deferred, undefined);
        }
    } else {
        napi_value callbackValue[INDEX_TWO] = {MEMORY_CLEAR};
        if (asyncContext.status == RESOLVED) {
            callbackValue[INDEX_ZERO] = CreateUndefined(env);
            callbackValue[INDEX_ONE] = asyncContext.value[INDEX_ZERO];
        } else {
            callbackValue[INDEX_ZERO] = CreateErrorMessage(env, "hangUp failed");
            callbackValue[INDEX_ONE] = CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_get_reference_value(env, asyncContext.callbackRef, &callback);
        napi_call_function(env, nullptr, callback, TWO, callbackValue, nullptr);
        HILOG_DEBUG("Exec ExecHangupCallback after napi_call_function");
    }
    napi_delete_reference(env, asyncContext.callbackRef);
    HILOG_DEBUG("Exec ExecHangupCallback end");
    delete &asyncContext;
}

static bool MatchHangupParameter(napi_env env, size_t parameterCount, napi_value parameters[])
{
    switch (parameterCount) {
        case ONE_PARAMETER:
            return MatchParamters(env, parameters, {napi_number});
        case TWO_PARAMETER:
            return MatchParamters(env, parameters, {napi_number, napi_function}) ||
                MatchParamters(env, parameters, {napi_number, napi_object});
        default:
            return false;
    }
}

static napi_value Hangup(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    NAPI_ASSERT(env, MatchHangupParameter(env, argc, argv), "type mismatch");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[INDEX_ZERO], &asyncContext->callId);
    if (argc == TWO_PARAMETER && MatchValueType(env, argv[INDEX_ONE], napi_function)) {
        napi_create_reference(env, argv[INDEX_ONE], ONE, &(asyncContext->callbackRef));
    }
    napi_value result = nullptr;
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &asyncContext->deferred, &result);
    } else {
        napi_get_undefined(env, &result);
    }
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "Hangup", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) -> void {
            auto asyncContext = (AsyncContext *)data;
            if (asyncContext) {
                ExecNativeHangup(env, *asyncContext);
            }
        },
        [](napi_env env, napi_status status, void *data) -> void {
            auto asyncContext = (AsyncContext *)data;
            if (asyncContext) {
                ExecHangupCallback(env, status, *asyncContext);
            }
        },
        (void *)asyncContext, &(asyncContext->work));
    napi_queue_async_work(env, asyncContext->work);
    return result;
}

static void ExecNativeGetCallState(napi_env env, AsyncContext &asyncContext)
{
    int32_t callState = DEFAULT_ERROR;
    if (InitCallManager()) {
        callState = g_callManager->GetCallState();
    }
    if (callState == DEFAULT_ERROR) {
        asyncContext.status = REJECT;
    } else {
        asyncContext.status = RESOLVED;
        napi_create_int32(env, callState, &asyncContext.value[INDEX_ZERO]);
        asyncContext.valueLen = ONE;
    }
}

static void ExecGetCallStateCallback(napi_env env, napi_status status, AsyncContext &asyncContext)
{
    if (asyncContext.deferred) {
        if (asyncContext.status == RESOLVED) {
            napi_resolve_deferred(env, asyncContext.deferred, asyncContext.value[INDEX_ZERO]);
        } else {
            napi_value undefined = CreateUndefined(env);
            napi_reject_deferred(env, asyncContext.deferred, undefined);
        }
    } else {
        napi_value callBackValue[INDEX_TWO] = {MEMORY_CLEAR};
        if (asyncContext.status == RESOLVED) {
            callBackValue[INDEX_ZERO] = CreateUndefined(env);
            callBackValue[INDEX_ONE] = asyncContext.value[INDEX_ZERO];
        } else {
            callBackValue[INDEX_ZERO] = CreateErrorMessage(env, "get sim state failed");
            callBackValue[INDEX_ONE] = CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_get_reference_value(env, asyncContext.callbackRef, &callback);
        napi_call_function(env, nullptr, callback, TWO, callBackValue, nullptr);
    }
    napi_delete_reference(env, asyncContext.callbackRef);
    delete &asyncContext;
}

static napi_value GetCallState(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    switch (argc) {
        case NONE_PARAMETER:
            break;
        case ONE_PARAMETER:
            NAPI_ASSERT(env, MatchParamters(env, argv, {napi_function}), "type mismatch");
            break;
        default:
            NAPI_ASSERT(env, false, "type mismatch");
            break;
    }
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    if (argc == ONE_PARAMETER) {
        napi_create_reference(env, argv[INDEX_ZERO], ONE, &(asyncContext->callbackRef));
    }
    napi_value result = nullptr;
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &asyncContext->deferred, &result);
    } else {
        napi_get_undefined(env, &result);
    }
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "GetCallState", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) -> void {
            auto asyncContext = (AsyncContext *)data;
            ExecNativeGetCallState(env, *asyncContext);
        },
        [](napi_env env, napi_status status, void *data) -> void {
            auto asyncContext = (AsyncContext *)data;
            ExecGetCallStateCallback(env, status, *asyncContext);
        },
        (void *)asyncContext, &(asyncContext->work));
    napi_queue_async_work(env, asyncContext->work);
    return result;
}

EXTERN_C_START
napi_value RegistCallManager(napi_env env, napi_value exports)
{
    napi_value callStateUnknown = nullptr;
    napi_value callStateIdle = nullptr;
    napi_value callStateRinging = nullptr;
    napi_value callStateOffHook = nullptr;
    napi_create_int32(env, static_cast<int32_t>(CALL_STATE_UNKNOWN), &callStateUnknown);
    napi_create_int32(env, static_cast<int32_t>(CALL_STATE_IDLE), &callStateIdle);
    napi_create_int32(env, static_cast<int32_t>(CALL_STATE_RINGING), &callStateRinging);
    napi_create_int32(env, static_cast<int32_t>(CALL_STATE_OFFHOOK), &callStateOffHook);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("dial", Dial),
        DECLARE_NAPI_FUNCTION("answer", Answer),
        DECLARE_NAPI_FUNCTION("reject", Reject),
        DECLARE_NAPI_FUNCTION("hangup", Hangup),
        DECLARE_NAPI_FUNCTION("getCallState", GetCallState),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_UNKNOWN", callStateUnknown),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_IDLE", callStateIdle),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_RINGING", callStateRinging),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_OFFHOOK", callStateOffHook),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[INDEX_ZERO]), desc));
    return exports;
}
EXTERN_C_END

static napi_module g_callManagerModule = {
    .nm_version = ONE,
    .nm_flags = ZERO,
    .nm_filename = nullptr,
    .nm_register_func = RegistCallManager,
    .nm_modname = "libtelephony_call.z.so",
    .nm_priv = ((void *)ZERO),
    .reserved = {
        MEMORY_CLEAR
    }
};

extern "C" __attribute__((constructor)) void RegisterRadioNetworkModule(void)
{
    napi_module_register(&g_callManagerModule);
}
} // namespace TelephonyNapi
} // namespace OHOS
