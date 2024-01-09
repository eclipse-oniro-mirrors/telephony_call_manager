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

#include <securec.h>
#include <string_ex.h>

#include "ability_manager_client.h"
#include "bool_wrapper.h"
#include "call_manager_client.h"
#include "call_manager_errors.h"
#include "napi_call_ability_callback.h"
#include "napi_call_manager_types.h"
#include "napi_call_manager_utils.h"
#include "napi_util.h"
#include "string_wrapper.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"
#include "telephony_napi_common_error.h"
#include "telephony_types.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
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
        DECLARE_NAPI_FUNCTION("dial", Dial),
        DECLARE_NAPI_FUNCTION("dialCall", DialCall),
        DECLARE_NAPI_FUNCTION("makeCall", MakeCall),
        DECLARE_NAPI_FUNCTION("answer", AnswerCall),
        DECLARE_NAPI_FUNCTION("reject", RejectCall),
        DECLARE_NAPI_FUNCTION("hangup", HangUpCall),
        DECLARE_NAPI_FUNCTION("answerCall", AnswerCall),
        DECLARE_NAPI_FUNCTION("rejectCall", RejectCall),
        DECLARE_NAPI_FUNCTION("hangUpCall", HangUpCall),
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
        DECLARE_NAPI_FUNCTION("kickOutFromConference", KickOutFromConference),
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
        DECLARE_NAPI_FUNCTION("setCallRestrictionPassword", SetCallRestrictionPassword),
        DECLARE_NAPI_FUNCTION("getCallTransferInfo", GetCallTransferInfo),
        DECLARE_NAPI_FUNCTION("setCallTransfer", SetCallTransferInfo),
        DECLARE_NAPI_FUNCTION("enableImsSwitch", EnableImsSwitch),
        DECLARE_NAPI_FUNCTION("disableImsSwitch", DisableImsSwitch),
        DECLARE_NAPI_FUNCTION("isImsSwitchEnabled", IsImsSwitchEnabled),
        DECLARE_NAPI_FUNCTION("setVoNRState", SetVoNRState),
        DECLARE_NAPI_FUNCTION("getVoNRState", GetVoNRState),
        DECLARE_NAPI_FUNCTION("canSetCallTransferTime", CanSetCallTransferTime),
        DECLARE_NAPI_FUNCTION("closeUnfinishedUssd", CloseUnFinishedUssd),
        DECLARE_NAPI_FUNCTION("inputDialerSpecialCode", InputDialerSpecialCode),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallExtendInterface(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("startDTMF", StartDTMF),
        DECLARE_NAPI_FUNCTION("stopDTMF", StopDTMF),
        DECLARE_NAPI_FUNCTION("postDialProceed", PostDialProceed),
        DECLARE_NAPI_FUNCTION("getCallState", GetCallState),
        DECLARE_NAPI_FUNCTION("getCallStateSync", GetCallStateSync),
        DECLARE_NAPI_FUNCTION("isRinging", IsRinging),
        DECLARE_NAPI_FUNCTION("hasCall", HasCall),
        DECLARE_NAPI_FUNCTION("hasCallSync", HasCallSync),
        DECLARE_NAPI_FUNCTION("isNewCallAllowed", IsNewCallAllowed),
        DECLARE_NAPI_FUNCTION("isInEmergencyCall", IsInEmergencyCall),
        DECLARE_NAPI_FUNCTION("isEmergencyPhoneNumber", IsEmergencyPhoneNumber),
        DECLARE_NAPI_FUNCTION("formatPhoneNumber", FormatPhoneNumber),
        DECLARE_NAPI_FUNCTION("formatPhoneNumberToE164", FormatPhoneNumberToE164),
        DECLARE_NAPI_FUNCTION("on", ObserverOn),
        DECLARE_NAPI_FUNCTION("off", ObserverOff),
        DECLARE_NAPI_FUNCTION("reportOttCallDetailsInfo", ReportOttCallDetailsInfo),
        DECLARE_NAPI_FUNCTION("reportOttCallEventInfo", ReportOttCallEventInfo),
        DECLARE_NAPI_FUNCTION("removeMissedIncomingCallNotification", RemoveMissedIncomingCallNotification),
        DECLARE_NAPI_FUNCTION("setVoIPCallState", SetVoIPCallState),
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
        DECLARE_NAPI_FUNCTION("updateImsCallMode", UpdateImsCallMode),
        DECLARE_NAPI_FUNCTION("cancelCallUpgrade", CancelCallUpgrade),
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
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallMediaEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // VideoStateType
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VOICE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_VOICE))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VIDEO_SEND_ONLY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_SEND_ONLY))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VIDEO_RECEIVE_ONLY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_RECEIVE_ONLY))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VIDEO",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_VIDEO))),
        // ImsCallMode
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_AUDIO_ONLY", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_AUDIO_ONLY)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_SEND_ONLY", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_SEND_ONLY)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_RECEIVE_ONLY", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_RECEIVE_ONLY)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_SEND_RECEIVE", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_SEND_RECEIVE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_VIDEO_PAUSED", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_VIDEO_PAUSED)),
        // VideoRequestResultType
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_SUCCESS", NapiCallManagerUtils::ToInt32Value(env, TYPE_REQUEST_SUCCESS)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_FAILURE", NapiCallManagerUtils::ToInt32Value(env, TYPE_REQUEST_FAIL)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_INVALID", NapiCallManagerUtils::ToInt32Value(env, TYPE_REQUEST_INVALID)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_TIMED_OUT", NapiCallManagerUtils::ToInt32Value(env, TYPE_REQUEST_TIMED_OUT)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_REJECTED_BY_REMOTE", NapiCallManagerUtils::ToInt32Value(env, REQUEST_REJECTED_BY_REMOTE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_UPGRADE_CANCELED", NapiCallManagerUtils::ToInt32Value(env, TYPE_REQUEST_UPGRADE_CANCELED)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_DOWNGRADE_RTP_OR_RTCP_TIMEOUT",
            NapiCallManagerUtils::ToInt32Value(env, TYPE_MODIFY_DOWNGRADE_RTP_OR_RTCP_TIMEOUT)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_DOWNGRADE_RTP_AND_RTCP_TIMEOUT",
            NapiCallManagerUtils::ToInt32Value(env, TYPE_MODIFY_DOWNGRADE_RTP_AND_RTCP_TIMEOUT)),
        // DeviceDirection
        DECLARE_NAPI_STATIC_PROPERTY(
            "DEVICE_DIRECTION_0", NapiCallManagerUtils::ToInt32Value(env, DEVICE_DIRECTION_0)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "DEVICE_DIRECTION_90", NapiCallManagerUtils::ToInt32Value(env, DEVICE_DIRECTION_90)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "DEVICE_DIRECTION_180", NapiCallManagerUtils::ToInt32Value(env, DEVICE_DIRECTION_180)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "DEVICE_DIRECTION_270", NapiCallManagerUtils::ToInt32Value(env, DEVICE_DIRECTION_270)),
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
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_EMERGENCY", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialScene::CALL_EMERGENCY))),
        // CallType
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_CS", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_CS))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_IMS", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_IMS))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_OTT", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_OTT))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_VOIP", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_VOIP))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_ERR_CALL", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_ERR_CALL))),
        // DialType
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_CARRIER_TYPE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialType::DIAL_CARRIER_TYPE))),
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_VOICE_MAIL_TYPE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialType::DIAL_VOICE_MAIL_TYPE))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "DIAL_OTT_TYPE", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DialType::DIAL_OTT_TYPE))),
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
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_ANSWERED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_ANSWERED))),
        // TelConferenceState
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_IDLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_IDLE))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_ACTIVE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_ACTIVE))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_HOLDING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_HOLDING))),
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
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_ANSWERED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallStateToApp::CALL_STATE_ANSWERED))),
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
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_HOLD_CALL_FAILED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_HOLD_CALL_FAILED))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_SWAP_CALL_FAILED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_SWAP_CALL_FAILED))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_COMBINE_CALL_FAILED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_COMBINE_CALL_FAILED))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_SPLIT_CALL_FAILED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_SPLIT_CALL_FAILED))),
        // CallSessionEventId
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_CONTROL_CAMERA_FAILURE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallSessionEventId::EVENT_CAMERA_FAILURE))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_CONTROL_CAMERA_READY",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallSessionEventId::EVENT_CAMERA_READY))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_DISPLAY_SURFACE_RELEASED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallSessionEventId::EVENT_RELEASE_DISPLAY_SURFACE))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_PREVIEW_SURFACE_RELEASED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallSessionEventId::EVENT_RELEASE_PREVIEW_SURFACE))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiCallManager::DeclareCallRestrictionEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // CallRestrictionType
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_INCOMING_SERVICES",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_INCOMING_SERVICES))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_OUTGOING_SERVICES",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_OUTGOING_SERVICES))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ALL_CALLS",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ROAMING_INCOMING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ROAMING_INCOMING))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ALL_INCOMING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_INTERNATIONAL",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_INTERNATIONAL))),
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_TYPE_ALL_OUTGOING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING))),
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
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_ENABLE", NapiCallManagerUtils::ToInt32Value(env, RESTRICTION_ENABLE)),
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
napi_value NapiCallManager::DeclareVoNRStateEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("VONR_STATE_ON",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VoNRState::VONR_STATE_ON))),
        DECLARE_NAPI_STATIC_PROPERTY("VONR_STATE_OFF",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VoNRState::VONR_STATE_OFF))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "VoNRState", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "VoNRState", result);
    return exports;
}

napi_value NapiCallManager::DeclareAudioDeviceEnum(napi_env env, napi_value exports)
{
    // AudioDeviceType
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_UNKNOWN",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDeviceType::DEVICE_UNKNOWN))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_DISABLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDeviceType::DEVICE_DISABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_BLUETOOTH_SCO",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDeviceType::DEVICE_BLUETOOTH_SCO))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_DISTRIBUTED_CAR",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDeviceType::DEVICE_DISTRIBUTED_CAR))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_WIRED_HEADSET",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDeviceType::DEVICE_WIRED_HEADSET))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_SPEAKER",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDeviceType::DEVICE_SPEAKER))),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_EARPIECE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(AudioDeviceType::DEVICE_EARPIECE))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "AudioDeviceType", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "AudioDeviceType", result);
    return exports;
}

napi_value NapiCallManager::DeclareVideoStateTypeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VOICE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_VOICE))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VIDEO_SEND_ONLY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_SEND_ONLY))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VIDEO_RECEIVE_ONLY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_RECEIVE_ONLY))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_VIDEO",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(VideoStateType::TYPE_VIDEO))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "VideoStateType", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "VideoStateType", result);
    return exports;
}

napi_value NapiCallManager::DeclareVideoRequestResultEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_SUCCESS", NapiCallManagerUtils::ToInt32Value(env, TYPE_REQUEST_SUCCESS)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_FAILURE", NapiCallManagerUtils::ToInt32Value(env, TYPE_REQUEST_FAIL)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_INVALID", NapiCallManagerUtils::ToInt32Value(env, TYPE_REQUEST_INVALID)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_TIMED_OUT", NapiCallManagerUtils::ToInt32Value(env, TYPE_REQUEST_TIMED_OUT)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_REJECTED_BY_REMOTE", NapiCallManagerUtils::ToInt32Value(env, REQUEST_REJECTED_BY_REMOTE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_REQUEST_UPGRADE_CANCELED", NapiCallManagerUtils::ToInt32Value(env, TYPE_REQUEST_UPGRADE_CANCELED)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_DOWNGRADE_RTP_OR_RTCP_TIMEOUT",
            NapiCallManagerUtils::ToInt32Value(env, TYPE_MODIFY_DOWNGRADE_RTP_OR_RTCP_TIMEOUT)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_DOWNGRADE_RTP_AND_RTCP_TIMEOUT",
            NapiCallManagerUtils::ToInt32Value(env, TYPE_MODIFY_DOWNGRADE_RTP_AND_RTCP_TIMEOUT)),
    };
    napi_value result = nullptr;
    napi_define_class(env, "VideoRequestResultType", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "VideoRequestResultType", result);
    return exports;
}

