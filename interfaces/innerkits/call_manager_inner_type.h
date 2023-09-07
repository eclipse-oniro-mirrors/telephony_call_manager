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

#ifndef CALL_MANAGER_INNER_TYPE_H
#define CALL_MANAGER_INNER_TYPE_H

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>

namespace OHOS {
namespace Telephony {
/**
 * @brief Indicates Maximum length of a string.
 */
constexpr int16_t kMaxNumberLen = 100;
/**
 * @brief Indicates Maximum length of a bundle name.
 */
constexpr int16_t kMaxBundleNameLen = 100;
/**
 * @brief Indicates Maximum length of a address.
 */
constexpr int16_t kMaxAddressLen = 100;
/**
 * @brief Indicates Maximum length of a MMI code message.
 */
constexpr int16_t kMaxMessageLen = 500;
/**
 * @brief Indicates Maximum length of the reject call message.
 */
constexpr uint16_t REJECT_CALL_MSG_MAX_LEN = 300;
/**
 * @brief Indicates Maximum length of the account number.
 */
constexpr uint16_t ACCOUNT_NUMBER_MAX_LENGTH = 100;
/**
 * @brief Indicates Maximum duration(ms) when connect service.
 */
constexpr uint16_t CONNECT_SERVICE_WAIT_TIME = 1000;
/**
 * @brief Indicates the main call id is invalid.
 */
constexpr int16_t ERR_ID = -1;
/**
 * @brief Indicates the call id is invalid.
 */
constexpr int16_t INVALID_CALLID = 0;
/**
 * @brief Indicates one second duration.
 */
constexpr int16_t WAIT_TIME_ONE_SECOND = 1;
/**
 * @brief Indicates No Call Exist.
 */
constexpr int16_t NO_CALL_EXIST = 0;
/**
 * @brief Indicates One Call Exist.
 */
constexpr int16_t ONE_CALL_EXIST = 1;
/**
 * @brief  The follow hour and minute was use to confirm the set
 * call transfer beginning and ending time restriction.
 *
 * MIN_HOUR: the minimum hour value.
 * MAX_HOUR: the maximum hour value.
 * MIN_MINUTE: the minimum minute value.
 * MAX_MINUTE: the maximum minute value.
 * INVALID_TIME: the time value is invalid.
 */
constexpr int16_t MIN_HOUR = 0;
constexpr int16_t MAX_HOUR = 24;
constexpr int16_t MIN_MINUTE = 0;
constexpr int16_t MAX_MINUTE = 60;
constexpr int16_t INVALID_TIME = -1;

/**
 * @brief Indicates the type of call, includs CS, IMS, OTT, OTHER.
 */
enum class CallType {
    /**
     * Indicates the call type is CS.
     */
    TYPE_CS = 0,
    /**
     * Indicates the call type is IMS.
     */
    TYPE_IMS = 1,
    /**
     * Indicates the call type is OTT.
     */
    TYPE_OTT = 2,
    /**
     * Indicates the call type is OTHER.
     */
    TYPE_ERR_CALL = 3,
};

/**
 * @brief Indicates the detailed state of call.
 */
enum class TelCallState {
    /**
     * Indicates the call is unknown.
     */
    CALL_STATUS_UNKNOWN = -1,
    /**
     * Indicates the call is active.
     */
    CALL_STATUS_ACTIVE = 0,
    /**
     * Indicates the call is holding.
     */
    CALL_STATUS_HOLDING,
    /**
     * Indicates the call is dialing.
     */
    CALL_STATUS_DIALING,
    /**
     * Indicates the call is alerting.
     */
    CALL_STATUS_ALERTING,
    /**
     * Indicates the call is incoming.
     */
    CALL_STATUS_INCOMING,
    /**
     * Indicates the call is waiting.
     */
    CALL_STATUS_WAITING,
    /**
     * Indicates the call is disconnected.
     */
    CALL_STATUS_DISCONNECTED,
    /**
     * Indicates the call is disconnecting.
     */
    CALL_STATUS_DISCONNECTING,
    /**
     * Indicates the call is idle.
     */
    CALL_STATUS_IDLE,
};

/**
 * @brief Indicates the state of conference call.
 */
enum class TelConferenceState {
    /**
     * Indicates the state is idle.
     */
    TEL_CONFERENCE_IDLE = 0,
    /**
     * Indicates the state is active.
     */
    TEL_CONFERENCE_ACTIVE,
    /**
     * Indicates the state is hold.
     */
    TEL_CONFERENCE_HOLDING,
    /**
     * Indicates the state is disconnecting.
     */
    TEL_CONFERENCE_DISCONNECTING,
    /**
     * Indicates the state is disconnected.
     */
    TEL_CONFERENCE_DISCONNECTED,
};

/**
 * @brief Indicates the Network type.
 */
enum class PhoneNetType {
    /**
     * Indicates the Network type is GSM.
     */
    PHONE_TYPE_GSM = 1,
    /**
     * Indicates the Network type is CDMA.
     */
    PHONE_TYPE_CDMA = 2,
};

/**
 * @brief Indicates the type of video state.
 */
enum class VideoStateType {
    /**
     * Indicates the call is in voice state.
     */
    TYPE_VOICE = 0,
    /**
     * Indicates the call is in video state.
     */
    TYPE_VIDEO,
};

/**
 * @brief Indicates the scenarios of the call to be made.
 */
enum class DialScene {
    /**
     * Indicates this is a common call.
     */
    CALL_NORMAL = 0,
    /**
     * Indicates this is a privileged call.
     */
    CALL_PRIVILEGED,
    /**
     * Indicates this is an emergency call.
     */
    CALL_EMERGENCY,
};

/**
 * @brief Indicates the call is MO or MT.
 */
enum class CallDirection {
    /**
     * Indicates the call is a incoming call.
     */
    CALL_DIRECTION_IN = 0,
    /**
     * Indicates the call is a outgoing call.
     */
    CALL_DIRECTION_OUT,
    /**
     * Indicates the call is unknown.
     */
    CALL_DIRECTION_UNKNOW,
};

/**
 * @brief Indicates the call state in progress.
 */
enum class CallRunningState {
    /**
     * Indicates to create a new call session.
     */
    CALL_RUNNING_STATE_CREATE = 0,
    /**
     * Indicates the call state is in connecting.
     */
    CALL_RUNNING_STATE_CONNECTING,
    /**
     * Indicates the call state is in dialing.
     */
    CALL_RUNNING_STATE_DIALING,
    /**
     * Indicates the call state is in ringing.
     */
    CALL_RUNNING_STATE_RINGING,
    /**
     * Indicates the call state is in active.
     */
    CALL_RUNNING_STATE_ACTIVE,
    /**
     * Indicates the call state is in hold.
     */
    CALL_RUNNING_STATE_HOLD,
    /**
     * Indicates the call state is ended.
     */
    CALL_RUNNING_STATE_ENDED,
    /**
     * Indicates the call state is in ending.
     */
    CALL_RUNNING_STATE_ENDING,
};

/**
 * @brief Indicates the cause of the ended call.
 */
enum class CallEndedType {
    /**
     * Indicates the cause is unknown.
     */
    UNKNOWN = 0,
    /**
     * Indicates the cause is phone busy.
     */
    PHONE_IS_BUSY,
    /**
     * Indicates the cause is invalid phone number.
     */
    INVALID_NUMBER,
    /**
     * Indicates the call is ended normally.
     */
    CALL_ENDED_NORMALLY,
};

/**
 * @brief Indicates the information of SIM card.
 */
struct SIMCardInfo {
    /**
     * Indicates the SIM ICC id.
     */
    int32_t simId = 0;
    /**
     * Indicated the country to which the SIM card belongs.
     */
    int32_t country = 0;
    /**
     * Indicates wether the SIM card is active.
     */
    int32_t state = 0;
    /**
     * Indicates the Network type.
     */
    PhoneNetType phoneNetType = PhoneNetType::PHONE_TYPE_GSM;
};

/**
 * @brief Indicates the dialing call type.
 */
enum class DialType {
    /**
     * Indicates the dialing call type is normal cellular call.
     */
    DIAL_CARRIER_TYPE = 0,
    /**
     * Indicates the dialing call type is voice mail.
     */
    DIAL_VOICE_MAIL_TYPE,
    /**
     * Indicates the dialing call type is OTT.
     */
    DIAL_OTT_TYPE,
};

/**
 * @brief Indicates the call state which will report to APP.
 */
enum class CallStateToApp {
    /**
     * Indicates an invalid state, which is used when the call state
     * fails to be obtained.
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
     * Indicates that a least one call is in the dialing, active, or hold
     * state, and there is no new incoming call ringing or waiting.
     */
    CALL_STATE_OFFHOOK = 2
};

/**
 * @brief Indicates the cause when the call is answered.
 */
enum class CallAnswerType {
    /**
     * Indicates the call answer is call missed.
     */
    CALL_ANSWER_MISSED = 0,
    /**
     * Indicates the call answer is call active.
     */
    CALL_ANSWER_ACTIVED,
    /**
     * Indicates the call answer is call rejected.
     */
    CALL_ANSWER_REJECT,
};

/**
 * @brief Indicates the detail information of a call.
 */
struct CallAttributeInfo {
    /**
     * Indicates the call phone number.
     */
    char accountNumber[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the bundle name.
     */
    char bundleName[kMaxBundleNameLen + 1] = { 0 };
    /**
     * Indicates whether the speaker phone is on.
     */
    bool speakerphoneOn = false;
    int32_t accountId = 0;
    /**
     * Indicates the type of video state. {@link VideoStateType}
     */
    VideoStateType videoState = VideoStateType::TYPE_VOICE;
    /**
     * Indicates the Call start time.
     */
    int64_t startTime = 0;
    /**
     * Indicates whether the call is emergency call.
     */
    bool isEcc = false;
    /**
     * Indicates the type of call, includs CS, IMS, OTT, OTHER. {@link CallType}
     */
    CallType callType = CallType::TYPE_ERR_CALL;
    /**
     * Indicates the call index.
     */
    int32_t callId = 0;
    /**
     * Indicates the detailed state of call. {@link TelCallState}
     */
    TelCallState callState = TelCallState::CALL_STATUS_UNKNOWN;
    /**
     * Indicates the state of conference call. {@link TelConferenceState}
     */
    TelConferenceState conferenceState = TelConferenceState::TEL_CONFERENCE_IDLE;
    /**
     * Indicates the call beginning and ending time.
     */
    time_t callBeginTime = 0;
    time_t callEndTime = 0;
    time_t ringBeginTime = 0;
    time_t ringEndTime = 0;
    /**
     * Indicates the call is MO or MT. {@link CallDirection}
     */
    CallDirection callDirection = CallDirection::CALL_DIRECTION_UNKNOW;
    /**
     * Indicates the cause when the call is answered. {@link CallAnswerType}
     */
    CallAnswerType answerType = CallAnswerType::CALL_ANSWER_MISSED;
    /*
     * Indicates the call index in cellular call
     */
    int32_t index = 0;
};

/**
 * @brief Indicates the detail information of call record.
 */
struct CallRecordInfo {
    /**
     * Indicates the call index.
     */
    int32_t callId = 0;
    /**
     * Indicates the call phone number.
     */
    char phoneNumber[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the phone number after formatted.
     */
    char formattedPhoneNumber[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the type of call, includs CS, IMS, OTT, OTHER. {@link CallType}
     */
    CallType callType = CallType::TYPE_ERR_CALL;
    /**
     * Indicates the call beginning time.
     */
    time_t callBeginTime = 0;
    /**
     * Indicates the call ending time.
     */
    time_t callEndTime = 0;
    /**
     * Indicates the call ring duration.
     */
    uint32_t ringDuration = 0;
    /**
     * Indicates the call session duration.
     */
    uint32_t callDuration = 0;
    /**
     * Indicates the call is MO or MT. {@link CallDirection}
     */
    CallDirection directionType = CallDirection::CALL_DIRECTION_UNKNOW;
    /**
     * Indicates the cause when the call is answered. {@link CallAnswerType}
     */
    CallAnswerType answerType = CallAnswerType::CALL_ANSWER_MISSED;
    /**
     * Indicates the country code of the call.
     */
    int32_t countryCode = 0;
    /**
     * Indicates the slot id.
     */
    int32_t slotId = 0;

    CallRecordInfo() {}

    CallRecordInfo(const CallRecordInfo &temp)
    {
        *this = temp;
    }

    CallRecordInfo &operator=(const CallRecordInfo &temp)
    {
        callId = temp.callId;
        std::copy(std::begin(temp.phoneNumber), std::end(temp.phoneNumber), std::begin(phoneNumber));
        std::copy(std::begin(temp.formattedPhoneNumber), std::end(temp.formattedPhoneNumber),
            std::begin(formattedPhoneNumber));
        callType = temp.callType;
        callBeginTime = temp.callBeginTime;
        callEndTime = temp.callEndTime;
        ringDuration = temp.ringDuration;
        callDuration = temp.callDuration;
        directionType = temp.directionType;
        answerType = temp.answerType;
        countryCode = temp.countryCode;
        slotId = temp.slotId;
        return *this;
    }
};

/**
 * @brief Indicates the event ID of call ability.
 */
enum class CallAbilityEventId {
    /**
     * Indicates that there is no available carrier during dialing.
     */
    EVENT_DIAL_NO_CARRIER = 1,
    /**
     * Indicates that FDN is invalid.
     */
    EVENT_INVALID_FDN_NUMBER,
    /**
     * Indicates hold call fail.
     */
    EVENT_HOLD_CALL_FAILED,
    /**
     * Indicates swap call fail.
     */
    EVENT_SWAP_CALL_FAILED,
    /**
     * Indicates that the OTT is not supported.
     */
    EVENT_OTT_FUNCTION_UNSUPPORTED,
};

/**
 * @brief Indicates the call event detail information.
 */
struct CallEventInfo {
    /**
     * Indicates the event ID of call ability. {@link CallAbilityEventId}
     */
    CallAbilityEventId eventId = CallAbilityEventId::EVENT_DIAL_NO_CARRIER;
    /**
     * Indicates the call phone number.
     */
    char phoneNum[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the bundle name.
     */
    char bundleName[kMaxBundleNameLen + 1] = { 0 };
};

struct AccountInfo {
    int32_t accountId = 0;
    int32_t power = 0;
    char bundleName[kMaxNumberLen + 1] = { 0 };
    bool isEnabled = false;
};

/**
 * @brief Indicates the call report information.
 */
struct CallReportInfo {
    int32_t index = 0;
    /**
     * Indicates the call phone number.
     */
    char accountNum[kMaxNumberLen + 1] = { 0 };
    int32_t accountId = 0;
    /**
     * Indicates the type of call, includs CS, IMS, OTT, OTHER. {@link CallType}
     */
    CallType callType = CallType::TYPE_ERR_CALL;
    /**
     * Indicates the type of video state. {@link VideoStateType}
     */
    VideoStateType callMode = VideoStateType::TYPE_VOICE;
    /**
     * Indicates the detailed state of call. {@link TelCallState}
     */
    TelCallState state = TelCallState::CALL_STATUS_UNKNOWN;
    /**
     * Indicates the voica domain, 0: CS, 1: IMS
     */
    int32_t voiceDomain = 0;
    /**
     * Indicates the Multi-party call status:
     * 0: not a multi-party(conference) call
     * 1: a multi-party(conference) call
     */
    int32_t mpty = 0;
};

/**
 * @brief Indicates the detail information of some calls record.
 */
struct CallsReportInfo {
    /**
     * Indicates a list of the call report information.
     */
    std::vector<CallReportInfo> callVec {};
    /**
     * Indicates the slot id.
     */
    int32_t slotId = 0;
};

/**
 * @brief Indicates the causes of call disconnection.
 *
 * 3GPP TS 24.008
 * V17.4.0 10.5.4.11 Cause
 * The purpose of the cause information element is to describe the reason for
 * generating certain messages, to provide diagnostic information in the event
 * of procedural errors and to indicate the location of the cause originator.
 */
enum class DisconnectedReason : int32_t {
    /**
     * Indicates the call disconnect due to Unassigned (unallocated) number.
     */
    UNASSIGNED_NUMBER = 1,
    /**
     * Indicates the call disconnect due to No route to destination.
     */
    NO_ROUTE_TO_DESTINATION = 3,
    /**
     * Indicates the call disconnect due to Channel unacceptable.
     */
    CHANNEL_UNACCEPTABLE = 6,
    /**
     * Indicates the call disconnect due to Operator determined barring.
     */
    OPERATOR_DETERMINED_BARRING = 8,
    /**
     * Indicates the call disconnect due to Call completed elsewhere.
     */
    CALL_COMPLETED_ELSEWHERE = 13,
    /**
     * Indicates the call disconnect due to Normal call clearing.
     */
    NORMAL_CALL_CLEARING = 16,
    /**
     * Indicates the call disconnect due to User busy.
     */
    USER_BUSY = 17,
    /**
     * Indicates the call disconnect due to No user responding.
     */
    NO_USER_RESPONDING = 18,
    /**
     * Indicates the call disconnect due to User alerting, no answer.
     */
    USER_ALERTING_NO_ANSWER = 19,
    /**
     * Indicates the call disconnect due to Call rejected.
     */
    CALL_REJECTED = 21,
    /**
     * Indicates the call disconnect due to Number changed.
     */
    NUMBER_CHANGED = 22,
    /**
     * Indicates the call disconnect cause is Call rejected due to feature
     * at the destination.
     */
    CALL_REJECTED_DUE_TO_FEATURE_AT_THE_DESTINATION = 24,
    /**
     * Indicates the call disconnect due to Pre-emption.
     */
    FAILED_PRE_EMPTION = 25,
    /**
     * Indicates the call disconnect due to Non selected user clearing.
     */
    NON_SELECTED_USER_CLEARING = 26,
    /**
     * Indicates the call disconnect due to Destination out of order.
     */
    DESTINATION_OUT_OF_ORDER = 27,
    /**
     * Indicates the call disconnect due to Invalid number format
     * (incomplete number).
     */
    INVALID_NUMBER_FORMAT = 28,
    /**
     * Indicates the call disconnect due to Facility rejected.
     */
    FACILITY_REJECTED = 29,
    /**
     * Indicates the call disconnect due to Response to STATUS ENQUIRY.
     */
    RESPONSE_TO_STATUS_ENQUIRY = 30,
    /**
     * Indicates the call disconnect due to Normal, unspecified.
     */
    NORMAL_UNSPECIFIED = 31,
    /**
     * Indicates the call disconnect due to No circuit/channel available.
     */
    NO_CIRCUIT_CHANNEL_AVAILABLE = 34,
    /**
     * Indicates the call disconnect due to Network out of order.
     */
    NETWORK_OUT_OF_ORDER = 38,
    /**
     * Indicates the call disconnect due to Temporary failure.
     */
    TEMPORARY_FAILURE = 41,
    /**
     * Indicates the call disconnect due to Switching equipment congestion.
     */
    SWITCHING_EQUIPMENT_CONGESTION = 42,
    /**
     * Indicates the call disconnect due to Access information discarded.
     */
    ACCESS_INFORMATION_DISCARDED = 43,
    /**
     * Indicates the call disconnect due to requested circuit/channel
     * not available.
     */
    REQUEST_CIRCUIT_CHANNEL_NOT_AVAILABLE = 44,
    /**
     * Indicates the call disconnect due to Resources unavailable, unspecified.
     */
    RESOURCES_UNAVAILABLE_UNSPECIFIED = 47,
    /**
     * Indicates the call disconnect due to Quality of service unavailable.
     */
    QUALITY_OF_SERVICE_UNAVAILABLE = 49,
    /**
     * Indicates the call disconnect due to Requested facility not subscribed.
     */
    REQUESTED_FACILITY_NOT_SUBSCRIBED = 50,
    /**
     * Indicates the call disconnect due to Incoming calls barred within the CUG.
     */
    INCOMING_CALLS_BARRED_WITHIN_THE_CUG = 55,
    /**
     * Indicates the call disconnect due to Bearer capability not authorized.
     */
    BEARER_CAPABILITY_NOT_AUTHORIZED = 57,
    /**
     * Indicates the call disconnect due to Bearer capability not presently
     * available.
     */
    BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE = 58,
    /**
     * Indicates the call disconnect due to Service or option not available,
     * unspecified.
     */
    SERVICE_OR_OPTION_NOT_AVAILABLE_UNSPECIFIED = 63,
    /**
     * Indicates the call disconnect due to Bearer service not implemented.
     */
    BEARER_SERVICE_NOT_IMPLEMENTED = 65,
    /**
     * Indicates the call disconnect due to ACM equal to or greater than ACMmax.
     */
    ACM_EQUALTO_OR_GREATE_THAN_ACMMAX = 68,
    /**
     * Indicates the call disconnect due to Requested facility not implemented.
     */
    REQUESTED_FACILITY_NOT_IMPLEMENTED = 69,
    /**
     * Indicates the call disconnect due to Only restricted digital information
     * bearer capability is available.
     */
    ONLY_RESTRICTED_DIGITAL_INFO_BEARER_CAPABILITY_IS_AVAILABLE = 70,
    /**
     * Indicates the call disconnect due to Service or option not implemented,
     * unspecified.
     */
    SERVICE_OR_OPTION_NOT_IMPLEMENTED_UNSPECIFIED = 79,
    /**
     * Indicates the call disconnect due to Invalid transaction identifier
     * value.
     */
    INVALID_TRANSACTION_IDENTIFIER_VALUE = 81,
    /**
     * Indicates the call disconnect due to User not member of CUG.
     */
    USER_NOT_MEMBER_OF_CUG = 87,
    /**
     * Indicates the call disconnect due to Incompatible destination.
     */
    INCOMPATIBLE_DESTINATION = 88,
    /**
     * Indicates the call disconnect due to Invalid transit network selection.
     */
    INVALID_TRANSIT_NETWORK_SELECTION = 91,
    /**
     * Indicates the call disconnect due to Semantically incorrect message.
     */
    SEMANTICALLY_INCORRECT_MESSAGE = 95,
    /**
     * Indicates the call disconnect due to Invalid mandatory information.
     */
    INVALID_MANDATORY_INFORMATION = 96,
    /**
     * Indicates the call disconnect due to Message type non-existent or not
     * implemented.
     */
    MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 97,
    /**
     * Indicates the call disconnect due to Message type not compatible with
     * protocol state.
     */
    MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 98,
    /**
     * Indicates the call disconnect due to Information element non-existent or
     * not implemented.
     */
    INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED = 99,
    /**
     * Indicates the call disconnect due to Conditional IE error.
     */
    CONDITIONAL_IE_ERROR = 100,
    /**
     * Indicates the call disconnect due to Message not compatible with
     * protocol state.
     */
    MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 101,
    /**
     * Indicates the call disconnect due to Recovery on timer expiry.
     */
    RECOVERY_ON_TIMER_EXPIRED = 102,
    /**
     * Indicates the call disconnect due to Protocol error, unspecified.
     */
    PROTOCOL_ERROR_UNSPECIFIED = 111,
    /**
     * Indicates the call disconnect due to Interworking, unspecified.
     */
    INTERWORKING_UNSPECIFIED = 127,
    /**
     * Indicates the call disconnect due to Call barred.
     */
    CALL_BARRED = 240,
    /**
     * Indicates the call disconnect due to FDN blocked.
     */
    FDN_BLOCKED = 241,
    /**
     * Indicates the call disconnect due to IMSI in VLR is unknown.
     */
    IMSI_UNKNOWN_IN_VLR = 242,
    /**
     * Indicates the call disconnect due to IMEI not accepted.
     */
    IMEI_NOT_ACCEPTED = 243,
    /**
     * Indicates the call disconnect due to dial modified to USSD.
     */
    DIAL_MODIFIED_TO_USSD = 244,
    /**
     * Indicates the call disconnect due to dial modified to SS.
     */
    DIAL_MODIFIED_TO_SS = 245,
    /**
     * Indicates the call disconnect due to dial modified to dial.
     */
    DIAL_MODIFIED_TO_DIAL = 246,
    /**
     * Indicates the call disconnect due to Radio off.
     */
    RADIO_OFF = 247,
    /**
     * Indicates the call disconnect due to out of service.
     */
    OUT_OF_SERVICE = 248,
    /**
     * Indicates the call disconnect due to invalid SIM.
     */
    NO_VALID_SIM = 249,
    /**
     * Indicates the call disconnect due to radio internal error in modem.
     */
    RADIO_INTERNAL_ERROR = 250,
    /**
     * Indicates the call disconnect due to network response timeout.
     */
    NETWORK_RESP_TIMEOUT = 251,
    /**
     * Indicates the call disconnect due to network reject.
     */
    NETWORK_REJECT = 252,
    /**
     * Indicates the call disconnect due to RRC connection failure. Eg.RACH
     */
    RADIO_ACCESS_FAILURE = 253,
    /**
     * Indicates the call disconnect due to radio link failure.
     */
    RADIO_LINK_FAILURE = 254,
    /**
     * Indicates the call disconnect due to poor coverage.
     */
    RADIO_LINK_LOST = 255,
    /**
     * Indicates the call disconnect due to Radio uplink failure.
     */
    RADIO_UPLINK_FAILURE = 256,
    /**
     * Indicates the call disconnect due to RRC connection setup failure.
     */
    RADIO_SETUP_FAILURE = 257,
    /**
     * Indicates the call disconnect due to RRC connection release, normal.
     */
    RADIO_RELEASE_NORMAL = 258,
    /**
     * Indicates the call disconnect due to RRC connection release, abnormal.
     */
    RADIO_RELEASE_ABNORMAL = 259,
    /**
     * Indicates the call disconnect due to Access class barring.
     */
    ACCESS_CLASS_BLOCKED = 260,
    /**
     * Indicates the call disconnect due to Explicit network detach.
     */
    NETWORK_DETACH = 261,
    /**
     * Indicates the call disconnect due to invalid parameter.
     */
    INVALID_PARAMETER = 1025,
    /**
     * Indicates the call disconnect due to sim not exit.
     */
    SIM_NOT_EXIT = 1026,
    /**
     * Indicates the call disconnect due to sim pin need.
     */
    SIM_PIN_NEED = 1027,
    /**
     * Indicates the call disconnect due to call not allow.
     */
    CALL_NOT_ALLOW = 1029,
    /**
     * Indicates the call disconnect due to sim invalid.
     */
    SIM_INVALID = 1045,
    /**
     * Indicates the call disconnect due to unknown error.
     */
    FAILED_UNKNOWN = 1279,
};

/**
 * @brief Indicates the cause detail of a call disconnection.
 */
struct DisconnectedDetails {
    /**
     * Indicates the reason for ending the call.
     */
    DisconnectedReason reason = DisconnectedReason::FAILED_UNKNOWN;
    /**
     * Indicates the detail message for the ending call reason.
     */
    std::string message = "";

    DisconnectedDetails() {}

    DisconnectedDetails(const DisconnectedDetails &temp)
    {
        *this = temp;
    }

    DisconnectedDetails &operator=(const DisconnectedDetails &temp)
    {
        reason = temp.reason;
        message = temp.message;
        return *this;
    }
};

/**
 * @brief Indicates the what the Audio device type is used.
 */
enum class AudioDeviceType {
    /**
     * Indicates the device type is a earphone speaker.
     */
    DEVICE_EARPIECE = 0,
    /**
     * Indicates the device type is the speaker system (i.e. a mono speaker or
     * stereo speakers) built in a device.
     */
    DEVICE_SPEAKER,
    /**
     * Indicates the device type is a headset, which is the combination of a
     * headphones and microphone.
     */
    DEVICE_WIRED_HEADSET,
    /**
     * Indicates the device type is a Bluetooth device typically used for
     * telephony.
     */
    DEVICE_BLUETOOTH_SCO,
    /**
     * Indicates the audio device is disabled.
     */
    DEVICE_DISABLE,
    /**
     * Indicates the device type is an unknown or uninitialized.
     */
    DEVICE_UNKNOWN,
};

/**
 * @brief Indicates the call event type.
 */
enum class CellularCallEventType {
    EVENT_REQUEST_RESULT_TYPE = 0,
};

/**
 * @brief Indicates the call event id, one id corresponds to one request.
 */
enum class RequestResultEventId {
    INVALID_REQUEST_RESULT_EVENT_ID = -1,
    RESULT_DIAL_SEND_FAILED = 0,
    RESULT_DIAL_NO_CARRIER,
    RESULT_END_SEND_FAILED,
    RESULT_REJECT_SEND_FAILED,
    RESULT_ACCEPT_SEND_FAILED,
    RESULT_HOLD_SEND_FAILED,
    RESULT_ACTIVE_SEND_FAILED,
    RESULT_SWAP_SEND_FAILED,
    RESULT_COMBINE_SEND_FAILED,
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

/**
 * @brief Indicates the call result report id in callback,
 * one id corresponds to one request.
 */
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
    START_RTT_REPORT_ID,
    STOP_RTT_REPORT_ID,
    GET_IMS_CONFIG_REPORT_ID,
    SET_IMS_CONFIG_REPORT_ID,
    GET_IMS_FEATURE_VALUE_REPORT_ID,
    SET_IMS_FEATURE_VALUE_REPORT_ID,
    INVITE_TO_CONFERENCE_REPORT_ID,
    UPDATE_MEDIA_MODE_REPORT_ID,
    CLOSE_UNFINISHED_USSD_REPORT_ID,
    SET_CALL_RESTRICTION_PWD_REPORT_ID,
};

/**
 * @brief Indicates the cellular call event information.
 */
struct CellularCallEventInfo {
    /**
     * Indicates the call event type. {@link CellularCallEventType}
     */
    CellularCallEventType eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;
    /**
     * Indicates the call event id, one id corresponds to one request.
     * {@link RequestResultEventId}
     */
    RequestResultEventId eventId = RequestResultEventId::INVALID_REQUEST_RESULT_EVENT_ID;
};

/**
 * @brief Indicates the ringback tone playing mode.
 */
enum class RBTPlayInfo {
    /**
     * Indicates the mode is network, the tone is from network.
     */
    NETWORK_ALERTING,
    /**
     * Indicates the mode is local, the tone is from local file.
     */
    LOCAL_ALERTING,
};

/**
 * @brief Indicates the Call Waiting response result.
 */
struct CallWaitResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * parameter sets/shows the result code presentation status in the TA.
     */
    int32_t status = 0;
    /**
     * parameter shows the subscriber CW service status in the network.
     */
    int32_t classCw = 0;
};

/**
 * @brief Indicates the Clip response result.
 */
struct ClipResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * parameter sets/shows the result code presentation status in the TA.
     */
    int32_t action = 0;
    /**
     * parameter shows the subscriber CLIP service status in the network.
     */
    int32_t clipStat = 0;
};

/**
 * @brief Indicates the Clir response result.
 */
struct ClirResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * parameter sets/shows the result code presentation status in the TA.
     */
    int32_t action = 0;
    /**
     * parameter shows the subscriber CLIR service status in the network.
     */
    int32_t clirStat = 0;
};

/**
 * @brief Indicates the Colr response result.
 */
struct ColrResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * parameter sets/shows the result code presentation status in the TA.
     */
    int32_t action = 0;
    /**
     * parameter shows the subscriber COLR service status in the network.
     */
    int32_t colrStat = 0;
};

/**
 * @brief Indicates the Colp response result.
 */
struct ColpResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * parameter sets/shows the result code presentation status in the TA.
     */
    int32_t action = 0;
    /**
     * parameter shows the subscriber COLP service status in the network.
     */
    int32_t colpStat = 0;
};

/**
 * @brief Indicates the MMI Code response result.
 */
struct MmiCodeInfo {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * Indicates the specific message of MMI code response.
     */
    char message[kMaxMessageLen + 1] = { 0 };
};

/**
 * @brief Indicates the Call Transfer response result.
 */
struct CallTransferResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * Indicates the status of the CF option. 0: not active; 1: active
     */
    int32_t status = 0;
    /**
     * <classx> is a sum of integers each representing a class of information
     * (default 7):
     * 1	voice (telephony)
     * 2	data (refers to all bearer services)
     * 4	fax (facsimile services)
     * 8	short message service
     * 16	data circuit sync
     * 32	data circuit async
     * 64	dedicated packet access
     * 128	dedicated PAD access
     */
    int32_t classx = 0;
    /**
     * type of address octet in integer format (refer GSM 04.08 [8]
     * subclause 10.5.4.7); default 145 when dialling string includes
     * international access code character "+", otherwise 129.
     */
    int32_t type = 0;
    /**
     * Indication the phone number which will forward to.
     */
    char number[kMaxNumberLen + 1] = { 0 };
    int32_t reason = 0;
    int32_t time = 0;
    /**
     * Indicates the Call forwarding start time.
     */
    int32_t startHour = INVALID_TIME;
    int32_t startMinute = INVALID_TIME;
    /**
     * Indicates the Call forwarding end time.
     */
    int32_t endHour = INVALID_TIME;
    int32_t endMinute = INVALID_TIME;
};

