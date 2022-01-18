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

#ifndef NATIVE_CALL_MANAGER_UTILS_H
#define NATIVE_CALL_MANAGER_UTILS_H

#include "string_ex.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "napi_call_manager_types.h"

namespace OHOS {
namespace Telephony {
/**
 * call_manager NAPI utility class.
 */
class NapiCallManagerUtils {
public:
    static bool MatchValueType(napi_env env, napi_value value, napi_valuetype targetType);
    static napi_value CreateUndefined(napi_env env);
    static napi_value CreateErrorMessage(napi_env env, std::string msg);
    static napi_value ToInt32Value(napi_env env, int32_t value);
    static napi_value GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName);
    static std::string GetStringProperty(napi_env env, napi_value object, const std::string &propertyName);
    static int32_t GetIntProperty(napi_env env, napi_value object, const std::string &propertyName);
    static bool GetBoolProperty(napi_env env, napi_value object, const std::string &propertyName);
    static void SetPropertyInt32(napi_env env, napi_value object, std::string name, int32_t value);
    static void SetPropertyStringUtf8(napi_env env, napi_value object, std::string name, std::string value);
    static void SetPropertyBoolean(napi_env env, napi_value object, std::string name, int32_t value);
};
} // namespace Telephony
} // namespace OHOS

#endif