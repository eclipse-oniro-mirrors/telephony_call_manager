/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "callback_stub_helper.h"
#include "telephony_log_wrapper.h"
#include "call_manager_errors.h"
#include "spam_call_adapter.h"

namespace OHOS {
namespace Telephony {
CallbackStubHelper::CallbackStubHelper(SpamCallAdapter *spamCallAdapter)
{
    spamCallAdapter_ = spamCallAdapter;
};
CallbackStubHelper::~CallbackStubHelper() {};

int32_t CallbackStubHelper::OnResult(int32_t &errCode, std::string &result)
{
    TELEPHONY_LOGI("OnResult errCode: %{public}d", errCode);
    if (spamCallAdapter_ != nullptr) {
        spamCallAdapter_->SetDetectResult(errCode, result);
        spamCallAdapter_->NotifyAll();
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS