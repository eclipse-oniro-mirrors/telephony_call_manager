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

class SpecialBranch2Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SpecialBranch2Test::SetUpTestCase()
{
    const char *perms[1] = {
        "ohos.permission.WRITE_CALL_LOG",
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "SpecialBranch2Test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void SpecialBranch2Test::TearDownTestCase()
{
}

void SpecialBranch2Test::SetUp()
{
}

void SpecialBranch2Test::TearDown()
{
    sleep(1);
}

/**
 * @tc.number   Telephony_CSCall_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_CSCall_001, TestSize.Level0)
{
    DialParaInfo info;
    auto csCall = std::make_shared<CSCall>(info);
    ASSERT_TRUE(csCall != nullptr);
    std::vector<std::u16string> callIdList;
    csCall->GetSubCallIdList(callIdList);
    int32_t ret = csCall->GetCallIdListForConference(callIdList);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallStateListener_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_CallStateListener_001, TestSize.Level0)
{
    auto callStateListener = std::make_shared<CallStateListener>();
    CallAttributeInfo info = {};
    TelCallState priorState = TelCallState::CALL_STATUS_UNKNOWN;
    TelCallState nextState = TelCallState::CALL_STATUS_UNKNOWN;
    callStateListener->MeeTimeStateUpdated(info, priorState, nextState);
    ASSERT_TRUE(callStateListener != nullptr);
}

/**
 * @tc.number   Telephony_CallControlManager_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_CallControlManager_001, TestSize.Level0)
{
    auto callControlManager = std::make_shared<CallControlManager>();
    ASSERT_TRUE(callControlManager != nullptr);
    std::u16string cameraId;
    callControlManager->ControlCamera(cameraId, 0, 0);

    VideoWindow window;
    callControlManager->SetPreviewWindow(window);
    int32_t ret = callControlManager->SetDisplayWindow(window);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CellularCallConnection_002
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_CellularCallConnection_002, TestSize.Level0)
{
    auto cellularCallConnection = std::make_shared<CellularCallConnection>();
    ASSERT_TRUE(cellularCallConnection != nullptr);
    std::string cameraId;
    cellularCallConnection->ControlCamera(0, 0, cameraId, 0, 0);

    std::string surfaceId;
    cellularCallConnection->SetPreviewWindow(0, 0, surfaceId, nullptr);
    cellularCallConnection->SetPreviewWindow(0, 0, surfaceId, nullptr);
    cellularCallConnection->SetCameraZoom(0.0f);

    std::string path;
    cellularCallConnection->SetPausePicture(0, 0, path);
    cellularCallConnection->SetDeviceDirection(0, 0, 0);

    bool enabled;
    cellularCallConnection->GetCarrierVtConfig(0, enabled);

    CellularCallInfo callInfo;
    cellularCallConnection->SendUpdateCallMediaModeRequest(callInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY);
    cellularCallConnection->SendUpdateCallMediaModeResponse(callInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY);

    std::string value;
    cellularCallConnection->SetImsConfig(ImsConfigItem::ITEM_VIDEO_QUALITY, value, 0);

    std::vector<std::string> numberList;
    cellularCallConnection->InviteToConference(numberList, 0);
    cellularCallConnection->SetMute(0, 0);
    cellularCallConnection->CancelCallUpgrade(0, 0);
    int ret = cellularCallConnection->RequestCameraCapabilities(0, 0);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CellularCallConnection_003
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_CellularCallConnection_003, TestSize.Level1)
{
    auto cellularCallConnection = std::make_shared<CellularCallConnection>();
    ASSERT_TRUE(cellularCallConnection != nullptr);
    CellularCallInfo callInfo;
    int ret = cellularCallConnection->SetCallRestrictionPassword(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING,
        "abc", "bcd");
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_MyLocationEngine_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_MyLocationEngine_001, TestSize.Level0)
{
    auto myLocationEngine = std::make_shared<MyLocationEngine>();
    myLocationEngine->mylocator = nullptr;
    ASSERT_TRUE(myLocationEngine->GetInstance() != nullptr);
}

/**
 * @tc.number   Telephony_MyLocationEngine_002
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_MyLocationEngine_002, TestSize.Level0)
{
    auto myLocationEngine = std::make_shared<MyLocationEngine>();
    myLocationEngine->mylocator = nullptr;
    myLocationEngine->RegisterLocationChange();
    ASSERT_TRUE(myLocationEngine->locatorCallback_ == nullptr);
}

/**
 * @tc.number   Telephony_MyLocationEngine_003
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_MyLocationEngine_003, TestSize.Level0)
{
    auto myLocationEngine = std::make_shared<MyLocationEngine>();
    myLocationEngine->mylocator = nullptr;
    myLocationEngine->RegisterSwitchCallback();
    ASSERT_TRUE(myLocationEngine->locatorCallback_ == nullptr);
}

/**
 * @tc.number   Telephony_MyLocationEngine_004
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_MyLocationEngine_004, TestSize.Level0)
{
    auto myLocationEngine = std::make_shared<MyLocationEngine>();
    myLocationEngine->mylocator = nullptr;
    myLocationEngine->LocationSwitchChange();
    ASSERT_TRUE(myLocationEngine->locatorCallback_ == nullptr);
}

/**
 * @tc.number   Telephony_MyLocationEngine_005
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_MyLocationEngine_005, TestSize.Level0)
{
    auto oOBESwitchObserver = std::make_shared<OOBESwitchObserver>("");
    oOBESwitchObserver->OnChange();
    ASSERT_TRUE(oOBESwitchObserver != nullptr);
}

/**
 * @tc.number   Telephony_MyLocationEngine_006
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_MyLocationEngine_006, TestSize.Level0)
{
    auto myLocationEngine = std::make_shared<MyLocationEngine>();
    EmergencyCallConnectCallback::isStartEccService = true;
    DialParaInfo info;
    AppExecFwk::PacMap extras;
    sptr<CallBase> call = (std::make_unique<IMSCall>(info, extras)).release();
    CallDetailInfo info2;
    myLocationEngine->StartEccService(call, info2);
    ASSERT_TRUE(myLocationEngine != nullptr);
}

/**
 * @tc.number   Telephony_NetCallBase_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_NetCallBase_001, TestSize.Level0)
{
    DialParaInfo paraInfo;
    sptr<NetCallBase> netCallBase = (std::make_unique<OTTCall>(paraInfo)).release();
    ASSERT_TRUE(netCallBase != nullptr);
    netCallBase->StartConference();
    netCallBase->JoinConference();
    netCallBase->KickOutConference();
    netCallBase->LeaveConference();
    netCallBase->GetNetCallType();
    int32_t ret = netCallBase->ChangeNetCallType();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   Telephony_CallRequestProcess_002
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_CallRequestProcess_002, TestSize.Level0)
{
    std::shared_ptr<CallRequestProcess> callRequestProcess = std::make_shared<CallRequestProcess>();
    ASSERT_TRUE(callRequestProcess != nullptr);
    DelayedSingleton<CallControlManager>::GetInstance()->dialSrcInfo_.isDialing = true;
    int32_t ret = callRequestProcess->DialRequest();
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallRequestProcess_003
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_CallRequestProcess_003, TestSize.Level0)
{
    std::shared_ptr<CallRequestProcess> callRequestProcess = std::make_shared<CallRequestProcess>();
    ASSERT_TRUE(callRequestProcess != nullptr);
    DialParaInfo info;
    info.callState = TelCallState::CALL_STATUS_ALERTING;
    AppExecFwk::PacMap extras;
    sptr<CallBase> call = (std::make_unique<IMSCall>(info, extras)).release();
    CallObjectManager::AddOneCallObject(call);
    callRequestProcess->HasConnectingCall(false);

    DialParaInfo info2;
    info2.callState = TelCallState::CALL_STATUS_DIALING;
    sptr<CallBase> call2 = (std::make_unique<IMSCall>(info2, extras)).release();
    CallObjectManager::AddOneCallObject(call2);
    callRequestProcess->HasConnectingCall(false);

    callRequestProcess->voipCallObjectList_.clear();
    ASSERT_TRUE(callRequestProcess->voipCallObjectList_.empty());
}

/**
 * @tc.number   Telephony_CallRequestProcess_004
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_CallRequestProcess_004, TestSize.Level0)
{
    std::shared_ptr<CallRequestProcess> callRequestProcess = std::make_shared<CallRequestProcess>();
    ASSERT_TRUE(callRequestProcess != nullptr);
    DialParaInfo info;
    info.callState = TelCallState::CALL_STATUS_ALERTING;
    AppExecFwk::PacMap extras;
    sptr<CallBase> call = (std::make_unique<IMSCall>(info, extras)).release();
    CallObjectManager::AddOneCallObject(call);
    callRequestProcess->HasActivedCall(false);

    DialParaInfo info2;
    info2.callState = TelCallState::CALL_STATUS_HOLDING;
    sptr<CallBase> call2 = (std::make_unique<IMSCall>(info2, extras)).release();
    CallObjectManager::AddOneCallObject(call2);
    callRequestProcess->HasActivedCall(false);

    callRequestProcess->voipCallObjectList_.clear();
    ASSERT_TRUE(callRequestProcess->voipCallObjectList_.empty());
}

/**
 * @tc.number   Telephony_CallRequestProcess_005
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_CallRequestProcess_005, TestSize.Level0)
{
    std::shared_ptr<CallRequestProcess> callRequestProcess = std::make_shared<CallRequestProcess>();
    ASSERT_TRUE(callRequestProcess != nullptr);
    DialParaInfo info4;
    info4.callId = 4;
    info4.callType = CallType::TYPE_IMS;
    AppExecFwk::PacMap extras;
    sptr<CallBase> call4 = (std::make_unique<IMSCall>(info4, extras)).release();
    call4->callRunningState_ = CallRunningState::CALL_RUNNING_STATE_CREATE;
    CallObjectManager::AddOneCallObject(call4);
    callRequestProcess->DisconnectOtherCallForVideoCall(0);

    DialParaInfo info;
    info.callId = 1;
    info.callType = CallType::TYPE_IMS;
    sptr<CallBase> call = (std::make_unique<IMSCall>(info, extras)).release();
    CallObjectManager::AddOneCallObject(call);
    call->callRunningState_ = CallRunningState::CALL_RUNNING_STATE_HOLD;
    callRequestProcess->DisconnectOtherCallForVideoCall(2);

    DialParaInfo info2;
    info2.callId = 2;
    info2.callType = CallType::TYPE_IMS;
    sptr<CallBase> call2 = (std::make_unique<IMSCall>(info2, extras)).release();
    call2->callRunningState_ = CallRunningState::CALL_RUNNING_STATE_DIALING;
    CallObjectManager::AddOneCallObject(call2);
    callRequestProcess->DisconnectOtherCallForVideoCall(3);

    DialParaInfo info3;
    info3.callId = 3;
    info3.callType = CallType::TYPE_IMS;
    sptr<CallBase> call3 = (std::make_unique<IMSCall>(info3, extras)).release();
    call3->callRunningState_ = CallRunningState::CALL_RUNNING_STATE_ACTIVE;
    CallObjectManager::AddOneCallObject(call3);
    callRequestProcess->DisconnectOtherCallForVideoCall(4);

    callRequestProcess->DisconnectOtherCallForVideoCall(1);
    callRequestProcess->DisconnectOtherCallForVideoCall(1);

    callRequestProcess->voipCallObjectList_.clear();
    ASSERT_TRUE(callRequestProcess->voipCallObjectList_.empty());
}

/**
 * @tc.number   Telephony_CallRequestProcess_006
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_CallRequestProcess_006, TestSize.Level0)
{
    std::shared_ptr<CallRequestProcess> callRequestProcess = std::make_shared<CallRequestProcess>();
    ASSERT_TRUE(callRequestProcess != nullptr);
    DialParaInfo info;
    callRequestProcess->needWaitHold_ = true;
    callRequestProcess->HandleStartDial(false, info);
    callRequestProcess->HandleStartDial(true, info);
    callRequestProcess->needWaitHold_ = false;
    int32_t ret = callRequestProcess->HandleStartDial(false, info);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_VideoReceiveState_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch2Test, Telephony_VideoReceiveState_001, TestSize.Level0)
{
    DialParaInfo paraInfo;
    sptr<NetCallBase> netCallBase = (std::make_unique<OTTCall>(paraInfo)).release();
    auto videoReceiveState = std::make_shared<VideoReceiveState>(netCallBase);
    ASSERT_TRUE(videoReceiveState != nullptr);
    int32_t ret = videoReceiveState->SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE);
    EXPECT_NE(ret, 0);
}
} // namespace Telephony
} // namespace OHOS
