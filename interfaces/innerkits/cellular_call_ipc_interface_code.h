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

#ifndef CELLULAR_CALL_IPC_INTERFACE_CODE_H
#define CELLULAR_CALL_IPC_INTERFACE_CODE_H

/* SAID:4005 */
namespace OHOS {
namespace Telephony {
enum class CellularCallInterfaceCode {
    /****************** basic ******************/
    DIAL = 1,
    HANG_UP,
    REJECT,
    ANSWER,
    HOLD_CALL,
    UN_HOLD_CALL,
    SWITCH_CALL,
    EMERGENCY_CALL,
    SET_EMERGENCY_CALL_LIST,
    COMBINE_CONFERENCE,
    SEPARATE_CONFERENCE,
    INVITE_TO_CONFERENCE,
    KICK_OUT_CONFERENCE,
    HANG_UP_ALL_CONNECTION,
    SET_READY_TO_CALL,
    UPDATE_CALL_MEDIA_MODE,
    REGISTER_CALLBACK,
    UNREGISTER_CALLBACK,

    /****************** dtmf rtt ******************/
    START_DTMF = 100,
    STOP_DTMF,
    SEND_DTMF,
    START_RTT,
    STOP_RTT,

    /****************** supplement ******************/
    SET_CALL_TRANSFER = 200,
    GET_CALL_TRANSFER,
    CAN_SET_CALL_TRANSFER_TIME,
    SET_CALL_WAITING,
    GET_CALL_WAITING,
    SET_CALL_RESTRICTION,
    GET_CALL_RESTRICTION,
    CLOSE_UNFINISHED_USSD,
    SET_CALL_RESTRICTION_PWD,

    /****************** config ******************/
    SET_DOMAIN_PREFERENCE_MODE = 300,
    GET_DOMAIN_PREFERENCE_MODE,
    SET_IMS_SWITCH_STATUS,
    GET_IMS_SWITCH_STATUS,
    SET_IMS_CONFIG_STRING,
    SET_IMS_CONFIG_INT,
    GET_IMS_CONFIG,
    SET_IMS_FEATURE,
    GET_IMS_FEATURE,
    SET_MUTE,
    GET_MUTE,
    SET_VONR_SWITCH_STATUS,
    GET_VONR_SWITCH_STATUS,

    /****************** video settings ******************/
    CTRL_CAMERA = 400,
    SET_PREVIEW_WINDOW,
    SET_DISPLAY_WINDOW,
    SET_CAMERA_ZOOM,
    SET_PAUSE_IMAGE,
    SET_DEVICE_DIRECTION,
};
} // namespace Telephony
} // namespace OHOS
#endif // CELLULAR_CALL_IPC_INTERFACE_CODE_H