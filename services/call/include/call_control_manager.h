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

#ifndef CALL_CONTROL_MANAGER_H
#define CALL_CONTROL_MANAGER_H

#include <cstring>
#include <list>
#include <memory>
#include <mutex>

#include "call_broadcast_subscriber.h"
#include "call_policy.h"
#include "call_request_handler.h"
#include "call_setting_manager.h"
#include "call_state_listener.h"
#include "incoming_call_wake_up.h"
#include "missed_call_notification.h"
#include "pac_map.h"
#include "singleton.h"
#include "system_ability_status_change_stub.h"

/**
 * Singleton
 * @ClassName:CallControlManager
 * @Description:CallControlManager is designed for performing dial/answer/reject etc ops
 * on kinds of calls(ims,cs,ott). usually as an entrance for downflowed [app->ril] telephony business
 */
namespace OHOS {
namespace Telephony {
class CallControlManager : public CallPolicy {
    DECLARE_DELAYED_SINGLETON(CallControlManager)

public:
    bool Init();
    int32_t DialCall(std::u16string &number, AppExecFwk::PacMap &extras);
    int32_t AnswerCall(int32_t callId, int32_t videoState);
    int32_t RejectCall(int32_t callId, bool rejectWithMessage, std::u16string textMessage);
    int32_t HangUpCall(int32_t callId);
    int32_t GetCallState();
    int32_t HoldCall(int32_t callId);
    int32_t UnHoldCall(int32_t callId);
    int32_t SwitchCall(int32_t callId);
    bool HasCall();
    int32_t IsNewCallAllowed(bool &enabled);
    int32_t IsRinging(bool &enabled);
    int32_t HasEmergency(bool &enabled);
    bool NotifyNewCallCreated(sptr<CallBase> &callObjectPtr);
    bool NotifyCallDestroyed(const DisconnectedDetails &details);
    bool NotifyCallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState);
    bool NotifyIncomingCallAnswered(sptr<CallBase> &callObjectPtr);
    bool NotifyIncomingCallRejected(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content);
    bool NotifyCallEventUpdated(CallEventInfo &info);
    int32_t StartDtmf(int32_t callId, char str);
    int32_t StopDtmf(int32_t callId);
    int32_t PostDialProceed(int32_t callId, bool proceed);
    int32_t GetCallWaiting(int32_t slotId);
    int32_t SetCallWaiting(int32_t slotId, bool activate);
    int32_t GetCallRestriction(int32_t slotId, CallRestrictionType type);
    int32_t SetCallRestriction(int32_t slotId, CallRestrictionInfo &info);
    int32_t SetCallRestrictionPassword(
        int32_t slotId, CallRestrictionType fac, const char *oldPassword, const char *newPassword);
    int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type);
    int32_t SetCallTransferInfo(int32_t slotId, CallTransferInfo &info);
    int32_t CanSetCallTransferTime(int32_t slotId, bool &result);
    int32_t SetCallPreferenceMode(int32_t slotId, int32_t mode);
    // merge calls
    int32_t CombineConference(int32_t mainCallId);
    int32_t SeparateConference(int32_t callId);
    int32_t KickOutFromConference(int32_t callId);
    int32_t GetMainCallId(int32_t callId, int32_t &mainCallId);
    int32_t GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList);
    int32_t GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList);
    int32_t GetImsConfig(int32_t slotId, ImsConfigItem item);
    int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value);
    int32_t GetImsFeatureValue(int32_t slotId, FeatureType type);
    int32_t SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value);
    int32_t EnableImsSwitch(int32_t slotId);
    int32_t DisableImsSwitch(int32_t slotId);
    int32_t IsImsSwitchEnabled(int32_t slotId, bool &enabled);
    int32_t SetVoNRState(int32_t slotId, int32_t state);
    int32_t GetVoNRState(int32_t slotId, int32_t &state);
    int32_t UpdateImsCallMode(int32_t callId, ImsCallMode mode);
    int32_t StartRtt(int32_t callId, std::u16string &msg);
    int32_t StopRtt(int32_t callId);
    // invite calls to participate conference
    int32_t JoinConference(int32_t callId, std::vector<std::u16string> &numberList);
    int32_t SetMuted(bool isMute);
    int32_t MuteRinger();
    int32_t SetAudioDevice(const AudioDevice &audioDevice);
    int32_t ControlCamera(std::u16string cameraId, int32_t callingUid, int32_t callingPid);
    int32_t SetPreviewWindow(VideoWindow &window);
    int32_t SetDisplayWindow(VideoWindow &window);
    int32_t SetCameraZoom(float zoomRatio);
    int32_t SetPausePicture(std::u16string path);
    int32_t SetDeviceDirection(int32_t rotation);
    int32_t IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, bool &enabled);
    int32_t FormatPhoneNumber(std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber);
    int32_t FormatPhoneNumberToE164(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber);
    int32_t CloseUnFinishedUssd(int32_t slotId);
    void GetDialParaInfo(DialParaInfo &info);
    void GetDialParaInfo(DialParaInfo &info, AppExecFwk::PacMap &extras);
    int32_t RemoveMissedIncomingCallNotification();
    bool getUserHangUp();
    void setUserHangUp(bool status);

private:
    void CallStateObserve();
    int32_t NumberLegalityCheck(std::string &number);
    int32_t BroadcastSubscriber();

private:
    class SystemAbilityListener : public SystemAbilityStatusChangeStub {
    public:
        explicit SystemAbilityListener(std::shared_ptr<CallBroadcastSubscriber> subscriberPtr);
        ~SystemAbilityListener() = default;
        void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
        void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

    private:
        std::shared_ptr<CallBroadcastSubscriber> subscriberPtr_;
    };

private:
    std::unique_ptr<CallStateListener> callStateListenerPtr_;
    std::unique_ptr<CallRequestHandler> CallRequestHandlerPtr_;
    // notify when incoming calls are ignored, not rejected or answered
    std::shared_ptr<IncomingCallWakeup> incomingCallWakeup_;
    std::shared_ptr<MissedCallNotification> missedCallNotification_;
    std::unique_ptr<CallSettingManager> callSettingManagerPtr_;
    sptr<ISystemAbilityStatusChange> statusChangeListener_ = nullptr;
    DialParaInfo dialSrcInfo_;
    AppExecFwk::PacMap extras_;
    std::mutex mutex_;
    bool userHangup_ = false;
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_CONTROL_MANAGER_H
