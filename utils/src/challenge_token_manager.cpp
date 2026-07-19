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

#include "challenge_token_manager.h"

#include <openssl/rand.h>

#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

std::list<ChallengeToken> ChallengeTokenManager::challengeTokenList_;
ffrt::mutex ChallengeTokenManager::tokenMutex_;

bool ChallengeTokenManager::TryUpdateChallengeTokenList(
    const std::string &phoneNumber, const ChallengeToken &challenge)
{
    std::lock_guard<ffrt::mutex> lock(tokenMutex_);
    CleanupExpiredChallengeTokensUnsafe();
    auto it = std::find_if(challengeTokenList_.begin(), challengeTokenList_.end(),
        [&challenge](const ChallengeToken &tc) { return tc.uid == challenge.uid; });
    if (it != challengeTokenList_.end()) {
        it->token = challenge.token;
        it->phoneNumber = phoneNumber;
        it->isCustomAccessibility = challenge.isCustomAccessibility;
        it->createTime = std::chrono::steady_clock::now();
        return true;
    }

    if (challengeTokenList_.size() >= MAX_CHALLENGE_TOKEN_COUNT) {
        TELEPHONY_LOGW("Token challenge list is full, reject new token for uid %{public}d", challenge.uid);
        return false;
    }

    challengeTokenList_.push_back(challenge);
    return true;
}

std::optional<ChallengeToken> ChallengeTokenManager::PopChallengeTokenByPhone(const std::string &phoneNumber)
{
    std::lock_guard<ffrt::mutex> lock(tokenMutex_);
    CleanupExpiredChallengeTokensUnsafe();
    auto it = std::find_if(challengeTokenList_.begin(), challengeTokenList_.end(),
        [&phoneNumber](const ChallengeToken &tc) { return tc.phoneNumber == phoneNumber; });
    if (it != challengeTokenList_.end()) {
        ChallengeToken result = *it;
        challengeTokenList_.erase(it);
        return result;
    }
    return std::nullopt;
}

void ChallengeTokenManager::CleanupExpiredChallengeTokensUnsafe()
{
    auto now = std::chrono::steady_clock::now();
    challengeTokenList_.remove_if([&now](const ChallengeToken &tc) {
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - tc.createTime);
        return duration.count() >= CHALLENGE_TOKEN_EXPIRE_MINUTES;
    });
}

void ChallengeTokenManager::FillExtrasFromChallengeToken(
    const std::string &phoneNumber, AppExecFwk::PacMap &extras)
{
    auto challenge = PopChallengeTokenByPhone(phoneNumber);
    if (challenge.has_value()) {
        extras.PutBooleanValue("isCustomAccessibility", challenge->isCustomAccessibility);
        extras.PutStringValue("token", challenge->token);
    }
}

std::string ChallengeTokenManager::GenerateToken()
{
    unsigned char nonce[TOKEN_BYTES_LENGTH];
    if (RAND_bytes(nonce, sizeof(nonce)) != 1) {
        return "";
    }
    std::string token;
    token.reserve(sizeof(nonce) * HEX_STRING_RATIO);
    for (int32_t i = 0; i < TOKEN_BYTES_LENGTH; ++i) {
        char hex[3];
        if (snprintf_s(hex, sizeof(hex), sizeof(hex), "%02x", nonce[i]) < 0) {
            return "";
        }
        token += hex;
    }
    return token;
}
} // namespace Telephony
} // namespace OHOS
