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
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_CallVoiceAssistantManager_001, Function | MediumTest | Level3)
{
    const int32_t CALL_ID = -1;
    const int32_t ACCOUNT_ID = -1;
    const std::string INCOMING = "come";
    const std::string DIALING = "dial";
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = TelCallState::CALL_STATUS_DIALING;
    TelCallState nextState = TelCallState::CALL_STATUS_INCOMING;
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    if (voicePtr == nullptr) {
        TELEPHONY_LOGE("voicePtr is nullptr");
        return;
    }
    voicePtr->mInstance_ = voicePtr;
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
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
    ASSERT_TRUE(voicePtr->settingsCallback_  == nullptr);
    ASSERT_TRUE(voicePtr->connectCallback_  == nullptr);
    ASSERT_TRUE(voicePtr->mRemoteObject == nullptr);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_002
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_CallVoiceAssistantManager_002, Function | MediumTest | Level3)
{
    const int32_t CALL_ID = -1;
    const int32_t ACCOUNT_ID = -1;
    const std::string CONTROL_SWITCH = "incoming_call_voice_control_switch";
    const std::string INCOMING = "come";
    const std::string DIALING = "dial";
    std::string value = "";
    ContactInfo contactInfo;
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(ACCOUNT_ID);
    if (voicePtr == nullptr) {
        TELEPHONY_LOGE("voicePtr is nullptr");
        return;
    }
    voicePtr->mInstance_ = voicePtr;
    voicePtr->GetMutex();
    voicePtr->SetIsControlSwitchOn(true);
    voicePtr->GetIsControlSwitchOn();
    voicePtr->GetIsPlayRing();
    voicePtr->UpdateVoipCallState(ACCOUNT_ID);
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
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
    ASSERT_TRUE(voicePtr->settingsCallback_  == nullptr);
    ASSERT_TRUE(voicePtr->connectCallback_  == nullptr);
    ASSERT_TRUE(voicePtr->mRemoteObject == nullptr);
}

/**
 * @tc.number   Telephony_VoiceAssistantSwitchObserver_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_VoiceAssistantSwitchObserver_001, Function | MediumTest | Level3)
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
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_VoiceAssistantConnectCallback_001, Function | MediumTest | Level3)
{
    const int32_t ACCOUNT_ID = 1;
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(ACCOUNT_ID);
    AppExecFwk::ElementName element;
    sptr<AAFwk::IAbilityConnection> connectCallback = sptr<VoiceAssistantConnectCallback>::MakeSptr(ACCOUNT_ID);
    connectCallback->OnAbilityConnectDone(element, remoteObject, ACCOUNT_ID);
    connectCallback->OnAbilityDisconnectDone(element, ACCOUNT_ID);
    ASSERT_TRUE(connectCallback != nullptr);
}

/**
 * @tc.number   Telephony_VoiceAssistantRingSubscriber_001
 * @tc.name     test normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, Telephony_VoiceAssistantRingSubscriber_001, Function | MediumTest | Level3)
{
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
}

}
}
