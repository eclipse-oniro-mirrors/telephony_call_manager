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

#ifndef CALL_MANAGER_HISYSEVENT_H
#define CALL_MANAGER_HISYSEVENT_H

#include <string>

#include "telephony_hisysevent.h"

namespace OHOS {
namespace Telephony {
static const int64_t NORMAL_DIAL_TIME = 500;     // dial time (ms)
static const int64_t NORMAL_INCOMING_TIME = 100; // incoming time (ms)
static const int64_t NORMAL_ANSWER_TIME = 300;   // answer time (ms)
static constexpr const char DOMAIN_PHONE_UE[] = "PHONE_UE";
// phoneUE const
static constexpr const char *KEY_CALL_MANAGER = "callmanager";
static constexpr const char *CALL_DIAL_IN_SUPER_PRIVACY = "CALL_DIAL_IN_SUPER_PRIVACY";
static constexpr const char *CALL_ANSWER_IN_SUPER_PRIVACY = "CALL_ANSWER_IN_SUPER_PRIVACY";
static constexpr const char *CALL_REJECT_IN_SUPER_PRIVACY = "CALL_REJECT_IN_SUPER_PRIVACY";
static constexpr const char *CALL_DIAL_CLOSE_SUPER_PRIVACY = "CALL_DIAL_CLOSE_SUPER_PRIVACY";
static constexpr const char *CALL_INCOMING_REJECT_BY_SYSTEM = "CALL_INCOMING_REJECT_BY_SYSTEM";
// KEY
static constexpr const char *PNAMEID_KEY = "PNAMEID";
static constexpr const char *PVERSIONID_KEY = "PVERSIONID";
static constexpr const char *ACTION_TYPE = "ACTION_TYPE";
// VALUE
static constexpr const int64_t REJECT_BY_OOBE = 0;
static constexpr const int64_t REJECT_BY_NUM_BLOCK = 1;
static constexpr const int64_t REJECT_IN_FOCUSMODE = 2;

enum class IncomingCallType {
    IMS_VOICE_INCOMING = 0,
    IMS_VIDEO_INCOMING,
    CS_VOICE_INCOMING,
};

class CallManagerHisysevent : public TelephonyHiSysEvent {
public:
    static void WriteCallStateBehaviorEvent(const int32_t slotId, const int32_t state, const int32_t index);
    static void WriteIncomingCallBehaviorEvent(const int32_t slotId, int32_t callType, int32_t callMode);
    static void WriteIncomingNumIdentityBehaviorEvent(const int32_t markType, const bool isBlock,
        const int32_t blockReason);
    static void WriteIncomingCallFaultEvent(const int32_t slotId, const int32_t callType, const int32_t videoState,
        const int32_t errCode, const std::string &desc);
    static void WriteDialCallFaultEvent(const int32_t slotId, const int32_t callType, const int32_t videoState,
        const int32_t errCode, const std::string &desc);
    static void WriteAnswerCallFaultEvent(const int32_t slotId, const int32_t callId, const int32_t videoState,
        const int32_t errCode, const std::string &desc);
    static void WriteHangUpFaultEvent(
        const int32_t slotId, const int32_t callId, const int32_t errCode, const std::string &desc);
    void GetErrorDescription(const int32_t errCode, std::string &errordesc);
    void SetDialStartTime();
    void SetIncomingStartTime();
    void SetAnswerStartTime();
    void JudgingDialTimeOut(const int32_t slotId, const int32_t callType, const int32_t videoState);
    void JudgingIncomingTimeOut(const int32_t slotId, const int32_t callType, const int32_t videoState);
    void JudgingAnswerTimeOut(const int32_t slotId, const int32_t callId, const int32_t videoState);
    static void WriteVoipCallStatisticalEvent(const std::string &voipCallId, const std::string &bundleName,
        const int32_t &uid, const std::string &statisticalField);
    static void WriteVoipCallStatisticalEvent(const int32_t &callId, const std::string &statisticalField);
public:
    template<typename... Types>
    static void HiWriteBehaviorEventPhoneUE(const std::string &eventName, Types... args)
    {
        HiSysEventWrite(DOMAIN_PHONE_UE, eventName, EventType::BEHAVIOR, args...);
    }

private:
    static int32_t ErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue);
    static int32_t CallDataErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue);
    static int32_t CallInterfaceErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue);
    static int32_t TelephonyErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue);
    static int32_t GetAppIndexByBundleName(std::string bundleName, int32_t uid, int32_t &appIndex);

private:
    int64_t dialStartTime_ = 0;
    int64_t incomingStartTime_ = 0;
    int64_t answerStartTime_ = 0;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_MANAGER_HISYSEVENT_H
