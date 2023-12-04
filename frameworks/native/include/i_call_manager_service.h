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

#ifndef I_CALL_MANAGER_SERVICE_H
#define I_CALL_MANAGER_SERVICE_H

#include <cstdio>
#include <string>
#include <vector>

#include "iremote_broker.h"
#include "surface.h"
#include "pac_map.h"

#include "call_manager_inner_type.h"
#include "i_call_ability_callback.h"

namespace OHOS {
namespace Telephony {
extern "C" {
enum CallManagerProxyType {
    PROXY_BLUETOOTH_CALL = 0,
};
} // end extern

class ICallManagerService : public IRemoteBroker {
public:
    virtual ~ICallManagerService() = default;
    virtual int32_t RegisterCallBack(const sptr<ICallAbilityCallback> &callback) = 0;
    virtual int32_t UnRegisterCallBack() = 0;
    virtual int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras) = 0;
    virtual int32_t AnswerCall(int32_t callId, int32_t videoState) = 0;
    virtual int32_t RejectCall(int32_t callId, bool rejectWithMessage, std::u16string textMessage) = 0;
    virtual int32_t HangUpCall(int32_t callId) = 0;
    virtual int32_t GetCallState() = 0;
    virtual int32_t HoldCall(int32_t callId) = 0;
    virtual int32_t UnHoldCall(int32_t callId) = 0;
    virtual int32_t SwitchCall(int32_t callId) = 0;
    virtual bool HasCall() = 0;
    virtual int32_t IsNewCallAllowed(bool &enabled) = 0;
    virtual int32_t SetMuted(bool isMute) = 0;
    virtual int32_t MuteRinger() = 0;
    virtual int32_t SetAudioDevice(const AudioDevice &audioDevice) = 0;
    virtual int32_t IsRinging(bool &enabled) = 0;
    virtual int32_t IsInEmergencyCall(bool &enabled) = 0;
    virtual int32_t StartDtmf(int32_t callId, char str) = 0;
    virtual int32_t StopDtmf(int32_t callId) = 0;
    virtual int32_t PostDialProceed(int32_t callId, bool proceed) = 0;
    virtual int32_t GetCallWaiting(int32_t slotId) = 0;
    virtual int32_t SetCallWaiting(int32_t slotId, bool activate) = 0;
    virtual int32_t GetCallRestriction(int32_t slotId, CallRestrictionType type) = 0;
    virtual int32_t SetCallRestriction(int32_t slotId, CallRestrictionInfo &info) = 0;
    virtual int32_t SetCallRestrictionPassword(
        int32_t slotId, CallRestrictionType fac, const char *oldPassword, const char *newPassword) = 0;
    virtual int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type) = 0;
    virtual int32_t SetCallTransferInfo(int32_t slotId, CallTransferInfo &info) = 0;
    virtual int32_t CanSetCallTransferTime(int32_t slotId, bool &result) = 0;
    virtual int32_t CombineConference(int32_t mainCallId) = 0;
    virtual int32_t SeparateConference(int32_t callId) = 0;
    virtual int32_t KickOutFromConference(int32_t callId) = 0;
    virtual int32_t IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, bool &enabled) = 0;
    virtual int32_t FormatPhoneNumber(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) = 0;
    virtual int32_t FormatPhoneNumberToE164(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) = 0;
    virtual int32_t GetMainCallId(int32_t callId, int32_t &mainCallId) = 0;
    virtual int32_t GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList) = 0;
    virtual int32_t GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList) = 0;
    virtual int32_t ControlCamera(int32_t callId, std::u16string &cameraId) = 0;
    virtual int32_t SetPreviewWindow(int32_t callId, std::string &surfaceId, sptr<Surface> surface) = 0;
    virtual int32_t SetDisplayWindow(int32_t callId, std::string &surfaceId, sptr<Surface> surface) = 0;
    virtual int32_t SetCameraZoom(float zoomRatio) = 0;
    virtual int32_t SetPausePicture(int32_t callId, std::u16string &path) = 0;
    virtual int32_t SetDeviceDirection(int32_t callId, int32_t rotation) = 0;
    virtual int32_t SetCallPreferenceMode(int32_t slotId, int32_t mode) = 0;
    virtual int32_t GetImsConfig(int32_t slotId, ImsConfigItem item) = 0;
    virtual int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value) = 0;
    virtual int32_t GetImsFeatureValue(int32_t slotId, FeatureType type) = 0;
    virtual int32_t SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value) = 0;
    virtual int32_t UpdateImsCallMode(int32_t callId, ImsCallMode mode) = 0;
    virtual int32_t EnableImsSwitch(int32_t slotId) = 0;
    virtual int32_t DisableImsSwitch(int32_t slotId) = 0;
    virtual int32_t IsImsSwitchEnabled(int32_t slotId, bool &enabled) = 0;
    virtual int32_t SetVoNRState(int32_t slotId, int32_t state) = 0;
    virtual int32_t GetVoNRState(int32_t slotId, int32_t &state) = 0;
    virtual int32_t StartRtt(int32_t callId, std::u16string &msg) = 0;
    virtual int32_t StopRtt(int32_t callId) = 0;
    virtual int32_t JoinConference(int32_t callId, std::vector<std::u16string> &numberList) = 0;
    virtual int32_t ReportOttCallDetailsInfo(std::vector<OttCallDetailsInfo> &ottVec) = 0;
    virtual int32_t ReportOttCallEventInfo(OttCallEventInfo &eventInfo) = 0;
    virtual int32_t CloseUnFinishedUssd(int32_t slotId) = 0;
    virtual int32_t InputDialerSpecialCode(const std::string &specialCode) = 0;
    virtual int32_t RemoveMissedIncomingCallNotification() = 0;
    virtual int32_t SetVoIPCallState(int32_t state) = 0;
    virtual int32_t GetVoIPCallState(int32_t &state) = 0;
    virtual sptr<IRemoteObject> GetProxyObjectPtr(CallManagerProxyType proxyType) = 0;
    virtual int32_t ReportAudioDeviceInfo() = 0;
    virtual int32_t CancelCallUpgrade(int32_t callId) = 0;
    virtual int32_t RequestCameraCapabilities(int32_t callId) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.ICallManagerService");
};
} // namespace Telephony
} // namespace OHOS

#endif // I_CALL_MANAGER_SERVICE_H
