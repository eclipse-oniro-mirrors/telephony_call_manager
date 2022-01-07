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

#include "napi_call_manager.h"

#include <string_ex.h>
#include <securec.h>

#include "system_ability_definition.h"
#include "ability.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "napi_call_manager_types.h"
#include "napi_call_ability_callback.h"
#include "call_manager_client.h"

namespace OHOS {
namespace Telephony {
bool NapiCallManager::registerStatus_ = false;
NapiCallManager::NapiCallManager() {}

void Init(std::u16string &bundleName)
{
    DelayedSingleton<CallManagerClient>::GetInstance()->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID, bundleName);
}

napi_value NapiCallManager::DeclareCallBasisInterface(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("dial", DialCall),
        DECLARE_NAPI_FUNCTION("answer", AnswerCall),
        DECLARE_NAPI_FUNCTION("reject", RejectCall),
        DECLARE_NAPI_FUNCTION("hangup", HangUpCall),
        DECLARE_NAPI_FUNCTION("holdCall", HoldCall),
        DECLARE_NAPI_FUNCTION("unHoldCall", UnHoldCall),
        DECLARE_NAPI_FUNCTION("switchCall", SwitchCall),
        DECLARE_NAPI_FUNCTION("setCallPreferenceMode", SetCallPreferenceMode),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallConferenceInterface(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("combineConference", CombineConference),
        DECLARE_NAPI_FUNCTION("separateConference", SeparateConference),
        DECLARE_NAPI_FUNCTION("getMainCallId", GetMainCallId),
        DECLARE_NAPI_FUNCTION("getSubCallIdList", GetSubCallIdList),
        DECLARE_NAPI_FUNCTION("getCallIdListForConference", GetCallIdListForConference),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallSupplementInterface(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getCallWaitingStatus", GetCallWaiting),
        DECLARE_NAPI_FUNCTION("setCallWaiting", SetCallWaiting),
        DECLARE_NAPI_FUNCTION("getCallRestrictionStatus", GetCallRestriction),
        DECLARE_NAPI_FUNCTION("setCallRestriction", SetCallRestriction),
        DECLARE_NAPI_FUNCTION("getCallTransferInfo", GetCallTransferInfo),
        DECLARE_NAPI_FUNCTION("setCallTransfer", SetCallTransferInfo),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallExtendInterface(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("startDTMF", StartDTMF),
        DECLARE_NAPI_FUNCTION("stopDTMF", StopDTMF),
        DECLARE_NAPI_FUNCTION("getCallState", GetCallState),
        DECLARE_NAPI_FUNCTION("isRinging", IsRinging),
        DECLARE_NAPI_FUNCTION("hasCall", HasCall),
        DECLARE_NAPI_FUNCTION("isNewCallAllowed", IsNewCallAllowed),
        DECLARE_NAPI_FUNCTION("isInEmergencyCall", IsInEmergencyCall),
        DECLARE_NAPI_FUNCTION("isEmergencyPhoneNumber", IsEmergencyPhoneNumber),
        DECLARE_NAPI_FUNCTION("formatPhoneNumber", FormatPhoneNumber),
        DECLARE_NAPI_FUNCTION("formatPhoneNumberToE164", FormatPhoneNumberToE164),
        DECLARE_NAPI_FUNCTION("on", ObserverOn),
        DECLARE_NAPI_FUNCTION("off", ObserverOff),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallMultimediaInterface(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("setMuted", SetMuted),
        DECLARE_NAPI_FUNCTION("cancelMuted", CancelMuted),
        DECLARE_NAPI_FUNCTION("muteRinger", MuteRinger),
        DECLARE_NAPI_FUNCTION("setAudioDevice", SetAudioDevice),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallMediaEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // AudioDevice
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_EARPIECE", ToInt32Value(env, DEVICE_EARPIECE)),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_SPEAKER", ToInt32Value(env, DEVICE_SPEAKER)),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_WIRED_HEADSET", ToInt32Value(env, DEVICE_WIRED_HEADSET)),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_BLUETOOTH_SCO", ToInt32Value(env, DEVICE_BLUETOOTH_SCO)),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_DISABLE", ToInt32Value(env, DEVICE_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_UNKNOWN", ToInt32Value(env, DEVICE_UNKNOWN)),
        // VideoStateType
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VOICE", ToInt32Value(env, TYPE_VOICE)),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VIDEO", ToInt32Value(env, TYPE_VIDEO)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallDialEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // DialScene
        DECLARE_NAPI_STATIC_PROPERTY("CALL_NORMAL", ToInt32Value(env, CALL_NORMAL)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_PRIVILEGED", ToInt32Value(env, CALL_PRIVILEGED)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_EMERGENCY", ToInt32Value(env, CALL_EMERGENCY)),
        // CallType
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_CS", ToInt32Value(env, TYPE_CS)),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_IMS", ToInt32Value(env, TYPE_IMS)),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_OTT", ToInt32Value(env, TYPE_OTT)),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_ERR_CALL", ToInt32Value(env, TYPE_ERR_CALL)),
        // DialType
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_CARRIER_TYPE", ToInt32Value(env, (int32_t)DialType::DIAL_CARRIER_TYPE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "DIAL_VOICE_MAIL_TYPE", ToInt32Value(env, (int32_t)DialType::DIAL_VOICE_MAIL_TYPE)),
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_OTT_TYPE", ToInt32Value(env, (int32_t)DialType::DIAL_OTT_TYPE)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallStateEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // TelCallState
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_ACTIVE", ToInt32Value(env, CALL_STATUS_ACTIVE)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_HOLDING", ToInt32Value(env, CALL_STATUS_HOLDING)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DIALING", ToInt32Value(env, CALL_STATUS_DIALING)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_ALERTING", ToInt32Value(env, CALL_STATUS_ALERTING)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_INCOMING", ToInt32Value(env, CALL_STATUS_INCOMING)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_WAITING", ToInt32Value(env, CALL_STATUS_WAITING)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DISCONNECTED", ToInt32Value(env, CALL_STATUS_DISCONNECTED)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DISCONNECTING", ToInt32Value(env, CALL_STATUS_DISCONNECTING)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_IDLE", ToInt32Value(env, CALL_STATUS_IDLE)),
        // TelConferenceState
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_IDLE", ToInt32Value(env, TEL_CONFERENCE_IDLE)),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_ACTIVE", ToInt32Value(env, TEL_CONFERENCE_ACTIVE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TEL_CONFERENCE_DISCONNECTING", ToInt32Value(env, TEL_CONFERENCE_DISCONNECTING)),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_DISCONNECTED", ToInt32Value(env, TEL_CONFERENCE_DISCONNECTED)),
        // CallStateToApp
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_STATE_UNKNOWN", ToInt32Value(env, (int32_t)CallStateToApp::CALL_STATE_UNKNOWN)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_IDLE", ToInt32Value(env, (int32_t)CallStateToApp::CALL_STATE_IDLE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_STATE_RINGING", ToInt32Value(env, (int32_t)CallStateToApp::CALL_STATE_RINGING)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_STATE_OFFHOOK", ToInt32Value(env, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallEventEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // CallAbilityEventId
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_UNKNOW_ID", ToInt32Value(env, EVENT_UNKNOW_ID)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "EVENT_DIAL_AT_CMD_SEND_FAILED", ToInt32Value(env, EVENT_DIAL_AT_CMD_SEND_FAILED)),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_DIAL_NO_CARRIER", ToInt32Value(env, EVENT_DIAL_NO_CARRIER)),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_START_DTMF_SUCCESS", ToInt32Value(env, EVENT_START_DTMF_SUCCESS)),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_START_DTMF_FAILED", ToInt32Value(env, EVENT_START_DTMF_FAILED)),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_STOP_DTMF_SUCCESS", ToInt32Value(env, EVENT_STOP_DTMF_SUCCESS)),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_STOP_DTMF_FAILED", ToInt32Value(env, EVENT_STOP_DTMF_FAILED)),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_SEND_DTMF_SUCCESS", ToInt32Value(env, EVENT_SEND_DTMF_SUCCESS)),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_SEND_DTMF_FAILED", ToInt32Value(env, EVENT_SEND_DTMF_FAILED)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "EVENT_SEND_DTMF_STRING_SUCCESS", ToInt32Value(env, EVENT_SEND_DTMF_STRING_SUCCESS)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "EVENT_SEND_DTMF_STRING_FAILED", ToInt32Value(env, EVENT_SEND_DTMF_STRING_FAILED)),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_DISCONNECTED_UNKNOW", ToInt32Value(env, EVENT_DISCONNECTED_UNKNOW)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallRestrictionEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // CallRestrictionType
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_TYPE_ALL_OUTGOING", ToInt32Value(env, RESTRICTION_TYPE_ALL_OUTGOING)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_TYPE_INTERNATIONAL", ToInt32Value(env, RESTRICTION_TYPE_INTERNATIONAL)),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME",
            ToInt32Value(env, RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_TYPE_ALL_INCOMING", ToInt32Value(env, RESTRICTION_TYPE_ALL_INCOMING)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_TYPE_ROAMING_INCOMING", ToInt32Value(env, RESTRICTION_TYPE_ROAMING_INCOMING)),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ALL_CALLS", ToInt32Value(env, RESTRICTION_TYPE_ALL_CALLS)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_TYPE_OUTGOING_SERVICES", ToInt32Value(env, RESTRICTION_TYPE_OUTGOING_SERVICES)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_TYPE_INCOMING_SERVICES", ToInt32Value(env, RESTRICTION_TYPE_INCOMING_SERVICES)),
        // CallRestrictionMode
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_MODE_DEACTIVATION", ToInt32Value(env, RESTRICTION_MODE_DEACTIVATION)),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_MODE_ACTIVATION", ToInt32Value(env, RESTRICTION_MODE_ACTIVATION)),
        // RestrictionStatus
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_DISABLE", ToInt32Value(env, RESTRICTION_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_ENABLE", ToInt32Value(env, RESTRICTION_ENABLE)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallWaitingEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // CallWaitingStatus
        DECLARE_NAPI_STATIC_PROPERTY("CALL_WAITING_DISABLE", ToInt32Value(env, CALL_WAITING_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_WAITING_ENABLE", ToInt32Value(env, CALL_WAITING_ENABLE)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallTransferEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // TransferStatus
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_DISABLE", ToInt32Value(env, TRANSFER_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_ENABLE", ToInt32Value(env, TRANSFER_ENABLE)),
        // CallTransferType
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_UNCONDITIONAL", ToInt32Value(env, TRANSFER_TYPE_UNCONDITIONAL)),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_BUSY", ToInt32Value(env, TRANSFER_TYPE_BUSY)),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_NO_REPLY", ToInt32Value(env, TRANSFER_TYPE_NO_REPLY)),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_NOT_REACHABLE", ToInt32Value(env, TRANSFER_TYPE_NOT_REACHABLE)),
        // CallTransferSettingType
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_ENABLE", ToInt32Value(env, CALL_TRANSFER_ENABLE)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_DISABLE", ToInt32Value(env, CALL_TRANSFER_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_REGISTRATION", ToInt32Value(env, CALL_TRANSFER_REGISTRATION)),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_ERASURE", ToInt32Value(env, CALL_TRANSFER_ERASURE)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::RegisterCallManagerFunc(napi_env env, napi_value exports)
{
    DeclareCallBasisInterface(env, exports);
    DeclareCallConferenceInterface(env, exports);
    DeclareCallSupplementInterface(env, exports);
    DeclareCallExtendInterface(env, exports);
    DeclareCallMultimediaInterface(env, exports);
    DeclareCallMediaEnum(env, exports);
    DeclareCallDialEnum(env, exports);
    DeclareCallStateEnum(env, exports);
    DeclareCallEventEnum(env, exports);
    DeclareCallRestrictionEnum(env, exports);
    DeclareCallWaitingEnum(env, exports);
    DeclareCallTransferEnum(env, exports);
    std::u16string bundleName = GetBundleName(env);
    Init(bundleName);
    return exports;
}

napi_value NapiCallManager::DialCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<DialAsyncContext>()).release();
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            GetDialInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT && MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        GetDialInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "DialCall", NativeDialCall, NativeDialCallBack);
}

