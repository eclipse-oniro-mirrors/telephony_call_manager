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

#ifndef CALL_MANAGER_SERVICE_H
#define CALL_MANAGER_SERVICE_H
#include <memory>

#include "iremote_stub.h"
#include "iservice_registry.h"
#include "singleton.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "call_state_report_proxy.h"
#include "call_manager_service_stub.h"
#include "call_control_manager.h"

namespace OHOS {
namespace TelephonyCallManager {
class CallManagerService : public SystemAbility,
                           public CallManagerServiceStub,
                           public std::enable_shared_from_this<CallManagerService> {
    DECLARE_DELAYED_SINGLETON(CallManagerService)
    DECLARE_SYSTEM_ABILITY(CallManagerService)
public:
    bool Init();
    void UnInit();

    void OnDump() override;
    void OnStart() override;
    void OnStop() override;
    /**
     * Call diale interface
     *
     * @param number[in], dial param.
     * @param extras[in], extras date.
     * @param callId[out], call id.
     * @return Returns callId when the value is greater than zero, others on failure.
     */
    int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras, int32_t &callId) override;

    /**
     * Answer call
     *
     * @param callId[in], call id
     * @param videoState[in], 0: audio, 1: video
     * @return Returns 0 on success, others on failure.
     */
    int32_t AcceptCall(int32_t callId, int32_t videoState) override;

    /**
     * Reject call
     *
     * @param callId[in], call id
     * @param isSendSms[in], Whether to enter the reason for rejection,true:yes false:no
     * @param content[in], The reason you reject the call
     * @return Returns 0 on success, others on failure.
     */
    int32_t RejectCall(int32_t callId, bool isSendSms, std::u16string content) override;

    /**
     * Disconnect call
     *
     * @param callId[in], call id
     * @return Returns 0 on success, others on failure.
     */
    int32_t HangUpCall(int32_t callId) override;

    /**
     * app get call state
     *
     * @return Returns call state.
     */
    int32_t GetCallState() override;

private:
    enum ServiceRunningState { STATE_STOPPED, STATE_RUNNING };

    ServiceRunningState state_ {ServiceRunningState::STATE_STOPPED};

    std::shared_ptr<CallControlManager> telephonyCallsManagerPtr_;
    std::mutex lock_;
};
} // namespace TelephonyCallManager
} // namespace OHOS
#endif // CALL_MANAGER_SERVICE_H
