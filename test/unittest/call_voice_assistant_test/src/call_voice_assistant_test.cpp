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
 * @tc.number   CallVoiceAssistantManager_GetInstance_001
 * @tc.name     test GetInstance normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetInstance_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::shared_ptr<CallVoiceAssistantManager> voicePtr2 = CallVoiceAssistantManager::GetInstance();
    ASSERT_TRUE(voicePtr == voicePtr2);
}

/**
 * @tc.number   CallVoiceAssistantManager_IsStartVoiceBroadcast_001
 * @tc.name     test IsStartVoiceBroadcast first query branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_IsStartVoiceBroadcast_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isQueryedBroadcastSwitch = false;
    voicePtr->isBroadcastSwitchOn = false;
    voicePtr->IsStartVoiceBroadcast();
    ASSERT_TRUE(voicePtr->isQueryedBroadcastSwitch);
}

/**
 * @tc.number   CallVoiceAssistantManager_IsStartVoiceBroadcast_002
 * @tc.name     test IsStartVoiceBroadcast cached branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_IsStartVoiceBroadcast_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isQueryedBroadcastSwitch = true;
    voicePtr->isBroadcastSwitchOn = true;
    bool result = voicePtr->IsStartVoiceBroadcast();
    ASSERT_TRUE(result);
}

/**
 * @tc.number   CallVoiceAssistantManager_Initial_001
 * @tc.name     test Initial return nullptr branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_Initial_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto result = voicePtr->Initial();
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.number   CallVoiceAssistantManager_IsSwitchOn_001
 * @tc.name     test IsSwitchOn hicar connected branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_IsSwitchOn_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->IsSwitchOn("test_switch");
}

/**
 * @tc.number   CallVoiceAssistantManager_RegisterListenSwitchState_001
 * @tc.name     test RegisterListenSwitchState settingsCallback null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_RegisterListenSwitchState_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->settingsCallback_ = nullptr;
    voicePtr->RegisterListenSwitchState();
}

/**
 * @tc.number   CallVoiceAssistantManager_RegisterListenSwitchState_002
 * @tc.name     test RegisterListenSwitchState already registered branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_RegisterListenSwitchState_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->settingsCallback_ = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
    bool result = voicePtr->RegisterListenSwitchState();
    ASSERT_TRUE(result);
}

/**
 * @tc.number   CallVoiceAssistantManager_UnRegisterListenSwitchState_001
 * @tc.name     test UnRegisterListenSwitchState settingsCallback null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UnRegisterListenSwitchState_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->settingsCallback_ = nullptr;
    bool result = voicePtr->UnRegisterListenSwitchState();
    ASSERT_FALSE(result);
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_001
 * @tc.name     test ConnectAbility connectCallback null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = nullptr;
    voicePtr->ConnectAbility(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_002
 * @tc.name     test ConnectAbility already connected branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->ConnectAbility(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_DisconnectAbility_001
 * @tc.name     test DisconnectAbility connectCallback null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_DisconnectAbility_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = nullptr;
    bool result = voicePtr->DisconnectAbility();
    ASSERT_FALSE(result);
}

/**
 * @tc.number   CallVoiceAssistantManager_PublishCommonEvent_001
 * @tc.name     test PublishCommonEvent connectCallback null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_PublishCommonEvent_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = nullptr;
    voicePtr->PublishCommonEvent(true, "test");
}

/**
 * @tc.number   CallVoiceAssistantManager_PublishCommonEvent_002
 * @tc.name     test PublishCommonEvent isConnect branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_PublishCommonEvent_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->isConnectService = true;
    voicePtr->PublishCommonEvent(true, "test");
}

/**
 * @tc.number   CallVoiceAssistantManager_OnStartService_001
 * @tc.name     test OnStartService remoteObject not null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_OnStartService_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->accountIds[1] = std::make_shared<IncomingContactInformation>();
    voicePtr->OnStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_OnStartService_002
 * @tc.name     test OnStartService remoteObject null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_OnStartService_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->remoteObject_ = nullptr;
    voicePtr->OnStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_ProcessStartService_001
 * @tc.name     test ProcessStartService incoming branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ProcessStartService_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isBroadcastSwitchOn = false;
    voicePtr->isControlSwitchOn = true;
    voicePtr->ProcessStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_ProcessStartService_002
 * @tc.name     test ProcessStartService dialing branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ProcessStartService_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isBroadcastSwitchOn = false;
    voicePtr->isControlSwitchOn = true;
    voicePtr->ProcessStartService("dial", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_ProcessStartService_003
 * @tc.name     test ProcessStartService all switches off branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ProcessStartService_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isBroadcastSwitchOn = false;
    voicePtr->isControlSwitchOn = false;
    voicePtr->ProcessStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_OnStopService_001
 * @tc.name     test OnStopService isDestructor true branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_OnStopService_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->OnStopService(true);
}

/**
 * @tc.number   CallVoiceAssistantManager_OnStopService_002
 * @tc.name     test OnStopService isDestructor false branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_OnStopService_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->OnStopService(false);
}

/**
 * @tc.number   CallVoiceAssistantManager_QueryValue_001
 * @tc.name     test QueryValue datashareHelper null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_QueryValue_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string value = "";
    int ret = voicePtr->QueryValue("test_key", value);
    EXPECT_TRUE(ret == -1 || ret == 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_GetContactInfo_001
 * @tc.name     test GetContactInfo null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetContactInfo_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds.clear();
    auto info = voicePtr->GetContactInfo(1);
    ASSERT_FALSE(info == nullptr);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateNumberLocation_001
 * @tc.name     test UpdateNumberLocation iterator end branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateNumberLocation_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds.clear();
    voicePtr->UpdateNumberLocation("test_location", 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateNumberLocation_002
 * @tc.name     test UpdateNumberLocation info null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateNumberLocation_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds[1] = nullptr;
    voicePtr->UpdateNumberLocation("test_location", 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateNumberLocation_003
 * @tc.name     test UpdateNumberLocation dialing branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateNumberLocation_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_DIALING;
    voicePtr->accountIds[1] = info;
    voicePtr->UpdateNumberLocation("test_location", 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateRemoteObject_001
 * @tc.name     test UpdateRemoteObject callId mismatch branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateRemoteObject_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    auto callback = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->UpdateRemoteObject(remoteObject, 1, callback);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateRemoteObject_002
 * @tc.name     test UpdateRemoteObject iterator end branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateRemoteObject_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    voicePtr->accountIds.clear();
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    auto callback = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->UpdateRemoteObject(remoteObject, 1, callback);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateContactInfo_001
 * @tc.name     test UpdateContactInfo call null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateContactInfo_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->UpdateContactInfo(999);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateContactInfo_002
 * @tc.name     test UpdateContactInfo iterator end branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateContactInfo_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo info;
    sptr<CallBase> call = std::make_unique<CSCall>(info).release();
    call->SetCallId(1);
    CallObjectManager::AddOneCallObject(call);
    voicePtr->accountIds.clear();
    voicePtr->UpdateContactInfo(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateContactInfo_003
 * @tc.name     test UpdateContactInfo info null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateContactInfo_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo info;
    sptr<CallBase> call = std::make_unique<CSCall>(info).release();
    call->SetCallId(1);
    CallObjectManager::AddOneCallObject(call);
    voicePtr->accountIds[1] = nullptr;
    voicePtr->UpdateContactInfo(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateContactInfo_004
 * @tc.name     test UpdateContactInfo dialing branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateContactInfo_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo info;
    sptr<CallBase> call = std::make_unique<CSCall>(info).release();
    call->SetCallId(1);
    CallObjectManager::AddOneCallObject(call);
    auto contactInfo = std::make_shared<IncomingContactInformation>();
    contactInfo->call_status = (int32_t)TelCallState::CALL_STATUS_DIALING;
    voicePtr->accountIds[1] = contactInfo;
    voicePtr->UpdateContactInfo(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_MuteRinger_001
 * @tc.name     test MuteRinger nowCallId invalid branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_MuteRinger_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = -1;
    voicePtr->MuteRinger();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_MuteRinger_002
 * @tc.name     test MuteRinger info null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_MuteRinger_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    voicePtr->accountIds[1] = nullptr;
    voicePtr->MuteRinger();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_001
 * @tc.name     test SendRequest remoteObject null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->remoteObject_ = nullptr;
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_002
 * @tc.name     test SendRequest info null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->SendRequest(nullptr, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_003
 * @tc.name     test SendRequest isQueryComplete false branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = false;
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_004
 * @tc.name     test SendRequest dialOrCome empty branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "";
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_005
 * @tc.name     test SendRequest numberLocation default branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_005, TestSize.Level0)
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
 * @tc.number   CallVoiceAssistantManager_GetSendString_001
 * @tc.name     test GetSendString nowInfo null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetSendString_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto result = voicePtr->GetSendString(nullptr);
    ASSERT_TRUE(result == u"");
}

/**
 * @tc.number   CallVoiceAssistantManager_GetSendString_002
 * @tc.name     test GetSendString normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetSendString_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->dialOrCome = "come";
    info->call_status = 1;
    info->callId = 1;
    info->phoneNumber = "123456";
    info->isContact = "1";
    info->incomingName = "test";
    info->accountId = 0;
    info->stopBroadcasting = 0;
    info->numberLocation = "test";
    auto result = voicePtr->GetSendString(info);
    ASSERT_TRUE(result != u"");
}

/**
 * @tc.number   CallVoiceAssistantManager_GetSendString_003
 * @tc.name     test GetSendString empty fields branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetSendString_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->dialOrCome = "come";
    info->incomingName = "";
    info->numberLocation = "";
    info->phoneNumber = "";
    auto result = voicePtr->GetSendString(info);
    ASSERT_TRUE(result != u"");
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateReplyData_001
 * @tc.name     test UpdateReplyData normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateReplyData_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string reply =
        "{\"incomingCallVoiceControlCheckResult\":\"1\",\"incomingCallVoiceBroadcastCheckResult\":\"1\"}";
    voicePtr->UpdateReplyData(reply);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateReplyData_002
 * @tc.name     test UpdateReplyData empty branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateReplyData_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->UpdateReplyData("");
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_001
 * @tc.name     test CallStateUpdated callObjectPtr null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<CallBase> callObjectPtr = nullptr;
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_002
 * @tc.name     test CallStateUpdated TYPE_VOIP branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    callObjectPtr->SetCallType(CallType::TYPE_VOIP);
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_003
 * @tc.name     test CallStateUpdated CALL_STATUS_ACTIVE branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    callObjectPtr->SetCallId(1);
    callObjectPtr->SetCallType(CallType::TYPE_CS);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    CallObjectManager::AddOneCallObject(callObjectPtr);
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_ACTIVE);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_004
 * @tc.name     test CallStateUpdated CALL_STATUS_DIALING branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_004, TestSize.Level0)
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
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_005
 * @tc.name     test CallStateUpdated CALL_STATUS_INCOMING branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    callObjectPtr->SetCallId(1);
    callObjectPtr->SetCallType(CallType::TYPE_CS);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    CallObjectManager::AddOneCallObject(callObjectPtr);
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_006
 * @tc.name     test CallStateUpdated CALL_STATUS_DISCONNECTED branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    callObjectPtr->SetCallId(1);
    callObjectPtr->SetCallType(CallType::TYPE_CS);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTED);
    CallObjectManager::AddOneCallObject(callObjectPtr);
    voicePtr->CallStateUpdated(callObjectPtr,
        TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_DISCONNECTED);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_001
 * @tc.name     test CallStatusIncoming callId match branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    voicePtr->nowAccountId = 0;
    voicePtr->CallStatusIncoming(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_002
 * @tc.name     test CallStatusIncoming voip offhook branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    voicePtr->nowVoipCallState = (int32_t)CallStateToApp::CALL_STATE_OFFHOOK;
    voicePtr->CallStatusIncoming(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_003
 * @tc.name     test CallStatusIncoming info null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    voicePtr->nowVoipCallState = 0;
    voicePtr->accountIds[1] = nullptr;
    voicePtr->CallStatusIncoming(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_001
 * @tc.name     test CallStatusDialing info null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds[1] = nullptr;
    voicePtr->CallStatusDialing(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_002
 * @tc.name     test CallStatusDialing callId match branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    voicePtr->nowAccountId = 0;
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDialing(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_003
 * @tc.name     test CallStatusDialing normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDialing(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusActive_001
 * @tc.name     test CallStatusActive normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusActive_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->CallStatusActive(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDisconnected_001
 * @tc.name     test CallStatusDisconnected normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDisconnected_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDisconnected(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CheckTelCallState_001
 * @tc.name     test CheckTelCallState nullptr info branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CheckTelCallState_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds[1] = nullptr;
    voicePtr->CheckTelCallState(TelCallState::CALL_STATUS_INCOMING);
}

/**
 * @tc.number   CallVoiceAssistantManager_CheckTelCallState_002
 * @tc.name     test CheckTelCallState find state branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CheckTelCallState_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    voicePtr->accountIds[1] = info;
    int32_t ret = voicePtr->CheckTelCallState(TelCallState::CALL_STATUS_INCOMING);
    ASSERT_TRUE(ret == 1);
}

/**
 * @tc.number   VoiceAssistantConnectCallback_OnAbilityConnectDone_001
 * @tc.name     test OnAbilityConnectDone remoteObject null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantConnectCallback_OnAbilityConnectDone_001, TestSize.Level1)
{
    sptr<VoiceAssistantConnectCallback> callback = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    AppExecFwk::ElementName element;
    callback->OnAbilityConnectDone(element, nullptr, 0);
}

/**
 * @tc.number   VoiceAssistantConnectCallback_OnAbilityConnectDone_002
 * @tc.name     test OnAbilityConnectDone resultCode not success branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantConnectCallback_OnAbilityConnectDone_002, TestSize.Level1)
{
    sptr<VoiceAssistantConnectCallback> callback = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    AppExecFwk::ElementName element;
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    callback->OnAbilityConnectDone(element, remoteObject, -1);
}

/**
 * @tc.number   VoiceAssistantSwitchObserver_OnChange_001
 * @tc.name     test OnChange voicePtr null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantSwitchObserver_OnChange_001, TestSize.Level0)
{
    sptr<VoiceAssistantSwitchObserver> observer = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
    observer->OnChange();
}

/**
 * @tc.number   VoiceAssistantSwitchObserver_OnChange_002
 * @tc.name     test OnChange control switch already close branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantSwitchObserver_OnChange_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isControlSwitchOn = false;
    sptr<VoiceAssistantSwitchObserver> observer = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
    observer->OnChange();
}

/**
 * @tc.number   VoiceAssistantRingSubscriber_Initial_001
 * @tc.name     test Initial voicePtr null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantRingSubscriber_Initial_001, TestSize.Level0)
{
    VoiceAssistantRingSubscriber::subscriber_ = nullptr;
    VoiceAssistantRingSubscriber::Initial();
}

/**
 * @tc.number   VoiceAssistantRingSubscriber_Release_001
 * @tc.name     test Release null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantRingSubscriber_Release_001, TestSize.Level0)
{
    VoiceAssistantRingSubscriber::subscriber_ = nullptr;
    VoiceAssistantRingSubscriber::Release();
}

/**
 * @tc.number   VoiceAssistantRingSubscriber_OnReceiveEvent_001
 * @tc.name     test OnReceiveEvent voicePtr null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantRingSubscriber_OnReceiveEvent_001, TestSize.Level0)
{
    std::string event = "test.test.test.test";
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(event);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriber = std::make_shared<VoiceAssistantRingSubscriber>(subscribeInfo);
    EventFwk::CommonEventData eventData;
    AAFwk::Want want;
    want.SetAction(event);
    eventData.SetWant(want);
    subscriber->OnReceiveEvent(eventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   VoiceAssistantRingSubscriber_OnReceiveEvent_002
 * @tc.name     test OnReceiveEvent action mismatch branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantRingSubscriber_OnReceiveEvent_002, TestSize.Level0)
{
    std::string event = "usual.event.CALL_UI_REPORT_SWITCH_STATE_CHANGE";
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(event);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriber = std::make_shared<VoiceAssistantRingSubscriber>(subscribeInfo);
    EventFwk::CommonEventData eventData;
    AAFwk::Want want;
    want.SetAction("wrong.action");
    eventData.SetWant(want);
    subscriber->OnReceiveEvent(eventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_SetGetIsControlSwitchOn_001
 * @tc.name     test SetIsControlSwitchOn and GetIsControlSwitchOn
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SetGetIsControlSwitchOn_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->SetIsControlSwitchOn(true);
    ASSERT_TRUE(voicePtr->GetIsControlSwitchOn());
    voicePtr->SetIsControlSwitchOn(false);
    ASSERT_FALSE(voicePtr->GetIsControlSwitchOn());
}

/**
 * @tc.number   CallVoiceAssistantManager_GetIsPlayRing_001
 * @tc.name     test GetIsPlayRing true branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetIsPlayRing_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isPlay = "1";
    ASSERT_TRUE(voicePtr->GetIsPlayRing());
}

/**
 * @tc.number   CallVoiceAssistantManager_GetIsPlayRing_002
 * @tc.name     test GetIsPlayRing false branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetIsPlayRing_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isPlay = "0";
    ASSERT_FALSE(voicePtr->GetIsPlayRing());
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateVoipCallState_001
 * @tc.name     test UpdateVoipCallState
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateVoipCallState_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->UpdateVoipCallState(1);
    ASSERT_TRUE(voicePtr->nowVoipCallState == 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_Release_001
 * @tc.name     test Release
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_Release_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->settingsCallback_ = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
    voicePtr->remoteObject_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->isBroadcastSwitchOn = true;
    voicePtr->isControlSwitchOn = true;
    voicePtr->isQueryedBroadcastSwitch = true;
    voicePtr->isConnectService = true;
    voicePtr->broadcastCheck = "1";
    voicePtr->isPlay = "1";
    voicePtr->Release();
    EXPECT_TRUE(voicePtr->connectCallback_ == nullptr);
    EXPECT_TRUE(voicePtr->settingsCallback_ == nullptr);
    EXPECT_TRUE(voicePtr->remoteObject_ == nullptr);
    EXPECT_FALSE(voicePtr->isBroadcastSwitchOn);
    EXPECT_FALSE(voicePtr->isControlSwitchOn);
    EXPECT_FALSE(voicePtr->isQueryedBroadcastSwitch);
    EXPECT_FALSE(voicePtr->isConnectService);
    EXPECT_FALSE(voicePtr->broadcastCheck == "0");
    EXPECT_TRUE(voicePtr->isPlay == "0");
    DelayedSingleton<AudioControlManager>::GetInstance()->UnInit();
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_003
 * @tc.name     test ConnectAbility broadcast switch on branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = nullptr;
    voicePtr->isBroadcastSwitchOn = true;
    voicePtr->ConnectAbility(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_006
 * @tc.name     test SendRequest sendStr invalid branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_007
 * @tc.name     test SendRequest isNeed false branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->SendRequest(info, false);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_008
 * @tc.name     test SendRequest control check on branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_008, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->controlCheck = "1";
    voicePtr->isControlSwitchOn = true;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->SendRequest(info, true);
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_009
 * @tc.name     test SendRequest should ring branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_009, TestSize.Level0)
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
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_004
 * @tc.name     test CallStatusIncoming active state branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    voicePtr->nowVoipCallState = 0;
    auto activeInfo = std::make_shared<IncomingContactInformation>();
    activeInfo->call_status = (int32_t)TelCallState::CALL_STATUS_ACTIVE;
    voicePtr->accountIds[2] = activeInfo;
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusIncoming(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_005
 * @tc.name     test CallStatusIncoming dialing state branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    voicePtr->nowVoipCallState = 0;
    auto dialingInfo = std::make_shared<IncomingContactInformation>();
    dialingInfo->call_status = (int32_t)TelCallState::CALL_STATUS_DIALING;
    voicePtr->accountIds[2] = dialingInfo;
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusIncoming(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_006
 * @tc.name     test CallStatusIncoming call not null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    voicePtr->nowVoipCallState = 0;
    DialParaInfo paraInfo;
    sptr<CallBase> call = std::make_unique<CSCall>(paraInfo).release();
    call->SetCallId(1);
    CallObjectManager::AddOneCallObject(call);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call = call;
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusIncoming(1, 0);
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_007
 * @tc.name     test CallStatusIncoming remoteObject not null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    voicePtr->nowVoipCallState = 0;
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusIncoming(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDisconnected_002
 * @tc.name     test CallStatusDisconnected has active call branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDisconnected_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto activeInfo = std::make_shared<IncomingContactInformation>();
    activeInfo->call_status = (int32_t)TelCallState::CALL_STATUS_ACTIVE;
    voicePtr->accountIds[2] = activeInfo;
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDisconnected(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDisconnected_003
 * @tc.name     test CallStatusDisconnected has incoming call branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDisconnected_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto incomingInfo = std::make_shared<IncomingContactInformation>();
    incomingInfo->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    incomingInfo->dialOrCome = "come";
    incomingInfo->callId = 2;
    incomingInfo->accountId = 0;
    voicePtr->accountIds[2] = incomingInfo;
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDisconnected(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CheckTelCallState_003
 * @tc.name     test CheckTelCallState erase invalid branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CheckTelCallState_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds[1] = nullptr;
    voicePtr->accountIds[2] = std::make_shared<IncomingContactInformation>();
    voicePtr->CheckTelCallState(TelCallState::CALL_STATUS_INCOMING);
}

/**
 * @tc.number   VoiceAssistantSwitchObserver_OnChange_003
 * @tc.name     test OnChange control switch on to off branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantSwitchObserver_OnChange_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isControlSwitchOn = true;
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    sptr<VoiceAssistantSwitchObserver> observer = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
    observer->OnChange();
}

/**
 * @tc.number   VoiceAssistantRingSubscriber_PlayRing_001
 * @tc.name     test PlayRing with ringing call
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantRingSubscriber_PlayRing_001, TestSize.Level0)
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
    CallObjectManager::DeleteOneCallObject(ringingCall->GetCallID());
}

/**
 * @tc.number   VoiceAssistantRingSubscriber_PlayRing_002
 * @tc.name     test PlayRing with custom ringtone
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantRingSubscriber_PlayRing_002, TestSize.Level0)
{
    std::string event = "test.test.test.test";
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(event);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriber = std::make_shared<VoiceAssistantRingSubscriber>(subscribeInfo);
    DialParaInfo info;
    sptr<CallBase> ringingCall = new CSCall(info);
    ringingCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    std::string url = "custom.mp3";
    ContactInfo contactInfo = ringingCall->GetCallerInfo();
    memcpy_s(contactInfo.ringtonePath, FILE_PATH_MAX_LEN, url.c_str(), url.length());
    ringingCall->SetCallerInfo(contactInfo);
    CallObjectManager::AddOneCallObject(ringingCall);
    ASSERT_NO_THROW(subscriber->PlayRing());
    CallObjectManager::DeleteOneCallObject(ringingCall->GetCallID());
}

/**
 * @tc.number   CallVoiceAssistantManager_ProcessStartService_004
 * @tc.name     test ProcessStartService broadcast switch on branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ProcessStartService_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isBroadcastSwitchOn = true;
    voicePtr->isControlSwitchOn = false;
    voicePtr->ProcessStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_ProcessStartService_005
 * @tc.name     test ProcessStartService both switches on branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ProcessStartService_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->isBroadcastSwitchOn = true;
    voicePtr->isControlSwitchOn = true;
    voicePtr->ProcessStartService("come", 1);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateReplyData_003
 * @tc.name     test UpdateReplyData partial data branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateReplyData_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string reply = "{\"incomingCallVoiceControlCheckResult\":\"1\"}";
    voicePtr->UpdateReplyData(reply);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateReplyData_004
 * @tc.name     test UpdateReplyData broadcast check only branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateReplyData_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string reply = "{\"incomingCallVoiceBroadcastCheckResult\":\"1\"}";
    voicePtr->UpdateReplyData(reply);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_007
 * @tc.name     test CallStateUpdated ignore state branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    callObjectPtr->SetCallId(1);
    callObjectPtr->SetCallType(CallType::TYPE_CS);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_HOLDING);
    CallObjectManager::AddOneCallObject(callObjectPtr);
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_HOLDING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_DisconnectAbility_002
 * @tc.name     test DisconnectAbility connectCallback not null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_DisconnectAbility_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    bool result = voicePtr->DisconnectAbility();
    ASSERT_TRUE(result);
}

/**
 * @tc.number   CallVoiceAssistantManager_UnRegisterListenSwitchState_002
 * @tc.name     test UnRegisterListenSwitchState settingsCallback not null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UnRegisterListenSwitchState_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->settingsCallback_ = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
    bool result = voicePtr->UnRegisterListenSwitchState();
    ASSERT_TRUE(result);
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_004
 * @tc.name     test ConnectAbility hicar connected branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = nullptr;
    voicePtr->ConnectAbility(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_GetContactInfo_002
 * @tc.name     test GetContactInfo already exists branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetContactInfo_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto existingInfo = std::make_shared<IncomingContactInformation>();
    existingInfo->callId = 1;
    voicePtr->accountIds[1] = existingInfo;
    auto info = voicePtr->GetContactInfo(1);
    ASSERT_TRUE(info == existingInfo);
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateNumberLocation_004
 * @tc.name     test UpdateNumberLocation normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateNumberLocation_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->accountIds[1] = info;
    voicePtr->UpdateNumberLocation("new_location", 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateContactInfo_005
 * @tc.name     test UpdateContactInfo normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateContactInfo_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    DialParaInfo info;
    sptr<CallBase> call = std::make_unique<CSCall>(info).release();
    call->SetCallId(1);
    CallObjectManager::AddOneCallObject(call);
    auto contactInfo = std::make_shared<IncomingContactInformation>();
    contactInfo->call_status = (int32_t)TelCallState::CALL_STATUS_INCOMING;
    contactInfo->isQueryComplete = true;
    contactInfo->dialOrCome = "come";
    contactInfo->numberLocation = "test";
    voicePtr->accountIds[1] = contactInfo;
    voicePtr->UpdateContactInfo(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_MuteRinger_003
 * @tc.name     test MuteRinger normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_MuteRinger_003, TestSize.Level0)
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
 * @tc.number   CallVoiceAssistantManager_CallStatusActive_002
 * @tc.name     test CallStatusActive with remoteObject
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusActive_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    voicePtr->isQueryedBroadcastSwitch = true;
    voicePtr->CallStatusActive(1, 0);
    ASSERT_TRUE(voicePtr->remoteObject_ == nullptr);
    ASSERT_FALSE(voicePtr->isQueryedBroadcastSwitch);
}

/**
 * @tc.number   CallVoiceAssistantManager_OnStopService_003
 * @tc.name     test OnStopService with all resources
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_OnStopService_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->settingsCallback_ = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
    voicePtr->remoteObject_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->OnStopService(false);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_004
 * @tc.name     test CallStatusDialing with remoteObject
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->remoteObject_ = remoteObject;
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDialing(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_QueryValue_002
 * @tc.name     test QueryValue result processing
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_QueryValue_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    std::string value = "";
    int ret = voicePtr->QueryValue("test_key", value);
    EXPECT_TRUE(ret == -1 || ret == 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_GetSendString_004
 * @tc.name     test GetSendString with contact name empty
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_GetSendString_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->dialOrCome = "come";
    info->incomingName = "contact_name";
    info->numberLocation = "location";
    info->phoneNumber = "123456";
    auto result = voicePtr->GetSendString(info);
    ASSERT_TRUE(result != u"");
}

/**
 * @tc.number   CallVoiceAssistantManager_UpdateRemoteObject_003
 * @tc.name     test UpdateRemoteObject normal branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UpdateRemoteObject_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->accountIds[1] = info;
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    auto callback = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->UpdateRemoteObject(remoteObject, 1, callback);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   CallVoiceAssistantManager_VoiceAssistantConnectCallback_OnAbilityDisconnectDone_001
 * @tc.name     test OnAbilityDisconnectDone
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantConnectCallback_OnAbilityDisconnectDone_001, TestSize.Level1)
{
    sptr<VoiceAssistantConnectCallback> callback = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    AppExecFwk::ElementName element;
    callback->OnAbilityDisconnectDone(element, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_005
 * @tc.name     test ConnectAbility hicar bundle name branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = nullptr;
    voicePtr->ConnectAbility(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_PublishCommonEvent_003
 * @tc.name     test PublishCommonEvent isConnect false branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_PublishCommonEvent_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->isConnectService = false;
    voicePtr->PublishCommonEvent(false, "test");
}

/**
 * @tc.number   CallVoiceAssistantManager_SendRequest_010
 * @tc.name     test SendRequest stopBroadcasting not zero branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_010, TestSize.Level0)
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
 * @tc.number   CallVoiceAssistantManager_SendRequest_011
 * @tc.name     test SendRequest dialing no ring branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_SendRequest_011, TestSize.Level0)
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
 * @tc.number   CallVoiceAssistantManager_Initial_002
 * @tc.name     test Initial saManager null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_Initial_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->Initial();
}

/**
 * @tc.number   CallVoiceAssistantManager_IsSwitchOn_003
 * @tc.name     test IsSwitchOn user locked branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_IsSwitchOn_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->IsSwitchOn("test_switch");
}

/**
 * @tc.number   CallVoiceAssistantManager_RegisterListenSwitchState_003
 * @tc.name     test RegisterListenSwitchState datashareHelper null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_RegisterListenSwitchState_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->settingsCallback_ = nullptr;
    voicePtr->RegisterListenSwitchState();
}

/**
 * @tc.number   CallVoiceAssistantManager_UnRegisterListenSwitchState_003
 * @tc.name     test UnRegisterListenSwitchState datashareHelper null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_UnRegisterListenSwitchState_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->settingsCallback_ = sptr<VoiceAssistantSwitchObserver>::MakeSptr();
    voicePtr->UnRegisterListenSwitchState();
}

/**
 * @tc.number   CallVoiceAssistantManager_ConnectAbility_006
 * @tc.name     test ConnectAbility callback make sptr fail branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_ConnectAbility_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->connectCallback_ = nullptr;
    voicePtr->ConnectAbility(1);
}

/**
 * @tc.number   VoiceAssistantConnectCallback_OnAbilityConnectDone_003
 * @tc.name     test OnAbilityConnectDone success branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantConnectCallback_OnAbilityConnectDone_003, TestSize.Level1)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 1;
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = true;
    info->dialOrCome = "come";
    info->numberLocation = "test";
    voicePtr->accountIds[1] = info;
    sptr<VoiceAssistantConnectCallback> callback = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    AppExecFwk::ElementName element;
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    callback->OnAbilityConnectDone(element, remoteObject, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.number   VoiceAssistantRingSubscriber_PlayRing_003
 * @tc.name     test PlayRing system video ring branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, VoiceAssistantRingSubscriber_PlayRing_003, TestSize.Level0)
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
    CallObjectManager::DeleteOneCallObject(ringingCall->GetCallID());
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStateUpdated_008
 * @tc.name     test CallStateUpdated info not null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStateUpdated_008, TestSize.Level0)
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
    voicePtr->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusIncoming_008
 * @tc.name     test CallStatusIncoming normal flow branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusIncoming_008, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    voicePtr->nowVoipCallState = 0;
    DialParaInfo paraInfo;
    sptr<CallBase> call = std::make_unique<CSCall>(paraInfo).release();
    call->SetCallId(1);
    call->SetAccountId(0);
    CallObjectManager::AddOneCallObject(call);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call = call;
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusIncoming(1, 0);
    ASSERT_TRUE(voicePtr->nowCallId == 1);
    ASSERT_TRUE(voicePtr->nowAccountId == 0);
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDialing_005
 * @tc.name     test CallStatusDialing normal flow branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDialing_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->nowCallId = 2;
    voicePtr->nowAccountId = 1;
    auto info = std::make_shared<IncomingContactInformation>();
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDialing(1, 0);
    ASSERT_TRUE(voicePtr->nowCallId == 1);
    ASSERT_TRUE(voicePtr->nowAccountId == 0);
    ASSERT_TRUE(info->dialOrCome == "dial");
    ASSERT_TRUE(info->numberLocation == "dial");
    ASSERT_TRUE(info->phoneNumber == "dial");
    ASSERT_TRUE(info->isQueryComplete == true);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDisconnected_004
 * @tc.name     test CallStatusDisconnected last info null branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDisconnected_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    voicePtr->accountIds[1] = nullptr;
    voicePtr->CallStatusDisconnected(1, 0);
}

/**
 * @tc.number   CallVoiceAssistantManager_CallStatusDisconnected_005
 * @tc.name     test CallStatusDisconnected no active or incoming branch
 * @tc.desc     Function test
 */
HWTEST_F(CallVoiceAssistantManagerTest, CallVoiceAssistantManager_CallStatusDisconnected_005, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr == nullptr);
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = (int32_t)TelCallState::CALL_STATUS_IDLE;
    voicePtr->accountIds[1] = info;
    voicePtr->CallStatusDisconnected(1, 0);
}
}
}