napi_value NapiCallManager::AnswerCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AnswerAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number)) {
            asyncContext->videoState = GetIntProperty(env, argv[ARRAY_INDEX_SECOND], "videoState");
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT) {
        asyncContext->videoState = GetIntProperty(env, argv[ARRAY_INDEX_SECOND], "videoState");
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "AnswerCall", NativeAnswerCall, NativeVoidCallBack);
}

napi_value NapiCallManager::RejectCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, FOUR_VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < FOUR_VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<RejectAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    asyncContext->isSendSms = false;
    if (argc == TWO_VALUE_LIMIT) {
        if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            GetSmsInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT) {
        if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            GetSmsInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        }
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "RejectCall", NativeRejectCall, NativeVoidCallBack);
}

napi_value NapiCallManager::HangUpCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "HangUpCall", NativeHangUpCall, NativeVoidCallBack);
}

napi_value NapiCallManager::HoldCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "HoldCall", NativeHoldCall, NativeVoidCallBack);
}

napi_value NapiCallManager::UnHoldCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    NAPI_ASSERT(
        env, MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number), "Type error, should be number type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "UnHoldCall", NativeUnHoldCall, NativeVoidCallBack);
}

napi_value NapiCallManager::SwitchCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc <= TWO_VALUE_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SwitchCall", NativeSwitchCall, NativeVoidCallBack);
}

