/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#ifndef TRANSFER_CONTROL_CALLBACK_IPC_INTERFACE_CODE_H
#define TRANSFER_CONTROL_CALLBACK_IPC_INTERFACE_CODE_H

namespace OHOS {
namespace Telephony {
enum TransferControlInterfaceCode {
    IS_TRANSFER_CALL_ALLOW,
    UPDATE_TRANSFER_CALL,
    DISCONNECT_SCO,
};
} // namespace Telephony
} // namespace OHOS
#endif // TRANSFER_CONTROL_CALLBACK_IPC_INTERFACE_CODE_H