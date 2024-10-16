/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "call_connect_ability.h"

#include "ability_manager_client.h"
#include "call_ability_connect_callback.h"
#include "call_number_utils.h"
#include "int_wrapper.h"
#include "string_wrapper.h"
#include "telephony_log_wrapper.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
std::condition_variable CallConnectAbility::cv_;

CallConnectAbility::~CallConnectAbility() {}

CallConnectAbility::CallConnectAbility() {}

void CallConnectAbility::ConnectAbility()
{
    SetConnectingFlag(true);
    if (isDisconnecting_) {
        TELEPHONY_LOGE("callui is disconnecting");
        return;
    }
    if (isConnected_) {
        TELEPHONY_LOGE("callui has already connected");
        SetConnectingFlag(false);
        return;
    }
    TELEPHONY_LOGW("Connect callui ability");
    AAFwk::Want want;
    AppExecFwk::ElementName element("", "com.ohos.callui", "com.ohos.callui.ServiceAbility");
    want.SetElement(element);
    if (connectCallback_ == nullptr) {
        connectCallback_ = new CallAbilityConnectCallback();
    }
    int32_t userId = -1;
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, connectCallback_, userId);
    IPCSkeleton::SetCallingIdentity(identity);
}

void CallConnectAbility::DisconnectAbility()
{
    SetDisconnectingFlag(true);
    if (isConnecting_) {
        TELEPHONY_LOGE("callui is connecting");
        return;
    }
    if (!WaitForConnectResult()) {
        SetDisconnectingFlag(false);
        return;
    }
    if (connectCallback_ != nullptr) {
        TELEPHONY_LOGW("Disconnect callui ability");
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(connectCallback_);
        IPCSkeleton::SetCallingIdentity(identity);
        connectCallback_ = nullptr;
    }
}

void CallConnectAbility::SetConnectFlag(bool isConnected)
{
    isConnected_ = isConnected;
}

bool CallConnectAbility::GetConnectFlag()
{
    return isConnected_;
}

void CallConnectAbility::SetDisconnectingFlag(bool isDisconnecting)
{
    isDisconnecting_ = isDisconnecting;
    if (!isDisconnecting_ && isConnecting_) {
        ConnectAbility();
        TELEPHONY_LOGE("reconnect ability");
    }
}

void CallConnectAbility::SetConnectingFlag(bool isConnecting)
{
    isConnecting_ = isConnecting;
    if (!isConnecting_ && isDisconnecting_) {
        DisconnectAbility();
        TELEPHONY_LOGE("redisconnect ability");
    }
}

void CallConnectAbility::NotifyAll()
{
    cv_.notify_all();
}

bool CallConnectAbility::WaitForConnectResult()
{
    if (!isConnected_) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!isConnected_) {
            if (cv_.wait_for(lock, std::chrono::seconds(WAIT_TIME_ONE_SECOND)) == std::cv_status::timeout) {
                TELEPHONY_LOGE("callui is not connected, no need to disconnect ability");
                return false;
            }
        }
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS
