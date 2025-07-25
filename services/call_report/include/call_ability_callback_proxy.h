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

#ifndef CALL_ABILITY_CALLBACK_PROXY_H
#define CALL_ABILITY_CALLBACK_PROXY_H

#include "iremote_proxy.h"

#include "call_ability_callback_ipc_interface_code.h"
#include "i_call_ability_callback.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
class CallAbilityCallbackProxy : public IRemoteProxy<ICallAbilityCallback> {
public:
    explicit CallAbilityCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~CallAbilityCallbackProxy() = default;

    int32_t OnCallDetailsChange(const CallAttributeInfo &info) override;
    int32_t OnMeeTimeDetailsChange(const CallAttributeInfo &info) override;
    int32_t OnCallEventChange(const CallEventInfo &info) override;
    int32_t OnCallDisconnectedCause(const DisconnectedDetails &details) override;
    int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo) override;
    int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info) override;
    int32_t OnReportMmiCodeResult(const MmiCodeInfo &info) override;
    int32_t OnReportAudioDeviceChange(const AudioDeviceInfo &info) override;
    int32_t OnReportPostDialDelay(const std::string &str) override;
    int32_t OnReportImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo) override;
    int32_t OnReportCallSessionEventChange(const CallSessionEvent &callSessionEventOptions) override;
    int32_t OnReportPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail) override;
    int32_t OnReportCallDataUsageChange(const int64_t dataUsage) override;
    int32_t OnReportCameraCapabilities(const CameraCapabilities &cameraCapabilities) override;
    int32_t OnPhoneStateChange(int32_t numActive, int32_t numHeld, int32_t callState,
        const std::string &number) override;

private:
    void PackDataParcel(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo, MessageParcel &dataParcel);

private:
    static inline BrokerDelegator<CallAbilityCallbackProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS

#endif
