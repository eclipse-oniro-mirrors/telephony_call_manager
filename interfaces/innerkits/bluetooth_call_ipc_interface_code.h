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

#ifndef BLUETOOTH_CALL_IPC_INTERFACE_CODE_H
#define BLUETOOTH_CALL_IPC_INTERFACE_CODE_H

/* SAID:4005 */
namespace OHOS {
namespace Telephony {
extern "C" {
enum class BluetoothCallInterfaceCode {
    INTERFACE_BT_ANSWER_CALL = 0,
    INTERFACE_BT_REJECT_CALL,
    INTERFACE_BT_HOLD_CALL,
    INTERFACE_BT_UNHOLD_CALL,
    INTERFACE_BT_DISCONNECT_CALL,
    INTERFACE_BT_GET_CALL_STATE,
    INTERFACE_BT_SWAP_CALL,
    INTERFACE_BT_COMBINE_CONFERENCE,
    INTERFACE_BT_SEPARATE_CONFERENCE,
    INTERFACE_BT_START_DTMF,
    INTERFACE_BT_STOP_DTMF,
    INTERFACE_BT_GET_CURRENT_CALL_LIST,
    INTERFACE_BT_KICK_OUT_CONFERENCE,
    INTERFACE_BT_ADD_AUDIO_DEVICE,
    INTERFACE_BT_REMOVE_AUDIO_DEVICE,
    INTERFACE_BT_RESET_NEARLINK_AUDIO_DEVICE,
    INTERFACE_BT_RESET_BT_HEARINGAID_AUDIO_DEVICE,
};
} // end extern
} // namespace Telephony
} // namespace OHOS
#endif // BLUETOOTH_CALL_IPC_INTERFACE_CODE_H