napi_value NapiCallManager::CombineConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "CombineConference", NativeCombineConference, NativeVoidCallBack);
}

napi_value NapiCallManager::SeparateConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SeparateConference", NativeSeparateConference, NativeVoidCallBack);
}

napi_value NapiCallManager::GetMainCallId(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "GetMainCallId", NativeGetMainCallId, NativePropertyCallBack);
}

napi_value NapiCallManager::GetSubCallIdList(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<ListAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "GetSubCallIdList", NativeGetSubCallIdList, NativeListCallBack);
}

napi_value NapiCallManager::GetCallIdListForConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<ListAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext, "GetCallIdListForConference", NativeGetCallIdListForConference, NativeListCallBack);
}

napi_value NapiCallManager::GetCallWaiting(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    asyncContext->thisVar = thisVar;
    return HandleAsyncWork(env, asyncContext, "GetCallWaiting", NativeGetCallWaiting, NativeCallBack);
}

napi_value NapiCallManager::SetCallWaiting(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_boolean);
    NAPI_ASSERT(env, matchFlag, "Type error, should be boolean");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    napi_get_value_bool(env, argv[ARRAY_INDEX_SECOND], &asyncContext->flag);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    asyncContext->thisVar = thisVar;
    return HandleAsyncWork(env, asyncContext, "SetCallWaiting", NativeSetCallWaiting, NativeCallBack);
}

napi_value NapiCallManager::GetCallRestriction(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->type);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    asyncContext->thisVar = thisVar;
    return HandleAsyncWork(env, asyncContext, "GetCallRestriction", NativeGetCallRestriction, NativeCallBack);
}

napi_value NapiCallManager::SetCallRestriction(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object);
    NAPI_ASSERT(env, matchFlag, "Type error, should be object type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    GetRestrictionInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    asyncContext->thisVar = thisVar;
    return HandleAsyncWork(env, asyncContext, "SetCallRestriction", NativeSetCallRestriction, NativeCallBack);
}

napi_value NapiCallManager::GetCallTransferInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->type);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    asyncContext->thisVar = thisVar;
    return HandleAsyncWork(env, asyncContext, "GetCallTransferInfo", NativeGetTransferNumber, NativeCallBack);
}

napi_value NapiCallManager::SetCallTransferInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object);
    NAPI_ASSERT(env, matchFlag, "Type error, should be object type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    GetTransferInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    asyncContext->thisVar = thisVar;
    return HandleAsyncWork(env, asyncContext, "SetCallTransferInfo", NativeSetTransferNumber, NativeCallBack);
}

napi_value NapiCallManager::StartDTMF(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_SECOND], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "StartDTMF", NativeStartDTMF, NativeVoidCallBack);
}

napi_value NapiCallManager::StopDTMF(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "StopDTMF", NativeStopDTMF, NativeVoidCallBack);
}

napi_value NapiCallManager::SendDtmf(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_SECOND], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SendDtmf", NativeSendDtmf, NativeCallBack);
}

