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

#ifndef NATIVE_CALL_MANAGER_UTILS_H
#define NATIVE_CALL_MANAGER_UTILS_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_call_manager_types.h"
#include "string_ex.h"
#include "telephony_napi_common_error.h"

namespace OHOS {
namespace Telephony {
enum {
    CALL_MANAGER_UNKNOWN_EVENT = 0,
    CALL_MANAGER_DIAL_CALL,
    CALL_MANAGER_ANSWER_CALL,
    CALL_MANAGER_REJECT_CALL,
    CALL_MANAGER_DISCONNECT_CALL,
    CALL_MANAGER_HOLD_CALL,
    CALL_MANAGER_UNHOLD_CALL,
    CALL_MANAGER_SWITCH_CALL,
    CALL_MANAGER_GET_CALL_WAITING,
    CALL_MANAGER_SET_CALL_WAITING,
    CALL_MANAGER_GET_CALL_RESTRICTION,
    CALL_MANAGER_SET_CALL_RESTRICTION,
    CALL_MANAGER_GET_CALL_TRANSFER,
    CALL_MANAGER_SET_CALL_TRANSFER,
    CALL_MANAGER_CAN_SET_CALL_TRANSFER_TIME,
    CALL_MANAGER_ENABLE_IMS_SWITCH,
    CALL_MANAGER_DISABLE_IMS_SWITCH,
    CALL_MANAGER_IS_EMERGENCY_CALL,
    CALL_MANAGER_IS_RINGING,
    CALL_MANAGER_MUTE_RINGER,
    CALL_MANAGER_CLOSE_UNFINISHED_USSD,
};

/**
 * call_manager NAPI utility class.
 */
class NapiCallManagerUtils {
public:
    static bool MatchValueType(napi_env env, napi_value value, napi_valuetype targetType);
    static napi_value CreateUndefined(napi_env env);
    static napi_value CreateErrorMessage(napi_env env, std::string msg);
    static napi_value CreateErrorMessageWithErrorCode(
        napi_env env, std::string message, int32_t errorCode = ERROR_DEFAULT);
    static napi_value ToInt32Value(napi_env env, int32_t value);
    static napi_value GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName);
    static std::string GetStringProperty(napi_env env, napi_value object, const std::string &propertyName);
    static int32_t GetIntProperty(napi_env env, napi_value object, const std::string &propertyName);
    static bool GetUssdIntProperty(napi_env env, napi_value object, const std::string &propertyName, int32_t &result);
    static bool GetUssdStringProperty(
        napi_env env, napi_value object, const std::string &propertyName, std::string &result);
    static bool GetBoolProperty(napi_env env, napi_value object, const std::string &propertyName);
    static void SetPropertyInt32(napi_env env, napi_value object, std::string name, int32_t value);
    static void SetPropertyStringUtf8(napi_env env, napi_value object, std::string name, std::string value);
    static void SetPropertyBoolean(napi_env env, napi_value object, std::string name, int32_t value);
    static napi_value CreateEnumConstructor(napi_env env, napi_callback_info info);
    static napi_value CreateErrorCodeAndMessageForJs(napi_env env, int32_t errorCode, int32_t eventId);

private:
    static std::string GetEventName(int32_t eventId);
    static std::string GetEventPermission(int32_t eventId);
};
} // namespace Telephony
} // namespace OHOS

#endif