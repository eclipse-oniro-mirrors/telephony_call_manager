/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define private public
#define protected public

#include <map>
#include <mutex>

#include "call_earthquake_alarm_locator.h"
#include "data_ability_observer_stub.h"
#include "call_manager_inner_type.h"
#include "call_state_listener_base.h"
#include "call_control_manager.h"
#include "audio_control_manager.h"
#include "ring.h"
#include "common_type.h"
#include "call_base.h"
#include "cs_call.h"
#include "call_voice_assistant_manager.h"
#include "gtest/gtest.h"
#include "token.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
using namespace std;

class CallVoiceAssistantManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    virtual void SetUp() {};
    virtual void TearDown() {};
};

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_CallVoiceAssistantManager_001, TestSize.Level0)
{
    const int32_t callId = -1;
    const int32_t accountId = -1;
    const std::string incoming = "come";
    const std::string dialing = "dial";
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = TelCallState::CALL_STATUS_DIALING;
    TelCallState nextState = TelCallState::CALL_STATUS_INCOMING;
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->mInstance_ = voicePtr;
    voicePtr->CallStateUpdated(callObjectPtr, priorState, nextState);
    callObjectPtr->SetCallType(CallType::TYPE_VOIP);
    voicePtr->CallStateUpdated(callObjectPtr, priorState, nextState);
    voicePtr->CallStatusDialing(callId, accountId);
    voicePtr->CallStatusIncoming(callId, accountId);
    voicePtr->CallStatusActive(callId, accountId);
    voicePtr->CallStatusDisconnected(callId, accountId);
    voicePtr->ConnectAbility(accountId);
    voicePtr->OnStartService(incoming, accountId);
    voicePtr->RegisterListenSwitchState();
    voicePtr->PublishCommonEvent(true, incoming);
    voicePtr->DisconnectAbility();
    voicePtr->UnRegisterListenSwitchState();
    voicePtr->PublishCommonEvent(false, dialing);
    voicePtr->OnStopService();
    voicePtr->Release();
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
    ASSERT_TRUE(voicePtr->settingsCallback_  == nullptr);
    ASSERT_TRUE(voicePtr->connectCallback_  == nullptr);
    ASSERT_TRUE(voicePtr->mRemoteObject == nullptr);
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_002
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_CallVoiceAssistantManager_002, TestSize.Level0)
{
    const int32_t accountId = -1;
    const std::string controlSwitch = "incoming_call_voice_control_switch";
    const std::string dialing = "dial";
    std::string value = "";
    ContactInfo contactInfo;
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(accountId);
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->mInstance_ = voicePtr;
    voicePtr->SetIsControlSwitchOn(true);
    voicePtr->GetIsControlSwitchOn();
    voicePtr->GetIsPlayRing();
    voicePtr->UpdateVoipCallState(accountId);
    auto infoptr = voicePtr->GetContactInfo(accountId);
    voicePtr->UpdateNumberLocation(value, accountId);
    voicePtr->UpdateContactInfo(contactInfo, accountId);
    voicePtr->UpdateRemoteObject(remoteObject, accountId, nullptr);
    voicePtr->OnStopService();
    voicePtr->Initial();
    voicePtr->QueryValue(controlSwitch, value);
    voicePtr->IsStartVoiceBroadcast();
    voicePtr->IsSwitchOn(controlSwitch);
    voicePtr->OnStartService(dialing, accountId);
    voicePtr->MuteRinger();
    voicePtr->UpdateReplyData(value);
    voicePtr->GetSendString(infoptr);
    voicePtr->SendRequest(infoptr, true);
    voicePtr->OnStopService();
    voicePtr->Release();
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
    ASSERT_TRUE(voicePtr->settingsCallback_  == nullptr);
    ASSERT_TRUE(voicePtr->connectCallback_  == nullptr);
    ASSERT_TRUE(voicePtr->mRemoteObject == nullptr);
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_VoiceAssistantSwitchObserver_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_VoiceAssistantSwitchObserver_001, TestSize.Level0)
{
    sptr<AAFwk::IDataAbilityObserver> settingsCallback = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
    settingsCallback->OnChange();
    ASSERT_TRUE(settingsCallback != nullptr);
}

/**
 * @tc.number   Telephony_VoiceAssistantConnectCallback_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_VoiceAssistantConnectCallback_001, TestSize.Level1)
{
    const int32_t accountId = 1;
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(accountId);
    AppExecFwk::ElementName element;
    sptr<AAFwk::IAbilityConnection> connectCallback = sptr<VoiceAssistantConnectCallback>::MakeSptr(accountId);
    connectCallback->OnAbilityConnectDone(element, remoteObject, accountId);
    connectCallback->OnAbilityDisconnectDone(element, accountId);
    ASSERT_TRUE(connectCallback != nullptr);
}

/**
 * @tc.number   Telephony_VoiceAssistantRingSubscriber_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_VoiceAssistantRingSubscriber_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
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
    subscriber->OnReceiveEvent(eventData);
    subscriber->Initial();
    subscriber->Release();
    ASSERT_TRUE(subscriber->subscriber_ == nullptr);
    event = voicePtr->CONTROL_SWITCH_STATE_CHANGE_EVENT;
    want.SetAction(event);
    want.SetParam(event, event);
    eventData.SetWant(want);
    ASSERT_NO_THROW(subscriber->OnReceiveEvent(eventData));
    std::string key1 = "publisher_name";
    std::string value1 = "remote_object_send_request";
    want.SetParam(key1, value1);
    eventData.SetWant(want);
    ASSERT_NO_THROW(subscriber->OnReceiveEvent(eventData));
    std::string value2 = "connect_voice_assistant_ability_failed";
    want.SetParam(key1, value2);
    eventData.SetWant(want);
    ASSERT_NO_THROW(subscriber->OnReceiveEvent(eventData));
    want.SetParam(voicePtr->IS_PLAY_RING, voicePtr->SWITCH_TURN_ON);
    eventData.SetWant(want);
    ASSERT_NO_THROW(subscriber->OnReceiveEvent(eventData));
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   Telephony_VoiceAssistantRingSubscriber_002
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_VoiceAssistantRingSubscriber_002, Function | MediumTest | Level3)
{
    std::string event = "test.test.test.test";
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(event);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriber = std::make_shared<VoiceAssistantRingSubscriber>(subscribeInfo);
    DialParaInfo info;
    sptr<CallBase> ringingCall = new CSCall(info);
    ringingCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    CallObjectManager::AddOneCallObject(ringingCall);
    ASSERT_NO_THROW(subscriber->PlayRing());
    std::string url = "abc.mp4";
    ContactInfo contactInfo = ringingCall->GetCallerInfo();
    memcpy_s(contactInfo.personalNotificationRingtone, FILE_PATH_MAX_LEN, url.c_str(), url.length());
    ringingCall->SetCallerInfo(contactInfo);
    ASSERT_NO_THROW(subscriber->PlayRing());
    AccessToken token;
    ASSERT_NO_THROW(subscriber->PlayRing());
    CallObjectManager::DeleteOneCallObject(ringingCall->GetCallID());
}
}
}