/**
 * @brief Indicates the Call Restriction response result.
 */
struct CallRestrictionResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * parameter sets/shows the result code presentation status in the TA
     */
    int32_t status = 0;
    /**
     * parameter shows the subscriber Call Restriction service status
     * in the network.
     */
    int32_t classCw = 0;
};

/**
 * @brief Indicates the Call Preference response result.
 */
struct CallPreferenceResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * Indicates the specific value of the call preference mode.
     *
     * 1：CS Voice only
     * 2：CS Voice preferred, IMS PS Voice as secondary
     * 3：IMS PS Voice preferred, CS Voice as secondary
     * 4：IMS PS Voice only
     */
    int32_t mode = 0;
};

/**
 * @brief Indicates the GetImsConfig response result.
 */
struct GetImsConfigResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * Indicates the specific value of the corresponding IMS config.
     */
    int32_t value = 0;
};

/**
 * @brief Indicates the GetImsFeatureValue response result.
 */
struct GetImsFeatureValueResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * Indicates the specific value of the corresponding IMS feature.
     */
    int32_t value = 0;
};

struct GetLteEnhanceModeResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * Indicates the specific value of the LTE Enhance mode.
     */
    int32_t value = 0;
};

struct MuteControlResponse {
    /**
     * 0: ok  other: error
     */
    int32_t result = 0;
    /**
     * Indicates the specific value of mute setting. 0: not mute 1: set mute
     */
    int32_t value = 0;
};

