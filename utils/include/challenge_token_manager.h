/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef CHALLENGE_TOKEN_MANAGER_H
#define CHALLENGE_TOKEN_MANAGER_H

#include <chrono>
#include <list>
#include <optional>
#include <string>

#include "ffrt.h"
#include "pac_map.h"

namespace OHOS {
namespace Telephony {
struct ChallengeToken {
    std::string token;
    std::string phoneNumber;
    int32_t uid;
    bool isCustomAccessibility;
    std::chrono::steady_clock::time_point createTime;
};

class ChallengeTokenManager {
public:
    ChallengeTokenManager() = default;
    ~ChallengeTokenManager() = default;

    std::string GenerateToken();
    bool TryUpdateChallengeTokenList(const std::string &phoneNumber, const ChallengeToken &challenge);
    std::optional<ChallengeToken> PopChallengeTokenByPhone(const std::string &phoneNumber);
    void CleanupExpiredChallengeTokensUnsafe();
    void FillExtrasFromChallengeToken(const std::string &phoneNumber, AppExecFwk::PacMap &extras);

private:
    static constexpr size_t MAX_CHALLENGE_TOKEN_COUNT = 3;
    static constexpr int32_t CHALLENGE_TOKEN_EXPIRE_MINUTES = 10;
    static constexpr int32_t TOKEN_BYTES_LENGTH = 32;
    static constexpr int32_t HEX_STRING_RATIO = 2;

    static std::list<ChallengeToken> challengeTokenList_;
    static ffrt::mutex tokenMutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CHALLENGE_TOKEN_MANAGER_H