napi_value NapiCallManager::DeclareImsCallModeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_AUDIO_ONLY", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_AUDIO_ONLY)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_SEND_ONLY", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_SEND_ONLY)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_RECEIVE_ONLY", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_RECEIVE_ONLY)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_SEND_RECEIVE", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_SEND_RECEIVE)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "CALL_MODE_VIDEO_PAUSED", NapiCallManagerUtils::ToInt32Value(env, CALL_MODE_VIDEO_PAUSED)),
    };
    napi_value result = nullptr;
    napi_define_class(env, "ImsCallMode", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "ImsCallMode", result);
    return exports;
}

napi_value NapiCallManager::DeclareDeviceDirectionEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // DeviceDirection
        DECLARE_NAPI_STATIC_PROPERTY(
            "DEVICE_DIRECTION_0", NapiCallManagerUtils::ToInt32Value(env, DEVICE_DIRECTION_0)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "DEVICE_DIRECTION_90", NapiCallManagerUtils::ToInt32Value(env, DEVICE_DIRECTION_90)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "DEVICE_DIRECTION_180", NapiCallManagerUtils::ToInt32Value(env, DEVICE_DIRECTION_180)),
        DECLARE_NAPI_STATIC_PROPERTY(
            "DEVICE_DIRECTION_270", NapiCallManagerUtils::ToInt32Value(env, DEVICE_DIRECTION_270)),
    };
    napi_value result = nullptr;
    napi_define_class(env, "DeviceDirection", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "DeviceDirection", result);
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
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_VOIP", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_VOIP))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_ERR_CALL", NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallType::TYPE_ERR_CALL))),
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
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_IDLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DISCONNECTING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DISCONNECTED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_WAITING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_WAITING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_INCOMING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_ALERTING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_ALERTING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_DIALING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_DIALING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_HOLDING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_HOLDING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_ACTIVE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATUS_ANSWERED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelCallState::CALL_STATUS_ANSWERED))),
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
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_DISCONNECTED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_DISCONNECTED))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_DISCONNECTING",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_DISCONNECTING))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_HOLDING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_HOLDING))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_ACTIVE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_ACTIVE))),
        DECLARE_NAPI_STATIC_PROPERTY("TEL_CONFERENCE_IDLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(TelConferenceState::TEL_CONFERENCE_IDLE))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "ConferenceState", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "ConferenceState", result);
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
        DECLARE_NAPI_STATIC_PROPERTY("CALL_STATE_ANSWERED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallStateToApp::CALL_STATE_ANSWERED))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallState", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallState", result);
    return exports;
}

napi_value NapiCallManager::DeclareCallEventEnumEx(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // CallAbilityEventId
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_DIAL_NO_CARRIER",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallAbilityEventId::EVENT_DIAL_NO_CARRIER))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_INVALID_FDN_NUMBER",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_INVALID_FDN_NUMBER))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_HOLD_CALL_FAILED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_HOLD_CALL_FAILED))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_SWAP_CALL_FAILED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_SWAP_CALL_FAILED))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_COMBINE_CALL_FAILED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_COMBINE_CALL_FAILED))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_SPLIT_CALL_FAILED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallAbilityEventId::EVENT_SPLIT_CALL_FAILED))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallAbilityEventId", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallAbilityEventId", result);
    return exports;
}

napi_value NapiCallManager::DeclareCallSessionEventEnumEx(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // CallSessionEventId
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_CONTROL_CAMERA_FAILURE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallSessionEventId::EVENT_CAMERA_FAILURE))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_CONTROL_CAMERA_READY",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallSessionEventId::EVENT_CAMERA_READY))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_DISPLAY_SURFACE_RELEASED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallSessionEventId::EVENT_RELEASE_DISPLAY_SURFACE))),
        DECLARE_NAPI_STATIC_PROPERTY("EVENT_PREVIEW_SURFACE_RELEASED",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallSessionEventId::EVENT_RELEASE_PREVIEW_SURFACE))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallSessionEventId", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallSessionEventId", result);
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
        DECLARE_NAPI_STATIC_PROPERTY("RESTRICTION_ENABLE", NapiCallManagerUtils::ToInt32Value(env, RESTRICTION_ENABLE)),
    };
    napi_value result = nullptr;
    napi_define_class(env, "RestrictionStatus", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
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
    napi_define_class(env, "CallWaitingStatus", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
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
    napi_define_class(env, "TransferStatus", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "TransferStatus", result);
    return exports;
}

napi_value NapiCallManager::DeclareTransferTypeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_NOT_REACHABLE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_NOT_REACHABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_NO_REPLY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_NO_REPLY))),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_BUSY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_BUSY))),
        DECLARE_NAPI_STATIC_PROPERTY("TRANSFER_TYPE_UNCONDITIONAL",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferType::TRANSFER_TYPE_UNCONDITIONAL))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallTransferType", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallTransferType", result);
    return exports;
}

napi_value NapiCallManager::DeclareTransferSettingTypeEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_ERASURE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_ERASURE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_REGISTRATION",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_REGISTRATION))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_DISABLE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_DISABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_TRANSFER_ENABLE",
            NapiCallManagerUtils::ToInt32Value(
                env, static_cast<int32_t>(CallTransferSettingType::CALL_TRANSFER_ENABLE))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "CallTransferSettingType", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "CallTransferSettingType", result);
    return exports;
}

napi_value NapiCallManager::DeclareMmiCodeResultEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("MMI_CODE_SUCCESS",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(MmiCodeResult::MMI_CODE_SUCCESS))),
        DECLARE_NAPI_STATIC_PROPERTY("MMI_CODE_FAILED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(MmiCodeResult::MMI_CODE_FAILED))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "MmiCodeResult", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "MmiCodeResult", result);
    return exports;
}

