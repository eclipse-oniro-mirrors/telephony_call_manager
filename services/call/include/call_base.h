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

#ifndef CALL_BASE_H
#define CALL_BASE_H
#include <unistd.h>
#include <cstring>
#include <memory>
#include <mutex>

#include "refbase.h"

#include "common_type.h"

namespace OHOS {
namespace TelephonyCallManager {
constexpr int32_t CAPABILITY_HOLD = 0x00000001;
class CallBase : public virtual RefBase {
public:
    CallBase(const CallInfo &info);
    virtual ~CallBase();

    int32_t Init();

    int32_t DialCallBase();
    int32_t IncomingCallBase();
    int32_t AccpetCallBase();
    int32_t RejectCallBase();
    int32_t HoldCallBase();
    int32_t UnHoldCallBase();
    int32_t HangUpBase();
    int32_t TurnOffVoice(bool silence);
    int32_t GetCallID();
    int32_t SetCallID(int32_t callId);
    CallStateType GetState();
    void SetState(TelCallStates nextState);
    bool GetBaseCallInfo(CallInfo &info);
    int32_t SetCallInfo(const CallInfo &callInfo);
    bool GetCallerInfo(ContactInfo &info);
    void SetCallerInfo(const ContactInfo &contactInfo);
    CallEndedType GetCallEndedType();
    int32_t SetCallEndedType(CallEndedType callEndedType);
    bool IsSpeakerphoneEnabled();
    bool GetEmergencyState();
    bool IsCurrentRinging();
    std::string GetPhoneNumber();
    int32_t SetSpeakerphoneOn(bool speakerphoneOn);
    bool IsSpeakerphoneOn();
    void SetAudio();
    bool CheckVoicemailNumber(std::string phoneNumber);

protected:
    CallStateType callState_;

private:
    CallInfo callInfo_;
    bool isSpeakerphoneOn_;
    CallEndedType callEndedType_;
    ContactInfo contactInfo_;
    std::mutex mutex_;
};
} // namespace TelephonyCallManager
} // namespace OHOS

#endif // CALL_BASE_H