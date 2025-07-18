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

#ifndef CALL_POLICY_H
#define CALL_POLICY_H

#include <string>
#include <memory>

#include "pac_map.h"

#include "call_object_manager.h"
#include "edm_call_policy.h"

/**
 * @ClassName: CallPolicy
 * @Description: check prerequisites before performing ops in callcontrolmanager
 */
namespace OHOS {
namespace Telephony {
const int32_t MCC_LEN = 3;
const std::string CHN_MCC = "460";
const std::string MC_MCC = "455";

class CallPolicy : public CallObjectManager {
public:
    CallPolicy();
    ~CallPolicy();

    int32_t DialPolicy(std::u16string &number, AppExecFwk::PacMap &extras, bool isEcc);
    int32_t AnswerCallPolicy(int32_t callId, int32_t videoState);
    int32_t RejectCallPolicy(int32_t callId);
    int32_t HoldCallPolicy(int32_t callId);
    int32_t UnHoldCallPolicy(int32_t callId);
    int32_t HangUpPolicy(int32_t callId);
    int32_t SwitchCallPolicy(int32_t callId);
    static int32_t VideoCallPolicy(int32_t callId);
    static int32_t StartRttPolicy(int32_t callId);
    static int32_t StopRttPolicy(int32_t callId);
    int32_t IsValidSlotId(int32_t slotId);
    int32_t EnableVoLtePolicy(int32_t slotId);
    int32_t DisableVoLtePolicy(int32_t slotId);
    int32_t IsVoLteEnabledPolicy(int32_t slotId);
    int32_t VoNRStatePolicy(int32_t slotId, int32_t state);
    int32_t GetCallWaitingPolicy(int32_t slotId);
    int32_t SetCallWaitingPolicy(int32_t slotId);
    int32_t GetCallRestrictionPolicy(int32_t slotId);
    int32_t SetCallRestrictionPolicy(int32_t slotId);
    int32_t GetCallTransferInfoPolicy(int32_t slotId);
    int32_t SetCallTransferInfoPolicy(int32_t slotId);
    int32_t SetCallPreferenceModePolicy(int32_t slotId);
    int32_t GetImsConfigPolicy(int32_t slotId);
    int32_t SetImsConfigPolicy(int32_t slotId);
    int32_t GetImsFeatureValuePolicy(int32_t slotId);
    int32_t SetImsFeatureValuePolicy(int32_t slotId);
    static int32_t InviteToConferencePolicy(int32_t callId, std::vector<std::string> &numberList);
    int32_t CloseUnFinishedUssdPolicy(int32_t slotId);
    bool IsSupportVideoCall(AppExecFwk::PacMap &extras);
    int32_t CanDialMulityCall(AppExecFwk::PacMap &extras, bool isEcc);
    int32_t IsValidCallType(CallType callType);
    int32_t IsVoiceCallValid(VideoStateType videoState);
    int32_t HasNormalCall(bool isEcc, int32_t slotId, CallType callType);
    int32_t GetAirplaneMode(bool &isAirplaneModeOn);
    int32_t SuperPrivacyMode(std::u16string &number, AppExecFwk::PacMap &extras, bool isEcc);
    int32_t SetEdmPolicy(bool isDialingTrustlist, const std::vector<std::string> &dialingList,
        bool isIncomingTrustlist, const std::vector<std::string> &incomingList);
    bool IsDialingEnable(const std::string &phoneNum);
    bool IsIncomingEnable(const std::string &phoneNum);

private:
    bool IsCtSimCardSwitchToChnOrMc(int32_t slotId);
    std::shared_ptr<EdmCallPolicy> edmCallPolicy_{nullptr};

private:
    uint16_t onlyTwoCall_ = 2;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_POLICY_H
