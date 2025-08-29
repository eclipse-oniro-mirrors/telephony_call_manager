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

#include "call_manager_special_mock.h"
#include <thread>
#include <chrono>
#include "call_manager_connect.h"
#include "call_voice_assistant_manager.h"
#include "spam_call_adapter.h"
#include "bluetooth_call_client.h"
#include "bluetooth_call_manager.h"
#include "bluetooth_call_service.h"
#include "bluetooth_connection.h"
#include "iremote_broker.h"
#include "call_ability_connect_callback.h"
#include "call_ability_report_proxy.h"
#include "call_connect_ability.h"
#include "call_control_manager.h"
#include "call_manager_client.h"
#include "call_manager_hisysevent.h"
#include "call_number_utils.h"
#include "call_policy.h"
#include "call_records_manager.h"
#include "call_request_event_handler_helper.h"
#include "call_request_handler.h"
#include "call_request_process.h"
#include "call_setting_manager.h"
#include "call_state_report_proxy.h"
#include "call_status_manager.h"
#include "cellular_call_connection.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "cs_call.h"
#include "cs_conference.h"
#include "distributed_call_manager.h"
#include "gtest/gtest.h"
#include "i_voip_call_manager_service.h"
#include "ims_call.h"
#include "ims_conference.h"
#include "incoming_call_notification.h"
#include "missed_call_notification.h"
#include "ott_call.h"
#include "ott_conference.h"
#include "reject_call_sms.h"
#include "report_call_info_handler.h"
#include "satellite_call.h"
#include "surface_utils.h"
#include "telephony_errors.h"
#include "telephony_hisysevent.h"
#include "telephony_log_wrapper.h"
#include "video_call_state.h"
#include "video_control_manager.h"
#include "voip_call_manager_proxy.h"
#include "voip_call.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"
#include "number_identity_data_base_helper.h"
#include "call_ability_callback_death_recipient.h"
#include "app_state_observer.h"
#include "call_ability_callback_proxy.h"
#include "super_privacy_manager_client.h"
#include "call_status_callback.h"
#include "satellite_call_control.h"
#include "proximity_sensor.h"
#include "status_bar.h"
#include "wired_headset.h"
#include "call_status_policy.h"
#include "call_superprivacy_control_manager.h"
#include "bluetooth_hfp_ag.h"
#include "call_manager_service.h"
#include "telephony_types.h"
#include "telephony_permission.h"
#include "voip_call_connection.h"
#include "number_identity_service.h"
#include "bluetooth_call.h"
#include "bluetooth_call_connection.h"
#include "bluetooth_call_state.h"
#include "call_broadcast_subscriber.h"
#include "call_incoming_filter_manager.h"
#include "call_object_manager.h"
#include "call_wired_headset.h"
#include "fold_status_manager.h"
#include "audio_control_manager.h"
#include "call_state_processor.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
const int PERMS_NUM = 6;

