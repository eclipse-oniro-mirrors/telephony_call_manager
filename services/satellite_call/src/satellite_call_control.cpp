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


#include <satellite_call_control.h>

#include "call_control_manager.h"
#include "call_dialog.h"
#include "call_object_manager.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "notification_content.h"
#include "notification_helper.h"
#include "notification_normal_content.h"
#include "notification_request.h"
#include "settings_datashare_helper.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "audio_control_manager.h"

#include "want.h"
#include "uri.h"
#include "parameters.h"

namespace OHOS {
namespace Telephony {
const int32_t DELAYED_TIME = 270000;
const int32_t COUNT_DOWN_TIME = 30000;
const int32_t SATELLITE_CONNECTED = 1;
const int32_t SATELLITE_MODE_ON = 1;
constexpr const char *SATELLITE_TYPE_DEFAULT_VALUE = "0";
constexpr const char *TEL_SATELLITE_SUPPORT_TYPE = "const.telephony.satellite.support_type";

SatelliteCallControl::SatelliteCallControl() = default;

SatelliteCallControl::~SatelliteCallControl() = default;

void SatelliteCallControl::SetSatcommTempLevel(int32_t level)
{
    if ((SatCommTempLevel)level == SatCommTempLevel_) {
        return;
    } else {
        if ((SatCommTempLevel)level == SatCommTempLevel::TEMP_LEVEL_HIGH &&
            CallObjectManager::HasSatelliteCallExist()) {
            DelayedSingleton<CallDialog>::GetInstance()->DialogConnectExtension("SATELLITE_CALL_DISCONNECT");
            SetShowDialog(true);
        }
        SatCommTempLevel_ = (SatCommTempLevel)level;
    }
}

int32_t SatelliteCallControl::IsAllowedSatelliteDialCall()
{
    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    OHOS::Uri satelliteModeUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI);
    std::string is_satellite_connected{"0"};
    int32_t err = datashareHelper->Query(satelliteModeUri, SettingsDataShareHelper::QUERY_SATELLITE_CONNECTED_KEY,
        is_satellite_connected);
    if (err == TELEPHONY_SUCCESS && stoi(is_satellite_connected) == SATELLITE_CONNECTED) {
        TELEPHONY_LOGI("satellite service is connected");
        std::string tempLevel = OHOS::system::GetParameter("persist.thermal.log.satcomm", "-1");
        if (tempLevel != "-1") {
            SetSatcommTempLevel(stoi(tempLevel));
        }
        TELEPHONY_LOGI("GetSatcommTempLevel = %{public}s", tempLevel.c_str());
        if (GetSatcommTempLevel() >= SatCommTempLevel::TEMP_LEVEL_MIDDLE) {
            DelayedSingleton<CallDialog>::GetInstance()->DialogConnectExtension("CANNOT_DIAL_SATELLITE_CALL");
            return TELEPHONY_ERROR;
        }
        SetUsedModem();
        return TELEPHONY_SUCCESS;
    } else {
        PublishSatelliteConnectEvent();
    }
    return TELEPHONY_ERROR;
}

int32_t SatelliteCallControl::IsSatelliteSwitchEnable()
{
    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    OHOS::Uri satelliteModeUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI);
    std::string is_satellite_mode_on{ "0" };
    int32_t err = datashareHelper->Query(satelliteModeUri, SettingsDataShareHelper::QUERY_SATELLITE_MODE_KEY,
        is_satellite_mode_on);
    if (err == TELEPHONY_SUCCESS && stoi(is_satellite_mode_on) == SATELLITE_MODE_ON) {
        TELEPHONY_LOGI("satellite mode on");
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERROR;
}

SatCommTempLevel SatelliteCallControl::GetSatcommTempLevel()
{
    return SatCommTempLevel_;
}
void SatelliteCallControl::SetUsedModem()
{
    char satelliteSupportType[SYSPARA_SIZE] = { 0 };
    GetParameter(TEL_SATELLITE_SUPPORT_TYPE, SATELLITE_TYPE_DEFAULT_VALUE, satelliteSupportType, SYSPARA_SIZE);
    int modem = static_cast<unsigned int>(std::atoi(satelliteSupportType));
    TELEPHONY_LOGI("satellite modem = %{public}d." modem);
    if (modem & 0b00000100 || modem & 0b00010000) {
        std::vector<std::pair<std::string, std::string>> vec = {
            std::pair<std::string, std::string>("USEDMODEM", "satemodem")
        };
        AudioStandard::AudioSystemManager::GetInstance()->SetExtraParameters("mmi",
            vec);
    }
}

void SatelliteCallControl::HandleSatelliteCallStateUpdate(sptr<CallBase> &call,
    TelCallState priorState, TelCallState nextState)
{
    bool isDisconnected = (nextState == TelCallState::CALL_STATUS_DISCONNECTED
        || nextState == TelCallState::CALL_STATUS_DISCONNECTING) ? true : false;
    if (isDisconnected && IsShowDialog()) {
        RemoveCallCountDownEventHandlerTask();
        SetShowDialog(false);
    }
    if (nextState == TelCallState::CALL_STATUS_INCOMING || nextState == TelCallState::CALL_STATUS_WAITING) {
        SetUsedModem();
    }
    if (nextState == TelCallState::CALL_STATUS_ACTIVE) {
        if (GetSatcommTempLevel() == SatCommTempLevel::TEMP_LEVEL_HIGH &&
            CallObjectManager::HasSatelliteCallExist()) {
            DelayedSingleton<CallDialog>::GetInstance()->DialogConnectExtension("SATELLITE_CALL_DISCONNECT");
            SetShowDialog(true);
        }
    }
    std::string model = OHOS::system::GetParameter("const.build.product", "0");
    if (model != "") {
        return;
    }
    if (nextState == TelCallState::CALL_STATUS_ACTIVE && !IsShowDialog()) {
        SetSatelliteCallDurationProcessing();
    }
    if (isDisconnected) {
        RemoveCallDurationEventHandlerTask();
    }
}

bool SatelliteCallControl::IsShowDialog()
{
    return isShowingDialog_;
}

void SatelliteCallControl::SetShowDialog(bool isShowDialog)
{
    isShowingDialog_ = isShowDialog;
    if (isShowingDialog_) {
        SetSatelliteCallCountDownProcessing();
    }
}

void SatelliteCallControl::PublishSatelliteConnectEvent()
{
    AAFwk::Want want;
    want.SetAction("event.custom.satellite.ACTION_SATELLITE_CONNECT");
    EventFwk::CommonEventData data;
    data.SetWant(want);
    EventFwk::CommonEventPublishInfo publishInfo;
    std::vector<std::string> callPermissions;
    callPermissions.emplace_back(Permission::SET_TELEPHONY_STATE);
    publishInfo.SetSubscriberPermissions(callPermissions);
    bool result = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGI("publish satellite call event result : %{public}d.", result);
}

int32_t SatelliteCallControl::SetSatelliteCallDurationProcessing()
{
    TELEPHONY_LOGI("start satellite call duration task");
    auto runner = AppExecFwk::EventRunner::Create("start_satellite_call_duration_dialog");
    if (CallDurationLimitHandler_ == nullptr) {
        CallDurationLimitHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }
    if (CallDurationLimitHandler_ == nullptr) {
        TELEPHONY_LOGI("CallDurationLimitHandler_ is nullptr");
        return TELEPHONY_ERROR;
    }
    auto task = [this]() {
        if (!IsShowDialog()) {
            DelayedSingleton<CallDialog>::GetInstance()->DialogConnectExtension("SATELLITE_CALL_DURATION_LIMIT");
            SetShowDialog(true);
        }
    };
    CallDurationLimitHandler_->RemoveTask("start_satellite_call_duration_dialog");
    bool ret = CallDurationLimitHandler_->PostTask(task, "start_satellite_call_duration_dialog", DELAYED_TIME);
    if (ret) {
        TELEPHONY_LOGI("start satellite call duration task failed");
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

int32_t SatelliteCallControl::SetSatelliteCallCountDownProcessing()
{
        TELEPHONY_LOGI("start satellite call count down task");
    auto runner = AppExecFwk::EventRunner::Create("satellite_call_count_down");
    if (CallCountDownHandler_ == nullptr) {
        CallCountDownHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }
    if (CallCountDownHandler_ == nullptr) {
        TELEPHONY_LOGI("CallCountDownHandler_ is nullptr");
        return TELEPHONY_ERROR;
    }
    auto task = []() {
        std::list<int32_t> satelliteCallIdList;
        int32_t ret = CallObjectManager::GetSatelliteCallList(satelliteCallIdList);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGI("GetSatelliteCallList failed");
            return;
        }
        for (auto satelliteCallId : satelliteCallIdList) {
            sptr<CallBase> satelliteCall = CallObjectManager::GetOneCallObject(satelliteCallId);
            if (satelliteCall->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING ||
                satelliteCall->GetTelCallState() == TelCallState::CALL_STATUS_WAITING) {
                DelayedSingleton<CallControlManager>::GetInstance()->RejectCall(satelliteCallId,
                    true, u"satellitecalldurationlimit");
            } else if (satelliteCall->GetTelCallState() == TelCallState::CALL_STATUS_ACTIVE) {
                DelayedSingleton<CallControlManager>::GetInstance()->HangUpCall(satelliteCallId);
            }
        }
    };
    CallCountDownHandler_->RemoveTask("satellite_call_count_down");
    bool ret = CallCountDownHandler_->PostTask(task, "satellite_call_count_down", COUNT_DOWN_TIME);
    if (ret) {
        TELEPHONY_LOGI("start satellite call count down task failed");
        return TELEPHONY_ERROR;
    }
    return TELEPHONY_SUCCESS;
}

void SatelliteCallControl::RemoveCallDurationEventHandlerTask()
{
    if (CallDurationLimitHandler_ != nullptr) {
        CallDurationLimitHandler_->RemoveTask("start_satellite_call_duration_dialog");
    }
}

void SatelliteCallControl::RemoveCallCountDownEventHandlerTask()
{
    if (CallCountDownHandler_ != nullptr) {
        CallCountDownHandler_->RemoveTask("satellite_call_count_down");
    }
}
} // namespace Telephony
} // namespace OHOSss