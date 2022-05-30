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

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "napi_call_manager_types.h"
#include "napi_call_ability_callback.h"
#include "call_manager_client.h"
#include "napi_call_manager_utils.h"

namespace OHOS {
namespace Telephony {
bool NapiCallManager::registerStatus_ = false;

NapiCallManager::NapiCallManager() {}

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
        DECLARE_NAPI_FUNCTION("enableLteEnhanceMode", EnableLteEnhanceMode),
        DECLARE_NAPI_FUNCTION("disableLteEnhanceMode", DisableLteEnhanceMode),
        DECLARE_NAPI_FUNCTION("isLteEnhanceModeEnabled", IsLteEnhanceModeEnabled),
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
    napi_define_class(env, "CallStateToApp", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallStateToApp", result);
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
    return exports;
}

napi_value NapiCallManager::DialCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<DialAsyncContext>()).release();
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
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
    return HandleAsyncWork(env, asyncContext, "DialCall", NativeDialCall, NativeDialCallBack);
}

napi_value NapiCallManager::MakeCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    NAPI_ASSERT(env, argc <= TWO_VALUE_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "MakeCall", NativeMakeCall, NativeVoidCallBack);
}

napi_value NapiCallManager::AnswerCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AnswerAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number)) {
            asyncContext->videoState =
                NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_SECOND], "videoState");
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT) {
        asyncContext->videoState =
            NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_SECOND], "videoState");
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "AnswerCall", NativeAnswerCall, NativeVoidCallBack);
}

napi_value NapiCallManager::RejectCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, FOUR_VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < FOUR_VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<RejectAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    asyncContext->isSendSms = false;
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            GetSmsInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "GetCallWaiting", NativeGetCallWaiting, NativeCallBack);
}

napi_value NapiCallManager::SetCallWaiting(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_boolean);
    NAPI_ASSERT(env, matchFlag, "Type error, should be boolean");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    napi_get_value_bool(env, argv[ARRAY_INDEX_SECOND], &asyncContext->flag);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "SetCallWaiting", NativeSetCallWaiting, NativeCallBack);
}

napi_value NapiCallManager::GetCallRestriction(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->type);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "GetCallRestriction", NativeGetCallRestriction, NativeCallBack);
}

napi_value NapiCallManager::SetCallRestriction(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object);
    NAPI_ASSERT(env, matchFlag, "Type error, should be object type");
    auto asyncContext = (std::make_unique<CallRestrictionAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    asyncContext->result = GetRestrictionInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "SetCallRestriction", NativeSetCallRestriction, NativeCallBack);
}

napi_value NapiCallManager::GetCallTransferInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->type);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "GetCallTransferInfo", NativeGetTransferNumber, NativeCallBack);
}

napi_value NapiCallManager::SetCallTransferInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object);
    NAPI_ASSERT(env, matchFlag, "Type error, should be object type");
    auto asyncContext = (std::make_unique<CallTransferAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    asyncContext->result = GetTransferInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "SetCallTransferInfo", NativeSetTransferNumber, NativeCallBack);
}

napi_value NapiCallManager::EnableImsSwitch(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    asyncContext->flag = true;
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "EnableImsSwitch", NativeEnableImsSwitch, NativeCallBack);
}

napi_value NapiCallManager::DisableImsSwitch(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    asyncContext->flag = false;
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "DisableImsSwitch", NativeDisableImsSwitch, NativeCallBack);
}

napi_value NapiCallManager::IsImsSwitchEnabled(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "IsImsSwitchEnabled", NativeIsImsSwitchEnabled, NativeCallBack);
}

napi_value NapiCallManager::StartDTMF(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "StopDTMF", NativeStopDTMF, NativeVoidCallBack);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<UtilsAsyncContext>()).release();
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
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
        env, asyncContext, "IsEmergencyPhoneNumber", NativeIsEmergencyPhoneNumber, NativeBoolCallBack);
}

napi_value NapiCallManager::FormatPhoneNumber(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<UtilsAsyncContext>()).release();
    asyncContext->code = "cn";
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            asyncContext->code =
                NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_SECOND], "countryCode");
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        asyncContext->code = NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_SECOND], "countryCode");
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext, "FormatPhoneNumber", NativeFormatPhoneNumber, NativeFormatNumberCallBack);
}