class SpecialBranch0Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SpecialBranch0Test::SetUpTestCase()
{
    const char *perms[PERMS_NUM] = {
        "ohos.permission.PLACE_CALL",
        "ohos.permission.ANSWER_CALL",
        "ohos.permission.SET_TELEPHONY_STATE",
        "ohos.permission.GET_TELEPHONY_STATE",
        "ohos.permission.READ_CALL_LOG",
        "ohos.permission.WRITE_CALL_LOG",
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = PERMS_NUM,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "SpecialBranch0Test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void SpecialBranch0Test::TearDownTestCase()
{
}

void SpecialBranch0Test::SetUp()
{
}

void SpecialBranch0Test::TearDown()
{
    sleep(1);
}

/**
 * @tc.number   Telephony_NumberIdentityServiceHelper_002
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_NumberIdentityServiceHelper_002, TestSize.Level0)
{
    auto &help = DelayedRefSingleton<NumberIdentityServiceHelper>::GetInstance();
    ConnectedCallback onConnected = [&help](sptr<IRemoteObject> remote) {
        TELEPHONY_LOGI("NumberIdentityService async notify end.");
        if (help.working_) {
            TELEPHONY_LOGI("NumberIdentityService notify task is working, skip this notify.");
        }
    };
    DisconnectedCallback onDisconnected = [&help]() {
        TELEPHONY_LOGI("NumberIdentityService notify task disconnected.");
        if (help.working_) {
            TELEPHONY_LOGI("NumberIdentityService notify task is working, skip this notify.");
        }
    };
    int ret = help.Connect(onConnected, onDisconnected);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_NumberIdentityServiceHelper_003
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_NumberIdentityServiceHelper_003, TestSize.Level0)
{
    auto &help = DelayedRefSingleton<NumberIdentityServiceHelper>().GetInstance();
    help.working_ = true;
    help.NotifyNumberMarkDataUpdate();
    ASSERT_TRUE(help.connection_ != nullptr);
}

/**
 * @tc.number   Telephony_NumberIdentityConnection_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_NumberIdentityConnection_001, TestSize.Level0)
{
    sptr<NumberIdentityConnection> connection = new (std::nothrow) NumberIdentityConnection(
            [](const sptr<IRemoteObject> &remoteObject) {}, []() {});
    connection->disconnectedCallback_ = nullptr;
    AppExecFwk::ElementName element;
    sptr<OHOS::IRemoteObject> failRemoteObj = new MockRemoteObject(-1);
    connection->OnAbilityConnectDone(element, failRemoteObj, -1);

    auto proxy = std::make_shared<NumberIdentityServiceProxy>(failRemoteObj);
    proxy->NotifyNumberMarkDataUpdate();

    auto &help = DelayedRefSingleton<NumberIdentityServiceHelper>().GetInstance();
    help.working_ = false;
    proxy->NotifyNumberMarkDataUpdate();

    help.connection_ = nullptr;
    help.Disconnect();
    ASSERT_TRUE(connection != nullptr);
}

/**
 * @tc.number   Telephony_CallManagerHisysevent_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerHisysevent_001, TestSize.Level0)
{
    std::shared_ptr<CallManagerHisysevent> callManagerHisysevent = std::make_shared<CallManagerHisysevent>();
    std::string errorDesc;
    callManagerHisysevent->GetErrorDescription(static_cast<int32_t>(CALL_ERR_PHONE_NUMBER_EMPTY), errorDesc);
    callManagerHisysevent->GetErrorDescription(static_cast<int32_t>(CALL_ERR_UNKNOW_DIAL_TYPE), errorDesc);
    callManagerHisysevent->GetErrorDescription(static_cast<int32_t>(TELEPHONY_ERR_LOCAL_PTR_NULL), errorDesc);
    callManagerHisysevent->GetErrorDescription(static_cast<int32_t>(CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE), errorDesc);
    callManagerHisysevent->GetErrorDescription(-1, errorDesc);
    ASSERT_TRUE(callManagerHisysevent != nullptr);
}

/**
 * @tc.number   Telephony_CallSettingManager_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallSettingManager_001, TestSize.Level1)
{
    std::shared_ptr<CallSettingManager> callSettingManager = std::make_shared<CallSettingManager>();
    ASSERT_TRUE(callSettingManager != nullptr);
    callSettingManager->cellularCallConnectionPtr_ = nullptr;
    callSettingManager->GetCallWaiting(0);
    callSettingManager->SetCallWaiting(0, false);
    int32_t ret = callSettingManager->GetImsFeatureValue(0, FeatureType::TYPE_VOICE_OVER_LTE);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_001, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    voicePtr->mInstance_ = nullptr;
    voicePtr->GetInstance();
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_002
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_002, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    ASSERT_FALSE(voicePtr->IsSwitchOn("1"));
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_003
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_003, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    voicePtr->PublishCommonEvent(false, "a");
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_004
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_004, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    voicePtr->SendRequest(nullptr, false);
    auto info = std::make_shared<IncomingContactInformation>();
    info->isQueryComplete = false;
    voicePtr->SendRequest(info, false);
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_005
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_005, TestSize.Level0)
{
    auto voicePtr = std::make_shared<VoiceAssistantConnectCallback>(1);
    AppExecFwk::ElementName element;
    voicePtr->OnAbilityConnectDone(element, nullptr, 0);
    sptr<IRemoteObject> remoteObject = sptr<VoiceAssistantConnectCallback>::MakeSptr(1);
    voicePtr->OnAbilityConnectDone(element, remoteObject, -1);
    ASSERT_TRUE(voicePtr != nullptr);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_006
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_006, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    std::u16string ret = voicePtr->GetSendString(nullptr);
    ASSERT_TRUE(ret.empty());
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_007
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_007, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    voicePtr->SetIsControlSwitchOn(true);
    std::shared_ptr<VoiceAssistantSwitchObserver> observer = std::make_shared<VoiceAssistantSwitchObserver>();
    observer->OnChange();
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_009
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_009, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    voicePtr->nowCallId = 0;
    voicePtr->nowAccountId = 0;
    voicePtr->nowVoipCallState = static_cast<int32_t>(CallStateToApp::CALL_STATE_OFFHOOK);
    voicePtr->CallStatusIncoming(1, 1);
    voicePtr->nowVoipCallState = static_cast<int32_t>(CallStateToApp::CALL_STATE_ANSWERED);
    voicePtr->CallStatusIncoming(1, 1);
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_010
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_010, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    voicePtr->accountIds[1] = std::make_shared<IncomingContactInformation>();
    voicePtr->nowCallId = 0;
    voicePtr->nowAccountId = 0;
    voicePtr->CallStatusDialing(1, 1);
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_011
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_011, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    voicePtr->accountIds[1] = info;
    voicePtr->nowCallId = 0;
    voicePtr->nowAccountId = 0;
    voicePtr->CallStatusDisconnected(1, 1);

    info->call_status = static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING);
    voicePtr->CallStatusDisconnected(1, 1);
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_012
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_012, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    auto info = std::make_shared<IncomingContactInformation>();
    info->call_status = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    voicePtr->accountIds[1] = info;
    voicePtr->nowCallId = 0;
    voicePtr->nowAccountId = 0;
    int32_t ret = voicePtr->CheckTelCallState(TelCallState::CALL_STATUS_WAITING);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallVoiceAssistantManager_014
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallVoiceAssistantManager_014, TestSize.Level0)
{
    std::shared_ptr<CallVoiceAssistantManager> voicePtr = CallVoiceAssistantManager::GetInstance();
    voicePtr->UpdateReplyData("a\"\"b");
    ASSERT_TRUE(voicePtr->GetInstance() != nullptr);
}

/**
 * @tc.number   Telephony_SpamCallAdapter_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_SpamCallAdapter_001, TestSize.Level0)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter = std::make_shared<SpamCallAdapter>();
    bool isBlock = false;
    NumberMarkInfo info;
    int32_t blockReason = 0;
    std::string detectDetails;
    spamCallAdapter->GetParseResult(isBlock, info, blockReason, detectDetails);
    EXPECT_EQ(blockReason, 0);
}

/**
 * @tc.number   Telephony_SpamCallAdapter_002
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_SpamCallAdapter_002, TestSize.Level0)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter = std::make_shared<SpamCallAdapter>();
    std::string phone = spamCallAdapter->GetDetectPhoneNum();
    ASSERT_TRUE(phone.empty());
}

/**
 * @tc.number   Telephony_SpamCallAdapter_003
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_SpamCallAdapter_003, TestSize.Level0)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter = std::make_shared<SpamCallAdapter>();
    spamCallAdapter->timeWaitHelper_ = nullptr;
    ASSERT_FALSE(spamCallAdapter->WaitForDetectResult());
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerServiceStub_001, TestSize.Level1)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    int32_t ret = callManagerService->OnRemoteRequest(
        static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_BLUETOOTH_REGISTER_CALLBACKPTR), data, reply, option);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_002
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerServiceStub_002, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    int32_t ret = callManagerService->OnRegisterCallBack(data, reply);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_003
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerServiceStub_003, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    int32_t ret = callManagerService->OnUnRegisterCallBack(data, reply);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_004
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerServiceStub_004, TestSize.Level1)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    int32_t ret = callManagerService->OnObserverOnCallDetailsChange(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_005
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerServiceStub_005, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    int32_t ret = callManagerService->OnMakeCall(data, reply);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_006
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerServiceStub_006, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    int32_t ret = callManagerService->OnSetPreviewWindow(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_007
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerServiceStub_007, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    int32_t ret = callManagerService->OnSetDisplayWindow(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_008
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerServiceStub_008, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    int32_t temp = 1;
    data.WriteRawData((const void *)&temp, sizeof(int32_t));
    int32_t ret = callManagerService->OnReportOttCallDetailsInfo(data, reply);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_009
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerServiceStub_009, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    callManagerService->CancelTimer(1);
    ASSERT_TRUE(callManagerService != nullptr);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_010
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerServiceStub_010, TestSize.Level1)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    int32_t temp = 1;
    data.WriteRawData((const void *)&temp, sizeof(int32_t));
    int32_t ret = callManagerService->OnRegisterBluetoothCallManagerCallbackPtr(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_001, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    callManagerService->OnAddSystemAbility(3009, "");
    callManagerService->OnAddSystemAbility(4802, "");
    ASSERT_TRUE(callManagerService != nullptr);
}

/**
 * @tc.number   Telephony_CallManagerService_002
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_002, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    callManagerService->UnInit();
    ASSERT_TRUE(callManagerService != nullptr);
}

/**
 * @tc.number   Telephony_CallManagerService_005
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_005, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    int32_t ret = callManagerService->PostDialProceed(1, false);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_006
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_006, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    CallTransferInfo info;
    int32_t ret = callManagerService->SetCallTransferInfo(0, info);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_007
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_007, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    std::u16string msg;
    int32_t ret = callManagerService->StartRtt(0, msg);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_008
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_008, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    int32_t ret = callManagerService->StopRtt(0);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_009
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_009, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    int32_t ret = callManagerService->KickOutFromConference(0);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_010
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_010, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    int32_t ret = callManagerService->SetMuted(false);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_011
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_011, TestSize.Level1)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    int32_t ret = callManagerService->MuteRinger();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_012
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_012, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = nullptr;
    int32_t ret = callManagerService->CloseUnFinishedUssd(0);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_013
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_013, TestSize.Level1)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->supportSpecialCode_[0] = "123";
    int32_t ret = callManagerService->InputDialerSpecialCode("123");
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_014
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_014, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = nullptr;
    int32_t ret = callManagerService->SetVoIPCallState(0);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_015
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_015, TestSize.Level1)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    int32_t ret = callManagerService->SetVoIPCallState(0);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_016
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_016, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    int32_t callId = 0;
    int32_t state = 0;
    std::string phoneNumber;
    int32_t ret = callManagerService->GetVoIPCallInfo(callId, state, phoneNumber);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_017
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_017, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    sptr<OHOS::IRemoteObject> failRemoteObj = new MockRemoteObject(-1);
    callManagerService->proxyObjectPtrMap_[1] = failRemoteObj;
    sptr<IRemoteObject> ret = callManagerService->GetProxyObjectPtr(static_cast<CallManagerProxyType>(1));
    EXPECT_NE(ret, nullptr);
}

/**
 * @tc.number   Telephony_CallManagerService_018
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_018, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    DialParaInfo dialParaInfo;
    dialParaInfo.callId = 1;
    sptr<CallBase> callObjectPtr = new CSCall(dialParaInfo);
    CallObjectManager::AddOneCallObject(callObjectPtr);
    int32_t ret = callManagerService->dealCeliaCallEvent(1);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_019
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_019, TestSize.Level1)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    CallObjectManager::callObjectPtrList_.clear();
    DialParaInfo info;
    sptr<CallBase> call = new IMSCall(info);
    call->SetCallId(0);
    CallObjectManager::AddOneCallObject(call);
    std::string eventName = "DISPLAY_SPECIFIED_CALL_PAGE_BY_CALL_ID";
    callManagerService->SendCallUiEvent(1, eventName);
    eventName = "EVENT_CELIA_AUTO_ANSWER_CALL_ON";
    callManagerService->SendCallUiEvent(1, eventName);
    eventName = "EVENT_CELIA_AUTO_ANSWER_CALL_OFF";
    callManagerService->SendCallUiEvent(1, eventName);

    CallObjectManager::callObjectPtrList_.clear();
    call->SetCallId(1);
    CallObjectManager::AddOneCallObject(call);
    eventName = "DISPLAY_SPECIFIED_CALL_PAGE_BY_CALL_ID";
    callManagerService->SendCallUiEvent(1, eventName);
    eventName = "EVENT_CELIA_AUTO_ANSWER_CALL_ON";
    callManagerService->SendCallUiEvent(1, eventName);
    ASSERT_TRUE(call->IsAiAutoAnswer());
    eventName = "EVENT_CELIA_AUTO_ANSWER_CALL_OFF";
    callManagerService->SendCallUiEvent(1, eventName);
    ASSERT_FALSE(call->IsAiAutoAnswer());
    eventName = "EVENT_BLUETOOTH_SCO_STATE_OFF";
    callManagerService->SendCallUiEvent(1, eventName);
    eventName = "EVENT_BLUETOOTH_SCO_STATE_ON";
    callManagerService->SendCallUiEvent(1, eventName);
    eventName = "EVENT_NOT_SUPPORT_BLUETOOTH_CALL";
    callManagerService->SendCallUiEvent(1, eventName);
    eventName = "EVENT_VOIP_CALL_SUCCESS";
    callManagerService->SendCallUiEvent(1, eventName);
    eventName = "EVENT_VOIP_CALL_FAILED";
    callManagerService->SendCallUiEvent(1, eventName);
    call->SetCallType(CallType::TYPE_VOIP);
    callManagerService->HandleVoIPCallEvent(1, eventName);
    callManagerService->HandleVoIPCallEvent(2, eventName);
    eventName = "123";
    int32_t ret = callManagerService->SendCallUiEvent(1, eventName);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_021
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_021, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = nullptr;
    int32_t ret = callManagerService->RemoveMissedIncomingCallNotification();
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_022
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_022, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    int32_t ret = callManagerService->UnHoldCall(0);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_023
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_023, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    bool result;
    int32_t ret = callManagerService->CanSetCallTransferTime(0, result);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_024
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_024, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    int32_t mode = 0;
    int32_t ret = callManagerService->SetCallPreferenceMode(0, mode);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_025
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_025, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    int32_t ret = callManagerService->SeparateConference(0);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_026
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch0Test, Telephony_CallManagerService_026, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    callManagerService->callControlManagerPtr_ = std::make_shared<CallControlManager>();
    AudioDevice audioDevice;
    int32_t ret = callManagerService->SetAudioDevice(audioDevice);
    EXPECT_NE(ret, 0);
}
} // namespace Telephony
} // namespace OHOS
