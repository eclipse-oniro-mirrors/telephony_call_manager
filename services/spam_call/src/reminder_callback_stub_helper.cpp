/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "reminder_callback_stub_helper.h"
#include "telephony_log_wrapper.h"
#include "call_manager_errors.h"
#include "spam_call_adapter.h"
#include "call_number_utils.h"
#include "call_manager_hisysevent.h"

namespace OHOS {
namespace Telephony {
ReminderCallbackStubHelper::ReminderCallbackStubHelper(std::shared_ptr<SpamCallAdapter> spamCallAdapter)
{
    spamCallAdapter_ = spamCallAdapter;
};
ReminderCallbackStubHelper::~ReminderCallbackStubHelper()
{
    TELEPHONY_LOGW("~ReminderCallbackStubHelper");
};

int32_t ReminderCallbackStubHelper::OnResult(int32_t &errCode, std::string &result)
{
    TELEPHONY_LOGI("OnResult errCode: %{public}d", errCode);
    if (errCode == 0) {
        spamCallAdapter_->ParseNeedNotifyResult(result);
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS