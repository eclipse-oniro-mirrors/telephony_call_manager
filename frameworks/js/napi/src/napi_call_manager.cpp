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

#include "napi_call_manager.h"

#include <string_ex.h>
#include <securec.h>

#include "system_ability_definition.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "napi_call_manager_types.h"
#include "napi_call_ability_callback.h"
#include "call_manager_client.h"
#include "napi_call_manager_utils.h"
#include "ability_manager_client.h"
#include "element_name.h"
#include "napi_util.h"
#include "string_wrapper.h"
#include "want.h"
#include "telephony_napi_common_error.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
static constexpr const char *JS_ERROR_TELEPHONY_INVALID_INPUT_PARAMETER_STRING =
    "BusinessError 401: Parameter error. The type of parameter should match or the number of parameters must match.";
static constexpr const char *OBSERVER_ON_JS_PERMISSION_ERROR_STRING =
    "BusinessError 201: Permission denied. An attempt was made to On forbidden by permission: "
    "ohos.permission.SET_TELEPHONY_STATE.";
static constexpr const char *OBSERVER_OFF_JS_PERMISSION_ERROR_STRING =
    "BusinessError 201: Permission denied. An attempt was made to Off forbidden by permission: "
    "ohos.permission.SET_TELEPHONY_STATE.";
int32_t NapiCallManager::registerStatus_ = TELEPHONY_ERROR;

NapiCallManager::NapiCallManager() {}

NapiCallManager::~NapiCallManager()
{
    DelayedSingleton<CallManagerClient>::GetInstance()->UnInit();
}

void Init()
{
    // Establish a connection with call_manager
    DelayedSingleton<CallManagerClient>::GetInstance()->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
}

napi_value NapiCallManager::DeclareCallBasisInterface(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("dial", DialCall),
        DECLARE_NAPI_FUNCTION("makeCall", MakeCall),
        DECLARE_NAPI_FUNCTION("answer", AnswerCall),
        DECLARE_NAPI_FUNCTION("reject", RejectCall),
        DECLARE_NAPI_FUNCTION("hangup", HangUpCall),
        DECLARE_NAPI_FUNCTION("holdCall", HoldCall),
        DECLARE_NAPI_FUNCTION("unHoldCall", UnHoldCall),
        DECLARE_NAPI_FUNCTION("switchCall", SwitchCall),
        DECLARE_NAPI_FUNCTION("setCallPreferenceMode", SetCallPreferenceMode),
        DECLARE_NAPI_FUNCTION("hasVoiceCapability", HasVoiceCapability),
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
        DECLARE_NAPI_FUNCTION("enableImsSwitch", EnableImsSwitch),
        DECLARE_NAPI_FUNCTION("disableImsSwitch", DisableImsSwitch),
        DECLARE_NAPI_FUNCTION("isImsSwitchEnabled", IsImsSwitchEnabled),
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
        DECLARE_NAPI_FUNCTION("reportOttCallDetailsInfo", ReportOttCallDetailsInfo),
        DECLARE_NAPI_FUNCTION("reportOttCallEventInfo", ReportOttCallEventInfo),
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
        DECLARE_NAPI_FUNCTION("controlCamera", ControlCamera),
        DECLARE_NAPI_FUNCTION("setPreviewWindow", SetPreviewWindow),
        DECLARE_NAPI_FUNCTION("setDisplayWindow", SetDisplayWindow),
        DECLARE_NAPI_FUNCTION("setCameraZoom", SetCameraZoom),
        DECLARE_NAPI_FUNCTION("setPausePicture", SetPausePicture),
        DECLARE_NAPI_FUNCTION("setDeviceDirection", SetDeviceDirection),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallImsInterface(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("startRTT", StartRTT),
        DECLARE_NAPI_FUNCTION("stopRTT", StopRTT),
        DECLARE_NAPI_FUNCTION("joinConference", JoinConference),
        DECLARE_NAPI_FUNCTION("updateImsCallMode", UpdateImsCallMode),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallMediaEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // AudioDevice
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_EARPIECE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_EARPIECE))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_SPEAKER",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_SPEAKER))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_WIRED_HEADSET",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_WIRED_HEADSET))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_BLUETOOTH_SCO",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_BLUETOOTH_SCO))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_DISABLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_DISABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_UNKNOWN",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_UNKNOWN))),
        // VideoStateType
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VOICE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_VOICE))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VIDEO",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_VIDEO))),
        // ImsCallMode
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_AUDIO_ONLY", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_AUDIO_ONLY)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_SEND_RECEIVE", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_SEND_RECEIVE)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallDialEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // DialScene
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_NORMAL", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialScene::CALL_NORMAL))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_PRIVILEGED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialScene::CALL_PRIVILEGED))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_EMERGENCY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialScene::CALL_EMERGENCY))),
        // CallType
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_CS", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_CS))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_IMS", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_IMS))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_OTT", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_OTT))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_ERR_CALL",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_ERR_CALL))),
        // DialType
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_CARRIER_TYPE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialType::DIAL_CARRIER_TYPE))),
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_VOICE_MAIL_TYPE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialType::DIAL_VOICE_MAIL_TYPE))),
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_OTT_TYPE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialType::DIAL_OTT_TYPE))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallStateEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // TelCallState
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_ACTIVE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_HOLDING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_HOLDING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DIALING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_DIALING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_ALERTING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_ALERTING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_INCOMING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_WAITING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_WAITING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DISCONNECTED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DISCONNECTING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_IDLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE))),
        // TelConferenceState
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_IDLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_IDLE))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_ACTIVE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_ACTIVE))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_HOLDING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_HOLDING))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_DISCONNECTING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_DISCONNECTING))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_DISCONNECTED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_DISCONNECTED))),
        // CallStateToApp
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_UNKNOWN",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_IDLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallStateToApp::CALL_STATE_IDLE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_RINGING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallStateToApp::CALL_STATE_RINGING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_OFFHOOK",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallStateToApp::CALL_STATE_OFFHOOK))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallEventEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // CallAbilityEventId
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_DIAL_NO_CARRIER",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_DIAL_NO_CARRIER))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_INVALID_FDN_NUMBER",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_INVALID_FDN_NUMBER))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallRestrictionEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // CallRestrictionType
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ALL_OUTGOING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_INTERNATIONAL",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_INTERNATIONAL))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ALL_INCOMING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ROAMING_INCOMING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ROAMING_INCOMING))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ALL_CALLS",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_OUTGOING_SERVICES",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_OUTGOING_SERVICES))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_INCOMING_SERVICES",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_INCOMING_SERVICES))),
        // CallRestrictionMode
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_MODE_DEACTIVATION",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionMode::RESTRICTION_MODE_DEACTIVATION))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_MODE_ACTIVATION",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionMode::RESTRICTION_MODE_ACTIVATION))),
        // RestrictionStatus
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_DISABLE", NapiCallManagerUtils::ToInt32Value(env, RESTRICTION_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_ENABLE", NapiCallManagerUtils::ToInt32Value(env, RESTRICTION_ENABLE)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallWaitingEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // CallWaitingStatus
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_WAITING_DISABLE", NapiCallManagerUtils::ToInt32Value(env, CALL_WAITING_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_WAITING_ENABLE", NapiCallManagerUtils::ToInt32Value(env, CALL_WAITING_ENABLE)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallTransferEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // TransferStatus
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_DISABLE", NapiCallManagerUtils::ToInt32Value(env, TRANSFER_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_ENABLE", NapiCallManagerUtils::ToInt32Value(env, TRANSFER_ENABLE)),
        // CallTransferType
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_UNCONDITIONAL",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_UNCONDITIONAL))),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_BUSY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_BUSY))),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_NO_REPLY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_NO_REPLY))),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_NOT_REACHABLE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_NOT_REACHABLE))),
        // CallTransferSettingType
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_ENABLE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_ENABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_DISABLE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_DISABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_REGISTRATION",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_REGISTRATION))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_ERASURE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_ERASURE))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

