/*
 * Copyright (C) 2023-2024 Huawei Device Co., Ltd.
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
#include <mutex>

#include "call_earthquake_alarm_locator.h"
#include "data_ability_observer_stub.h"
#include "call_status_manager.h"
#include "call_manager_inner_type.h"
#include "call_state_listener_base.h"
#include "call_control_manager.h"
#include "audio_control_manager.h"
#include "audio_device_manager.h"
#include "ring.h"

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
    int QueryValue(const std::string& key, std::string& value);
    bool IsSwitchOn(const std::string& switchState);
    bool RegisterListenSwitchState();
    bool UnRegisterListenSwitchState();
    bool ConnectAbility(int32_t callId);
    bool DisconnectAbility();
    void PublishCommonEvent(bool isConnect, std::string publisher);
    void OnStartService(const std::string& isDial, const int32_t& callId);
    void OnStopService();
    void UpdateReplyData(const std::string& str);
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    void CallStatusIncoming(const int32_t& callId, const int32_t& accountId);
    void CallStatusDialing(const int32_t& callId, const int32_t& accountId);
    void CallStatusActive(const int32_t& callId, const int32_t& accountId);
    void CallStatusDisconnected(const int32_t& callId, const int32_t& accountId);
    void MuteRinger();
    std::shared_ptr<IncomingContactInformation> GetContactInfo(int32_t callId);
    void SendRequest(const std::shared_ptr<IncomingContactInformation> info, bool isNeed);
    void UpdateRemoteObject(const sptr<IRemoteObject> &object, int32_t callId, const sptr<AAFwk::IAbilityConnection> callback);
    std::string GetSendString(const std::shared_ptr<IncomingContactInformation> nowInfo);
    void SetIsControlSwitchOn(bool state);
    bool GetIsControlSwitchOn();
    bool GetIsPlayRing();
    void UpdateVoipCallState(int32_t state);
    bool IsStartVoiceBroadcast();
    int32_t CheckTelCallState(TelCallState state);
    void UpdateNumberLocation(const std::string& location, int32_t callId);
    void UpdateContactInfo(const ContactInfo &info, int32_t callId);
    bool CheckValidUTF8(const std::string& str);
    bool CheckContactInfo(const std::shared_ptr<IncomingContactInformation> info);
    static std::shared_ptr<CallVoiceAssistantManager> GetInstance();

private:
    std::string broadcastCheck = "0";
    std::string controlCheck = "0";
    bool isConnectService = false;
    std::string isplay = "0";
    bool isControlSwitchOn = false;
    bool isBroadcastSwitchOn = false;
    bool isQueryedBroadcastSwitch = false;
    sptr<AAFwk::IAbilityConnection> connectCallback_ = nullptr;
    sptr<AAFwk::IDataAbilityObserver> settingsCallback_ = nullptr;
    std::mutex mutex_;
    int32_t nowCallId = -1;
    int32_t nowAccountId = -1;
    int32_t nowVoipCallState = -1;
    sptr<IRemoteObject> mRemoteObject = nullptr;
    std::map<int32_t, std::shared_ptr<IncomingContactInformation>> accountIds = {};
    static std::shared_ptr<CallVoiceAssistantManager> mInstance_;

public:
    const char *SETTINGS_DATASHARE_URI =
        "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
    const char *SETTINGS_DATASHARE_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
    const std::string SETTINGS_DATASHARE_URI_KEY = std::string(SETTINGS_DATASHARE_URI) + std::string("&key=");
    const std::string CONTROL_SWITCH = "incoming_call_voice_control_switch";
    const std::string BROADCAST_SWITCH = "incoming_call_voice_broadcast_switch";
    const std::string DEFAULT_STRING = "";
    const std::string SWITCH_TURN_OFF = "0";
    const std::string SWITCH_TURN_ON = "1";
    const std::string INCOMING = "come";
    const std::string DIALING = "dial";
    const std::string CONTROL_SWITCH_STATE_CHANGE_EVENT = "usual.event.CALL_UI_REPORT_SWITCH_STATE_CHANGE";
    const std::string IS_CONNECT_SERVICE = "isConnectVoiceAssistant";
    const std::string IS_PLAY_RING = "call_manager_play_ring";
    const std::string BUNDLE_NAME = "";
    const std::string ABILITY_NAME = "CallVoiceControlAbility";
    const std::string CONTROL_CHECK_RESULT = "incomingCallVoiceControlCheckResult";
    const std::string BROADCAST_CHECK_RESULT = "incomingCallVoiceBroadcastCheckResult";
    const int CHECK_CODE = 1006;
    const int FAIL_CODE = -1;
    const int UTF8_1BYTES = 1;
    const int UTF8_2BYTES = 2;
    const int UTF8_3BYTES = 3;
};

class VoiceAssistantConnectCallback : public IRemoteStub<AAFwk::IAbilityConnection> {
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
    static std::shared_ptr<VoiceAssistantRingSubscriber> subscriber_;
};

}
}
#endif