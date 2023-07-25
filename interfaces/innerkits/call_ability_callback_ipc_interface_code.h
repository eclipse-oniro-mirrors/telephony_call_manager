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

#ifndef CALL_ABILITY_CALLBACK_IPC_INTERFACE_CODE_H
#define CALL_ABILITY_CALLBACK_IPC_INTERFACE_CODE_H

/* SAID:4005 */
namespace OHOS {
namespace Telephony {
enum CallManagerCallAbilityInterfaceCode {
    UPDATE_CALL_STATE_INFO = 0,
    UPDATE_CALL_EVENT,
    UPDATE_CALL_DISCONNECTED_CAUSE,
    UPDATE_CALL_ASYNC_RESULT_REQUEST,
    REPORT_OTT_CALL_REQUEST,
    UPDATE_MMI_CODE_RESULT_REQUEST,
    UPDATE_AUDIO_DEVICE_CHANGE_RESULT_REQUEST,
    REPORT_POST_DIAL_DELAY,
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_ABILITY_CALLBACK_IPC_INTERFACE_CODE_H
