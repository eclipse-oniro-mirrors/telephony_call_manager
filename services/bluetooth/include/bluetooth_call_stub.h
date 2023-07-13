/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef BLUETOOTH_CALL_STUB_H
#define BLUETOOTH_CALL_STUB_H

#include <map>

#include "iremote_object.h"
#include "iremote_stub.h"

#include "bluetooth_call_ipc_interface_code.h"
#include "i_bluetooth_call.h"

namespace OHOS {
namespace Telephony {
class BluetoothCallStub : public IRemoteStub<IBluetoothCall> {
public:
    BluetoothCallStub();
    ~BluetoothCallStub();
    virtual int32_t OnRemoteRequest(
            uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using BluetoothCallFunc = int32_t (BluetoothCallStub::*)(MessageParcel &data, MessageParcel &reply);

    int32_t OnAnswerCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnRejectCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnHangUpCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetBtCallState(MessageParcel &data, MessageParcel &reply);
    int32_t OnHoldCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnUnHoldCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnSwitchCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnStartDtmf(MessageParcel &data, MessageParcel &reply);
    int32_t OnStopDtmf(MessageParcel &data, MessageParcel &reply);
    int32_t OnCombineConference(MessageParcel &data, MessageParcel &reply);
    int32_t OnSeparateConference(MessageParcel &data, MessageParcel &reply);
    int32_t OnKickOutFromConference(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetCurrentCallList(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothCallFunc> memberFuncMap_;
};
} // namespace Telephony
} // namespace OHOS

#endif // BLUETOOTH_CALL_STUB_H