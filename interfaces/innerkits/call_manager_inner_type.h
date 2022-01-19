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

#ifndef CALL_MANAGER_INNER_TYPE_H
#define CALL_MANAGER_INNER_TYPE_H

#include <cstdio>
#include <string>
#include <vector>
#include <ctime>

namespace OHOS {
namespace Telephony {
constexpr int16_t kMaxNumberLen = 100;
constexpr int16_t kMaxBundleNameLen = 100;
constexpr uint16_t REJECT_CALL_MSG_MAX_LEN = 300;
constexpr uint16_t ACCOUNT_NUMBER_MAX_LENGTH = 100;
constexpr uint16_t CONNECT_SERVICE_WAIT_TIME = 1000; // ms
constexpr int16_t ERR_ID = -1;

// call type
enum class CallType {
    TYPE_CS = 0, // CS
    TYPE_IMS = 1, // IMS
    TYPE_OTT = 2, // OTT
    TYPE_ERR_CALL = 3, // OTHER
};

// call state
enum class TelCallState {
    CALL_STATUS_ACTIVE = 0,
    CALL_STATUS_HOLDING,
    CALL_STATUS_DIALING,
    CALL_STATUS_ALERTING,
    CALL_STATUS_INCOMING,
    CALL_STATUS_WAITING,
    CALL_STATUS_DISCONNECTED,
    CALL_STATUS_DISCONNECTING,
    CALL_STATUS_IDLE,
};

enum class TelConferenceState {
    TEL_CONFERENCE_IDLE = 0,
    TEL_CONFERENCE_ACTIVE,
    TEL_CONFERENCE_HOLDING,
    TEL_CONFERENCE_DISCONNECTING,
    TEL_CONFERENCE_DISCONNECTED,
};

// phone type
enum class PhoneNetType {
    PHONE_TYPE_GSM = 1, // gsm
    PHONE_TYPE_CDMA = 2, // cdma
};

// call mode
enum class VideoStateType {
    TYPE_VOICE = 0, // Voice
    TYPE_VIDEO, // Video
};

enum class DialScene {
    CALL_NORMAL = 0,
    CALL_PRIVILEGED,
    CALL_EMERGENCY,
};

enum class CallDirection {
    CALL_DIRECTION_OUT = 0,
    CALL_DIRECTION_IN,
    CALL_DIRECTION_UNKNOW,
};

enum class CallRunningState {
    CALL_RUNNING_STATE_CREATE = 0, // A new session
    CALL_RUNNING_STATE_CONNECTING,
    CALL_RUNNING_STATE_DIALING,
    CALL_RUNNING_STATE_RINGING,
    CALL_RUNNING_STATE_ACTIVE,
    CALL_RUNNING_STATE_HOLD,
    CALL_RUNNING_STATE_ENDED,
    CALL_RUNNING_STATE_ENDING,
};

enum class CallEndedType {
    UNKNOWN = 0,
    PHONE_IS_BUSY,
    INVALID_NUMBER,
    CALL_ENDED_NORMALLY,
};

struct SIMCardInfo {
    int32_t simId; // IccId
    int32_t country;
    int32_t state; // SIM card active status
    PhoneNetType phoneNetType;
};

enum class DialType {
    DIAL_CARRIER_TYPE = 0,
    DIAL_VOICE_MAIL_TYPE,
    DIAL_OTT_TYPE,
};

enum class CallStateToApp {
    /**
     * Indicates an invalid state, which is used when the call state fails to be obtained.
     */
    CALL_STATE_UNKNOWN = -1,

    /**
     * Indicates that there is no ongoing call.
     */
    CALL_STATE_IDLE = 0,

    /**
     * Indicates that an incoming call is ringing or waiting.
     */
    CALL_STATE_RINGING = 1,