napi_value NapiCallManager::SendBurstDtmf(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc <= TWO_VALUE_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object);
    NAPI_ASSERT(env, matchFlag, "Type error, should be object type");
    auto asyncContext = (std::make_unique<DtmfAsyncContext>()).release();
    GetDtmfBunchInfo(env, argv[ARRAY_INDEX_FIRST], *asyncContext);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SendBurstDtmf", NativeSendDtmfBunch, NativeCallBack);
}

napi_value NapiCallManager::GetCallState(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc < TWO_VALUE_LIMIT, "parameter error!");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "GetCallState", NativeGetCallState, NativePropertyCallBack);
}

napi_value NapiCallManager::IsRinging(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc < TWO_VALUE_LIMIT, "parameter error!");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "IsRinging", NativeIsRinging, NativeBoolCallBack);
}

napi_value NapiCallManager::HasCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc < TWO_VALUE_LIMIT, "parameter error!");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "HasCall", NativeHasCall, NativeBoolCallBack);
}

napi_value NapiCallManager::IsNewCallAllowed(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc < TWO_VALUE_LIMIT, "parameter error!");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "IsNewCallAllowed", NativeIsNewCallAllowed, NativeBoolCallBack);
}

napi_value NapiCallManager::IsInEmergencyCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc < TWO_VALUE_LIMIT, "parameter error!");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "IsInEmergencyCall", NativeIsInEmergencyCall, NativeBoolCallBack);
}

napi_value NapiCallManager::IsEmergencyPhoneNumber(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<UtilsAsyncContext>()).release();
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
    asyncContext->slotId = 0;
    if (argc == TWO_VALUE_LIMIT) {
        if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            asyncContext->slotId = GetIntProperty(env, argv[ARRAY_INDEX_SECOND], "slotId");
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT && MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        asyncContext->slotId = GetIntProperty(env, argv[ARRAY_INDEX_SECOND], "slotId");
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext, "IsEmergencyPhoneNumber", NativeIsEmergencyPhoneNumber, NativeBoolCallBack);
}

napi_value NapiCallManager::FormatPhoneNumber(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<UtilsAsyncContext>()).release();
    asyncContext->code = "cn";
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            asyncContext->code = GetStringProperty(env, argv[ARRAY_INDEX_SECOND], "countryCode");
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT && MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        asyncContext->code = GetStringProperty(env, argv[ARRAY_INDEX_SECOND], "countryCode");
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext, "FormatPhoneNumber", NativeFormatPhoneNumber, NativeFormatNumberCallBack);
}

napi_value NapiCallManager::FormatPhoneNumberToE164(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    matchFlag = MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<UtilsAsyncContext>()).release();
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
    char tmpStr[kMaxNumberLen];
    size_t strLen = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_SECOND], tmpStr, PHONE_NUMBER_MAXIMUM_LIMIT, &strLen);
    std::string tmpCode(tmpStr, strLen);
    asyncContext->code = tmpCode;
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext, "FormatPhoneNumberToE164", NativeFormatPhoneNumberToE164, NativeFormatNumberCallBack);
}

napi_value NapiCallManager::ObserverOn(napi_env env, napi_callback_info info)
{
    RegisterCallBack();
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    char listenerType[PHONE_NUMBER_MAXIMUM_LIMIT];
    size_t strLength = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], listenerType, PHONE_NUMBER_MAXIMUM_LIMIT, &strLength);
    std::string tmpStr = listenerType;
    TELEPHONY_LOGI("listenerType == %{public}s", tmpStr.c_str());
    if (tmpStr == "callDetailsChange") {
        EventListener callStateListener;
        (void)memset_s(&callStateListener, sizeof(EventListener), 0, sizeof(EventListener));
        callStateListener.env = env;
        callStateListener.thisVar = thisVar;
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &callStateListener.callbackRef);
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCallStateCallback(callStateListener);
    } else if (tmpStr == "callEventChange") {
        EventListener callEventListener;
        (void)memset_s(&callEventListener, sizeof(EventListener), 0, sizeof(EventListener));
        callEventListener.env = env;
        callEventListener.thisVar = thisVar;
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &callEventListener.callbackRef);
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCallEventCallback(callEventListener);
    }

    napi_value result = nullptr;
    return result;
}

napi_value NapiCallManager::ObserverOff(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    char listenerType[PHONE_NUMBER_MAXIMUM_LIMIT];
    size_t strLength = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], listenerType, PHONE_NUMBER_MAXIMUM_LIMIT, &strLength);
    std::string tmpStr = listenerType;
    TELEPHONY_LOGI("listenerType == %{public}s", tmpStr.c_str());
    if (tmpStr == "callDetailsChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterCallStateCallback();
    } else if (tmpStr == "callEventChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterCallEventCallback();
    }
    asyncContext->result = TELEPHONY_SUCCESS;
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext, "Off", [](napi_env env, void *data) {}, NativeVoidCallBack);
}

napi_value NapiCallManager::SetMuted(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    auto asyncContext = (std::make_unique<AudioAsyncContext>()).release();
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SetMuted", NativeSetMuted, NativeVoidCallBack);
}

napi_value NapiCallManager::CancelMuted(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    auto asyncContext = (std::make_unique<AudioAsyncContext>()).release();
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "CancelMuted", NativeCancelMuted, NativeVoidCallBack);
}

napi_value NapiCallManager::MuteRinger(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    auto asyncContext = (std::make_unique<AudioAsyncContext>()).release();
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "MuteRinger", NativeMuteRinger, NativeVoidCallBack);
}

napi_value NapiCallManager::SetAudioDevice(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AudioAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->dudioDevice);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SetAudioDevice", NativeSetAudioDevice, NativeVoidCallBack);
}

