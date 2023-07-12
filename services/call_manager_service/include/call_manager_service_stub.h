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

#ifndef CALL_MANAGER_SERVICE_STUB_H
#define CALL_MANAGER_SERVICE_STUB_H

#include <map>

#include "iremote_object.h"
#include "iremote_stub.h"

#include "call_manager_service_ipc_interface_code.h"
#include "i_call_manager_service.h"

namespace OHOS {
namespace Telephony {
class CallManagerServiceStub : public IRemoteStub<ICallManagerService> {
public:
    CallManagerServiceStub();
    ~CallManagerServiceStub();
    virtual int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using CallManagerServiceFunc = int32_t (CallManagerServiceStub::*)(MessageParcel &data, MessageParcel &reply);

    void InitCallBasicRequest();
    void InitCallUtilsRequest();
    void InitCallConferenceRequest();
    void InitCallDtmfRequest();
    void InitCallSupplementRequest();
    void initCallConferenceExRequest();
    void InitCallMultimediaRequest();
    void InitImsServiceRequest();
    void InitOttServiceRequest();

    int32_t OnRegisterCallBack(MessageParcel &data, MessageParcel &reply);
    int32_t OnUnRegisterCallBack(MessageParcel &data, MessageParcel &reply);
    int32_t OnDialCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnAcceptCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnRejectCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnHangUpCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetCallState(MessageParcel &data, MessageParcel &reply);
    int32_t OnHoldCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnUnHoldCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnSwitchCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnHasCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnIsNewCallAllowed(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetMute(MessageParcel &data, MessageParcel &reply);
    int32_t OnMuteRinger(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetAudioDevice(MessageParcel &data, MessageParcel &reply);
    int32_t OnIsRinging(MessageParcel &data, MessageParcel &reply);
    int32_t OnIsInEmergencyCall(MessageParcel &data, MessageParcel &reply);
    int32_t OnStartDtmf(MessageParcel &data, MessageParcel &reply);
    int32_t OnStopDtmf(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetCallWaiting(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetCallWaiting(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetCallRestriction(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetCallRestriction(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetCallRestrictionPassword(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetTransferNumber(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetTransferNumber(MessageParcel &data, MessageParcel &reply);
    int32_t OnCanSetCallTransferTime(MessageParcel &data, MessageParcel &reply);
    int32_t OnCombineConference(MessageParcel &data, MessageParcel &reply);
    int32_t OnSeparateConference(MessageParcel &data, MessageParcel &reply);
    int32_t OnJoinConference(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetCallPreferenceMode(MessageParcel &data, MessageParcel &reply);
    int32_t OnControlCamera(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetPreviewWindow(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetDisplayWindow(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetCameraZoom(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetPausePicture(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetDeviceDirection(MessageParcel &data, MessageParcel &reply);
    int32_t OnIsEmergencyPhoneNumber(MessageParcel &data, MessageParcel &reply);
    int32_t OnFormatPhoneNumber(MessageParcel &data, MessageParcel &reply);
    int32_t OnFormatPhoneNumberToE164(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetMainCallId(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetSubCallIdList(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetCallIdListForConference(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetImsConfig(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetImsConfig(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetImsFeatureValue(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetImsFeatureValue(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateCallMediaMode(MessageParcel &data, MessageParcel &reply);
    int32_t OnEnableVoLte(MessageParcel &data, MessageParcel &reply);
    int32_t OnDisableVoLte(MessageParcel &data, MessageParcel &reply);
    int32_t OnIsVoLteEnabled(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetVoNRState(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetVoNRState(MessageParcel &data, MessageParcel &reply);
    int32_t OnStartRtt(MessageParcel &data, MessageParcel &reply);
    int32_t OnStopRtt(MessageParcel &data, MessageParcel &reply);
    int32_t OnReportOttCallDetailsInfo(MessageParcel &data, MessageParcel &reply);
    int32_t OnReportOttCallEventInfo(MessageParcel &data, MessageParcel &reply);
    int32_t OnCloseUnFinishedUssd(MessageParcel &data, MessageParcel &reply);
    int32_t OnInputDialerSpecialCode(MessageParcel &data, MessageParcel &reply);
    int32_t OnRemoveMissedIncomingCallNotification(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetProxyObjectPtr(MessageParcel &data, MessageParcel &reply);
    int32_t OnReportAudioDeviceInfo(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, CallManagerServiceFunc> memberFuncMap_;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_MANAGER_SERVICE_STUB_H
