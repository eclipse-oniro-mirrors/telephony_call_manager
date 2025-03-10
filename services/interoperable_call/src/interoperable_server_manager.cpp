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

#include "interoperable_server_manager.h"
#include "interoperable_device_observer.h"
#include "telephony_log_wrapper.h"
#include "transmission_manager.h"

namespace OHOS {
namespace Telephony {
constexpr const char* SOFTNET_SESSION_NAME = "ohos.telephony.callmanager.distributed_communication";
 
void InteroperableServerManager::OnDeviceOnline(const std::string &networkId, const std::string &devName,
                                                uint16_t devType)
{
    TELEPHONY_LOGI("server session manager OnDeviceOnline");
    if (session_ != nullptr) {
        return;
    }
    auto transMgr = DelayedSingleton<TransmissionManager>::GetInstance();
    session_ = transMgr->CreateServerSession(shared_from_this());
    if (session_ != nullptr) {
        session_->Create(SOFTNET_SESSION_NAME);
    }
}
 
void InteroperableServerManager::OnDeviceOffline(const std::string &networkId, const std::string &devName,
                                                 uint16_t devType)
{
    TELEPHONY_LOGI("server session manager OnDeviceOffline");
    if (session_ == nullptr) {
        return;
    }
    session_->Destroy();
    session_.reset();
    session_ = nullptr;
}
}
}