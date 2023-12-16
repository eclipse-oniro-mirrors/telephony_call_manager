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

#ifndef VOIP_CALL_MANAGER_INFO_H
#define VOIP_CALL_MANAGER_INFO_H
#include <cstdint>
#include <string>

#include "pixel_map.h"

namespace OHOS {
namespace Telephony {

/**
 * @brief Indicates the voip call state.
 */
enum class VoipCallState {
    /**
     * Indicates the call state of idle.
     */
    VOIP_CALL_STATE_IDLE = 0,
    /**
     * Indicates the call state of incoming.
     */
    VOIP_CALL_STATE_INCOMING,
    /**
     * Indicates the call state of active.
     */
    VOIP_CALL_STATE_ACTIVE,
    /**
     * Indicates the call state is holding.
     */
    VOIP_CALL_STATE_HOLDING,
    /**
     * Indicates the call state of disconnected.
     */
    VOIP_CALL_STATE_DISCONNECTED,
};

/**
 * @brief Indicates the voip call type.
 */
enum class VoipCallType {
    /**
     * Indicates the type of voip voice call.
     */
    VOIP_CALL_VOICE = 0,
    /**
     * Indicates the type of voip video call.
     */
    VOIP_CALL_VIDEO,
};

/**
 * @brief Indicates the voip call attribute.
 */
struct VoipCallAttribute {
    /**
     * Indicates the callid
     */
    std::string callId = "";
    /**
     * Indicates the bundleName.
     */
    std::string bundleName = "";
    /**
     * Indicates the type of voip call.
     */
    VoipCallType voipCallType = VoipCallType::VOIP_CALL_VOICE;
    /**
     * Indicates the userName.
     */
    std::string userName = "";
    /**
     * Indicates the user Porfile.
     */
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    /**
     * Indicates the package Profile.
     */
    std::string abilityName = "";
    /**
     * Indicates the state of voip call.
     */
    VoipCallState voipCallState = VoipCallState::VOIP_CALL_STATE_IDLE;
    /**
     * Indicates the extensionId of voip call.
     */
    std::string extensionId = "";
};

/**
 * @brief Indicates the voip call event.
 */
enum class VoipCallEvent {
    /**
     * Indicates the click event of default.
     */
    VOIP_CALL_EVENT_NONE = 0,
    /**
     * Indicates the click event of answer voice.
     */
    VOIP_CALL_EVENT_ANSWER_VOICE,
    /**
     * Indicates the click event of answer video.
     */
    VOIP_CALL_EVENT_ANSWER_VIDEO,
    /**
     * Indicates the click event of reject.
     */
    VOIP_CALL_EVENT_REJECT,
    /**
     * Indicates the voip call was hung up by other cause.
     */
    VOIP_CALL_EVENT_HANGUP,
};

/**
 * @brief Indicates the ErrorReason.
 */
enum class ErrorReason {
    /**
     * Indicates there is no error.
     */
    ERROR_NONE = 0,

    /**
     * Indicates there is already a cellular call.
     */
    CELLULAR_CALL_EXISTS = 1,

    /**
     * Indicates there is already a voip call.
     */
    VOIP_CALL_EXISTS = 2,

    /**
     * Indicates this is a invalid call.
     */
    INVALID_CALL = 3,

    /**
     * Indicates the user answered the cellular call first.
     */
    USER_ANSWER_CELLULAR_FIRST = 4,
};

/**
 * @brief Indicates the voip call Events.
 */
struct VoipCallEvents {
    /**
     * Indicates the callid
     */
    std::string voipCallId = "";
    /**
     * Indicates the bundleName.
     */
    std::string bundleName = "";
    /**
     * Indicates the click event of voip.
     */
    VoipCallEvent voipCallEvent = VoipCallEvent::VOIP_CALL_EVENT_NONE;
    /**
     * Indicates the error reason.
     */
    ErrorReason errorReason = ErrorReason::ERROR_NONE;
};

/**
 * @brief Indicates the ErrorReason.
 */
enum class ReportVoipCallFailedCause {
    /**
     * Indicates other failed cause.
     */
    OTHER_CAUSED,
    /**
     * Indicates application line is busy.
     */
    ROUTE_BUSY,
    /**
     * Indicates application failed to establish connect.
     */
    CONNECT_FAILED,
};

/**
 * @brief Indicates the fail cause of voip call.
 */
struct ReportVoipCallFailedCauses {
    /**
     * Indicates the callid
     */
    std::string callId = "";
    /**
     * Indicates the bundleName.
     */
    std::string bundleName = "";
    /**
     * Indicates the the fail cause of voip call.
     */
    ReportVoipCallFailedCause reportVoipCallFailedCause = ReportVoipCallFailedCause::OTHER_CAUSED;
};

struct ReportVoIPCallParams {
    std::string callId = "";
    int32_t status;
};

enum VoipCallErrorCode {
    VOIP_ERR_CALLBACK_NOT_EXIST,
};

enum class VoIPCallStatus : uint32_t {
    ORIGINAL_STATE = 0,

    VOIP_EXTENSION_CONNECT_SUCCEED = 1,
    VOIP_EXTENSION_CONNECT_FAILED = 2,
    VOIP_EXTENSION_BUSY = 3,
    VOIP_EXTENSION_CALLBACK_ILLEAGAL_PARAM = 4,
    VOIP_EXTENSION_OTHER_REASON = 5,

    VOIP_MANAGER_BUSY = 6,
    VOIP_MANAGER_OTHER_REASON = 7,

    ANSWERED_SUCCEED = 20,
    ANSWERED_FAILED = 21,
    REFUSE_ANSWER = 22,
    DELETE_CALL_NOTIFICATION = 23,
    HANG_UP_DURING_CALLING = 24,
    LONGTIME_CALL_NOT_ANSWER = 25,
    CALL_END_OTHER_REASON = 26,

    HANG_UP_DURING_ANSWERED = 40,
    VOIP_MANAGER_DESTROYED = 41,
    HANG_UP_DURING_ANSWERED_OTHER_REASON = 42,

    VOIP_MANAGER_INVALID_PARAM = 58,
    VOIP_MANAGER_CALL_NOT_EXIST = 59,

    PUSH_MANAGER_SERVICE_LOAD_SUCCESS = 60,
    PUSH_MANAGER_SERVICE_LOAD_FAILED_SYSTEM_ERROR = 61,
    PUSH_MANAGER_SERVICE_LOAD_FAILED_ILLEGAL_EXTENSION = 62,
    PUSH_MANAGER_SERVICE_LOAD_FAILED_CALLS_EXCEEDED = 63,
    PUSH_MANAGER_SERVICE_LOAD_FAILED_VOIP_INTERNAL_ERROR = 64,
    PUSH_MANAGER_SERVICE_LOAD_FAILED_ILLEGAL_CALL_ID = 65,
    PUSH_MANAGER_SERVICE_LOAD_FAILED_OTHER_REASON = 66,

    VOIP_TIMER_TRIGGER = 70,
};
} // namespace Telephony
} // namespace OHOS
#endif
