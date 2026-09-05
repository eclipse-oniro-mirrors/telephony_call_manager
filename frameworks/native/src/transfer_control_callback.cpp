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

#include "transfer_control_callback.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "i_transfer_control_callback.h"

namespace OHOS {
namespace Telephony {
TransferControlCallback::TransferControlCallback()
{
    TELEPHONY_LOGI("TransferControlCallback::TransferControlCallback");
}

TransferControlCallback::~TransferControlCallback()
{
    TELEPHONY_LOGI("TransferControlCallback::~TransferControlCallback");
}

int32_t TransferControlCallback::SetProcessCallback(std::unique_ptr<TransferControl> callback)
{
    TELEPHONY_LOGI("TransferControlCallback::SetProcessCallback");
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (callback == nullptr) {
        return -1;
    }

    callbackPtr_ = std::move(callback);
    return 0;
}

int32_t TransferControlCallback::OnGetTransferResult(const TransferCallInfo &info, TransferResultInfo &result)
{
    TELEPHONY_LOGI("TransferControlCallback::OnGetTransferResult");
    std::weak_ptr<TransferControl> weakPtr;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        weakPtr = callbackPtr_;
    }
    auto callbackPtr = weakPtr.lock();
    if (callbackPtr == nullptr) {
        TELEPHONY_LOGE("TransferControlCallback::OnGetTransferResult, callbackPtr is nullptr");
        return -1;
    }

    bool isNeed = callbackPtr->IsCallNeedTransfer(info, result);
    TELEPHONY_LOGI("TransferControlCallback::OnGetTransferResult, isNeed=%{public}d", isNeed);
    if (isNeed) {
        return 1;
    }

    return 0;
}

int32_t TransferControlCallback::OnUpdateTransferCall(const TransferCallInfo &info)
{
    TELEPHONY_LOGI("TransferControlCallback::OnUpdateTransferCall");
    std::weak_ptr<TransferControl> weakPtr;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        weakPtr = callbackPtr_;
    }
    auto callbackPtr = weakPtr.lock();
    if (callbackPtr == nullptr) {
        TELEPHONY_LOGE("TransferControlCallback::OnUpdateTransferCall, callbackPtr is nullptr");
        return -1;
    }

    bool isSucc = callbackPtr->NotifyCallState(info);
    if (isSucc) {
        return 0;
    }

    TELEPHONY_LOGE("TransferControlCallback::OnUpdateTransferCall, NotifyCallState fail");
    return -1;
}

int32_t TransferControlCallback::OnDisconnectSco(const std::string &devMac)
{
    TELEPHONY_LOGI("TransferControlCallback::OnDisconnectSco");
    std::weak_ptr<TransferControl> weakPtr;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        weakPtr = callbackPtr_;
    }
    auto callbackPtr = weakPtr.lock();
    if (callbackPtr == nullptr) {
        TELEPHONY_LOGE("TransferControlCallback::OnDisconnectSco, callbackPtr is nullptr");
        return -1;
    }

    if (callbackPtr->DisconnectSco(devMac)) {
        return 0;
    }

    TELEPHONY_LOGE("TransferControlCallback::OnDisconnectSco, DisconnectSco fail");
    return -1;
}
} // namespace Telephony
} // namespace OHOS