napi_value NapiCallManager::DeclareDisconnectedReasonEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("UNASSIGNED_NUMBER",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::UNASSIGNED_NUMBER))),
        DECLARE_NAPI_STATIC_PROPERTY("NO_ROUTE_TO_DESTINATION",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::NO_ROUTE_TO_DESTINATION))),
        DECLARE_NAPI_STATIC_PROPERTY("CHANNEL_UNACCEPTABLE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::CHANNEL_UNACCEPTABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("OPERATOR_DETERMINED_BARRING",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::OPERATOR_DETERMINED_BARRING))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_COMPLETED_ELSEWHERE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::CALL_COMPLETED_ELSEWHERE))),
        DECLARE_NAPI_STATIC_PROPERTY("NORMAL_CALL_CLEARING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::NORMAL_CALL_CLEARING))),
        DECLARE_NAPI_STATIC_PROPERTY("USER_BUSY",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::USER_BUSY))),
        DECLARE_NAPI_STATIC_PROPERTY("NO_USER_RESPONDING",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::NO_USER_RESPONDING))),
        DECLARE_NAPI_STATIC_PROPERTY("USER_ALERTING_NO_ANSWER",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::USER_ALERTING_NO_ANSWER))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_REJECTED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::CALL_REJECTED))),
        DECLARE_NAPI_STATIC_PROPERTY("NUMBER_CHANGED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::NUMBER_CHANGED))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_REJECTED_DUE_TO_FEATURE_AT_THE_DESTINATION",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::CALL_REJECTED_DUE_TO_FEATURE_AT_THE_DESTINATION))),
        DECLARE_NAPI_STATIC_PROPERTY("FAILED_PRE_EMPTION",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::FAILED_PRE_EMPTION))),
        DECLARE_NAPI_STATIC_PROPERTY("NON_SELECTED_USER_CLEARING",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::NON_SELECTED_USER_CLEARING))),
        DECLARE_NAPI_STATIC_PROPERTY("DESTINATION_OUT_OF_ORDER",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::DESTINATION_OUT_OF_ORDER))),
        DECLARE_NAPI_STATIC_PROPERTY("INVALID_NUMBER_FORMAT",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::INVALID_NUMBER_FORMAT))),
        DECLARE_NAPI_STATIC_PROPERTY("FACILITY_REJECTED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::FACILITY_REJECTED))),
        DECLARE_NAPI_STATIC_PROPERTY("RESPONSE_TO_STATUS_ENQUIRY",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::RESPONSE_TO_STATUS_ENQUIRY))),
        DECLARE_NAPI_STATIC_PROPERTY("NORMAL_UNSPECIFIED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::NORMAL_UNSPECIFIED))),
        DECLARE_NAPI_STATIC_PROPERTY("NO_CIRCUIT_CHANNEL_AVAILABLE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::NO_CIRCUIT_CHANNEL_AVAILABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("NETWORK_OUT_OF_ORDER",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::NETWORK_OUT_OF_ORDER))),
        DECLARE_NAPI_STATIC_PROPERTY("TEMPORARY_FAILURE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::TEMPORARY_FAILURE))),
        DECLARE_NAPI_STATIC_PROPERTY("SWITCHING_EQUIPMENT_CONGESTION",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::SWITCHING_EQUIPMENT_CONGESTION))),
        DECLARE_NAPI_STATIC_PROPERTY("ACCESS_INFORMATION_DISCARDED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::ACCESS_INFORMATION_DISCARDED))),
        DECLARE_NAPI_STATIC_PROPERTY("REQUEST_CIRCUIT_CHANNEL_NOT_AVAILABLE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::REQUEST_CIRCUIT_CHANNEL_NOT_AVAILABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("RESOURCES_UNAVAILABLE_UNSPECIFIED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::RESOURCES_UNAVAILABLE_UNSPECIFIED))),
        DECLARE_NAPI_STATIC_PROPERTY("QUALITY_OF_SERVICE_UNAVAILABLE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::QUALITY_OF_SERVICE_UNAVAILABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("REQUESTED_FACILITY_NOT_SUBSCRIBED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::REQUESTED_FACILITY_NOT_SUBSCRIBED))),
        DECLARE_NAPI_STATIC_PROPERTY("INCOMING_CALLS_BARRED_WITHIN_THE_CUG",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::INCOMING_CALLS_BARRED_WITHIN_THE_CUG))),
        DECLARE_NAPI_STATIC_PROPERTY("BEARER_CAPABILITY_NOT_AUTHORIZED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::BEARER_CAPABILITY_NOT_AUTHORIZED))),
        DECLARE_NAPI_STATIC_PROPERTY("BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("SERVICE_OR_OPTION_NOT_AVAILABLE_UNSPECIFIED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::SERVICE_OR_OPTION_NOT_AVAILABLE_UNSPECIFIED))),
        DECLARE_NAPI_STATIC_PROPERTY("BEARER_SERVICE_NOT_IMPLEMENTED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::BEARER_SERVICE_NOT_IMPLEMENTED))),
        DECLARE_NAPI_STATIC_PROPERTY("ACM_EQUALTO_OR_GREATER_THAN_THE_MAXIMUM_VALUE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::ACM_EQUALTO_OR_GREATE_THAN_ACMMAX))),
        DECLARE_NAPI_STATIC_PROPERTY("REQUESTED_FACILITY_NOT_IMPLEMENTED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::REQUESTED_FACILITY_NOT_IMPLEMENTED))),
        DECLARE_NAPI_STATIC_PROPERTY("ONLY_RESTRICTED_DIGITAL_INFO_BEARER_CAPABILITY_IS_AVAILABLE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::ONLY_RESTRICTED_DIGITAL_INFO_BEARER_CAPABILITY_IS_AVAILABLE))),
        DECLARE_NAPI_STATIC_PROPERTY("SERVICE_OR_OPTION_NOT_IMPLEMENTED_UNSPECIFIED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::SERVICE_OR_OPTION_NOT_IMPLEMENTED_UNSPECIFIED))),
        DECLARE_NAPI_STATIC_PROPERTY("INVALID_TRANSACTION_IDENTIFIER_VALUE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::INVALID_TRANSACTION_IDENTIFIER_VALUE))),
        DECLARE_NAPI_STATIC_PROPERTY("USER_NOT_MEMBER_OF_CUG",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::USER_NOT_MEMBER_OF_CUG))),
        DECLARE_NAPI_STATIC_PROPERTY("INCOMPATIBLE_DESTINATION",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::INCOMPATIBLE_DESTINATION))),
        DECLARE_NAPI_STATIC_PROPERTY("INVALID_TRANSIT_NETWORK_SELECTION",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::INVALID_TRANSIT_NETWORK_SELECTION))),
        DECLARE_NAPI_STATIC_PROPERTY("SEMANTICALLY_INCORRECT_MESSAGE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::SEMANTICALLY_INCORRECT_MESSAGE))),
        DECLARE_NAPI_STATIC_PROPERTY("INVALID_MANDATORY_INFORMATION",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::INVALID_MANDATORY_INFORMATION))),
        DECLARE_NAPI_STATIC_PROPERTY("MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED))),
        DECLARE_NAPI_STATIC_PROPERTY("MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE))),
        DECLARE_NAPI_STATIC_PROPERTY("INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED))),
        DECLARE_NAPI_STATIC_PROPERTY("CONDITIONAL_IE_ERROR",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::CONDITIONAL_IE_ERROR))),
        DECLARE_NAPI_STATIC_PROPERTY("MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE))),
        DECLARE_NAPI_STATIC_PROPERTY("RECOVERY_ON_TIMER_EXPIRED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::RECOVERY_ON_TIMER_EXPIRED))),
        DECLARE_NAPI_STATIC_PROPERTY("PROTOCOL_ERROR_UNSPECIFIED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::PROTOCOL_ERROR_UNSPECIFIED))),
        DECLARE_NAPI_STATIC_PROPERTY("INTERWORKING_UNSPECIFIED",
            NapiCallManagerUtils::ToInt32Value(env,
            static_cast<int32_t>(DisconnectedReason::INTERWORKING_UNSPECIFIED))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_BARRED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::CALL_BARRED))),
        DECLARE_NAPI_STATIC_PROPERTY("FDN_BLOCKED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::FDN_BLOCKED))),
        DECLARE_NAPI_STATIC_PROPERTY("IMSI_UNKNOWN_IN_VLR",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::IMSI_UNKNOWN_IN_VLR))),
        DECLARE_NAPI_STATIC_PROPERTY("IMEI_NOT_ACCEPTED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::IMEI_NOT_ACCEPTED))),
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_MODIFIED_TO_USSD",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::DIAL_MODIFIED_TO_USSD))),
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_MODIFIED_TO_SS",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::DIAL_MODIFIED_TO_SS))),
        DECLARE_NAPI_STATIC_PROPERTY("DIAL_MODIFIED_TO_DIAL",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::DIAL_MODIFIED_TO_DIAL))),
        DECLARE_NAPI_STATIC_PROPERTY("RADIO_OFF",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::RADIO_OFF))),
        DECLARE_NAPI_STATIC_PROPERTY("OUT_OF_SERVICE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::OUT_OF_SERVICE))),
        DECLARE_NAPI_STATIC_PROPERTY("NO_VALID_SIM",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::NO_VALID_SIM))),
        DECLARE_NAPI_STATIC_PROPERTY("RADIO_INTERNAL_ERROR",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::RADIO_INTERNAL_ERROR))),
        DECLARE_NAPI_STATIC_PROPERTY("NETWORK_RESP_TIMEOUT",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::NETWORK_RESP_TIMEOUT))),
        DECLARE_NAPI_STATIC_PROPERTY("NETWORK_REJECT",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::NETWORK_REJECT))),
        DECLARE_NAPI_STATIC_PROPERTY("RADIO_ACCESS_FAILURE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::RADIO_ACCESS_FAILURE))),
        DECLARE_NAPI_STATIC_PROPERTY("RADIO_LINK_FAILURE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::RADIO_LINK_FAILURE))),
        DECLARE_NAPI_STATIC_PROPERTY("RADIO_LINK_LOST",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::RADIO_LINK_LOST))),
        DECLARE_NAPI_STATIC_PROPERTY("RADIO_UPLINK_FAILURE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::RADIO_UPLINK_FAILURE))),
        DECLARE_NAPI_STATIC_PROPERTY("RADIO_SETUP_FAILURE",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::RADIO_SETUP_FAILURE))),
        DECLARE_NAPI_STATIC_PROPERTY("RADIO_RELEASE_NORMAL",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::RADIO_RELEASE_NORMAL))),
        DECLARE_NAPI_STATIC_PROPERTY("RADIO_RELEASE_ABNORMAL",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::RADIO_RELEASE_ABNORMAL))),
        DECLARE_NAPI_STATIC_PROPERTY("ACCESS_CLASS_BLOCKED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::ACCESS_CLASS_BLOCKED))),
        DECLARE_NAPI_STATIC_PROPERTY("NETWORK_DETACH",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::NETWORK_DETACH))),
        DECLARE_NAPI_STATIC_PROPERTY("INVALID_PARAMETER",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::INVALID_PARAMETER))),
        DECLARE_NAPI_STATIC_PROPERTY("SIM_NOT_EXIT",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::SIM_NOT_EXIT))),
        DECLARE_NAPI_STATIC_PROPERTY("SIM_PIN_NEED",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::SIM_PIN_NEED))),
        DECLARE_NAPI_STATIC_PROPERTY("CALL_NOT_ALLOW",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::CALL_NOT_ALLOW))),
        DECLARE_NAPI_STATIC_PROPERTY("SIM_INVALID",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::SIM_INVALID))),
        DECLARE_NAPI_STATIC_PROPERTY("UNKNOWN",
            NapiCallManagerUtils::ToInt32Value(env, static_cast<int32_t>(DisconnectedReason::FAILED_UNKNOWN))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "DisconnectedReason", NAPI_AUTO_LENGTH, NapiCallManagerUtils::CreateEnumConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "DisconnectedReason", result);
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
    DeclareVoNRStateEnum(env, exports);
    DeclareAudioDeviceEnum(env, exports);
    DeclareVideoStateTypeEnum(env, exports);
    DeclareVideoRequestResultEnum(env, exports);
    DeclareImsCallModeEnum(env, exports);
    DeclareDeviceDirectionEnum(env, exports);
    DeclareDialSceneEnum(env, exports);
    DeclareCallTypeEnum(env, exports);
    DeclareDialTypeEnum(env, exports);
    DeclareTelCallStateEnum(env, exports);
    DeclareConferenceStateEnum(env, exports);
    DeclareCallStateToAppEnum(env, exports);
    DeclareCallEventEnumEx(env, exports);
    DeclareCallSessionEventEnumEx(env, exports);
    DeclareRestrictionTypeEnum(env, exports);
    DeclareRestrictionModeEnum(env, exports);
    DeclareRestrictionStatusEnum(env, exports);
    DeclareCallWaitingEnumEx(env, exports);
    DeclareTransferStatusEnum(env, exports);
    DeclareTransferTypeEnum(env, exports);
    DeclareTransferSettingTypeEnum(env, exports);
    DeclareMmiCodeResultEnum(env, exports);
    DeclareDisconnectedReasonEnum(env, exports);
    Init();
    RegisterCallBack();
    return exports;
}

napi_value NapiCallManager::Dial(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    NAPI_ASSERT(env, argc <= VALUE_MAXIMUM_LIMIT, "parameter error!");
    bool matchFlag = NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string);
    NAPI_ASSERT(env, matchFlag, "Dial type error, should be string type");
    auto asyncContext = (std::make_unique<DialAsyncContext>());
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "Dial error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_undefined) ||
            NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_null)) {
            TELEPHONY_LOGI("undefined or null param is detected, second param is ignored.");
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
            GetDialInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        }
    } else if (argc == VALUE_MAXIMUM_LIMIT &&
        NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_object)) {
        GetDialInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "Dial", NativeDial, NativeDialCallBack);
}

bool NapiCallManager::MatchStringAndVariableObjectParameters(
    napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchStringAndVariableObjectParameters %{public}zu", parameterCount);
    switch (parameterCount) {
        case ONLY_ONE_VALUE:
            return NapiUtil::MatchParameters(env, parameters, { napi_string });
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_string, napi_function }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_string, napi_null}) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_string, napi_undefined }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_string, napi_object });
        case VALUE_MAXIMUM_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_string, napi_object, napi_function });
        default:
            return false;
    }
}

napi_value NapiCallManager::DialCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if (!MatchStringAndVariableObjectParameters(env, argv, argc)) {
        TELEPHONY_LOGE("MatchStringAndVariableObjectParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = (std::make_unique<DialAsyncContext>());
    if (asyncContext == nullptr) {
        NapiUtil::ThrowParameterError(env);
        TELEPHONY_LOGE("asyncContext is nullptr.");
        return nullptr;
    }
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else {
            GetDialInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        }
    } else {
        GetDialInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "DialCall", NativeDialCall, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::MakeCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if (!MatchOneStringParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::MakeCall MatchOneStringParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "MakeCall", NativeMakeCall, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::AnswerCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneOptionalNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("MatchOneOptionalNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto answerAsyncContext = std::make_unique<AnswerAsyncContext>();
    if (answerAsyncContext == nullptr) {
        TELEPHONY_LOGE("answerAsyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }

    if (argc == ZERO_VALUE) {
        TELEPHONY_LOGI("no param input");
    } else if (argc == ONLY_ONE_VALUE) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_undefined) ||
            NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_null)) {
            TELEPHONY_LOGI("undefined or null param is detected, treating as no param input.");
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(answerAsyncContext->callbackRef));
        } else {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &answerAsyncContext->callId);
        }
    } else if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &answerAsyncContext->callId);
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(answerAsyncContext->callbackRef));
        } else {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &answerAsyncContext->videoState);
            napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &answerAsyncContext->callId);
        }
    }

    return HandleAsyncWork(
        env, answerAsyncContext.release(), "AnswerCall", NativeAnswerCall, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::AnswerVideoCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchAnswerCallParameter(env, argv, argc)) {
        TELEPHONY_LOGE("MatchOneOptionalNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto answerAsyncContext = std::make_unique<AnswerAsyncContext>();
    if (answerAsyncContext == nullptr) {
        TELEPHONY_LOGE("answerAsyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (argc == ZERO_VALUE) {
        TELEPHONY_LOGI("no param input");
    } else if (argc == ONLY_ONE_VALUE) {
        napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &answerAsyncContext->videoState);
    } else if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &answerAsyncContext->videoState);
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(answerAsyncContext->callbackRef));
        } else {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &answerAsyncContext->videoState);
            napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &answerAsyncContext->callId);
        }
    }

    return HandleAsyncWork(
        env, answerAsyncContext.release(), "AnswerCall", NativeAnswerCall, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::RejectCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchRejectCallParameters(env, argv, argc)) {
        TELEPHONY_LOGE("MatchRejectCallParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<RejectAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    asyncContext->isSendSms = false;
    if (argc == ZERO_VALUE) {
        TELEPHONY_LOGI("no param input.");
    } else if (argc == ONLY_ONE_VALUE) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_undefined)) {
            TELEPHONY_LOGI("undefined or null param is detected, treating as no param input.");
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number)) {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        } else {
            GetSmsInfo(env, argv[ARRAY_INDEX_FIRST], *asyncContext);
        }
    } else if (argc == TWO_VALUE_LIMIT) {
        if (MatchRejectCallTwoIllegalParameters(env, argv)) {
            TELEPHONY_LOGI("undefined or null params are detected, treating as no param input.");
        } else if (MatchRejectCallFirstIllegalParameters(env, argv)) {
            TELEPHONY_LOGI("undefined or null param is detected, first param is ignored.");
            GetSmsInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        } else if (MatchRejectCallSecondIllegalParameters(env, argv)) {
            TELEPHONY_LOGI("undefined or null param is detected, second param is ignored.");
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_object) &&
            NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            GetSmsInfo(env, argv[ARRAY_INDEX_FIRST], *asyncContext);
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number) &&
                   NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
            GetSmsInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        }
    } else {
        napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        GetSmsInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }

    return HandleAsyncWork(
        env, asyncContext.release(), "RejectCall", NativeRejectCall, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::HangUpCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneOptionalNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("MatchOneOptionalNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }

    if (argc == ZERO_VALUE) {
        TELEPHONY_LOGI("no param input");
    } else if (argc == ONLY_ONE_VALUE) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_undefined) ||
            NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_null)) {
            TELEPHONY_LOGI("undefined or null param is detected, treating as no param input.");
        } else if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else {
            napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        }
    } else {
        napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }

    return HandleAsyncWork(
        env, asyncContext.release(), "HangUpCall", NativeHangUpCall, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::HoldCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::HoldCall MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::HoldCall asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "HoldCall", NativeHoldCall, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::UnHoldCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::UnHoldCall MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::UnHoldCall asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "UnHoldCall", NativeUnHoldCall, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::SwitchCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::SwitchCall MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::SwitchCall asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }

    return HandleAsyncWork(
        env, asyncContext.release(), "SwitchCall", NativeSwitchCall, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::CombineConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::CombineConference MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::CombineConference asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }

    return HandleAsyncWork(
        env, asyncContext.release(), "CombineConference", NativeCombineConference, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::SeparateConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::SeparateConference MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::SeparateConference asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "SeparateConference", NativeSeparateConference, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::KickOutFromConference(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::KickoutFromConference MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::KickoutFromConference asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "KickoutFromConference", NativeKickOutFromConference,
        NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::GetMainCallId(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::GetMainCallId MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<IntResultAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::GetMainCallId asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "GetMainCallId", NativeGetMainCallId, NativeGetMainCallIdCallBack);
}

napi_value NapiCallManager::GetSubCallIdList(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::GetSubCallIdList MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<ListAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::GetSubCallIdList asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
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
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::GetCallIdListForConference MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<ListAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::GetCallIdListForConference asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "GetCallIdListForConference", NativeGetCallIdListForConference,
        NativeListCallBack);
}

