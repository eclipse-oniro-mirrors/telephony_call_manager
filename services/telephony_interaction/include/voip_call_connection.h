/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef VOIP_CALL_CONNECTION_H
#define VOIP_CALL_CONNECTION_H

#include <mutex>

#include "call_status_callback.h"
#include "i_call_status_callback.h"
#include "i_voip_call_manager_callback.h"
#include "i_voip_call_manager_service.h"
#include "if_system_ability_manager.h"
#include "refbase.h"
#include "rwlock.h"
#include "singleton.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace Telephony {
class VoipCallConnection : public std::enable_shared_from_this<VoipCallConnection> {
    DECLARE_DELAYED_SINGLETON(VoipCallConnection)

public:
    void Init(int32_t systemAbilityId);
    void UnInit();
    int32_t GetCallManagerProxy();
    int32_t ReportVoipCallEventChange(const VoipCallEvents &events);

    int32_t RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback);

    int32_t UnRegisterCallManagerCallBack();

private:
#ifndef TELEPHONY_VOIP_CALL_MANAGER_SYS_ABILITY_ID
#define TELEPHONY_VOIP_CALL_MANAGER_SYS_ABILITY_ID 65968
#endif
    int32_t systemAbilityId_ = TELEPHONY_VOIP_CALL_MANAGER_SYS_ABILITY_ID;
    sptr<ICallStatusCallback> voipCallCallbackPtr_;
    sptr<IVoipCallManagerService> voipCallManagerInterfacePtr_ = nullptr;
    bool connectCallManagerState_ = false;
    Utils::RWLock rwClientLock_;
};
} // namespace Telephony
} // namespace OHOS

#endif
