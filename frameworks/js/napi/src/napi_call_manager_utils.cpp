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

#include "napi_call_manager_utils.h"

#include "napi_util.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr const char *PLACE_CALL = "ohos.permission.PLACE_CALL";
static constexpr const char *ANSWER_CALL = "ohos.permission.ANSWER_CALL";
static constexpr const char *SET_TELEPHONY_STATE = "ohos.permission.SET_TELEPHONY_STATE";
static constexpr const char *GET_TELEPHONY_STATE = "ohos.permission.GET_TELEPHONY_STATE";

static std::unordered_map<int32_t, const char *> eventNameMap_ = {
    { CALL_MANAGER_DIAL_CALL, "dial" },
    { CALL_MANAGER_ANSWER_CALL, "answer" },
    { CALL_MANAGER_REJECT_CALL, "reject" },
    { CALL_MANAGER_DISCONNECT_CALL, "hangup" },
    { CALL_MANAGER_HOLD_CALL, "holdCall" },
    { CALL_MANAGER_UNHOLD_CALL, "unHoldCall" },
    { CALL_MANAGER_SWITCH_CALL, "switchCall" },
    { CALL_MANAGER_GET_CALL_WAITING, "getCallWaitingStatus" },
    { CALL_MANAGER_SET_CALL_WAITING, "setCallWaiting" },
    { CALL_MANAGER_GET_CALL_RESTRICTION, "getCallRestrictionStatus" },
    { CALL_MANAGER_SET_CALL_RESTRICTION, "setCallRestriction" },
    { CALL_MANAGER_SET_CALL_RESTRICTION_PASSWORD, "setCallRestrictionPassword" },
    { CALL_MANAGER_GET_CALL_TRANSFER, "getCallTransferInfo" },
    { CALL_MANAGER_SET_CALL_TRANSFER, "setCallTransfer" },
    { CALL_MANAGER_CAN_SET_CALL_TRANSFER_TIME, "canSetCallTransferTime" },
    { CALL_MANAGER_ENABLE_IMS_SWITCH, "enableImsSwitch" },
    { CALL_MANAGER_DISABLE_IMS_SWITCH, "disableImsSwitch" },
    { CALL_MANAGER_IS_EMERGENCY_CALL, "isInEmergencyCall" },
    { CALL_MANAGER_IS_RINGING, "isRinging" },
    { CALL_MANAGER_MUTE_RINGER, "muteRinger" },
    { CALL_MANAGER_SET_VONR_STATE, "setVoNRState" },
    { CALL_MANAGER_GET_VONR_STATE, "getVoNRState" },
};

static std::unordered_map<int32_t, const char *> eventPermissionMap_ = {
    { CALL_MANAGER_DIAL_CALL, PLACE_CALL },
    { CALL_MANAGER_ANSWER_CALL, ANSWER_CALL },
    { CALL_MANAGER_REJECT_CALL, ANSWER_CALL },
    { CALL_MANAGER_DISCONNECT_CALL, ANSWER_CALL },
    { CALL_MANAGER_HOLD_CALL, ANSWER_CALL },
    { CALL_MANAGER_UNHOLD_CALL, ANSWER_CALL },
    { CALL_MANAGER_SWITCH_CALL, ANSWER_CALL },
    { CALL_MANAGER_GET_CALL_WAITING, GET_TELEPHONY_STATE },
    { CALL_MANAGER_SET_CALL_WAITING, SET_TELEPHONY_STATE },
    { CALL_MANAGER_GET_CALL_RESTRICTION, GET_TELEPHONY_STATE },
    { CALL_MANAGER_SET_CALL_RESTRICTION, SET_TELEPHONY_STATE },
    { CALL_MANAGER_SET_CALL_RESTRICTION_PASSWORD, SET_TELEPHONY_STATE },
    { CALL_MANAGER_GET_CALL_TRANSFER, GET_TELEPHONY_STATE },
    { CALL_MANAGER_SET_CALL_TRANSFER, SET_TELEPHONY_STATE },
    { CALL_MANAGER_CAN_SET_CALL_TRANSFER_TIME, GET_TELEPHONY_STATE },
    { CALL_MANAGER_ENABLE_IMS_SWITCH, SET_TELEPHONY_STATE },
    { CALL_MANAGER_DISABLE_IMS_SWITCH, SET_TELEPHONY_STATE },
    { CALL_MANAGER_IS_EMERGENCY_CALL, SET_TELEPHONY_STATE },
    { CALL_MANAGER_IS_RINGING, SET_TELEPHONY_STATE },
    { CALL_MANAGER_MUTE_RINGER, SET_TELEPHONY_STATE },
    { CALL_MANAGER_SET_VONR_STATE, SET_TELEPHONY_STATE },
    { CALL_MANAGER_GET_VONR_STATE, GET_TELEPHONY_STATE },
};

