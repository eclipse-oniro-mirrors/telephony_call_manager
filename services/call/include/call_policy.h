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

#ifndef CALL_POLICY_H
#define CALL_POLICY_H

#include <string>
#include <memory>

#include "pac_map.h"

#include "call_object_manager.h"

/**
 * @ClassName: CallPolicy
 * @Description: check prerequisites before performing ops in callcontrolmanager
 */
namespace OHOS {
namespace Telephony {
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
    static int32_t UpdateCallMediaModePolicy(int32_t callId, ImsCallMode mode);
    static int32_t StartRttPolicy(int32_t callId);
    static int32_t StopRttPolicy(int32_t callId);
    int32_t IsValidSlotId(int32_t slotId);
    int32_t EnableVoLtePolicy(int32_t slotId);
    int32_t DisableVoLtePolicy(int32_t slotId);
    int32_t IsVoLteEnabledPolicy(int32_t slotId);
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
    int32_t GetLteEnhanceModePolicy(int32_t slotId);
    int32_t SetLteEnhanceModePolicy(int32_t slotId);
    static int32_t InviteToConferencePolicy(int32_t callId, std::vector<std::string> &numberList);

private:
    uint16_t onlyTwoCall_ = 2;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_POLICY_H
