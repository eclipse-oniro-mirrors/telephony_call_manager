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

#ifndef TELEPHONY_CALL_RECORDS_MANAGER_H
#define TELEPHONY_CALL_RECORDS_MANAGER_H

#include <mutex>
#include <set>

#include "call_records_handler.h"
#include "call_state_listener_base.h"
#include "common_event_subscriber.h"
#include "iservice_registry.h"
#include "singleton.h"
#include "system_ability_definition.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
using CommonEventSubscribeInfo = OHOS::EventFwk::CommonEventSubscribeInfo;
using CommonEventSubscriber = OHOS::EventFwk::CommonEventSubscriber;

class UserSwitchEventSubscriber : public CommonEventSubscriber {
public:
    explicit UserSwitchEventSubscriber(const CommonEventSubscribeInfo &info) : CommonEventSubscriber(info) {}
    ~UserSwitchEventSubscriber() = default;
    void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data) override;
};

class DataShareReadyEventSubscriber : public CommonEventSubscriber {
public:
    explicit DataShareReadyEventSubscriber(const CommonEventSubscribeInfo &info) : CommonEventSubscriber(info) {}
    ~DataShareReadyEventSubscriber() = default;
    void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data) override;
};

class AccountSystemAbilityListener : public SystemAbilityStatusChangeStub {
public:
    AccountSystemAbilityListener() = default;
    ~AccountSystemAbilityListener() = default;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

private:
    std::shared_ptr<UserSwitchEventSubscriber> userSwitchSubscriber_ = nullptr;
};

/**
 * @class CallRecordsManager
 * Responsible for recording calls. Logging operations will be performed in a background thread
 * to avoid blocking the main thread.
 */
class CallRecordsManager : public CallStateListenerBase {
    DECLARE_DELAYED_SINGLETON(CallRecordsManager)
public:
    void Init();
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    void AddOneCallRecord(CallAttributeInfo &info);
    void CopyCallInfoToRecord(CallAttributeInfo &info, CallRecordInfo &data);
    void AddOneCallRecord(sptr<CallBase> call, CallAnswerType answerType);
    int32_t RemoveMissedIncomingCallNotification();
    int32_t GetCallFeatures(int32_t videoState);
    bool IsVideoCall(int32_t videoState);
    void SetDataShareReady(bool isDataShareReady);
    void SetSystemAbilityAdd(bool isSystemAbilityAdd);
    void QueryUnReadMissedCallLog(int32_t userId);

private:
    std::string GetCountryIso();
    int32_t CopyFormatNumberToRecord(std::string &countryIso, CallRecordInfo &data);
    int32_t CopyFormatNumberToE164ToRecord(std::string &countryIso, CallRecordInfo &data);
    void RegisterDataShareReadySubscriber();
    void GetNumberMarkSource(int32_t userId, char *source, unsigned int size);
    std::shared_ptr<CallRecordsHandlerService> callRecordsHandlerServerPtr_;
    std::mutex mutex_;
    sptr<ISystemAbilityStatusChange> statusChangeListener_ = nullptr;
    std::shared_ptr<DataShareReadyEventSubscriber> dataShareReadySubscriber_ = nullptr;
    bool isDataShareReady_ = false;
    bool isSystemAbilityAdd_ = false;
    bool isUnReadMissedCallLogQuery_ = false;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_CALL_RECORDS_MANAGER_H