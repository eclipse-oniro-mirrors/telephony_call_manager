/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef CALL_ABILITY_CALLBACK_STUB_H
#define CALL_ABILITY_CALLBACK_STUB_H

#include <map>

#include "iremote_object.h"
#include "iremote_stub.h"

#include "call_ability_callback_ipc_interface_code.h"
#include "i_call_ability_callback.h"

namespace OHOS {
namespace Telephony {
class CallAbilityCallbackStub : public IRemoteStub<ICallAbilityCallback> {
public:
    CallAbilityCallbackStub();
    virtual ~CallAbilityCallbackStub();
    int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using CallAbilityCallbackFunc = std::function<int32_t(MessageParcel &data, MessageParcel &reply)>;

    int32_t OnUpdateCallStateInfo(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateMeeTimeStateInfo(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateCallEvent(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateCallDisconnectedCause(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateAysncResults(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateOttCallRequest(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateMmiCodeResults(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateAudioDeviceChange(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdatePostDialDelay(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateImsCallModeChange(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateCallSessionEventChange(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdatePeerDimensionsChange(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateCallDataUsageChange(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateCameraCapabilities(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdatePhoneState(MessageParcel &data, MessageParcel &reply);

    std::map<uint32_t, CallAbilityCallbackFunc> memberFuncMap_;
};
} // namespace Telephony
} // namespace OHOS

#endif