bool NapiCallManager::MatchEmptyParameter(napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchEmptyParameter %{public}zu", parameterCount);
    switch (parameterCount) {
        case ZERO_VALUE:
            return true;
        case ONLY_ONE_VALUE:
            return NapiUtil::MatchParameters(env, parameters, { napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchOneOptionalNumberParameter(
    napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchAnswerParameters %{public}zu", parameterCount);
    switch (parameterCount) {
        case ZERO_VALUE:
            return true;
        case ONLY_ONE_VALUE:
            return NapiUtil::MatchParameters(env, parameters, { napi_number }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_function }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_null }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_undefined });
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_number }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_number, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchAnswerCallParameter(
    napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchAnswerParameters %{public}zu", parameterCount);
    switch (parameterCount) {
        case ZERO_VALUE:
            return true;
        case ONLY_ONE_VALUE:
            return NapiUtil::MatchParameters(env, parameters, { napi_number });
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_number }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_number, napi_function});
        case THREE_VALUE_MAXIMUM_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_number, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchOneStringParameter(napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchOneNumberParameter %{public}zu", parameterCount);
    switch (parameterCount) {
        case ONLY_ONE_VALUE:
            return NapiUtil::MatchParameters(env, parameters, { napi_string });
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_string, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchObserverOffParameter(
    napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("parameter count: %{public}zu", parameterCount);
    switch (parameterCount) {
        case ONLY_ONE_VALUE:
            return NapiUtil::MatchParameters(env, parameters, { napi_string });
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_string, napi_function }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_string, napi_null }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_string, napi_undefined });
        default:
            return false;
    }
}

bool NapiCallManager::MatchOneNumberParameter(napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchOneNumberParameter %{public}zu", parameterCount);
    switch (parameterCount) {
        case ONLY_ONE_VALUE:
            return NapiUtil::MatchParameters(env, parameters, { napi_number });
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchTwoNumberParameters(napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchTwoNumberParameters %{public}zu", parameterCount);
    switch (parameterCount) {
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_number });
        case THREE_VALUE_MAXIMUM_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_number, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchTwoStringParameter(napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchTwoStringParameter %{public}zu", parameterCount);
    switch (parameterCount) {
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_string, napi_string });
        case THREE_VALUE_MAXIMUM_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_string, napi_string, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchNumberAndBoolParameters(
    napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchNumberAndBoolParameters %{public}zu", parameterCount);
    switch (parameterCount) {
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_boolean });
        case THREE_VALUE_MAXIMUM_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_boolean, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchNumberAndStringParameters(
    napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchNumberAndBoolParameters %{public}zu", parameterCount);
    switch (parameterCount) {
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_string });
        case THREE_VALUE_MAXIMUM_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_string, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchAudioDeviceParameters(
    napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchNumberAndBoolParameters %{public}zu", parameterCount);
    switch (parameterCount) {
        case ONLY_ONE_VALUE:
            return NapiUtil::MatchParameters(env, parameters, { napi_object });
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_object, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchRejectCallParameters(
    napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchNumberAndBoolParameters %{public}zu", parameterCount);
    switch (parameterCount) {
        case ZERO_VALUE:
            return true;
        case ONLY_ONE_VALUE:
            return NapiUtil::MatchParameters(env, parameters, { napi_number }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_object }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_function }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_null }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_undefined });
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_object, napi_function }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_number, napi_function }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_number, napi_object }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_number, napi_null }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_number, napi_undefined }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_null, napi_object }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_undefined, napi_object }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_undefined, napi_undefined }) ||
                   NapiUtil::MatchParameters(env, parameters, { napi_null, napi_null });
        case THREE_VALUE_MAXIMUM_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_object, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchRejectCallFirstIllegalParameters(napi_env env, const napi_value parameters[])
{
    return NapiUtil::MatchParameters(env, parameters, { napi_null, napi_object }) ||
           NapiUtil::MatchParameters(env, parameters, { napi_undefined, napi_object });
}

bool NapiCallManager::MatchRejectCallSecondIllegalParameters(napi_env env, const napi_value parameters[])
{
    return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_null }) ||
           NapiUtil::MatchParameters(env, parameters, { napi_number, napi_undefined });
}

bool NapiCallManager::MatchRejectCallTwoIllegalParameters(napi_env env, const napi_value parameters[])
{
    return NapiUtil::MatchParameters(env, parameters, { napi_undefined, napi_undefined }) ||
           NapiUtil::MatchParameters(env, parameters, { napi_null, napi_null });
}

bool NapiCallManager::MatchNumberAndObjectParameters(
    napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Telephony_CallManager MatchNumberAndObjectParameters %{public}zu", parameterCount);
    switch (parameterCount) {
        case TWO_VALUE_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_object });
        case THREE_VALUE_MAXIMUM_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_object, napi_function });
        default:
            return false;
    }
}

bool NapiCallManager::MatchCallRestrictionPasswordParameter(
    napi_env env, const napi_value parameters[], const size_t parameterCount)
{
    TELEPHONY_LOGI("Match parmameter count %{public}zu", parameterCount);
    switch (parameterCount) {
        case THREE_VALUE_MAXIMUM_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_string, napi_string });
        case FOUR_VALUE_MAXIMUM_LIMIT:
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_string, napi_string, napi_function });
        default:
            return false;
    }
}

napi_value NapiCallManager::GetCallWaiting(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::GetCallWaiting MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::GetCallWaiting asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
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
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchNumberAndBoolParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::SetCallWaiting MatchNumberAndBoolParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::SetCallWaiting asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
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
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchTwoNumberParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::GetCallRestriction MatchTwoNumberParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto restrictionAsyncContext = std::make_unique<SupplementAsyncContext>();
    if (restrictionAsyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::GetCallRestriction asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &restrictionAsyncContext->slotId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &restrictionAsyncContext->type);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(restrictionAsyncContext->callbackRef));
    }
    restrictionAsyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(restrictionAsyncContext->thisVar));
    return HandleAsyncWork(
        env, restrictionAsyncContext.release(), "GetCallRestriction", NativeGetCallRestriction, NativeCallBack);
}

napi_value NapiCallManager::SetCallRestriction(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchNumberAndObjectParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::SetCallRestriction MatchNumberAndObjectParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<CallRestrictionAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::SetCallRestriction asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    asyncContext->errorCode = GetRestrictionInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
    if (asyncContext->errorCode == ERROR_PARAMETER_TYPE_INVALID) {
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext.release(), "SetCallRestriction", NativeSetCallRestriction, NativeCallBack);
}

napi_value NapiCallManager::SetCallRestrictionPassword(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, FOUR_VALUE_MAXIMUM_LIMIT);
    if (!MatchCallRestrictionPasswordParameter(env, argv, argc)) {
        TELEPHONY_LOGE("MatchCallRestrictionPasswordParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<CallBarringPasswordAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("CallBarringPasswordAsyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    size_t strLen = 0;
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_SECOND], asyncContext->oldPassword, PHONE_NUMBER_MAXIMUM_LIMIT, &strLen);
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_THIRD], asyncContext->newPassword, PHONE_NUMBER_MAXIMUM_LIMIT, &strLen);
    if (argc == FOUR_VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_FOURTH], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(
        env, asyncContext.release(), "SetCallRestrictionPassword", NativeSetCallRestrictionPassword, NativeCallBack);
}

napi_value NapiCallManager::GetCallTransferInfo(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchTwoNumberParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::GetCallTransferInfo MatchTwoNumberParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::GetCallTransferInfo asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
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
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchNumberAndObjectParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::SetCallTransferInfo MatchNumberAndObjectParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<CallTransferAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::SetCallTransferInfo asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    asyncContext->errorCode = GetTransferInfo(env, argv[ARRAY_INDEX_SECOND], *asyncContext);
    if (asyncContext->errorCode == ERROR_PARAMETER_TYPE_INVALID) {
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(env, asyncContext.release(), "SetCallTransferInfo", NativeSetTransferNumber, NativeCallBack);
}

static inline bool IsValidSlotId(int32_t slotId)
{
    return ((slotId >= DEFAULT_SIM_SLOT_ID) && (slotId < SIM_SLOT_COUNT));
}

napi_value NapiCallManager::CanSetCallTransferTime(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<BoolResultAsyncContext>();
    asyncContext->eventId = CALL_MANAGER_CAN_SET_CALL_TRANSFER_TIME;
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "CanSetCallTransferTime", NativeCanSetCallTransferTime,
        NativeBoolCallBackWithErrorCode);
}

napi_value NapiCallManager::EnableImsSwitch(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::EnableImsSwitch MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<ImsSwitchAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::EnableImsSwitch asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }

    return HandleAsyncWork(
        env, asyncContext.release(), "EnableImsSwitch", NativeEnableImsSwitch, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::DisableImsSwitch(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::DisableImsSwitch MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<ImsSwitchAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::DisableImsSwitch asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }

    return HandleAsyncWork(
        env, asyncContext.release(), "DisableImsSwitch", NativeDisableImsSwitch, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::IsImsSwitchEnabled(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::IsImsSwitchEnabled MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<ImsSwitchAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::IsImsSwitchEnabled asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "IsImsSwitchEnabled", NativeIsImsSwitchEnabled, NativeIsImsSwitchEnabledCallBack);
}

napi_value NapiCallManager::SetVoNRState(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchTwoNumberParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::SetVoNRState MatchTwoNumberParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<VoNRSwitchAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::SetVoNRState asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->state);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "SetVoNRState", NativeSetVoNRState, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::GetVoNRState(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::GetVoNRState MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<VoNRSwitchAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::GetVoNRState asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "GetVoNRState", NativeGetVoNRState, NativeGetVoNRStateCallBack);
}

napi_value NapiCallManager::StartDTMF(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchNumberAndStringParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::StartDTMF MatchNumberAndStringParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::StartDTMF asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_SECOND], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "StartDTMF", NativeStartDTMF, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::StopDTMF(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::StopDTMF MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::StopDTMF asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "StopDTMF", NativeStopDTMF, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::PostDialProceed(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchNumberAndBoolParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::PostDialProceed MatchNumberAndBoolParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<PostDialAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::PostDialProceed asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    napi_get_value_bool(env, argv[ARRAY_INDEX_SECOND], &asyncContext->proceed);
    if (argc == VALUE_MAXIMUM_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "PostDialProceed", NativePostDialProceed,
        NativeVoidCallBackWithErrorCode);
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

napi_value NapiCallManager::GetCallStateSync(napi_env env, napi_callback_info info)
{
    size_t parameterCount = 0;
    napi_value parameters[] = { nullptr };
    napi_get_cb_info(env, info, &parameterCount, parameters, nullptr, nullptr);
    int32_t callState = static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN);
    if (parameterCount == 0) {
        callState = DelayedSingleton<CallManagerClient>::GetInstance()->GetCallState();
    }
    napi_value value = nullptr;
    NAPI_CALL(env, napi_create_int32(env, callState, &value));
    return value;
}

napi_value NapiCallManager::IsRinging(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, ONLY_ONE_VALUE);
    if (!MatchEmptyParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::IsRinging MatchEmptyParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<BoolResultAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::IsRinging asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "IsRinging", NativeIsRinging, NativeBoolCallBackWithErrorCode);
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

napi_value NapiCallManager::HasCallSync(napi_env env, napi_callback_info info)
{
    size_t parameterCount = 0;
    napi_value parameters[] = { nullptr };
    napi_get_cb_info(env, info, &parameterCount, parameters, nullptr, nullptr);
    bool hasCall = false;
    if (parameterCount == 0) {
        hasCall = DelayedSingleton<CallManagerClient>::GetInstance()->HasCall();
    }
    napi_value value = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, hasCall, &value));
    return value;
}

napi_value NapiCallManager::IsNewCallAllowed(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, ONLY_ONE_VALUE);
    if (!MatchEmptyParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::IsNewCallAllowed MatchEmptyParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<BoolResultAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::IsNewCallAllowed asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "IsNewCallAllowed", NativeIsNewCallAllowed, NativeBoolCallBackWithErrorCode);
}

napi_value NapiCallManager::IsInEmergencyCall(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, ONLY_ONE_VALUE);
    if (!MatchEmptyParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::IsInEmergencyCall MatchEmptyParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<BoolResultAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::IsInEmergencyCall asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "IsInEmergencyCall", NativeIsInEmergencyCall, NativeBoolCallBackWithErrorCode);
}

napi_value NapiCallManager::IsEmergencyPhoneNumber(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if (!MatchStringAndVariableObjectParameters(env, argv, argc)) {
        TELEPHONY_LOGE("MatchStringAndVariableObjectParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto emergencyAsyncContext = std::make_unique<UtilsAsyncContext>();
    if (emergencyAsyncContext == nullptr) {
        TELEPHONY_LOGE("emergencyAsyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], emergencyAsyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT,
        &(emergencyAsyncContext->numberLen));
    emergencyAsyncContext->slotId = 0;
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(
                env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(emergencyAsyncContext->callbackRef));
        } else {
            emergencyAsyncContext->slotId =
                NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_SECOND], "slotId");
        }
    }
    if (argc == VALUE_MAXIMUM_LIMIT) {
        emergencyAsyncContext->slotId = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_SECOND], "slotId");
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(emergencyAsyncContext->callbackRef));
    }
    return HandleAsyncWork(env, emergencyAsyncContext.release(), "IsEmergencyPhoneNumber", NativeIsEmergencyPhoneNumber,
        NativeIsEmergencyPhoneNumberCallBack);
}

napi_value NapiCallManager::FormatPhoneNumber(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if (!MatchStringAndVariableObjectParameters(env, argv, argc)) {
        TELEPHONY_LOGE("MatchStringAndVariableObjectParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<UtilsAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    asyncContext->code = "cn";
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        if (NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function)) {
            napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
        } else {
            asyncContext->code = NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_SECOND], "countryCode");
        }
    }
    if (argc == VALUE_MAXIMUM_LIMIT) {
        asyncContext->code = NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_SECOND], "countryCode");
        napi_create_reference(env, argv[ARRAY_INDEX_THIRD], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    if ((argc >= TWO_VALUE_LIMIT) && (asyncContext->code == "")) {
        TELEPHONY_LOGI("country code is undefined, using default country code: 'cn'.");
        asyncContext->code = "cn";
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "FormatPhoneNumber", NativeFormatPhoneNumber, NativeFormatNumberCallBack);
}

napi_value NapiCallManager::FormatPhoneNumberToE164(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if (!MatchTwoStringParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::FormatPhoneNumberToE164 MatchTwoStringParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<UtilsAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::FormatPhoneNumberToE164 asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
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
    if ((argc != TWO_VALUE_LIMIT) ||
        (!NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_string)) ||
        (!NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_SECOND], napi_function))) {
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (registerStatus_ != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterCallBack failed!");
        if (registerStatus_ == TELEPHONY_ERR_PERMISSION_ERR) {
            NapiUtil::ThrowError(env, JS_ERROR_TELEPHONY_PERMISSION_DENIED, OBSERVER_ON_JS_PERMISSION_ERROR_STRING);
            return nullptr;
        }
        JsError error = NapiUtil::ConverErrorMessageForJs(registerStatus_);
        NapiUtil::ThrowError(env, error.errorCode, error.errorMessage);
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
    } else if (tmpStr == "audioDeviceChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterAudioDeviceCallback(stateCallback);
        int32_t result = DelayedSingleton<CallManagerClient>::GetInstance()->ReportAudioDeviceInfo();
        TELEPHONY_LOGI("result == %{public}d", result);
    } else if (tmpStr == "postDialDelay") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterPostDialDelay(stateCallback);
    } else {
        RegisterImsVideoCallFuncCallback(tmpStr, stateCallback);
    }
    napi_value result = nullptr;
    return result;
}

void NapiCallManager::RegisterImsVideoCallFuncCallback(std::string tmpStr, EventCallback stateCallback)
{
    if (tmpStr == "imsCallModeChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterImsCallModeChangeCallback(stateCallback);
    } else if (tmpStr == "callSessionEvent") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCallSessionEventChangeCallback(
            stateCallback);
    } else if (tmpStr == "peerDimensionsChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterPeerDimensionsChangeCallback(stateCallback);
    } else if (tmpStr == "cameraCapabilitiesChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCameraCapabilitiesChangeCallback(
            stateCallback);
    }
}

napi_value NapiCallManager::ObserverOff(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchObserverOffParameter(env, argv, argc)) {
        TELEPHONY_LOGE("MatchObserverOffParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (registerStatus_ != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterCallBack failed!");
        if (registerStatus_ == TELEPHONY_ERR_PERMISSION_ERR) {
            NapiUtil::ThrowError(env, JS_ERROR_TELEPHONY_PERMISSION_DENIED, OBSERVER_OFF_JS_PERMISSION_ERROR_STRING);
            return nullptr;
        }
        JsError error = NapiUtil::ConverErrorMessageForJs(registerStatus_);
        NapiUtil::ThrowError(env, error.errorCode, error.errorMessage);
        return nullptr;
    }
    char listenerType[PHONE_NUMBER_MAXIMUM_LIMIT + 1] = { 0 };
    size_t strLength = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_FIRST], listenerType, PHONE_NUMBER_MAXIMUM_LIMIT, &strLength);
    std::string tmpStr = listenerType;
    TELEPHONY_LOGI("listenerType == %{public}s", tmpStr.c_str());
    UnRegisterCallbackWithListenerType(tmpStr);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "Off", [](napi_env env, void *data) {}, NativeOffCallBack);
}

void NapiCallManager::UnRegisterCallbackWithListenerType(std::string tmpStr)
{
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
    } else if (tmpStr == "audioDeviceChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterAudioDeviceCallback();
    } else if (tmpStr == "postDialDelay") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterPostDialDelayCallback();
    } else if (tmpStr == "imsCallModeChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterImsCallModeChangeCallback();
    } else if (tmpStr == "callSessionEvent") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterCallSessionEventChangeCallback();
    } else if (tmpStr == "peerDimensionsChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterPeerDimensionsChangeCallback();
    } else if (tmpStr == "cameraCapabilitiesChange") {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterCameraCapabilitiesChangeCallback();
    }
}

napi_value NapiCallManager::SetMuted(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, ONLY_ONE_VALUE);
    if (!MatchEmptyParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::SetMuted MatchEmptyParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AudioAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::SetMuted asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "SetMuted", NativeSetMuted, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::CancelMuted(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, ONLY_ONE_VALUE);
    if (!MatchEmptyParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::CancelMuted MatchEmptyParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AudioAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::CancelMuted asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "CancelMuted", NativeCancelMuted, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::MuteRinger(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, ONLY_ONE_VALUE);
    if (!MatchEmptyParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::MuteRinger MatchEmptyParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AudioAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::MuteRinger asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "MuteRinger", NativeMuteRinger, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::SetAudioDevice(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchAudioDeviceParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::SetAudioDevice MatchAudioDeviceParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AudioAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::SetAudioDevice asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    asyncContext->deviceType = NapiCallManagerUtils::GetIntProperty(env, argv[ARRAY_INDEX_FIRST], "deviceType");
    asyncContext->address = NapiCallManagerUtils::GetStringProperty(env, argv[ARRAY_INDEX_FIRST], "address");
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }

    return HandleAsyncWork(
        env, asyncContext.release(), "SetAudioDevice", NativeSetAudioDevice, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::ControlCamera(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchNumberAndStringParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::ControlCamera MatchNumberAndStringParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<VideoAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "ControlCamera error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    char tmpStr[kMaxNumberLen + 1] = {0};
    size_t strLen = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_SECOND], tmpStr, PHONE_NUMBER_MAXIMUM_LIMIT, &strLen);
    std::string tmpCode(tmpStr, strLen);
    asyncContext->cameraId = tmpCode;
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "ControlCamera", NativeControlCamera, NativeVoidCallBack);
}

napi_value NapiCallManager::SetPreviewWindow(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchNumberAndStringParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::ControlCamera MatchNumberAndStringParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }

    auto previwWindowContext = std::make_unique<VideoAsyncContext>();
    if (previwWindowContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetPreviewWindow error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &previwWindowContext->callId);
    char tmpStr[kMaxNumberLen + 1] = { 0 };
    size_t strLen = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_SECOND], tmpStr, MESSAGE_CONTENT_MAXIMUM_LIMIT, &strLen);
    std::string tmpCode(tmpStr, strLen);
    previwWindowContext->surfaceId = tmpCode;
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(previwWindowContext->callbackRef));
    }
    return HandleAsyncWork(env, previwWindowContext.release(), "SetPreviewWindow", NativeSetPreviewWindow,
        NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::SetDisplayWindow(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchNumberAndStringParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::ControlCamera MatchNumberAndStringParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }

    auto dislpayWindowContext = std::make_unique<VideoAsyncContext>();
    if (dislpayWindowContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetDisplayWindow error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &dislpayWindowContext->callId);
    char tmpStr[kMaxNumberLen + 1] = { 0 };
    size_t strLen = 0;
    napi_get_value_string_utf8(env, argv[ARRAY_INDEX_SECOND], tmpStr, MESSAGE_CONTENT_MAXIMUM_LIMIT, &strLen);
    std::string tmpCode(tmpStr, strLen);
    dislpayWindowContext->surfaceId = tmpCode;
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(dislpayWindowContext->callbackRef));
    }
    return HandleAsyncWork(env, dislpayWindowContext.release(), "SetDisplayWindow", NativeSetDisplayWindow,
        NativeVoidCallBackWithErrorCode);
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
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::ControlCamera MatchNumberAndStringParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }

    auto asyncContext = std::make_unique<VideoAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetPausePicture error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "SetPausePicture", NativeSetPausePicture, NativeVoidCallBack);
}

napi_value NapiCallManager::SetDeviceDirection(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchTwoNumberParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::SetDeviceDirection MatchTwoNumberParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }

    auto asyncContext = std::make_unique<VideoAsyncContext>();
    if (asyncContext == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "SetDeviceDirection error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    napi_get_value_int32(env, argv[ARRAY_INDEX_SECOND], &asyncContext->rotation);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "SetDeviceDirection", NativeSetDeviceDirection, NativeVoidCallBack);
}