    /**
     * Indicates that a least one call is in the dialing, active, or hold state, and there is no new incoming call
     * ringing or waiting.
     */
    CALL_STATE_OFFHOOK = 2
};

enum class CallAnswerType {
    CALL_ANSWER_MISSED = 0,
    CALL_ANSWER_ACTIVED,
    CALL_ANSWER_REJECT,
};

struct CallAttributeInfo {
    char accountNumber[kMaxNumberLen + 1];
    char bundleName[kMaxBundleNameLen + 1];
    bool speakerphoneOn;
    int32_t accountId;
    VideoStateType videoState;
    int64_t startTime; // Call start time
    bool isEcc;
    CallType callType;
    int32_t callId;
    TelCallState callState;
    TelConferenceState conferenceState;
    time_t callBeginTime;
    time_t callEndTime;
    time_t ringBeginTime;
    time_t ringEndTime;
    CallDirection callDirection;
    CallAnswerType answerType;
};

struct CallRecordInfo {
    int32_t callId;
    char phoneNumber[kMaxNumberLen + 1];
    CallType callType;
    time_t callBeginTime;
    time_t callEndTime;
    uint32_t ringDuration;
    uint32_t callDuration;
    CallDirection directionType;
    CallAnswerType answerType;
    int32_t countryCode;
};

enum class CallAbilityEventId {
    EVENT_DIAL_NO_CARRIER = 1,
    EVENT_INVALID_FDN_NUMBER,
    EVENT_OTT_FUNCTION_UNSUPPORTED,
};

struct CallEventInfo {
    CallAbilityEventId eventId;
    char phoneNum[kMaxNumberLen + 1];
    char bundleName[kMaxBundleNameLen + 1];
};

struct AccountInfo {
    int32_t accountId;
    int32_t power;
    char bundleName[kMaxNumberLen + 1];
    bool isEnabled;
};

struct CallReportInfo {
    int32_t index;
    char accountNum[kMaxNumberLen + 1]; // call phone number
    int32_t accountId;
    CallType callType; // call type: CS、IMS
    VideoStateType callMode; // call mode: video or audio
    TelCallState state; // call state
    int32_t voiceDomain; // 0:CS、1:IMS
};

struct CallsReportInfo {
    std::vector<CallReportInfo> callVec;
    int32_t slotId;
};

/*
 * 3GPP TS 24.008
 * V17.4.0 10.5.4.11 Cause
 * The purpose of the cause information element is to describe the reason for generating
 * certain messages, to provide diagnostic information in the event of procedural
 * errors and to indicate the location of the cause originator.
 */
enum class DisconnectedDetails : int32_t {
    UNASSIGNED_NUMBER = 1,
    NO_ROUTE_TO_DESTINATION = 3,
    CHANNEL_UNACCEPTABLE = 6,
    OPERATOR_DETERMINED_BARRING = 8,
    NORMAL_CALL_CLEARING = 16,
    USER_BUSY = 17,
    NO_USER_RESPONDING = 18,
    USER_ALERTING_NO_ANSWER = 19,
    CALL_REJECTED = 21,
    NUMBER_CHANGED = 22,
    DESTINATION_OUT_OF_ORDER = 27,
    INVALID_NUMBER_FORMAT = 28,
    NETWORK_OUT_OF_ORDER = 38,
    TEMPORARY_FAILURE = 41,
    INVALID_PARAMETER = 1025,
    SIM_NOT_EXIT = 1026,
    SIM_PIN_NEED = 1027,
    CALL_NOT_ALLOW = 1029,
    SIM_INVALID = 1045,
    UNKNOWN = 1279,
};

enum class AudioDevice {
    DEVICE_EARPIECE = 0,
    DEVICE_SPEAKER,
    DEVICE_WIRED_HEADSET,
    DEVICE_BLUETOOTH_SCO,
    DEVICE_DISABLE,
    DEVICE_UNKNOWN,
};

enum class CellularCallEventType {
    EVENT_REQUEST_RESULT_TYPE = 0,
};

enum class RequestResultEventId {
    RESULT_DIAL_SEND_FAILED = 0,
    RESULT_DIAL_NO_CARRIER,
    RESULT_END_SEND_FAILED,
    RESULT_REJECT_SEND_FAILED,
    RESULT_ACCEPT_SEND_FAILED,
    RESULT_HOLD_SEND_FAILED,
    RESULT_ACTIVE_SEND_FAILED,
    RESULT_SWAP_SEND_FAILED,
    RESULT_JOIN_SEND_FAILED,
    RESULT_SPLIT_SEND_FAILED,
    RESULT_SUPPLEMENT_SEND_FAILED,
    RESULT_INVITE_TO_CONFERENCE_SUCCESS,
    RESULT_INVITE_TO_CONFERENCE_FAILED,
    RESULT_KICK_OUT_FROM_CONFERENCE_SUCCESS,
    RESULT_KICK_OUT_FROM_CONFERENCE_FAILED,

