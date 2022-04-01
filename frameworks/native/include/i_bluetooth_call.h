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

#ifndef I_BLUETOOTH_CALL_H
#define I_BLUETOOTH_CALL_H

#include <cstdio>
#include <memory>
#include <string>

#include "iremote_broker.h"
#include "pac_map.h"

#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
extern "C" {
enum BluetoothCallSurfaceCode {
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
};
} // end extern

class IBluetoothCall : public IRemoteBroker {
public:
    virtual ~IBluetoothCall() = default;
    virtual int32_t AnswerCall() = 0;
    virtual int32_t RejectCall() = 0;
    virtual int32_t HangUpCall() = 0;
    virtual int32_t GetCallState() = 0;
    virtual int32_t HoldCall() = 0;
    virtual int32_t UnHoldCall() = 0;
    virtual int32_t SwitchCall() = 0;
    virtual int32_t CombineConference() = 0;
    virtual int32_t SeparateConference() = 0;
    virtual int32_t StartDtmf(char str) = 0;
    virtual int32_t StopDtmf() = 0;
    virtual std::vector<CallAttributeInfo> GetCurrentCallList(int32_t slotId) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.IBluetoothCall");
};
} // namespace Telephony
} // namespace OHOS

#endif // I_BLUETOOTH_CALL_H