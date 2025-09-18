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

#include "ohos.telephony.call.proj.hpp"
#include "ohos.telephony.call.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"

#include "call_manager_client.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include <string_ex.h>

#include "taihe_call_manager.h"
#include "taihe_call_ability_callback.h"

using namespace taihe;
using namespace OHOS::Telephony;
namespace {
const int32_t TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID = 4005;
class CallManagerClientInitializer {
public:
    CallManagerClientInitializer()
    {
        static bool isInit = false;
        if (!isInit) {
            OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
            isInit = true;
        }
    }
};

void MakeCallSync(::taihe::string_view phoneNumber)
{
    CallManagerClientInitializer init;
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->MakeCall(std::string(phoneNumber));
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "makeCall return error");
    }
    return;
}

void MakeCallSync2(uintptr_t context, ::taihe::string_view phoneNumber)
{
    CallManagerClientInitializer init;
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->MakeCall(std::string(phoneNumber));
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "makeCall2 return error");
    }
    return;
}

bool HasVoiceCapability()
{
    CallManagerClientInitializer init;
    auto ret = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HasVoiceCapability();
    return static_cast<ani_boolean>(ret);
}

void FormatPhoneNumberSync(::taihe::string_view phoneNumber,
    ::ohos::telephony::call::NumberFormatOptions const& options)
{
    CallManagerClientInitializer init;
    std::u16string phoneNum = OHOS::Str8ToStr16(std::string(phoneNumber));
    std::u16string countryCode = OHOS::Str8ToStr16(std::string(options.countryCode));
    std::u16string formatNum = OHOS::Str8ToStr16("");
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
        phoneNum, countryCode, formatNum);
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "formatPhoneNumber return error");
    }
    return;
}

void FormatPhoneNumberSync2(::taihe::string_view phoneNumber)
{
    CallManagerClientInitializer init;
    std::u16string phoneNum = OHOS::Str8ToStr16(std::string(phoneNumber));
    std::u16string countryCode = OHOS::Str8ToStr16("cn");
    std::u16string formatNum = OHOS::Str8ToStr16("");
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
        phoneNum, countryCode, formatNum);
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "formatPhoneNumber2 return error");
    }
    return;
}

::ohos::telephony::call::CallState GetCallStateSync()
{
    CallManagerClientInitializer init;
    int32_t callState = static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN);
    callState = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetCallState();
    return static_cast<::ohos::telephony::call::CallState::key_t>(callState);
}

bool HasCallSync()
{
    CallManagerClientInitializer init;
    auto ret = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HasCall();
    return static_cast<ani_boolean>(ret);
}