/**
 * @brief Indicates the cellular call detail information.
 */
struct CellularCallInfo {
    /**
     * Indicates the call index
     */
    int32_t callId = 0;
    /**
     * Indicates the call phone number.
     */
    char phoneNum[kMaxNumberLen] = { 0 };
    /**
     * Indicates the slot id.
     */
    int32_t slotId = 0;
    int32_t accountId = 0;
    /**
     * Indicates the type of call, includs CS, IMS, OTT, OTHER. {@link CallType}
     */
    CallType callType = CallType::TYPE_ERR_CALL;
    /**
     * Indicates whether the call is video. 0: audio 1:video
     */
    int32_t videoState = 0;
    int32_t index = 0;
};

/**
 * @brief Indicates the type of call transfer setting.
 *
 * 27007-430_2001 7.11	Call forwarding number and conditions +CCFC
 * 3GPP TS 22.082 [4]
 * <mode>:
 * 0	disable
 * 1	enable
 * 3	registration
 * 4	erasure
 */
enum class CallTransferSettingType {
    /**
     * Indicates disable the call transfer.
     */
    CALL_TRANSFER_DISABLE = 0,
    /**
     * Indicates enable the call transfer.
     */
    CALL_TRANSFER_ENABLE = 1,
    /**
     * Indicates register the call transfer.
     */
    CALL_TRANSFER_REGISTRATION = 3,
    /**
     * Indicates erasure the call transfer.
     */
    CALL_TRANSFER_ERASURE = 4,
};

