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
class CallStatusManager : public CallStatusPolicy {
public:
    CallStatusManager();
    ~CallStatusManager();
    int32_t Init();
    int32_t UnInit();
    int32_t HandleCallReportInfo(const CallDetailInfo &info);
    int32_t HandleCallsReportInfo(const CallDetailsInfo &info);
    int32_t HandleDisconnectedCause(int32_t cause);
    int32_t HandleEventResultReportInfo(const CellularCallEventInfo &info);
    int32_t HandleOttEventReportInfo(const OttCallEventInfo &info);
    void CallFilterCompleteResult(const CallDetailInfo &info);

private:
    void InitCallBaseEvent();
    int32_t IncomingHandle(const CallDetailInfo &info);
    int32_t DialingHandle(const CallDetailInfo &info);
    int32_t ActiveHandle(const CallDetailInfo &info);
    int32_t HoldingHandle(const CallDetailInfo &info);
    int32_t WaitingHandle(const CallDetailInfo &info);
    int32_t AlertHandle(const CallDetailInfo &info);
    int32_t DisconnectingHandle(const CallDetailInfo &info);
    int32_t DisconnectedHandle(const CallDetailInfo &info);
    sptr<CallBase> CreateNewCall(const CallDetailInfo &info, CallDirection dir);
    void PackParaInfo(
        DialParaInfo &paraInfo, const CallDetailInfo &info, CallDirection dir, AppExecFwk::PacMap &extras);
    int32_t UpdateCallState(sptr<CallBase> &call, TelCallState nextState);
    int32_t ToSpeakerPhone(sptr<CallBase> &call);
    int32_t TurnOffMute(sptr<CallBase> &call);
    int32_t IncomingFilterPolicy(const CallDetailInfo &info);

private:
    CallDetailInfo callReportInfo_;
    CallDetailsInfo callDetailsInfo_;
    sptr<CallIncomingFilterManager> CallIncomingFilterManagerPtr_;
    std::map<RequestResultEventId, CallAbilityEventId> mEventIdTransferMap_;
    std::map<OttCallEventId, CallAbilityEventId> mOttEventIdTransferMap_;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_STATUS_MANAGER_H