void OnCallDetailsChange(
    ::taihe::callback_view<void(::ohos::telephony::call::CallAttributeOptions const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().callStateCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::CallAttributeOptions const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void OffCallDetailsChange(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::CallAttributeOptions const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().callStateCallback_.reset();
    return;
}

void OnCallEventChange(::taihe::callback_view<void(::ohos::telephony::call::CallEventOptions const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().callEventCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::CallEventOptions const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void OffCallEventChange(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::CallEventOptions const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().callEventCallback_.reset();
    return;
}

void OnCallDisconnectedCauses(
    ::taihe::callback_view<void(::ohos::telephony::call::DisconnectedDetails const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().disconnectedCauseCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::DisconnectedDetails const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void OffCallDisconnectedCauses(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::DisconnectedDetails const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().disconnectedCauseCallback_.reset();
    return;
}

void OnMmiCodeResult(::taihe::callback_view<void(::ohos::telephony::call::MmiCodeResults const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().mmiCodeResultCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::MmiCodeResults const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void OffMmiCodeResult(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::MmiCodeResults const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().mmiCodeResultCallback_.reset();
    return;
}

void OnAudioDeviceChange(
    ::taihe::callback_view<void(::ohos::telephony::call::AudioDeviceCallbackInfo const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().audioDeviceCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::AudioDeviceCallbackInfo const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void OffAudioDeviceChange(::taihe::optional_view<
    ::taihe::callback<void(::ohos::telephony::call::AudioDeviceCallbackInfo const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().audioDeviceCallback_.reset();
    return;
}

void OnPostDialDelay(::taihe::callback_view<void(::taihe::string_view data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().postDialDelayCallback_ =
        ::taihe::optional<taihe::callback<void(::taihe::string_view)>>{
            std::in_place_t{},
            callback};
    return;
}

void OffPostDialDelay(::taihe::optional_view<::taihe::callback<void(::taihe::string_view data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().postDialDelayCallback_.reset();
    return;
}

void OnImsCallModeChange(
    ::taihe::callback_view<void(::ohos::telephony::call::ImsCallModeInfo const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().imsCallModeCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::ImsCallModeInfo const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void OffImsCallModeChange(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::ImsCallModeInfo const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().imsCallModeCallback_.reset();
    return;
}

void OnCallSessionEvent(::taihe::callback_view<void(::ohos::telephony::call::CallSessionEvent const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().callSessionEventCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::CallSessionEvent const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void OffCallSessionEvent(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::CallSessionEvent const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().callSessionEventCallback_.reset();
    return;
}

void OnPeerDimensionsChange(
    ::taihe::callback_view<void(::ohos::telephony::call::PeerDimensionsDetail const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().peerDimensionsCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::PeerDimensionsDetail const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void OffPeerDimensionsChange(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::PeerDimensionsDetail const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().peerDimensionsCallback_.reset();
    return;
}

void OnCameraCapabilitiesChange(
    ::taihe::callback_view<void(::ohos::telephony::call::CameraCapabilities const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().cameraCapabilitiesCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::CameraCapabilities const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void OffCameraCapabilitiesChange(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::CameraCapabilities const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().cameraCapabilitiesCallback_.reset();
    return;
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_makeCallSync(MakeCallSync);
TH_EXPORT_CPP_API_makeCallSync2(MakeCallSync2);
TH_EXPORT_CPP_API_HasVoiceCapability(HasVoiceCapability);
TH_EXPORT_CPP_API_formatPhoneNumberSync(FormatPhoneNumberSync);
TH_EXPORT_CPP_API_formatPhoneNumberSync2(FormatPhoneNumberSync2);
TH_EXPORT_CPP_API_getCallStateSync(GetCallStateSync);
TH_EXPORT_CPP_API_HasCallSync(HasCallSync);
TH_EXPORT_CPP_API_onCallDetailsChange(OnCallDetailsChange);
TH_EXPORT_CPP_API_offCallDetailsChange(OffCallDetailsChange);
TH_EXPORT_CPP_API_onCallEventChange(OnCallEventChange);
TH_EXPORT_CPP_API_offCallEventChange(OffCallEventChange);
TH_EXPORT_CPP_API_onCallDisconnectedCauses(OnCallDisconnectedCauses);
TH_EXPORT_CPP_API_offCallDisconnectedCauses(OffCallDisconnectedCauses);
TH_EXPORT_CPP_API_onMmiCodeResult(OnMmiCodeResult);
TH_EXPORT_CPP_API_offMmiCodeResult(OffMmiCodeResult);
TH_EXPORT_CPP_API_onAudioDeviceChange(OnAudioDeviceChange);
TH_EXPORT_CPP_API_offAudioDeviceChange(OffAudioDeviceChange);
TH_EXPORT_CPP_API_onPostDialDelay(OnPostDialDelay);
TH_EXPORT_CPP_API_offPostDialDelay(OffPostDialDelay);
TH_EXPORT_CPP_API_onImsCallModeChange(OnImsCallModeChange);
TH_EXPORT_CPP_API_offImsCallModeChange(OffImsCallModeChange);
TH_EXPORT_CPP_API_onCallSessionEvent(OnCallSessionEvent);
TH_EXPORT_CPP_API_offCallSessionEvent(OffCallSessionEvent);
TH_EXPORT_CPP_API_onPeerDimensionsChange(OnPeerDimensionsChange);
TH_EXPORT_CPP_API_offPeerDimensionsChange(OffPeerDimensionsChange);
TH_EXPORT_CPP_API_onCameraCapabilitiesChange(OnCameraCapabilitiesChange);
TH_EXPORT_CPP_API_offCameraCapabilitiesChange(OffCameraCapabilitiesChange);
// NOLINTEND