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

#include "call_manager_dump_helper.h"

#include "call_manager_service.h"

namespace OHOS {
namespace Telephony {
bool CallManagerDumpHelper::Dump(const std::vector<std::string> &args, std::string &result) const
{
    result.clear();
    ShowHelp(result);
    ShowCallManagerInfo(result);
    return true;
}

CallManagerDumpHelper::CallManagerDumpHelper()
{
    TELEPHONY_LOGI("CallManagerDumpHelper() entry.");
}

void CallManagerDumpHelper::ShowHelp(std::string &result) const
{
    result.append("Usage:dump <command> [options]\n")
        .append("Description:\n")
        .append("-call_manager_info          ")
        .append("dump all call_manager information in the system\n")
        .append("-input_simulate <event>    ")
        .append("simulate event from ohos call_manager, supported events: login/logout/token_invalid\n")
        .append("-output_simulate <event>    ")
        .append("simulate event output\n")
        .append("-show_log_level        ")
        .append("show call_manager SA's log level\n")
        .append("-set_log_level <level>     ")
        .append("set call_manager SA's log level\n")
        .append("-perf_dump         ")
        .append("dump performance statistics\n");
}

void CallManagerDumpHelper::ShowCallManagerInfo(std::string &result) const
{
    result.append("Ohos call_manager service:");
    result.append("\n");
    result.append("Ohos call_manager bind time:  ");
    result.append(DelayedSingleton<CallManagerService>::GetInstance()->GetBindTime());
    result.append("\n");
    result.append("Ohos call_manager start spend time(milliseconds):  ");
    result.append(DelayedSingleton<CallManagerService>::GetInstance()->GetStartServiceSpent());
    result.append("\n");
}
} // namespace Telephony
} // namespace OHOS