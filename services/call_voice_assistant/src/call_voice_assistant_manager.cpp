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
 
#include "call_voice_assistant_manager.h"
#include "distributed_call_manager.h"
#include "ffrt_inner.h"

namespace {
    ffrt::queue callVoiceAssistantQueue { "call_voice_assistant_manager",
        ffrt::queue_attr().qos(ffrt_qos_user_interactive)};
}

namespace OHOS {
namespace Telephony {
constexpr const char *SYSTEM_VIDEO_RING = "system_video_ring";
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
        bool queryResult = IsSwitchOn(BROADCAST_SWITCH);
        isBroadcastSwitchOn = queryResult;
        isQueryedBroadcastSwitch = true;
        return queryResult;
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
    OnStopService(true);
    callIdNeedSendToVoiceAssisant.clear();
}

void CallVoiceAssistantManager::Release()
{
    connectCallback_ = nullptr;
    settingsCallback_ = nullptr;
    mRemoteObject = nullptr;
    isBroadcastSwitchOn = false;
    isControlSwitchOn = false;
    isQueryedBroadcastSwitch = false;
    isConnectService = false;
    broadcastCheck = DEFAULT_STRING;
    isplay = SWITCH_TURN_OFF;
};

bool CallVoiceAssistantManager::IsSwitchOn(const std::string& switchState)
{
    bool isHiCarConnected = DelayedSingleton<DistributedCallManager>::GetInstance()->IsDistributedCarDeviceOnline();
    if (isHiCarConnected) {
        TELEPHONY_LOGI("hicar is connected, voice control by hicar");
        return true;
    }
    int32_t userId = 0;
    bool isUserUnlocked = false;
    AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(userId);
    AccountSA::OsAccountManager::IsOsAccountVerified(userId, isUserUnlocked);
    if (!isUserUnlocked) {
        TELEPHONY_LOGE("user is locked");
        return false;
    }
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

bool CallVoiceAssistantManager::ConnectAbility(int32_t callId)
{
    if (connectCallback_ == nullptr) {
        connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(callId);
        if (connectCallback_ == nullptr) {
            TELEPHONY_LOGE("connectCallback is nullptr");
            return false;
        }
    }
    bool isHiCarConnected = DelayedSingleton<DistributedCallManager>::GetInstance()->IsDistributedCarDeviceOnline();
    AAFwk::Want want;
    AppExecFwk::ElementName element(
        DEFAULT_STRING, isHiCarConnected ? HICAR_BUNDLE_NAME : BUNDLE_NAME, ABILITY_NAME);
    want.SetElement(element);
    int32_t userId = FAIL_CODE;
    auto ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, connectCallback_, userId);
    if (ret != TELEPHONY_SUCCESS && isBroadcastSwitchOn) {
        VoiceAssistantRingSubscriber::Initial();
        isplay = SWITCH_TURN_ON;
        isConnectService = false;
        PublishCommonEvent(false, std::string("connect_voice_assistant_ability_failed"));
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

void CallVoiceAssistantManager::PublishCommonEvent(bool isConnect, std::string publisher)
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
    want.SetParam(BROADCAST_SWITCH.c_str(), broadcastCheck);
    want.SetParam(IS_PLAY_RING, isplay);
    want.SetParam("publisher_name", publisher);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    EventFwk::CommonEventPublishInfo publishInfo;
    std::vector<std::string> recePermissions;
    recePermissions.emplace_back("ohos.permission.ANSWER_CALL");
    publishInfo.SetSubscriberPermissions(recePermissions);
    EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGI("publish commonEvent done, [%{public}s].", publisher.c_str());
}

void CallVoiceAssistantManager::OnStartService(const std::string& isDial, const int32_t& callId)
{
    if (mRemoteObject != nullptr) {
        TELEPHONY_LOGI("mRemote Object is not nullptr");
        this->SendRequest(accountIds[callId], true);
        return;
    }
    OnStopService(!isBroadcastSwitchOn);
    if (isDial == INCOMING) {
        IsStartVoiceBroadcast();
    }
    isControlSwitchOn = IsSwitchOn(CONTROL_SWITCH);
    if (!isControlSwitchOn && !isBroadcastSwitchOn) {
        TELEPHONY_LOGE("the switch is all close");
        return;
    }
    if (ConnectAbility(callId)) {
        TELEPHONY_LOGI("start service success");
        return;
    }
    TELEPHONY_LOGE("start service failed");
};

void CallVoiceAssistantManager::OnStopService(bool isDestructor)
{
    TELEPHONY_LOGI("OnStopService enter");
    DisconnectAbility();
    UnRegisterListenSwitchState();
    if (!isDestructor) {
        DelayedSingleton<AudioControlManager>::GetInstance()->StopRingtone();
    }
    VoiceAssistantRingSubscriber::Release();
    PublishCommonEvent(false, std::string("on_stop_service"));
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
        TELEPHONY_LOGE("gotofirst row error");
        result->Close();
        return FAIL_CODE;
    }
    int columnIndex = 0;
    result->GetColumnIndex("VALUE", columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    return TELEPHONY_SUCCESS;
}

void CallVoiceAssistantManager::UpdateNumberLocation(const std::string& location, int32_t callId)
{
    callVoiceAssistantQueue.submit([=]() {
        TELEPHONY_LOGI("update location callId, %{public}d", callId);
        auto it_callId = accountIds.find(callId);
        if (it_callId == accountIds.end()) {
            TELEPHONY_LOGE("iterator is end");
            return;
        }
        auto nowInfo = it_callId->second;
        if (nowInfo == nullptr) {
            TELEPHONY_LOGE("info is nullptr");
            return;
        }
        if ((TelCallState)nowInfo->call_status == TelCallState::CALL_STATUS_DIALING) {
            TELEPHONY_LOGE("dialing without number location");
            return;
        }
        nowInfo->numberLocation = location;
        SendRequest(nowInfo, true);
    });
}

std::shared_ptr<IncomingContactInformation> CallVoiceAssistantManager::GetContactInfo(int32_t callId)
{
    if (accountIds[callId] == nullptr) {
        TELEPHONY_LOGI("initial accountIds callId, %{public}d", callId);
        accountIds[callId] = std::make_shared<IncomingContactInformation>();
    }
    return accountIds[callId];
}

void CallVoiceAssistantManager::UpdateRemoteObject(const sptr<IRemoteObject> &object, int32_t callId,
    const sptr<AAFwk::IAbilityConnection> callback)
{
    callVoiceAssistantQueue.submit([=]() {
        TELEPHONY_LOGI("update remote object callId, %{public}d", callId);
        if (nowCallId != callId || accountIds.find(callId) == accountIds.end()) {
            TELEPHONY_LOGE("nowCallId, %{public}d", nowCallId);
            AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(callback);
            return;
        }
        mRemoteObject = object;
        this->SendRequest(accountIds[callId], true);
    });
}

void CallVoiceAssistantManager::UpdateContactInfo(const ContactInfo& info, int32_t callId)
{
    callVoiceAssistantQueue.submit([=]() {
        TELEPHONY_LOGI("update contact info callId, %{public}d", callId);
        auto it_callId = accountIds.find(callId);
        if (it_callId == accountIds.end()) {
            TELEPHONY_LOGE("iterator is end");
            if (callIdNeedSendToVoiceAssisant.count(callId) == EMPTY_SIZE) {
                callIdNeedSendToVoiceAssisant.insert(callId);
                TELEPHONY_LOGI("insert callId and updateContactInfo after call add");
            }
            return;
        }
        auto nowInfo = it_callId->second;
        if (nowInfo == nullptr) {
            TELEPHONY_LOGE("info is nullptr");
            return;
        }
        if ((TelCallState)nowInfo->call_status == TelCallState::CALL_STATUS_DIALING) {
            TELEPHONY_LOGE("dialing without contact info");
            return;
        }
        nowInfo->incomingName = info.name;
        nowInfo->phoneNumber = info.number;
        nowInfo->isQueryComplete = info.isQueryComplete;
        nowInfo->isContact = (nowInfo->incomingName == "") ? SWITCH_TURN_OFF : SWITCH_TURN_ON;
        SendRequest(nowInfo, true);
    });
}

void CallVoiceAssistantManager::MuteRinger()
{
    callVoiceAssistantQueue.submit([=]() {
        TELEPHONY_LOGI("stop broadcast event and SetRingToneVolume");
        DelayedSingleton<AudioControlManager>::GetInstance()->SetRingToneVolume(0.0f);
        VoiceAssistantRingSubscriber::Release();
        isplay = SWITCH_TURN_OFF;
        if (nowCallId == FAIL_CODE) {
            TELEPHONY_LOGE("nowCallId is invalid");
            return;
        }
        auto info = accountIds[nowCallId];
        if (info != nullptr) {
            info->stopBroadcasting = 1;
        }
        this->SendRequest(info, false);
        this->PublishCommonEvent(isConnectService, std::string("stop_broadcast_event"));
    });
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
    if (!info->isQueryComplete || info->dialOrCome == DEFAULT_STRING || info->numberLocation == "default") {
        TELEPHONY_LOGE("exist null string: %{public}s.", (info->dialOrCome).c_str());
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::u16string sendStr = GetSendString(info);
    if (sendStr == DEFAULT_U16STRING) {
        TELEPHONY_LOGE("send string is invalid");
        return;
    }
    data.WriteString16(sendStr);
    int32_t retCode = mRemoteObject->SendRequest(CHECK_CODE, data, reply, option);
    TELEPHONY_LOGI("send request ret code: %{public}d.", retCode);
    if (!isNeed) {
        return;
    }
    isConnectService = false;
    isplay = SWITCH_TURN_OFF;
    UpdateReplyData(Str16ToStr8(reply.ReadString16()));
    if (controlCheck == SWITCH_TURN_ON && isControlSwitchOn) {
        isConnectService = true;
        RegisterListenSwitchState();
    }
    bool isShouldRing = (broadcastCheck == SWITCH_TURN_ON || retCode != TELEPHONY_SUCCESS);
    if (isShouldRing && isBroadcastSwitchOn && info->stopBroadcasting == 0 && info->dialOrCome != DIALING) {
        VoiceAssistantRingSubscriber::Initial();
        isplay = SWITCH_TURN_ON;
    }
    PublishCommonEvent(isConnectService, std::string("remote_object_send_request"));
}

void VoiceAssistantConnectCallback::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    TELEPHONY_LOGI("connect callback result code, %{public}d", resultCode);
    if (remoteObject == nullptr) {
        TELEPHONY_LOGE("RemoteObject is nullptr");
        return;
    }
    if (resultCode == TELEPHONY_SUCCESS) {
        sptr<AAFwk::IAbilityConnection> callback = sptr<AAFwk::IAbilityConnection>(this);
        CallVoiceAssistantManager::GetInstance()->UpdateRemoteObject(remoteObject, startId, callback);
    }
};

void VoiceAssistantConnectCallback::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    TELEPHONY_LOGI("disconnect callback result code, %{public}d", resultCode);
};

std::u16string CallVoiceAssistantManager::GetSendString(const std::shared_ptr<IncomingContactInformation> nowInfo)
{
    if (nowInfo == nullptr) {
        TELEPHONY_LOGE("nowInfo is nullptr");
        return DEFAULT_U16STRING;
    }
    if (Str8ToStr16(nowInfo->incomingName) == DEFAULT_U16STRING) {
        TELEPHONY_LOGE("incomingName is invalid.");
        nowInfo->incomingName = DEFAULT_STRING;
    }
    if (Str8ToStr16(nowInfo->numberLocation) == DEFAULT_U16STRING) {
        TELEPHONY_LOGE("numberLocation is invalid.");
        nowInfo->numberLocation = DEFAULT_STRING;
    }
    if (Str8ToStr16(nowInfo->phoneNumber) == DEFAULT_U16STRING) {
        TELEPHONY_LOGE("phoneNumber is invalid.");
        nowInfo->phoneNumber = DEFAULT_STRING;
    }
    std::string str = DEFAULT_STRING;
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
    return Str8ToStr16(str);
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
        voicePtr->PublishCommonEvent(false, std::string("control_switch_state_on_change"));
        voicePtr->SetIsControlSwitchOn(false);
    }
}

void CallVoiceAssistantManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    callVoiceAssistantQueue.submit([=]() {
        if (callObjectPtr == nullptr) {
            TELEPHONY_LOGE("callobject is nullptr");
            return;
        }
        TelCallState callState = callObjectPtr->GetTelCallState();
        if (callState != TelCallState::CALL_STATUS_ACTIVE && callState != TelCallState::CALL_STATUS_DIALING &&
            callState != TelCallState::CALL_STATUS_INCOMING && callState != TelCallState::CALL_STATUS_DISCONNECTED) {
            return;
        }
        int32_t callId = callObjectPtr->GetCallID();
        int32_t accountId = callObjectPtr->GetAccountId();
        auto info = GetContactInfo(callId);
        if (info != nullptr) {
            info->call_status = (int32_t)callState;
            info->accountId = accountId;
            info->callId = callId;
            info->call = callObjectPtr;
        }
        switch (callState) {
            case TelCallState::CALL_STATUS_ACTIVE:
                CallStatusActive(callId, accountId);
                break;
            case TelCallState::CALL_STATUS_DIALING:
                CallStatusDialing(callId, accountId);
                break;
            case TelCallState::CALL_STATUS_INCOMING:
                if (!CallObjectManager::IsNeedSilentInDoNotDisturbMode()) {
                    CallStatusIncoming(callId, accountId);
                    UpdateContactInfoIfNecessary(callId);
                } else {
                    TELEPHONY_LOGI("need silent, no need voice assistant");
                }
                break;
            case TelCallState::CALL_STATUS_DISCONNECTED:
                CallStatusDisconnected(callId, accountId);
                break;
            default:
                break;
        }
    });
}

