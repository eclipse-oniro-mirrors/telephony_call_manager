/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef CALL_SUPERPRIVACY_CONTROL_MANAGER_H
#define CALL_SUPERPRIVACY_CONTROL_MANAGER_H

#include "ffrt.h"
#include "singleton.h"
#include "call_manager_base.h"
#include "super_privacy_policy_info.h"

/**
 * @ClassName: CallSuperPrivacyControlManager
 * @Description: check super privacy mode in CallSuperPrivacyControlManager
 */
namespace OHOS {
namespace Telephony {
class CallSuperPrivacyControlManager {
public:
    static const int SUPER_PRIVACY_MODE_REQUEST_SUCCESS = 0;
    void RegisterSuperPrivacyPolicy();
    void RecordChangedPrivacyPolicy();
    void RestorePrivacyPolicy();
    void GetCurrentPrivacyPolicy(SuperPrivacyPolicyInfo &policyInfo);
    void CloseAllCall();
    void CloseCallAccordingPolicy(const SuperPrivacyPolicyInfo &policyInfo);

    bool CanCallWithSuperPrivacyPolicy();
    bool CanCallWithSuperPrivacyPolicy(const SuperPrivacyPolicyInfo &policyInfo);
    int32_t CloseSuperPrivacyMode();
    void CloseCallSuperPrivacyMode(std::u16string &phoneNumber, int32_t &accountId,
        int32_t &videoState, int32_t &dialType, int32_t &dialScene, int32_t &callType);
    void CloseAnswerSuperPrivacyMode(int32_t callId, int32_t videoState);

private:
    int32_t SetSuperPrivacyPolicy(const SuperPrivacyPolicyInfo &policyInfo, int32_t source);
    void ParsePolicyFromParam(SuperPrivacyPolicyInfo &policyInfo, int32_t valueNum);
    static void ParamChangeCallback(const char *key, const char *value, void *context);
    static void SuperPrivacyModeChangeEvent();

    void SetSuperPrivacyChanged(bool flag);
    bool IsSuperPrivacyChanged();
    SuperPrivacyMode GetOldSuperPrivacyMode();
    void SetOldSuperPrivacyMode(SuperPrivacyMode);

    ffrt::shared_mutex superPrivacyModeMutex_;
    SuperPrivacyMode oldSuperPrivacyMode_ = SuperPrivacyMode::OFF;
    bool isSuperPrivacyModeChanged_ = false;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_POLICY_H