    RESULT_SEND_DTMF_SUCCESS,
    RESULT_SEND_DTMF_FAILED,

    RESULT_GET_CURRENT_CALLS_FAILED,

    RESULT_SET_CALL_PREFERENCE_MODE_SUCCESS,
    RESULT_SET_CALL_PREFERENCE_MODE_FAILED,
    RESULT_GET_IMS_CALLS_DATA_FAILED,

    RESULT_GET_CALL_WAITING_SUCCESS,
    RESULT_GET_CALL_WAITING_FAILED,
    RESULT_SET_CALL_WAITING_SUCCESS,
    RESULT_SET_CALL_WAITING_FAILED,
    RESULT_GET_CALL_RESTRICTION_SUCCESS,
    RESULT_GET_CALL_RESTRICTION_FAILED,
    RESULT_SET_CALL_RESTRICTION_SUCCESS,
    RESULT_SET_CALL_RESTRICTION_FAILED,
    RESULT_GET_CALL_TRANSFER_SUCCESS,
    RESULT_GET_CALL_TRANSFER_FAILED,
    RESULT_SET_CALL_TRANSFER_SUCCESS,
    RESULT_SET_CALL_TRANSFER_FAILED,
    RESULT_SEND_USSD_SUCCESS,
    RESULT_SEND_USSD_FAILED,

    RESULT_SET_MUTE_SUCCESS,
    RESULT_SET_MUTE_FAILED,

    RESULT_CTRL_CAMERA_SUCCESS,
    RESULT_CTRL_CAMERA_FAILED,
    RESULT_SET_PREVIEW_WINDOW_SUCCESS,
    RESULT_SET_PREVIEW_WINDOW_FAILED,
    RESULT_SET_DISPLAY_WINDOW_SUCCESS,
    RESULT_SET_DISPLAY_WINDOW_FAILED,
    RESULT_SET_CAMERA_ZOOM_SUCCESS,
    RESULT_SET_CAMERA_ZOOM_FAILED,
    RESULT_SET_PAUSE_IMAGE_SUCCESS,
    RESULT_SET_PAUSE_IMAGE_FAILED,
    RESULT_SET_DEVICE_DIRECTION_SUCCESS,
    RESULT_SET_DEVICE_DIRECTION_FAILED,
};

enum class CallResultReportId {
    START_DTMF_REPORT_ID = 0,
    STOP_DTMF_REPORT_ID,
    SEND_USSD_REPORT_ID,
    GET_IMS_CALL_DATA_REPORT_ID,
    GET_CALL_WAITING_REPORT_ID,
    SET_CALL_WAITING_REPORT_ID,
    GET_CALL_RESTRICTION_REPORT_ID,
    SET_CALL_RESTRICTION_REPORT_ID,
    GET_CALL_TRANSFER_REPORT_ID,
    SET_CALL_TRANSFER_REPORT_ID,
    GET_CALL_CLIP_ID,
    GET_CALL_CLIR_ID,
    SET_CALL_CLIR_ID,
    GET_CALL_VOTLE_REPORT_ID,
    SET_CALL_VOTLE_REPORT_ID,
    START_RTT_REPORT_ID,
    STOP_RTT_REPORT_ID,
    GET_IMS_CONFIG_REPORT_ID,
    SET_IMS_CONFIG_REPORT_ID,
    GET_IMS_FEATURE_VALUE_REPORT_ID,
    SET_IMS_FEATURE_VALUE_REPORT_ID,
    GET_LTE_ENHANCE_MODE_REPORT_ID,
    SET_LTE_ENHANCE_MODE_REPORT_ID,
    INVITE_TO_CONFERENCE_REPORT_ID,
    UPDATE_MEDIA_MODE_REPORT_ID,
};

struct CellularCallEventInfo {
    CellularCallEventType eventType;
    RequestResultEventId eventId;
};

enum class RBTPlayInfo {
    NETWORK_ALERTING,
    LOCAL_ALERTING,
};

struct CallWaitResponse {
    int32_t result; // 0: ok  other: error
    int32_t status;
    int32_t classCw;
};

struct ClipResponse {
    int32_t result; // 0: ok  other: error
    int32_t action;
    int32_t clipStat;
};

struct ClirResponse {
    int32_t result; // 0: ok  other: error
    int32_t action;
    int32_t clirStat;
};

struct CallTransferResponse {
    int32_t result; // 0: ok  other: error
    int32_t status;
    int32_t classx;
    int32_t type;
    char number[kMaxNumberLen + 1];
};

struct CallRestrictionResponse {
    int32_t result; // 0: ok  other: error
    int32_t status; // parameter sets/shows the result code presentation status in the TA
    int32_t classCw; // parameter shows the subscriber CLIP service status in the network, <0-4>
};

struct CallPreferenceResponse {
    int32_t result; // 0: ok  other: error
    /*
     * 1：CS Voice only
     * 2：CS Voice preferred, IMS PS Voice as secondary
     * 3：IMS PS Voice preferred, CS Voice as secondary
     * 4：IMS PS Voice only
     */
    int32_t mode;
};

struct LteImsSwitchResponse {
    int32_t result; // 0: ok  other: error
    int32_t active; // 0: off 1: on
};

struct GetImsConfigResponse {
    int32_t result;
    int32_t value;
};

struct GetImsFeatureValueResponse {
    int32_t result;
    int32_t value;
};

struct GetLteEnhanceModeResponse {
    int32_t result;
    int32_t value;
};

struct MuteControlResponse {
    int32_t result;
    int32_t value; // 0: Un mute 1: Set mute
};

/**
 * 27007-430_2001 7.11	Call forwarding number and conditions +CCFC
 * 3GPP TS 22.082 [4]
 * <mode>:
 * 0	disable
 * 1	enable
 * 3	registration
 * 4	erasure
 */
enum class CallTransferSettingType {
    CALL_TRANSFER_DISABLE = 0,
    CALL_TRANSFER_ENABLE = 1,
    CALL_TRANSFER_REGISTRATION = 3,
    CALL_TRANSFER_ERASURE = 4,
};

/**
 * 27007-430_2001 7.11	Call forwarding number and conditions +CCFC
 * 3GPP TS 22.082 [4]
 * <reason>:
 * 0	unconditional
 * 1	mobile busy
 * 2	no reply
 * 3	not reachable
 */
enum class CallTransferType {
    TRANSFER_TYPE_UNCONDITIONAL = 0,
    TRANSFER_TYPE_BUSY = 1,
    TRANSFER_TYPE_NO_REPLY = 2,
    TRANSFER_TYPE_NOT_REACHABLE = 3,
};

struct CallTransferInfo {
    char transferNum[kMaxNumberLen + 1];
    CallTransferSettingType settingType;
    CallTransferType type;
};

struct VideoWindow {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t width;
    int32_t height;
};

// 3GPP TS 22.030 V4.0.0 (2001-03)
// 3GPP TS 22.088 V4.0.0 (2001-03)
enum class CallRestrictionType {
    RESTRICTION_TYPE_ALL_INCOMING = 0,
    RESTRICTION_TYPE_ALL_OUTGOING,
    RESTRICTION_TYPE_INTERNATIONAL,
    RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME,
    RESTRICTION_TYPE_ROAMING_INCOMING,
    RESTRICTION_TYPE_ALL_CALLS,
    RESTRICTION_TYPE_OUTGOING_SERVICES,
    RESTRICTION_TYPE_INCOMING_SERVICES,
};

// 3GPP TS 22.088 V4.0.0 (2001-03)
enum class CallRestrictionMode {
    RESTRICTION_MODE_DEACTIVATION = 0,
    RESTRICTION_MODE_ACTIVATION = 1,
};

struct CallRestrictionInfo {
    char password[kMaxNumberLen + 1];
    CallRestrictionType fac;
    CallRestrictionMode mode;
};

// 3GPP TS 27.007 V3.9.0 (2001-06) Call related supplementary services +CHLD
// 3GPP TS 27.007 V3.9.0 (2001-06) 7.22	Informative examples
enum CallSupplementType {
    TYPE_DEFAULT = 0, // default type
    TYPE_HANG_UP_HOLD_WAIT = 1, // release the held call and the wait call
    TYPE_HANG_UP_ACTIVE = 2, // release the active call and recover the held call
    TYPE_HANG_UP_ALL = 3, // release all calls
};

enum CellularCallReturnType {
    // 3GPP TS 27.007 V3.9.0 (2001-06) 6.27	Informative examples
    RETURN_TYPE_MMI = 0,
};

// 3GPP TS 27.007 V17.3.0 (2021-09) 10.1.35	UE's voice domain preference E-UTRAN +CEVDP
enum DomainPreferenceMode {
    CS_VOICE_ONLY = 1,
    CS_VOICE_PREFERRED = 2,
    IMS_PS_VOICE_PREFERRED = 3,
    IMS_PS_VOICE_ONLY = 4,
};

enum ImsCallMode {
    CALL_MODE_AUDIO_ONLY = 0,
    CALL_MODE_SEND_ONLY,
    CALL_MODE_RECEIVE_ONLY,
    CALL_MODE_SEND_RECEIVE,
    CALL_MODE_VIDEO_PAUSED,
};

struct CallMediaModeRequest {
    char phoneNum[kMaxNumberLen + 1];
    ImsCallMode mode;
};

struct CallMediaModeResponse {
    char phoneNum[kMaxNumberLen + 1];
    int32_t result;
};

enum ImsConfigItem {
    /**
     * video quality
     *
     * format: bool
     */
    ITEM_VIDEO_QUALITY = 0,