napi_value NapiCallManager::RequestCameraCapabilities(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::RequestCameraCapabilities MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::RequestCameraCapabilities asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "RequestCameraCapabilities",
        NativeRequestCameraCapabilities, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::CancelCallUpgrade(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::CancelCallUpgrade MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::CancelCallUpgrade asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->callId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "CancelCallUpgrade", NativeCancelCallUpgrade,
        NativeVoidCallBackWithErrorCode);
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
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchNumberAndObjectParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::JoinConference MatchNumberAndObjectParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    uint32_t arrayLength = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[ARRAY_INDEX_SECOND], &arrayLength));
    if (!NapiCallManagerUtils::MatchValueType(env, argv[ARRAY_INDEX_FIRST], napi_number) || arrayLength == 0) {
        TELEPHONY_LOGE("NapiCallManager::JoinConference parameter type matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<ListAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::JoinConference asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
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
        if (!NapiCallManagerUtils::MatchValueType(env, napiFormId, napi_string)) {
            TELEPHONY_LOGE("NapiCallManager::JoinConference parameter type matching failed.");
            NapiUtil::ThrowParameterError(env);
            return nullptr;
        }
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
    return HandleAsyncWork(
        env, asyncContext.release(), "JoinConference", NativeJoinConference, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::UpdateImsCallMode(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, THREE_VALUE_MAXIMUM_LIMIT);
    if (!MatchTwoNumberParameters(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::UpdateImsCallMode MatchTwoNumberParameters failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::UpdateImsCallMode asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
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
        env, asyncContext.release(), "UpdateImsCallMode", NativeUpdateImsCallMode, NativeVoidCallBackWithErrorCode);
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

napi_value NapiCallManager::CloseUnFinishedUssd(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::CloseUnFinishedUssd MatchOneNumberParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<SupplementAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::CloseUnFinishedUssd asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->slotId);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    asyncContext->env = env;
    napi_create_reference(env, thisVar, DATA_LENGTH_ONE, &(asyncContext->thisVar));
    return HandleAsyncWork(
        env, asyncContext.release(), "CloseUnFinishedUssd", NativeCloseUnFinishedUssd, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::InputDialerSpecialCode(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if (!MatchOneStringParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::InputDialerSpecialCode MatchOneStringParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }

    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::InputDialerSpecialCode asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_string_utf8(
        env, argv[ARRAY_INDEX_FIRST], asyncContext->number, PHONE_NUMBER_MAXIMUM_LIMIT, &(asyncContext->numberLen));
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "InputDialerSpecialCode", NativeInputDialerSpecialCode,
        NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::RemoveMissedIncomingCallNotification(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, VALUE_MAXIMUM_LIMIT);
    if (!MatchEmptyParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::RemoveMissedIncomingCallNotification "
                       "MatchEmptyParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }

    auto asyncContext = std::make_unique<AsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::RemoveMissedIncomingCallNotification "
                       "asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }

    if (argc == ONLY_ONE_VALUE) {
        napi_create_reference(env, argv[ARRAY_INDEX_FIRST], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(env, asyncContext.release(), "RemoveMissedIncomingCallNotification",
        NativeRemoveMissedIncomingCallNotification, NativeVoidCallBackWithErrorCode);
}

napi_value NapiCallManager::SetVoIPCallState(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, TWO_VALUE_LIMIT);
    if (!MatchOneNumberParameter(env, argv, argc)) {
        TELEPHONY_LOGE("NapiCallManager::SetVoIPCallState MatchEmptyParameter failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<VoIPCallStateAsyncContext>();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::SetVoIPCallState asyncContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_INDEX_FIRST], &asyncContext->state);
    if (argc == TWO_VALUE_LIMIT) {
        napi_create_reference(env, argv[ARRAY_INDEX_SECOND], DATA_LENGTH_ONE, &(asyncContext->callbackRef));
    }
    return HandleAsyncWork(
        env, asyncContext.release(), "SetVoIPCallState", NativeSetVoIPCallState, NativeVoidCallBackWithErrorCode);
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
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto callBackContext = (AsyncContext *)data;
    if (callBackContext->deferred != nullptr) {
        if (callBackContext->resolved == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_int32(env, callBackContext->resolved, &promiseValue);
            napi_resolve_deferred(env, callBackContext->deferred, promiseValue);
        } else {
            napi_reject_deferred(env, callBackContext->deferred,
                NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(
                    env, callBackContext->errorCode, callBackContext->eventId));
        }
    } else if (callBackContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (callBackContext->resolved == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_int32(env, callBackContext->resolved, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(
                env, callBackContext->errorCode, callBackContext->eventId);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, callBackContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, callBackContext->callbackRef);
    }
    napi_delete_async_work(env, callBackContext->work);
    delete callBackContext;
    callBackContext = nullptr;
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
    auto voidCallBackContext = (AsyncContext *)data;
    if (voidCallBackContext->deferred != nullptr) {
        if (voidCallBackContext->resolved == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_status ret = napi_resolve_deferred(env, voidCallBackContext->deferred, promiseValue);
            TELEPHONY_LOGI("promise successful result = %{public}d", ret);
        } else {
            std::string errTip = std::to_string(voidCallBackContext->resolved);
            napi_status ret = napi_reject_deferred(
                env, voidCallBackContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        }
    } else if (voidCallBackContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (voidCallBackContext->resolved == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_null(env, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(voidCallBackContext->resolved);
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, voidCallBackContext->callbackRef, &callback);
        napi_status ret = napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        TELEPHONY_LOGI("callback result = %{public}d", ret);
        napi_delete_reference(env, voidCallBackContext->callbackRef);
    }
    napi_delete_async_work(env, voidCallBackContext->work);
    delete voidCallBackContext;
    voidCallBackContext = nullptr;
}

void NapiCallManager::NativeVoidCallBackWithErrorCode(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeVoidCallBackWithErrorCode data is nullptr");
        NapiUtil::ThrowParameterError(env);
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
            napi_status ret = napi_reject_deferred(env, asyncContext->deferred,
                NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(
                    env, asyncContext->errorCode, asyncContext->eventId));
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        }
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_null(env, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(
                env, asyncContext->errorCode, asyncContext->eventId);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_value result = nullptr;
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

void NapiCallManager::NativeGetMainCallIdCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeGetMainCallIdCallBack data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (IntResultAsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_int32(env, asyncContext->result, &promiseValue);
            napi_status ret = napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
            TELEPHONY_LOGI("promise successful result = %{public}d", ret);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(asyncContext->errorCode);
            napi_status ret = napi_reject_deferred(env, asyncContext->deferred,
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode));
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        }
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (asyncContext->resolved == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_int32(env, asyncContext->result, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(asyncContext->errorCode);
            callbackValue[ARRAY_INDEX_FIRST] =
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_value result = nullptr;
        napi_status ret = napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        TELEPHONY_LOGI("callback result = %{public}d", ret);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
    asyncContext = nullptr;
}

void NapiCallManager::NativeIsImsSwitchEnabledCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeIsImsSwitchEnabledCallBack data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (ImsSwitchAsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, asyncContext->enabled, &promiseValue);
            napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(asyncContext->errorCode);
            napi_reject_deferred(env, asyncContext->deferred,
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, asyncContext->enabled, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(asyncContext->errorCode);
            callbackValue[ARRAY_INDEX_FIRST] =
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode);
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

void NapiCallManager::NativeGetVoNRStateCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeGetVoNRStateCallBack data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto getVoNRStateContext = (VoNRSwitchAsyncContext *)data;
    if (getVoNRStateContext->deferred != nullptr) {
        if (getVoNRStateContext->errorCode == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_int32(env, getVoNRStateContext->state, &promiseValue);
            napi_resolve_deferred(env, getVoNRStateContext->deferred, promiseValue);
        } else {
            napi_reject_deferred(env, getVoNRStateContext->deferred,
                NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(
                    env, getVoNRStateContext->errorCode, getVoNRStateContext->eventId));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (getVoNRStateContext->errorCode == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_int32(env, getVoNRStateContext->state, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(
                env, getVoNRStateContext->errorCode, getVoNRStateContext->eventId);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, getVoNRStateContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, getVoNRStateContext->callbackRef);
    }
    napi_delete_async_work(env, getVoNRStateContext->work);
    delete getVoNRStateContext;
    getVoNRStateContext = nullptr;
}

void NapiCallManager::NativeBoolCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto boolCallBackContext = (AsyncContext *)data;
    if (boolCallBackContext->deferred != nullptr) {
        if (boolCallBackContext->resolved == BOOL_VALUE_IS_TRUE) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, true, &promiseValue);
            napi_resolve_deferred(env, boolCallBackContext->deferred, promiseValue);
        } else if (boolCallBackContext->errorCode == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, false, &promiseValue);
            napi_resolve_deferred(env, boolCallBackContext->deferred, promiseValue);
        } else if (boolCallBackContext->errorCode == SLOT_ID_INVALID) {
            napi_reject_deferred(env, boolCallBackContext->deferred,
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, "slotId is invalid", SLOT_ID_INVALID));
        } else {
            std::string errTip = std::to_string(boolCallBackContext->errorCode);
            napi_reject_deferred(
                env, boolCallBackContext->deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    } else {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (boolCallBackContext->resolved == BOOL_VALUE_IS_TRUE) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, true, &callbackValue[ARRAY_INDEX_SECOND]);
        } else if (boolCallBackContext->errorCode == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, false, &callbackValue[ARRAY_INDEX_SECOND]);
        } else if (boolCallBackContext->errorCode == SLOT_ID_INVALID) {
            callbackValue[ARRAY_INDEX_FIRST] =
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, "slotId is invalid", SLOT_ID_INVALID);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        } else {
            std::string errTip = std::to_string(boolCallBackContext->errorCode);
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, boolCallBackContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, boolCallBackContext->callbackRef);
    }
    napi_delete_async_work(env, boolCallBackContext->work);
    delete boolCallBackContext;
    boolCallBackContext = nullptr;
}

void NapiCallManager::NativeIsEmergencyPhoneNumberCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeIsEmergencyPhoneNumberCallBack data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (UtilsAsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, asyncContext->enabled, &promiseValue);
            napi_status ret = napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(asyncContext->errorCode);
            napi_status ret = napi_reject_deferred(env, asyncContext->deferred,
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode));
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        }
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, asyncContext->enabled, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(asyncContext->errorCode);
            callbackValue[ARRAY_INDEX_FIRST] =
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode);
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

void NapiCallManager::NativeBoolCallBackWithErrorCode(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeBoolCallBackWithErrorCode data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (BoolResultAsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_boolean(env, asyncContext->enabled, &promiseValue);
            napi_status ret = napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        } else {
            napi_status ret = napi_reject_deferred(env, asyncContext->deferred,
                NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(
                    env, asyncContext->errorCode, asyncContext->eventId));
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        }
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_boolean(env, asyncContext->enabled, &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(
                env, asyncContext->errorCode, asyncContext->eventId);
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

void NapiCallManager::NativeFormatNumberCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeFormatNumberCallBack data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto formatNumberContext = (UtilsAsyncContext *)data;
    if (formatNumberContext->deferred != nullptr) {
        if (formatNumberContext->resolved == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_string_utf8(env, Str16ToStr8(formatNumberContext->formatNumber).data(),
                formatNumberContext->formatNumber.length(), &promiseValue);
            napi_status ret = napi_resolve_deferred(env, formatNumberContext->deferred, promiseValue);
            TELEPHONY_LOGI("promise successful result = %{public}d", ret);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(formatNumberContext->errorCode);
            napi_status ret = napi_reject_deferred(env, formatNumberContext->deferred,
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode));
            TELEPHONY_LOGI("promise failed result = %{public}d", ret);
        }
    } else if (formatNumberContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (formatNumberContext->resolved == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_string_utf8(env, Str16ToStr8(formatNumberContext->formatNumber).data(),
                formatNumberContext->formatNumber.length(), &callbackValue[ARRAY_INDEX_SECOND]);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(formatNumberContext->errorCode);
            callbackValue[ARRAY_INDEX_FIRST] =
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, formatNumberContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, formatNumberContext->callbackRef);
    }
    napi_delete_async_work(env, formatNumberContext->work);
    delete formatNumberContext;
    formatNumberContext = nullptr;
}

void NapiCallManager::NativeListCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        NapiUtil::ThrowParameterError(env);
        return;
    }
    int32_t i = 0;
    napi_value info = nullptr;
    auto listCallBakcContext = (ListAsyncContext *)data;
    std::vector<std::u16string>::iterator it = listCallBakcContext->listResult.begin();
    if (listCallBakcContext->deferred != nullptr) {
        if (listCallBakcContext->resolved == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_array(env, &promiseValue);
            for (; it != listCallBakcContext->listResult.end(); ++it) {
                napi_create_string_utf8(env, Str16ToStr8(*it).data(), (*it).length(), &info);
                napi_set_element(env, promiseValue, i, info);
                ++i;
            }
            napi_resolve_deferred(env, listCallBakcContext->deferred, promiseValue);
        } else {
            napi_reject_deferred(env, listCallBakcContext->deferred,
                NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(
                    env, listCallBakcContext->errorCode, listCallBakcContext->eventId));
        }
    } else if (listCallBakcContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        if (listCallBakcContext->resolved == TELEPHONY_SUCCESS) {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_array(env, &callbackValue[ARRAY_INDEX_SECOND]);
            for (; it != listCallBakcContext->listResult.end(); ++it) {
                napi_create_string_utf8(env, Str16ToStr8(*it).data(), (*it).length(), &info);
                napi_set_element(env, callbackValue[ARRAY_INDEX_SECOND], i, info);
                ++i;
            }
        } else {
            callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorCodeAndMessageForJs(
                env, listCallBakcContext->errorCode, listCallBakcContext->eventId);
            callbackValue[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, listCallBakcContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, listCallBakcContext->callbackRef);
    }
    napi_delete_async_work(env, listCallBakcContext->work);
    delete listCallBakcContext;
    listCallBakcContext = nullptr;
}

void NapiCallManager::NativeOffCallBack(napi_env env, napi_status status, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeOffCallBack data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->deferred != nullptr) {
        napi_value promiseValue = nullptr;
        napi_get_null(env, &promiseValue);
        napi_status ret = napi_resolve_deferred(env, asyncContext->deferred, promiseValue);
        TELEPHONY_LOGI("promise result = %{public}d", ret);
    } else if (asyncContext->callbackRef != nullptr) {
        napi_value callbackValue[ARRAY_INDEX_THIRD] = { 0 };
        callbackValue[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_status ret = napi_call_function(env, nullptr, callback, DATA_LENGTH_ONE, callbackValue, &result);
        TELEPHONY_LOGI("callback result = %{public}d", ret);
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
    int32_t type = 0;
    int32_t mode = 0;
    std::string pw = "";
    if (!NapiCallManagerUtils::GetUssdIntProperty(env, objValue, "type", type) ||
        !NapiCallManagerUtils::GetUssdIntProperty(env, objValue, "mode", mode) ||
        !NapiCallManagerUtils::GetUssdStringProperty(env, objValue, "password", pw)) {
        NapiUtil::ThrowParameterError(env);
        return ERROR_PARAMETER_TYPE_INVALID;
    }
    if (pw.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
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
    int32_t type = 0;
    int32_t settingType = 0;
    std::string transferNum = "";
    if (!NapiCallManagerUtils::GetUssdIntProperty(env, objValue, "type", type) ||
        !NapiCallManagerUtils::GetUssdIntProperty(env, objValue, "settingType", settingType) ||
        !NapiCallManagerUtils::GetUssdStringProperty(env, objValue, "transferNum", transferNum)) {
        return ERROR_PARAMETER_TYPE_INVALID;
    }
    int32_t startHour = NapiCallManagerUtils::GetIntProperty(env, objValue, "startHour");
    int32_t startMinute = NapiCallManagerUtils::GetIntProperty(env, objValue, "startMinute");
    int32_t endHour = NapiCallManagerUtils::GetIntProperty(env, objValue, "endHour");
    int32_t endMinute = NapiCallManagerUtils::GetIntProperty(env, objValue, "endMinute");
    if (transferNum.length() > static_cast<size_t>(kMaxNumberLen)) {
        TELEPHONY_LOGE("Number out of limit!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (memcpy_s(asyncContext.info.transferNum, kMaxNumberLen, transferNum.c_str(), transferNum.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return TELEPHONY_ERR_MEMCPY_FAIL;
    }
    asyncContext.info.settingType = static_cast<CallTransferSettingType>(settingType);
    asyncContext.info.type = static_cast<CallTransferType>(type);
    asyncContext.info.startHour = startHour;
    asyncContext.info.startMinute = startMinute;
    asyncContext.info.endHour = endHour;
    asyncContext.info.endMinute = endMinute;
    TELEPHONY_LOGI(
        "GetTransferInfo: type = %{public}d, settingType = %{public}d, startHour = %{public}d, startMinute = "
        "%{public}d, endHour = %{public}d, endMinute = %{public}d",
        asyncContext.info.type, asyncContext.info.settingType, asyncContext.info.startHour,
        asyncContext.info.startMinute, asyncContext.info.endHour, asyncContext.info.endMinute);
    return TELEPHONY_SUCCESS;
}

void NapiCallManager::NativeDial(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (DialAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->accountId)) {
        TELEPHONY_LOGE("NativeDial slotId is invalid");
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

void NapiCallManager::NativeDialCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeDialCall data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (DialAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->accountId)) {
        TELEPHONY_LOGE("NativeDialCall slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    asyncContext->eventId = CALL_MANAGER_DIAL_CALL;
    std::string phoneNumber(asyncContext->number, asyncContext->numberLen);
    std::u16string tmpPhoneNumber = Str8ToStr16(phoneNumber);
    bool isEmergencyNumber = false;
    if(!(DelayedSingleton<CallManagerClient>::GetInstance()->IsEmergencyPhoneNumber(
        tmpPhoneNumber, asyncContext->accountId, isEmergencyNumber) == TELEPHONY_SUCCESS && isEmergencyNumber)) {
        int32_t state;
        DelayedSingleton<CallManagerClient>::GetInstance()->GetVoIPCallState(state);
        if (state == (int32_t)CallStateToApp::CALL_STATE_OFFHOOK
            || state == (int32_t)CallStateToApp::CALL_STATE_RINGING) {
            TELEPHONY_LOGE("VoIP CALL is active, cannot dial now");
            JsError error = NapiUtil::ConverErrorMessageForJs(CALL_ERR_CALL_COUNTS_EXCEED_LIMIT);
            asyncContext->errorCode = error.errorCode;
            return;
        }
    }
   
    OHOS::AppExecFwk::PacMap dialInfo;
    dialInfo.PutIntValue("accountId", asyncContext->accountId);
    dialInfo.PutIntValue("videoState", asyncContext->videoState);
    dialInfo.PutIntValue("dialScene", asyncContext->dialScene);
    dialInfo.PutIntValue("dialType", asyncContext->dialType);
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->DialCall(Str8ToStr16(phoneNumber), dialInfo);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeMakeCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeMakeCall data is nullptr");
        NapiUtil::ThrowParameterError(env);
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
    wantParams.SetParam(AAFwk::Want::PARAM_BACK_TO_OTHER_MISSION_STACK, AAFwk::Boolean::Box(true));
    want.SetParams(wantParams);
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (err != ERR_OK) {
        TELEPHONY_LOGE("Fail to make call, err:%{public}d", err);
        asyncContext->errorCode = TELEPHONY_ERR_UNINIT;
        return;
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
}

void NapiCallManager::NativeAnswerCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeAnswerCall data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AnswerAsyncContext *)data;
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->AnswerCall(asyncContext->callId, asyncContext->videoState);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_ANSWER_CALL;
}

void NapiCallManager::NativeRejectCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeRejectCall data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (RejectAsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->RejectCall(
        asyncContext->callId, asyncContext->isSendSms, Str8ToStr16(asyncContext->messageContent));
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_REJECT_CALL;
}

void NapiCallManager::NativeHangUpCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeHangUpCall data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->HangUpCall(asyncContext->callId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_DISCONNECT_CALL;
}

void NapiCallManager::NativeHoldCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeHoldCall data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->HoldCall(asyncContext->callId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_HOLD_CALL;
}

void NapiCallManager::NativeUnHoldCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeUnHoldCall data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->UnHoldCall(asyncContext->callId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_UNHOLD_CALL;
}

void NapiCallManager::NativeSwitchCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeSwitchCall data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->SwitchCall(asyncContext->callId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_SWITCH_CALL;
}

void NapiCallManager::NativeCombineConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeCombineConference data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->CombineConference(asyncContext->callId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeSeparateConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeSeparateConference data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->SeparateConference(asyncContext->callId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeKickOutFromConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeKickOutFromConference data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->KickOutFromConference(asyncContext->callId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeGetMainCallId(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeGetMainCallId data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (IntResultAsyncContext *)data;
    asyncContext->result = 0;
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetMainCallId(asyncContext->callId, asyncContext->result);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeGetSubCallIdList(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeGetSubCallIdList data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (ListAsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->GetSubCallIdList(
        asyncContext->callId, asyncContext->listResult);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeGetCallIdListForConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeGetCallIdListForConference data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (ListAsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->GetCallIdListForConference(
        asyncContext->callId, asyncContext->listResult);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

/**
 * Register a callback to NapiCallAbilityCallback. When call_manager has data returned,
 * NapiCallAbilityCallback will notify JavaScript via the registered callback.
 * The whole process is asynchronous.
 */
void NapiCallManager::NativeGetCallWaiting(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeGetCallWaiting data is nullptr");
        NapiUtil::ThrowParameterError(env);
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
    asyncContext->errorCode =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetWaitingCallback(infoListener);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetWaitingCallback failed!");
        return;
    }
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->GetCallWaiting(asyncContext->slotId);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        asyncContext->eventId = CALL_MANAGER_GET_CALL_WAITING;
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetWaitingCallback();
        TELEPHONY_LOGE("GetCallWaiting failed!");
        return;
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeSetCallWaiting(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeSetCallWaiting data is nullptr");
        NapiUtil::ThrowParameterError(env);
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
    asyncContext->errorCode =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetWaitingCallback(infoListener);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetWaitingCallback failed!");
        return;
    }
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetCallWaiting(asyncContext->slotId, asyncContext->flag);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        asyncContext->eventId = CALL_MANAGER_SET_CALL_WAITING;
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterSetWaitingCallback();
        TELEPHONY_LOGE("SetCallWaiting failed!");
        return;
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeGetCallRestriction(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeGetCallRestriction data is nullptr");
        NapiUtil::ThrowParameterError(env);
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
    asyncContext->errorCode =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetRestrictionCallback(infoListener);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetRestrictionCallback failed!");
        return;
    }
    CallRestrictionType type = static_cast<CallRestrictionType>(asyncContext->type);
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetCallRestriction(asyncContext->slotId, type);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        asyncContext->eventId = CALL_MANAGER_GET_CALL_RESTRICTION;
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetRestrictionCallback();
        TELEPHONY_LOGE("GetCallRestriction failed!");
        return;
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeSetCallRestriction(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeSetCallRestriction data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (CallRestrictionAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeSetCallRestriction slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Data acquisition failure!");
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->errorCode =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetRestrictionCallback(infoListener);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetRestrictionCallback failed!");
        return;
    }
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->SetCallRestriction(
        asyncContext->slotId, asyncContext->info);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        asyncContext->eventId = CALL_MANAGER_SET_CALL_RESTRICTION;
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterSetRestrictionCallback();
        TELEPHONY_LOGE("SetCallRestriction failed!");
        return;
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeSetCallRestrictionPassword(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (CallBarringPasswordAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->errorCode =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetRestrictionPasswordCallback(infoListener);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetRestrictionPasswordCallback failed!");
        return;
    }
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetCallRestrictionPassword(asyncContext->slotId,
        CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS, asyncContext->oldPassword, asyncContext->newPassword);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        asyncContext->eventId = CALL_MANAGER_SET_CALL_RESTRICTION_PASSWORD;
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterSetRestrictionPasswordCallback();
        TELEPHONY_LOGE("SetCallRestrictionPassword failed!");
        return;
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
    (void)memset_s(asyncContext->oldPassword, sizeof(asyncContext->oldPassword), 0, sizeof(asyncContext->oldPassword));
    (void)memset_s(asyncContext->newPassword, sizeof(asyncContext->newPassword), 0, sizeof(asyncContext->newPassword));
}

void NapiCallManager::NativeGetTransferNumber(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeGetTransferNumber data is nullptr");
        NapiUtil::ThrowParameterError(env);
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
    asyncContext->errorCode = DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterGetTransferCallback(
        infoListener, asyncContext->type);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterGetTransferCallback failed!");
        return;
    }
    CallTransferType type = static_cast<CallTransferType>(asyncContext->type);
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->GetCallTransferInfo(asyncContext->slotId, type);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        asyncContext->eventId = CALL_MANAGER_GET_CALL_TRANSFER;
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterGetTransferCallback();
        TELEPHONY_LOGE("GetCallTransferInfo failed!");
        return;
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeSetTransferNumber(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeSetTransferNumber data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (CallTransferAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeSetTransferNumber slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }

    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Data acquisition failure!");
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->errorCode =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterSetTransferCallback(infoListener);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetTransferCallback failed!");
        return;
    }
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->SetCallTransferInfo(
        asyncContext->slotId, asyncContext->info);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        asyncContext->eventId = CALL_MANAGER_SET_CALL_TRANSFER;
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterSetTransferCallback();
        TELEPHONY_LOGE("SetCallTransferInfo failed!");
        return;
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeCanSetCallTransferTime(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (BoolResultAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("[slot%{public}d]slotId is invalid", asyncContext->slotId);
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->CanSetCallTransferTime(
        asyncContext->slotId, asyncContext->enabled);
}

void NapiCallManager::NativeEnableImsSwitch(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeEnableImsSwitch data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (ImsSwitchAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeEnableImsSwitch slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->EnableImsSwitch(asyncContext->slotId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_ENABLE_IMS_SWITCH;
}

void NapiCallManager::NativeDisableImsSwitch(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeDisableImsSwitch data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (ImsSwitchAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeDisableImsSwitch slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->DisableImsSwitch(asyncContext->slotId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_DISABLE_IMS_SWITCH;
}

void NapiCallManager::NativeIsImsSwitchEnabled(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeIsImsSwitchEnabled data is nullptr");
        NapiUtil::ThrowParameterError(env);
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

void NapiCallManager::NativeSetVoNRState(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeSetVoNRState data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (VoNRSwitchAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeSetVoNRState slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->SetVoNRState(
        asyncContext->slotId, asyncContext->state);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_SET_VONR_STATE;
}

void NapiCallManager::NativeGetVoNRState(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeGetVoNRState data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (VoNRSwitchAsyncContext *)data;
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeIsImsSwitchEnabled slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->GetVoNRState(
        asyncContext->slotId, asyncContext->state);
    asyncContext->eventId = CALL_MANAGER_GET_VONR_STATE;
}

void NapiCallManager::NativeStartDTMF(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeStartDTMF data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    if (asyncContext->numberLen < TWO_VALUE_LIMIT) {
        asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->StartDtmf(
            asyncContext->callId, asyncContext->number[ARRAY_INDEX_FIRST]);
    } else {
        asyncContext->errorCode = TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeStopDTMF(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeStopDTMF data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->StopDtmf(asyncContext->callId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativePostDialProceed(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativePostDialProceed data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (PostDialAsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->PostDialProceed(
        asyncContext->callId, asyncContext->proceed);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
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
        TELEPHONY_LOGE("NapiCallManager::NativeIsRinging data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (BoolResultAsyncContext *)data;
    asyncContext->enabled = false;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->IsRinging(asyncContext->enabled);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_IS_RINGING;
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
        TELEPHONY_LOGE("NapiCallManager::NativeIsNewCallAllowed data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (BoolResultAsyncContext *)data;
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->IsNewCallAllowed(asyncContext->enabled);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeIsInEmergencyCall(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeIsInEmergencyCall data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (BoolResultAsyncContext *)data;
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->IsInEmergencyCall(asyncContext->enabled);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_IS_EMERGENCY_CALL;
}

void NapiCallManager::NativeIsEmergencyPhoneNumber(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeIsEmergencyPhoneNumber data is nullptr");
        NapiUtil::ThrowParameterError(env);
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
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->IsEmergencyPhoneNumber(
        phoneNumber, asyncContext->slotId, asyncContext->enabled);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeFormatPhoneNumber(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeFormatPhoneNumber data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (UtilsAsyncContext *)data;
    std::string tmpPhoneNumber(asyncContext->number, asyncContext->numberLen);
    std::u16string phoneNumber = Str8ToStr16(tmpPhoneNumber);
    std::u16string countryCode = Str8ToStr16(asyncContext->code);
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
        phoneNumber, countryCode, asyncContext->formatNumber);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeFormatPhoneNumberToE164(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeFormatPhoneNumberToE164 data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (UtilsAsyncContext *)data;
    std::string tmpPhoneNumber(asyncContext->number, asyncContext->numberLen);
    std::u16string phoneNumber = Str8ToStr16(tmpPhoneNumber);
    std::u16string countryCode = Str8ToStr16(asyncContext->code);
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumberToE164(
        phoneNumber, countryCode, asyncContext->formatNumber);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeSetMuted(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeSetMuted data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AudioAsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->SetMuted(true);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeCancelMuted(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeCancelMuted data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AudioAsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->SetMuted(false);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeMuteRinger(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeMuteRinger data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AudioAsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->MuteRinger();
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_MUTE_RINGER;
}

void NapiCallManager::NativeSetAudioDevice(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeSetAudioDevice data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AudioAsyncContext *)data;
    AudioDevice device;
    if (memset_s(&device, sizeof(AudioDevice), 0, sizeof(AudioDevice)) != EOK) {
        TELEPHONY_LOGE("memset_s fail");
        return;
    }
    device.deviceType = static_cast<AudioDeviceType>(asyncContext->deviceType);
    if (asyncContext->address.length() > kMaxAddressLen) {
        TELEPHONY_LOGE("address is not too long");
        return;
    }
    if (memcpy_s(device.address, kMaxAddressLen, asyncContext->address.c_str(), asyncContext->address.length()) !=
        EOK) {
        TELEPHONY_LOGE("memcpy_s address fail");
        return;
    }
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->SetAudioDevice(device);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeUpdateImsCallMode(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeUpdateImsCallMode data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (SupplementAsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->UpdateImsCallMode(
        asyncContext->callId, (ImsCallMode)asyncContext->type);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeControlCamera(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->ControlCamera(
        asyncContext->callId, Str8ToStr16(asyncContext->cameraId));
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeSetPreviewWindow(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    TELEPHONY_LOGI("surfaceId: %{public}s", asyncContext->surfaceId.c_str());
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->SetPreviewWindow(
        asyncContext->callId, asyncContext->surfaceId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeSetDisplayWindow(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    TELEPHONY_LOGI("surfaceId: %{public}s", asyncContext->surfaceId.c_str());
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->SetDisplayWindow(
        asyncContext->callId, asyncContext->surfaceId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
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
        DelayedSingleton<CallManagerClient>::GetInstance()->SetPausePicture(
            asyncContext->callId, Str8ToStr16(asyncContext->path));
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeSetDeviceDirection(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (VideoAsyncContext *)data;
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->SetDeviceDirection(
            asyncContext->callId, asyncContext->rotation);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeRequestCameraCapabilities(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->RequestCameraCapabilities(asyncContext->callId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeCancelCallUpgrade(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeCancelCallUpgrade data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (AsyncContext *)data;
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->CancelCallUpgrade(asyncContext->callId);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
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
    auto startRTTContext = (SupplementAsyncContext *)data;
    EventCallback infoListener;
    infoListener.env = startRTTContext->env;
    infoListener.thisVar = startRTTContext->thisVar;
    infoListener.callbackRef = startRTTContext->callbackRef;
    infoListener.deferred = startRTTContext->deferred;
    startRTTContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterStartRttCallback(infoListener);
    if (startRTTContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterStartRttCallback failed!");
        return;
    }

    std::u16string msg = Str8ToStr16(startRTTContext->content);
    startRTTContext->resolved =
        DelayedSingleton<CallManagerClient>::GetInstance()->StartRtt(startRTTContext->callId, msg);
    if (startRTTContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterStartRttCallback();
        TELEPHONY_LOGE("StartRtt failed!");
        return;
    }
    startRTTContext->callbackRef = nullptr;
    startRTTContext->deferred = nullptr;
}

void NapiCallManager::NativeStopRTT(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("data is nullptr");
        return;
    }
    auto stopRTTContext = (SupplementAsyncContext *)data;
    EventCallback infoListener;
    infoListener.env = stopRTTContext->env;
    infoListener.thisVar = stopRTTContext->thisVar;
    infoListener.callbackRef = stopRTTContext->callbackRef;
    infoListener.deferred = stopRTTContext->deferred;
    stopRTTContext->resolved =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterStopRttCallback(infoListener);
    if (stopRTTContext->resolved != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterStopRttCallback failed!");
        return;
    }
    stopRTTContext->resolved = DelayedSingleton<CallManagerClient>::GetInstance()->StopRtt(stopRTTContext->callId);
    if (stopRTTContext->resolved != TELEPHONY_SUCCESS) {
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterStopRttCallback();
        TELEPHONY_LOGE("StopRtt failed!");
        return;
    }
    stopRTTContext->callbackRef = nullptr;
    stopRTTContext->deferred = nullptr;
}

void NapiCallManager::NativeJoinConference(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeJoinConference data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (ListAsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->JoinConference(
        asyncContext->callId, asyncContext->listResult);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
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

void NapiCallManager::NativeCloseUnFinishedUssd(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeCloseUnFinishedUssd data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }

    SupplementAsyncContext *asyncContext = static_cast<SupplementAsyncContext *>(data);
    if (!IsValidSlotId(asyncContext->slotId)) {
        TELEPHONY_LOGE("NativeCloseUnFinishedUssd slotId is invalid");
        asyncContext->errorCode = SLOT_ID_INVALID;
        return;
    }
    EventCallback infoListener;
    infoListener.env = asyncContext->env;
    infoListener.thisVar = asyncContext->thisVar;
    infoListener.callbackRef = asyncContext->callbackRef;
    infoListener.deferred = asyncContext->deferred;
    asyncContext->errorCode =
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->RegisterCloseUnFinishedUssdCallback(infoListener);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RegisterSetWaitingCallback failed!");
        return;
    }

    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->CloseUnFinishedUssd(asyncContext->slotId);
    if (asyncContext->errorCode != TELEPHONY_SUCCESS) {
        asyncContext->eventId = CALL_MANAGER_CLOSE_UNFINISHED_USSD;
        DelayedSingleton<NapiCallAbilityCallback>::GetInstance()->UnRegisterCloseUnFinishedUssdCallback();
        TELEPHONY_LOGE("NativeCloseUnFinishedUssd failed!");
        return;
    }
    asyncContext->resolved = TELEPHONY_SUCCESS;
    asyncContext->callbackRef = nullptr;
    asyncContext->deferred = nullptr;
}

void NapiCallManager::NativeInputDialerSpecialCode(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeInputDialerSpecialCode data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    AsyncContext *asyncContext = static_cast<AsyncContext *>(data);
    std::string specialCode(asyncContext->number, asyncContext->numberLen);
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->InputDialerSpecialCode(specialCode);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeRemoveMissedIncomingCallNotification(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeRemoveMissedIncomingCallNotification data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    AsyncContext *asyncContext = static_cast<AsyncContext *>(data);
    asyncContext->errorCode =
        DelayedSingleton<CallManagerClient>::GetInstance()->RemoveMissedIncomingCallNotification();
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
}

void NapiCallManager::NativeSetVoIPCallState(napi_env env, void *data)
{
    if (data == nullptr) {
        TELEPHONY_LOGE("NapiCallManager::NativeSetVoIPCallState data is nullptr");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    auto asyncContext = (VoIPCallStateAsyncContext *)data;
    asyncContext->errorCode = DelayedSingleton<CallManagerClient>::GetInstance()->SetVoIPCallState(
        asyncContext->state);
    if (asyncContext->errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = TELEPHONY_SUCCESS;
    }
    asyncContext->eventId = CALL_MANAGER_SET_VOIP_CALL_STATE;
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
    napi_queue_async_work_with_qos(env, context->work, napi_qos_default);
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
