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

#include "call_manager_proxy.h"
#include "parameter.h"
#include "telephony_errors.h"

namespace OHOS {
namespace Telephony {
std::shared_ptr<CallManagerProxy> g_callManagerProxy = nullptr;

const std::string KEY_VOICECALL_CAP = "const.telephony.voice.capable";
const int32_t VOICECALL_CAP_VAL_LEN = 6;

CallManagerClient::CallManagerClient() {}

CallManagerClient::~CallManagerClient() {}

void CallManagerClient::Init(int32_t systemAbilityId)
{
    if (g_callManagerProxy == nullptr) {
        g_callManagerProxy = DelayedSingleton<CallManagerProxy>::GetInstance();
        if (g_callManagerProxy == nullptr) {
            TELEPHONY_LOGE("g_callManagerProxy is nullptr");
            return;
        }
        g_callManagerProxy->Init(systemAbilityId);
    }
    TELEPHONY_LOGI("CallManagerClient init success!");
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

int32_t CallManagerClient::SetAudioDevice(AudioDevice deviceType, const std::string &bluetoothAddress)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetAudioDevice(deviceType, bluetoothAddress);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::ControlCamera(std::u16string cameraId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->ControlCamera(cameraId);
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

int32_t CallManagerClient::GetImsConfig(int32_t slotId, ImsConfigItem item)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->GetImsConfig(slotId, item);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetImsConfig(slotId, item, value);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::GetImsFeatureValue(int32_t slotId, FeatureType type)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->GetImsFeatureValue(slotId, type);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->SetImsFeatureValue(slotId, type, value);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::UpdateImsCallMode(int32_t callId, ImsCallMode mode)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->UpdateImsCallMode(callId, mode);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::EnableImsSwitch(int32_t slotId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->EnableImsSwitch(slotId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::DisableImsSwitch(int32_t slotId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->DisableImsSwitch(slotId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::IsImsSwitchEnabled(int32_t slotId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->IsImsSwitchEnabled(slotId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::EnableLteEnhanceMode(int32_t slotId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->EnableLteEnhanceMode(slotId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::DisableLteEnhanceMode(int32_t slotId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->DisableLteEnhanceMode(slotId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::IsLteEnhanceModeEnabled(int32_t slotId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->IsLteEnhanceModeEnabled(slotId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::StartRtt(int32_t callId, std::u16string &msg)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->StartRtt(callId, msg);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::StopRtt(int32_t callId)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->StopRtt(callId);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::JoinConference(int32_t callId, std::vector<std::u16string> &numberList)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->JoinConference(callId, numberList);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::ReportOttCallDetailsInfo(std::vector<OttCallDetailsInfo> &ottVec)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->ReportOttCallDetailsInfo(ottVec);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

int32_t CallManagerClient::ReportOttCallEventInfo(OttCallEventInfo &eventInfo)
{
    if (g_callManagerProxy != nullptr) {
        return g_callManagerProxy->ReportOttCallEventInfo(eventInfo);
    } else {
        TELEPHONY_LOGE("init first please!");
        return TELEPHONY_ERR_UNINIT;
    }
}

bool CallManagerClient::HasVoiceCapability()
{
    char retValue[VOICECALL_CAP_VAL_LEN + 1] = {"true"};
    int retLen = GetParameter(KEY_VOICECALL_CAP.c_str(), "true", retValue, VOICECALL_CAP_VAL_LEN);
    TELEPHONY_LOGI("HasVoiceCapability retValue %{public}s, retLen %{public}d", retValue, retLen);
    if (strcmp(retValue, "false") == 0) {
        return false;
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS
