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

#include "napi_call_manager_utils.h"

#include "system_ability_definition.h"
#include "ability.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
bool NapiCallManagerUtils::MatchValueType(napi_env env, napi_value value, napi_valuetype targetType)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    return valueType == targetType;
}

napi_value NapiCallManagerUtils::CreateUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiCallManagerUtils::CreateErrorMessage(napi_env env, std::string msg)
{
    napi_value result = nullptr;
    napi_value message = nullptr;
    napi_create_string_utf8(env, msg.c_str(), msg.length(), &message);
    napi_create_error(env, nullptr, message, &result);
    return result;
}

napi_value NapiCallManagerUtils::ToInt32Value(napi_env env, int32_t value)
{
    napi_value staticValue = nullptr;
    napi_create_int32(env, value, &staticValue);
    return staticValue;
}

napi_value NapiCallManagerUtils::GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    napi_value value = nullptr;
    napi_get_named_property(env, object, propertyName.c_str(), &value);
    return value;
}

std::string NapiCallManagerUtils::GetStringProperty(
    napi_env env, napi_value object, const std::string &propertyName)
{
    napi_value value = nullptr;
    napi_status getNameStatus = napi_get_named_property(env, object, propertyName.c_str(), &value);
    if (getNameStatus == napi_ok) {
        char tmpStr[MESSAGE_CONTENT_MAXIMUM_LIMIT + 1] = {0};
        size_t len = 0;
        napi_status getStringStatus =
            napi_get_value_string_utf8(env, value, tmpStr, MESSAGE_CONTENT_MAXIMUM_LIMIT, &len);
        if (getStringStatus == napi_ok && len > 0) {
            return std::string(tmpStr, len);
        }
    }
    TELEPHONY_LOGW("GetStringProperty failed!");
    return "";
}

int32_t NapiCallManagerUtils::GetIntProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    int32_t intValue = 0;
    napi_value value = nullptr;
    napi_status getNameStatus = napi_get_named_property(env, object, propertyName.c_str(), &value);
    if (getNameStatus == napi_ok) {
        napi_status getIntStatus = napi_get_value_int32(env, value, &intValue);
        if (getIntStatus == napi_ok) {
            return intValue;
        }
    }
    TELEPHONY_LOGW("GetIntProperty failed!");
    return intValue;
}

bool NapiCallManagerUtils::GetBoolProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    bool boolValue = false;
    napi_value value = nullptr;
    napi_status getNameStatus = napi_get_named_property(env, object, propertyName.c_str(), &value);
    if (getNameStatus == napi_ok) {
        napi_status getIntStatus = napi_get_value_bool(env, value, &boolValue);
        if (getIntStatus == napi_ok) {
            return boolValue;
        }
    }
    TELEPHONY_LOGW("GetBoolProperty failed!");
    return boolValue;
}

std::u16string NapiCallManagerUtils::GetBundleName(napi_env env)
{
    // get global value
    napi_value global = nullptr;
    std::string bundleName = "";
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok || global == nullptr) {
        TELEPHONY_LOGE("can't get global instance for %{public}d", status);
        return u"";
    }
    // get ability
    napi_value abilityObj = nullptr;
    status = napi_get_named_property(env, global, "ability", &abilityObj);
    if (status != napi_ok || abilityObj == nullptr) {
        TELEPHONY_LOGE("can't get ability obj for %{public}d", status);
        return u"";
    }
    // get ability pointer
    OHOS::AppExecFwk::Ability *ability = nullptr;
    status = napi_get_value_external(env, abilityObj, (void **)&ability);
    if (status != napi_ok || ability == nullptr) {
        TELEPHONY_LOGE("get ability from property failed for %{public}d", status);
        return u"";
    }
    // get bundle path
    bundleName = ability->GetBundleName();
    TELEPHONY_LOGI("getBundleName = %{public}s", bundleName.c_str());
    return Str8ToStr16(bundleName);
}

void NapiCallManagerUtils::SetPropertyInt32(napi_env env, napi_value object, std::string name, int32_t value)
{
    napi_value peopertyValue = nullptr;
    napi_create_int32(env, value, &peopertyValue);
    napi_set_named_property(env, object, name.c_str(), peopertyValue);
}

void NapiCallManagerUtils::SetPropertyStringUtf8(
    napi_env env, napi_value object, std::string name, std::string value)
{
    napi_value peopertyValue = nullptr;
    napi_create_string_utf8(env, value.c_str(), value.length(), &peopertyValue);
    napi_set_named_property(env, object, name.c_str(), peopertyValue);
}

void NapiCallManagerUtils::SetPropertyBoolean(napi_env env, napi_value object, std::string name, int32_t value)
{
    napi_value peopertyValue = nullptr;
    napi_get_boolean(env, value, &peopertyValue);
    napi_set_named_property(env, object, name.c_str(), peopertyValue);
}
} // namespace Telephony
} // namespace OHOS