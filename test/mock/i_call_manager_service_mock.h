/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#ifndef I_CALL_MANAGER_SERVICE_MOCK_H
#define I_CALL_MANAGER_SERVICE_MOCK_H

#include <gmock/gmock.h>
#include "i_call_manager_service.h"

namespace OHOS {
namespace Telephony {
class MockICallManagerService : public IRemoteStub<ICallManagerService> {
public:
    MOCK_METHOD1(RegisterCallBack, int32_t(const sptr<ICallAbilityCallback> &callback));
    MOCK_METHOD0(UnRegisterCallBack, int32_t());
    MOCK_METHOD0(ObserverOnCallDetailsChange, int32_t());
    MOCK_METHOD2(DialCall, int32_t(std::u16string number, AppExecFwk::PacMap &extras));
    MOCK_METHOD1(MakeCall, int32_t(std::string number));
    MOCK_METHOD3(AnswerCall, int32_t(int32_t callId, int32_t videoState, bool isRTT));
    MOCK_METHOD3(RejectCall, int32_t(int32_t callId, bool rejectWithMessage, std::u16string textMessage));
    MOCK_METHOD1(HangUpCall, int32_t(int32_t callId));
    MOCK_METHOD0(GetCallState, int32_t());
    MOCK_METHOD1(HoldCall, int32_t(int32_t callId));
    MOCK_METHOD1(UnHoldCall, int32_t(int32_t callId));
    MOCK_METHOD1(SwitchCall, int32_t(int32_t callId));
    MOCK_METHOD1(HasCall, bool(const bool isInCludeVoipCall));
    MOCK_METHOD1(IsNewCallAllowed, int32_t(bool &enabled));
    MOCK_METHOD1(SetMuted, int32_t(bool isMute));
    MOCK_METHOD0(MuteRinger, int32_t());
    MOCK_METHOD1(SetAudioDevice, int32_t(const AudioDevice &audioDevice));
    MOCK_METHOD1(IsRinging, int32_t(bool &enabled));
    MOCK_METHOD1(IsInEmergencyCall, int32_t(bool &enabled));
    MOCK_METHOD2(StartDtmf, int32_t(int32_t callId, char str));
    MOCK_METHOD1(StopDtmf, int32_t(int32_t callId));
    MOCK_METHOD2(PostDialProceed, int32_t(int32_t callId, bool proceed));
    MOCK_METHOD1(GetCallWaiting, int32_t(int32_t slotId));
    MOCK_METHOD2(SetCallWaiting, int32_t(int32_t slotId, bool activate));
    MOCK_METHOD2(GetCallRestriction, int32_t(int32_t slotId, CallRestrictionType type));
    MOCK_METHOD2(SetCallRestriction, int32_t(int32_t slotId, CallRestrictionInfo &info));
    MOCK_METHOD4(SetCallRestrictionPassword,
        int32_t(int32_t slotId, CallRestrictionType fac, const char *oldPassword, const char *newPassword));
    MOCK_METHOD2(GetCallTransferInfo, int32_t(int32_t slotId, CallTransferType type));
    MOCK_METHOD2(SetCallTransferInfo, int32_t(int32_t slotId, CallTransferInfo &info));
    MOCK_METHOD2(CanSetCallTransferTime, int32_t(int32_t slotId, bool &result));
    MOCK_METHOD1(CombineConference, int32_t(int32_t mainCallId));
    MOCK_METHOD1(SeparateConference, int32_t(int32_t callId));
    MOCK_METHOD1(KickOutFromConference, int32_t(int32_t callId));
    MOCK_METHOD3(IsEmergencyPhoneNumber, int32_t(std::u16string &number, int32_t slotId, bool &enabled));
    MOCK_METHOD3(FormatPhoneNumber,
        int32_t(std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber));
    MOCK_METHOD3(FormatPhoneNumberToE164,
        int32_t(std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber));
    MOCK_METHOD2(GetMainCallId, int32_t(int32_t callId, int32_t &mainCallId));
    MOCK_METHOD2(GetSubCallIdList, int32_t(int32_t callId, std::vector<std::u16string> &callIdList));
    MOCK_METHOD2(GetCallIdListForConference, int32_t(int32_t callId, std::vector<std::u16string> &callIdList));
    MOCK_METHOD2(ControlCamera, int32_t(int32_t callId, std::u16string &cameraId));
    MOCK_METHOD3(SetPreviewWindow, int32_t(int32_t callId, std::string &surfaceId, sptr<Surface> surface));
    MOCK_METHOD3(SetDisplayWindow, int32_t(int32_t callId, std::string &surfaceId, sptr<Surface> surface));
    MOCK_METHOD1(SetCameraZoom, int32_t(float zoomRatio));
    MOCK_METHOD2(SetPausePicture, int32_t(int32_t callId, std::u16string &path));
    MOCK_METHOD2(SetDeviceDirection, int32_t(int32_t callId, int32_t rotation));
    MOCK_METHOD2(SetCallPreferenceMode, int32_t(int32_t slotId, int32_t mode));
    MOCK_METHOD2(GetImsConfig, int32_t(int32_t slotId, ImsConfigItem item));
    MOCK_METHOD3(SetImsConfig, int32_t(int32_t slotId, ImsConfigItem item, std::u16string &value));
    MOCK_METHOD2(GetImsFeatureValue, int32_t(int32_t slotId, FeatureType type));
    MOCK_METHOD3(SetImsFeatureValue, int32_t(int32_t slotId, FeatureType type, int32_t value));
    MOCK_METHOD2(UpdateImsCallMode, int32_t(int32_t callId, ImsCallMode mode));
    MOCK_METHOD1(EnableImsSwitch, int32_t(int32_t slotId));
    MOCK_METHOD1(DisableImsSwitch, int32_t(int32_t slotId));
    MOCK_METHOD2(IsImsSwitchEnabled, int32_t(int32_t slotId, bool &enabled));
    MOCK_METHOD2(SetVoNRState, int32_t(int32_t slotId, int32_t state));
    MOCK_METHOD2(GetVoNRState, int32_t(int32_t slotId, int32_t &state));
    MOCK_METHOD2(JoinConference, int32_t(int32_t callId, std::vector<std::u16string> &numberList));
    MOCK_METHOD1(ReportOttCallDetailsInfo, int32_t(std::vector<OttCallDetailsInfo> &ottVec));
    MOCK_METHOD1(ReportOttCallEventInfo, int32_t(OttCallEventInfo &eventInfo));
    MOCK_METHOD1(CloseUnFinishedUssd, int32_t(int32_t slotId));
    MOCK_METHOD1(InputDialerSpecialCode, int32_t(const std::string &specialCode));
    MOCK_METHOD0(RemoveMissedIncomingCallNotification, int32_t());
    MOCK_METHOD1(SetVoIPCallState, int32_t(int32_t state));
    MOCK_METHOD1(GetVoIPCallState, int32_t(int32_t &state));
    MOCK_METHOD3(SetVoIPCallInfo, int32_t(const int32_t callId, const int32_t state, const std::string phoneNumber));
    MOCK_METHOD3(GetVoIPCallInfo, int32_t(int32_t &callId, int32_t &state, std::string &phoneNumber));
    MOCK_METHOD1(GetProxyObjectPtr, sptr<IRemoteObject>(CallManagerProxyType proxyType));
    MOCK_METHOD0(ReportAudioDeviceInfo, int32_t());
    MOCK_METHOD1(CancelCallUpgrade, int32_t(int32_t callId));
    MOCK_METHOD1(RequestCameraCapabilities, int32_t(int32_t callId));
    MOCK_METHOD0(RegisterVoipCallManagerCallback, int32_t());
    MOCK_METHOD0(UnRegisterVoipCallManagerCallback, int32_t());
    MOCK_METHOD2(SendCallUiEvent, int32_t(int32_t callId, std::string &eventName));
    MOCK_METHOD1(SetRegMmiCodeCallbackState, int32_t(bool isReg));
    MOCK_METHOD1(PreloadCallUi, int32_t(bool enable));
    MOCK_METHOD2(RegisterBluetoothCallManagerCallbackPtr, sptr<ICallStatusCallback>(int32_t phoneIndex,
        std::string &macAddress));
    MOCK_METHOD2(SendUssdResponse, int32_t(int32_t slotId, const std::string &content));
    MOCK_METHOD4(SetCallPolicyInfo,
        int32_t(bool isDialingTrustlist, const std::vector<std::string> &dialingList,
            bool isIncomingTrustlist, const std::vector<std::string> &incomingList));
    MOCK_METHOD0(EndCall, bool());
    MOCK_METHOD0(HasDistributedCommunicationCapability, bool());
    MOCK_METHOD1(NotifyVoIPAudioStreamStart, int32_t(int32_t uid));
    MOCK_METHOD2(SetCallAudioMode, int32_t(int32_t mode, int32_t scenarios));
    MOCK_METHOD0(AnswerCall, int32_t());
    MOCK_METHOD0(RejectCall, int32_t());
    MOCK_METHOD0(HangUpCall, int32_t());
    MOCK_METHOD2(GetCallTransferInfo, int32_t(const std::string number, CallTransferType type));
    MOCK_METHOD2(GetCallTransferInfoByNumber, int32_t(const std::string number, CallTransferType type));
    MOCK_METHOD3(MakeCallWithToken, int32_t(std::string number, AppExecFwk::PacMap &options, std::string &token));
    MOCK_METHOD2(CheckCallRecordingPermission, bool(const std::string &cellularRecordPhoneNum,
        const std::string &cellularRecordToken));
#ifdef SUPPORT_RTT_CALL
    MOCK_METHOD3(SendRttMessage, int32_t(int32_t callId, const std::string &rttMessage));
    MOCK_METHOD2(SetRttCapability, int32_t(int32_t slotId, bool enabled));
    MOCK_METHOD2(UpdateImsRttCallMode, int32_t(int32_t callId, ImsRTTCallMode mode));
#endif
#ifdef CALL_MANAGER_CALL_TRANSFER
    MOCK_METHOD1(RegisterTransferController, int32_t(const sptr<ITransferControlCallback> callback));
    MOCK_METHOD0(UnRegisterTransferController, int32_t());
    MOCK_METHOD1(NotifyTransferCallContact, int32_t(const std::string &contactName));
#endif
    MOCK_METHOD4(OnRemoteRequest,
        int32_t(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option));
};
} // namespace Telephony
} // namespace OHOS

#endif // I_CALL_MANAGER_SERVICE_MOCK_H