/**
 * @brief Indicates the Call forwarding type
 *
 * 27007-430_2001 7.11	Call forwarding number and conditions +CCFC
 * 3GPP TS 22.082 [4]
 * <reason>:
 * 0	unconditional
 * 1	mobile busy
 * 2	no reply
 * 3	not reachable
 */
enum class CallTransferType {
    /**
     * Indicates transfer the call unconditionally.
     */
    TRANSFER_TYPE_UNCONDITIONAL = 0,
    /**
     * Indicates transfer the call when busy.
     */
    TRANSFER_TYPE_BUSY = 1,
    /**
     * Indicates transfer the call when no reply.
     */
    TRANSFER_TYPE_NO_REPLY = 2,
    /**
     * Indicates transfer the call when unreachable.
     */
    TRANSFER_TYPE_NOT_REACHABLE = 3,
};

/**
 * @brief Indicates the information of call transfer.
 */
struct CallTransferInfo {
    /**
     * Indication the phone number which will forward to.
     */
    char transferNum[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the type of call transfer setting. {@link CallTransferSettingType}
     */
    CallTransferSettingType settingType = CallTransferSettingType::CALL_TRANSFER_DISABLE;
    /**
     * Indicates the Call forwarding type. {@link CallTransferType}
     */
    CallTransferType type = CallTransferType::TRANSFER_TYPE_UNCONDITIONAL;
    /**
     * Start time hours.
     */
    int32_t startHour = 0;
    /**
     * Start time minutes.
     */
    int32_t startMinute = 0;
    /**
     * End time hours.
     */
    int32_t endHour = 0;
    /**
     * End time minutes.
     */
    int32_t endMinute = 0;
};

/**
 * @brief Indicates the type of supplementary service request.
 */
enum class SsRequestType {
    SS_ACTIVATION = 0,
    SS_DEACTIVATION,
    SS_INTERROGATION,
    SS_REGISTRATION,
    SS_ERASURE,
};

/**
 * @brief Indicates the video window size and location in video call.
 */
struct VideoWindow {
    /**
     * X coordinate of window
     */
    int32_t x = 0;
    /**
     * Y coordinate of window
     */
    int32_t y = 0;
    /**
     * Z coordinate of window
     */
    int32_t z = 0;
    /**
     * the width of window
     */
    int32_t width = 0;
    /**
     * the height of window
     */
    int32_t height = 0;
};

/**
 * @brief Indicates the type of call restriction.
 *
 * 3GPP TS 22.030 V4.0.0 (2001-03)
 * 3GPP TS 22.088 V4.0.0 (2001-03)
 */
enum class CallRestrictionType {
    /**
     * Indicates restrict all incoming calls.
     */
    RESTRICTION_TYPE_ALL_INCOMING = 0,
    /**
     * Indicates restrict all outgoing calls.
     */
    RESTRICTION_TYPE_ALL_OUTGOING,
    /**
     * Indicates restrict international calls.
     */
    RESTRICTION_TYPE_INTERNATIONAL,
    /**
     * Indicates restrict international roaming calls.
     */
    RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME,
    /**
     * Indicates restrict roaming calls.
     */
    RESTRICTION_TYPE_ROAMING_INCOMING,
    /**
     * Indicates restrict all calls.
     */
    RESTRICTION_TYPE_ALL_CALLS,
    /**
     * Indicates restrict all outgoing services.
     */
    RESTRICTION_TYPE_OUTGOING_SERVICES,
    /**
     * Indicates restrict all incoming services.
     */
    RESTRICTION_TYPE_INCOMING_SERVICES,
};

/**
 * @brief Indicates the mode of call restriction.
 *
 * 3GPP TS 22.088 V4.0.0 (2001-03)
 */
enum class CallRestrictionMode {
    /**
     * Indicates call restriction is deactivated.
     */
    RESTRICTION_MODE_DEACTIVATION = 0,
    /**
     * Indicates call restriction is activated.
     */
    RESTRICTION_MODE_ACTIVATION = 1,
};

/**
 * @brief Indicates the information of call restriction.
 */
struct CallRestrictionInfo {
    /**
     * Indicates the password required to set call restrictions.
     */
    char password[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the type of call restriction.
     */
    CallRestrictionType fac = CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING;
    /**
     * Indicates the mode of call restriction.
     */
    CallRestrictionMode mode = CallRestrictionMode::RESTRICTION_MODE_DEACTIVATION;
};

/**
 * @brief Indicates the +CHLD related supplementary services
 *
 * 3GPP TS 27.007 V3.9.0 (2001-06) Call related supplementary services +CHLD
 * 3GPP TS 27.007 V3.9.0 (2001-06) 7.22	Informative examples
 */
enum CallSupplementType {
    /**
     * default type
     */
    TYPE_DEFAULT = 0,
    /**
     * release the held call and the wait call
     */
    TYPE_HANG_UP_HOLD_WAIT = 1,
    /**
     * release the active call and recover the held call
     */
    TYPE_HANG_UP_ACTIVE = 2,
    /**
     * release all calls
     */
    TYPE_HANG_UP_ALL = 3,
};

enum CellularCallReturnType {
    /**
     * 3GPP TS 27.007 V3.9.0 (2001-06) 6.27	Informative examples
     */
    RETURN_TYPE_MMI = 0,
};

/**
 * @brief Indicates the voice domain preference.
 *
 * 3GPP TS 27.007 V17.3.0 (2021-09) 10.1.35	UE's voice domain
 * preference E-UTRAN +CEVDP
 */
enum DomainPreferenceMode {
    /**
     * Indicates the voice domain preference is CS only.
     */
    CS_VOICE_ONLY = 1,
    /**
     * Indicates the voice domain preference is CS prefered.
     */
    CS_VOICE_PREFERRED = 2,
    /**
     * Indicates the voice domain preference is PS prefered.
     */
    IMS_PS_VOICE_PREFERRED = 3,
    /**
     * Indicates the voice domain preference is PS only.
     */
    IMS_PS_VOICE_ONLY = 4,
};

/**
 * @brief Indicates the IMS call mode.
 */
enum ImsCallMode {
    CALL_MODE_AUDIO_ONLY = 0,
    CALL_MODE_SEND_ONLY,
    CALL_MODE_RECEIVE_ONLY,
    CALL_MODE_SEND_RECEIVE,
    CALL_MODE_VIDEO_PAUSED,
};

/**
 * @brief Indicates the request call media mode.
 */
struct CallMediaModeRequest {
    /**
     * Indicates the call phone number.
     */
    char phoneNum[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the IMS call mode. {@link ImsCallMode}
     */
    ImsCallMode mode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
};

/**
 * @brief Indicates the response of the call media mode request.
 */
struct CallMediaModeResponse {
    /**
     * Indicates the call phone number.
     */
    char phoneNum[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the response result.
     */
    int32_t result = 0;

    CallMediaModeResponse() {}

    CallMediaModeResponse(const CallMediaModeResponse &temp)
    {
        *this = temp;
    }

    CallMediaModeResponse &operator=(const CallMediaModeResponse &temp)
    {
        std::copy(std::begin(temp.phoneNum), std::end(temp.phoneNum), std::begin(phoneNum));
        result = temp.result;
        return *this;
    }
};

/**
 * @brief Indicates the specific IMS config item.
 */
enum ImsConfigItem {
    /**
     * Indicates the specific IMS config item is video quality.
     */
    ITEM_VIDEO_QUALITY = 0,

