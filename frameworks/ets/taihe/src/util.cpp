/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include "util.h"

namespace OHOS {
namespace Telephony {

void Utils::CallAttributeInfoToTaihe(::ohos::telephony::call::CallAttributeOptions& taiheCallAttributeInfo,
    const CallAttributeInfo &info)
{
    taiheCallAttributeInfo.accountNumber = ::taihe::string_view(info.accountNumber);
    taiheCallAttributeInfo.speakerphoneOn = info.speakerphoneOn;
    taiheCallAttributeInfo.accountId = info.accountId;
    taiheCallAttributeInfo.videoState =
        ::ohos::telephony::call::VideoStateType::from_value(static_cast<int32_t>(info.videoState));
    taiheCallAttributeInfo.startTime = info.startTime;
    taiheCallAttributeInfo.isEcc = info.isEcc;
    taiheCallAttributeInfo.callType =
        ::ohos::telephony::call::CallType::from_value(static_cast<int32_t>(info.callType));
    taiheCallAttributeInfo.callId = info.callId;
    taiheCallAttributeInfo.callState =
        ::ohos::telephony::call::DetailedCallState::from_value(static_cast<int32_t>(info.callState));
    taiheCallAttributeInfo.conferenceState =
        ::ohos::telephony::call::ConferenceState::from_value(static_cast<int32_t>(info.conferenceState));
    taiheCallAttributeInfo.crsType = info.crsType;
    taiheCallAttributeInfo.originalCallType = info.originalCallType;
    taiheCallAttributeInfo.numberLocation = ::taihe::optional<::taihe::string>(std::in_place_t{}, info.numberLocation);
}

void Utils::CallEventInfoToTaihe(::ohos::telephony::call::CallEventOptions& taiheCallEventInfo,
    const CallEventInfo &info)
{
    taiheCallEventInfo.eventId =
        ::ohos::telephony::call::CallAbilityEventId::from_value(static_cast<int32_t>(info.eventId));
}

void Utils::DisconnectedDetailsToTaihe(::ohos::telephony::call::DisconnectedDetails& taiheDisconnectedDetails,
    const DisconnectedDetails &details)
{
    taiheDisconnectedDetails.reason =
        ::ohos::telephony::call::DisconnectedReason::from_value(static_cast<int32_t>(details.reason));
    taiheDisconnectedDetails.message = ::taihe::string_view(details.message);
}

void Utils::MmiCodeInfoToTaihe(::ohos::telephony::call::MmiCodeResults& taiheMmiCodeInfo,
    const MmiCodeInfo &info)
{
    taiheMmiCodeInfo.result = ::ohos::telephony::call::MmiCodeResult::from_value(static_cast<int32_t>(info.result));
    taiheMmiCodeInfo.message = info.message;
}

void Utils::AudioDeviceInfoToTaihe(::ohos::telephony::call::AudioDeviceCallbackInfo& taiheAudioDeviceInfo,
    const AudioDeviceInfo &info)
{
    std::vector<::ohos::telephony::call::AudioDevice> taiheAudioDeviceList;
    for (auto iter = info.audioDeviceList.begin(); iter != info.audioDeviceList.end(); ++iter) {
        auto audioDevice = *iter;
        ::ohos::telephony::call::AudioDevice taiheAudioDevice = ::ohos::telephony::call::AudioDevice{
            .deviceType = ::ohos::telephony::call::AudioDeviceType::from_value(
                static_cast<int32_t>(audioDevice.deviceType)),
            .address = ::taihe::optional<::taihe::string>(std::in_place_t{}, audioDevice.address),
            .deviceName = ::taihe::optional<::taihe::string>(std::in_place_t{}, audioDevice.deviceName)
        };
        taiheAudioDeviceList.push_back(taiheAudioDevice);
    }
    taiheAudioDeviceInfo.audioDeviceList = ::taihe::array<::ohos::telephony::call::AudioDevice>{taihe::copy_data_t{},
            taiheAudioDeviceList.data(), taiheAudioDeviceList.size()};
    taiheAudioDeviceInfo.currentAudioDevice = ::ohos::telephony::call::AudioDevice{
        .deviceType = ::ohos::telephony::call::AudioDeviceType::from_value(
            static_cast<int32_t>(info.currentAudioDevice.deviceType)),
        .address = ::taihe::optional<::taihe::string>(std::in_place_t{}, info.currentAudioDevice.address),
        .deviceName = ::taihe::optional<::taihe::string>(std::in_place_t{}, info.currentAudioDevice.deviceName)
    };
    taiheAudioDeviceInfo.isMuted = info.isMuted;
}

void Utils::CallMediaModeInfoToTaihe(::ohos::telephony::call::ImsCallModeInfo& taiheCallMediaModeInfo,
    const CallMediaModeInfo &info)
{
    taiheCallMediaModeInfo.callId = info.callId;
    taiheCallMediaModeInfo.isRequestInfo = info.isRequestInfo;
    taiheCallMediaModeInfo.result =
        ::ohos::telephony::call::VideoRequestResultType::from_value(static_cast<int32_t>(info.result));
    taiheCallMediaModeInfo.imsCallMode = ::ohos::telephony::call::ImsCallMode::from_value(
        static_cast<int32_t>(info.callMode));
}

void Utils::CallSessionEventToTaihe(::ohos::telephony::call::CallSessionEvent& taiheCallSessionEvent,
    const CallSessionEvent &event)
{
    taiheCallSessionEvent.eventId =
        ::ohos::telephony::call::CallSessionEventId::from_value(static_cast<int32_t>(event.eventId));
    taiheCallSessionEvent.callId = event.callId;
}

void Utils::PeerDimensionsDetailToTaihe(::ohos::telephony::call::PeerDimensionsDetail& taihePeerDimensionsDetail,
    const PeerDimensionsDetail &detail)
{
    taihePeerDimensionsDetail.callId = detail.callId;
    taihePeerDimensionsDetail.width = detail.width;
    taihePeerDimensionsDetail.height = detail.height;
}

void Utils::CameraCapabilitiesToTaihe(::ohos::telephony::call::CameraCapabilities& taiheCameraCapabilities,
    const CameraCapabilities &capabilities)
{
    taiheCameraCapabilities.callId = capabilities.callId;
    taiheCameraCapabilities.width = capabilities.width;
    taiheCameraCapabilities.height = capabilities.height;
}

}
}