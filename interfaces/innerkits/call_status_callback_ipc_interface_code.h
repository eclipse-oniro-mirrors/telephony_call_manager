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

#ifndef CALL_STATUS_CALLBACK_IPC_INTERFACE_CODE_H
#define CALL_STATUS_CALLBACK_IPC_INTERFACE_CODE_H

/* SAID:4005 */
namespace OHOS {
namespace Telephony {
enum CallStatusInterfaceCode {
    UPDATE_CALL_INFO = 0,
    UPDATE_CALLS_INFO,
    UPDATE_DISCONNECTED_CAUSE,
    UPDATE_EVENT_RESULT_INFO,
    UPDATE_RBT_PLAY_INFO,
    UPDATE_GET_WAITING,
    UPDATE_SET_WAITING,
    UPDATE_GET_RESTRICTION,
    UPDATE_SET_RESTRICTION,
    UPDATE_GET_TRANSFER,
    UPDATE_SET_TRANSFER,
    UPDATE_GET_CALL_CLIP,
    UPDATE_GET_CALL_CLIR,
    UPDATE_SET_CALL_CLIR,
    GET_IMS_SWITCH_STATUS,
    SET_IMS_SWITCH_STATUS,
    UPDATE_STARTRTT_STATUS,
    UPDATE_STOPRTT_STATUS,
    GET_IMS_CONFIG,
    SET_IMS_CONFIG,
    GET_IMS_FEATURE_VALUE,
    SET_IMS_FEATURE_VALUE,
    GET_LTE_ENHANCE_MODE,
    SET_LTE_ENHANCE_MODE,
    RECEIVE_UPDATE_MEDIA_MODE_RESPONSE,
    INVITE_TO_CONFERENCE,
    START_DTMF,
    STOP_DTMF,
    SEND_USSD,
    GET_IMS_CALL_DATA,
    MMI_CODE_INFO_RESPONSE,
    CLOSE_UNFINISHED_USSD,
    UPDATE_SET_RESTRICTION_PWD,
    POST_DIAL_CHAR,
    POST_DIAL_DELAY,
    CALL_SESSION_EVENT,
    PEER_DIMENSION_CHANGE,
    CALL_DATA_USAGE,
    CAMERA_CAPBILITIES_CHANGE,
    RECEIVE_UPDATE_MEDIA_MODE_REQUEST,
    UPDATE_VOIP_EVENT_INFO,
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_STATUS_CALLBACK_IPC_INTERFACE_CODE_H