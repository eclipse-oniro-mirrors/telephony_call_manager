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

#ifndef TRANSFER_CONTROL_CALLBACK_STUB_H
#define TRANSFER_CONTROL_CALLBACK_STUB_H

#include <map>

#include "iremote_object.h"
#include "iremote_stub.h"

#include "transfer_control_callback_ipc_interface_code.h"
#include "i_transfer_control_callback.h"

namespace OHOS {
namespace Telephony {
class TransferControlCallbackStub : public IRemoteStub<ITransferControlCallback> {
public:
    TransferControlCallbackStub();
    virtual ~TransferControlCallbackStub();

    int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using TransferControlCallbackFunc = std::function<int32_t(MessageParcel &data, MessageParcel &reply)>;

    int32_t OnGetTransferResultInner(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateTransferCallInner(MessageParcel &data, MessageParcel &reply);
    int32_t OnDisconnectScoInner(MessageParcel &data, MessageParcel &reply);

    std::map<uint32_t, TransferControlCallbackFunc> memberFuncMap_;
};
} // namespace Telephony
} // namespace OHOS

#endif // TRANSFER_CONTROL_CALLBACK_STUB_H