/**
 * Enumeration type extension.
 */
napi_value NapiCallManager::DeclareAudioDeviceEnum(napi_env env, napi_value exports)
{
    // AudioDevice
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_EARPIECE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_EARPIECE))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_SPEAKER",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_SPEAKER))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_WIRED_HEADSET",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_WIRED_HEADSET))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_BLUETOOTH_SCO",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_BLUETOOTH_SCO))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_DISABLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_DISABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_UNKNOWN",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDevice::DEVICE_UNKNOWN))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "AudioDevice", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "AudioDevice", result);
    return exports;
}

napi_value NapiCallManager::DeclareVideoStateTypeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VOICE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_VOICE))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VIDEO",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_VIDEO))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "VideoStateType", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "VideoStateType", result);
    return exports;
}

napi_value NapiCallManager::DeclareImsCallModeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_AUDIO_ONLY", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_AUDIO_ONLY)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_SEND_RECEIVE", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_SEND_RECEIVE)),
    };
    napi_value result = nullptr;
    napi_define_class(env, "ImsCallMode", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "ImsCallMode", result);
    return exports;
}

napi_value NapiCallManager::DeclareDialSceneEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_NORMAL", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialScene::CALL_NORMAL))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_PRIVILEGED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialScene::CALL_PRIVILEGED))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_EMERGENCY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialScene::CALL_EMERGENCY))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "DialScene", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "DialScene", result);
    return exports;
}

napi_value NapiCallManager::DeclareCallTypeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_CS", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_CS))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_IMS", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_IMS))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_OTT", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_OTT))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_ERR_CALL",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_ERR_CALL))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallType", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallType", result);
    return exports;
}

napi_value NapiCallManager::DeclareDialTypeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_CARRIER_TYPE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialType::DIAL_CARRIER_TYPE))),
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_VOICE_MAIL_TYPE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialType::DIAL_VOICE_MAIL_TYPE))),
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_OTT_TYPE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialType::DIAL_OTT_TYPE))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "DialType", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "DialType", result);
    return exports;
}

napi_value NapiCallManager::DeclareTelCallStateEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_ACTIVE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_HOLDING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_HOLDING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DIALING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_DIALING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_ALERTING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_ALERTING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_INCOMING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_WAITING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_WAITING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DISCONNECTED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DISCONNECTING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_IDLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "TelCallState", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "TelCallState", result);
    return exports;
}

napi_value NapiCallManager::DeclareConferenceStateEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_IDLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_IDLE))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_ACTIVE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_ACTIVE))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_HOLDING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_HOLDING))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_DISCONNECTING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_DISCONNECTING))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_DISCONNECTED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_DISCONNECTED))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "TelConferenceState", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "TelConferenceState", result);
    return exports;
}

napi_value NapiCallManager::DeclareCallStateToAppEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_UNKNOWN",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_IDLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallStateToApp::CALL_STATE_IDLE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_RINGING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallStateToApp::CALL_STATE_RINGING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_OFFHOOK",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallStateToApp::CALL_STATE_OFFHOOK))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallState", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallState", result);
    return exports;
}

napi_value NapiCallManager::DeclareCallEventEnumEx(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // CallAbilityEventId
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_DIAL_NO_CARRIER",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_DIAL_NO_CARRIER))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_INVALID_FDN_NUMBER",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_INVALID_FDN_NUMBER))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallAbilityEventId", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallAbilityEventId", result);
    return exports;
}

napi_value NapiCallManager::DeclareRestrictionTypeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ALL_OUTGOING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_INTERNATIONAL",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_INTERNATIONAL))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ALL_INCOMING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ROAMING_INCOMING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ROAMING_INCOMING))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ALL_CALLS",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_OUTGOING_SERVICES",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_OUTGOING_SERVICES))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_INCOMING_SERVICES",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_INCOMING_SERVICES))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallRestrictionType", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallRestrictionType", result);
    return exports;
}

napi_value NapiCallManager::DeclareRestrictionModeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_MODE_DEACTIVATION",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionMode::RESTRICTION_MODE_DEACTIVATION))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_MODE_ACTIVATION",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionMode::RESTRICTION_MODE_ACTIVATION))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallRestrictionMode", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallRestrictionMode", result);
    return exports;
}

napi_value NapiCallManager::DeclareRestrictionStatusEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_DISABLE", NapiCallManagerUtils::ToInt32Value(env, RESTRICTION_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "RESTRICTION_ENABLE", NapiCallManagerUtils::ToInt32Value(env, RESTRICTION_ENABLE)),
    };
    napi_value result = nullptr;
    napi_define_class(env, "RestrictionStatus", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "RestrictionStatus", result);
    return exports;
}

napi_value NapiCallManager::DeclareCallWaitingEnumEx(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_WAITING_DISABLE", NapiCallManagerUtils::ToInt32Value(env, CALL_WAITING_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_WAITING_ENABLE", NapiCallManagerUtils::ToInt32Value(env, CALL_WAITING_ENABLE)),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallWaitingStatus", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallWaitingStatus", result);
    return exports;
}

napi_value NapiCallManager::DeclareTransferStatusEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_DISABLE", NapiCallManagerUtils::ToInt32Value(env, TRANSFER_DISABLE)),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_ENABLE", NapiCallManagerUtils::ToInt32Value(env, TRANSFER_ENABLE)),
    };
    napi_value result = nullptr;
    napi_define_class(env, "TransferStatus", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "TransferStatus", result);
    return exports;
}

napi_value NapiCallManager::DeclareTransferTypeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_UNCONDITIONAL",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_UNCONDITIONAL))),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_BUSY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_BUSY))),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_NO_REPLY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_NO_REPLY))),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_NOT_REACHABLE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_NOT_REACHABLE))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallTransferType", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallTransferType", result);
    return exports;
}

napi_value NapiCallManager::DeclareTransferSettingTypeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_ENABLE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_ENABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_DISABLE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_DISABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_REGISTRATION",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_REGISTRATION))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_ERASURE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_ERASURE))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallTransferSettingType", NAPI_AUTO_LENGTH,
        NapiCallManagerUtils::CreateEnumConstructor, nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallTransferSettingType", result);
    return exports;
}

/**
 * The call_manager is initialized.
 */
napi_value NapiCallManager::RegisterCallManagerFunc(napi_env env, napi_value exports)
{
    // Interface initialization
    DeclareCallBasisInterface(env, exports);
    DeclareCallConferenceInterface(env, exports);
    DeclareCallSupplementInterface(env, exports);
    DeclareCallExtendInterface(env, exports);
    DeclareCallMultimediaInterface(env, exports);
    // Enumeration class initialization
    DeclareCallMediaEnum(env, exports);
    DeclareCallDialEnum(env, exports);
    DeclareCallStateEnum(env, exports);
    DeclareCallEventEnum(env, exports);
    DeclareCallRestrictionEnum(env, exports);
    DeclareCallWaitingEnum(env, exports);
    DeclareCallTransferEnum(env, exports);
    DeclareCallImsInterface(env, exports);
    // Enumeration class extension initialization
    DeclareAudioDeviceEnum(env, exports);
    DeclareVideoStateTypeEnum(env, exports);
    DeclareImsCallModeEnum(env, exports);
    DeclareDialSceneEnum(env, exports);
    DeclareCallTypeEnum(env, exports);
    DeclareDialTypeEnum(env, exports);
    DeclareTelCallStateEnum(env, exports);
    DeclareConferenceStateEnum(env, exports);
    DeclareCallStateToAppEnum(env, exports);
    DeclareCallEventEnumEx(env, exports);
    DeclareRestrictionTypeEnum(env, exports);
    DeclareRestrictionModeEnum(env, exports);
    DeclareRestrictionStatusEnum(env, exports);
    DeclareCallWaitingEnumEx(env, exports);
    DeclareTransferStatusEnum(env, exports);
    DeclareTransferTypeEnum(env, exports);
    DeclareTransferSettingTypeEnum(env, exports);
    Init();
    RegisterCallBack();
    return exports;
}

