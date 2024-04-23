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

#include "ring_once_helper.h"

#include "call_manager_base.h"
#include "call_manager_info.h"
#include "telephony_log_wrapper.h"
#include "uri.h"
#include "settings_datashare_helper.h"
#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
std::condition_variable RingOnceHelper::cv_;

RingOnceHelper::RingOnceHelper() {}

RingOnceHelper::~RingOnceHelper() {}

bool RingOnceHelper::IsRingOnceCall(const sptr<CallBase> &call, const CallDetailInfo &info)
{
    NumberMarkInfo numberMarkInfo = call->GetNumberMarkInfo();
    ContactInfo contactInfo = call->GetCallerInfo();
    if (numberMarkInfo.markType == MarkType::MARK_TYPE_YELLOW_PAGE ||
        std::string(contactInfo.name) != "") {
        TELEPHONY_LOGI("is not unknown phonenumber");
        return false;
    }
    auto datashareHelper = std::make_shared<SettingsDataShareHelper>();
    std::string is_check_ring_once {"0"};
    std::string key = "spamshield_sim" + std::to_string(info.accountId + 1) + "_phone_switch_ring_once";
    OHOS::Uri uri(
        "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=" + key);
    int32_t ret = datashareHelper->Query(uri, key, is_check_ring_once);
    if (ret != TELEPHONY_SUCCESS || is_check_ring_once == "0") {
        TELEPHONY_LOGI("is_check_ring_once = 0, not need check ring once call");
        return false;
    }
    if (!WaitForDetectResult()) {
        return false;
    }
    TELEPHONY_LOGI("is ring once call");
    SetDetectFlag(false);
    return true;
}

int32_t RingOnceHelper::HandleRingOnceCall(sptr<CallBase> &call)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("call is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    if (ret != TELEPHONY_SUCCESS && ret != CALL_ERR_NOT_NEW_STATE) {
        TELEPHONY_LOGE("Set CallState failed!");
        return ret;
    }
    return DelayedSingleton<CallControlManager>::GetInstance()->AddCallLogAndNotification(call);
}

bool RingOnceHelper::GetDetectFlag()
{
    return isDetected_;
}

void RingOnceHelper::SetDetectFlag(bool isDetected)
{
    isDetected_ = isDetected;
}

void RingOnceHelper::NotifyAll()
{
    TELEPHONY_LOGI("RingOnceHelper NotifyAll");
    cv_.notify_all();
}

bool RingOnceHelper::WaitForDetectResult()
{
    std::unique_lock<std::mutex> lock(mutex_);
    isDetected_ = false;
    while (!isDetected_) {
        if (cv_.wait_for(lock, std::chrono::seconds(WAIT_TIME_THREE_SECOND)) == std::cv_status::timeout) {
            TELEPHONY_LOGE("detect ring once call is time out");
            return false;
        }
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS
