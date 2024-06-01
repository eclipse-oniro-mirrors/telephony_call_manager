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

#include "call_broadcast_subscriber.h"

#include <string_ex.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "call_control_manager.h"
#include "satellite_call_control.h"
#include "securec.h"


namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
CallBroadcastSubscriber::CallBroadcastSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
    memberFuncMap_[UNKNOWN_BROADCAST_EVENT] = &CallBroadcastSubscriber::UnknownBroadcast;
    memberFuncMap_[SIM_STATE_BROADCAST_EVENT] = &CallBroadcastSubscriber::SimStateBroadcast;
    memberFuncMap_[CONNECT_CALLUI_SERVICE] = &CallBroadcastSubscriber::ConnectCallUiServiceBroadcast;
    memberFuncMap_[HIGH_TEMP_LEVEL_CHANGED] = &CallBroadcastSubscriber::HighTempLevelChangedBroadcast;
    memberFuncMap_[SUPER_PRIVACY_MODE] = &CallBroadcastSubscriber::ConnectCallUiSuperPrivacyModeBroadcast;
}

void CallBroadcastSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    uint32_t code = UNKNOWN_BROADCAST_EVENT;
    OHOS::EventFwk::Want want = data.GetWant();
    std::string action = data.GetWant().GetAction();
    TELEPHONY_LOGI("receive one broadcast:%{public}s", action.c_str());
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_SIM_STATE_CHANGED) {
        code = SIM_STATE_BROADCAST_EVENT;
    } else if (action == "event.custom.contacts.PAGE_STATE_CHANGE") {
        code = CONNECT_CALLUI_SERVICE;
    } else if (action == "usual.event.thermal.satcomm.HIGH_TEMP_LEVEL") {
        code = HIGH_TEMP_LEVEL_CHANGED;
    } else if (action == "usual.event.SUPER_PRIVACY_MODE") {
        code = SUPER_PRIVACY_MODE;
    } else {
        code = UNKNOWN_BROADCAST_EVENT;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data);
        }
    }
}

void CallBroadcastSubscriber::UnknownBroadcast(const EventFwk::CommonEventData &data)
{
    TELEPHONY_LOGI("you receive one unknown broadcast!");
}

void CallBroadcastSubscriber::SimStateBroadcast(const EventFwk::CommonEventData &data)
{
    TELEPHONY_LOGI("sim state broadcast code:%{public}d", data.GetCode());
}

void CallBroadcastSubscriber::ConnectCallUiServiceBroadcast(const EventFwk::CommonEventData &data)
{
    bool isConnectService = data.GetWant().GetBoolParam("isShouldConnect", false);
    TELEPHONY_LOGI("isConnectService:%{public}d", isConnectService);
    DelayedSingleton<CallControlManager>::GetInstance()->ConnectCallUiService(isConnectService);
}

void CallBroadcastSubscriber::HighTempLevelChangedBroadcast(const EventFwk::CommonEventData &data)
{
    int32_t satcommHighTempLevel = data.GetWant().GetIntParam("satcomm_high_temp_level", -1);
    TELEPHONY_LOGI("satcommHighTempLevel:%{public}d", satcommHighTempLevel);
    DelayedSingleton<SatelliteCallControl>::GetInstance()->SetSatcommTempLevel(satcommHighTempLevel);
}

void CallBroadcastSubscriber::ConnectCallUiSuperPrivacyModeBroadcast(const EventFwk::CommonEventData &data)
{
    int32_t videoState = data.GetWant().GetIntParam("videoState", -1);
	bool isAnswer = data.GetWant().GetBoolParam("isAnswer", false);
	TELEPHONY_LOGI("Connect CallUiSuperPrivacyModeBroadcast isAnswer:%{public}d", isAnswer);
    if (isAnswer) {
    int32_t callId = data.GetWant().GetIntParam("callId", -1);
    TELEPHONY_LOGI("Connect CallUiSuperPrivacyModeBroadcast_Answer callId:%{public}d", callId);
    DelayedSingleton<CallControlManager>::GetInstance()->CloseAnswerSuperPrivacyMode(callId, videoState);
    } else {
    std::string phoneNumber = data.GetWant().GetStringParam("phoneNumber");
    TELEPHONY_LOGI("Connect CallUiSuperPrivacyModeBroadcast_Answer callId:%{public}s", phoneNumber.c_str());
    std::u16string phNumber = Str8ToStr16(phoneNumber);
    int32_t accountId = data.GetWant().GetIntParam("accountId", -1);
    int32_t dialScene = data.GetWant().GetIntParam("dialScene", -1);
    int32_t dialType = data.GetWant().GetIntParam("dialType", -1);
    int32_t callType = data.GetWant().GetIntParam("callType", -1);
    DelayedSingleton<CallControlManager>::GetInstance()->CloseSuperPrivacyMode(phNumber,
	accountId, videoState, dialScene, dialType, callType);
		}
}
} // namespace Telephony
} // namespace OHOS
