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

TimeWaitHelper::~TimeWaitHelper() {}

void TimeWaitHelper::NotifyAll()
{
    TELEPHONY_LOGI("TimeWaitHelper NotifyAll");
    cv_.notify_all();
}

bool TimeWaitHelper::WaitForResult()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (cv_.wait_for(lock, std::chrono::seconds(waitTime_)) == std::cv_status::timeout) {
        TELEPHONY_LOGE("TimeWaitHelper time out");
        return false;
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS