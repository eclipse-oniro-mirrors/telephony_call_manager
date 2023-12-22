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

void CallConnectAbility::ConnectAbility(const CallAttributeInfo &info)
{
    if (isConnected_) {
        TELEPHONY_LOGE("callui has already connected");
        return;
    }
    TELEPHONY_LOGI("Connect callui ability");
    AAFwk::Want want;
    AppExecFwk::ElementName element("", "com.ohos.callui", "com.ohos.callui.ServiceAbility");
    want.SetElement(element);
    AAFwk::WantParams wantParams;
    std::string accountNumber =
        DelayedSingleton<CallNumberUtils>::GetInstance()->RemovePostDialPhoneNumber(std::string(info.accountNumber));
    wantParams.SetParam("accountNumber", AAFwk::String::Box(accountNumber));
    wantParams.SetParam("videoState", AAFwk::Integer::Box(static_cast<int32_t>(info.videoState)));
    wantParams.SetParam("callType", AAFwk::Integer::Box(static_cast<int32_t>(info.callType)));
    wantParams.SetParam("callState", AAFwk::Integer::Box(static_cast<int32_t>(info.callState)));
    wantParams.SetParam("callId", AAFwk::Integer::Box(static_cast<int32_t>(info.callId)));
    wantParams.SetParam("startTime", AAFwk::Integer::Box(static_cast<int32_t>(info.startTime)));
    wantParams.SetParam("accountId", AAFwk::Integer::Box(static_cast<int32_t>(info.accountId)));
    wantParams.SetParam("isEcc", AAFwk::Integer::Box(static_cast<bool>(info.isEcc)));
    wantParams.SetParam("conferenceState", AAFwk::Integer::Box(static_cast<int32_t>(info.conferenceState)));
    wantParams.SetParam("crsType", AAFwk::Integer::Box(static_cast<int32_t>(info.crsType)));
    wantParams.SetParam("originalCallType", AAFwk::Integer::Box(static_cast<int32_t>(info.originalCallType)));
    if (info.callType == CallType::TYPE_VOIP) {
        TELEPHONY_LOGI("Connect callui ability voip call");
        wantParams.SetParam("userName", AAFwk::String::Box(std::string(info.voipCallInfo.userName)));
        wantParams.SetParam("extensionId", AAFwk::String::Box(std::string(info.voipCallInfo.extensionId)));
        wantParams.SetParam("voipBundleName", AAFwk::String::Box(std::string(info.voipCallInfo.voipBundleName)));
        wantParams.SetParam("abilityName", AAFwk::String::Box(std::string(info.voipCallInfo.abilityName)));
        wantParams.SetParam("voipCallId", AAFwk::String::Box(std::string(info.voipCallInfo.voipCallId)));
    }
    want.SetParams(wantParams);
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
    if (!WaitForConnectResult()) {
        return;
    }
    if (connectCallback_ != nullptr) {
        TELEPHONY_LOGI("Disconnect callui ability");
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