void CallVoiceAssistantManager::UpdateContactInfoIfNecessary(int32_t callId)
{
    if (callIdNeedSendToVoiceAssisant.count(callId) > EMPTY_SIZE) {
        sptr<CallBase> call = CallObjectManager::GetOneCallObject(callId);
        if (call != nullptr && call->GetTelCallState() == TelCallState::CALL_STATUS_INCOMING) {
            UpdateContactInfo(call->GetCallerInfo(), callId);
        }
        callIdNeedSendToVoiceAssisant.erase(callId);
    }
}

void CallVoiceAssistantManager::UpdateVoipCallState(int32_t state)
{
    TELEPHONY_LOGI("voip callstate is %{public}d", state);
    nowVoipCallState = state;
}

void CallVoiceAssistantManager::CallStatusIncoming(const int32_t& callId, const int32_t& accountId)
{
    if (nowCallId != callId && nowAccountId != accountId) {
        TELEPHONY_LOGI("call_status_incoming, [%{public}d][%{public}d]", accountId, callId);
        if ((CallStateToApp)nowVoipCallState == CallStateToApp::CALL_STATE_OFFHOOK) {
            TELEPHONY_LOGE("the meetime callstate is off_hook");
            OnStopService();
            return;
        }
        auto info = accountIds[callId];
        if (info == nullptr) {
            TELEPHONY_LOGE("info is nullptr");
            return;
        }
        info->dialOrCome = INCOMING;
        if (CheckTelCallState(TelCallState::CALL_STATUS_ACTIVE) != FAIL_CODE) {
            return;
        }
        if (CheckTelCallState(TelCallState::CALL_STATUS_DIALING) != FAIL_CODE) {
            info->dialOrCome = DIALING;
        }
        if (info->call != nullptr) {
            ContactInfo contactInfo = info->call->GetCallerInfo();
            info->numberLocation = info->call->GetNumberLocation();
            info->incomingName = contactInfo.name;
            info->phoneNumber = contactInfo.number;
            info->isContact = (info->incomingName == "") ? SWITCH_TURN_OFF : SWITCH_TURN_ON;
            info->isQueryComplete = contactInfo.isQueryComplete;
        }
        nowCallId = callId;
        nowAccountId = accountId;
        OnStartService(info->dialOrCome, callId);
    }
}

