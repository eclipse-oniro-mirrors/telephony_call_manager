/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef CALL_STATUS_CALLBACK_STUB_H
#define CALL_STATUS_CALLBACK_STUB_H

#include <map>

#include "iremote_object.h"
#include "iremote_stub.h"

#include "call_status_callback_ipc_interface_code.h"
#include "i_call_status_callback.h"

namespace OHOS {
namespace Telephony {
class CallStatusCallbackStub : public IRemoteStub<ICallStatusCallback> {
public:
    CallStatusCallbackStub();
    virtual ~CallStatusCallbackStub();
    int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using CallStatusCallbackFunc = int32_t (CallStatusCallbackStub::*)(MessageParcel &data, MessageParcel &reply);

    int32_t OnUpdateCallReportInfo(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateCallsReportInfo(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateDisconnectedCause(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateEventReport(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateRBTPlayInfo(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateGetWaitingResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateSetWaitingResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateGetRestrictionResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateSetRestrictionResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateSetRestrictionPasswordResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateGetTransferResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateSetTransferResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateGetCallClipResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateGetCallClirResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateSetCallClirResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnStartRttResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnStopRttResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetImsConfigResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetImsConfigResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetImsFeatureValueResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetImsFeatureValueResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnReceiveImsCallModeRequest(MessageParcel &data, MessageParcel &reply);
    int32_t OnReceiveImsCallModeResponse(MessageParcel &data, MessageParcel &reply);
    int32_t OnInviteToConferenceResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnStartDtmfResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnStopDtmfResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnSendUssdResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetImsCallDataResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnSendMmiCodeResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnCloseUnFinishedUssdResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnPostDialNextChar(MessageParcel &data, MessageParcel &reply);
    int32_t OnReportPostDialDelay(MessageParcel &data, MessageParcel &reply);
    int32_t OnCallSessionEventChange(MessageParcel &data, MessageParcel &reply);
    int32_t OnPeerDimensionsChange(MessageParcel &data, MessageParcel &reply);
    int32_t OnCallDataUsageChange(MessageParcel &data, MessageParcel &reply);
    int32_t OnCameraCapabilitiesChange(MessageParcel &data, MessageParcel &reply);

    std::map<uint32_t, CallStatusCallbackFunc> memberFuncMap_;
};
} // namespace Telephony
} // namespace OHOS

#endif
