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

#ifndef I_CALL_ABILITY_CALLBACK_H
#define I_CALL_ABILITY_CALLBACK_H

#include "pac_map.h"
#include "iremote_broker.h"

#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
class ICallAbilityCallback : public IRemoteBroker {
public:
    ICallAbilityCallback() : bundleInfo_("") {}
    virtual ~ICallAbilityCallback() = default;

    virtual int32_t OnCallDetailsChange(const CallAttributeInfo &info) = 0;
    virtual int32_t OnMeeTimeDetailsChange(const CallAttributeInfo &info) = 0;
    virtual int32_t OnCallEventChange(const CallEventInfo &info) = 0;
    virtual int32_t OnCallDisconnectedCause(const DisconnectedDetails &details) = 0;
    virtual int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo) = 0;
    virtual int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info) = 0;
    virtual int32_t OnReportMmiCodeResult(const MmiCodeInfo &info) = 0;
    virtual int32_t OnReportAudioDeviceChange(const AudioDeviceInfo &info) = 0;
    virtual int32_t OnReportPostDialDelay(const std::string &str) = 0;
    virtual int32_t OnReportImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo) = 0;
    virtual int32_t OnReportCallSessionEventChange(const CallSessionEvent &callSessionEventOptions) = 0;
    virtual int32_t OnReportPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail) = 0;
    virtual int32_t OnReportCallDataUsageChange(const int64_t dataUsage) = 0;
    virtual int32_t OnReportCameraCapabilities(const CameraCapabilities &cameraCapabilities) = 0;
    virtual int32_t OnPhoneStateChange(int32_t numActive, int32_t numHeld, int32_t, callState,
        const std::string &number) = 0;

    void SetBundleInfo(const std::string &info)
    {
        bundleInfo_ = info;
    }

    std::string GetBundleInfo()
    {
        return bundleInfo_;
    }

public:
    std::string bundleInfo_;
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.ICallAbilityCallback");
};
} // namespace Telephony
} // namespace OHOS

#endif