void CallVoiceAssistantManager::CallStatusDialing(const int32_t& callId, const int32_t& accountId)
{
    auto info = accountIds[callId];
    if (info == nullptr) {
        TELEPHONY_LOGE("info is nullptr");
        return;
    }
    if (nowCallId != callId && nowAccountId != accountId) {
        TELEPHONY_LOGI("call_status_dialing, [%{public}d][%{public}d]", accountId, callId);
        info->dialOrCome = DIALING;
        info->numberLocation = DIALING;
        info->phoneNumber = DIALING;
        info->isQueryComplete = true;
        nowCallId = callId;
        nowAccountId = accountId;
        OnStartService(info->dialOrCome, callId);
    }
}

void CallVoiceAssistantManager::CallStatusActive(const int32_t& callId, const int32_t& accountId)
{
    TELEPHONY_LOGI("muteRinger before playSoundTone, [%{public}d][%{public}d]", accountId, callId);
    DelayedSingleton<AudioControlManager>::GetInstance()->SetRingToneVolume(0.0f);
    VoiceAssistantRingSubscriber::Release();
    PublishCommonEvent(false, std::string("call_status_active"));
    mRemoteObject = nullptr;
    isQueryedBroadcastSwitch = false;
}

void CallVoiceAssistantManager::CallStatusDisconnected(const int32_t& callId, const int32_t& accountId)
{
    TELEPHONY_LOGI("call_status_disconnected, [%{public}d][%{public}d]", accountId, callId);
    auto lastInfo = accountIds[callId];
    accountIds.erase(callId);
    if (CheckTelCallState(TelCallState::CALL_STATUS_ACTIVE) != FAIL_CODE) {
        return;
    }
    int32_t comeCallId = CheckTelCallState(TelCallState::CALL_STATUS_INCOMING);
    TELEPHONY_LOGI("comeCallId, %{public}d.", comeCallId);
    if (comeCallId != FAIL_CODE) {
        auto nowInfo = accountIds[comeCallId];
        if (nowInfo != nullptr && nowInfo->dialOrCome == INCOMING) {
            nowCallId = nowInfo->callId;
            nowAccountId = nowInfo->accountId;
            SendRequest(nowInfo, true);
        }
        return;
    }
    TELEPHONY_LOGI("accountIds size is %{public}zu", accountIds.size());
    SendRequest(lastInfo, false);
    OnStopService();
    nowCallId = FAIL_CODE;
    nowAccountId = FAIL_CODE;
}

