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

#include "interoperable_settings_handler.h"
#include "call_control_manager.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr const char *SYNERGY_INCOMING_MUTE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=synergy_incoming_mute";
constexpr const char *CALL_MUTE_RINGER_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=call_mute_ringer";
constexpr const char *SYNERGY_MUTE_KEY = "synergy_incoming_mute";
constexpr const char *CALL_MUTE_KEY = "call_mute_ringer";
constexpr const char *VALUE_DEFAULT = "0";
constexpr const char *VALUE_MUTE = "3";
sptr<InteroperableRecvObserver> InteroperableSettingsHandler::recvObserver_ = nullptr;

void InteroperableSettingsHandler::RegisterObserver()
{
    if (recvObserver_ != nullptr) {
        return;
    }
    recvObserver_ = new (std::nothrow) InteroperableRecvObserver();
    if (recvObserver_ == nullptr) {
        TELEPHONY_LOGE("Interoperable recvObserver_ is null");
        return;
    }
    Uri uri(SYNERGY_INCOMING_MUTE_URI);
    auto helper = DelayedSingleton<SettingsDataShareHelper>().GetInstance();
    if (!helper->RegisterToDataShare(uri, recvObserver_)) {
        TELEPHONY_LOGE("Register recvObserver_ failed");
    }
}

std::string InteroperableSettingsHandler::QueryMuteRinger()
{
    Uri settingsDataUri(SYNERGY_INCOMING_MUTE_URI);
    std::string muteValue;
    auto helper = DelayedSingleton<SettingsDataShareHelper>().GetInstance();
    int32_t ret = helper->Query(settingsDataUri, SYNERGY_MUTE_KEY, muteValue);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Query MuteRinger failed");
        return VALUE_DEFAULT;
    }
    TELEPHONY_LOGI("Query end muteValue is %{public}s", muteValue.c_str());
    return muteValue;
}

void InteroperableSettingsHandler::RecoverMuteRinger(std::string muteRinger)
{
    if (muteRinger == VALUE_DEFAULT) {
        return;
    }
    Uri settingsDataUri(SYNERGY_INCOMING_MUTE_URI);
    auto helper = DelayedSingleton<SettingsDataShareHelper>().GetInstance();
    if (helper->Update(settingsDataUri, SYNERGY_MUTE_KEY, VALUE_DEFAULT) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Recover MuteRinger failed");
    }
}

void InteroperableSettingsHandler::SendMuteRinger()
{
    TELEPHONY_LOGI("Send MuteRinger");
    Uri settingsDataUri(CALL_MUTE_RINGER_URI);
    auto helper = DelayedSingleton<SettingsDataShareHelper>().GetInstance();
    if (helper->Update(settingsDataUri, CALL_MUTE_KEY, VALUE_MUTE) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Send MuteRinger failed");
    }
}

void InteroperableRecvObserver::OnChange()
{
    TELEPHONY_LOGI("InteroperableRecvObserver Onchange");
    std::string muteRinger = InteroperableSettingsHandler::QueryMuteRinger();
    if (muteRinger == VALUE_MUTE) {
        DelayedSingleton<CallControlManager>::GetInstance()->MuteRinger();
        InteroperableSettingsHandler::RecoverMuteRinger(muteRinger);
    }
}
}
}