std::string NapiCallManagerUtils::GetEventName(int32_t eventId)
{
    std::string result = "";
    auto iter = eventNameMap_.find(eventId);
    if (iter == eventNameMap_.end()) {
        TELEPHONY_LOGE("NapiCallManagerUtils::GetEventName return null.");
        return result;
    }
    result = iter->second;
    TELEPHONY_LOGI("NapiCallManagerUtils::GetEventName message = %{public}s", result.c_str());
    return result;
}

std::string NapiCallManagerUtils::GetEventPermission(int32_t eventId)
{
    std::string result = "";
    auto iter = eventPermissionMap_.find(eventId);
    if (iter == eventPermissionMap_.end()) {
        TELEPHONY_LOGE("NapiCallManagerUtils::GetEventPermission return null.");
        return result;
    }
    result = iter->second;
    TELEPHONY_LOGI("NapiCallManagerUtils::GetEventPermission message = %{public}s", result.c_str());
    return result;
}

napi_value NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(napi_env env, int32_t errorCode, int32_t eventId)
{
    JsError error = {};
    switch (errorCode) {
        case TELEPHONY_ERR_PERMISSION_ERR: {
            std::string funcName = GetEventName(eventId);
            std::string permission = GetEventPermission(eventId);
            error = NapiUtil::ConverErrorMessageWithPermissionForJs(errorCode, funcName, permission);
            break;
        }
        default:
            error = NapiUtil::ConverErrorMessageForJs(errorCode);
            break;
    }
    return CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode);
}

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

napi_value NapiCallManagerUtils::CreateErrorMessageWithErrorCode(napi_env env, std::string msg, int32_t errorCode)
{
    napi_value message = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, msg.c_str(), msg.length(), &message));
    napi_value codeValue = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errorCode, &codeValue));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_set_named_property(env, result, "code", codeValue));
    NAPI_CALL(env, napi_set_named_property(env, result, "message", message));
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

std::string NapiCallManagerUtils::GetStringProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    napi_value value = nullptr;
    napi_status getNameStatus = napi_get_named_property(env, object, propertyName.c_str(), &value);
    if (getNameStatus == napi_ok) {
        char tmpStr[MESSAGE_CONTENT_MAXIMUM_LIMIT + 1] = { 0 };
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

bool NapiCallManagerUtils::GetUssdIntProperty(
    napi_env env, napi_value object, const std::string &propertyName, int32_t &result)
{
    napi_value value = nullptr;
    napi_status getNameStatus = napi_get_named_property(env, object, propertyName.c_str(), &value);
    if (getNameStatus == napi_ok) {
        napi_status getIntStatus = napi_get_value_int32(env, value, &result);
        if (getIntStatus == napi_ok) {
            return true;
        }
    }
    TELEPHONY_LOGW("GetUssdIntProperty failed!");
    return false;
}

bool NapiCallManagerUtils::GetUssdStringProperty(
    napi_env env, napi_value object, const std::string &propertyName, std::string &result)
{
    napi_value value = nullptr;
    napi_status getNameStatus = napi_get_named_property(env, object, propertyName.c_str(), &value);
    if (getNameStatus == napi_ok) {
        char tmpStr[MESSAGE_CONTENT_MAXIMUM_LIMIT + 1] = { 0 };
        size_t len = 0;
        napi_status getStringStatus =
            napi_get_value_string_utf8(env, value, tmpStr, MESSAGE_CONTENT_MAXIMUM_LIMIT, &len);
        if (getStringStatus == napi_ok && len > 0) {
            result = std::string(tmpStr, len);
            return true;
        }
    }
    TELEPHONY_LOGW("GetUssdStringProperty failed!");
    return false;
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

void NapiCallManagerUtils::SetPropertyInt32(napi_env env, napi_value object, std::string name, int32_t value)
{
    napi_value peopertyValue = nullptr;
    napi_create_int32(env, value, &peopertyValue);
    napi_set_named_property(env, object, name.c_str(), peopertyValue);
}

void NapiCallManagerUtils::SetPropertyStringUtf8(napi_env env, napi_value object, std::string name, std::string value)
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

napi_value NapiCallManagerUtils::CreateEnumConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    return thisArg;
}
} // namespace Telephony
} // namespace OHOS
