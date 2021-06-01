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

#ifndef CELLULAR_CALL_IPC_INTERFACE_PROXY_H
#define CELLULAR_CALL_IPC_INTERFACE_PROXY_H
#include <mutex>

#include "if_system_ability_manager.h"
#include "refbase.h"
#include "rwlock.h"
#include "singleton.h"

#include "call_status_callback.h"
#include "cellular_call_interface.h"
#include "i_call_status_callback.h"
#include "timer.h"

namespace OHOS {
namespace TelephonyCallManager {
class CellularCallIpcInterfaceProxy : public Timer,
                                      public std::enable_shared_from_this<CellularCallIpcInterfaceProxy> {
    DECLARE_DELAYED_SINGLETON(CellularCallIpcInterfaceProxy)
public:
    void Init(int32_t systemAbilityId);
    void unInit();
    static void task();
    /**
     * Call management diale interface
     *
     * @param CallInfo, dial param.
     * @return Returns 0 on success, others on failure.
     */
    int Dial(const CellularCall::CellularCallInfo &callInfo);

    /**
     * End.
     *
     * @param CallInfo, End param.
     * @return Returns 0 on success, others on failure.
     */
    int End(const CellularCall::CellularCallInfo &callInfo);

    /**
     * Reject.
     *
     * @param CallInfo, Reject param.
     * @return Returns 0 on success, others on failure.
     */
    int Reject(const CellularCall::CellularCallInfo &callInfo);

    /**
     * Answer.
     *
     * @param CallInfo, Answer param.
     * @return Returns 0 on success, others on failure.
     */
    int Answer(const CellularCall::CellularCallInfo &callInfo);

    /**
     * @brief Is it an emergency call
     * @param string &phoneNum
     * @param slotId
     * @return Returns 0 on success, others on failure.
     */
    int IsUrgentCall(const std::string &phoneNum, int32_t slotId);

    /**
     * RegisterCallBack
     *
     * @return Returns 0 on success, others on failure.
     */
    int RegisterCallBack(const sptr<TelephonyCallManager::ICallStatusCallback> &callback);

private:
    int32_t ConnectService();
    int32_t RegisterCallBackFun();
    void DisconnectService();
    void ReConnectService();
    void OnDeath();
    void Clean();
    void NotifyDeath();

private:
    int32_t systemAbilityId_;
    sptr<ICallStatusCallback> cellularCallCallbackPtr_;
    sptr<CellularCall::CellularCallInterface> cellularCallInterfacePtr_;
    sptr<IRemoteObject::DeathRecipient> cellularCallRecipient_;
    bool connectState_;
    Utils::RWLock rwClientlock_;
    std::mutex mutex_;
};
} // namespace TelephonyCallManager
} // namespace OHOS

#endif