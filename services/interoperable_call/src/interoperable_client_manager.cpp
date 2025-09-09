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
    if (session_ != nullptr) {
        TELEPHONY_LOGI("disconnect session_");
        session_->Disconnect();
        session_.reset();
        session_ = nullptr;
    }
    std::unique_lock<ffrt::mutex> lock(mutex_);
    phoneNum_ = "";
}
 
void InteroperableClientManager::ConnectRemote(const std::string &networkId)
{
    if (session_ != nullptr) {
        return;
    }
    auto transMgr = DelayedSingleton<TransmissionManager>::GetInstance();
    session_ = transMgr->CreateClientSession(shared_from_this());
    if (session_ != nullptr) {
        TELEPHONY_LOGI("connect session_");
        session_->Connect(networkId, SOFTNET_SESSION_NAME, SOFTNET_SESSION_NAME, QOS_BW_BT);
    }
}

void InteroperableClientManager::OnConnected()
{
    TELEPHONY_LOGI("client on connected");
    std::string phoneNum = "";
    std::unique_lock<ffrt::mutex> lock(mutex_);
    phoneNum = phoneNum_;
    lock.unlock();

    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(phoneNum);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("client connect, get call failed");
        return;
    }
    TELEPHONY_LOGI("phoneOrWatchDial[%{public}d]", callPtr->GetPhoneOrWatchDial());
    if (callPtr->GetCallDirection() == CallDirection::CALL_DIRECTION_OUT &&
        callPtr->GetPhoneOrWatchDial() == static_cast<int32_t>(PhoneOrWatchDial::WATCH_DIAL)) {
        SendRequisiteDataToPeer(callPtr->GetAccountId(), callPtr->GetAccountNumber());
    } else {
        SendRequisiteDataQueryToPeer(phoneNum);
    }
}

void InteroperableClientManager::CallCreated(const sptr<CallBase> &call, const std::string &networkId)
{
    TELEPHONY_LOGI("client mgr call created");
    std::unique_lock<ffrt::mutex> lock(mutex_);
    phoneNum_ = call->GetAccountNumber();
    lock.unlock();
    ConnectRemote(networkId);
    if (call->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING ||
        call->GetTelCallState() == TelCallState::CALL_STATUS_WAITING) {
        SendRequisiteDataQueryToPeer(call->GetAccountNumber()); // multi-call
    }
}

void InteroperableClientManager::OnDeviceOffline(const std::string &networkId,
    const std::string &devName, uint16_t devType)
{
    TELEPHONY_LOGI("client offline, set false");
    SetIsSlotIdVisible(false);
}
} // namespace Telephony
} // namespace OHOS