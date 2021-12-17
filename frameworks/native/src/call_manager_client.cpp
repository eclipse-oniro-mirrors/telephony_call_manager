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

#include "call_manager_client.h"

#include <memory>

#include "telephony_errors.h"

#include "call_manager_proxy.h"

namespace OHOS {
namespace Telephony {
std::unique_ptr<CallManagerProxy> g_callManagerProxy = nullptr;

CallManagerClient::CallManagerClient() {}

CallManagerClient::~CallManagerClient() {}

void CallManagerClient::Init(int32_t systemAbilityId, std::u16string &bundleName)
{
    if (g_callManagerProxy == nullptr) {
        g_callManagerProxy = std::make_unique<CallManagerProxy>();
        if (g_callManagerProxy == nullptr) {
            TELEPHONY_LOGE("make_unique CallManagerProxy failed");
            return;
        }
        g_callManagerProxy->Init(systemAbilityId, bundleName);
    }
}

void CallManagerClient::UnInit()
{
    if (g_callManagerProxy != nullptr) {
        g_callManagerProxy->UnInit();
    } else {
        TELEPHONY_LOGE("init first please!");
    }
}

int32_t CallManagerClient::RegisterCallBack(std::unique_ptr<CallManagerCallback> callback)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->RegisterCallBack(std::move(callback));
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::UnRegisterCallBack()
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->UnRegisterCallBack();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::DialCall(std::u16string number, AppExecFwk::PacMap &extras)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->DialCall(number, extras);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::AnswerCall(int32_t callId, int32_t videoState)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->AnswerCall(callId, videoState);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::RejectCall(int32_t callId, bool isSendSms, std::u16string content)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->RejectCall(callId, isSendSms, content);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::HangUpCall(int32_t callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->HangUpCall(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::GetCallState()
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->GetCallState();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::HoldCall(int32_t callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->HoldCall(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::UnHoldCall(int32_t callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->UnHoldCall(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SwitchCall(int32_t callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SwitchCall(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::CombineConference(int32_t callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->CombineConference(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SeparateConference(int32_t callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SeparateConference(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::GetMainCallId(int32_t &callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->GetMainCallId(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

std::vector<std::u16string> CallManagerClient::GetSubCallIdList(int32_t callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->GetSubCallIdList(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        std::vector<std::u16string> vec;
        vec.clear();
        return vec;
    }
}

std::vector<std::u16string> CallManagerClient::GetCallIdListForConference(int32_t callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->GetCallIdListForConference(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        std::vector<std::u16string> vec;
        vec.clear();
        return vec;
    }
}

int32_t CallManagerClient::GetCallWaiting(int32_t slotId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->GetCallWaiting(slotId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetCallWaiting(int32_t slotId, bool activate)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetCallWaiting(slotId, activate);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->GetCallRestriction(slotId, type);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetCallRestriction(slotId, info);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->GetCallTransferInfo(slotId, type);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetCallTransferInfo(slotId, info);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetCallPreferenceMode(slotId, mode);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::StartDtmf(int32_t callId, char str)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->StartDtmf(callId, str);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::StopDtmf(int32_t callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->StopDtmf(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SendDtmf(int32_t callId, char str)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SendDtmf(callId, str);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SendBurstDtmf(int32_t callId, std::u16string str, int32_t on, int32_t off)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SendBurstDtmf(callId, str, on, off);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

bool CallManagerClient::IsRinging()
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->IsRinging();
    } else {
        TELEPHONY_LOGE("init first please!");
        return false;
    }
}

bool CallManagerClient::HasCall()
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->HasCall();
    } else {
        TELEPHONY_LOGE("init first please!");
        return false;
    }
}

bool CallManagerClient::IsNewCallAllowed()
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->IsNewCallAllowed();
    } else {
        TELEPHONY_LOGE("init first please!");
        return false;
    }
}

bool CallManagerClient::IsInEmergencyCall()
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->IsInEmergencyCall();
    } else {
        TELEPHONY_LOGE("init first please!");
        return false;
    }
}

bool CallManagerClient::IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, int32_t &errorCode)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->IsEmergencyPhoneNumber(number, slotId, errorCode);
    } else {
        TELEPHONY_LOGE("init first please!");
        return false;
    }
}

int32_t CallManagerClient::FormatPhoneNumber(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->FormatPhoneNumber(number, countryCode, formatNumber);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::FormatPhoneNumberToE164(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->FormatPhoneNumberToE164(number, countryCode, formatNumber);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetMuted(bool isMute)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetMuted(isMute);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::MuteRinger()
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->MuteRinger();
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetAudioDevice(AudioDevice deviceType)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetAudioDevice(deviceType);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::CancelMissedCallsNotification(int32_t id)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->CancelMissedCallsNotification(id);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::ControlCamera(std::u16string cameraId, std::u16string callingPackage)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->ControlCamera(cameraId, callingPackage);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetPreviewWindow(VideoWindow &window)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetPreviewWindow(window);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetDisplayWindow(VideoWindow &window)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetDisplayWindow(window);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetCameraZoom(float zoomRatio)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetCameraZoom(zoomRatio);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetPausePicture(std::u16string path)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetPausePicture(path);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetDeviceDirection(int32_t rotation)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetDeviceDirection(rotation);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}
} // namespace Telephony
} // namespace OHOS