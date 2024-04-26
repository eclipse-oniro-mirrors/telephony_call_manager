/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef CALL_MANAGER_BASE_H
#define CALL_MANAGER_BASE_H

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
constexpr int16_t kMaxNumberLen = 255;
/**
 * @brief Indicates Maximum length of a bundle name.
 */
constexpr int16_t kMaxBundleNameLen = 100;
/**
 * @brief Indicates Maximum length of a address.
 */
constexpr int16_t kMaxAddressLen = 225;
/**
 * @brief Indicates Maximum length of a sco device name.
 */
constexpr int16_t kMaxDeviceNameLen = 64;
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
constexpr uint16_t ACCOUNT_NUMBER_MAX_LENGTH = 255;
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
 * @brief Indicates three second duration.
 */
constexpr int16_t WAIT_TIME_THREE_SECOND = 3;
/**
 * @brief Indicates two five duration.
 */
constexpr int16_t WAIT_TIME_FIVE_SECOND = 5;
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
    /**
     * Indicates the call type is VoIP.
     */
    TYPE_VOIP = 4,
    /**
     * Indicates the call type is SATELLITE.
     */
    TYPE_SATELLITE = 5,
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
    /**
     * Indicates the call is answered.
     */
    CALL_STATUS_ANSWERED,
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
     * Indicates the call is in send only state.
     */
    TYPE_SEND_ONLY,
    /**
     * Indicates the call is in send only state.
     */
    TYPE_RECEIVE_ONLY,
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
    CALL_STATE_OFFHOOK = 2,

    CALL_STATE_ANSWERED = 3
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
    /**
     * Indicates the call answer is call blocked.
     */
    CALL_ANSWER_BLOCKED = 6,
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
     * Indicates that the combine call failed.
     */
    EVENT_COMBINE_CALL_FAILED,
    /**
     * Indicates that the split call failed.
     */
    EVENT_SPLIT_CALL_FAILED,
    /**
     * Indicates that the OTT is not supported.
     */
    EVENT_OTT_FUNCTION_UNSUPPORTED,
};

/**
 * @brief Indicates the event ID of call ability.
 */
enum class CallSessionEventId {
    /**
     * Indicates the camera event failure type.
     */
    EVENT_CAMERA_FAILURE = 0,
    /**
     * Indicates the camera event ready type.
     */
    EVENT_CAMERA_READY,
    /**
     * Indicates the display surface release type.
     */
    EVENT_RELEASE_DISPLAY_SURFACE = 100,
    /**
     * Indicates the preview surface release type.
     */
    EVENT_RELEASE_PREVIEW_SURFACE,
};

/**
 * @brief Indicates the type of device direction.
 */
enum DeviceDirection {
    /**
     * Indicates the device direction is 0 degree.
     */
    DEVICE_DIRECTION_0 = 0,
    /**
     * Indicates the device direction is 90 degree.
     */
    DEVICE_DIRECTION_90 = 90,
    /**
     * Indicates the device direction is 180 degree.
     */
    DEVICE_DIRECTION_180 = 180,
    /**
     * Indicates the device direction is 270 degree.
     */
    DEVICE_DIRECTION_270 = 270,
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
    /**
     * Indicates the device type is a distributed car device.
     */
    DEVICE_DISTRIBUTED_AUTOMOTIVE,
    /**
     * Indicates the device type is a distributed phone device.
     */
    DEVICE_DISTRIBUTED_PHONE,
    /**
     * Indicates the device type is a distributed pad device.
     */
    DEVICE_DISTRIBUTED_PAD,
};

/**
 * @brief Indicates the type of the number mark..
 */
enum class MarkType {
    /**
     * Indicates the mark is none.
     */
    MARK_TYPE_NONE = 0,
    /**
     * Indicates the mark is crank.
     */
    MARK_TYPE_CRANK,
    /**
     * Indicates the mark is fraud.
     */
    MARK_TYPE_FRAUD,
    /**
     * Indicates the mark is express.
     */
    MARK_TYPE_EXPRESS,
    /**
     * Indicates the mark is promote sales.
     */
    MARK_TYPE_PROMOTE_SALES,
    /**
     * Indicates the mark is house agent.
     */
    MARK_TYPE_HOUSE_AGENT,
    /**
     * Indicates the mark is insurance.
     */
    MARK_TYPE_INSURANCE,
    /**
     * Indicates the mark is taxi.
     */
    MARK_TYPE_TAXI,
    /**
     * Indicates the mark is custom.
     */
    MARK_TYPE_CUSTOM,
    /**
     * Indicates the mark is others.
     */
    MARK_TYPE_OTHERS,
    /**
     * Indicates the mark is yellow page.
     */
    MARK_TYPE_YELLOW_PAGE,
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
 * @brief Indicates the temperature level for satellite call.
 */
enum class SatCommTempLevel {
    /*
     *  Indicates the low temperature level.(< 51 degressCelsius)
     */
    TEMP_LEVEL_LOW = 0,
    /*
     *  Indicates the middle temperature level.(>= 51 degressCelsius  && < 53 degressCelsius)
     */
    TEMP_LEVEL_MIDDLE,
    /*
     *  Indicates the high temperature level.(>= 53 degressCelsius)
     */
    TEMP_LEVEL_HIGH,
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_MANAGER_BASE_H