napi_value NapiCallManager::ControlCamera(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");

    auto asyncContext = (std::make_unique<VideoAsyncContext>()).release();
    asyncContext->callingPackage = GetBundleName(env);
    char tmpStr[kMaxNumberLen];
    size_t strLen = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], tmpStr, PHONE_NUMBER_MAXIMUM_LIMIT, &strLen);
    std::string tmpCode(tmpStr, strLen);
    asyncContext->cameraId = tmpCode;
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "ControlCamera", NativeControlCamera, NativeVoidCallBack);
}

napi_value NapiCallManager::SetPreviewWindow(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object);
    NAPI_ASSERT(env, matchFlag, "Type error, should be object type");

    auto asyncContext = (std::make_unique<VideoAsyncContext>()).release();
    asyncContext->x = GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "x");
    asyncContext->y = GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "y");
    asyncContext->z = GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "z");
    asyncContext->width = GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "width");
    asyncContext->height = GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "height");
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SetPreviewWindow", NativeSetPreviewWindow, NativeVoidCallBack);
}

napi_value NapiCallManager::SetDisplayWindow(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object);
    NAPI_ASSERT(env, matchFlag, "Type error, should be object type");

    auto asyncContext = (std::make_unique<VideoAsyncContext>()).release();
    asyncContext->x = GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "x");
    asyncContext->y = GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "y");
    asyncContext->z = GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "z");
    asyncContext->width = GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "width");
    asyncContext->height = GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "height");
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SetDisplayWindow", NativeSetDisplayWindow, NativeVoidCallBack);
}

napi_value NapiCallManager::SetCameraZoom(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");

    auto asyncContext = (std::make_unique<VideoAsyncContext>()).release();
    napi_get_value_double(env, argv[ARRAY_INDEX_FIRST], &asyncContext->zoomRatio);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SetCameraZoom", NativeSetCameraZoom, NativeVoidCallBack);
}

napi_value NapiCallManager::SetPausePicture(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    size_t result = 0;
    auto asyncContext = (std::make_unique<VideoAsyncContext>()).release();
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->path, PHONE_NUMBER_MAXIMUM_LIMIT, &result);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SetPausePicture", NativeSetPausePicture, NativeVoidCallBack);
}

napi_value NapiCallManager::SetDeviceDirection(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<VideoAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->rotation);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SetDeviceDirection", NativeSetDeviceDirection, NativeVoidCallBack);
}

napi_value NapiCallManager::SetCallPreferenceMode(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->mode);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext, "SetCallPreferenceMode", NativeSetCallPreferenceMode, NativeVoidCallBack);
}

bool NapiCallManager::MatchValueType(napi_env env, napi_value value, napi_valuetype targetType)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    return valueType == targetType;
}

napi_value NapiCallManager::CreateUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiCallManager::CreateErrorMessage(napi_env env, std::string msg)
{
    napi_value result = nullptr;
    napi_value message = nullptr;
    napi_create_string_utf8(env, (char *)msg.data(), msg.size(), &message);
    napi_create_error(env, nullptr, message, &result);
    return result;
}

void NapiCallManager::NativeCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_int32(env, asyncContext->result, &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(asyncContext->result);
            napi_reject_deferred(env, asyncContext->deferred, NapiCallManager::CreateErrorMessage(env, errTip));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateUndefined(env);
            napi_create_int32(env, asyncContext->result, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            int32_t errCode = TELEPHONY_ERR_FAIL;
            errCode = asyncContext->result;
            std::string errTip = std::to_string(errCode);
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateErrorMessage(env, errTip);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManager::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

void NapiCallManager::NativeDialCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, true, &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(asyncContext->result);
            napi_reject_deferred(env, asyncContext->deferred, NapiCallManager::CreateErrorMessage(env, errTip));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateUndefined(env);
            napi_get_boolean(env, true, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            int32_t errCode = TELEPHONY_ERR_FAIL;
            errCode = asyncContext->result;
            std::string errTip = std::to_string(errCode);
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateErrorMessage(env, errTip);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManager::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

void NapiCallManager::NativeVoidCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_status ret = napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
            TELEPHONY_LOGI("promise successful result = %{public}d", ret);
        } else {
            std::string errTip = std::to_string(asyncContext->result);
            napi_status ret = napi_reject_deferred(
                env, asyncContext->deferred, NapiCallManager::CreateErrorMessage(env, errTip));
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateUndefined(env);
            napi_get_null(env, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            int32_t errCode = TELEPHONY_ERR_FAIL;
            errCode = asyncContext->result;
            std::string errTip = std::to_string(errCode);
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateErrorMessage(env, errTip);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManager::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_status ret =
            napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        TELEPHONY_LOGI("callback result = %{public}d", ret);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

void NapiCallManager::NativePropertyCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        napi_value promiseValue = nullptr;
        napi_create_int32(env, asyncContext->result, &promiseValue);
        napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateUndefined(env);
        napi_create_int32(env, asyncContext->result, &callbackValue[ARRAY_INDEX_SECOND]);
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

void NapiCallManager::NativeBoolCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->result != BOOL_VALUE_IS_FALSE) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, true, &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, false, &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(asyncContext->errorCode);
            napi_reject_deferred(env, asyncContext->deferred, NapiCallManager::CreateErrorMessage(env, errTip));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        if (asyncContext->result != BOOL_VALUE_IS_FALSE) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateUndefined(env);
            napi_get_boolean(env, true, &callbackValue[ARRAY_INDEX_SECOND]);
        } else if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateUndefined(env);
            napi_get_boolean(env, false, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(asyncContext->errorCode);
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateErrorMessage(env, errTip);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManager::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

void NapiCallManager::NativeFormatNumberCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (UtilsAsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_string_utf8(env, Str16ToStr8(asyncContext->formatNumber).data(),
                asyncContext->formatNumber.length(), &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(asyncContext->result);
            napi_reject_deferred(env, asyncContext->deferred, NapiCallManager::CreateErrorMessage(env, errTip));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateUndefined(env);
            napi_create_string_utf8(env, Str16ToStr8(asyncContext->formatNumber).data(),
                asyncContext->formatNumber.length(), &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            int32_t errCode = TELEPHONY_ERR_FAIL;
            errCode = asyncContext->result;
            std::string errTip = std::to_string(errCode);
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateErrorMessage(env, errTip);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManager::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

void NapiCallManager::NativeListCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (ListAsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        napi_value promiseValue = nullptr;
        napi_create_array(env, &promiseValue);
        std::vector<std::u16string>::iterator it = asyncContext->listResult.begin();
        int i = 0;
        for (; it != asyncContext->listResult.end(); it++) {
            napi_value info = nullptr;
            napi_create_string_utf8(env, Str16ToStr8(*it).data(), (*it).length(), &info);
            napi_set_element(env, promiseValue, i, info);
            ++i;
        }
        napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        callbackValue[ARRAY_INDEX_FIRST] = NapiCallManager::CreateUndefined(env);
        napi_create_array(env, &callbackValue[ARRAY_INDEX_SECOND]);
        std::vector<std::u16string>::iterator it = asyncContext->listResult.begin();
        int i = 0;
        for (; it != asyncContext->listResult.end(); it++) {
            napi_value info = nullptr;
            napi_create_string_utf8(env, Str16ToStr8(*it).data(), (*it).length(), &info);
            napi_set_element(env, callbackValue[ARRAY_INDEX_SECOND], i, info);
            ++i;
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

void NapiCallManager::GetDialInfo(napi_env env, napi_value objValue, DialAsyncContext &asyncContext)
{
    asyncContext.accountId = GetIntProperty(env, objValue, "accountId");
    asyncContext.videoState = GetIntProperty(env, objValue, "videoState");
    asyncContext.dialScene = GetIntProperty(env, objValue, "dialScene");
    asyncContext.dialType = GetIntProperty(env, objValue, "dialType");
}

void NapiCallManager::GetSmsInfo(napi_env env, napi_value objValue, RejectAsyncContext &asyncContext)
{
    asyncContext.isSendSms = true;
    asyncContext.messageContent = GetStringProperty(env, objValue, "messageContent");
}

void NapiCallManager::GetDtmfBunchInfo(napi_env env, napi_value objValue, DtmfAsyncContext &asyncContext)
{
    asyncContext.callId = GetIntProperty(env, objValue, "callId");
    asyncContext.code = GetStringProperty(env, objValue, "str");
    asyncContext.on = GetIntProperty(env, objValue, "on");
    asyncContext.off = GetIntProperty(env, objValue, "off");
    if (asyncContext.off == 0) {
        asyncContext.off = DTMF_DEFAULT_OFF;
    }
}

void NapiCallManager::GetRestrictionInfo(napi_env env, napi_value objValue, SupplementAsyncContext &asyncContext)
{
    asyncContext.type = GetIntProperty(env, objValue, "type");
    asyncContext.mode = GetIntProperty(env, objValue, "mode");
    asyncContext.content = GetStringProperty(env, objValue, "password");
}

void NapiCallManager::GetTransferInfo(napi_env env, napi_value objValue, SupplementAsyncContext &asyncContext)
{
    asyncContext.content = GetStringProperty(env, objValue, "transferNum");
    asyncContext.type = GetIntProperty(env, objValue, "type");
    asyncContext.mode = GetIntProperty(env, objValue, "settingType");
}

void NapiCallManager::NativeDialCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (DialAsyncContext *)data;
    std::string phoneNumber(asyncContext->number, asyncContext->numberLen);
    OHOS::AppExecFwk::PacMap dialInfo;
    dialInfo.PutIntValue("accountId", asyncContext->accountId);
    dialInfo.PutIntValue("videoState", asyncContext->videoState);
    dialInfo.PutIntValue("dialScene", asyncContext->dialScene);
    dialInfo.PutIntValue("dialType", asyncContext->dialType);

    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->DialCall(Str8ToStr16(phoneNumber), dialInfo);
}

void NapiCallManager::NativeAnswerCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AnswerAsyncContext *)data;
    int32_t ret = DelayedSingleton<CallManagerClient>::GetInstance()->AnswerCall(
        asyncContext->callId, asyncContext->videoState);
    asyncContext->result = ret;
}

void NapiCallManager::NativeRejectCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (RejectAsyncContext *)data;
    int32_t ret = DelayedSingleton<CallManagerClient>::GetInstance()->RejectCall(
        asyncContext->callId, asyncContext->isSendSms, Str8ToStr16(asyncContext->messageContent));
    asyncContext->result = ret;
}

void NapiCallManager::NativeHangUpCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->HangUpCall(asyncContext->callId);
}

void NapiCallManager::NativeHoldCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->HoldCall(asyncContext->callId);
}

void NapiCallManager::NativeUnHoldCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->UnHoldCall(asyncContext->callId);
}

void NapiCallManager::NativeSwitchCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SwitchCall(asyncContext->callId);
}

void NapiCallManager::NativeCombineConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->CombineConference(asyncContext->callId);
}

void NapiCallManager::NativeSeparateConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->SeparateConference(asyncContext->callId);
}

