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

#include "bluetooth_call_manager.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "audio_control_manager.h"
#include "call_control_manager.h"
#include "bluetooth_state_observer.h"
#include "bluetooth_connection.h"

#ifdef ABILITY_BLUETOOTH_SUPPORT
#include "bluetooth_mgr_client.h"
#include "bluetooth_remote_device.h"
#endif

namespace OHOS {
namespace Telephony {
BluetoothCallManager::BluetoothCallManager() : btConnection_(std::make_unique<BluetoothConnection>()) {}

BluetoothCallManager::~BluetoothCallManager() {}

int32_t BluetoothCallManager::Init()
{
    if (!BluetoothStateObserver::SubscribeBluetoothEvent()) {
        TELEPHONY_LOGE("subscribe bluetooth connection event failed");
        return CALL_ERR_BLUETOOTH_CONNECTION_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

void BluetoothCallManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object nullptr");
        return;
    }
    std::string number = callObjectPtr->GetAccountNumber();
    TelCallState callState = callObjectPtr->GetTelCallState();
    VideoStateType videoState = callObjectPtr->GetVideoStateType();
    SendCallState(callState, number, videoState);
}

bool BluetoothCallManager::ConnectBtSco()
{
    if (btConnection_ == nullptr) {
        TELEPHONY_LOGE("bluetooth connection nullptr");
        return false;
    }
    return btConnection_->ConnectBtSco();
}

bool BluetoothCallManager::DisconnectBtSco()
{
    if (btConnection_ == nullptr) {
        TELEPHONY_LOGE("bluetooth connection nullptr");
        return false;
    }
    return btConnection_->DisconnectBtSco();
}

BtScoState BluetoothCallManager::GetBtScoState()
{
    if (btConnection_ == nullptr) {
        TELEPHONY_LOGE("bluetooth connection nullptr");
        return BtScoState::SCO_STATE_UNKNOWN;
    }
    return btConnection_->GetBtScoState();
}

bool BluetoothCallManager::IsBtScoConnected()
{
    if (btConnection_ == nullptr) {
        TELEPHONY_LOGE("bluetooth connection nullptr");
        return false;
    }
    return btConnection_->IsBtScoConnected();
}

bool BluetoothCallManager::AnswerBtCall()
{
    if (!IsBtScoConnected()) {
        TELEPHONY_LOGE("bluetooth sco is not connected");
        return false;
    }
    auto callList = DelayedSingleton<AudioControlManager>::GetInstance()->GetCallList();
    bool result = true;
    for (auto call : callList) {
        if (call->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING) {
            result = DelayedSingleton<CallControlManager>::GetInstance()->AnswerCall(
                call->GetCallID(), static_cast<int32_t>(call->GetVideoStateType()));
            TELEPHONY_LOGI("call id : %{public}d, answer result : %{public}d", call->GetCallID(), result);
            break;
        }
    }
    return result;
}

bool BluetoothCallManager::HangupBtCall()
{
    if (!IsBtScoConnected()) {
        TELEPHONY_LOGE("bluetooth sco is not connected");
        return false;
    }
    auto callList = DelayedSingleton<AudioControlManager>::GetInstance()->GetCallList();
    bool result = true;
    for (auto call : callList) {
        if (call->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING ||
            call->GetTelCallState() == TelCallState::CALL_STATUS_ACTIVE ||
            call->GetTelCallState() == TelCallState::CALL_STATUS_HOLDING) {
            int32_t ret = DelayedSingleton<CallControlManager>::GetInstance()->HangUpCall(call->GetCallID());
            TELEPHONY_LOGI("call id : %{public}d, state : %{public}d, hangup result : %{public}d",
                call->GetCallID(), call->GetTelCallState(), ret);
            if (ret != TELEPHONY_SUCCESS) {
                result = false;
            }
        }
    }
    return result;
}

int32_t BluetoothCallManager::SendCallState(
    TelCallState callState, const std::string &number, VideoStateType videoState)
{
    if (!IsBtScoConnected()) {
        TELEPHONY_LOGE("bluetooth sco is not connected");
        return CALL_ERR_BLUETOOTH_CONNECTION_FAILED;
    }
#ifdef ABILITY_BLUETOOTH_SUPPORT
    return BluetoothMgr::BluetoothMgrClient::GetInstance()->PhoneStateChanged(
        static_cast<int32_t>(callState, number), static_cast<int32_t>(videoState));
#endif
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallManager::SendCallList()
{
    if (!IsBtScoConnected()) {
        TELEPHONY_LOGE("bluetooth sco is not connected");
        return CALL_ERR_BLUETOOTH_CONNECTION_FAILED;
    }
#ifdef ABILITY_BLUETOOTH_SUPPORT
    auto callList = DelayedSingleton<AudioControlManager>::GetInstance()->GetCallList();
    bool result = false;
    for (auto call : callList) {
        result = BluetoothMgr::BluetoothMgrClient::GetInstance()->ClccResponse(call->GetCallID(),
            static_cast<int32_t>((call->GetTelCallState())), call->GetAccountNumber(),
            static_cast<int32_t>((call->GetVideoStateType())));
    }
    return result;
#endif
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallManager::SendDtmf(int32_t callId, char str)
{
    return DelayedSingleton<CallControlManager>::GetInstance()->StartDtmf(callId, str);
}

void BluetoothCallManager::NewCallCreated(sptr<CallBase> &callObjectPtr) {}

void BluetoothCallManager::CallDestroyed(int32_t cause) {}

void BluetoothCallManager::IncomingCallActivated(sptr<CallBase> &callObjectPtr) {}

void BluetoothCallManager::IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{}
} // namespace Telephony
} // namespace OHOS