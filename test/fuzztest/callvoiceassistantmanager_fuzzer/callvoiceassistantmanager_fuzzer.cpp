/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "callvoiceassistantmanager_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "cs_call.h"
#include "call_voice_assistant_manager.h"
#include "call_manager_base.h"

using namespace OHOS::Telephony;
namespace OHOS {
const int32_t CALL_STATE_NUM = 8;
const int32_t CALL_ID = -1;
const int32_t ACCOUNT_ID = -1;
const std::string CONTROL_SWITCH = "incoming_call_voice_control_switch";
const std::string INCOMING = "come";
const std::string DIALING = "dial";

void ListenCallStateFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    voicePtr->CallStateUpdated(callObjectPtr, priorState, nextState);
    voicePtr->CallStatusDialing(CALL_ID, ACCOUNT_ID);
    voicePtr->CallStatusIncoming(CALL_ID, ACCOUNT_ID);
    voicePtr->CallStatusActive(CALL_ID, ACCOUNT_ID);
    voicePtr->CallStatusDisconnected(CALL_ID, ACCOUNT_ID);
    voicePtr->ConnectAbility(ACCOUNT_ID);
    voicePtr->OnStartService(INCOMING, ACCOUNT_ID);
    voicePtr->RegisterListenSwitchState();
    voicePtr->PublishCommonEvent(true, INCOMING);
    voicePtr->DisconnectAbility();
    voicePtr->UnRegisterListenSwitchState();
    voicePtr->PublishCommonEvent(false, DIALING);
    voicePtr->OnStopService();
    voicePtr->Release();
}

void SendRequestFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::string value = "";
    ContactInfo contactInfo;
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(ACCOUNT_ID);
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    auto infoptr = voicePtr->GetContactInfo(ACCOUNT_ID);
    voicePtr->UpdateNumberLocation(value, ACCOUNT_ID);
    voicePtr->UpdateContactInfo(contactInfo, ACCOUNT_ID);
    voicePtr->UpdateRemoteObject(remoteObject, ACCOUNT_ID);
    voicePtr->OnStopService();
    voicePtr->Initial();
    voicePtr->QueryValue(CONTROL_SWITCH, value);
    voicePtr->IsStartVoiceBroadcast();
    voicePtr->IsSwitchOn(CONTROL_SWITCH);
    voicePtr->OnStartService(DIALING, ACCOUNT_ID);
    voicePtr->MuteRinger();
    voicePtr->UpdateReplyData(value);
    voicePtr->GetSendString(infoptr);
    voicePtr->SendRequest(infoptr, true);
    voicePtr->OnStopService();
    voicePtr->Release();
}

void UpdateValueFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    AppExecFwk::ElementName element;
    sptr<AAFwk::IAbilityConnection> connectCallback = sptr<VoiceAssistantConnectCallback>::MakeSptr(ACCOUNT_ID);
    sptr<AAFwk::IDataAbilityObserver> settingsCallback = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(CALL_STATE_NUM);
    std::string event = "test.test.test.test";
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(event);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriber = std::make_shared<VoiceAssistantRingSubscriber>(subscribeInfo);
    EventFwk::CommonEventData eventData;
    AAFwk::Want want;
    want.SetAction(event);
    want.SetParam(event, event);
    eventData.SetWant(want);
    settingsCallback->OnChange();
    connectCallback->OnAbilityConnectDone(element, remoteObject, ACCOUNT_ID);
    connectCallback->OnAbilityDisconnectDone(element, ACCOUNT_ID);
    subscriber->OnReceiveEvent(eventData);
    subscriber->Initial();
    subscriber->Release();
    voicePtr->GetMutex();
    voicePtr->SetIsControlSwitchOn(true);
    voicePtr->GetIsControlSwitchOn();
    voicePtr->GetIsPlayRing();
    voicePtr->UpdateVoipCallState(ACCOUNT_ID);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    ListenCallStateFunc(data, size);
    SendRequestFunc(data, size);
    UpdateValueFunc(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