    /**
     * Indicates the specific IMS config item is IMS switch status.
     *
     * 3GPP TS 24.167 V17.1.0 (2020-12) 5.31
     * /<X>/Mobility_Management_IMS_Voice_Termination
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

/**
 * @brief Indicates the VoNR switch status.
 */
enum class VoNRState {
    /**
     * Indicates the VoNR switch is off.
     */
    VONR_STATE_OFF = 0,
    /**
     * Indicates the VoNR switch is on.
     */
    VONR_STATE_ON = 1,
};

/**
 * @brief Indicates the video quality is high or low.
 */
enum VideoQualityValue {
    /**
     * Indicates the video quality is high.
     */
    LOW = 0,
    /**
     * Indicates the video quality is low.
     */
    HIGH = 1,
};

enum ImsConfigValue {
    CONFIG_SUCCESS = 0,
    CONFIG_FAILED = 1,
};

/**
 * @brief Indicates the specific IMS feature.
 *
 * Official Document IR.92 - IMS Profile for Voice and SMS
 * Annex C MNO provisioning and Late Customization
 *
 * format: bool
 * 3GPP TS 24.167 V17.1.0 (2020-12) 5.43 /<X>/Media_type_restriction_policy
 */
enum FeatureType {
    /**
     * Indicates the IMS feature is Voice over LTE.
     */
    TYPE_VOICE_OVER_LTE = 0,
    /**
     * Indicates the IMS feature is Video over LTE.
     */
    TYPE_VIDEO_OVER_LTE,
    /**
     * Indicates the IMS feature is SS over UT.
     */
    TYPE_SS_OVER_UT,
};

/**
 * @brief Indicates the specific OTT call request service.
 */
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
    OTT_REQUEST_KICK_OUT_CONFERENCE,
    OTT_REQUEST_INVITE_TO_CONFERENCE,
    OTT_REQUEST_UPDATE_CALL_MEDIA_MODE,
};

/**
 * @brief Indicates the OTT call request information.
 */
struct OttCallRequestInfo {
    /**
     * Indicates the call phone number.
     */
    char phoneNum[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the bundle name.
     */
    char bundleName[kMaxBundleNameLen + 1] = { 0 };
    /**
     * Indicates the call video state, 0: audio 1:video.
     */
    VideoStateType videoState = VideoStateType::TYPE_VOICE;
};

/**
 * @brief Indicates the OTT call detail information.
 */
struct OttCallDetailsInfo {
    /**
     * Indicates the call phone number.
     */
    char phoneNum[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the bundle name.
     */
    char bundleName[kMaxBundleNameLen + 1] = { 0 };
    /**
     * Indicates the call video state, 0: audio 1:video.
     */
    VideoStateType videoState = VideoStateType::TYPE_VOICE;
    /**
     * Indicates the call status.
     */
    TelCallState callState = TelCallState::CALL_STATUS_UNKNOWN;
};

/**
 * @brief Indicates the OTT call event ID.
 */
enum class OttCallEventId {
    OTT_CALL_EVENT_FUNCTION_UNSUPPORTED = 0,
};

/**
 * @brief Indicates the OTT call event information.
 */
struct OttCallEventInfo {
    /**
     * Indicates the OTT call event ID.
     */
    OttCallEventId ottCallEventId = OttCallEventId::OTT_CALL_EVENT_FUNCTION_UNSUPPORTED;
    /**
     * Indicates the bundle name.
     */
    char bundleName[kMaxBundleNameLen + 1] = { 0 };

