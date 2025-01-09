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
#include "call_number_utils.h"
#include "call_manager_hisysevent.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t DECISION_REASON_TRUSTLIST = 2;
CallbackStubHelper::CallbackStubHelper(std::shared_ptr<SpamCallAdapter> spamCallAdapter)
{
    spamCallAdapter_ = spamCallAdapter;
};
CallbackStubHelper::~CallbackStubHelper()
{
    TELEPHONY_LOGW("~CallbackStubHelper");
};

int32_t CallbackStubHelper::OnResult(int32_t &errCode, std::string &result)
{
    TELEPHONY_LOGI("OnResult errCode: %{public}d", errCode);
    if (spamCallAdapter_ == nullptr) {
        TELEPHONY_LOGE("spamCallAdapter_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    spamCallAdapter_->SetDetectResult(errCode, result);
    if (errCode == 0) {
        NumberMarkInfo numberMarkInfo;
        bool isBlock = false;
        int32_t blockReason;
        std::string detectDetails = "";
        spamCallAdapter_->ParseDetectResult(result, isBlock, numberMarkInfo, blockReason, detectDetails);
        if (blockReason == DECISION_REASON_TRUSTLIST) {
            TELEPHONY_LOGW("trustlist, need query numbermark");
            DelayedSingleton<CallNumberUtils>::GetInstance()->
                QueryYellowPageAndMarkInfo(numberMarkInfo, spamCallAdapter_->GetDetectPhoneNum());
        }
        spamCallAdapter_->SetParseResult(isBlock, numberMarkInfo, blockReason, detectDetails);
        CallManagerHisysevent::WriteIncomingNumIdentityBehaviorEvent(
            static_cast<int32_t>(numberMarkInfo.markType), isBlock, blockReason);
    }
    spamCallAdapter_->NotifyAll();
    if (spamCallAdapter_->GetCalDisconnect()) {
        spamCallAdapter_->DisconnectSpamCallAbility();
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallbackStubHelper::OnNeedNotifyResult(int32_t &errCode, std::string &result)
{
    TELEPHONY_LOGI("OnNeedNotifyResult errCode: %{public}d", errCode);
    if (errCode == 0) {
        spamCallAdapter_->ParseNeedNotifyResult(result);
    }
    spamCallAdapter_->DisconnectSpamCallAbility();
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS