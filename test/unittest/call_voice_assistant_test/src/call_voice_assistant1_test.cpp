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
 * @tc.number   CallVoiceAssistantManager_ProcessStartService_006
 * @tc.name     test ProcessStartService connect success branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ProcessStartService_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isBroadcastSwitchOn = false;
    voicePtr->isControlSwitchOn = true;
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->ProcessStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_ProcessStartService_007
 * @tc.name     test ProcessStartService connect fail branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ProcessStartService_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isBroadcastSwitchOn = false;
    voicePtr->isControlSwitchOn = true;
    voicePtr->connectCallback_ = nullptr;
    voicePtr->ProcessStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_012
 * @tc.name     test SendRequest broadcast check off branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_012, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->broadcastCheck = "0";
    voicePtr->isBroadcastSwitchOn = true;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    info->stopBroadcasting = 0;
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_013
 * @tc.name     test SendRequest broadcast switch off branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_013, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->broadcastCheck = "1";
    voicePtr->isBroadcastSwitchOn = false;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    info->stopBroadcasting = 0;
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_014
 * @tc.name     test SendRequest control check off branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_014, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->controlCheck = "0";
    voicePtr->isControlSwitchOn = true;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_015
 * @tc.name     test SendRequest control switch off branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_015, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->controlCheck = "1";
    voicePtr->isControlSwitchOn = false;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateReplyData_005
 * @tc.name     test UpdateReplyData parse error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateReplyData_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string reply = "{invalid json";
    voicePtr->UpdateReplyData(reply);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateReplyData_006
 * @tc.name     test UpdateReplyData empty value branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateReplyData_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string reply = "{\"key\":\"\"}";
    voicePtr->UpdateReplyData(reply);
}

/**
 * @tc.number   CallVoiceAssistantManager_GetSendString_005
 * @tc.name     test GetSendString isContact calculation branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetSendString_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->dialOrCome = "come";
    info->incomingName = "";
    auto result = voicePtr->GetSendString(info);
    ASSERT_TRUE(result != u"");
    ASSERT_TRUE(info->isContact == "0");
}

/**
 * @tc.number   CallVoiceAssistantManager_GetSendString_006
 * @tc.name     test GetSendString isContact true branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetSendString_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->dialOrCome = "come";
    info->incomingName = "test_name";
    auto result = voicePtr->GetSendString(info);
    EXPECT_TRUE(result != u"");
    EXPECT_FALSE(info->isContact == "1");
}

/**
 * @tc.number   CallVoiceAssistantManager_MuteRinger_004
 * @tc.name     test MuteRinger stopBroadcasting set branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_MuteRinger_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    auto info = std::make_shared<IncomingContactInformation>();
    info->stopBroadcasting = 0;
    voicePtr->accountIds[1] = info;
    voicePtr->MuteRinger();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(info->stopBroadcasting == 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_OnStartService_003
 * @tc.name     test OnStartService accountIds exist branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_OnStartService_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->accountIds[1] = info;
    voicePtr->OnStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_009
 * @tc.name     test CallStateUpdated default switch branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_009, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    callObjectPtr->SetCallId(1);
    callObjectPtr->SetCallType(CallType::TYPE_CS);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_IDLE);
    CallObjectManager::AddOneCallObject(callObjectPtr);
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_IDLE);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_OnStopService_004
 * @tc.name     test OnStopService with ringing subscriber
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_OnStopService_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    VoiceAssistantRingSubscriber::Initial();
    voicePtr->OnStopService(false);
}

/**
 * @tc.number   VoiceAssistantRingSubscriber_Initial_002
 * @tc.name     test Initial already initialized branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantRingSubscriber_Initial_002, TestSize.Level0)
{
    VoiceAssistantRingSubscriber::subscriber_ = std::make_shared<VoiceAssistantRingSubscriber>();
    bool result = VoiceAssistantRingSubscriber::Initial();
    ASSERT_TRUE(result);
    VoiceAssistantRingSubscriber::Release();
}

/**
 * @tc.number   CallVoiceAssistantManager_IsStartVoiceBroadcast_003
 * @tc.name     test IsStartVoiceBroadcast broadcast switch on branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_IsStartVoiceBroadcast_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isQueryedBroadcastSwitch = true;
    voicePtr->isBroadcastSwitchOn = false;
    bool result = voicePtr->IsStartVoiceBroadcast();
    ASSERT_FALSE(result);
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_007
 * @tc.name     test ConnectAbility return false branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = nullptr;
    voicePtr->ConnectAbility(1);
    EXPECT_TRUE(voicePtr->connectCallback_ != nullptr);
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_008
 * @tc.name     test ConnectAbility return true branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_008, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->ConnectAbility(1);
    EXPECT_TRUE(voicePtr->connectCallback_ != nullptr);
}

/**
 * @tc.number   CallVoiceAssistantManager_PublishCommonEvent_004
 * @tc.name     test PublishCommonEvent with all params
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_PublishCommonEvent_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->isConnectService = true;
    voicePtr->controlCheck = "1";
    voicePtr->broadcastCheck = "1";
    voicePtr->isPlay = "1";
    voicePtr->PublishCommonEvent(true, "test_publisher");
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateContactInfo_006
 * @tc.name     test UpdateContactInfo isContact calculation branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateContactInfo_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo info;
    sptr<CallBase> call = std::make_unique<CSCall>(info).release();
    call->SetCallId(1);
    ContactInfo contactInfo;
    contactInfo.name = "test_name";
    contactInfo.number = "123456";
    contactInfo.isQueryComplete = true;
    call->SetCallerInfo(contactInfo);
    CallObjectManager::AddOneCallObject(call);
    auto infoPtr = std::make_shared<IncomingContactInformation>();
    infoPtr->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    infoPtr->isQueryComplete = true;
    infoPtr->dialOrCome = "come";
    infoPtr->numberLocation = "test";
    voicePtr->accountIds[1] = infoPtr;
    voicePtr->UpdateContactInfo(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(infoPtr->incomingName == "test_name");
    EXPECT_TRUE(infoPtr->phoneNumber == "123456");
    EXPECT_TRUE(infoPtr->isContact == "1");
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateContactInfo_007
 * @tc.name     test UpdateContactInfo isContact false branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateContactInfo_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo info;
    sptr<CallBase> call = std::make_unique<CSCall>(info).release();
    call->SetCallId(1);
    ContactInfo contactInfo;
    contactInfo.name = "";
    contactInfo.number = "123456";
    contactInfo.isQueryComplete = true;
    call->SetCallerInfo(contactInfo);
    CallObjectManager::AddOneCallObject(call);
    auto infoPtr = std::make_shared<IncomingContactInformation>();
    infoPtr->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    infoPtr->isQueryComplete = true;
    infoPtr->dialOrCome = "come";
    infoPtr->numberLocation = "test";
    voicePtr->accountIds[1] = infoPtr;
    voicePtr->UpdateContactInfo(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(infoPtr->isContact == "0");
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_010
 * @tc.name     test CallStateUpdated VOIP call type branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_010, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    callObjectPtr->SetCallId(1);
    callObjectPtr->SetCallType(CallType::TYPE_VOIP);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    CallObjectManager::AddOneCallObject(callObjectPtr);
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_009
 * @tc.name     test CallStatusIncoming with active call branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_009, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    DialParaInfo paraInfo;
    sptr<CallBase> activeCall = std::make_unique<CSCall>(paraInfo).release();
    activeCall->SetCallId(2);
    activeCall->SetAccountId(1);
    activeCall->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    CallObjectManager::AddOneCallObject(activeCall);
    sptr<CallBase> incomingCall = std::make_unique<CSCall>(paraInfo).release();
    incomingCall->SetCallId(1);
    incomingCall->SetAccountId(0);
    CallObjectManager::AddOneCallObject(incomingCall);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call = incomingCall;
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusIncoming(1, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
    CallObjectManager::DeleteOneCallObject(2);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_010
 * @tc.name     test CallStatusIncoming nowVoipCallState offhook branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_010, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowVoipCallState = (int32_t)CallStateToApp::CALL_STATE_OFFHOOK;
    DialParaInfo paraInfo;
    sptr<CallBase> call = std::make_unique<CSCall>(paraInfo).release();
    call->SetCallId(1);
    call->SetAccountId(0);
    CallObjectManager::AddOneCallObject(call);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call = call;
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusIncoming(1, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_011
 * @tc.name     test CallStatusIncoming info call not null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_011, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> call = std::make_unique<CSCall>(paraInfo).release();
    call->SetCallId(1);
    call->SetAccountId(0);
    CallObjectManager::AddOneCallObject(call);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call = call;
    info->phoneNumber = "";
    info->numberLocation = "";
    info->dialOrCome = "";
    info->incomingName = "";
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusIncoming(1, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_016
 * @tc.name     test SendRequest info not query complete branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_016, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = false;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_017
 * @tc.name     test SendRequest dialOrCome default branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_017, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "";
    info->numberLocation = "test";
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_018
 * @tc.name     test SendRequest numberLocation default branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_018, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "default";
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_GetSendString_007
 * @tc.name     test GetSendString incomingName empty branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetSendString_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->dialOrCome = "come";
    info->incomingName = "";
    info->numberLocation = "";
    info->phoneNumber = "";
    voicePtr->GetSendString(info);
}

/**
 * @tc.number   CallVoiceAssistantManager_GetSendString_008
 * @tc.name     test GetSendString numberLocation empty branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetSendString_008, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->dialOrCome = "come";
    info->incomingName = "test";
    info->numberLocation = "";
    info->phoneNumber = "";
    voicePtr->GetSendString(info);
}

/**
 * @tc.number   CallVoiceAssistantManager_GetSendString_009
 * @tc.name     test GetSendString phoneNumber empty branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetSendString_009, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->dialOrCome = "come";
    info->incomingName = "test";
    info->numberLocation = "loc";
    info->phoneNumber = "";
    voicePtr->GetSendString(info);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDisconnected_006
 * @tc.name     test CallStatusDisconnected comeCallId not fail branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDisconnected_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> activeCall = std::make_unique<CSCall>(paraInfo).release();
    activeCall->SetCallId(2);
    activeCall->SetAccountId(1);
    activeCall->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    CallObjectManager::AddOneCallObject(activeCall);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    info->dialOrCome = "come";
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDisconnected(1, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(2);
}

/**
 * @tc.number   CallVoiceAssistantManager_OnStopService_005
 * @tc.name     test OnStopService isDestructor true branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_OnStopService_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->OnStopService(true);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_011
 * @tc.name     test CallStateUpdated callObjectPtr null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_011, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<CallBase> callObjectPtr = nullptr;
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_012
 * @tc.name     test CallStateUpdated callState neither active nor dialing branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_012, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    callObjectPtr->SetCallId(1);
    callObjectPtr->SetCallType(CallType::TYPE_CS);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
    CallObjectManager::AddOneCallObject(callObjectPtr);
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_DIALING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_IsStartVoiceBroadcast_004
 * @tc.name     test IsStartVoiceBroadcast isQueryedBroadcastSwitch false branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_IsStartVoiceBroadcast_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isQueryedBroadcastSwitch = false;
    bool result = voicePtr->IsStartVoiceBroadcast();
    ASSERT_FALSE(result);
}

/**
 * @tc.number   CallVoiceAssistantManager_ProcessStartService_008
 * @tc.name     test ProcessStartService both switches off branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ProcessStartService_008, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isBroadcastSwitchOn = false;
    voicePtr->isControlSwitchOn = false;
    voicePtr->ProcessStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateReplyData_007
 * @tc.name     test UpdateReplyData control key exists branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateReplyData_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string reply = "{\"control\":\"1\"}";
    voicePtr->UpdateReplyData(reply);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateReplyData_008
 * @tc.name     test UpdateReplyData broadcast key exists branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateReplyData_008, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string reply = "{\"broadcast\":\"1\"}";
    voicePtr->UpdateReplyData(reply);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateReplyData_009
 * @tc.name     test UpdateReplyData both keys exist branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateReplyData_009, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string reply = "{\"control\":\"1\",\"broadcast\":\"1\"}";
    voicePtr->UpdateReplyData(reply);
}

/**
 * @tc.number   VoiceAssistantRingSubscriber_OnReceiveEvent_008
 * @tc.name     test OnReceiveEvent action mismatch branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantRingSubscriber_OnReceiveEvent_008, TestSize.Level0)
{
    std::string event = "test.event";
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(event);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriber = std::make_shared<VoiceAssistantRingSubscriber>(subscribeInfo);
    EventFwk::CommonEventData eventData;
    AAFwk::Want want;
    want.SetAction("other.event");
    eventData.SetWant(want);
    subscriber->OnReceiveEvent(eventData);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDisconnected_007
 * @tc.name     test CallStatusDisconnected accountIds end branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDisconnected_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds.clear();
    voicePtr->CallStatusDisconnected(999, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_006
 * @tc.name     test CallStatusDialing info call_status is dialing branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_DIALING;
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDialing(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_019
 * @tc.name     test SendRequest isNeed false branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_019, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->controlCheck = "0";
    voicePtr->isControlSwitchOn = false;
    voicePtr->broadcastCheck = "0";
    voicePtr->isBroadcastSwitchOn = false;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_020
 * @tc.name     test SendRequest should ring and stopBroadcasting 0 branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_020, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->broadcastCheck = "1";
    voicePtr->isBroadcastSwitchOn = true;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    info->stopBroadcasting = 0;
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_021
 * @tc.name     test SendRequest stopBroadcasting 1 branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_021, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->broadcastCheck = "1";
    voicePtr->isBroadcastSwitchOn = true;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    info->stopBroadcasting = 1;
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_022
 * @tc.name     test SendRequest dialing branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_022, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->broadcastCheck = "1";
    voicePtr->isBroadcastSwitchOn = true;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "dial";
    info->numberLocation = "test";
    info->stopBroadcasting = 0;
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_009
 * @tc.name     test ConnectAbility ret not success and isBroadcastSwitchOn true branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_009, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isBroadcastSwitchOn = true;
    voicePtr->ConnectAbility(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_OnStartService_004
 * @tc.name     test OnStartService remoteObject null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_OnStartService_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->remoteObject_ = nullptr;
    voicePtr->OnStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_OnStartService_005
 * @tc.name     test OnStartService accountIds not exist branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_OnStartService_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->accountIds[999] = nullptr;
    voicePtr->OnStartService("come", 999);
}

/**
 * @tc.number   CallVoiceAssistantManager_MuteRinger_005
 * @tc.name     test MuteRinger info null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_MuteRinger_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    voicePtr->accountIds[1] = nullptr;
    voicePtr->MuteRinger();
}

/**
 * @tc.number   CallVoiceAssistantManager_MuteRinger_006
 * @tc.name     test MuteRinger nowCallId fail code branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_MuteRinger_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = -1;
    voicePtr->MuteRinger();
}

/**
 * @tc.number   CallVoiceAssistantManager_PublishCommonEvent_005
 * @tc.name     test PublishCommonEvent isConnectService false branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_PublishCommonEvent_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isConnectService = false;
    voicePtr->PublishCommonEvent(true, "test_publisher");
}

/**
 * @tc.number   CallVoiceAssistantManager_PublishCommonEvent_006
 * @tc.name     test PublishCommonEvent controlCheck off branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_PublishCommonEvent_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isConnectService = true;
    voicePtr->controlCheck = "0";
    voicePtr->broadcastCheck = "1";
    voicePtr->PublishCommonEvent(true, "test_publisher");
}

/**
 * @tc.number   CallVoiceAssistantManager_PublishCommonEvent_007
 * @tc.name     test PublishCommonEvent broadcastCheck off branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_PublishCommonEvent_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isConnectService = true;
    voicePtr->controlCheck = "1";
    voicePtr->broadcastCheck = "0";
    voicePtr->PublishCommonEvent(true, "test_publisher");
}

/**
 * @tc.number   CallVoiceAssistantManager_PublishCommonEvent_008
 * @tc.name     test PublishCommonEvent isPlay off branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_PublishCommonEvent_008, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isConnectService = true;
    voicePtr->controlCheck = "1";
    voicePtr->broadcastCheck = "1";
    voicePtr->isPlay = "0";
    voicePtr->PublishCommonEvent(true, "test_publisher");
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_012
 * @tc.name     test CallStatusIncoming accountIds not exist branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_012, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds.clear();
    voicePtr->CallStatusIncoming(999, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_013
 * @tc.name     test CallStatusIncoming info null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_013, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds[1] = nullptr;
    voicePtr->CallStatusIncoming(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_014
 * @tc.name     test CallStatusIncoming info call_status dialing branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_014, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_DIALING;
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusIncoming(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_007
 * @tc.name     test CallStatusDialing accountIds null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds[1] = nullptr;
    voicePtr->CallStatusDialing(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_008
 * @tc.name     test CallStatusDialing nowCallId not equal callId branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_008, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->accountIds[1] = std::make_shared<IncomingContactInformation>();
    voicePtr->CallStatusDialing(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_009
 * @tc.name     test CallStatusDialing call null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_009, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call = nullptr;
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDialing(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_010
 * @tc.name     test CallStatusDialing accountIds not exist branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_010, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds.clear();
    voicePtr->CallStatusDialing(999, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_011
 * @tc.name     test CallStatusDialing info null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_011, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    voicePtr->accountIds[1] = nullptr;
    voicePtr->CallStatusDialing(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_012
 * @tc.name     test CallStatusDialing info call_status dialing branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_012, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_DIALING;
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDialing(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_MuteRinger_007
 * @tc.name     test MuteRinger nowCallId FAIL_CODE branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_MuteRinger_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = -1;
    voicePtr->MuteRinger();
}

/**
 * @tc.number   CallVoiceAssistantManager_MuteRinger_008
 * @tc.name     test MuteRinger info not null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_MuteRinger_008, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    auto info = std::make_shared<IncomingContactInformation>();
    info->stopBroadcasting = 0;
    voicePtr->accountIds[1] = info;
    voicePtr->MuteRinger();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_013
 * @tc.name     test CallStateUpdated info not null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_013, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    callObjectPtr->SetCallId(1);
    callObjectPtr->SetCallType(CallType::TYPE_CS);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    callObjectPtr->SetAccountId(0);
    CallObjectManager::AddOneCallObject(callObjectPtr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    voicePtr->accountIds[1] = info;
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDisconnected_008
 * @tc.name     test CallStatusDisconnected active call exists branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDisconnected_008, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> activeCall = std::make_unique<CSCall>(paraInfo).release();
    activeCall->SetCallId(2);
    activeCall->SetAccountId(1);
    activeCall->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    CallObjectManager::AddOneCallObject(activeCall);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    info->dialOrCome = "come";
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDisconnected(1, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(2);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDisconnected_009
 * @tc.name     test CallStatusDisconnected comeCallId not fail branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDisconnected_009, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> incomingCall = std::make_unique<CSCall>(paraInfo).release();
    incomingCall->SetCallId(1);
    incomingCall->SetAccountId(0);
    incomingCall->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    CallObjectManager::AddOneCallObject(incomingCall);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    info->dialOrCome = "come";
    voicePtr->accountIds[0] = info;
    voicePtr->CallStatusDisconnected(2, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDisconnected_010
 * @tc.name     test CallStatusDisconnected nowInfo dialOrCome incoming branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDisconnected_010, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->dialOrCome = "come";
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDisconnected(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CheckTelCallState_004
 * @tc.name     test CheckTelCallState accountIds end branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CheckTelCallState_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds.clear();
    voicePtr->CheckTelCallState(TelCallState::CALL_STATUS_INCOMING);
}

/**
 * @tc.number   CallVoiceAssistantManager_CheckTelCallState_005
 * @tc.name     test CheckTelCallState id second null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CheckTelCallState_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds[1] = nullptr;
    voicePtr->CheckTelCallState(TelCallState::CALL_STATUS_INCOMING);
}

/**
 * @tc.number   CallVoiceAssistantManager_CheckTelCallState_006
 * @tc.name     test CheckTelCallState call_status match branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CheckTelCallState_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    voicePtr->accountIds[1] = info;
    int result = voicePtr->CheckTelCallState(TelCallState::CALL_STATUS_INCOMING);
    ASSERT_TRUE(result == 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_QueryValue_004
 * @tc.name     test QueryValue datashareHelper null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_QueryValue_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string value = "";
    voicePtr->QueryValue("test_key", value);
}

/**
 * @tc.number   CallVoiceAssistantManager_QueryValue_005
 * @tc.name     test QueryValue result null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_QueryValue_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string value = "";
    voicePtr->QueryValue("non_existent_key", value);
}

/**
 * @tc.number   CallVoiceAssistantManager_QueryValue_006
 * @tc.name     test QueryValue rowCount 0 branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_QueryValue_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string value = "";
    voicePtr->QueryValue("empty_key", value);
}

/**
 * @tc.number   CallVoiceAssistantManager_QueryValue_007
 * @tc.name     test QueryValue GoToFirstRow fail branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_QueryValue_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string value = "";
    voicePtr->QueryValue("invalid_key", value);
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_010
 * @tc.name     test ConnectAbility ret success branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_010, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->ConnectAbility(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_011
 * @tc.name     test ConnectAbility connectCallback null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_011, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = nullptr;
    voicePtr->ConnectAbility(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_012
 * @tc.name     test ConnectAbility isConnect true branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_012, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->ConnectAbility(1);
}
} // namespace Telephony
} // namespace OHOS
