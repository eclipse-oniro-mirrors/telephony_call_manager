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
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include "taihe/array.hpp"
#include "taihe/vector.hpp"
#include "call_manager_errors.h"
#include "telephony_types.h"
#include "ani_common_want.h"
#include "want_params_wrapper.h"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "call_manager_client.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include <string_ex.h>
#include "taihe_call_manager.h"
#include "taihe_call_ability_callback.h"
#include "napi_util.h"
#include "taihe_ani_callback_common.h"
#include "taihe_ani_call_update_async_result.h"

using namespace taihe;
using namespace OHOS::Telephony;
namespace {
const int32_t DEFAULT_CALL_ID = 0;

static void ConvertErrorForBusinessError(int32_t errorCode)
{
    if (errorCode == TELEPHONY_ERR_PERMISSION_ERR) {
        set_business_error(JS_ERROR_TELEPHONY_PERMISSION_DENIED,
            "BusinessError 201:Permission denied");
    } else {
        OHOS::Telephony::JsError error = NapiUtil::ConverErrorMessageForJs(errorCode);
        set_business_error(error.errorCode, error.errorMessage);
    }
}

static inline bool IsValidSlotId(int32_t slotId)
{
    return ((slotId >= DEFAULT_SIM_SLOT_ID) && (slotId < SIM_SLOT_COUNT));
}

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
        ConvertErrorForBusinessError(errCode);
    }
    return;
}

void MakeCallSync2(uintptr_t context, ::taihe::string_view phoneNumber)
{
    CallManagerClientInitializer init;
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->MakeCall(std::string(phoneNumber));
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errCode);
    }
    return;
}

bool HasVoiceCapability()
{
    CallManagerClientInitializer init;
    auto ret = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HasVoiceCapability();
    return static_cast<ani_boolean>(ret);
}

void FormatPhoneNumberSync(
    ::taihe::string_view phoneNumber, ::ohos::telephony::call::NumberFormatOptions const& options)
{
    CallManagerClientInitializer init;
    std::u16string phoneNum = OHOS::Str8ToStr16(std::string(phoneNumber));
    std::string code = std::string(options.countryCode.value_or("cn"));
    if (code.size() < 1) {
        code = "cn";
    }
    std::u16string countryCode = OHOS::Str8ToStr16(code);
    std::u16string formatNum = OHOS::Str8ToStr16("");
    auto errCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
        phoneNum, countryCode, formatNum);
    if (errCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errCode);
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
        ConvertErrorForBusinessError(errCode);
    }
    return;
}

bool IsImsSwitchEnabledSync(int32_t slotId)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return false;
    }

    bool enabled = false;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->IsImsSwitchEnabled(slotId, enabled);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    return enabled;
}

