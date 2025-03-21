/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#include "interoperable_client_manager.h"
#include "telephony_log_wrapper.h"
#include "call_object_manager.h"
#include "transmission_manager.h"

namespace OHOS {
namespace Telephony {
constexpr const char* SOFTNET_SESSION_NAME = "ohos.telephony.callmanager.interoperable_communication";
 
void InteroperableClientManager::OnCallCreated(const sptr<CallBase> &call, const std::string &networkId)
{
    ConnectRemote(networkId);
}
 
void InteroperableClientManager::OnCallDestroyed()
{
    if (CallObjectManager::HasCallExist()) {
        return;
    }
    if (session_ != nullptr) {
        session_->Disconnect();
        session_.reset();
        session_ = nullptr;
    }
}
 
void InteroperableClientManager::ConnectRemote(const std::string &networkId)
{
    if (session_ != nullptr) {
        return;
    }
    auto transMgr = DelayedSingleton<TransmissionManager>::GetInstance();
    session_ = transMgr->CreateClientSession(shared_from_this());
    if (session_ != nullptr) {
        session_->Connect(networkId, SOFTNET_SESSION_NAME, SOFTNET_SESSION_NAME);
    }
}
} // namespace Telephony
} // namespace OHOS