/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "bluetooth_call.h"
#include "call_control_manager.h"
#include "call_manager_errors.h"
#include "call_object_manager.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

BluetoothCall::BluetoothCall(DialParaInfo &info, const std::string &macAddress) : CarrierCall(info)
{
    callId_ = info.callId;
    macAddress_ = macAddress;
}

BluetoothCall::BluetoothCall(DialParaInfo &info, AppExecFwk::PacMap &extras, const std::string &macAddress)
    : CarrierCall(info, extras)
{
    callId_ = info.callId;
    macAddress_ = macAddress;
    if (macAddress_.empty()) {
        TELEPHONY_LOGI("macAddress is empty");
    }
}

BluetoothCall::~BluetoothCall() {}

int32_t BluetoothCall::DialingProcess()
{
    TELEPHONY_LOGI("BluetoothCall DialingProcess start");
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::AnswerCall(int32_t videoState)
{
    TELEPHONY_LOGI("BluetoothCall AnswerCall");
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return CALL_ERR_ANSWER_FAILED;
    }
    if (macAddress_.empty()) {
        TELEPHONY_LOGE("error: macAddress is empty");
    }
    Bluetooth::BluetoothRemoteDevice device(macAddress_);
    profile->AcceptIncomingCall(device, 0);
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::RejectCall()
{
    TELEPHONY_LOGI("BluetoothCall RejectCall enter");
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return CALL_ERR_REJECT_FAILED;
    }
    if (macAddress_.empty()) {
        TELEPHONY_LOGE("error: macAddress is empty");
    }
    Bluetooth::BluetoothRemoteDevice device(macAddress_);
    profile->RejecttIncomingCall(device);
    int32_t ret = RejectCallBase();
    return ret;
}

int32_t BluetoothCall::HangUpCall()
{
    TELEPHONY_LOGI("BluetoothCall HangUpCall enter");
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return CALL_ERR_HANGUP_FAILED;
    }
    if (macAddress_.empty()) {
        TELEPHONY_LOGE("error: macAddress is empty");
    }
    Bluetooth::HandsFreeUnitCall call;
    Bluetooth::BluetoothRemoteDevice device(macAddress_);
    profile->FinishActiveCall(device, call);
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::HoldCall()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::UnHoldCall()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::SwitchCall()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::SetMute(int32_t mute, int32_t slotId)
{
    return TELEPHONY_SUCCESS;
}

void BluetoothCall::GetCallAttributeInfo(CallAttributeInfo &info)
{
    GetCallAttributeCarrierInfo(info);
    info.callId = callId_;
    return;
}

int32_t BluetoothCall::CombineConference()
{
    return TELEPHONY_SUCCESS;
}

void BluetoothCall::HandleCombineConferenceFailEvent()
{
    return;
}

int32_t BluetoothCall::SeparateConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::KickOutFromConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::CanCombineConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::CanSeparateConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::CanKickOutFromConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::GetMainCallId(int32_t &mainCallId)
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::GetSubCallIdList(std::vector<std::u16string> &callIdList)
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::GetCallIdListForConference(std::vector<std::u16string> &callIdList)
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::IsSupportConferenceable()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::LaunchConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::ExitConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCall::HoldConference()
{
    return TELEPHONY_SUCCESS;
}

int BluetoothCall::StartDtmf(char str)
{
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return CALL_ERR_FUNCTION_NOT_SUPPORTED;
    }
    if (macAddress_.empty()) {
        TELEPHONY_LOGE("error: macAddress is empty");
    }
    Bluetooth::BluetoothRemoteDevice device(macAddress_);
    uint8_t code = (uint8_t)str;
    profile->SendDTMFTone(device, code);
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