void DisableImsSwitchSync(int32_t slotId)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->DisableImsSwitch(slotId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void EnableImsSwitchSync(int32_t slotId)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->EnableImsSwitch(slotId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void UpdateImsCallModeSync(int32_t callId, ::ohos::telephony::call::ImsCallMode mode)
{
    CallManagerClientInitializer init;
    ImsCallMode Mode = static_cast<ImsCallMode>(mode.get_key());
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->UpdateImsCallMode(callId, Mode);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void JoinConferenceSync(int32_t mainCallId, ::taihe::array_view<::taihe::string> callNumberList)
{
    CallManagerClientInitializer init;
    std::vector<std::u16string> callNumberList16;
    for (size_t i = 0; i < callNumberList.size(); i++) {
        callNumberList16.push_back(OHOS::Str8ToStr16(std::string(callNumberList[i])));
    }
    int32_t errorCode =
        OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->JoinConference(mainCallId, callNumberList16);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void SetAudioDeviceSync(::ohos::telephony::call::AudioDevice const & device)
{
    CallManagerClientInitializer init;
    AudioDevice audioDevice;
    if (device.address.has_value() && device.address->size() > kMaxAddressLen) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_ARGUMENT_INVALID);
        return;
    }

    const ::taihe::string &deviceAddress = device.address.value_or("");
    if (memcpy_s(audioDevice.address, kMaxAddressLen, deviceAddress.data(), deviceAddress.size()) != EOK) {
        ConvertErrorForBusinessError(TELEPHONY_ERROR);
        return;
    }
    const ::taihe::string &deviceNameTrans = device.deviceName.value_or("");
    if (memcpy_s(audioDevice.deviceName, kMaxDeviceNameLen, deviceNameTrans.data(), deviceNameTrans.size()) != EOK) {
        ConvertErrorForBusinessError(TELEPHONY_ERROR);
        return;
    }
    int32_t value = device.deviceType.get_value();
    audioDevice.deviceType = static_cast<OHOS::Telephony::AudioDeviceType>(value);
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetAudioDevice(audioDevice);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void CancelMutedSync()
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetMuted(false);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void SetMutedSync()
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetMuted(true);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

bool IsRingingSync()
{
    CallManagerClientInitializer init;
    bool enabled = false;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->IsRinging(enabled);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    return enabled;
}

void SeparateConferenceSync(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SeparateConference(callId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

bool IsNewCallAllowedSync()
{
    CallManagerClientInitializer init;
    bool enabled = false;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->IsNewCallAllowed(enabled);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    return enabled;
}

bool IsInEmergencyCallSync()
{
    CallManagerClientInitializer init;
    bool enabled = false;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->IsInEmergencyCall(enabled);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    return enabled;
}

void StopDTMFSync(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->StopDtmf(callId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void StartDTMFSync(int32_t callId, ::taihe::string_view character)
{
    CallManagerClientInitializer init;
    if (character.empty()) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_ARGUMENT_INVALID);
        return;
    }

    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->StartDtmf(
        callId, character.c_str()[0]);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

::taihe::array<::taihe::string> GetCallIdListForConferenceSync(int32_t callId)
{
    CallManagerClientInitializer init;
    std::vector<std::u16string> messageArray;
    int32_t errorCode =
        OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetCallIdListForConference(callId, messageArray);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }

    taihe::vector<taihe::string> messageVector;
    for (const auto &message : messageArray) {
        messageVector.push_back(OHOS::Str16ToStr8(message));
    }
    return taihe::array<taihe::string>{ taihe::copy_data_t{}, messageVector.begin(), messageVector.size() };
}

::taihe::array<::taihe::string> GetSubCallIdListSync(int32_t callId)
{
    CallManagerClientInitializer init;
    std::vector<std::u16string> listResult;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetSubCallIdList(callId, listResult);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    taihe::vector<taihe::string> resultVector;
    for (const auto &message : listResult) {
        resultVector.push_back(OHOS::Str16ToStr8(message));
    }
    return taihe::array<taihe::string>{ taihe::copy_data_t{}, resultVector.begin(), resultVector.size() };
}

int32_t GetMainCallIdSync(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t result = 0;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetMainCallId(callId, result);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    return result;
}

void CombineConferenceSync(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->CombineConference(callId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void SwitchCallSync(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SwitchCall(callId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void UnHoldCallSync(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->UnHoldCall(callId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void HoldCallSync(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HoldCall(callId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

::taihe::string FormatPhoneNumberToE164Sync(::taihe::string_view phoneNumber, ::taihe::string_view countryCode)
{
    CallManagerClientInitializer init;
    std::u16string formatNumber = u"";
    std::u16string u16PhoneNumber = OHOS::Str8ToStr16(std::string(phoneNumber));
    std::u16string u16CountryCode = OHOS::Str8ToStr16(std::string(countryCode));
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumberToE164(
        u16PhoneNumber, u16CountryCode, formatNumber);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    return OHOS::Str16ToStr8(formatNumber);
}

bool IsEmergencyPhoneNumberSyncDefault(::taihe::string_view phoneNumber,
    ::ohos::telephony::call::EmergencyNumberOptions const & options)
{
    CallManagerClientInitializer init;
    bool enable = false;
    auto slotId = options.slotId.value_or(0);
    if (!IsValidSlotId(slotId)) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return enable;
    }

    std::u16string u16strPhoneNumber = OHOS::Str8ToStr16(std::string(phoneNumber));
    int32_t errorCode =
        OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->IsEmergencyPhoneNumber(
            u16strPhoneNumber, slotId, enable);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    return enable;
}

bool IsEmergencyPhoneNumberSyncOptional(::taihe::string_view phoneNumber,
    ::taihe::optional_view<::ohos::telephony::call::EmergencyNumberOptions> options)
{
    CallManagerClientInitializer init;
    int32_t slotId = 0;
    if (options.has_value()) {
        slotId = options.value().slotId.value_or(0);
    }

    bool enable = false;
    if (!IsValidSlotId(slotId)) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return enable;
    }

    std::u16string u16strPhoneNumber = OHOS::Str8ToStr16(std::string(phoneNumber));
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->IsEmergencyPhoneNumber(
        u16strPhoneNumber, slotId, enable);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    return enable;
}

bool IsEmergencyPhoneNumberSyncPhoneNumber(::taihe::string_view phoneNumber)
{
    CallManagerClientInitializer init;
    bool enable = false;
    std::u16string u16strPhoneNumber = OHOS::Str8ToStr16(std::string(phoneNumber));
    int32_t errorCode =
        OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->IsEmergencyPhoneNumber(u16strPhoneNumber, 0, enable);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    return enable;
}

void MuteRingerSync()
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->MuteRinger();
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

bool DialCallSyncDefault(::taihe::string_view phoneNumber, ::ohos::telephony::call::DialCallOptions const & options)
{
    CallManagerClientInitializer init;
    auto accountId = options.accountId.value_or(0);
    if (!IsValidSlotId(accountId)) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return false;
    }

    std::string phoneNumberString(phoneNumber.c_str());
    std::u16string tmpPhoneNumber = OHOS::Str8ToStr16(phoneNumberString);
    bool isEmergencyNumber = false;
    if (!(OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->IsEmergencyPhoneNumber(tmpPhoneNumber, accountId,
        isEmergencyNumber) == TELEPHONY_ERR_SUCCESS &&
        isEmergencyNumber)) {
        int32_t state;
        OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetVoIPCallState(state);
        if (state == (int32_t)CallStateToApp::CALL_STATE_OFFHOOK ||
            state == (int32_t)CallStateToApp::CALL_STATE_RINGING) {
            ConvertErrorForBusinessError(CALL_ERR_CALL_COUNTS_EXCEED_LIMIT);
            return false;
        }
    }

    OHOS::AppExecFwk::PacMap dialInfo;
    dialInfo.PutIntValue("accountId", accountId);
    auto videoState = options.videoState.value_or(ohos::telephony::call::VideoStateType::from_value(0)).get_value();
    dialInfo.PutIntValue("videoState", videoState);
    auto dialScene = options.dialScene.value_or(ohos::telephony::call::DialScene::from_value(0)).get_value();
    dialInfo.PutIntValue("dialScene", dialScene);
    auto dialType = options.dialType.value_or(ohos::telephony::call::DialType::from_value(0)).get_value();
    dialInfo.PutIntValue("dialType", dialType);
    if (options.extraParams.has_value()) {
        ani_env *env = ::taihe::get_env();
        auto extraParamsRef = reinterpret_cast<ani_ref>(options.extraParams.value());
        OHOS::AAFwk::WantParams wantParams;
        if (!OHOS::AppExecFwk::UnwrapWantParams(env, extraParamsRef, wantParams)) {
            ConvertErrorForBusinessError(TELEPHONY_ERR_ARGUMENT_INVALID);
            return false;
        }
        dialInfo.PutStringValue("extraParams", OHOS::AAFwk::WantParamWrapper(wantParams).ToString());
    }
    int32_t errorCode =
        OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->DialCall(
            OHOS::Str8ToStr16(phoneNumberString), dialInfo);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
        return false;
    }
    return true;
}

bool DialCallSyncOptional(::taihe::string_view phoneNumber,
    ::taihe::optional_view<::ohos::telephony::call::DialCallOptions> options)
{
    using namespace ::ohos::telephony;
    ::ohos::telephony::call::DialCallOptions opt = ::ohos::telephony::call::DialCallOptions {
        ::taihe::optional<int32_t>(std::in_place, 0),
        ::taihe::optional<call::VideoStateType>(std::in_place, call::VideoStateType::key_t::TYPE_VOICE),
        ::taihe::optional<call::DialScene>(std::in_place, call::DialScene::key_t::CALL_NORMAL),
        ::taihe::optional<call::DialType>(std::in_place, call::DialType::key_t::DIAL_CARRIER_TYPE)
    };
    if (options.has_value()) {
        opt = options.value();
    }

    return DialCallSyncDefault(phoneNumber, opt);
}

bool DialCallSyncPhoneNumber(::taihe::string_view phoneNumber)
{
    using namespace ::ohos::telephony;
    ::ohos::telephony::call::DialCallOptions opt = ::ohos::telephony::call::DialCallOptions {
        ::taihe::optional<int32_t>(std::in_place, 0),
        ::taihe::optional<call::VideoStateType>(std::in_place, call::VideoStateType::key_t::TYPE_VOICE),
        ::taihe::optional<call::DialScene>(std::in_place, call::DialScene::key_t::CALL_NORMAL),
        ::taihe::optional<call::DialType>(std::in_place, call::DialType::key_t::DIAL_CARRIER_TYPE)
    };
    return DialCallSyncDefault(phoneNumber, opt);
}

::ohos::telephony::call::CallState GetCallStateSync()
{
    CallManagerClientInitializer init;
    int32_t callState = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetCallState();
    return ::ohos::telephony::call::CallState:from_value(callState);
}

bool HasCallSync()
{
    CallManagerClientInitializer init;
    auto ret = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HasCall();
    return static_cast<ani_boolean>(ret);
}

void SendCallUiEventSync(int32_t callId, ::taihe::string_view eventName)
{
    CallManagerClientInitializer init;
    TELEPHONY_LOGD("SendCallUiEventSync enter");
    std::string name(eventName.c_str());
    auto errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SendCallUiEvent(callId, name);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void InputDialerSpecialCodeSync(::taihe::string_view inputCode)
{
    CallManagerClientInitializer init;
    std::string specialCode(inputCode.c_str());
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->InputDialerSpecialCode(specialCode);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

bool CanSetCallTransferTimeSync(int32_t slotId)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return false;
    }

    bool enabled = false;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->CanSetCallTransferTime(
        slotId, enabled);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
    return enabled;
}

::ohos::telephony::call::VoNRState GetVoNRStateSync(int32_t slotId)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return ::ohos::telephony::call::VoNRState::key_t(0);
    }

    int32_t state;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetVoNRState(slotId, state);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
        return ::ohos::telephony::call::VoNRState::key_t(0);
    }
    return ::ohos::telephony::call::VoNRState::key_t(state);
}

void RemoveMissedIncomingCallNotificationSync()
{
    CallManagerClientInitializer init;
    int32_t errorCode =
        OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->RemoveMissedIncomingCallNotification();
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void SetVoNRStateSync(int32_t slotId, ::ohos::telephony::call::VoNRState state)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }

    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetVoNRState(slotId, state);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void SetDeviceDirectionSync(int32_t callId, ::ohos::telephony::call::DeviceDirection deviceDirection)
{
    CallManagerClientInitializer init;
    int32_t rotation = deviceDirection.get_value();
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetDeviceDirection(callId, rotation);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void SetDisplaySurfaceSync(int32_t callId, ::taihe::string_view surfaceId)
{
    CallManagerClientInitializer init;
    std::string strSurfaceId(surfaceId);
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetDisplayWindow(
        callId, strSurfaceId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void SetPreviewSurfaceSync(int32_t callId, ::taihe::string_view surfaceId)
{
    CallManagerClientInitializer init;
    std::string strSurfaceId(surfaceId);
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetPreviewWindow(
        callId, strSurfaceId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void ControlCameraSync(int32_t callId, ::taihe::string_view cameraId)
{
    CallManagerClientInitializer init;
    int32_t errorCode =
        OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->ControlCamera(
            callId, OHOS::Str8ToStr16(std::string(cameraId)));
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void CancelCallUpgradeSync(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->CancelCallUpgrade(callId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void KickOutFromConferenceSync(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->KickOutFromConference(callId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void PostDialProceedSync(int32_t callId, bool proceed)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->PostDialProceed(callId, proceed);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void AnswerCallSyncDefault(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->AnswerCall(callId, 0);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void AnswerCallSyncOptional(::taihe::optional_view<int32_t> callId)
{
    CallManagerClientInitializer init;
    int32_t callIdValue = callId.value_or(static_cast<int32_t>(DEFAULT_CALL_ID));
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->AnswerCall(callIdValue, 0);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void AnswerCallSyncVoid()
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->AnswerCall(0, 0);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void AnswerCallSync(::ohos::telephony::call::VideoStateType videoState, int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t videoStateValue = videoState.get_value();
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->AnswerCall(callId, videoStateValue);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void RejectCallDefaultDefaultSync(int32_t callId, ::ohos::telephony::call::RejectMessageOptions options)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->RejectCall(
        callId, false, OHOS::Str8ToStr16(std::string(options.messageContent)));
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void RejectCallOptionalSync(
    ::taihe::optional_view<int32_t> callId,
    ::taihe::optional_view<::ohos::telephony::call::RejectMessageOptions> options)
{
    CallManagerClientInitializer init;
    int32_t callIdValue = callId.value_or(static_cast<int32_t>(DEFAULT_CALL_ID));
    std::string message = "";
    if (options.has_value()) {
        message = std::string(options.value().messageContent);
    }

    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->RejectCall(
        callIdValue, false, OHOS::Str8ToStr16(std::string(message)));
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void RejectCallWithCallIdSync(int32_t callId)
{
    CallManagerClientInitializer init;
    std::string message = "";
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->RejectCall(
        callId, false, OHOS::Str8ToStr16(std::string(message)));
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void RejectCallWithVoidSync()
{
    CallManagerClientInitializer init;
    int32_t callId = DEFAULT_CALL_ID;
    std::string message = "";
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->RejectCall(
        callId, false, OHOS::Str8ToStr16(std::string(message)));
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void RejectCallWithRejectMessageSync(::ohos::telephony::call::RejectMessageOptions options)
{
    CallManagerClientInitializer init;
    int32_t callId = DEFAULT_CALL_ID;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->RejectCall(
        callId, false, OHOS::Str8ToStr16(std::string(options.messageContent)));
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void HangUpCallSync(int32_t callId)
{
    CallManagerClientInitializer init;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HangUpCall(callId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void HangUpCallOptionalSync(::taihe::optional_view<int32_t> callId)
{
    CallManagerClientInitializer init;
    int32_t callIdValue = callId.value_or(static_cast<int32_t>(DEFAULT_CALL_ID));
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HangUpCall(callIdValue);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

void HangUpCallWithVoidSync()
{
    CallManagerClientInitializer init;
    int32_t callId = DEFAULT_CALL_ID;
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->HangUpCall(callId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        ConvertErrorForBusinessError(errorCode);
    }
}

static void GetCallWaitingStatusCallback(int32_t slotId, uintptr_t opq, ani_object& promise)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("GetCallWaitingStatusSync slotId is invalid");
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }

    TaiheCallManager::GetInstance().RegisterCallBack();
    std::shared_ptr<AniCallbackInfo> cb = std::make_shared<AniCallbackInfo>();
    if (nullptr == cb) {
        TELEPHONY_LOGE("AniCallbackInfo is null");
        ConvertErrorForBusinessError(TELEPHONY_ERR_LOCAL_PTR_NULL);
        return;
    }
    if (!cb->init(opq)) {
        TELEPHONY_LOGE("AniCallbackInfo init fail");
        ConvertErrorForBusinessError(TELEPHONY_ERR_UNINIT);
        return;
    }
    promise = cb->promise_;
    auto callback = [cb](int32_t errorCode, ::ohos::telephony::call::CallWaitingStatus status) {
        cb->AttachThread();
        AniCallUpdateAsyncResult::GetInstance().UpdateGetCallWaitingStatus(cb, errorCode, status);
        cb->DetachThread();
    };
    TaiheCallAbilityCallback::GetInstance().getCallWaitingStatusCallback_ = callback;

    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetCallWaiting(slotId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("GetCallWaiting return error");
        ConvertErrorForBusinessError(errorCode);
    }
}

void GetCallWaitingStatusAsync(int32_t slotId, uintptr_t opq)
{
    ani_object promise;
    GetCallWaitingStatusCallback(slotId, opq, promise);
    return;
}

uintptr_t GetCallWaitingStatusPromise(int32_t slotId)
{
    ani_object promise;
    GetCallWaitingStatusCallback(slotId, 0, promise);
    return reinterpret_cast<uintptr_t>(promise);
}

static void SetCallWaitingCallback(int32_t slotId, bool activate, uintptr_t opq, ani_object& promise)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("SetCallWaitingSync slotId is invalid");
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }

    TaiheCallManager::GetInstance().RegisterCallBack();
    std::shared_ptr<AniCallbackInfo> cb = std::make_shared<AniCallbackInfo>();
    if (nullptr == cb) {
        TELEPHONY_LOGE("AniCallbackInfo is null");
        ConvertErrorForBusinessError(TELEPHONY_ERR_LOCAL_PTR_NULL);
        return;
    }
    if (!cb->init(opq)) {
        TELEPHONY_LOGE("AniCallbackInfo init fail");
        ConvertErrorForBusinessError(TELEPHONY_ERR_UNINIT);
        return;
    }
    promise = cb->promise_;
    auto callback = [cb](int32_t errorCode) {
        cb->AttachThread();
        AniCallUpdateAsyncResult::GetInstance().UpdateUndefinedCallState(cb, errorCode);
        cb->DetachThread();
    };
    TaiheCallAbilityCallback::GetInstance().setCallWatingStatusCallback_ = callback;

    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetCallWaiting(slotId, activate);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SetCallWaiting return error");
        ConvertErrorForBusinessError(errorCode);
    }
}

void SetCallWaitingAsync(int32_t slotId, bool activate, uintptr_t opq)
{
    ani_object promise;
    SetCallWaitingCallback(slotId, activate, opq, promise);
    return;
}

uintptr_t SetCallWaitingPromise(int32_t slotId, bool activate)
{
    ani_object promise;
    SetCallWaitingCallback(slotId, activate, 0, promise);
    return reinterpret_cast<uintptr_t>(promise);
}

static void GetCallRestrictionStatusCallback(int32_t slotId, ::ohos::telephony::call::CallRestrictionType type,
    uintptr_t opq, ani_object& promise)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("GetCallRestrictionStatusSync slotId is invalid");
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }

    TaiheCallManager::GetInstance().RegisterCallBack();
    std::shared_ptr<AniCallbackInfo> cb = std::make_shared<AniCallbackInfo>();
    if (nullptr == cb) {
        TELEPHONY_LOGE("AniCallbackInfo is null");
        ConvertErrorForBusinessError(TELEPHONY_ERR_LOCAL_PTR_NULL);
        return;
    }
    if (!cb->init(opq)) {
        TELEPHONY_LOGE("AniCallbackInfo init fail");
        ConvertErrorForBusinessError(TELEPHONY_ERR_UNINIT);
        return;
    }
    promise = cb->promise_;
    auto callback = [cb](int32_t errorCode, ::ohos::telephony::call::RestrictionStatus status) {
        cb->AttachThread();
        AniCallUpdateAsyncResult::GetInstance().UpdateGetCallRestrictionStatus(cb, errorCode, status);
        cb->DetachThread();
    };
    TaiheCallAbilityCallback::GetInstance().getCallRestrictionStatusCallback_ = callback;

    auto callRestrictionType = static_cast<CallRestrictionType>(type.get_value());
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetCallRestriction(
        slotId, callRestrictionType);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("GetCallRestriction return error");
        ConvertErrorForBusinessError(errorCode);
    }

    return;
}

void GetCallRestrictionStatusAsync(int32_t slotId, ::ohos::telephony::call::CallRestrictionType type, uintptr_t opq)
{
    ani_object promise;
    GetCallRestrictionStatusCallback(slotId, type, opq, promise);
    return;
}

uintptr_t GetCallRestrictionStatusPromise(int32_t slotId, ::ohos::telephony::call::CallRestrictionType type)
{
    ani_object promise;
    GetCallRestrictionStatusCallback(slotId, type, 0, promise);
    return reinterpret_cast<uintptr_t>(promise);
}

static void SetCallRestrictionCallback(int32_t slotId, ::ohos::telephony::call::CallRestrictionInfo const & info,
    uintptr_t opq, ani_object& promise)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("SetCallRestrictionSync slotId is invalid");
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }

    TaiheCallManager::GetInstance().RegisterCallBack();
    std::shared_ptr<AniCallbackInfo> cb = std::make_shared<AniCallbackInfo>();
    if (nullptr == cb) {
        TELEPHONY_LOGE("AniCallbackInfo is null");
        ConvertErrorForBusinessError(TELEPHONY_ERR_LOCAL_PTR_NULL);
        return;
    }
    if (!cb->init(opq)) {
        TELEPHONY_LOGE("AniCallbackInfo init fail");
        ConvertErrorForBusinessError(TELEPHONY_ERR_UNINIT);
        return;
    }
    promise = cb->promise_;
    auto callback = [cb](int32_t errorCode) {
        cb->AttachThread();
        AniCallUpdateAsyncResult::GetInstance().UpdateUndefinedCallState(cb, errorCode);
        cb->DetachThread();
    };
    TaiheCallAbilityCallback::GetInstance().setCallRestrictionStatusCallback_ = callback;

    CallRestrictionInfo infores;
    infores.mode = static_cast<OHOS::Telephony::CallRestrictionMode>(info.mode.get_value());
    std::copy(info.password.begin(), info.password.end(), infores.password);
    infores.fac = static_cast<OHOS::Telephony::CallRestrictionType>(info.type.get_value());
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetCallRestriction(slotId, infores);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SetCallRestriction return error");
        ConvertErrorForBusinessError(errorCode);
    }
}

void SetCallRestrictionAsync(int32_t slotId, ::ohos::telephony::call::CallRestrictionInfo const & info, uintptr_t opq)
{
    ani_object promise;
    SetCallRestrictionCallback(slotId, info, opq, promise);
    return;
}

uintptr_t SetCallRestrictionPromise(int32_t slotId, ::ohos::telephony::call::CallRestrictionInfo const & info)
{
    ani_object promise;
    SetCallRestrictionCallback(slotId, info, 0, promise);
    return reinterpret_cast<uintptr_t>(promise);
}

static void SetCallRestrictionPasswordCallback(int32_t slotId, ::taihe::string_view oldPassword,
    ::taihe::string_view newPassword, uintptr_t opq, ani_object& promise)
{
    CallManagerClientInitializer init;
    if (oldPassword.size() > kMaxNumberLen || newPassword.size() > kMaxNumberLen) {
        TELEPHONY_LOGE("SetCallRestrictionPasswordSync password too long");
        ConvertErrorForBusinessError(TELEPHONY_ERR_ARGUMENT_INVALID);
        return;
    }

    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("SetCallRestrictionPasswordSync slotId is invalid");
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }

    TaiheCallManager::GetInstance().RegisterCallBack();
    std::shared_ptr<AniCallbackInfo> cb = std::make_shared<AniCallbackInfo>();
    if (nullptr == cb) {
        TELEPHONY_LOGE("AniCallbackInfo is null");
        ConvertErrorForBusinessError(TELEPHONY_ERR_LOCAL_PTR_NULL);
        return;
    }
    if (!cb->init(opq)) {
        TELEPHONY_LOGE("AniCallbackInfo init fail");
        ConvertErrorForBusinessError(TELEPHONY_ERR_UNINIT);
        return;
    }
    promise = cb->promise_;
    auto callback = [cb](int32_t errorCode) {
        cb->AttachThread();
        AniCallUpdateAsyncResult::GetInstance().UpdateUndefinedCallState(cb, errorCode);
        cb->DetachThread();
    };
    TaiheCallAbilityCallback::GetInstance().setCallRestrictionPasswordCallback_ = callback;

    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetCallRestrictionPassword(slotId,
        CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS, oldPassword.c_str(), newPassword.c_str());
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SetCallRestrictionPassword return error");
        ConvertErrorForBusinessError(errorCode);
    }
}

void SetCallRestrictionPasswordAsync(int32_t slotId, ::taihe::string_view oldPassword,
    ::taihe::string_view newPassword, uintptr_t opq)
{
    ani_object promise;
    SetCallRestrictionPasswordCallback(slotId, oldPassword, newPassword, opq, promise);
    return;
}

uintptr_t SetCallRestrictionPasswordPromise(int32_t slotId, ::taihe::string_view oldPassword,
    ::taihe::string_view newPassword)
{
    ani_object promise;
    SetCallRestrictionPasswordCallback(slotId, oldPassword, newPassword, 0, promise);
    return reinterpret_cast<uintptr_t>(promise);
}

static void GetCallTransferInfoCallback(int32_t slotId, ::ohos::telephony::call::CallTransferType type,
    uintptr_t opq, ani_object& promise)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("GetCallTransferInfoSync slotId is invalid");
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }

    TaiheCallManager::GetInstance().RegisterCallBack();
    std::shared_ptr<AniCallbackInfo> cb = std::make_shared<AniCallbackInfo>();
    if (nullptr == cb) {
        TELEPHONY_LOGE("AniCallbackInfo is null");
        ConvertErrorForBusinessError(TELEPHONY_ERR_LOCAL_PTR_NULL);
        return;
    }
    if (!cb->init(opq)) {
        TELEPHONY_LOGE("AniCallbackInfo init fail");
        ConvertErrorForBusinessError(TELEPHONY_ERR_UNINIT);
        return;
    }
    promise = cb->promise_;
    auto callback = [cb](int32_t errorCode, ::ohos::telephony::call::CallTransferResult info) {
        cb->AttachThread();
        AniCallUpdateAsyncResult::GetInstance().UpdateGetCallTransferInfo(cb, errorCode, info);
        cb->DetachThread();
    };
    TaiheCallAbilityCallback::GetInstance().getCallTransferInfoCallback_ = callback;

    CallTransferType callTranferType = static_cast<CallTransferType>(type.get_value());
    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->GetCallTransferInfo(
        slotId, callTranferType);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("GetCallTransferInfo return error");
        ConvertErrorForBusinessError(errorCode);
    }

    return;
}

void GetCallTransferInfoAsync(int32_t slotId, ::ohos::telephony::call::CallTransferType type, uintptr_t opq)
{
    ani_object promise;
    GetCallTransferInfoCallback(slotId, type, opq, promise);
    return;
}

uintptr_t GetCallTransferInfoPromise(int32_t slotId, ::ohos::telephony::call::CallTransferType type)
{
    ani_object promise;
    GetCallTransferInfoCallback(slotId, type, 0, promise);
    return reinterpret_cast<uintptr_t>(promise);
}

static void SetCallTransferCallback(int32_t slotId, ::ohos::telephony::call::CallTransferInfo const & info,
    uintptr_t opq, ani_object& promise)
{
    CallManagerClientInitializer init;

    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("SetCallTransferSync slotId is invalid");
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }

    CallTransferInfo information;
    information.endHour = info.endHour.value_or(0);
    information.startHour = info.startHour.value_or(0);
    information.startMinute = info.startMinute.value_or(0);
    information.endMinute = info.endMinute.value_or(0);
    information.settingType = static_cast<OHOS::Telephony::CallTransferSettingType>(info.settingType.get_value());
    information.type = static_cast<OHOS::Telephony::CallTransferType>(info.type.get_value());
    if (memcpy_s(information.transferNum, kMaxNumberLen, info.transferNum.data(), info.transferNum.size()) != EOK) {
        TELEPHONY_LOGE("memcpy_s information.transferNum failed");
        ConvertErrorForBusinessError(TELEPHONY_ERR_ARGUMENT_INVALID);
        return;
    }

    TaiheCallManager::GetInstance().RegisterCallBack();
    std::shared_ptr<AniCallbackInfo> cb = std::make_shared<AniCallbackInfo>();
    if (nullptr == cb) {
        TELEPHONY_LOGE("AniCallbackInfo is null");
        ConvertErrorForBusinessError(TELEPHONY_ERR_LOCAL_PTR_NULL);
        return;
    }
    if (!cb->init(opq)) {
        TELEPHONY_LOGE("AniCallbackInfo init fail");
        ConvertErrorForBusinessError(TELEPHONY_ERR_UNINIT);
        return;
    }
    promise = cb->promise_;
    auto callback = [cb](int32_t errorCode) {
        cb->AttachThread();
        AniCallUpdateAsyncResult::GetInstance().UpdateUndefinedCallState(cb, errorCode);
        cb->DetachThread();
    };
    TaiheCallAbilityCallback::GetInstance().setCallTransferCallback_ = callback;

    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->SetCallTransferInfo(
        slotId, information);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SetCallTransferInfo return error");
        ConvertErrorForBusinessError(errorCode);
    }
}

void SetCallTransferAsync(int32_t slotId, ::ohos::telephony::call::CallTransferInfo const & info, uintptr_t opq)
{
    ani_object promise;
    SetCallTransferCallback(slotId, info, opq, promise);
    return;
}

uintptr_t SetCallTransferPromise(int32_t slotId, ::ohos::telephony::call::CallTransferInfo const & info)
{
    ani_object promise;
    SetCallTransferCallback(slotId, info, 0, promise);
    return reinterpret_cast<uintptr_t>(promise);
}

static void CloseUnfinishedUssdCallback(int32_t slotId, uintptr_t opq, ani_object& promise)
{
    CallManagerClientInitializer init;
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("CloseUnfinishedUssdSync slotId is invalid");
        ConvertErrorForBusinessError(TELEPHONY_ERR_SLOTID_INVALID);
        return;
    }

    TaiheCallManager::GetInstance().RegisterCallBack();
    std::shared_ptr<AniCallbackInfo> cb = std::make_shared<AniCallbackInfo>();
    if (nullptr == cb) {
        TELEPHONY_LOGE("AniCallbackInfo is null");
        ConvertErrorForBusinessError(TELEPHONY_ERR_LOCAL_PTR_NULL);
        return;
    }
    if (!cb->init(opq)) {
        TELEPHONY_LOGE("AniCallbackInfo init fail");
        ConvertErrorForBusinessError(TELEPHONY_ERR_UNINIT);
        return;
    }
    promise = cb->promise_;
    auto callback = [cb](int32_t errorCode) {
        cb->AttachThread();
        AniCallUpdateAsyncResult::GetInstance().UpdateUndefinedCallState(cb, errorCode);
        cb->DetachThread();
    };
    TaiheCallAbilityCallback::GetInstance().closeUnfinishedUssdCallback_ = callback;

    int32_t errorCode = OHOS::DelayedSingleton<CallManagerClient>::GetInstance()->CloseUnFinishedUssd(slotId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("CloseUnFinishedUssd return error");
        ConvertErrorForBusinessError(errorCode);
    }
}

void CloseUnfinishedUssdAsync(int32_t slotId, uintptr_t opq)
{
    ani_object promise;
    CloseUnfinishedUssdCallback(slotId, opq, promise);
    return;
}

uintptr_t CloseUnfinishedUssdPromise(int32_t slotId)
{
    ani_object promise;
    CloseUnfinishedUssdCallback(slotId, 0, promise);
    return reinterpret_cast<uintptr_t>(promise);
}

void onCallDetailsChange(
    ::taihe::callback_view<void(::ohos::telephony::call::CallAttributeOptions const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().callStateCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::CallAttributeOptions const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void offCallDetailsChange(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::CallAttributeOptions const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().callStateCallback_.reset();
    return;
}

void onCallEventChange(::taihe::callback_view<void(::ohos::telephony::call::CallEventOptions const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().callEventCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::CallEventOptions const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void offCallEventChange(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::CallEventOptions const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().callEventCallback_.reset();
    return;
}

void onCallDisconnectedCauses(
    ::taihe::callback_view<void(::ohos::telephony::call::DisconnectedDetails const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().disconnectedCauseCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::DisconnectedDetails const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void offCallDisconnectedCauses(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::DisconnectedDetails const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().disconnectedCauseCallback_.reset();
    return;
}

void onMmiCodeResult(::taihe::callback_view<void(::ohos::telephony::call::MmiCodeResults const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().mmiCodeResultCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::MmiCodeResults const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void offMmiCodeResult(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::MmiCodeResults const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().mmiCodeResultCallback_.reset();
    return;
}

void onAudioDeviceChange(
    ::taihe::callback_view<void(::ohos::telephony::call::AudioDeviceCallbackInfo const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().audioDeviceCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::AudioDeviceCallbackInfo const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void offAudioDeviceChange(::taihe::optional_view<
    ::taihe::callback<void(::ohos::telephony::call::AudioDeviceCallbackInfo const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().audioDeviceCallback_.reset();
    return;
}

void onPostDialDelay(::taihe::callback_view<void(::taihe::string_view data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().postDialDelayCallback_ =
        ::taihe::optional<taihe::callback<void(::taihe::string_view)>>{
            std::in_place_t{},
            callback};
    return;
}

void offPostDialDelay(::taihe::optional_view<::taihe::callback<void(::taihe::string_view data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().postDialDelayCallback_.reset();
    return;
}

void onImsCallModeChange(
    ::taihe::callback_view<void(::ohos::telephony::call::ImsCallModeInfo const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().imsCallModeCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::ImsCallModeInfo const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void offImsCallModeChange(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::ImsCallModeInfo const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().imsCallModeCallback_.reset();
    return;
}

void onCallSessionEvent(::taihe::callback_view<void(::ohos::telephony::call::CallSessionEvent const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().callSessionEventCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::CallSessionEvent const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void offCallSessionEvent(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::CallSessionEvent const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().callSessionEventCallback_.reset();
    return;
}

void onPeerDimensionsChange(
    ::taihe::callback_view<void(::ohos::telephony::call::PeerDimensionsDetail const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().peerDimensionsCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::PeerDimensionsDetail const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void offPeerDimensionsChange(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::PeerDimensionsDetail const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().peerDimensionsCallback_.reset();
    return;
}

void onCameraCapabilitiesChange(
    ::taihe::callback_view<void(::ohos::telephony::call::CameraCapabilities const& data)> callback)
{
    TaiheCallManager::GetInstance().RegisterCallBack();
    TaiheCallAbilityCallback::GetInstance().cameraCapabilitiesCallback_ =
        ::taihe::optional<taihe::callback<void(::ohos::telephony::call::CameraCapabilities const&)>>{
            std::in_place_t{},
            callback};
    return;
}

void offCameraCapabilitiesChange(
    ::taihe::optional_view<::taihe::callback<void(::ohos::telephony::call::CameraCapabilities const& data)>> callback)
{
    TaiheCallAbilityCallback::GetInstance().cameraCapabilitiesCallback_.reset();
    return;
}

}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_MakeCallSync(MakeCallSync);
TH_EXPORT_CPP_API_MakeCallSync2(MakeCallSync2);
TH_EXPORT_CPP_API_HasVoiceCapability(HasVoiceCapability);
TH_EXPORT_CPP_API_FormatPhoneNumberSync(FormatPhoneNumberSync);
TH_EXPORT_CPP_API_FormatPhoneNumberSync2(FormatPhoneNumberSync2);
TH_EXPORT_CPP_API_GetCallStateSync(GetCallStateSync);
TH_EXPORT_CPP_API_HasCallSync(HasCallSync);
TH_EXPORT_CPP_API_IsImsSwitchEnabledSync(IsImsSwitchEnabledSync);
TH_EXPORT_CPP_API_DisableImsSwitchSync(DisableImsSwitchSync);
TH_EXPORT_CPP_API_EnableImsSwitchSync(EnableImsSwitchSync);
TH_EXPORT_CPP_API_UpdateImsCallModeSync(UpdateImsCallModeSync);
TH_EXPORT_CPP_API_JoinConferenceSync(JoinConferenceSync);
TH_EXPORT_CPP_API_SetAudioDeviceSync(SetAudioDeviceSync);
TH_EXPORT_CPP_API_CancelMutedSync(CancelMutedSync);
TH_EXPORT_CPP_API_SetMutedSync(SetMutedSync);
TH_EXPORT_CPP_API_IsRingingSync(IsRingingSync);
TH_EXPORT_CPP_API_SeparateConferenceSync(SeparateConferenceSync);
TH_EXPORT_CPP_API_IsNewCallAllowedSync(IsNewCallAllowedSync);
TH_EXPORT_CPP_API_IsInEmergencyCallSync(IsInEmergencyCallSync);
TH_EXPORT_CPP_API_StopDTMFSync(StopDTMFSync);
TH_EXPORT_CPP_API_StartDTMFSync(StartDTMFSync);
TH_EXPORT_CPP_API_GetCallIdListForConferenceSync(GetCallIdListForConferenceSync);
TH_EXPORT_CPP_API_GetSubCallIdListSync(GetSubCallIdListSync);
TH_EXPORT_CPP_API_GetMainCallIdSync(GetMainCallIdSync);
TH_EXPORT_CPP_API_CombineConferenceSync(CombineConferenceSync);
TH_EXPORT_CPP_API_SwitchCallSync(SwitchCallSync);
TH_EXPORT_CPP_API_UnHoldCallSync(UnHoldCallSync);
TH_EXPORT_CPP_API_HoldCallSync(HoldCallSync);
TH_EXPORT_CPP_API_FormatPhoneNumberToE164Sync(FormatPhoneNumberToE164Sync);
TH_EXPORT_CPP_API_IsEmergencyPhoneNumberSyncDefault(IsEmergencyPhoneNumberSyncDefault);
TH_EXPORT_CPP_API_IsEmergencyPhoneNumberSyncOptional(IsEmergencyPhoneNumberSyncOptional);
TH_EXPORT_CPP_API_IsEmergencyPhoneNumberSyncPhoneNumber(IsEmergencyPhoneNumberSyncPhoneNumber);
TH_EXPORT_CPP_API_MuteRingerSync(MuteRingerSync);
TH_EXPORT_CPP_API_DialCallSyncDefault(DialCallSyncDefault);
TH_EXPORT_CPP_API_DialCallSyncOptional(DialCallSyncOptional);
TH_EXPORT_CPP_API_DialCallSyncPhoneNumber(DialCallSyncPhoneNumber);
TH_EXPORT_CPP_API_SendCallUiEventSync(SendCallUiEventSync);
TH_EXPORT_CPP_API_InputDialerSpecialCodeSync(InputDialerSpecialCodeSync);
TH_EXPORT_CPP_API_CanSetCallTransferTimeSync(CanSetCallTransferTimeSync);
TH_EXPORT_CPP_API_GetVoNRStateSync(GetVoNRStateSync);
TH_EXPORT_CPP_API_RemoveMissedIncomingCallNotificationSync(RemoveMissedIncomingCallNotificationSync);
TH_EXPORT_CPP_API_SetVoNRStateSync(SetVoNRStateSync);
TH_EXPORT_CPP_API_SetDeviceDirectionSync(SetDeviceDirectionSync);
TH_EXPORT_CPP_API_SetDisplaySurfaceSync(SetDisplaySurfaceSync);
TH_EXPORT_CPP_API_SetPreviewSurfaceSync(SetPreviewSurfaceSync);
TH_EXPORT_CPP_API_ControlCameraSync(ControlCameraSync);
TH_EXPORT_CPP_API_CancelCallUpgradeSync(CancelCallUpgradeSync);
TH_EXPORT_CPP_API_KickOutFromConferenceSync(KickOutFromConferenceSync);
TH_EXPORT_CPP_API_PostDialProceedSync(PostDialProceedSync);
TH_EXPORT_CPP_API_AnswerCallSyncDefault(AnswerCallSyncDefault);
TH_EXPORT_CPP_API_AnswerCallSyncOptional(AnswerCallSyncOptional);
TH_EXPORT_CPP_API_AnswerCallSyncVoid(AnswerCallSyncVoid);
TH_EXPORT_CPP_API_AnswerCallSync(AnswerCallSync);
TH_EXPORT_CPP_API_RejectCallDefaultDefaultSync(RejectCallDefaultDefaultSync);
TH_EXPORT_CPP_API_RejectCallOptionalSync(RejectCallOptionalSync);
TH_EXPORT_CPP_API_RejectCallWithCallIdSync(RejectCallWithCallIdSync);
TH_EXPORT_CPP_API_RejectCallWithVoidSync(RejectCallWithVoidSync);
TH_EXPORT_CPP_API_RejectCallWithRejectMessageSync(RejectCallWithRejectMessageSync);
TH_EXPORT_CPP_API_HangUpCallSync(HangUpCallSync);
TH_EXPORT_CPP_API_HangUpCallOptionalSync(HangUpCallOptionalSync);
TH_EXPORT_CPP_API_HangUpCallWithVoidSync(HangUpCallWithVoidSync);

TH_EXPORT_CPP_API_GetCallWaitingStatusAsync(GetCallWaitingStatusAsync);
TH_EXPORT_CPP_API_GetCallWaitingStatusPromise(GetCallWaitingStatusPromise);
TH_EXPORT_CPP_API_SetCallWaitingAsync(SetCallWaitingAsync);
TH_EXPORT_CPP_API_SetCallWaitingPromise(SetCallWaitingPromise);
TH_EXPORT_CPP_API_GetCallRestrictionStatusAsync(GetCallRestrictionStatusAsync);
TH_EXPORT_CPP_API_GetCallRestrictionStatusPromise(GetCallRestrictionStatusPromise);
TH_EXPORT_CPP_API_SetCallRestrictionAsync(SetCallRestrictionAsync);
TH_EXPORT_CPP_API_SetCallRestrictionPromise(SetCallRestrictionPromise);
TH_EXPORT_CPP_API_SetCallRestrictionPasswordAsync(SetCallRestrictionPasswordAsync);
TH_EXPORT_CPP_API_SetCallRestrictionPasswordPromise(SetCallRestrictionPasswordPromise);
TH_EXPORT_CPP_API_GetCallTransferInfoAsync(GetCallTransferInfoAsync);
TH_EXPORT_CPP_API_GetCallTransferInfoPromise(GetCallTransferInfoPromise);
TH_EXPORT_CPP_API_SetCallTransferAsync(SetCallTransferAsync);
TH_EXPORT_CPP_API_SetCallTransferPromise(SetCallTransferPromise);
TH_EXPORT_CPP_API_CloseUnfinishedUssdAsync(CloseUnfinishedUssdAsync);
TH_EXPORT_CPP_API_CloseUnfinishedUssdPromise(CloseUnfinishedUssdPromise);

TH_EXPORT_CPP_API_onCallDetailsChange(onCallDetailsChange);
TH_EXPORT_CPP_API_offCallDetailsChange(offCallDetailsChange);
TH_EXPORT_CPP_API_onCallEventChange(onCallEventChange);
TH_EXPORT_CPP_API_offCallEventChange(offCallEventChange);
TH_EXPORT_CPP_API_onCallDisconnectedCauses(onCallDisconnectedCauses);
TH_EXPORT_CPP_API_offCallDisconnectedCauses(offCallDisconnectedCauses);
TH_EXPORT_CPP_API_onMmiCodeResult(onMmiCodeResult);
TH_EXPORT_CPP_API_offMmiCodeResult(offMmiCodeResult);
TH_EXPORT_CPP_API_onAudioDeviceChange(onAudioDeviceChange);
TH_EXPORT_CPP_API_offAudioDeviceChange(offAudioDeviceChange);
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