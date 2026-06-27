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

#include "call_manager_service.h"

#include "call_object_manager.h"
#include "call_manager_errors.h"
#include "ipc_skeleton.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include <openssl/rand.h>

namespace OHOS {
namespace Telephony {
using namespace Security::AccessToken;
static constexpr const char *OHOS_PERMISSION_GET_TELEPHONY_STATE = "ohos.permission.GET_TELEPHONY_STATE";
static constexpr int32_t TOKEN_BYTES_LENGTH = 32;
static constexpr int32_t HEX_STRING_RATIO = 2;

bool CallManagerService::TryUpdateChallengeTokenList(const std::string &phoneNumber, const ChallengeToken &challenge)
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
 
std::optional<CallManagerService::ChallengeToken> CallManagerService::PopChallengeTokenByPhone(
    const std::string &phoneNumber)
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
 
void CallManagerService::CleanupExpiredChallengeTokensUnsafe()
{
    auto now = std::chrono::steady_clock::now();
    challengeTokenList_.remove_if([&now](const ChallengeToken &tc) {
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - tc.createTime);
        return duration.count() >= CHALLENGE_TOKEN_EXPIRE_MINUTES;
    });
}
 
void CallManagerService::FillExtrasFromChallengeToken(const std::string &phoneNumber, AppExecFwk::PacMap &extras)
{
    auto challenge = PopChallengeTokenByPhone(phoneNumber);
    if (challenge.has_value()) {
        extras.PutBooleanValue("isCustomAccessibility", challenge->isCustomAccessibility);
        extras.PutStringValue("token", challenge->token);
    }
}

std::string CallManagerService::GenerateToken()
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
 
int32_t CallManagerService::MakeCallWithToken(std::string number, AppExecFwk::PacMap &options, std::string &token)
{
    std::string hexToken = GenerateToken();
    if (hexToken.empty()) {
        TELEPHONY_LOGE("Fail to generate token");
        return TELEPHONY_ERR_UNINIT;
    }
    bool isCustomAccessibility = options.GetBooleanValue("isCustomAccessibility", false);
    if (number.empty() || number.length() > ACCOUNT_NUMBER_MAX_LENGTH) {
        TELEPHONY_LOGE("MakeCallWithToken number is invalid");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    int32_t uid = IPCSkeleton::GetCallingUid();
    ChallengeToken challenge;
    challenge.token = hexToken;
    challenge.phoneNumber = number;
    challenge.uid = uid;
    challenge.isCustomAccessibility = isCustomAccessibility;
    challenge.createTime = std::chrono::steady_clock::now();
    if (!TryUpdateChallengeTokenList(number, challenge)) {
        TELEPHONY_LOGE("Fail to store token, token list is full");
        return TELEPHONY_ERR_UNINIT;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    AAFwk::Want want;
    AppExecFwk::ElementName element("", "com.ohos.contacts", "com.ohos.contacts.MainAbility");
    want.SetElement(element);
    AAFwk::WantParams wantParams;
    wantParams.SetParam("phoneNumber", AAFwk::String::Box(number));
    wantParams.SetParam("pageFlag", AAFwk::String::Box("page_flag_edit_before_calling"));
    want.SetParams(wantParams);
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    IPCSkeleton::SetCallingIdentity(identity);
    if (err != ERR_OK) {
        TELEPHONY_LOGE("Fail to make call with token, err:%{public}d", err);
        PopChallengeTokenByPhone(number);
        return TELEPHONY_ERR_UNINIT;
    }
    TELEPHONY_LOGI("Stored hex token length: %{public}zu", hexToken.length());
    token = hexToken;
    return TELEPHONY_SUCCESS;
}

bool CallManagerService::CheckCallRecordingPermission(const std::string& cellularRecordPhoneNum,
    const std::string& cellularRecordToken)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return false;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return false;
    }
    std::string phoneNumber = cellularRecordPhoneNum;
    sptr<CallBase> call = CallObjectManager::GetOneCallObject(phoneNumber);
    if (call == nullptr) {
        TELEPHONY_LOGE("Call not found phoneNumber");
        return false;
    }
    
    TelCallState state = call->GetTelCallState();
    if (state != TelCallState::CALL_STATUS_ACTIVE) {
        TELEPHONY_LOGE("Call is not active, state: %{public}d", state);
        return false;
    }
    
    std::string callToken = call->GetToken();
    if (callToken.empty()) {
        TELEPHONY_LOGE("Token is empty for phoneNumber");
        return false;
    }
    
    if (callToken != cellularRecordToken) {
        TELEPHONY_LOGE("Token mismatch");
        return false;
    }
 
    TELEPHONY_LOGI("Call recording permission check passed");
    return true;
}
} // namespace Telephony
} // namespace OHOS