int32_t CallVoiceAssistantManager::CheckTelCallState(TelCallState state)
{
    TELEPHONY_LOGI("check state, %{public}d.", state);
    int32_t invalid = FAIL_CODE;
    for (auto& id : accountIds) {
        if (id.second == nullptr) {
            TELEPHONY_LOGE("invalid callId, %{public}d.", id.first);
            invalid = id.first;
            continue;
        }
        TELEPHONY_LOGI("callId %{public}d, state %{public}d.", id.first, id.second->call_status);
        if ((TelCallState)id.second->call_status == state) {
            return id.first;
        }
    }
    accountIds.erase(invalid);
    return FAIL_CODE;
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
    if (subscriber_ != nullptr) {
        TELEPHONY_LOGI("UnSubscribeCommonEvent");
        EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
        subscriber_ = nullptr;
    }
}

void VoiceAssistantRingSubscriber::PlayRing()
{
    TELEPHONY_LOGI("broadcast switch is open, start play system ring");
    DelayedSingleton<AudioControlManager>::GetInstance()->StopRingtone();
    sptr<CallBase> incomingCall =
        CallObjectManager::GetOneCarrierCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    ContactInfo contactInfo = incomingCall->GetCallerInfo();
    if (std::string(contactInfo.ringtonePath).empty() &&
        DelayedSingleton<AudioControlManager>::GetInstance()->IsSystemVideoRing(incomingCall)) {
        if (memcpy_s(contactInfo.ringtonePath, FILE_PATH_MAX_LEN, SYSTEM_VIDEO_RING, strlen(SYSTEM_VIDEO_RING)) !=
            EOK) {
            TELEPHONY_LOGE("memcpy_s ringtonePath fail");
        };
    }
    if (DelayedSingleton<AudioControlManager>::GetInstance()->NeedPlayVideoRing(contactInfo, incomingCall)) {
        AAFwk::WantParams params = incomingCall->GetExtraParams();
        params.SetParam("VideoRingPath", AAFwk::String::Box(std::string(contactInfo.ringtonePath)));
        incomingCall->SetExtraParams(params);
        CallAttributeInfo info;
        incomingCall->GetCallAttributeBaseInfo(info);
        DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportCallStateInfo(info);
    }
    DelayedSingleton<AudioControlManager>::GetInstance()->PlayRingtone();
}

void VoiceAssistantRingSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &eventData)
{
    callVoiceAssistantQueue.submit([=]() {
        auto voicePtr = CallVoiceAssistantManager::GetInstance();
        if (voicePtr == nullptr) {
            TELEPHONY_LOGE("voicePtr is nullptr");
            return;
        }
        const AAFwk::Want &want = eventData.GetWant();
        std::string action = want.GetAction();
        if (action != voicePtr->CONTROL_SWITCH_STATE_CHANGE_EVENT) {
            return;
        }
        std::string publisher = want.GetStringParam("publisher_name");
        if (publisher != std::string("remote_object_send_request") &&
            publisher != std::string("connect_voice_assistant_ability_failed")) {
            TELEPHONY_LOGE("publisher name, [%{public}s]", publisher.c_str());
            return;
        }
        std::string isplay = want.GetStringParam(voicePtr->IS_PLAY_RING);
        bool isPlayRing = voicePtr->GetIsPlayRing();
        if (isplay == voicePtr->SWITCH_TURN_ON && isPlayRing) {
            PlayRing();
        }
    });
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

void CallVoiceAssistantManager::UpdateReplyData(const std::string& str)
{
    TELEPHONY_LOGI("receiveData, %{public}s.", str.c_str());
    std::size_t pos1 = 0;
    std::size_t pos2 = 0;
    std::map<std::string, std::string> replyData;
    while (pos1 != std::string::npos && pos2 != std::string::npos) {
        pos1 = str.find("\"", pos2 + 1);
        if (pos1 == std::string::npos) {
            break;
        }
        pos2 = str.find("\"", pos1 + 1);
        if (pos2 == std::string::npos) {
            break;
        }
        auto key = str.substr(pos1 + 1, pos2 -pos1 - 1);
        pos1 = str.find("\"", pos2 + 1);
        if (pos1 == std::string::npos) {
            break;
        }
        pos2 = str.find("\"", pos1 + 1);
        if (pos2 == std::string::npos) {
            break;
        }
        auto value = str.substr(pos1 + 1, pos2 -pos1 - 1);
        replyData.insert({key, value});
    }
    controlCheck = SWITCH_TURN_OFF;
    broadcastCheck = DEFAULT_STRING;
    auto it_control = replyData.find(CONTROL_CHECK_RESULT);
    if (it_control != replyData.end()) {
        controlCheck = it_control->second;
    }
    auto it_broadcast = replyData.find(BROADCAST_CHECK_RESULT);
    if (it_broadcast != replyData.end()) {
        broadcastCheck = it_broadcast->second;
    }
    TELEPHONY_LOGI("%{public}s, %{public}s.", broadcastCheck.c_str(), controlCheck.c_str());
}
}
}