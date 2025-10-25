/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "taihe_call_ability_callback.h"
#include "telephony_log_wrapper.h"
#include "telephony_errors.h"
#include "util.h"

namespace OHOS {
namespace Telephony {

TaiheCallAbilityCallback &TaiheCallAbilityCallback::GetInstance()
{
    static TaiheCallAbilityCallback instance;
    return instance;
}

// 通话详情变化回调
int32_t TaiheCallAbilityCallback::UpdateCallStateInfo(const CallAttributeInfo &info)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    ::ohos::telephony::call::CallAttributeOptions taiheCallAttributeInfo =
        ::ohos::telephony::call::CallAttributeOptions{
        .accountNumber = "",
        .speakerphoneOn = false,
        .accountId = -1,
        .videoState = ::ohos::telephony::call::VideoStateType::key_t::TYPE_VOICE,
        .startTime = -1,
        .isEcc = false,
        .callType = ::ohos::telephony::call::CallType::key_t::TYPE_CS,
        .callId = -1,
        .callState = ::ohos::telephony::call::DetailedCallState::key_t::CALL_STATUS_IDLE,
        .conferenceState = ::ohos::telephony::call::ConferenceState::key_t::TEL_CONFERENCE_IDLE,
        .crsType = -1,
        .originalCallType = -1,
        .numberLocation = ::taihe::optional<::taihe::string>(std::in_place_t{}, "")
    };
    Utils::CallAttributeInfoToTaihe(taiheCallAttributeInfo, info);
    if (callStateCallback_) {
        (*callStateCallback_)(taiheCallAttributeInfo);
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

// 通话事件变化回调
int32_t TaiheCallAbilityCallback::UpdateCallEventInfo(const CallEventInfo &info)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    ::ohos::telephony::call::CallEventOptions taiheCallEventInfo = ::ohos::telephony::call::CallEventOptions{
        .eventId = ::ohos::telephony::call::CallAbilityEventId::key_t::EVENT_DIAL_NO_CARRIER
    };
    Utils::CallEventInfoToTaihe(taiheCallEventInfo, info);
    if (callEventCallback_) {
        (*callEventCallback_)(taiheCallEventInfo);
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

// 通话断开原因回调
int32_t TaiheCallAbilityCallback::UpdateDisconnectedCause(const DisconnectedDetails &details)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    ::ohos::telephony::call::DisconnectedDetails taiheDisconnectedDetails =
        ::ohos::telephony::call::DisconnectedDetails{
        .reason = ::ohos::telephony::call::DisconnectedReason::key_t::UNKNOWN,
        .message = ""
    };
    Utils::DisconnectedDetailsToTaihe(taiheDisconnectedDetails, details);
    if (disconnectedCauseCallback_) {
        (*disconnectedCauseCallback_)(taiheDisconnectedDetails);
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

// MMI码结果回调
int32_t TaiheCallAbilityCallback::UpdateMmiCodeResult(const MmiCodeInfo &info)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    ::ohos::telephony::call::MmiCodeResults taiheMmiCodeInfo = ::ohos::telephony::call::MmiCodeResults{
        .result = ::ohos::telephony::call::MmiCodeResult::key_t::MMI_CODE_SUCCESS,
        .message = ""
    };
    Utils::MmiCodeInfoToTaihe(taiheMmiCodeInfo, info);
    if (mmiCodeResultCallback_) {
        (*mmiCodeResultCallback_)(taiheMmiCodeInfo);
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

// 音频设备变化回调
int32_t TaiheCallAbilityCallback::UpdateAudioDeviceInfo(const AudioDeviceInfo &info)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    ::ohos::telephony::call::AudioDeviceCallbackInfo taiheAudioDeviceInfo =
        ::ohos::telephony::call::AudioDeviceCallbackInfo{
        .audioDeviceList = ::taihe::array<::ohos::telephony::call::AudioDevice>{},
        .currentAudioDevice = ::ohos::telephony::call::AudioDevice{
            .deviceType = ::ohos::telephony::call::AudioDeviceType::key_t::DEVICE_EARPIECE,
            .address = ::taihe::optional<::taihe::string>(std::in_place_t{}, ""),
            .deviceName = ::taihe::optional<::taihe::string>(std::in_place_t{}, "")
        },
        .isMuted = false
    };
    Utils::AudioDeviceInfoToTaihe(taiheAudioDeviceInfo, info);
    if (audioDeviceCallback_) {
        (*audioDeviceCallback_)(taiheAudioDeviceInfo);
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

// 拨号后延迟回调
int32_t TaiheCallAbilityCallback::UpdatePostDialDelay(const std::string &str)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    if (postDialDelayCallback_) {
        (*postDialDelayCallback_)(::taihe::string_view(str));
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

// IMS通话模式变化回调
int32_t TaiheCallAbilityCallback::UpdateImsCallModeInfo(const CallMediaModeInfo &info)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    ::ohos::telephony::call::ImsCallModeInfo taiheCallMediaModeInfo = ::ohos::telephony::call::ImsCallModeInfo{
        .callId = -1,
        .result = ::ohos::telephony::call::VideoRequestResultType::key_t::TYPE_REQUEST_SUCCESS,
        .isRequestInfo = false,
        .imsCallMode = ::ohos::telephony::call::ImsCallMode::key_t::CALL_MODE_AUDIO_ONLY
    };
    Utils::CallMediaModeInfoToTaihe(taiheCallMediaModeInfo, info);
    if (imsCallModeCallback_) {
        (*imsCallModeCallback_)(taiheCallMediaModeInfo);
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

// 通话会话事件回调
int32_t TaiheCallAbilityCallback::UpdateCallSessionEvent(const CallSessionEvent &event)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    ::ohos::telephony::call::CallSessionEvent taiheCallSessionEvent = ::ohos::telephony::call::CallSessionEvent{
        .callId = -1,
        .eventId = ::ohos::telephony::call::CallSessionEventId::key_t::EVENT_CONTROL_CAMERA_FAILURE
    };
    Utils::CallSessionEventToTaihe(taiheCallSessionEvent, event);
    if (callSessionEventCallback_) {
        (*callSessionEventCallback_)(taiheCallSessionEvent);
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

// 对端尺寸变化回调
int32_t TaiheCallAbilityCallback::UpdatePeerDimensions(const PeerDimensionsDetail &detail)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    ::ohos::telephony::call::PeerDimensionsDetail taihePeerDimensionsDetail =
        ::ohos::telephony::call::PeerDimensionsDetail{};
    Utils::PeerDimensionsDetailToTaihe(taihePeerDimensionsDetail, detail);
    if (peerDimensionsCallback_) {
        (*peerDimensionsCallback_)(taihePeerDimensionsDetail);
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

// 相机能力变化回调
int32_t TaiheCallAbilityCallback::UpdateCameraCapabilities(const CameraCapabilities &capabilities)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    ::ohos::telephony::call::CameraCapabilities taiheCameraCapabilities = ::ohos::telephony::call::CameraCapabilities{};
    Utils::CameraCapabilitiesToTaihe(taiheCameraCapabilities, capabilities);
    if (cameraCapabilitiesCallback_) {
        (*cameraCapabilitiesCallback_)(taiheCameraCapabilities);
        return TELEPHONY_SUCCESS;
    }
    return TELEPHONY_ERR_FAIL;
}

int32_t TaiheCallAbilityCallback::UpdateAsyncResultsInfo(const CallResultReportId reportId,
    AppExecFwk::PacMap &resultInfo)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    int32_t result = TELEPHONY_ERR_FAIL;
    TELEPHONY_LOGI("UpdateAsyncResultsInfo reportId = %{public}d", reportId);
    switch (reportId) {
        case CallResultReportId::GET_CALL_WAITING_REPORT_ID: {
            result = ReportGetCallWaitingStatus(resultInfo);
            break;
        }
        case CallResultReportId::SET_CALL_WAITING_REPORT_ID: {
            result = ReportSetCallWaiting(resultInfo);
            break;
        }
        case CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID: {
            result = ReportGetCallRestrictionStatus(resultInfo);
            break;
        }
        case CallResultReportId::SET_CALL_RESTRICTION_REPORT_ID: {
            result = ReportSetCallRestriction(resultInfo);
            break;
        }
        case CallResultReportId::SET_CALL_RESTRICTION_PWD_REPORT_ID: {
            result = ReportSetCallRestrictionPassword(resultInfo);
            break;
        }
        case CallResultReportId::GET_CALL_TRANSFER_REPORT_ID: {
            result = ReportGetCallTransferInfo(resultInfo);
            break;
        }
        case CallResultReportId::SET_CALL_TRANSFER_REPORT_ID: {
            result = ReportSetCallTransfer(resultInfo);
            break;
        }
        case CallResultReportId::CLOSE_UNFINISHED_USSD_REPORT_ID: {
            result = ReportCloseUnfinishedUssd(resultInfo);
            break;
        }
        default:
            TELEPHONY_LOGI("UpdateAsyncResultsInfo reportId %{public}d is unsupport", reportId);;
    }
    return result;
}

int32_t TaiheCallAbilityCallback::ReportGetCallWaitingStatus(AppExecFwk::PacMap &resultInfo)
{
    int32_t result = resultInfo.GetIntValue("result");
    int32_t status = resultInfo.GetIntValue("status");
    auto waitingStatus = ::ohos::telephony::call::CallWaitingStatus::from_value(status);
    if (getCallWaitingStatusCallback_) {
        getCallWaitingStatusCallback_(result, waitingStatus);
    }

    return result;
}

int32_t TaiheCallAbilityCallback::ReportSetCallWaiting(AppExecFwk::PacMap &resultInfo)
{
    int32_t result = resultInfo.GetIntValue("result");
    if (setCallWatingStatusCallback_) {
        setCallWatingStatusCallback_(result);
    }

    return result;
}

int32_t TaiheCallAbilityCallback::ReportGetCallRestrictionStatus(AppExecFwk::PacMap &resultInfo)
{
    int32_t result = resultInfo.GetIntValue("result");
    int32_t status = resultInfo.GetIntValue("status");
    auto restrictionStatus = ::ohos::telephony::call::RestrictionStatus::from_value(status);
    if (getCallRestrictionStatusCallback_) {
        getCallRestrictionStatusCallback_(result, restrictionStatus);
    }

    return result;
}

int32_t TaiheCallAbilityCallback::ReportSetCallRestriction(AppExecFwk::PacMap &resultInfo)
{
    int32_t result = resultInfo.GetIntValue("result");
    if (setCallRestrictionStatusCallback_) {
        setCallRestrictionStatusCallback_(result);
    }

    return result;
}

int32_t TaiheCallAbilityCallback::ReportSetCallRestrictionPassword(AppExecFwk::PacMap &resultInfo)
{
    int32_t result = resultInfo.GetIntValue("result");
    if (setCallRestrictionPasswordCallback_) {
        setCallRestrictionPasswordCallback_(result);
    }

    return result;
}

int32_t TaiheCallAbilityCallback::ReportGetCallTransferInfo(AppExecFwk::PacMap &resultInfo)
{
    int32_t result = resultInfo.GetIntValue("result");
    int32_t status = resultInfo.GetIntValue("status");
    std::string teleNumber = resultInfo.GetStringValue("teleNumber");
    int32_t startHour = resultInfo.GetIntValue("startHour");
    int32_t endHour = resultInfo.GetIntValue("endHour");
    int32_t startMinute = resultInfo.GetIntValue("startMinute");
    int32_t endMinute = resultInfo.GetIntValue("endMinute");
    ohos::telephony::call::TransferStatus transferStatus = ohos::telephony::call::TransferStatus::from_value(status);
    ohos::telephony::call::CallTransferResult transferResult = {
        transferStatus, teleNumber, startHour, endHour, startMinute, endMinute
    };

    if (getCallTransferInfoCallback_) {
        getCallTransferInfoCallback_(result, transferResult);
    }

    return result;
}

int32_t TaiheCallAbilityCallback::ReportSetCallTransfer(AppExecFwk::PacMap &resultInfo)
{
    int32_t result = resultInfo.GetIntValue("result");
    if (setCallTransferCallback_) {
        setCallTransferCallback_(result);
    }

    return result;
}

int32_t TaiheCallAbilityCallback::ReportCloseUnfinishedUssd(AppExecFwk::PacMap &resultInfo)
{
    int32_t result = resultInfo.GetIntValue("result");
    if (closeUnfinishedUssdCallback_) {
        closeUnfinishedUssdCallback_(result);
    }

    return result;
}
} // namespace Telephony
} // namespace OHOS