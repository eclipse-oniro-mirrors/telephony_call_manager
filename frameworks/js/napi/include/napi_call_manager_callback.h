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

#ifndef NAPI_CALL_MANAGER_CALLBACK_H
#define NAPI_CALL_MANAGER_CALLBACK_H

#include <cstdio>

#include "pac_map.h"

#include "call_manager_callback.h"

namespace OHOS {
namespace Telephony {
class NapiCallManagerCallback : public CallManagerCallback {
public:
    NapiCallManagerCallback() = default;
    ~NapiCallManagerCallback() = default;
    int32_t OnCallDetailsChange(const CallAttributeInfo &info) override;
    int32_t OnMeeTimeDetailsChange(const CallAttributeInfo &info) override;
    int32_t OnCallEventChange(const CallEventInfo &info) override;
    int32_t OnCallDisconnectedCause(const DisconnectedDetails &details) override;
    int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo) override;
    int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info) override;
    int32_t OnReportMmiCodeResult(const MmiCodeInfo &info) override;
    int32_t OnReportAudioDeviceChange(const AudioDeviceInfo &info) override;
    int32_t OnReportPostDialDelay(const std::string &str) override;
    int32_t OnUpdateImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo) override;
    int32_t OnCallSessionEventChange(const CallSessionEvent &callSessionEventOptions) override;
    int32_t OnPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail) override;
    int32_t OnCallDataUsageChange(const int64_t dataUsage) override;
    int32_t OnUpdateCameraCapabilities(const CameraCapabilities &cameraCapabilities) override;
};
} // namespace Telephony
} // namespace OHOS
#endif // NAPI_CALL_MANAGER_CALLBACK_H
