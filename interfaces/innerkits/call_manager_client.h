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

#ifndef CALL_MANAGER_CLIENT_H
#define CALL_MANAGER_CLIENT_H

#include "singleton.h"
#include "pac_map.h"

#include "call_manager_callback.h"

namespace OHOS {
namespace Telephony {
class CallManagerClient : public std::enable_shared_from_this<CallManagerClient> {
    DECLARE_DELAYED_SINGLETON(CallManagerClient)
public:
    void Init(int32_t systemAbilityId);
    void UnInit();
    int32_t RegisterCallBack(std::unique_ptr<CallManagerCallback> callback);
    int32_t UnRegisterCallBack();
    int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras);
    int32_t AnswerCall(int32_t callId, int32_t videoState);
    int32_t RejectCall(int32_t callId, bool isSendSms, std::u16string content);
    int32_t HangUpCall(int32_t callId);
    int32_t GetCallState();
    int32_t HoldCall(int32_t callId);
    int32_t UnHoldCall(int32_t callId);
    int32_t SwitchCall(int32_t callId);
    int32_t CombineConference(int32_t callId);
    int32_t SeparateConference(int32_t callId);
    int32_t GetMainCallId(int32_t &callId, int32_t &mainCallId);
    int32_t GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList);
    int32_t GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList);
    int32_t GetCallWaiting(int32_t slotId);
    int32_t SetCallWaiting(int32_t slotId, bool activate);
    int32_t GetCallRestriction(int32_t slotId, CallRestrictionType type);
    int32_t SetCallRestriction(int32_t slotId, CallRestrictionInfo &info);
    int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type);
    int32_t SetCallTransferInfo(int32_t slotId, CallTransferInfo &info);
    int32_t CanSetCallTransferTime(int32_t slotId, bool &result);
    int32_t SetCallPreferenceMode(int32_t slotId, int32_t mode);
    int32_t StartDtmf(int32_t callId, char str);
    int32_t StopDtmf(int32_t callId);
    int32_t IsRinging(bool &enabled);
    bool HasCall();
    int32_t IsNewCallAllowed(bool &enabled);
    int32_t IsInEmergencyCall(bool &enabled);
    int32_t IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, bool &enabled);
    int32_t FormatPhoneNumber(std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber);
    int32_t FormatPhoneNumberToE164(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber);
    int32_t SetMuted(bool isMute);
    int32_t MuteRinger();
    int32_t SetAudioDevice(const AudioDevice &audioDevice);
    int32_t ControlCamera(std::u16string cameraId);
    int32_t SetPreviewWindow(VideoWindow &window);
    int32_t SetDisplayWindow(VideoWindow &window);
    int32_t SetCameraZoom(float zoomRatio);
    int32_t SetPausePicture(std::u16string path);
    int32_t SetDeviceDirection(int32_t rotation);
    int32_t GetImsConfig(int32_t slotId, ImsConfigItem item);
    int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value);
    int32_t GetImsFeatureValue(int32_t slotId, FeatureType type);
    int32_t SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value);
    int32_t UpdateImsCallMode(int32_t callId, ImsCallMode mode);
    int32_t EnableImsSwitch(int32_t slotId);
    int32_t DisableImsSwitch(int32_t slotId);
    int32_t IsImsSwitchEnabled(int32_t slotId, bool &enabled);
    int32_t StartRtt(int32_t callId, std::u16string &msg);
    int32_t StopRtt(int32_t callId);
    int32_t JoinConference(int32_t callId, std::vector<std::u16string> &numberList);
    int32_t ReportOttCallDetailsInfo(std::vector<OttCallDetailsInfo> &ottVec);
    int32_t ReportOttCallEventInfo(OttCallEventInfo &eventInfo);
    bool HasVoiceCapability();
    int32_t ReportAudioDeviceInfo();
};
} // namespace Telephony
} // namespace OHOS

#endif
