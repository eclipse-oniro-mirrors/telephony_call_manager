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

#ifndef TRANSFER_CONTROL_CALLBACK_H
#define TRANSFER_CONTROL_CALLBACK_H

#include <memory>
#include <mutex>

#include "ffrt.h"
#include "ipc_types.h"
#include "transfer_control.h"
#include "transfer_control_callback_stub.h"

namespace OHOS {
namespace Telephony {
class TransferControlCallback : public TransferControlCallbackStub {
public:
    TransferControlCallback();
    ~TransferControlCallback() override;

    int32_t SetProcessCallback(std::unique_ptr<TransferControl> control);

    int32_t OnGetTransferResult(const TransferCallInfo &info, TransferResultInfo &result) override;
    int32_t OnUpdateTransferCall(const TransferCallInfo &info) override;
    int32_t OnDisconnectSco(const std::string &devMac) override;

private:
    std::shared_ptr<TransferControl> callbackPtr_ = nullptr;
    ffrt::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS

#endif