    /**
     * 3GPP TS 24.167 V17.1.0 (2020-12) 5.31 /<X>/Mobility_Management_IMS_Voice_Termination
     * Ims Switch Status
     *
     * format: bool
     * Access Types: Get, Replace
     * Values: 0, 1
     * 0 – Mobility Management for IMS Voice Termination disabled.
     * 1 – Mobility Management for IMS Voice Termination enabled.
     */
    ITEM_IMS_SWITCH_STATUS,
};

enum VideoQualityValue {
    LOW = 0,
    HIGH = 1,
};

enum ImsConfigValue {
    CONFIG_SUCCESS = 0,
    CONFIG_FAILED = 1,
};

enum FeatureType {
    /**
     * Official Document IR.92 - IMS Profile for Voice and SMS
     * Annex C MNO provisioning and Late Customization
     *
     * format: bool
     * 3GPP TS 24.167 V17.1.0 (2020-12) 5.43 /<X>/Media_type_restriction_policy
     */
    TYPE_VOICE_OVER_LTE = 0,

    /**
     * Official Document IR.92 - IMS Profile for Voice and SMS
     * Annex C MNO provisioning and Late Customization
     *
     * format: bool
     * 3GPP TS 24.167 V17.1.0 (2020-12) 5.43 /<X>/Media_type_restriction_policy
     */
    TYPE_VIDEO_OVER_LTE,
};

enum class OttCallRequestId {
    OTT_REQUEST_DIAL = 0,
    OTT_REQUEST_HANG_UP,
    OTT_REQUEST_REJECT,
    OTT_REQUEST_ANSWER,
    OTT_REQUEST_HOLD,
    OTT_REQUEST_UN_HOLD,
    OTT_REQUEST_SWITCH,
    OTT_REQUEST_COMBINE_CONFERENCE,
    OTT_REQUEST_SEPARATE_CONFERENCE,
    OTT_REQUEST_INVITE_TO_CONFERENCE,
    OTT_REQUEST_UPDATE_CALL_MEDIA_MODE,
};

struct OttCallRequestInfo {
    char phoneNum[kMaxNumberLen + 1]; // call phone number
    char bundleName[kMaxBundleNameLen + 1];
    VideoStateType videoState; // 0: audio 1:video
};

struct OttCallDetailsInfo {
    char phoneNum[kMaxNumberLen + 1]; // call phone number
    char bundleName[kMaxBundleNameLen + 1];
    VideoStateType videoState; // 0: audio 1:video
    TelCallState callState;
};

enum class OttCallEventId {
    OTT_CALL_EVENT_FUNCTION_UNSUPPORTED = 0,
};

struct OttCallEventInfo {
    OttCallEventId ottCallEventId;
    char bundleName[kMaxBundleNameLen + 1];
};

struct CallDetailInfo {
    int32_t index;
    char phoneNum[kMaxNumberLen + 1]; // call phone number
    char bundleName[kMaxBundleNameLen + 1];
    int32_t accountId;
    CallType callType; // call type: CS、IMS
    VideoStateType callMode; // call mode: video or audio
    TelCallState state; // call state
    int32_t voiceDomain; // 0:CS、1:IMS
};

struct CallDetailsInfo {
    std::vector<CallDetailInfo> callVec;
    int32_t slotId;
    char bundleName[kMaxBundleNameLen + 1];
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_MANAGER_INNER_TYPE_H
