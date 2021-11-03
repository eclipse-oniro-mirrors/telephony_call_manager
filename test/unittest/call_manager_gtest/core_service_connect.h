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

#ifndef CORE_SERVICE_CONNECT_H
#define CORE_SERVICE_CONNECT_H

#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"

#include "refbase.h"
#include "rwlock.h"

#include "core_service_proxy.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t DEFAULT_SLOT_ID = 0;

class CoreServiceConnect {
public:
    CoreServiceConnect()
    {
        coreServicePtr_ = nullptr;
        systemAbilityId_ = TELEPHONY_CORE_SERVICE_SYS_ABILITY_ID;
    }

    ~CoreServiceConnect()
    {
        if (coreServicePtr_) {
            coreServicePtr_.clear();
            coreServicePtr_ = nullptr;
        }
    }

    int32_t Init(int32_t systemAbilityId)
    {
        systemAbilityId_ = systemAbilityId;
        return ConnectService();
    }

    void UnInit()
    {
        DisconnectService();
    }

    bool HasSimCard()
    {
        if (coreServicePtr_) {
            return coreServicePtr_->HasSimCard(DEFAULT_SLOT_ID);
        }
        return false;
    }

private:
    int32_t systemAbilityId_;
    Utils::RWLock rwClientLock_;
    sptr<ICoreService> coreServicePtr_;

    int32_t ConnectService()
    {
        Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
        if (coreServicePtr_ != nullptr) {
            return TELEPHONY_SUCCESS;
        }
        sptr<ISystemAbilityManager> managerPtr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (managerPtr == nullptr) {
            return TELEPHONY_FAIL;
        }
        sptr<ICoreService> coreServicePtr = nullptr;
        sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(systemAbilityId_);
        if (iRemoteObjectPtr == nullptr) {
            return TELEPHONY_CONNECT_SYSTEM_ABILITY_STUB_FAIL;
        }
        coreServicePtr = iface_cast<ICoreService>(iRemoteObjectPtr);
        if (!coreServicePtr) {
            return TELEPHONY_LOCAL_PTR_NULL;
        }
        coreServicePtr_ = coreServicePtr;
        return TELEPHONY_SUCCESS;
    }

    void DisconnectService()
    {
        Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
        if (coreServicePtr_) {
            coreServicePtr_.clear();
            coreServicePtr_ = nullptr;
        }
    }
};
} // namespace Telephony
} // namespace OHOS
#endif