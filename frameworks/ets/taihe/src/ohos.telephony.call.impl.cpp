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

 using namespace taihe;
 using namespace OHOS::Telephony;
 namespace{
// To be implemented.

void makeCallSync(::taihe::string_view phoneNumber)
{
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->MakeCall(std::string(phoneNumber));
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "makeCall return error");
    }
    return;
}

void makeCallSync2(uintptr_t context, ::taihe::string_view phoneNumber)
{
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->MakeCall(std::string(phoneNumber));
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "makeCall2 return error");
    }
    return;
}

bool hasVoiceCapability()
{
    auto ret = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HasVoiceCapability();
    return static_cast<ani_boolean>(ret);
}

void formatPhoneNumberSync(::taihe::string_view phoneNumber,
    ::ohos::telephony::call::NumberFormatOptions const& options)
{
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

void formatPhoneNumberSync2(::taihe::string_view phoneNumber)
{
    std::u16string phoneNum = OHOS::Str8ToStr16(std::string(phoneNumber));
    std::u16string countryCode = OHOS::Str8ToStr16("");
    std::u16string formatNum = OHOS::Str8ToStr16("");
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
        phoneNum, countryCode, formatNum);
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errCode, "formatPhoneNumber2 return error");
    }
    return;
}

::ohos::telephony::call::CallState getCallStateSync()
{
    int32_t callState = static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN);
    callState = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetCallState();
    return static_cast<::ohos::telephony::call::CallState::key_t>(callState);
}

bool hasCallSync()
{
    auto ret = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HasCall();
    return static_cast<ani_boolean>(ret);
}

void onCallDetailsChange(::taihe::callback_view<void(::ohos::telephony::call::CallAttributeOptions const&)> callback)
{
    TELEPHONY_LOGI("onCallDetailsChange");
    return;
}

void offCallDetailsChange(::taihe::optional_view<::taihe::callback<void(
    ::ohos::telephony::call::CallAttributeOptions const&)>> callback)
{
    TELEPHONY_LOGI("offCallDetailsChange");
    return;
}

void onCallEventChange(::taihe::callback_view<void(::ohos::telephony::call::CallEventOptions const&)> callback)
{
    TELEPHONY_LOGI("onCallEventChange");
    return;
}

void offCallEventChange(::taihe::optional_view<::taihe::callback<void(
    ::ohos::telephony::call::CallEventOptions const&)>> callback)
{
    TELEPHONY_LOGI("offCallEventChange");
    return;
}

void onCallDisconnectedCauses(::taihe::callback_view<void(
    ::ohos::telephony::call::DisconnectedDetails const&)> callback)
{
    TELEPHONY_LOGI("onCallDisconnectedCause");
    return;
}

void offCallDisconnectedCauses(::taihe::optional_view<::taihe::callback<void(
    ::ohos::telephony::call::DisconnectedDetails const&)>> callback)
{
    TELEPHONY_LOGI("offCallDisconnectedCauses");
    return;
}

void onMmiCodeResult(::taihe::callback_view<void(::ohos::telephony::call::MmiCodeResults const&)> callback)
{
    TELEPHONY_LOGI("onMmiCodeResult");
    return;
}

void offMmiCodeResult(::taihe::optional_view<::taihe::callback<void(
    ::ohos::telephony::call::MmiCodeResults const&)>> callback)
{
    TELEPHONY_LOGI("offMmiCodeResult");
    return;
}

void onAudioDeviceChange(::taihe::callback_view<void(::ohos::telephony::call::AudioDeviceCallbackinfo const&)> callback)
{
    TElEPHONY_LOGI("onAudioDeviceChange");
    return;
}

void offAudioDeviceChange(::taihe::optional_view<::taihe::callback<void(
    ::ohos::telephony::call::AudioDeviceCallbackinfo const&)>> callback)
{
    TELEPHONY_LOGI("offAudioDeviceChange");
    return;
}

void onPostDialDelay(::taihe::callback_view<void(::taihe::string_view)> callback)
{
    TELEPHONY_LOGI("onPostDialDelay");
    return;
}

void offPostDialDelay(::taihe::optional_view<::taihe::callback<void(::taihe::string_view)>> callback)
{
    TELEPHONY_LOGI("offPostDialDelay");
    return;
}