void NapiCallManager::NativeGetMainCallId(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->GetMainCallId(asyncContext->callId);
}

void NapiCallManager::NativeGetSubCallIdList(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (ListAsyncContext *)data;
    asyncContext->listResult =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetSubCallIdList(asyncContext->callId);
}

void NapiCallManager::NativeGetCallIdListForConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (ListAsyncContext *)data;
    asyncContext->listResult =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetCallIdListForConference(asyncContext->callId);
}

void NapiCallManager::NativeGetCallWaiting(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    EventListener waitingInfoListener;
    waitingInfoListener.env = asyncContext->env;
    waitingInfoListener.thisVar = asyncContext->thisVar;
    waitingInfoListener.callbackRef = asyncContext->callbackRef;
    waitingInfoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetWaitingCallback(waitingInfoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        return;
    }
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->GetCallWaiting(asyncContext->slotId);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetWaitingCallback();
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeSetCallWaiting(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    EventListener waitingInfoListener;
    waitingInfoListener.env = asyncContext->env;
    waitingInfoListener.thisVar = asyncContext->thisVar;
    waitingInfoListener.callbackRef = asyncContext->callbackRef;
    waitingInfoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetWaitingCallback(waitingInfoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        return;
    }
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetCallWaiting(
        asyncContext->slotId, asyncContext->flag);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterSetWaitingCallback();
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeGetCallRestriction(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    EventListener infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetRestrictionCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        return;
    }
    CallRestrictionType type = static_cast<CallRestrictionType>(asyncContext->type);
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetCallRestriction(asyncContext->slotId, type);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetRestrictionCallback();
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeSetCallRestriction(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    EventListener infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetRestrictionCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        return;
    }
    CallRestrictionInfo info;
    info.fac = static_cast<CallRestrictionType>(asyncContext->type);
    info.mode = static_cast<CallRestrictionMode>(asyncContext->mode);
    (void)memcpy_s(
        info.password, PHONE_NUMBER_MAXIMUM_LIMIT, asyncContext->content.c_str(), asyncContext->content.length());
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetCallRestriction(asyncContext->slotId, info);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterSetRestrictionCallback();
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeGetTransferNumber(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    EventListener infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetTransferCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        return;
    }
    CallTransferType type = static_cast<CallTransferType>(asyncContext->type);
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetCallTransferInfo(asyncContext->slotId, type);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetTransferCallback();
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeSetTransferNumber(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    EventListener infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetTransferCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        return;
    }
    CallTransferInfo info;
    info.settingType = static_cast<CallTransferSettingType>(asyncContext->mode);
    info.type = static_cast<CallTransferType>(asyncContext->type);
    (void)memcpy_s(info.transferNum, PHONE_NUMBER_MAXIMUM_LIMIT, asyncContext->content.c_str(),
        asyncContext->content.length());
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetCallTransferInfo(asyncContext->slotId, info);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterSetTransferCallback();
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeStartDTMF(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->numberLen < TWO_VALUE_LIMIT) {
        asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->StartDtmf(
            asyncContext->callId, asyncContext->number[ARRAY_INDEX_FIRST]);
    } else {
        asyncContext->result = TELEPHONY_ERR_FAIL;
    }
}

void NapiCallManager::NativeStopDTMF(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->StopDtmf(asyncContext->callId);
}

void NapiCallManager::NativeSendDtmf(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->numberLen < TWO_VALUE_LIMIT) {
        asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SendDtmf(
            asyncContext->callId, asyncContext->number[ARRAY_INDEX_FIRST]);
    } else {
        asyncContext->result = TELEPHONY_ERR_FAIL;
    }
}

void NapiCallManager::NativeSendDtmfBunch(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (DtmfAsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SendBurstDtmf(
        asyncContext->callId, Str8ToStr16(asyncContext->code), asyncContext->on, asyncContext->off);
}

void NapiCallManager::NativeGetCallState(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->GetCallState();
}

void NapiCallManager::NativeIsRinging(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->IsRinging();
}

void NapiCallManager::NativeHasCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->HasCall();
}

void NapiCallManager::NativeIsNewCallAllowed(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->IsNewCallAllowed();
}

void NapiCallManager::NativeIsInEmergencyCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->IsInEmergencyCall();
}

void NapiCallManager::NativeIsEmergencyPhoneNumber(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (UtilsAsyncContext *)data;
    std::string tmpPhoneNumber(asyncContext->number, asyncContext->numberLen);
    std::u16string phoneNumber = Str8ToStr16(tmpPhoneNumber);
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->IsEmergencyPhoneNumber(
        phoneNumber, asyncContext->slotId, asyncContext->errorCode);
}

void NapiCallManager::NativeFormatPhoneNumber(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (UtilsAsyncContext *)data;
    std::string tmpPhoneNumber(asyncContext->number, asyncContext->numberLen);
    std::u16string phoneNumber = Str8ToStr16(tmpPhoneNumber);
    std::u16string countryCode = Str8ToStr16(asyncContext->code);
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
        phoneNumber, countryCode, asyncContext->formatNumber);
}

void NapiCallManager::NativeFormatPhoneNumberToE164(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (UtilsAsyncContext *)data;
    std::string tmpPhoneNumber(asyncContext->number, asyncContext->numberLen);
    std::u16string phoneNumber = Str8ToStr16(tmpPhoneNumber);
    std::u16string countryCode = Str8ToStr16(asyncContext->code);
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumberToE164(
        phoneNumber, countryCode, asyncContext->formatNumber);
}

void NapiCallManager::NativeSetMuted(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AudioAsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetMuted(true);
}

