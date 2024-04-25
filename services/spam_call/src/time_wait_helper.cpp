//
// Created by h00840185 on 2024/4/25.
//

#include "time_wait_helper.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

TimeWaitHelper::TimeWaitHelper(int16_t waitTime) {
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