void onImsCallModeChange(::ohos::taihe::callback_view<void(::ohos::telephony::call::ImsCallModeInfo const&)> callback)
{
    TELEPHONY_LOGI("onImsCallModeChange");
    return;
}

void offImsCallModeChange(::taihe::optional_view<::taihe::callback<void(
    ::ohos::telephony::call::ImsCallModeInfo const&)>> callback)
{
    TELEPHONY_LOGI("offImsCallmodeChange");
    return;
}

void oncallSessionEvent(::taihe::callback_view<void(::ohos::telephony::call::CallSessionEvent const&)> callback)
{
    TELEPHONY_LOGI("onCallSessionEvent");
    return;
}

void offCallSessionEvent(::taihe::optional_view<::taihe::callback<void(
    ::ohos::telephony::call::CallSessionEvent const&)>> callback)
{
    TELEPHONY_LOGI("offCallSessionEvent");
    return;
}

void onPeerDimensionsChange(::taihe::callback_view<void(::ohos::telephony::call::PeerDimensionsDetail const&)> callback)
{
    TELEPHONY_LOGI("onPeerDimensionsChange");
    return;
}

void offPeerDimensionsChange(::taihe::optional_view<::taihe::callback<void(
    ::ohos::telephony::call::PeerDimensionsDetail const&)>> callback)
{
    TELEPHONY_LOGI("offPeerDimensionsChange");
    return;
}

void onCameraCapabilitiesChange(::taihe::callback_view<void(
    ::ohos::telephony::call::CameraCapabilities const&)> callback)
{
    TELEPHONY_LOGI("onCameraCapabilitiesChange");
    return;
}

void offCameraCapabilitiesChange(::taihe::optional_view<::taihe::callback<void(
    ::ohos::telephony::call::CameraCapabilities const&)>> callback)
{
    TELEPHONY_LOGI("offCameraCapabilitiesChange");
    return;
}
} //namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_makeCallSync(makeCallSync);
TH_EXPORT_CPP_API_makeCallSync2(makeCallSync2);
TH_EXPORT_CPP_API_hasVoiceCapability(hasVoiceCapability);
TH_EXPORT_CPP_API_formatPhoneNumberSync(formatPhoneNumberSync);
TH_EXPORT_CPP_API_formatPhoneNumberSync2(formatPhoneNumberSync2);
TH_EXPORT_CPP_API_getCallStateSync(getCallStateSync);
TH_EXPORT_CPP_API_hasCallSync(hasCallSync);
TH_EXPORT_CPP_API_onCallDetailsChange(onCallDetailsChange);
TH_EXPORT_CPP_API_offCallDetailsChange(offCallDetailsChange);
TH_EXPORT_CPP_API_onCallEventChange(onCallEventChange);
TH_EXPORT_CPP_API_offCallEventChange(offCallEventChange);
TH_EXPORT_CPP_API_onCallDisconnectedCauses(onCallDisconnectedCauses);
TH_EXPORT_CPP_API_offCallDisconnectedCauses(offCallDisconnectedCauses);
TH_EXPORT_CPP_API_onMmiCodeResult(onMmiCodeResult);
TH_EXPORT_CPP_API_offMmiCodeResult(offMmiCodeResult);
TH_EXPORT_CPP_API_onAudioDeviceChange(onAudioDeviceChange);
TH_EXPORT_CPP_API_offAudioDevicechange(offAudioDeviceChange);
TH_EXPORT_CPP_API_onPostDialDelay(onPostDialDelay);
TH_EXPORT_CPP_API_offPostDialDelay(offPostDialDelay);
TH_EXPORT_CPP_API_onImsCallModeChange(onImsCallModeChange);
TH_EXPORT_CPP_API_offImsCallModeChange(offImsCallModeChange);
TH_EXPORT_CPP_API_onCallSessionEvent(onCallSessionEvent);
TH_EXPORT_CPP_API_offCallSessionEvent(offCallSessionEvent);
TH_EXPORT_CPP_API_onPeerDimensionsChange(onPeerDimensionsChange);
TH_EXPORT_CPP_API_offPeerDimensionsChange(offPeerDimensionsChange);
TH_EXPORT_CPP_API_onCameraCapabilitiesChange(onCameraCapabilitiesChange);
TH_EXPORT_CPP_API_offCameraCapabilitiesChange(offCameraCapabilitiesChange);
// NOLINTEND