napi_value NapiCallManager::DialCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "DialCall type error, should be string type");
    auto asyncContext = (std::make_unique<DialAsyncContext>());
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "DialCall error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            GetDialInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        GetDialInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "DialCall", NativeDialCall, NativeDialCallBack);
}

napi_value NapiCallManager::MakeCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc <= TWO_VALUE_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "MakeCall type error, should be string type");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MakeCall error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "MakeCall", NativeMakeCall, NativeVoidCallBack);
}

napi_value NapiCallManager::AnswerCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    auto asyncContext = std::make_unique<AnswerAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "AnswerCall error at answerAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }

    if (argc == ZERO_VALUE) {
        TELEPHONY_LOGI("no param input");
    } else if (argc == ONLY_ONE_VALUE) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number)) {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        } else {
            TELEPHONY_LOGE("AnswerCall args error, argv type is not correct");
        }
    } else if (argc == TWO_VALUE_LIMIT &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number) &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
        napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    } else {
        TELEPHONY_LOGE("AnswerCall args error, argv type is not correct");
    }

    return HandleAsyncWork(env, asyncContext.release(), "AnswerCall", NativeAnswerCall, NativeVoidCallBack);
}

napi_value NapiCallManager::RejectCallWithZeroArgc(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, FOUR_VALUE_MAXIMUM_LIMIT);
    auto asyncContext = std::make_unique<RejectAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "reject with zero argc, error at rejectAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    asyncContext->isSendSms = false;

    return HandleAsyncWork(env, asyncContext.release(), "RejectCall", NativeRejectCall, NativeVoidCallBack);
}

napi_value NapiCallManager::RejectCallWithOneArgc(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, FOUR_VALUE_MAXIMUM_LIMIT);
    auto asyncContext = std::make_unique<RejectAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "reject with one argc, error at rejectAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    asyncContext->isSendSms = false;
    if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_function)) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number)) {
        napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object)) {
        GetSmsInfo(env, argv[ARRAY_INDEX_FIRST], *asyncContext);
    } else {
        TELEPHONY_LOGE("args error, argv type is not correct");
    }

    return HandleAsyncWork(env, asyncContext.release(), "RejectCall", NativeRejectCall, NativeVoidCallBack);
}

napi_value NapiCallManager::RejectCallWithTwoArgc(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, FOUR_VALUE_MAXIMUM_LIMIT);
    auto asyncContext = std::make_unique<RejectAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "reject with two argcs, error at rejectAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    asyncContext->isSendSms = false;
    if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object) &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
        GetSmsInfo(env, argv[ARRAY_INDEX_FIRST], *asyncContext);
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number) &&
               NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
        napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number) &&
               NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        GetSmsInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
    } else {
        TELEPHONY_LOGE("args error, argv type is not correct");
    }

    return HandleAsyncWork(env, asyncContext.release(), "RejectCall", NativeRejectCall, NativeVoidCallBack);
}

napi_value NapiCallManager::RejectCallWithThreeArgc(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, FOUR_VALUE_MAXIMUM_LIMIT);
    auto asyncContext = std::make_unique<RejectAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "reject with three argcs, error at rejectAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    asyncContext->isSendSms = false;
    if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number) &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        GetSmsInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }

    return HandleAsyncWork(env, asyncContext.release(), "RejectCall", NativeRejectCall, NativeVoidCallBack);
}

napi_value NapiCallManager::RejectCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, FOUR_VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < FOUR_VALUE_MAXIMUM_LIMIT, "parameter error!");
    napi_value result = nullptr;
    switch (argc) {
        case ZERO_VALUE:
            TELEPHONY_LOGI("no param input");
            result = RejectCallWithZeroArgc(env, info);
            break;
        case ONLY_ONE_VALUE:
            result = RejectCallWithOneArgc(env, info);
            break;
        case TWO_VALUE_LIMIT:
            result = RejectCallWithTwoArgc(env, info);
            break;
        case VALUE_MAXIMUM_LIMIT:
            result = RejectCallWithThreeArgc(env, info);
            break;
        default:
            TELEPHONY_LOGE("args error, argv type is not correct");
            break;
    }
    return result;
}

napi_value NapiCallManager::HangUpCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "HangUpCall error at asyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }

    if (argc == ZERO_VALUE) {
        TELEPHONY_LOGI("no param input");
    } else if (argc == ONLY_ONE_VALUE) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number)) {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        } else {
            TELEPHONY_LOGE("HangUpCall args error, argv type is not correct");
        }
    } else if (argc == TWO_VALUE_LIMIT &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number) &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
        napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    } else {
        TELEPHONY_LOGE("HangUpCall args error, argv type is not correct");
    }

    return HandleAsyncWork(env, asyncContext.release(), "HangUpCall", NativeHangUpCall, NativeVoidCallBack);
}

napi_value NapiCallManager::HoldCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "HoldCall type error, should be number type");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "HoldCall error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "HoldCall", NativeHoldCall, NativeVoidCallBack);
}

napi_value NapiCallManager::UnHoldCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "UnHoldCall error at asyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "UnHoldCall", NativeUnHoldCall, NativeVoidCallBack);
}

napi_value NapiCallManager::SwitchCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc <= TWO_VALUE_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "SwitchCall type error, should be number type");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SwitchCall error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "SwitchCall", NativeSwitchCall, NativeVoidCallBack);
}

napi_value NapiCallManager::CombineConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "CombineConference type error, should be number type");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "CombineConference error at asyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "CombineConference", NativeCombineConference, NativeVoidCallBack);
}

napi_value NapiCallManager::SeparateConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "SeparateConference type error, should be number type");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SeparateConference error at asyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "SeparateConference", NativeSeparateConference, NativeVoidCallBack);
}

napi_value NapiCallManager::GetMainCallId(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "GetMainCallId type error, should be number type");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "GetMainCallId error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "GetMainCallId", NativeGetMainCallId, NativePropertyCallBack);
}

napi_value NapiCallManager::GetSubCallIdList(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "GetSubCallIdList type error, should be number type");
    auto asyncContext = std::make_unique<ListAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "GetSubCallIdList error at listAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "GetSubCallIdList", NativeGetSubCallIdList, NativeListCallBack);
}

napi_value NapiCallManager::GetCallIdListForConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "GetCallIdListForConference type error, should be number type");
    auto asyncContext = std::make_unique<ListAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "GetCallIdListForConference error at listAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "GetCallIdListForConference", NativeGetCallIdListForConference,
        NativeListCallBack);
}

napi_value NapiCallManager::GetCallWaiting(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "GetCallWaiting type error, should be number type");
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "GetCallWaiting error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext.release(), "GetCallWaiting", NativeGetCallWaiting, NativeCallBack);
}

napi_value NapiCallManager::SetCallWaiting(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "SetCallWaiting type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_boolean);
    NAPI_ASSERT(env, matchFlag, "SetCallWaiting type error, should be boolean");
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetCallWaiting error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    napi_get_value_bool(env, argv[ARRAY_INDEX_SECOND], &asyncContext->flag);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext.release(), "SetCallWaiting", NativeSetCallWaiting, NativeCallBack);
}

napi_value NapiCallManager::GetCallRestriction(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "GetCallRestriction type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
    NAPI_ASSERT(env, matchFlag, "GetCallRestriction type error, should be number type");
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "GetCallRestriction error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->type);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext.release(), "GetCallRestriction", NativeGetCallRestriction, NativeCallBack);
}

napi_value NapiCallManager::SetCallRestriction(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "SetCallRestriction type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object);
    NAPI_ASSERT(env, matchFlag, "SetCallRestriction type error, should be object type");
    auto asyncContext = std::make_unique<CallRestrictionAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetCallRestriction error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    asyncContext->resolved = GetRestrictionInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext.release(), "SetCallRestriction", NativeSetCallRestriction, NativeCallBack);
}

napi_value NapiCallManager::GetCallTransferInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "GetCallTransferInfo type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
    NAPI_ASSERT(env, matchFlag, "GetCallTransferInfo type error, should be number type");
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "GetCallTransferInfo error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->type);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext.release(), "GetCallTransferInfo", NativeGetTransferNumber, NativeCallBack);
}

napi_value NapiCallManager::SetCallTransferInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "SetCallTransferInfo type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object);
    NAPI_ASSERT(env, matchFlag, "SetCallTransferInfo type error, should be object type");
    auto asyncContext = std::make_unique<CallTransferAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetCallTransferInfo error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    asyncContext->resolved = GetTransferInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext.release(), "SetCallTransferInfo", NativeSetTransferNumber, NativeCallBack);
}

napi_value NapiCallManager::EnableImsSwitch(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "EnableImsSwitch type error, should be number type");
    auto asyncContext = std::make_unique<ImsSwitchAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "EnableImsSwitch error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "EnableImsSwitch", NativeEnableImsSwitch, NativeVoidCallBack);
}

napi_value NapiCallManager::DisableImsSwitch(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "DisableImsSwitch type error, should be number type");
    auto asyncContext = std::make_unique<ImsSwitchAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "DisableImsSwitch error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "DisableImsSwitch", NativeDisableImsSwitch, NativeVoidCallBack);
}

napi_value NapiCallManager::IsImsSwitchEnabled(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "IsImsSwitchEnabled type error, should be number type");
    auto asyncContext = std::make_unique<ImsSwitchAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "IsImsSwitchEnabled error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "IsImsSwitchEnabled", NativeIsImsSwitchEnabled, NativeBoolCallBack);
}

napi_value NapiCallManager::StartDTMF(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "StartDTMF type error, should be number type");
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "StartDTMF error at supplementAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_SECOND], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "StartDTMF", NativeStartDTMF, NativeVoidCallBack);
}

napi_value NapiCallManager::StopDTMF(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "StopDTMF type error, should be number type");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "StopDTMF error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "StopDTMF", NativeStopDTMF, NativeVoidCallBack);
}

napi_value NapiCallManager::GetCallState(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc < TWO_VALUE_LIMIT, "parameter error!");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "GetCallState error at asyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "GetCallState", NativeGetCallState, NativePropertyCallBack);
}

napi_value NapiCallManager::IsRinging(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc < TWO_VALUE_LIMIT, "parameter error!");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "IsRinging error at asyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "IsRinging", NativeIsRinging, NativeBoolCallBack);
}

napi_value NapiCallManager::HasCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc < TWO_VALUE_LIMIT, "parameter error!");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "HasCall error at asyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "HasCall", NativeHasCall, NativeBoolCallBack);
}

napi_value NapiCallManager::IsNewCallAllowed(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc < TWO_VALUE_LIMIT, "parameter error!");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "IsNewCallAllowed error at asyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "IsNewCallAllowed", NativeIsNewCallAllowed, NativeBoolCallBack);
}

napi_value NapiCallManager::IsInEmergencyCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc < TWO_VALUE_LIMIT, "parameter error!");
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "IsInEmergencyCall error at asyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "IsInEmergencyCall", NativeIsInEmergencyCall, NativeBoolCallBack);
}

napi_value NapiCallManager::IsEmergencyPhoneNumber(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "IsEmergencyPhoneNumber type error, type must be of type string");
    auto asyncContext = std::make_unique<UtilsAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "IsEmergencyPhoneNumber error at utilsAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    asyncContext->slotId = 0;
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            asyncContext->slotId = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_SECOND], "slotId");
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        asyncContext->slotId = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_SECOND], "slotId");
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "IsEmergencyPhoneNumber", NativeIsEmergencyPhoneNumber, NativeBoolCallBack);
}

napi_value NapiCallManager::FormatPhoneNumber(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "FormatPhoneNumber type error, type should be string type");
    auto asyncContext = std::make_unique<UtilsAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "FormatPhoneNumber error at UtilsAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    asyncContext->code = "cn";
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            asyncContext->code = NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_SECOND], "countryCode");
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        asyncContext->code = NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_SECOND], "countryCode");
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "FormatPhoneNumber", NativeFormatPhoneNumber, NativeFormatNumberCallBack);
}

napi_value NapiCallManager::FormatPhoneNumberToE164(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "FormatPhoneNumberToE164 type error, should be string type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_string);
    NAPI_ASSERT(env, matchFlag, "FormatPhoneNumberToE164 type error, should be string type");
    auto asyncContext = std::make_unique<UtilsAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "FormatPhoneNumberToE164 error at utilsAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    char tmpStr[kMaxNumberLen + 1] = {0};
    size_t strLen = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_SECOND], tmpStr, PHONE_NUMBER_MAXIMUM_LIMIT, &strLen);
    std::string tmpCode(tmpStr, strLen);
    asyncContext->code = tmpCode;
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "FormatPhoneNumberToE164", NativeFormatPhoneNumberToE164,
        NativeFormatNumberCallBack);
}

napi_value NapiCallManager::ObserverOn(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if ((argc > TWO_VALUE_LIMIT) ||
        (!NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string))) {
        NapiUtil::ThrowError(
            env, JS_ERROR_TELEPHONY_INVALID_INPUT_PARAMETER, JS_ERROR_TELEPHONY_INVALID_INPUT_PARAMETER_STRING);
        return nullptr;
    }

    if ((argc == TWO_VALUE_LIMIT) &&
        (!NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function))) {
        NapiUtil::ThrowError(
            env, JS_ERROR_TELEPHONY_INVALID_INPUT_PARAMETER, JS_ERROR_TELEPHONY_INVALID_INPUT_PARAMETER_STRING);
        return nullptr;
    }

    if (registerStatus_ == TELEPHONY_ERR_PERMISSION_ERR) {
        NapiUtil::ThrowError(env, JS_ERROR_TELEPHONY_PERMISSION_DENIED, OBSERVER_ON_JS_PERMISSION_ERROR_STRING);
        return nullptr;
    }
    char listenerType[PHONE_NUMBER_MAXIMUM_LIMIT + 1] = { 0 };
    size_t strLength = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], listenerType, PHONE_NUMBER_MAXIMUM_LIMIT, &strLength);
    std::string tmpStr = listenerType;
    TELEPHONY_LOGI("listenerType == %{public}s", tmpStr.c_str());
    EventCallback stateCallback;
    (void)memset_s(&stateCallback, sizeof(EventCallback), 0, sizeof(EventCallback));
    stateCallback.env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &stateCallback.thisVar);
    napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &stateCallback.callbackRef);
    if (tmpStr == "callDetailsChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCallStateCallback(stateCallback);
    } else if (tmpStr == "callEventChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCallEventCallback(stateCallback);
    } else if (tmpStr == "callOttRequest") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCallOttRequestCallback(stateCallback);
    } else if (tmpStr == "callDisconnectedCause") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterDisconnectedCauseCallback(stateCallback);
    } else if (tmpStr == "mmiCodeResult") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterMmiCodeCallback(stateCallback);
    }
    napi_value result = nullptr;
    return result;
}

napi_value NapiCallManager::ObserverOff(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if ((argc > TWO_VALUE_LIMIT) ||
        (!NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string))) {
        NapiUtil::ThrowError(
            env, JS_ERROR_TELEPHONY_INVALID_INPUT_PARAMETER, JS_ERROR_TELEPHONY_INVALID_INPUT_PARAMETER_STRING);
        return nullptr;
    }
    if ((argc == TWO_VALUE_LIMIT) &&
        (!NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function))) {
        NapiUtil::ThrowError(
            env, JS_ERROR_TELEPHONY_INVALID_INPUT_PARAMETER, JS_ERROR_TELEPHONY_INVALID_INPUT_PARAMETER_STRING);
        return nullptr;
    }
    if (registerStatus_ == TELEPHONY_ERR_PERMISSION_ERR) {
        NapiUtil::ThrowError(env, JS_ERROR_TELEPHONY_PERMISSION_DENIED, OBSERVER_OFF_JS_PERMISSION_ERROR_STRING);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        JsError error = NapiUtil::ConverErrorMessageForJs(ERROR_PARAMETER_TYPE_INVALID);
        NapiUtil::ThrowError(env, error.errorCode, error.errorMessage);
        return nullptr;
    }
    char listenerType[PHONE_NUMBER_MAXIMUM_LIMIT + 1] = {0};
    size_t strLength = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], listenerType, PHONE_NUMBER_MAXIMUM_LIMIT, &strLength);
    std::string tmpStr = listenerType;
    TELEPHONY_LOGI("listenerType == %{public}s", tmpStr.c_str());
    if (tmpStr == "callDetailsChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterCallStateCallback();
    } else if (tmpStr == "callEventChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterCallEventCallback();
    } else if (tmpStr == "callOttRequest") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterCallOttRequestCallback();
    } else if (tmpStr == "callDisconnectedCause") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterDisconnectedCauseCallback();
    } else if (tmpStr == "mmiCodeResult") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterMmiCodeCallback();
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "Off", [](napi_env env, void *data) {}, NativeVoidCallBack);
}

napi_value NapiCallManager::SetMuted(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    auto asyncContext = std::make_unique<AudioAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetMuted error at audioAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "SetMuted", NativeSetMuted, NativeVoidCallBack);
}

napi_value NapiCallManager::CancelMuted(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    auto asyncContext = std::make_unique<AudioAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "CancelMuted error at audioAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "CancelMuted", NativeCancelMuted, NativeVoidCallBack);
}

napi_value NapiCallManager::MuteRinger(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    auto asyncContext = std::make_unique<AudioAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MuteRinger error at audioAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "MuteRinger", NativeMuteRinger, NativeVoidCallBack);
}

napi_value NapiCallManager::SetAudioDevice(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "SetAudioDevice type error, should be number type");
    auto asyncContext = std::make_unique<AudioAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetAudioDevice error at audioAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->audioDevice);
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            asyncContext->address =
                NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_SECOND], "bluetoothAddress");
        } else {
            TELEPHONY_LOGE("args error, argv type is not correct");
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        asyncContext->address =
            NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_SECOND], "bluetoothAddress");
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    } else {
        TELEPHONY_LOGE("args error, argv type is not correct");
    }

    return HandleAsyncWork(env, asyncContext.release(), "SetAudioDevice", NativeSetAudioDevice, NativeVoidCallBack);
}

napi_value NapiCallManager::ControlCamera(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "ControlCamera type error, should be string type");

    auto asyncContext = std::make_unique<VideoAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "ControlCamera error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    char tmpStr[kMaxNumberLen + 1] = {0};
    size_t strLen = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], tmpStr, PHONE_NUMBER_MAXIMUM_LIMIT, &strLen);
    std::string tmpCode(tmpStr, strLen);
    asyncContext->cameraId = tmpCode;
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "ControlCamera", NativeControlCamera, NativeVoidCallBack);
}

napi_value NapiCallManager::SetPreviewWindow(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object);
    NAPI_ASSERT(env, matchFlag, "SetPreviewWindow type error, should be object type");

    auto asyncContext = std::make_unique<VideoAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetPreviewWindow error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    asyncContext->x = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "x");
    asyncContext->y = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "y");
    asyncContext->z = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "z");
    asyncContext->width = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "width");
    asyncContext->height = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "height");
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "SetPreviewWindow", NativeSetPreviewWindow, NativeVoidCallBack);
}

napi_value NapiCallManager::SetDisplayWindow(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object);
    NAPI_ASSERT(env, matchFlag, "SetDisplayWindow type error, should be object type");

    auto asyncContext = std::make_unique<VideoAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetDisplayWindow error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    asyncContext->x = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "x");
    asyncContext->y = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "y");
    asyncContext->z = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "z");
    asyncContext->width = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "width");
    asyncContext->height = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "height");
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "SetDisplayWindow", NativeSetDisplayWindow, NativeVoidCallBack);
}

napi_value NapiCallManager::SetCameraZoom(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "SetCameraZoom type error, should be number type");

    auto asyncContext = std::make_unique<VideoAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetCameraZoom error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_double(env, argv[ARRAY_INDEX_FIRST], &asyncContext->zoomRatio);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "SetCameraZoom", NativeSetCameraZoom, NativeVoidCallBack);
}

napi_value NapiCallManager::SetPausePicture(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "SetPausePicture type error, should be string type");
    size_t result = 0;
    auto asyncContext = std::make_unique<VideoAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetPausePicture error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], asyncContext->path, PHONE_NUMBER_MAXIMUM_LIMIT, &result);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "SetPausePicture", NativeSetPausePicture, NativeVoidCallBack);
}

napi_value NapiCallManager::SetDeviceDirection(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "SetDeviceDirection type error, should be number type");
    auto asyncContext = std::make_unique<VideoAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetDeviceDirection error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->rotation);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "SetDeviceDirection", NativeSetDeviceDirection, NativeVoidCallBack);
}

napi_value NapiCallManager::SetCallPreferenceMode(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "SetCallPreferenceMode type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
    NAPI_ASSERT(env, matchFlag, "SetCallPreferenceMode type error, should be number type");
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetCallPreferenceMode error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->mode);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "SetCallPreferenceMode", NativeSetCallPreferenceMode, NativeVoidCallBack);
}

napi_value NapiCallManager::StartRTT(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "StartRTT type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_string);
    NAPI_ASSERT(env, matchFlag, "StartRTT type error, should be string type");
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "StartRTT error at supplementAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    char tmpStr[kMaxNumberLen + 1] = {0};
    size_t strLen = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_SECOND], tmpStr, PHONE_NUMBER_MAXIMUM_LIMIT, &strLen);
    std::string tmpCode(tmpStr, strLen);
    asyncContext->content = tmpCode;
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext.release(), "StartRTT", NativeStartRTT, NativeVoidCallBack);
}

napi_value NapiCallManager::StopRTT(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "StopRTT type error, should be number type");
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "StopRTT error at supplementAsyncContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext.release(), "StopRTT", NativeStopRTT, NativeVoidCallBack);
}

napi_value NapiCallManager::JoinConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "JoinConference type error, should be number type");
    uint32_t arrayLength = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[ARRAY_INDEX_SECOND], &arrayLength));
    NAPI_ASSERT(env, arrayLength > 0, "Parameter cannot be empty");
    auto asyncContext = std::make_unique<ListAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "JoinConference error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    (asyncContext->listResult).clear();

    napi_value napiFormId;
    std::string str = "";
    size_t len = 0;
    char chars[PHONE_NUMBER_MAXIMUM_LIMIT + 1] = { 0 };
    napi_status getStringStatus = napi_invalid_arg;
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_get_element(env, argv[ARRAY_INDEX_SECOND], i, &napiFormId);
        matchFlag = NapiCallManagerUtils::MatchValueType(env, napiFormId, napi_string);
        NAPI_ASSERT(env, matchFlag, "JoinConference type error, should be napi_string type");
        getStringStatus = napi_get_value_string_utf8(env, napiFormId, chars, PHONE_NUMBER_MAXIMUM_LIMIT, &len);
        if (getStringStatus == napi_ok && len > 0) {
            str = std::string(chars, len);
            (asyncContext->listResult).push_back(Str8ToStr16(str));
            TELEPHONY_LOGI("napi_string  %{public}s", str.c_str());
        }
    }

    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "JoinConference", NativeJoinConference, NativeVoidCallBack);
}

napi_value NapiCallManager::UpdateImsCallMode(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "UpdateImsCallMode type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
    NAPI_ASSERT(env, matchFlag, "UpdateImsCallMode type error, should be number type");
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "UpdateImsCallMode error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->type);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(
        env, asyncContext.release(), "UpdateImsCallMode", NativeUpdateImsCallMode, NativeVoidCallBack);
}

napi_value NapiCallManager::ReportOttCallDetailsInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    uint32_t arrayLength = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[ARRAY_INDEX_FIRST], &arrayLength));
    NAPI_ASSERT(env, arrayLength > 0, "Parameter cannot be empty");
    auto asyncContext = std::make_unique<OttCallAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "ReportOttCallDetailsInfo error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    (asyncContext->ottVec).clear();

    napi_value napiFormId;
    OttCallDetailsInfo tmpOttVec;
    (void)memset_s(&tmpOttVec, sizeof(OttCallDetailsInfo), 0, sizeof(OttCallDetailsInfo));
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_get_element(env, argv[ARRAY_INDEX_FIRST], i, &napiFormId);
        bool matchFlag = NapiCallManagerUtils::MatchValueType(env, napiFormId, napi_object);
        NAPI_ASSERT(env, matchFlag, "ReportOttCallDetailsInfo type error, should be napi_object type");
        std::string tmpStr = NapiCallManagerUtils::GetStringProperty(env, napiFormId, "phoneNumber");
        if (tmpStr.length() > static_cast<size_t>(kMaxNumberLen)) {
            TELEPHONY_LOGE("Number out of limit!");
            return (napi_value) nullptr;
        }
        if (memcpy_s(tmpOttVec.phoneNum, kMaxNumberLen, tmpStr.c_str(), tmpStr.length()) != EOK) {
            return (napi_value) nullptr;
        }
        tmpStr = NapiCallManagerUtils::GetStringProperty(env, napiFormId, "bundleName");
        if (tmpStr.length() > static_cast<size_t>(kMaxNumberLen)) {
            TELEPHONY_LOGE("Number out of limit!");
            return (napi_value) nullptr;
        }
        if (memcpy_s(tmpOttVec.bundleName, kMaxNumberLen, tmpStr.c_str(), tmpStr.length()) != EOK) {
            return (napi_value) nullptr;
        }
        int32_t tmpValue = NapiCallManagerUtils::GetIntProperty(env, napiFormId, "videoState");
        tmpOttVec.videoState = static_cast<VideoStateType>(tmpValue);
        tmpValue = NapiCallManagerUtils::GetIntProperty(env, napiFormId, "callState");
        tmpOttVec.callState = static_cast<TelCallState>(tmpValue);
        (asyncContext->ottVec).push_back(tmpOttVec);
    }

    return HandleAsyncWork(
        env, asyncContext.release(), "ReportOttCallDetailsInfo", NativeReportOttCallDetailsInfo, NativeVoidCallBack);
}

napi_value NapiCallManager::ReportOttCallEventInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object);
    NAPI_ASSERT(env, matchFlag, "ReportOttCallEventInfo type error, should be object type");
    auto asyncContext = std::make_unique<OttEventAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "ReportOttCallEventInfo error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    (void)memset_s(&asyncContext->eventInfo, sizeof(OttCallEventInfo), 0, sizeof(OttCallEventInfo));
    int32_t eventId = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "eventId");
    std::string tmpStr = NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_FIRST], "bundleName");
    if (tmpStr.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return (napi_value) nullptr;
    }
    asyncContext->eventInfo.ottCallEventId = static_cast<OttCallEventId>(eventId);
    if (memcpy_s(asyncContext->eventInfo.bundleName, kMaxNumberLen, tmpStr.c_str(), tmpStr.length()) != EOK) {
        return (napi_value) nullptr;
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "ReportOttCallEventInfo", NativeReportOttCallEventInfo, NativeVoidCallBack);
}

napi_value NapiCallManager::HasVoiceCapability(napi_env env, napi_callback_info)
{
    TELEPHONY_LOGI("napi_call HasVoiceCapability");
    napi_value result = nullptr;
    napi_get_boolean(env, DelayedSingleton<CallManagerClient>::GetInstance()->HasVoiceCapability(), &result);
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
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_int32(env, asyncContext->resolved, &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else {
            if (asyncContext->errorCode == SLOT_ID_INVALID) {
                napi_reject_deferred(env, asyncContext->deferred,
                    NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, "slotId is invalid", SLOT_ID_INVALID));
            } else {
                std::string errTip = std::to_string(asyncContext->resolved);
                napi_reject_deferred(
                    env, asyncContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
            }
        }
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_int32(env, asyncContext->resolved, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            if (asyncContext->errorCode == SLOT_ID_INVALID) {
                callbackValue[ARRAY_INDEX_FIRST] =
                    NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, "slotId is invalid", SLOT_ID_INVALID);
                callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
            } else {
                std::string errTip = std::to_string(asyncContext->resolved);
                callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
                callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
            }
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
    asyncContext = nullptr;
}

void NapiCallManager::NativeDialCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, true, &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else {
            if (asyncContext->errorCode == SLOT_ID_INVALID) {
                napi_reject_deferred(env, asyncContext->deferred,
                    NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, "slotId is invalid", SLOT_ID_INVALID));
            } else {
                std::string errTip = std::to_string(asyncContext->resolved);
                napi_reject_deferred(
                    env, asyncContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
            }
        }
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, true, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            if (asyncContext->errorCode == SLOT_ID_INVALID) {
                callbackValue[ARRAY_INDEX_FIRST] =
                    NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, "slotId is invalid", SLOT_ID_INVALID);
                callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
            } else {
                std::string errTip = std::to_string(asyncContext->resolved);
                callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
                callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
            }
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
    asyncContext = nullptr;
}

void NapiCallManager::NativeVoidCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_status ret = napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
            TELEPHONY_LOGI("promise successful result = %{public}d", ret);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(asyncContext->resolved);
            napi_status ret = napi_reject_deferred(env, asyncContext->deferred,
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(
                    env, error.errorMessage.c_str(), error.errorCode));
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        }
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_null(env, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(asyncContext->resolved);
            callbackValue[ARRAY_INDEX_FIRST] =
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage.c_str(), error.errorCode);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_status ret = napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        TELEPHONY_LOGI("callback result = %{public}d", ret);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
    asyncContext = nullptr;
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
        napi_create_int32(env, asyncContext->resolved, &promiseValue);
        napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
        napi_create_int32(env, asyncContext->resolved, &callbackValue[ARRAY_INDEX_SECOND]);
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
    asyncContext = nullptr;
}

void NapiCallManager::NativeBoolCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->resolved == BOOL_VALUE_IS_TRUE) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, true, &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, false, &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else if (asyncContext->errorCode == SLOT_ID_INVALID) {
            napi_reject_deferred(env, asyncContext->deferred,
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, "slotId is invalid", SLOT_ID_INVALID));
        } else {
            std::string errTip = std::to_string(asyncContext->errorCode);
            napi_reject_deferred(
                env, asyncContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (asyncContext->resolved == BOOL_VALUE_IS_TRUE) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, true, &callbackValue[ARRAY_INDEX_SECOND]);
        } else if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, false, &callbackValue[ARRAY_INDEX_SECOND]);
        } else if (asyncContext->errorCode == SLOT_ID_INVALID) {
            callbackValue[ARRAY_INDEX_FIRST] =
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, "slotId is invalid", SLOT_ID_INVALID);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        } else {
            std::string errTip = std::to_string(asyncContext->errorCode);
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
    asyncContext = nullptr;
}

void NapiCallManager::NativeFormatNumberCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (UtilsAsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_string_utf8(env, Str16ToStr8(asyncContext->formatNumber).data(),
                asyncContext->formatNumber.length(), &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(asyncContext->resolved);
            napi_reject_deferred(env, asyncContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_string_utf8(env, Str16ToStr8(asyncContext->formatNumber).data(),
                asyncContext->formatNumber.length(), &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(asyncContext->resolved);
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
    asyncContext = nullptr;
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
        int32_t i = 0;
        for (; it != asyncContext->listResult.end(); ++it) {
            napi_value info = nullptr;
            napi_create_string_utf8(env, Str16ToStr8(*it).data(), (*it).length(), &info);
            napi_set_element(env, promiseValue, i, info);
            ++i;
        }
        napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
        napi_create_array(env, &callbackValue[ARRAY_INDEX_SECOND]);
        std::vector<std::u16string>::iterator it = asyncContext->listResult.begin();
        int32_t i = 0;
        for (; it != asyncContext->listResult.end(); ++it) {
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
    asyncContext = nullptr;
}

void NapiCallManager::GetDialInfo(napi_env env, napi_value objValue, DialAsyncContext &asyncContext)
{
    asyncContext.accountId = NapiCallManagerUtils::GetIntProperty(env, objValue, "accountId");
    asyncContext.videoState = NapiCallManagerUtils::GetIntProperty(env, objValue, "videoState");
    asyncContext.dialScene = NapiCallManagerUtils::GetIntProperty(env, objValue, "dialScene");
    asyncContext.dialType = NapiCallManagerUtils::GetIntProperty(env, objValue, "dialType");
}

void NapiCallManager::GetSmsInfo(napi_env env, napi_value objValue, RejectAsyncContext &asyncContext)
{
    asyncContext.isSendSms = true;
    asyncContext.messageContent = NapiCallManagerUtils::GetStringProperty(env, objValue, "messageContent");
}

int32_t NapiCallManager::GetRestrictionInfo(
    napi_env env, napi_value objValue, CallRestrictionAsyncContext &asyncContext)
{
    int32_t type = NapiCallManagerUtils::GetIntProperty(env, objValue, "type");
    int32_t mode = NapiCallManagerUtils::GetIntProperty(env, objValue, "mode");
    std::string pw = NapiCallManagerUtils::GetStringProperty(env, objValue, "password");
    if (pw.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    if (memcpy_s(asyncContext.info.password, kMaxNumberLen, pw.c_str(), pw.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    asyncContext.info.fac = static_cast<CallRestrictionType>(type);
    asyncContext.info.mode = static_cast<CallRestrictionMode>(mode);
    TELEPHONY_LOGI("GetRestrictionInfo: type = %{public}d, mode = %{public}d",
        asyncContext.info.fac, asyncContext.info.mode);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallManager::GetTransferInfo(napi_env env, napi_value objValue, CallTransferAsyncContext &asyncContext)
{
    int32_t type = NapiCallManagerUtils::GetIntProperty(env, objValue, "type");
    int32_t settingType = NapiCallManagerUtils::GetIntProperty(env, objValue, "settingType");
    std::string transferNum = NapiCallManagerUtils::GetStringProperty(env, objValue, "transferNum");
    if (transferNum.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    if (memcpy_s(asyncContext.info.transferNum, kMaxNumberLen, transferNum.c_str(), transferNum.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    asyncContext.info.settingType = static_cast<CallTransferSettingType>(settingType);
    asyncContext.info.type = static_cast<CallTransferType>(type);
    TELEPHONY_LOGI("GetTransferInfo: type = %{public}d, settingType = %{public}d, transferNum = %{public}s",
        asyncContext.info.type, asyncContext.info.settingType, asyncContext.info.transferNum);
    return TELEPHONY_SUCCESS;
}

static inline bool IsValidSlotId(int32_t slotId)
{
    return ((slotId >= DEFAULT_SIM_SLOT_ID) && (slotId < SIM_SLOT_COUNT));
}

void NapiCallManager::NativeDialCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (DialAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->accountId)) {
        TELEPHONY_LOGE("NativeDialCall slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    std::string phoneNumber(asyncContext->number, asyncContext->numberLen);
    OHOS::AppExecFwk::PacMap dialInfo;
    dialInfo.PutIntValue("accountId", asyncContext->accountId);
    dialInfo.PutIntValue("videoState", asyncContext->videoState);
    dialInfo.PutIntValue("dialScene", asyncContext->dialScene);
    dialInfo.PutIntValue("dialType", asyncContext->dialType);
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->DialCall(Str8ToStr16(phoneNumber), dialInfo);
}

void NapiCallManager::NativeMakeCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    std::string phoneNumber(asyncContext->number, asyncContext->numberLen);
    AAFwk::Want want;
    AppExecFwk::ElementName element("", "com.ohos.contacts", "com.ohos.contacts.MainAbility");
    want.SetElement(element);
    AAFwk::WantParams wantParams;
    wantParams.SetParam("phoneNumber", AAFwk::String::Box(phoneNumber));
    wantParams.SetParam("pageFlag", AAFwk::String::Box("page_flag_edit_before_calling"));
    want.SetParams(wantParams);
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (err != ERR_OK) {
        TELEPHONY_LOGE("Fail to make call, err:%{public}d", err);
        asyncContext->resolved = TELEPHONY_ERR_FAIL;
        return;
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
}

void NapiCallManager::NativeAnswerCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AnswerAsyncContext *)data;
    int32_t ret =
        DelayedSingleton<CallManagerClient>::GetInstance()->AnswerCall(asyncContext->callId, asyncContext->videoState);
    asyncContext->resolved = ret;
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
    asyncContext->resolved = ret;
}

void NapiCallManager::NativeHangUpCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->HangUpCall(asyncContext->callId);
}

void NapiCallManager::NativeHoldCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->HoldCall(asyncContext->callId);
}

void NapiCallManager::NativeUnHoldCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->UnHoldCall(asyncContext->callId);
}

void NapiCallManager::NativeSwitchCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->SwitchCall(asyncContext->callId);
}

void NapiCallManager::NativeCombineConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->CombineConference(asyncContext->callId);
}

void NapiCallManager::NativeSeparateConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->SeparateConference(asyncContext->callId);
}

void NapiCallManager::NativeGetMainCallId(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->GetMainCallId(asyncContext->callId);
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

/**
 * Register a callback to NapiCallAbilityCallback. When call_manager has data returned,
 * NapiCallAbilityCallback will notify JavaScript via the registered callback.
 * The whole process is asynchronous.
 */
void NapiCallManager::NativeGetCallWaiting(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeGetCallWaiting slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetWaitingCallback(infoListener);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetWaitingCallback failed!");
        return;
    }
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->GetCallWaiting(asyncContext->slotId);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetWaitingCallback();
        TELEPHONY_LOGE("GetCallWaiting failed!");
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
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeSetCallWaiting slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }

    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetWaitingCallback(infoListener);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetWaitingCallback failed!");
        return;
    }
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetCallWaiting(asyncContext->slotId, asyncContext->flag);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterSetWaitingCallback();
        TELEPHONY_LOGE("SetCallWaiting failed!");
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
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeGetCallRestriction slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetRestrictionCallback(infoListener);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetRestrictionCallback failed!");
        return;
    }
    CallRestrictionType type = static_cast<CallRestrictionType>(asyncContext->type);
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetCallRestriction(asyncContext->slotId, type);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetRestrictionCallback();
        TELEPHONY_LOGE("GetCallRestriction failed!");
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
    auto asyncContext = (CallRestrictionAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeSetCallRestriction slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Data acquisition failure!");
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetRestrictionCallback(infoListener);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetRestrictionCallback failed!");
        return;
    }
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->SetCallRestriction(
        asyncContext->slotId, asyncContext->info);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterSetRestrictionCallback();
        TELEPHONY_LOGE("SetCallRestriction failed!");
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
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeGetTransferNumber slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetTransferCallback(infoListener);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetTransferCallback failed!");
        return;
    }
    CallTransferType type = static_cast<CallTransferType>(asyncContext->type);
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetCallTransferInfo(asyncContext->slotId, type);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetTransferCallback();
        TELEPHONY_LOGE("GetCallTransferInfo failed!");
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
    auto asyncContext = (CallTransferAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeSetTransferNumber slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }

    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Data acquisition failure!");
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetTransferCallback(infoListener);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetTransferCallback failed!");
        return;
    }
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->SetCallTransferInfo(
        asyncContext->slotId, asyncContext->info);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterSetTransferCallback();
        TELEPHONY_LOGE("SetCallTransferInfo failed!");
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeEnableImsSwitch(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (ImsSwitchAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeEnableImsSwitch slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->EnableImsSwitch(asyncContext->slotId);
}

void NapiCallManager::NativeDisableImsSwitch(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (ImsSwitchAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeDisableImsSwitch slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->DisableImsSwitch(asyncContext->slotId);
}

void NapiCallManager::NativeIsImsSwitchEnabled(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (ImsSwitchAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeIsImsSwitchEnabled slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->IsImsSwitchEnabled(
        asyncContext->slotId, asyncContext->enabled);
    if (asyncContext->enabled) {
        asyncContext->resolved = BOOL_VALUE_IS_TRUE;
    }
}

void NapiCallManager::NativeStartDTMF(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->numberLen < TWO_VALUE_LIMIT) {
        asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->StartDtmf(
            asyncContext->callId, asyncContext->number[ARRAY_INDEX_FIRST]);
    } else {
        asyncContext->resolved = CALL_ERR_DTMF_EXCEED_LIMIT;
    }
}

void NapiCallManager::NativeStopDTMF(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->StopDtmf(asyncContext->callId);
}

void NapiCallManager::NativeGetCallState(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->GetCallState();
}

void NapiCallManager::NativeIsRinging(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->IsRinging();
}

void NapiCallManager::NativeHasCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->HasCall();
}

void NapiCallManager::NativeIsNewCallAllowed(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->IsNewCallAllowed();
}

void NapiCallManager::NativeIsInEmergencyCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->IsInEmergencyCall();
}

void NapiCallManager::NativeIsEmergencyPhoneNumber(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (UtilsAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeIsEmergencyPhoneNumber slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    std::string tmpPhoneNumber(asyncContext->number, asyncContext->numberLen);
    std::u16string phoneNumber = Str8ToStr16(tmpPhoneNumber);
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->IsEmergencyPhoneNumber(
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
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
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
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumberToE164(
        phoneNumber, countryCode, asyncContext->formatNumber);
}

void NapiCallManager::NativeSetMuted(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AudioAsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->SetMuted(true);
}

void NapiCallManager::NativeCancelMuted(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AudioAsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->SetMuted(false);
}

void NapiCallManager::NativeMuteRinger(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AudioAsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->MuteRinger();
}

void NapiCallManager::NativeSetAudioDevice(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    AudioDevice type;
    auto asyncContext = (AudioAsyncContext *)data;
    type = static_cast<AudioDevice>(asyncContext->audioDevice);
    // For interface compatibility, when AudioDevice::DEVICE_MIC is deleted, this code block should also be deleted
    // When the parameter passed is DEVICE_MIC, point to DEVICE_EARPIECE
    int32_t DEVICE_MIC = 4;
    if (static_cast<int32_t>(type) == DEVICE_MIC) {
        type = AudioDevice::DEVICE_EARPIECE;
    }
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetAudioDevice(type, asyncContext->address);
}

void NapiCallManager::NativeControlCamera(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->ControlCamera(Str8ToStr16(asyncContext->cameraId));
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
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->SetPreviewWindow(window);
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
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->SetDisplayWindow(window);
}

void NapiCallManager::NativeSetCameraZoom(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->SetCameraZoom(asyncContext->zoomRatio);
}

void NapiCallManager::NativeSetPausePicture(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetPausePicture(Str8ToStr16(asyncContext->path));
}

void NapiCallManager::NativeSetDeviceDirection(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetDeviceDirection(asyncContext->rotation);
}

void NapiCallManager::NativeSetCallPreferenceMode(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->SetCallPreferenceMode(
        asyncContext->callId, asyncContext->mode);
}

void NapiCallManager::NativeStartRTT(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterStartRttCallback(infoListener);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterStartRttCallback failed!");
        return;
    }

    std::u16string msg = Str8ToStr16(asyncContext->content);
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->StartRtt(asyncContext->callId, msg);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterStartRttCallback();
        TELEPHONY_LOGE("StartRtt failed!");
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeStopRTT(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterStopRttCallback(infoListener);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterStopRttCallback failed!");
        return;
    }
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->StopRtt(asyncContext->callId);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterStopRttCallback();
        TELEPHONY_LOGE("StopRtt failed!");
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeJoinConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (ListAsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->JoinConference(
        asyncContext->callId, asyncContext->listResult);
}

void NapiCallManager::NativeUpdateImsCallMode(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterUpdateCallMediaModeCallback(infoListener);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterUpdateCallMediaModeCallback failed!");
        return;
    }
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->UpdateImsCallMode(
        asyncContext->callId, (ImsCallMode)asyncContext->type);
    if (asyncContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterUpdateCallMediaModeCallback();
        TELEPHONY_LOGE("UnRegisterUpdateCallMediaModeCallback failed!");
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeReportOttCallDetailsInfo(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (OttCallAsyncContext *)data;
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->ReportOttCallDetailsInfo(asyncContext->ottVec);
}

void NapiCallManager::NativeReportOttCallEventInfo(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (OttEventAsyncContext *)data;
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->ReportOttCallEventInfo(asyncContext->eventInfo);
}

void NapiCallManager::RegisterCallBack()
{
    if (registerStatus_ == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("you are already registered!");
        return;
    }
    std::unique_ptr<NapiCallManagerCallback> callbackPtr = std::make_unique<NapiCallManagerCallback>();
    if (callbackPtr == nullptr) {
        TELEPHONY_LOGE("make_unique NapiCallManagerCallback failed!");
        return;
    }
    registerStatus_ = DelayedSingleton<CallManagerClient>::GetInstance()->RegisterCallBack(std::move(callbackPtr));
    if (registerStatus_ != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterCallBack failed!");
        return;
    }
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
    napi_value resource = NapiCallManagerUtils::CreateUndefined(env);
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
