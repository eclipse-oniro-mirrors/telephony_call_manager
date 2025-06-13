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

#include "edm_call_policy.h"

#include <shared_mutex>
#include "call_manager_base.h"
#include "call_manager_errors.h"
#include "call_manager_inner_type.h"
#include "call_number_utils.h"
#include "parameters.h"

namespace OHOS {
namespace Telephony {
const std::string PARAM_DISALLOWED_TELEPHONY_CALL = "persist.edm.telephony_call_disable";
constexpr int32_t MAX_COUNT = 1000;

EdmCallPolicy::EdmCallPolicy() {}
EdmCallPolicy::~EdmCallPolicy() {}

int32_t EdmCallPolicy::SetCallPolicy(int32_t dialingPolicy, const std::vector<std::string> &dialingList,
    int32_t incomingPolicy, const std::vector<std::string> &incomingList)
{
    TELEPHONY_LOGI("enter EdmCallPolicy::SetCallPolicy");
    if (dialingList.size() > MAX_COUNT || incomingList.size() > MAX_COUNT) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::unique_lock<ffrt::shared_mutex> lock(rwMutex_);
    dialingList_.numberList.clear();
    incomingList_.numberList.clear();
    dialingList_.policyFlag = (EDMPolicyFlag)dialingPolicy;
    for (auto item : dialingList) {
        dialingList_.numberList.insert(item);
    }
    incomingList_.policyFlag = (EDMPolicyFlag)incomingPolicy;
    for (auto item : incomingList) {
        incomingList_.numberList.insert(item);
    }

    return TELEPHONY_ERR_SUCCESS;
}

bool EdmCallPolicy::IsCallEnable(const std::string &phoneNum, const EdmCallPolicyList &list)
{
    if (system::GetBoolParameter(PARAM_DISALLOWED_TELEPHONY_CALL, false)) {
        return false;
    }
    std::shared_lock<ffrt::shared_mutex> lock(rwMutex_);
    switch (list.policyFlag) {
        case EDMPolicyFlag::POLICY_FLAG_NONE:
            return true;
        case EDMPolicyFlag::POLICY_FLAG_BLOCK: {
               // 黑名单
            if (list.numberList.empty()) {
                return true;
            }
            auto iter = list.numberList.find(phoneNum);
            return iter == list.numberList.end();
        }
        case EDMPolicyFlag::POLICY_FLAG_TRUST: {
            // 白名单
            if (list.numberList.empty()) {
                return true;
            }
            auto iter = list.numberList.find(phoneNum);
            return iter != list.numberList.end();
        }
        default:
            break;
    }

    return true;
}

bool EdmCallPolicy::IsDialingEnable(const std::string &phoneNum)
{
    return IsCallEnable(phoneNum, dialingList_);
}

bool EdmCallPolicy::IsIncomingEnable(const std::string &phoneNum)
{
    return IsCallEnable(phoneNum, incomingList_);
}

} // namespace Telephony
} // namespace OHOS
