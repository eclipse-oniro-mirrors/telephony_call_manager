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

#ifndef TRANSFER_CONTROL_CALLBACK_PROXY_H
#define TRANSFER_CONTROL_CALLBACK_PROXY_H

#include "iremote_proxy.h"

#include "call_ability_callback_ipc_interface_code.h"
#include "i_transfer_control_callback.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
class TransferControlCallbackProxy : public IRemoteProxy<ITransferControlCallback> {
public:
    explicit TransferControlCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~TransferControlCallbackProxy() = default;

    int32_t OnGetTransferResult(const TransferCallInfo &info, TransferResultInfo &result) override;
    int32_t OnUpdateTransferCall(const TransferCallInfo &info) override;
    int32_t OnDisconnectSco(const std::string &devMac) override;

private:
    __attribute__((noinline)) int32_t SendRequest(int32_t msgId, MessageParcel &dataParcel, MessageParcel &replyParcel,
        MessageOption &option);

private:
    static inline BrokerDelegator<TransferControlCallbackProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS

#endif
