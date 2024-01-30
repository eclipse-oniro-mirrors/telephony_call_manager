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

#ifndef CALL_STATUS_MANAGER_H
#define CALL_STATUS_MANAGER_H

#include <map>

#include "refbase.h"

#include "common_type.h"
#include "call_status_policy.h"
#include "call_incoming_filter_manager.h"

/**
 * Singleton
 * @ClassName:CallStatusManager
 * @Description:CallStatusManager is designed for watching call state changes,
 * when call state changes, cellularcall will notify callstatusmanager to handle it.
 * call state: idle,disconnected, disconnecting,dialing,alerting,active,holding,waiting
 * incoming
 */
namespace OHOS {
namespace Telephony {
const int32_t SLOT_NUM = 2;

class CallStatusManager : public CallStatusPolicy {
public:
    CallStatusManager();
    ~CallStatusManager();
    int32_t Init();
    int32_t UnInit();
    int32_t HandleCallReportInfo(const CallDetailInfo &info);
    void HandleDsdaInfo(int32_t slotId);
    int32_t HandleCallsReportInfo(const CallDetailsInfo &info);
    int32_t HandleDisconnectedCause(const DisconnectedDetails &details);
    int32_t HandleEventResultReportInfo(const CellularCallEventInfo &info);
    int32_t HandleOttEventReportInfo(const OttCallEventInfo &info);
    int32_t HandleVoipCallReportInfo(const CallDetailInfo &info);
    void CallFilterCompleteResult(const CallDetailInfo &info);

private:
    void InitCallBaseEvent();
    int32_t IncomingHandle(const CallDetailInfo &info);
    int32_t IncomingVoipCallHandle(const CallDetailInfo &info);
    int32_t DialingHandle(const CallDetailInfo &info);
    int32_t ActiveHandle(const CallDetailInfo &info);
    int32_t ActiveVoipCallHandle(const CallDetailInfo &info);
    int32_t HoldingHandle(const CallDetailInfo &info);
    int32_t WaitingHandle(const CallDetailInfo &info);
    int32_t AlertHandle(const CallDetailInfo &info);
    int32_t DisconnectingHandle(const CallDetailInfo &info);
    int32_t DisconnectedVoipCallHandle(const CallDetailInfo &info);
    int32_t DisconnectedHandle(const CallDetailInfo &info);
    void IsCanUnHold(int32_t activeCallNum, int32_t waitingCallNum, int32_t size, bool &canUnHold);
    void AutoAnswer(int32_t activeCallNum, int32_t waitingCallNum);
    void AutoHandleForDsda(
        bool canSwitchCallState, TelCallState priorState, int32_t activeCallNum, int32_t slotId, bool continueAnswer);
    void AutoAnswerForVoiceCall(sptr<CallBase> ringCall, int32_t slotId, bool continueAnswer);
    void AutoAnswerForVideoCall(int32_t activeCallNum);
    void AutoUnHoldForDsda(bool canSwitchCallState, TelCallState priorState, int32_t activeCallNum, int32_t slotId);
    sptr<CallBase> CreateNewCall(const CallDetailInfo &info, CallDirection dir);
    sptr<CallBase> CreateNewCallByCallType(
        DialParaInfo &paraInfo, const CallDetailInfo &info, CallDirection dir, AppExecFwk::PacMap &extras);
    sptr<CallBase> RefreshCallIfNecessary(const sptr<CallBase> &call, const CallDetailInfo &info);
    void SetOriginalCallTypeForActiveState(sptr<CallBase> &call);
    void SetOriginalCallTypeForDisconnectState(sptr<CallBase> &call);
    void PackParaInfo(
        DialParaInfo &paraInfo, const CallDetailInfo &info, CallDirection dir, AppExecFwk::PacMap &extras);
    int32_t UpdateCallState(sptr<CallBase> &call, TelCallState nextState);
    int32_t ToSpeakerPhone(sptr<CallBase> &call);
    int32_t TurnOffMute(sptr<CallBase> &call);
    int32_t IncomingFilterPolicy(const CallDetailInfo &info);
    void QueryCallerInfo(ContactInfo &contactInfo, std::string phoneNum);
    int32_t UpdateDialingCallInfo(const CallDetailInfo &info);
    void BuildAndQueryCallerInfo(sptr<CallBase> &call, const CallDetailInfo &info);

private:
    CallDetailInfo callReportInfo_;
    CallDetailsInfo callDetailsInfo_[SLOT_NUM];
    sptr<CallIncomingFilterManager> CallIncomingFilterManagerPtr_;
    std::map<RequestResultEventId, CallAbilityEventId> mEventIdTransferMap_;
    std::map<OttCallEventId, CallAbilityEventId> mOttEventIdTransferMap_;
    const std::string DETAIL_INFO = "detail_info";
    const std::string CONTENT_TYPE = "content_type";
    const std::string PHONE = "phone";
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_STATUS_MANAGER_H
