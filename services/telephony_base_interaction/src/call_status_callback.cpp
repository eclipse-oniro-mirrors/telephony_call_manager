/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "call_status_callback.h"

#include "call_manager_errors.h"
#include "call_manager_log.h"

#include "cellular_call_info_handler.h"

namespace OHOS {
namespace TelephonyCallManager {
CallStatusCallback::CallStatusCallback() {}

CallStatusCallback::~CallStatusCallback() {}

int32_t CallStatusCallback::OnUpdateCallReportInfo(const CallReportInfo &info)
{
    int32_t ret = DelayedSingleton<CellularCallInfoHandlerService>::GetInstance()->UpdateCallReportInfo(info);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("UpdateCallReportInfo failed! errCode:%{public}d", ret);
    } else {
        CALLMANAGER_INFO_LOG("UpdateCallReportInfo success! state:%{public}d", info.state);
    }
    return ret;
}

int32_t CallStatusCallback::OnUpdateCallsReportInfo(const CallsReportInfo &info)
{
    int32_t ret = DelayedSingleton<CellularCallInfoHandlerService>::GetInstance()->UpdateCallsReportInfo(info);
    if (ret != TELEPHONY_NO_ERROR) {
        CALLMANAGER_ERR_LOG("UpdateCallReportInfo failed! errCode:%{public}d", ret);
    } else {
        CALLMANAGER_INFO_LOG("UpdateCallsReportInfo success!");
    }
    return ret;
}

int32_t CallStatusCallback::OnUpdateDisconnectedCause(const DisconnectedDetails &cause)
{
    return TELEPHONY_NO_ERROR;
}
} // namespace TelephonyCallManager
} // namespace OHOS