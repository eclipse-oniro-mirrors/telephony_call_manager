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
#include "call_ability_connect_callback.h"
#include "ability_manager_client.h"
#include "want.h"
#include "string_wrapper.h"
#include "int_wrapper.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallConnectAbility::~CallConnectAbility()
{
    if (connectCallback_ != nullptr) {
        delete connectCallback_;
        connectCallback_ = nullptr;
    }
}

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
    wantParams.SetParam("accountNumber", AAFwk::String::Box(std::string(info.accountNumber)));
    wantParams.SetParam("videoState", AAFwk::Integer::Box(static_cast<int32_t>(info.videoState)));
    wantParams.SetParam("callType", AAFwk::Integer::Box(static_cast<int32_t>(info.callType)));
    wantParams.SetParam("callState", AAFwk::Integer::Box(static_cast<int32_t>(info.callState)));
    wantParams.SetParam("callId", AAFwk::Integer::Box(static_cast<int32_t>(info.callId)));
    wantParams.SetParam("startTime", AAFwk::Integer::Box(static_cast<int32_t>(info.startTime)));
    wantParams.SetParam("accountId", AAFwk::Integer::Box(static_cast<int32_t>(info.accountId)));
    wantParams.SetParam("isEcc", AAFwk::Integer::Box(static_cast<bool>(info.isEcc)));
    wantParams.SetParam("conferenceState", AAFwk::Integer::Box(static_cast<int32_t>(info.conferenceState)));
    want.SetParams(wantParams);
    if (connectCallback_ == nullptr) {
        connectCallback_ = new CallAbilityConnectCallback();
    }
    int32_t userId = 0;
    AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, connectCallback_, userId);
}

void CallConnectAbility::DisconnectAbility()
{
    if (!isConnected_) {
        TELEPHONY_LOGE("callui is not connected, no need to disconnect ability");
        return;
    }
    if (connectCallback_ != nullptr) {
        TELEPHONY_LOGI("Disconnect callui ability");
        AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(connectCallback_);
        connectCallback_ = nullptr;
    }
}

void CallConnectAbility::SetConnectFlag(bool isConnected)
{
    isConnected_ = isConnected;
}
} // namespace Telephony
} // namespace OHOS
