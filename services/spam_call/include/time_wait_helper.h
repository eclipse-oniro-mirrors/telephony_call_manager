/*
 * Copyright (C) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_TIME_WAIT_HELPER_H
#define TELEPHONY_TIME_WAIT_HELPER_H

#include <condition_variable>
#include <cstdio>
#include "ffrt.h"

namespace OHOS {
namespace Telephony {
class TimeWaitHelper {
public:
    TimeWaitHelper(std::chrono::milliseconds waitTime);
    ~TimeWaitHelper();
    void NotifyAll();
    bool WaitForResult();

private:
    ffrt::condition_variable cv_;
    ffrt::mutex mutex_;
    std::chrono::milliseconds waitTime_{0};
    bool isNotified_ = false;
};
} // namespace Telephony
} // namespace OHOS
#endif //TELEPHONY_TIME_WAIT_HELPER_H
