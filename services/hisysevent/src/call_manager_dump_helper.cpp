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

#include "call_manager_dump_helper.h"

#include "call_manager_service.h"
#include "core_service_client.h"

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

bool CallManagerDumpHelper::WhetherHasSimCard(const int32_t slotId) const
{
    bool hasSimCard = false;
    DelayedRefSingleton<CoreServiceClient>::GetInstance().HasSimCard(slotId, hasSimCard);
    return hasSimCard;
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
    result.append("CurrentTime:  ");
    result.append(DelayedSingleton<CallManagerService>::GetInstance()->GetBindTime());
    result.append("\n");
    result.append("SpendTime:");
    result.append(DelayedSingleton<CallManagerService>::GetInstance()->GetStartServiceSpent());
    result.append("\n");
    result.append("ServiceRunningState:");
    result.append(std::to_string(DelayedSingleton<CallManagerService>::GetInstance()->GetServiceRunningState()));
    result.append("\n");
    for (int32_t i = 0; i < SIM_SLOT_COUNT; i++) {
        if (WhetherHasSimCard(i)) {
            result.append("SlotId = ");
            result.append(std::to_string(i));
            result.append("\n");
        }
    }
    result.append("CallState:");
    result.append(std::to_string(DelayedSingleton<CallManagerService>::GetInstance()->GetCallState()));
    result.append("\n");
    result.append("RingingCallState:");
    bool enabled = false;
    DelayedSingleton<CallManagerService>::GetInstance()->IsRinging(enabled);
    result.append(std::to_string(enabled));
    result.append("\n");
    result.append("HasCall:");
    result.append(std::to_string(DelayedSingleton<CallManagerService>::GetInstance()->HasCall()));
    result.append("\n");
}
} // namespace Telephony
} // namespace OHOS