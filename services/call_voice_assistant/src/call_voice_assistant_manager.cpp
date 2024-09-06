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
 
#include "call_voice_assistant_manager.h"
 
namespace OHOS {
namespace Telephony {

std::shared_ptr<VoiceAssistantRingSubscriber> VoiceAssistantRingSubscriber::subscriber_ = nullptr;
std::shared_ptr<CallVoiceAssistantManager> CallVoiceAssistantManager::mInstance_ =
    std::make_shared<CallVoiceAssistantManager>();
 
std::shared_ptr<CallVoiceAssistantManager> CallVoiceAssistantManager::GetInstance()
{
    if (mInstance_ != nullptr) {
        return mInstance_;
    }
    return std::make_shared<CallVoiceAssistantManager>();
};

bool CallVoiceAssistantManager::IsStartVoiceBroadcast()
{
    if (!isQueryedBroadcastSwitch) {
        TELEPHONY_LOGI("first query broad switch");
        isBroadcastSwitchOn = IsSwitchOn(BROADCAST_SWITCH);
        isQueryedBroadcastSwitch = true;
    }
    return isBroadcastSwitchOn;
}

std::shared_ptr<DataShare::DataShareHelper> CallVoiceAssistantManager::Initial()
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("saManager is nullptr");
        return nullptr;
    }
    sptr<IRemoteObject> remote = saManager->GetSystemAbility(OHOS::TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (remote == nullptr) {
        TELEPHONY_LOGE("remote is nullptr");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remote, SETTINGS_DATASHARE_URI, SETTINGS_DATASHARE_EXT_URI);
};

CallVoiceAssistantManager::~CallVoiceAssistantManager()
{
    OnStopService();
}

void CallVoiceAssistantManager::Release()
{
    connectCallback_ = nullptr;
    settingsCallback_ = nullptr;
    mRemoteObject = nullptr;
    isBroadcastSwitchOn = false;
    isControlSwitchOn = false;
    isQueryedBroadcastSwitch = false;
};

bool CallVoiceAssistantManager::IsSwitchOn(const std::string& switchState)
{
    std::string value = SWITCH_TURN_OFF;
    this->QueryValue(switchState, value);
    TELEPHONY_LOGI("%{public}s is %{public}s", switchState.c_str(), value.c_str());
    if (value == SWITCH_TURN_OFF) {
        return false;
    }
    return true;
};

bool CallVoiceAssistantManager::RegisterListenSwitchState()
{
    if (settingsCallback_ == nullptr) {
        settingsCallback_ = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
        if (settingsCallback_ == nullptr) {
            TELEPHONY_LOGE("settingsCallback is nullptr");
            return false;
        }
        auto datashareHelper_ = Initial();
        if (datashareHelper_ == nullptr) {
            TELEPHONY_LOGE("datashareHelper is nullptr");
            return false;
        }
        OHOS::Uri uri_listen(SETTINGS_DATASHARE_URI_KEY + CONTROL_SWITCH);
        datashareHelper_->RegisterObserver(uri_listen, settingsCallback_);
        datashareHelper_->Release();
    }
    TELEPHONY_LOGI("listen success");
    return true;
};

bool CallVoiceAssistantManager::UnRegisterListenSwitchState()
{
    if (settingsCallback_ == nullptr) {
        TELEPHONY_LOGE("listen already close");
        return false;
    };
    auto datashareHelper_ = Initial();
    if (datashareHelper_ == nullptr) {
        TELEPHONY_LOGE("datashareHelper is nullptr");
        return false;
    }
    OHOS::Uri uri_listen(SETTINGS_DATASHARE_URI_KEY + CONTROL_SWITCH);
    datashareHelper_->UnregisterObserver(uri_listen, settingsCallback_);
    datashareHelper_->Release();
    TELEPHONY_LOGI("listen close success");
    return true;
};

bool CallVoiceAssistantManager::ConnectAbility(int32_t accountId)
{
    if (connectCallback_ == nullptr) {
        connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(accountId);
        if (connectCallback_ == nullptr) {
            TELEPHONY_LOGE("connectCallback is nullptr");
            return false;
        }
    }
    AAFwk::Want want;
    AppExecFwk::ElementName element(DEFAULT_STRING, BUNDLE_NAME, ABILITY_NAME);
    want.SetElement(element);
    int32_t userId = FAIL_CODE;
    auto ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, connectCallback_, userId);
    if (ret != TELEPHONY_SUCCESS && isBroadcastSwitchOn) {
        VoiceAssistantRingSubscriber::Initial();
        isplay = SWITCH_TURN_ON;
        isConnectService = false;
        PublishCommonEvent(false);
    }
    return ret == TELEPHONY_SUCCESS;
};

bool CallVoiceAssistantManager::DisconnectAbility()
{
    if (connectCallback_ == nullptr) {
        TELEPHONY_LOGI("disconnectAbility already close");
        return false;
    }
    auto ret = AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(connectCallback_);
    TELEPHONY_LOGI("disconnectAbility %{public}d", ret);
    return true;
};

void CallVoiceAssistantManager::PublishCommonEvent(bool isConnect)
{
    if (connectCallback_ == nullptr) {
        TELEPHONY_LOGI("service is not start");
        return;
    }
    if (isConnect) {
        isConnect = isConnectService;
    }
    std::string startService = isConnect ? SWITCH_TURN_ON : SWITCH_TURN_OFF;
    EventFwk::Want want;
    want.SetAction(CONTROL_SWITCH_STATE_CHANGE_EVENT);
    want.SetParam(IS_CONNECT_SERVICE, startService);
    want.SetParam(CONTROL_SWITCH.c_str(), controlCheck);
    want.SetParam(IS_PLAY_RING, isplay);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    EventFwk::CommonEventPublishInfo publishInfo;
    std::vector<std::string> recePermissions;
    recePermissions.emplace_back("ohos.permission.ANSWER_CALL");
    publishInfo.SetSubscriberPermissions(recePermissions);
    EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGI("publish commonEvent done");
}

void CallVoiceAssistantManager::OnStartService(const std::string& isDial, const int32_t& accountId)
{
    auto info = accountIds[accountId];
    if (info == nullptr) {
        return;
    }
    if (mRemoteObject != nullptr) {
        TELEPHONY_LOGI("mRemote Object is not nullptr");
        if (!isBroadcastSwitchOn) {
            TELEPHONY_LOGI("start play system ring");
            info->stopBroadcasting = 1;
        }
        this->SendRequest(accountIds[accountId], true);
        return;
    }
    OnStopService();
    if (isDial == INCOMING) {
        IsStartVoiceBroadcast();
    }
    isControlSwitchOn = IsSwitchOn(CONTROL_SWITCH);
    if (!isControlSwitchOn && !isBroadcastSwitchOn) {
        TELEPHONY_LOGE("the switch is all close");
        return;
    }
    if (!isBroadcastSwitchOn) {
        TELEPHONY_LOGI("start play system ring");
        info->stopBroadcasting = 1;
    }
    if (ConnectAbility(accountId)) {
        TELEPHONY_LOGI("start service success");
        return;
    }
    TELEPHONY_LOGE("start service failed");
};

void CallVoiceAssistantManager::OnStopService()
{
    DisconnectAbility();
    UnRegisterListenSwitchState();
    VoiceAssistantRingSubscriber::Release();
    PublishCommonEvent(false);
    Release();
};

int CallVoiceAssistantManager::QueryValue(const std::string& key, std::string& value)
{
    auto datashareHelper_ = Initial();
    if (datashareHelper_ == nullptr) {
        TELEPHONY_LOGE("datashareHelper is nullptr");
        return FAIL_CODE;
    }
    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("KEYWORD", key);
    OHOS::Uri uri(SETTINGS_DATASHARE_URI_KEY + key);
    auto result = datashareHelper_->Query(uri, predicates, columns);
    datashareHelper_->Release();
    if (result == nullptr) {
        TELEPHONY_LOGE("result is nullptr");
        return FAIL_CODE;
    }
    int rowCount = 0;
    result->GetRowCount(rowCount);
    if (rowCount == 0) {
        TELEPHONY_LOGI("rowCount is 0");
        result->Close();
        return FAIL_CODE;
    }
    if (result->GoToFirstRow() != DataShare::E_OK) {
        TELEPHONY_LOGE("gotofirst row is error");
        result->Close();
        return FAIL_CODE;
    }
    int columnIndex = 0;
    result->GetColumnIndex("VALUE", columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    return TELEPHONY_SUCCESS;
}

void CallVoiceAssistantManager::UpdateNumberLocation(const std::string& location, int32_t accountId)
{
    TELEPHONY_LOGI("update location id is %{public}d", accountId);
    auto voicePtr = CallVoiceAssistantManager::GetInstance();
    if (voicePtr == nullptr) {
        TELEPHONY_LOGE("voicePtr is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(voicePtr->GetMutex());
    auto nowInfo = voicePtr->GetContactInfo(accountId);
    if (nowInfo == nullptr) {
        TELEPHONY_LOGE("info is nullptr");
        return;
    }
    nowInfo->numberLocation = location;
    nowInfo->isSetNumberLocation = true;
    voicePtr->SendRequest(nowInfo, true);
}

std::shared_ptr<IncomingContactInformation> CallVoiceAssistantManager::GetContactInfo(int32_t accountId)
{
    if (accountIds[accountId] == nullptr) {
        TELEPHONY_LOGI("initial contactinfo id is %{public}d", accountId);
        accountIds[accountId] = std::make_shared<IncomingContactInformation>();
    }
    return accountIds[accountId];
}

void CallVoiceAssistantManager::UpdateRemoteObject(const sptr<IRemoteObject> &object, int32_t accountId)
{
    mRemoteObject = object;
    this->SendRequest(accountIds[accountId], true);
}

void CallVoiceAssistantManager::UpdateContactInfo(const ContactInfo& info, int32_t accountId)
{
    TELEPHONY_LOGI("update contactinfo id is %{public}d", accountId);
    auto voicePtr = CallVoiceAssistantManager::GetInstance();
    if (voicePtr == nullptr) {
        TELEPHONY_LOGE("voicePtr is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(voicePtr->GetMutex());
    auto nowInfo = voicePtr->GetContactInfo(accountId);
    if (nowInfo == nullptr) {
        TELEPHONY_LOGE("nowInfo is nullptr");
        return;
    }
    nowInfo->incomingName = info.name;
    nowInfo->phoneNumber = info.number;
    nowInfo->isContact =
        (nowInfo->incomingName == "") ? voicePtr->SWITCH_TURN_OFF : voicePtr->SWITCH_TURN_ON;
    nowInfo->isSetContactInfo = true;
    voicePtr->SendRequest(nowInfo, true);
}

void CallVoiceAssistantManager::MuteRinger()
{
    TELEPHONY_LOGI("stop broadcast event");
    std::lock_guard<std::mutex> lock(mutex_);
    VoiceAssistantRingSubscriber::Release();
    isplay = SWITCH_TURN_OFF;
    if (nowAccountId == FAIL_CODE) {
        TELEPHONY_LOGE("nowAccountId is invalid");
        return;
    }
    auto info = accountIds[nowAccountId];
    if (info != nullptr) {
        info->stopBroadcasting = 1;
    }
    this->SendRequest(info, true);
}

void CallVoiceAssistantManager::SendRequest(const std::shared_ptr<IncomingContactInformation> info, bool isNeed)
{
    if (mRemoteObject == nullptr) {
        TELEPHONY_LOGE("mRemoteObject is nullptr");
        return;
    }
    if (info == nullptr) {
        TELEPHONY_LOGE("info is nullptr");
        return;
    }
    if (info->dialOrCome == DIALING) {
        info->isSetNumberLocation = true;
        info->isSetContactInfo = true;
    }

    if (!info->isSetNumberLocation || !info->isSetContactInfo || info->dialOrCome == DEFAULT_STRING) {
        return;
    }
    MessageParcel data, reply;
    MessageOption option;
    isplay = SWITCH_TURN_OFF;
    isConnectService = true;
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    data.WriteString16(convert.from_bytes(GetSendString(info)));
    int32_t retCode = mRemoteObject->SendRequest(CHECK_CODE, data, reply, option);
    if (!isNeed) {
        return;
    }
    if (retCode != TELEPHONY_SUCCESS && isBroadcastSwitchOn && info->stopBroadcasting == 0) {
        TELEPHONY_LOGE("ret is %{public}d", retCode);
        VoiceAssistantRingSubscriber::Initial();
        isConnectService = false;
        isplay = SWITCH_TURN_ON;
    }
    std::u16string _reply1 = reply.ReadString16();
    TELEPHONY_LOGI("receiveData is %{public}s", convert.to_bytes(_reply1).c_str());
    UpdateReplyData(convert.to_bytes(_reply1));
    if (controlCheck == SWITCH_TURN_ON && isControlSwitchOn) {
        RegisterListenSwitchState();
    } else {
        controlCheck = SWITCH_TURN_OFF;
    }
    if (broadcastCheck == SWITCH_TURN_ON && isBroadcastSwitchOn &&
        retCode == TELEPHONY_SUCCESS && info->stopBroadcasting == 0) {
        VoiceAssistantRingSubscriber::Initial();
        isplay = SWITCH_TURN_ON;
    }
    PublishCommonEvent(isConnectService);
}

void VoiceAssistantConnectCallback::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    TELEPHONY_LOGI("update mRemote Object code is %{public}d", resultCode);
    if (remoteObject == nullptr) {
        TELEPHONY_LOGE("RemoteObject is nullptr");
        return;
    }
    CallVoiceAssistantManager::GetInstance()->UpdateRemoteObject(remoteObject, startId);
};

void VoiceAssistantConnectCallback::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    TELEPHONY_LOGI("disconnect callback result code: %{public}d", resultCode);
};

std::string CallVoiceAssistantManager::GetSendString(const std::shared_ptr<IncomingContactInformation> nowInfo)
{
    std::string str = "";
    if (nowInfo == nullptr) {
        TELEPHONY_LOGE("nowInfo is nullptr");
        return str;
    }
    auto fun = [&str](std::string key, std::string value, bool start = false, bool end = false) {
        std::string first = (start) ? "{" : "";
        std::string last = (end) ? "}" : ",";
        str = str + first + "\"" + key + "\"" + ":" + "\"" + value + "\"" + last;
    };
    fun("dialOrCome", nowInfo->dialOrCome, true);
    fun("callStatus", std::to_string(nowInfo->call_status));
    fun("callId", std::to_string(nowInfo->callId));
    fun("phoneNumber", nowInfo->phoneNumber);
    fun("isContact", nowInfo->isContact);
    fun("contactName", nowInfo->incomingName);
    fun("sim", std::to_string(nowInfo->accountId));
    fun("stopBroadcasting", std::to_string(nowInfo->stopBroadcasting));
    fun("location", nowInfo->numberLocation, false, true);
    return str;
}

void VoiceAssistantSwitchObserver::OnChange()
{
    auto voicePtr = CallVoiceAssistantManager::GetInstance();
    if (voicePtr == nullptr) {
        TELEPHONY_LOGE("voicePtr is nullptr");
        return;
    }
    std::string switch_off = voicePtr->SWITCH_TURN_OFF;
    auto lastControlVal = voicePtr->GetIsControlSwitchOn();
    if (!lastControlVal) {
        TELEPHONY_LOGI("the controlswitch already close");
        return;
    }
    auto ret = voicePtr->IsSwitchOn(voicePtr->CONTROL_SWITCH);
    if (lastControlVal && !ret) {
        TELEPHONY_LOGI("controlswitch from true to false");
        voicePtr->PublishCommonEvent(false);
        voicePtr->SetIsControlSwitchOn(false);
    }
}

void CallVoiceAssistantManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("callobject is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    TelCallState callState = callObjectPtr->GetTelCallState();
    if (callState != TelCallState::CALL_STATUS_ACTIVE && callState != TelCallState::CALL_STATUS_DIALING &&
        callState != TelCallState::CALL_STATUS_INCOMING && callState != TelCallState::CALL_STATUS_DISCONNECTED) {
        return;
    }
    int32_t callId = callObjectPtr->GetCallID();
    int32_t accountId = callObjectPtr->GetAccountId();
    auto info = GetContactInfo(accountId);
    if (info != nullptr) {
        info->call_status = (int32_t)callState;
        info->accountId = accountId;
        info->callId = callId;
    }
    switch (callState) {
        case TelCallState::CALL_STATUS_ACTIVE:
            CallStatusActive(callId, accountId);
            break;
        case TelCallState::CALL_STATUS_DIALING:
            CallStatusDialing(callId, accountId);
            break;
        case TelCallState::CALL_STATUS_INCOMING:
            CallStatusIncoming(callId, accountId);
            break;
        case TelCallState::CALL_STATUS_DISCONNECTED:
            CallStatusDisconnected(callId, accountId);
            break;
        default:
            break;
    }
}

void CallVoiceAssistantManager::UpdateVoipCallState(int32_t state)
{
    TELEPHONY_LOGI("voip callstate is %{public}d", state);
    nowVoipCallState = state;
}

void CallVoiceAssistantManager::CallStatusIncoming(const int32_t& callId, const int32_t& accountId)
{
    if ((CallStateToApp)nowVoipCallState == CallStateToApp::CALL_STATE_OFFHOOK) {
        TELEPHONY_LOGE("the meetime callstate is off_hook");
        OnStopService();
        return;
    }
    auto info = GetContactInfo(accountId);
    if (info == nullptr) {
        TELEPHONY_LOGE("info is nullptr");
        return;
    }
    info->dialOrCome = INCOMING;
    for (auto& id : accountIds) {
        if (id.second == nullptr) {
            break;
        }
        if ((TelCallState)id.second->call_status == TelCallState::CALL_STATUS_DIALING) {
            info->stopBroadcasting = 1;
        }
        if ((TelCallState)id.second->call_status == TelCallState::CALL_STATUS_ACTIVE) {
            TELEPHONY_LOGI("id %{public}d is active", id.first);
            return;
        }
    }
    if (nowCallId != callId && nowAccountId != accountId) {
        TELEPHONY_LOGI("call_status_incoming id %{public}d", accountId);
        nowCallId = callId;
        nowAccountId = accountId;
        OnStartService(INCOMING, accountId);
    }
}

void CallVoiceAssistantManager::CallStatusDialing(const int32_t& callId, const int32_t& accountId)
{
    auto info = GetContactInfo(accountId);
    if (info == nullptr) {
        TELEPHONY_LOGE("info is nullptr");
        return;
    }
    if (nowCallId != callId && nowAccountId != accountId) {
        TELEPHONY_LOGI("call_status_dialing id %{public}d", accountId);
        info->dialOrCome = DIALING;
        nowCallId = callId;
        nowAccountId = accountId;
        OnStartService(DIALING, accountId);
    }
}

void CallVoiceAssistantManager::CallStatusActive(const int32_t& callId, const int32_t& accountId)
{
    TELEPHONY_LOGI("call_status_active id %{public}d", accountId);
    VoiceAssistantRingSubscriber::Release();
    PublishCommonEvent(false);
    mRemoteObject = nullptr;
}

void CallVoiceAssistantManager::CallStatusDisconnected(const int32_t& callId, const int32_t& accountId)
{
    TELEPHONY_LOGI("call_status_disconnected id %{public}d", accountId);
    auto info = accountIds[accountId];
    VoiceAssistantRingSubscriber::Release();
    accountIds.erase(accountId);
    TELEPHONY_LOGI("counts is %{public}d", (int)accountIds.size());
    for (auto& id : accountIds) {
        if (id.second == nullptr) {
            break;
        }
        TELEPHONY_LOGI("id %{public}d state %{public}d", id.first, id.second->call_status);
        if ((TelCallState)id.second->call_status == TelCallState::CALL_STATUS_INCOMING) {
            CallStatusIncoming(id.second->callId, id.second->accountId);
            return;
        }
    }
    SendRequest(info, false);
    OnStopService();
    nowCallId = FAIL_CODE;
    nowAccountId = FAIL_CODE;
}

VoiceAssistantRingSubscriber::VoiceAssistantRingSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : EventFwk::CommonEventSubscriber(subscriberInfo) {}

bool VoiceAssistantRingSubscriber::Initial()
{
    if (subscriber_ == nullptr) {
        auto voicePtr = CallVoiceAssistantManager::GetInstance();
        if (voicePtr == nullptr) {
            TELEPHONY_LOGE("voicePtr is nullptr");
            return false;
        }
        EventFwk::MatchingSkills matchingSkills;
        matchingSkills.AddEvent(voicePtr->CONTROL_SWITCH_STATE_CHANGE_EVENT);
        EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
        subscribeInfo.SetPermission("ohos.permission.GET_TELEPHONY_STATE");
        subscriber_ = std::make_shared<VoiceAssistantRingSubscriber>(subscribeInfo);
        EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
    }
    TELEPHONY_LOGI("prepare ring initial");
    return true;
};

void VoiceAssistantRingSubscriber::Release()
{
    DelayedSingleton<AudioControlManager>::GetInstance()->StopRingtone();
    if (subscriber_ != nullptr) {
        EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
        subscriber_ = nullptr;
    }
}

void VoiceAssistantRingSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &eventData)
{
    auto voicePtr = CallVoiceAssistantManager::GetInstance();
    if (voicePtr == nullptr) {
        TELEPHONY_LOGE("voicePtr is nullptr");
        return;
    }
    const AAFwk::Want &want = eventData.GetWant();
    std::string action = want.GetAction();
    std::string isplay = want.GetStringParam(voicePtr->IS_PLAY_RING);
    if (action != voicePtr->CONTROL_SWITCH_STATE_CHANGE_EVENT) {
        return;
    }
    bool isPlayRing = voicePtr->GetIsPlayRing();
    if (isplay == voicePtr->SWITCH_TURN_ON && isPlayRing) {
        TELEPHONY_LOGI("broadcast switch is open, start play system ring");
        DelayedSingleton<AudioControlManager>::GetInstance()->StopRingtone();
        DelayedSingleton<AudioControlManager>::GetInstance()->PlayRingtone();
    }
};

bool CallVoiceAssistantManager::GetIsPlayRing()
{
    return (isplay == SWITCH_TURN_ON) ? true : false;
}

void CallVoiceAssistantManager::SetIsControlSwitchOn(bool state)
{
    isControlSwitchOn = state;
};

bool CallVoiceAssistantManager::GetIsControlSwitchOn()
{
    return isControlSwitchOn;
};

std::mutex& CallVoiceAssistantManager::GetMutex()
{
    return this->mutex_;
};

void CallVoiceAssistantManager::UpdateReplyData(const std::string& str)
{
    int pos1 = 0, pos2 = 0;
    std::map<std::string, std::string> replyData;
    while (pos1 != FAIL_CODE && pos2 != FAIL_CODE) {
        pos1 = str.find("\"", pos2 + 1);
        if (pos1 == FAIL_CODE) {
            break;
        }
        pos2 = str.find("\"", pos1 + 1);
        if (pos2 == FAIL_CODE) {
            break;
        }
        auto key = str.substr(pos1 + 1, pos2 -pos1 - 1);
        pos1 = str.find("\"", pos2 + 1);
        if (pos1 == FAIL_CODE) {
            break;
        }
        pos2 = str.find("\"", pos1 + 1);
        if (pos2 == FAIL_CODE) {
            break;
        }
        auto value = str.substr(pos1 + 1, pos2 -pos1 - 1);
        replyData.insert({key, value});
    }
    controlCheck = SWITCH_TURN_OFF;
    broadcastCheck = SWITCH_TURN_OFF;
    auto it_control = replyData.find(CONTROL_CHECK_RESULT);
    if (it_control != replyData.end()) {
        controlCheck = it_control->second;
    }
    auto it_broadcast = replyData.find(BROADCAST_CHECK_RESULT);
    if (it_broadcast != replyData.end()) {
        broadcastCheck = it_broadcast->second;
    }
    TELEPHONY_LOGI("%{public}s %{public}s", broadcastCheck.c_str(), controlCheck.c_str());
}

}
}