    OttCallEventInfo() {}

    OttCallEventInfo(const OttCallEventInfo &temp)
    {
        *this = temp;
    }

    OttCallEventInfo &operator=(const OttCallEventInfo &temp)
    {
        ottCallEventId = temp.ottCallEventId;
        std::copy(std::begin(temp.bundleName), std::end(temp.bundleName), std::begin(bundleName));
        return *this;
    }
};

/**
 * @brief Indicates detail information of a call.
 */
struct CallDetailInfo {
    /**
     * Indicates the call index.
     */
    int32_t index = 0;
    /**
     * Indicates the call phone number.
     */
    char phoneNum[kMaxNumberLen + 1] = { 0 };
    /**
     * Indicates the bundle name.
     */
    char bundleName[kMaxBundleNameLen + 1] = { 0 };
    /**
     * Indicates the account id.
     */
    int32_t accountId = 0;
    /**
     * Indicates the call type, CS, IMS, ERROR
     */
    CallType callType = CallType::TYPE_ERR_CALL;
    /**
     * Indicates the call video state, 0: audio 1:video.
     */
    VideoStateType callMode = VideoStateType::TYPE_VOICE;
    /**
     * Indicates the call status.
     */
    TelCallState state = TelCallState::CALL_STATUS_UNKNOWN;
    /**
     * Indicates the voice domain. 0: CS, 1: IMS
     */
    int32_t voiceDomain = 0;
    /**
     * Indicates the Multi-party call status:
     * 0: not a multi-party(conference) call
     * 1: a multi-party(conference) call
     */
    int32_t mpty = 0;

