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

#ifndef TELEPHONY_CALL_EDM_CALL_POLICY_H
#define TELEPHONY_CALL_EDM_CALL_POLICY_H

#include <unordered_set>
#include "ffrt.h"

namespace OHOS {
namespace Telephony {

enum EdmPolicyFlag : int32_t {
    POLICY_FLAG_NONE = -1,  // 无策略
    POLICY_FLAG_BLOCK = 0,  // 黑名单
    POLICY_FLAG_TRUST = 1,  // 白名单
};

struct EdmCallPolicyList {
    EdmPolicyFlag policyFlag { EdmPolicyFlag::POLICY_FLAG_NONE };
    std::unordered_set<std::string> numberList {};
};

class EdmCallPolicy {
public:
    EdmCallPolicy();
    ~EdmCallPolicy();
    int32_t SetCallPolicy(bool isDialingTrustlist, const std::vector<std::string> &dialingList,
        bool isIncomingTrustlist, const std::vector<std::string> &incomingList);
    bool IsDialingEnable(const std::string &phoneNum);
    bool IsIncomingEnable(const std::string &phoneNum);

private:
    bool IsCallEnable(const std::string &phoneNum, const EdmCallPolicyList &list);
    ffrt::shared_mutex rwMutex_{};
    EdmCallPolicyList dialingList_{};
    EdmCallPolicyList incomingList_{};
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_CALL_EDM_CALL_POLICY_H