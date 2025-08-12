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
#ifndef UTIL_H
#define UTIL_H

#include "call_manager_inner_type.h"
#include "ohos.telephony.call.proj.hpp"
#include "ohos.telephony.call.impl.hpp"

namespace OHOS {
namespace Telephony {
class Utils {
public:
    static void CallAttributeInfoToTaihe(::ohos::telephony::call::CallAttributeOptions& taiheCallAttributeInfo,
        const CallAttributeInfo &info);
    static void CallEventInfoToTaihe(::ohos::telephony::call::CallEventOptions& taiheCallEventInfo,
        const CallEventInfo &info);
    static void DisconnectedDetailsToTaihe(::ohos::telephony::call::DisconnectedDetails& taiheDisconnectedDetails,
        const DisconnectedDetails &details);
    static void MmiCodeInfoToTaihe(::ohos::telephony::call::MmiCodeResults& taiheMmiCodeInfo,
        const MmiCodeInfo &info);
    static void AudioDeviceInfoToTaihe(::ohos::telephony::call::AudioDeviceCallbackInfo& taiheAudioDeviceInfo,
        const AudioDeviceInfo &info);
    static void CallMediaModeInfoToTaihe(::ohos::telephony::call::ImsCallModeInfo& taiheCallMediaModeInfo,
        const CallMediaModeInfo &info);
    static void CallSessionEventToTaihe(::ohos::telephony::call::CallSessionEvent& taiheCallSessionEvent,
        const CallSessionEvent &event);
    static void PeerDimensionsDetailToTaihe(::ohos::telephony::call::PeerDimensionsDetail& taihePeerDimensionsDetail,
        const PeerDimensionsDetail &detail);
    static void CameraCapabilitiesToTaihe(::ohos::telephony::call::CameraCapabilities& taiheCameraCapabilities,
        const CameraCapabilities &capabilities);
};
}
}
#endif