    CallDetailInfo() {}

    CallDetailInfo(const CallDetailInfo &temp)
    {
        *this = temp;
    }

    CallDetailInfo &operator=(const CallDetailInfo &temp)
    {
        index = temp.index;
        std::copy(std::begin(temp.phoneNum), std::end(temp.phoneNum), std::begin(phoneNum));
        std::copy(std::begin(temp.bundleName), std::end(temp.bundleName), std::begin(bundleName));
        accountId = temp.accountId;
        callType = temp.callType;
        callMode = temp.callMode;
        state = temp.state;
        voiceDomain = temp.voiceDomain;
        mpty = temp.mpty;
        return *this;
    }

};

/**
 * @brief Indicates detail information of some calls.
 */
struct CallDetailsInfo {
    /**
     * Indicates a CallDetailInfo list. {@link CallDetailInfo}
     */
    std::vector<CallDetailInfo> callVec {};
    /**
     * Indicates the slot id.
     */
    int32_t slotId = 0;
    /**
     * Indicates the bundle name.
     */
    char bundleName[kMaxBundleNameLen + 1] = { 0 };
};

/**
 * @brief Indicates the audio device type and address.
 */
struct AudioDevice {
    /**
     * Indicates the audio device type. {@link AudioDeviceType}
     */
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_UNKNOWN;
    /**
     * Indicates the device address.
     */
    char address[kMaxAddressLen + 1] = { 0 };
};

/**
 * @brief Indicates the audio device information.
 */
struct AudioDeviceInfo {
    /**
     * Indicates the list of all audio devices.
     */
    std::vector<AudioDevice> audioDeviceList {};
    /**
     * Indicates the current audio device.
     */
    AudioDevice currentAudioDevice;
    /**
     * Indicates wether the device is muted, true: mute, false: no mute
     */
    bool isMuted = false;
};

enum class MmiCodeResult {
    MMI_CODE_SUCCESS = 0,
    MMI_CODE_FAILED = 1
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_MANAGER_INNER_TYPE_H
