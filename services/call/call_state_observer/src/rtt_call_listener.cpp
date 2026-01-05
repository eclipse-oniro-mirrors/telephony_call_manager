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

#include "rtt_call_listener.h"

#include "ims_rtt_manager.h"
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"


namespace OHOS {
namespace Telephony {

void RttCallListener::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr || callObjectPtr->GetCallType() != CallType::TYPE_IMS) {
        TELEPHONY_LOGE("callObjectPtr is nullptr, rtt call must based on IMS call.");
        return;
    }

    sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(callObjectPtr.GetRefPtr());
    switch (nextState) {
        case TelCallState::CALL_STATUS_ACTIVE:
            InitRttManager(imsCall);
            break;
        case TelCallState::CALL_STATUS_DISCONNECTED:
            UnInitRttManager();
            break;
        default:
            break;
    }
}

void RttCallListener::InitRttManager(sptr<IMSCall> &imsCall)
{
    TELEPHONY_LOGI("Start to InitRttManager");
    if (imsCall->GetRttState() != RttCallState::RTT_STATE_YES) {
        TELEPHONY_LOGI("cannot InitRttManager, rttState: %{public}d", imsCall->GetRttState());
        return;
    }
    int32_t currCallID = imsCall->GetCallID();
    int32_t currChannelID = imsCall->GetRttChannelId();
    if (rttManager_ == nullptr) {
        rttManager_ = std::make_shared<ImsRttManager>(currCallID, currChannelID);
    } else {
        rttManager_->SetCallID(currCallID);
        rttManager_->SetChannelID(currChannelID);
    }
    rttManager_->InitRtt();
    imsCall->SetPrevRtt(true);
    TELEPHONY_LOGI("InitRtt success, callId: %{public}d, channelId: %{public}d", currCallID, currChannelID);
}

void RttCallListener::UnInitRttManager()
{
    TELEPHONY_LOGI("Start to UnInitRttManager");
    if (rttManager_ == nullptr) {
        TELEPHONY_LOGI("RttManager already un-initialized");
        return;
    }

    rttManager_->DestroyRtt();
    rttManager_ = nullptr;
}

int32_t RttCallListener::SendRttMessage(const std::string &rttMessage)
{
    if (rttManager_ == nullptr) {
        TELEPHONY_LOGE("rttManager_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    return rttManager_->SendRttMessage(rttMessage);
}
} // namespace Telephony
} // namespace OHOS
