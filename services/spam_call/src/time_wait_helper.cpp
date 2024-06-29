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

#include "time_wait_helper.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

TimeWaitHelper::TimeWaitHelper(int16_t waitTime)
{
    waitTime_ = waitTime;
}

TimeWaitHelper::~TimeWaitHelper()
{
    TELEPHONY_LOGI("~TimeWaitHelper: %{public}d", waitTime_);
}

void TimeWaitHelper::NotifyAll()
{
    std::unique_lock<ffrt::mutex> lock(mutex_);
    TELEPHONY_LOGI("TimeWaitHelper: %{public}d NotifyAll", waitTime_);
    isNotified_ = true;
    cv_.notify_all();
}

bool TimeWaitHelper::WaitForResult()
{
    if (!isNotified_) {
        std::unique_lock<ffrt::mutex> lock(mutex_);
        auto now = std::chrono::system_clock::now();
        while (!isNotified_) {
            if (cv_.wait_until(lock, now + std::chrono::seconds(waitTime_)) == ffrt::cv_status::timeout) {
                TELEPHONY_LOGE("TimeWaitHelper: %{public}d time out", waitTime_);
                return false;
            }
        }
        return true;
    }
    TELEPHONY_LOGE("TimeWaitHelper: %{public}d isNotified_ is true", waitTime_);
    return false;
}
} // namespace Telephony
} // namespace OHOS