napi_value NapiCallManager::FormatPhoneNumberToE164(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<UtilsAsyncContext>()).release();
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(asyncContext->numberLen));
    char tmpStr[kMaxNumberLen + 1] = {0};
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    char listenerType[PHONE_NUMBER_MAXIMUM_LIMIT + 1] = {0};
    size_t strLength = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], listenerType, PHONE_NUMBER_MAXIMUM_LIMIT, &strLength);
    std::string tmpStr = listenerType;
    TELEPHONY_LOGI("listenerType == %{public}s", tmpStr.c_str());
    if (tmpStr == "callDetailsChange") {
        EventCallback callStateListener;
        (void)memset_s(&callStateListener, sizeof(EventCallback), 0, sizeof(EventCallback));
        callStateListener.env = env;
        napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &callStateListener.thisVar);
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &callStateListener.callbackRef);
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCallStateCallback(callStateListener);
    } else if (tmpStr == "callEventChange") {
        EventCallback callEventCallback;
        (void)memset_s(&callEventCallback, sizeof(EventCallback), 0, sizeof(EventCallback));
        callEventCallback.env = env;
        napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &callEventCallback.thisVar);
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &callEventCallback.callbackRef);
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCallEventCallback(callEventCallback);
    } else if (tmpStr == "callOttRequest") {
        EventCallback callOttCallback;
        (void)memset_s(&callOttCallback, sizeof(EventCallback), 0, sizeof(EventCallback));
        callOttCallback.env = env;
        napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &callOttCallback.thisVar);
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &callOttCallback.callbackRef);
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCallOttRequestCallback(callOttCallback);
    } else if (tmpStr == "callDisconnectedCause") {
        EventCallback causeCallback;
        (void)memset_s(&causeCallback, sizeof(EventCallback), 0, sizeof(EventCallback));
        causeCallback.env = env;
        napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &causeCallback.thisVar);
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &causeCallback.callbackRef);
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterDisconnectedCauseCallback(causeCallback);
    } else if (tmpStr == "mmiCodeResult") {
        EventCallback mmiCodeResultListener;
        (void)memset_s(&mmiCodeResultListener, sizeof(EventCallback), 0, sizeof(EventCallback));
        mmiCodeResultListener.env = env;
        napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &mmiCodeResultListener.thisVar);
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &mmiCodeResultListener.callbackRef);
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterMmiCodeCallback(mmiCodeResultListener);
    }
    napi_value result = nullptr;
    return result;
}

napi_value NapiCallManager::ObserverOff(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<AsyncContext>()).release();
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");

    auto asyncContext = (std::make_unique<VideoAsyncContext>()).release();
    char tmpStr[kMaxNumberLen + 1] = {0};
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object);
    NAPI_ASSERT(env, matchFlag, "Type error, should be object type");

    auto asyncContext = (std::make_unique<VideoAsyncContext>()).release();
    asyncContext->x = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "x");
    asyncContext->y = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "y");
    asyncContext->z = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "z");
    asyncContext->width = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "width");
    asyncContext->height = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "height");
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SetPreviewWindow", NativeSetPreviewWindow, NativeVoidCallBack);
}

napi_value NapiCallManager::SetDisplayWindow(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object);
    NAPI_ASSERT(env, matchFlag, "Type error, should be object type");

    auto asyncContext = (std::make_unique<VideoAsyncContext>()).release();
    asyncContext->x = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "x");
    asyncContext->y = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "y");
    asyncContext->z = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "z");
    asyncContext->width = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "width");
    asyncContext->height = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "height");
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext, "SetDisplayWindow", NativeSetDisplayWindow, NativeVoidCallBack);
}

napi_value NapiCallManager::SetCameraZoom(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
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
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
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

napi_value NapiCallManager::StartRTT(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_string);
    NAPI_ASSERT(env, matchFlag, "Type error, should be string type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
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
    return HandleAsyncWork(env, asyncContext, "StartRTT", NativeStartRTT, NativeVoidCallBack);
}

napi_value NapiCallManager::StopRTT(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "StopRTT", NativeStopRTT, NativeVoidCallBack);
}

napi_value NapiCallManager::JoinConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    uint32_t arrayLength = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[ARRAY_INDEX_SECOND], &arrayLength));
    NAPI_ASSERT(env, arrayLength > 0, "Parameter cannot be empty");
    auto asyncContext = (std::make_unique<ListAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    (asyncContext->listResult).clear();

    napi_value napiFormId;
    std::string str = "";
    size_t len = 0;
    char chars[PHONE_NUMBER_MAXIMUM_LIMIT + 1] = {0};
    napi_status getStringStatus = napi_invalid_arg;
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_get_element(env, argv[ARRAY_INDEX_SECOND], i, &napiFormId);
        matchFlag = NapiCallManagerUtils::MatchValueType(env, napiFormId, napi_string);
        NAPI_ASSERT(env, matchFlag, "Type error, should be napi_string type");
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
    return HandleAsyncWork(env, asyncContext, "JoinConference", NativeJoinConference, NativeVoidCallBack);
}

napi_value NapiCallManager::UpdateImsCallMode(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->type);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext, "UpdateImsCallMode", NativeUpdateImsCallMode, NativeVoidCallBack);
}

napi_value NapiCallManager::EnableLteEnhanceMode(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(
        env, asyncContext, "EnableLteEnhanceMode", NativeEnableLteEnhanceMode, NativeVoidCallBack);
}

napi_value NapiCallManager::DisableLteEnhanceMode(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(
        env, asyncContext, "DisableLteEnhanceMode", NativeDisableLteEnhanceMode, NativeVoidCallBack);
}

napi_value NapiCallManager::IsLteEnhanceModeEnabled(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number);
    NAPI_ASSERT(env, matchFlag, "Type error, should be number type");
    auto asyncContext = (std::make_unique<SupplementAsyncContext>()).release();
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(
        env, asyncContext, "IsLteEnhanceModeEnabled", NativeIsLteEnhanceModeEnabled, NativeDialCallBack);
}

napi_value NapiCallManager::ReportOttCallDetailsInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    uint32_t arrayLength = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[ARRAY_INDEX_FIRST], &arrayLength));
    NAPI_ASSERT(env, arrayLength > 0, "Parameter cannot be empty");
    auto asyncContext = (std::make_unique<OttCallAsyncContext>()).release();
    (asyncContext->ottVec).clear();

    napi_value napiFormId;
    OttCallDetailsInfo tmpOttVec;
    (void)memset_s(&tmpOttVec, sizeof(OttCallDetailsInfo), 0, sizeof(OttCallDetailsInfo));
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_get_element(env, argv[ARRAY_INDEX_FIRST], i, &napiFormId);
        bool matchFlag = NapiCallManagerUtils::MatchValueType(env, napiFormId, napi_object);
        NAPI_ASSERT(env, matchFlag, "Type error, should be napi_object type");
        std::string tmpStr = NapiCallManagerUtils::GetStringProperty(env, napiFormId, "phoneNumber");
        if (memcpy_s(tmpOttVec.phoneNum, kMaxNumberLen, tmpStr.c_str(), tmpStr.length()) != EOK) {
            return (napi_value) nullptr;
        }
        tmpStr = NapiCallManagerUtils::GetStringProperty(env, napiFormId, "bundleName");
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
        env, asyncContext, "GetLteEnhanceMode", NativeReportOttCallDetailsInfo, NativeVoidCallBack);
}

napi_value NapiCallManager::ReportOttCallEventInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc < VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object);
    NAPI_ASSERT(env, matchFlag, "Type error, should be object type");
    auto asyncContext = (std::make_unique<OttEventAsyncContext>()).release();
    (void)memset_s(&asyncContext->eventInfo, sizeof(OttCallEventInfo), 0, sizeof(OttCallEventInfo));
    int32_t eventId = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "eventId");
    std::string tmpStr = NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_FIRST], "bundleName");
    asyncContext->eventInfo.ottCallEventId = static_cast<OttCallEventId>(eventId);
    if (memcpy_s(asyncContext->eventInfo.bundleName, kMaxNumberLen, tmpStr.c_str(), tmpStr.length()) != EOK) {
        return (napi_value) nullptr;
    }
    return HandleAsyncWork(
        env, asyncContext, "ReportOttCallEventInfo", NativeReportOttCallEventInfo, NativeVoidCallBack);
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
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_int32(env, asyncContext->result, &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(asyncContext->result);
            napi_reject_deferred(
                env, asyncContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_int32(env, asyncContext->result, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(asyncContext->result);
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
            napi_reject_deferred(
                env, asyncContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, true, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(asyncContext->result);
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
                env, asyncContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        }
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_null(env, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(asyncContext->result);
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
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
        napi_create_int32(env, asyncContext->result, &promiseValue);
        napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
        napi_create_int32(env, asyncContext->result, &callbackValue[ARRAY_INDEX_SECOND]);
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
            napi_reject_deferred(
                env, asyncContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        if (asyncContext->result != BOOL_VALUE_IS_FALSE) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, true, &callbackValue[ARRAY_INDEX_SECOND]);
        } else if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, false, &callbackValue[ARRAY_INDEX_SECOND]);
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
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_string_utf8(env, Str16ToStr8(asyncContext->formatNumber).data(),
                asyncContext->formatNumber.length(), &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(asyncContext->result);
            napi_reject_deferred(
                env, asyncContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = {0};
        if (asyncContext->result == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_string_utf8(env, Str16ToStr8(asyncContext->formatNumber).data(),
                asyncContext->formatNumber.length(), &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(asyncContext->result);
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
    if (transferNum.length() > kMaxNumberLen) {
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

void NapiCallManager::NativeMakeCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    std::string phoneNumber(asyncContext->number, asyncContext->numberLen);
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->MakeCall(phoneNumber);
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
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetWaitingCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetWaitingCallback failed!");
        return;
    }
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->GetCallWaiting(asyncContext->slotId);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
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
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetWaitingCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetWaitingCallback failed!");
        return;
    }
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetCallWaiting(
        asyncContext->slotId, asyncContext->flag);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
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
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetRestrictionCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetRestrictionCallback failed!");
        return;
    }
    CallRestrictionType type = static_cast<CallRestrictionType>(asyncContext->type);
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetCallRestriction(asyncContext->slotId, type);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
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
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Data acquisition failure!");
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetRestrictionCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetRestrictionCallback failed!");
        return;
    }
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetCallRestriction(
        asyncContext->slotId, asyncContext->info);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
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
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetTransferCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetTransferCallback failed!");
        return;
    }
    CallTransferType type = static_cast<CallTransferType>(asyncContext->type);
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetCallTransferInfo(asyncContext->slotId, type);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
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
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Data acquisition failure!");
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetTransferCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetTransferCallback failed!");
        return;
    }
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetCallTransferInfo(
        asyncContext->slotId, asyncContext->info);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
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
    auto asyncContext = (SupplementAsyncContext *)data;
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterEnableVolteCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterEnableVolteCallback failed!");
        return;
    }
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->EnableImsSwitch(asyncContext->slotId);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterEnableVolteCallback();
        TELEPHONY_LOGE("EnableImsSwitch failed!");
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeDisableImsSwitch(napi_env env, void *data)
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
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterDisableVolteCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterDisableVolteCallback failed!");
        return;
    }
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->DisableImsSwitch(asyncContext->slotId);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterDisableVolteCallback();
        TELEPHONY_LOGE("DisableImsSwitch failed!");
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeIsImsSwitchEnabled(napi_env env, void *data)
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
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetVolteCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetVolteCallback failed!");
        return;
    }
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->IsImsSwitchEnabled(asyncContext->slotId);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetVolteCallback();
        TELEPHONY_LOGE("IsImsSwitchEnabled failed!");
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
        asyncContext->result = CALL_ERR_DTMF_EXCEED_LIMIT;
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
    
    // For interface compatibility, when AudioDevice::DEVICE_MIC is deleted, this code block should also be deleted
    // When the parameter passed is DEVICE_MIC, point to DEVICE_EARPIECE
    int32_t DEVICE_MIC = 4;
    if (static_cast<int32_t>(type) == DEVICE_MIC) {
        type = AudioDevice::DEVICE_EARPIECE;
    }
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->SetAudioDevice(type);
}

void NapiCallManager::NativeControlCamera(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->result =
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
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterStartRttCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterStartRttCallback failed!");
        return;
    }

    std::u16string msg = Str8ToStr16(asyncContext->content);
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->StartRtt(asyncContext->callId, msg);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
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
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterStopRttCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterStopRttCallback failed!");
        return;
    }
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->StopRtt(asyncContext->callId);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
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
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->JoinConference(
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
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterUpdateCallMediaModeCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterUpdateCallMediaModeCallback failed!");
        return;
    }
    asyncContext->result = DelayedSingleton<CallManagerClient>::GetInstance()->UpdateImsCallMode(
        asyncContext->callId, (ImsCallMode)asyncContext->type);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterUpdateCallMediaModeCallback();
        TELEPHONY_LOGE("UnRegisterUpdateCallMediaModeCallback failed!");
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeEnableLteEnhanceMode(napi_env env, void *data)
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
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterEnableLteEnhanceModeCallback(
            infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterEnableLteEnhanceModeCallback failed!");
        return;
    }
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->EnableLteEnhanceMode(asyncContext->slotId);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterEnableLteEnhanceModeCallback();
        TELEPHONY_LOGE("UnRegisterEnableLteEnhanceModeCallback failed!");
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeDisableLteEnhanceMode(napi_env env, void *data)
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
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterDisableLteEnhanceModeCallback(
            infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterEnableLteEnhanceModeCallback failed!");
        return;
    }
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->DisableLteEnhanceMode(asyncContext->slotId);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterDisableLteEnhanceModeCallback();
        TELEPHONY_LOGE("UnRegisterDisableLteEnhanceModeCallback failed!");
        return;
    }
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeIsLteEnhanceModeEnabled(napi_env env, void *data)
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
    asyncContext->result =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetLteEnhanceCallback(infoListener);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetLteEnhanceCallback failed!");
        return;
    }
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->IsLteEnhanceModeEnabled(asyncContext->slotId);
    if (asyncContext->result != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetLteEnhanceCallback();
        TELEPHONY_LOGE("IsLteEnhanceModeEnabled failed!");
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
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->ReportOttCallDetailsInfo(asyncContext->ottVec);
}

void NapiCallManager::NativeReportOttCallEventInfo(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (OttEventAsyncContext *)data;
    asyncContext->result =
        DelayedSingleton<CallManagerClient>::GetInstance()->ReportOttCallEventInfo(asyncContext->eventInfo);
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
