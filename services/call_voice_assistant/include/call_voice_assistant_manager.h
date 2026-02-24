/*
 * Copyright (C) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef CALL_VOICE_ASSISTANT_MANAGER_H
#define CALL_VOICE_ASSISTANT_MANAGER_H

#include <map>
#include "ffrt.h"
#include <string_ex.h>

#include "call_earthquake_alarm_locator.h"
#include "data_ability_observer_stub.h"
#include "call_manager_inner_type.h"
#include "call_state_listener_base.h"
#include "audio_control_manager.h"
#include "ffrt.h"

namespace OHOS {
namespace Telephony {

struct IncomingContactInformation {
    std::string dialOrCome = "";
    std::string phoneNumber = "";
    std::string incomingName = "";
    std::string isContact = "0";
    std::string numberLocation = "";
    int32_t accountId = -1;
    int32_t callId = -1;
    int32_t stopBroadcasting = 0;
    int32_t call_status = -1;
    sptr<CallBase> call = nullptr;
    bool isQueryComplete = false;
};

class CallVoiceAssistantManager : public CallStateListenerBase {
public:
    CallVoiceAssistantManager() = default;
    virtual ~CallVoiceAssistantManager();
    std::shared_ptr<DataShare::DataShareHelper> Initial();
    void Release();
    bool IsSwitchOn(const std::string& switchState);
    void PublishCommonEvent(bool isConnect, std::string publisher);
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    void MuteRinger();
    void UpdateRemoteObject(const sptr<IRemoteObject> &object, int32_t callId,
        const sptr<AAFwk::IAbilityConnection> callback);
    void SetIsControlSwitchOn(bool state);
    bool GetIsControlSwitchOn();
    bool GetIsPlayRing();
    void UpdateVoipCallState(int32_t state);
    bool IsStartVoiceBroadcast();
    void UpdateNumberLocation(const std::string& location, int32_t callId);
    static std::shared_ptr<CallVoiceAssistantManager> GetInstance();

private:
    int QueryValue(const std::string& key, std::string& value);
    bool RegisterListenSwitchState();
    bool UnRegisterListenSwitchState();
    bool ConnectAbility(int32_t callId);
    bool DisconnectAbility();
    void OnStartService(const std::string& isDial, const int32_t& callId);
    void OnStopService(bool isDestructor = false);
    void UpdateReplyData(const std::string& str);
    void CallStatusIncoming(const int32_t& callId, const int32_t& accountId);
    void CallStatusDialing(const int32_t& callId, const int32_t& accountId);
    void CallStatusActive(const int32_t& callId, const int32_t& accountId);
    void CallStatusDisconnected(const int32_t& callId, const int32_t& accountId);
    std::shared_ptr<IncomingContactInformation> GetContactInfo(int32_t callId);
    void SendRequest(const std::shared_ptr<IncomingContactInformation> info, bool isNeed);
    std::u16string GetSendString(const std::shared_ptr<IncomingContactInformation> nowInfo);
    int32_t CheckTelCallState(TelCallState state);
    void UpdateContactInfo(int32_t callId);
    void ProcessStartService(const std::string& isDial, const int32_t& callId);
    std::string broadcastCheck = "0";
    std::string controlCheck = "0";
    bool isConnectService = false;
    std::string isPlay = "0";
    bool isControlSwitchOn = false;
    bool isBroadcastSwitchOn = false;
    bool isQueryedBroadcastSwitch = false;
    sptr<AAFwk::IAbilityConnection> connectCallback_ = nullptr;
    sptr<AAFwk::IDataAbilityObserver> settingsCallback_ = nullptr;
    int32_t nowCallId = -1;
    int32_t nowAccountId = -1;
    int32_t nowVoipCallState = -1;
    sptr<IRemoteObject> remoteObject_ = nullptr;
    std::map<int32_t, std::shared_ptr<IncomingContactInformation>> accountIds = {};
    static std::shared_ptr<CallVoiceAssistantManager> mInstance_;
};

class VoiceAssistantConnectCallback : public AAFwk::AbilityConnectionStub {
public:
    VoiceAssistantConnectCallback(int32_t id) : startId(id) {};
    virtual ~VoiceAssistantConnectCallback() = default;
    void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
        int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

private:
    int32_t startId = -1;
};

class VoiceAssistantSwitchObserver : public AAFwk::DataAbilityObserverStub {
public:
    VoiceAssistantSwitchObserver() = default;
    virtual ~VoiceAssistantSwitchObserver() = default;
    void OnChange() override;
};

class VoiceAssistantRingSubscriber : public EventFwk::CommonEventSubscriber {
public:
    VoiceAssistantRingSubscriber() = default;
    explicit VoiceAssistantRingSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo);
    virtual ~VoiceAssistantRingSubscriber() = default;
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &eventData) override;
    static bool Initial();
    static void Release();

private:
    void PlayRing();
    static std::shared_ptr<VoiceAssistantRingSubscriber> subscriber_;
};

}
}
#endif