void NapiCallManager::NativeCancelMuted(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AudioAsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetMuted(false);
}

void NapiCallManager::NativeMuteRinger(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AudioAsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->MuteRinger();
}

void NapiCallManager::NativeSetAudioDevice(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    AudioDevice type;
    auto asyncContext = (AudioAsyncContext *)data;
    type = static_cast<AudioDevice>(asyncContext->dudioDevice);
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetAudioDevice(type);
}

void NapiCallManager::NativeControlCamera(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }

    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->ControlCamera(
        Str8ToStr16(asyncContext->cameraId), asyncContext->callingPackage);
}

void NapiCallManager::NativeSetPreviewWindow(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }

    auto asyncContext = (VideoAsyncContext *)data;
    VideoWindow window;
    window.x = asyncContext->x;
    window.y = asyncContext->y;
    window.z = asyncContext->z;
    window.width = asyncContext->width;
    window.height = asyncContext->height;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetPreviewWindow(window);
}

void NapiCallManager::NativeSetDisplayWindow(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }

    auto asyncContext = (VideoAsyncContext *)data;
    VideoWindow window;
    window.x = asyncContext->x;
    window.y = asyncContext->y;
    window.z = asyncContext->z;
    window.width = asyncContext->width;
    window.height = asyncContext->height;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetDisplayWindow(window);
}

void NapiCallManager::NativeSetCameraZoom(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }

    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetCameraZoom(asyncContext->zoomRatio);
}

void NapiCallManager::NativeSetPausePicture(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetPausePicture(Str8ToStr16(asyncContext->path));
}

void NapiCallManager::NativeSetDeviceDirection(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetDeviceDirection(asyncContext->rotation);
}

void NapiCallManager::NativeSetCallPreferenceMode(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetCallPreferenceMode(
        asyncContext->callId, asyncContext->mode);
}

napi_value NapiCallManager::ToInt32Value(napi_env env, int32_t value)
{
    napi_value staticValue = nullptr;
    napi_create_int32(env, value, &staticValue);
    return staticValue;
}

napi_value NapiCallManager::GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    napi_value value = nullptr;
    napi_get_named_property(env, object, propertyName.c_str(), &value);
    return value;
}

std::string NapiCallManager::GetStringProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    napi_value value = nullptr;
    napi_status getNameStatus = napi_get_named_property(env, object, propertyName.c_str(), &value);
    if (getNameStatus == napi_ok) {
        char chars[MESSAGE_CONTENT_MAXIMUM_LIMIT] = {0};
        size_t charLength = 0;
        napi_status getStringStatus =
            napi_get_value_string_utf8(env, value, chars, MESSAGE_CONTENT_MAXIMUM_LIMIT, &charLength);
        if (getStringStatus == napi_ok && charLength > 0) {
            return std::string(chars, charLength);
        }
    }
    return "";
}

int32_t NapiCallManager::GetIntProperty(napi_env env, napi_value object, const std::string &propertyName)
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
    return intValue;
}

bool NapiCallManager::GetBoolProperty(napi_env env, napi_value object, const std::string &propertyName)
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
    return boolValue;
}

void NapiCallManager::RegisterCallBack()
{
    if (registerStatus_) {
        TELEPHONY_LOGW("you are already registered!");
        return;
    }
    std::unique_ptr<NapiCallManagerCallback> callbackPtr = std::make_unique<NapiCallManagerCallback>();
    if (callbackPtr == nullptr) {
        TELEPHONY_LOGE("make_unique NapiCallManagerCallback failed!");
        return;
    }
    int32_t ret = DelayedSingleton<CallManagerClient>::GetInstance()->RegisterCallBack(std::move(callbackPtr));
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterCallBack failed!");
        return;
    }
    registerStatus_ = true;
}

std::u16string NapiCallManager::GetBundleName(napi_env env)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok || global == nullptr) {
        TELEPHONY_LOGE("can't get global instance for %{public}d", status);
        return u"";
    }

    napi_value abilityObj = nullptr;
    status = napi_get_named_property(env, global, "ability", &abilityObj);
    if (status != napi_ok || abilityObj == nullptr) {
        TELEPHONY_LOGE("can't get ability obj for %{public}d", status);
        return u"";
    }

    OHOS::AppExecFwk::Ability *ability = nullptr;
    status = napi_get_value_external(env, abilityObj, (void **)&ability);
    if (status != napi_ok || ability == nullptr) {
        TELEPHONY_LOGE("get ability from property failed for %{public}d", status);
        return u"";
    }

    return Str8ToStr16(ability->GetBundleName());
}

napi_value NapiCallManager::HandleAsyncWork(napi_env env, AsyncContext *context, std::string workName,
    napi_async_execute_callback execute, napi_async_complete_callback complete)
{
    TELEPHONY_LOGI("HandleAsyncWork start workName = %{public}s", workName.c_str());
    napi_value result = nullptr;
    if (context->callbackRef == nullptr) {
        napi_create_promise(env, &context->deferred, &result);
    } else {
        napi_get_undefined(env, &result);
    }
    napi_value resource = CreateUndefined(env);
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, workName.data(), NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, resource, resourceName, execute, complete, (void *)context, &context->work);
    napi_queue_async_work(env, context->work);
    return result;
}

static napi_module g_nativeCallManagerModule = {
    .nm_version = NATIVE_VERSION,
    .nm_flags = NATIVE_FLAGS,
    .nm_filename = nullptr,
    .nm_register_func = NapiCallManager::RegisterCallManagerFunc,
    .nm_modname = "telephony.call",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&g_nativeCallManagerModule);
}
} // namespace Telephony
} // namespace OHOS