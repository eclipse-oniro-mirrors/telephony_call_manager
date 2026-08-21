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
#include "call_object_manager.h"
#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
constexpr const char* SOFTNET_SESSION_NAME = "ohos.telephony.callmanager.interoperable_communication";
 
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
        session_->Create(SOFTNET_SESSION_NAME, QOS_BW_BT);
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

void InteroperableServerManager::HandleSpecificMsg(int32_t msgType, const cJSON *msg)
{
    if (msg == nullptr) {
        return;
    }
    switch (msgType) {
        case static_cast<int32_t>(InteroperableMsgType::DATA_TYPE_QUERY_REQUISITES_DATA):
            OnQueryRequisitesDataMsgReceived(msg);
            break;
        default:
            break;
    }
}

void InteroperableServerManager::OnQueryRequisitesDataMsgReceived(const cJSON *msg)
{
    std::string phoneNum = "";
    sptr<CallBase> callPtr = nullptr;
    if (!GetStringValue(msg, INTEROPERABLE_ITEM_PHONE_NUMBER, phoneNum)) {
        TELEPHONY_LOGE("bt call remote query dara, parse data failed");
        return;
    }

    callPtr = CallObjectManager::GetOneCallObject(phoneNum);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("query call failed!");
        return;
    }
    SendRequisiteDataToPeer(callPtr->GetAccountId(), callPtr->GetAccountNumber());
}

void InteroperableServerManager::OnCallDestroyed()
{
    ClearBtSlotId();
}

void InteroperableServerManager::CallCreated(const sptr<CallBase> &call, const std::string &networkId)
{
    TELEPHONY_LOGI("server manager call create");
    if (call != nullptr) {
        SendRequisiteDataToPeer(call->GetAccountId(), call->GetAccountNumber());
    }
}
}
}