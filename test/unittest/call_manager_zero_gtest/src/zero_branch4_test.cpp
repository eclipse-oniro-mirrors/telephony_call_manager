/*
 * Copyright (C) 2022-2024 Huawei Device Co., Ltd.
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
#define private public
#define protected public
#include "bluetooth_call_client.h"
#include "bluetooth_call_manager.h"
#include "bluetooth_call_service.h"
#include "bluetooth_connection.h"
#include "iremote_broker.h"
#include "call_ability_callback.h"
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
#include "native_call_manager_hisysevent.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
const int32_t INVALID_SLOTID = 2;
const int32_t SIM1_SLOTID = 0;
const int32_t DEFAULT_INDEX = 1;
const int16_t CAMERA_ROTATION_0 = 0;
const int16_t CAMERA_ROTATION_90 = 90;
const int16_t CAMERA_ROTATION_180 = 180;
const int16_t CAMERA_ROTATION_270 = 270;
const int32_t INVALID_MODE = 0;
const int32_t VALID_CALLID = 1;
const int32_t ERROR_CALLID = -1;
const int32_t ONE_TIME = 1;
const int32_t STEP_1 = 1;
const int32_t SOURCE_CALL = 2;
constexpr int16_t DEFAULT_TIME = 0;
constexpr const char *TEST_STR = "123";
constexpr const char *LONG_STR =
    "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
    "111111111";
} // namespace

class DemoHandler : public AppExecFwk::EventHandler {
public:
    explicit DemoHandler(std::shared_ptr<AppExecFwk::EventRunner> &eventRunner) : AppExecFwk::EventHandler(eventRunner)
    {}
    virtual ~DemoHandler() {}
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) {}
};

class ZeroBranch3Test : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void ZeroBranch3Test::SetUp() {}

void ZeroBranch3Test::TearDown() {}

void ZeroBranch3Test::SetUpTestCase()
{
    constexpr int permissionNum = 1;
    const char *perms[permissionNum] = {
        "ohos.permission.GET_TELEPHONY_STATE"
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,  // Indicates the capsbility list of the sa.
        .permsNum = permissionNum,
        .aclsNum = 0,  // acls is the list of rights thar can be escalated.
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "ZeroBranch3Test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void ZeroBranch3Test::TearDownTestCase() {}

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(std::string uri)
{
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        printf("Get system ability mgr failed.");
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (remoteObj == nullptr) {
        printf("GetSystemAbility Service Failed.");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remoteObj, uri);
}

/**
 * @tc.number   Telephony_VideoCallState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_VideoCallState_001, Function | MediumTest | Level1)
{
    AudioOnlyState audioOnlyState = AudioOnlyState(nullptr);
    ASSERT_NE(audioOnlyState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(audioOnlyState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        audioOnlyState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    CallMediaModeInfo imsCallModeRequestInfo;
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_NE(audioOnlyState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_SEND_ONLY;
    ASSERT_EQ(audioOnlyState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_RECEIVE_ONLY;
    ASSERT_EQ(audioOnlyState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_SEND_RECEIVE;
    ASSERT_NE(audioOnlyState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_VIDEO_PAUSED;
    ASSERT_NE(audioOnlyState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(audioOnlyState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    CallMediaModeInfo imsCallModeResponseInfo;
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_EQ(audioOnlyState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_SEND_ONLY;
    ASSERT_EQ(audioOnlyState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_RECEIVE_ONLY;
    ASSERT_NE(audioOnlyState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_SEND_RECEIVE;
    ASSERT_NE(audioOnlyState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_VIDEO_PAUSED;
    ASSERT_EQ(audioOnlyState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_VideoCallState_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_VideoCallState_002, Function | MediumTest | Level1)
{
    VideoSendState videoSendState = VideoSendState(nullptr);
    ASSERT_NE(videoSendState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoSendState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoSendState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoSendState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoSendState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    CallMediaModeInfo imsCallModeRequestInfo;
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_NE(videoSendState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_SEND_ONLY;
    ASSERT_EQ(videoSendState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_RECEIVE_ONLY;
    ASSERT_NE(videoSendState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_SEND_RECEIVE;
    ASSERT_EQ(videoSendState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_VIDEO_PAUSED;
    ASSERT_NE(videoSendState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoSendState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoSendState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoSendState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoSendState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoSendState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    CallMediaModeInfo imsCallModeResponseInfo;
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_NE(videoSendState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_SEND_ONLY;
    ASSERT_EQ(videoSendState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_RECEIVE_ONLY;
    ASSERT_EQ(videoSendState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_SEND_RECEIVE;
    ASSERT_NE(videoSendState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_VIDEO_PAUSED;
    ASSERT_NE(videoSendState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_VideoCallState_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_VideoCallState_003, Function | MediumTest | Level1)
{
    VideoReceiveState videoReceiveState = VideoReceiveState(nullptr);
    ASSERT_NE(
        videoReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    CallMediaModeInfo imsCallModeRequestInfo;
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_NE(videoReceiveState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_SEND_ONLY;
    ASSERT_EQ(videoReceiveState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_RECEIVE_ONLY;
    ASSERT_EQ(videoReceiveState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_SEND_RECEIVE;
    ASSERT_NE(videoReceiveState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_VIDEO_PAUSED;
    ASSERT_NE(videoReceiveState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    CallMediaModeInfo imsCallModeResponseInfo;
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_NE(videoReceiveState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_SEND_ONLY;
    ASSERT_EQ(videoReceiveState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_RECEIVE_ONLY;
    ASSERT_EQ(videoReceiveState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_SEND_RECEIVE;
    ASSERT_NE(videoReceiveState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeResponseInfo.callMode = ImsCallMode::CALL_MODE_VIDEO_PAUSED;
    ASSERT_NE(videoReceiveState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_VideoCallState_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_VideoCallState_004, Function | MediumTest | Level1)
{
    VideoSendReceiveState videoSendReceiveState = VideoSendReceiveState(nullptr);
    ASSERT_NE(videoSendReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoSendReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoSendReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoSendReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoSendReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED),
        TELEPHONY_ERR_SUCCESS);
    CallMediaModeInfo imsCallModeRequestInfo;
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_NE(videoSendReceiveState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_SEND_ONLY;
    ASSERT_EQ(videoSendReceiveState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_RECEIVE_ONLY;
    ASSERT_EQ(videoSendReceiveState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_SEND_RECEIVE;
    ASSERT_EQ(videoSendReceiveState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_VIDEO_PAUSED;
    ASSERT_EQ(videoSendReceiveState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoSendReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY),
        TELEPHONY_ERR_SUCCESS);
    CallMediaModeInfo imsCallModeResponseInfo;
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_RECEIVE_ONLY;
    ASSERT_NE(
        videoSendReceiveState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_VideoCallState_005
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_VideoCallState_005, Function | MediumTest | Level1)
{
    VideoPauseState videoPauseState = VideoPauseState(nullptr);
    ASSERT_NE(
        videoPauseState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoPauseState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoPauseState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoPauseState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoPauseState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    CallMediaModeInfo imsCallModeRequestInfo;
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_NE(videoPauseState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_SEND_ONLY;
    ASSERT_EQ(videoPauseState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_RECEIVE_ONLY;
    ASSERT_EQ(videoPauseState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_SEND_RECEIVE;
    ASSERT_NE(videoPauseState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    imsCallModeRequestInfo.callMode = ImsCallMode::CALL_MODE_VIDEO_PAUSED;
    ASSERT_NE(videoPauseState.RecieveUpdateCallMediaModeRequest(imsCallModeRequestInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoPauseState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    CallMediaModeInfo imsCallModeResponseInfo;
    ASSERT_EQ(videoPauseState.ReceiveUpdateCallMediaModeResponse(imsCallModeResponseInfo), TELEPHONY_ERR_SUCCESS);
    // Test VideoCallState class cases
    videoPauseState.IsCallSupportVideoCall();
    videoPauseState.SetVideoUpdateStatus(VideoUpdateStatus::STATUS_NONE);
    videoPauseState.GetVideoUpdateStatus();
    videoPauseState.SwitchCallVideoState(ImsCallMode::CALL_MODE_AUDIO_ONLY);
    videoPauseState.DispatchUpdateVideoRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY);
    videoPauseState.DispatchUpdateVideoResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY);
    CallMediaModeInfo imsCallModeInfo;
    videoPauseState.DispatchReportVideoCallInfo(imsCallModeInfo);
}

/**
 * @tc.number   Telephony_IncomingCallNotification_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_IncomingCallNotification_001, TestSize.Level0)
{
    std::shared_ptr<IncomingCallNotification> incomingCallNotification = std::make_shared<IncomingCallNotification>();
    sptr<CallBase> callObjectPtr = nullptr;
    incomingCallNotification->NewCallCreated(callObjectPtr);
    incomingCallNotification->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
    incomingCallNotification->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    incomingCallNotification->IncomingCallActivated(callObjectPtr);
    incomingCallNotification->IncomingCallHungUp(callObjectPtr, false, "");
    DialParaInfo info;
    info.number = TEST_STR;
    callObjectPtr = new CSCall(info);
    incomingCallNotification->NewCallCreated(callObjectPtr);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    incomingCallNotification->NewCallCreated(callObjectPtr);
    incomingCallNotification->NewCallCreated(callObjectPtr);
    incomingCallNotification->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
    incomingCallNotification->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    incomingCallNotification->IncomingCallActivated(callObjectPtr);
    incomingCallNotification->IncomingCallHungUp(callObjectPtr, false, "");

    DialParaInfo emptyNumberInfo;
    sptr<CallBase> emptyNumberInfocall = new CSCall(emptyNumberInfo);
    incomingCallNotification->CallStateUpdated(
        emptyNumberInfocall, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    incomingCallNotification->IncomingCallActivated(emptyNumberInfocall);
    incomingCallNotification->IncomingCallHungUp(callObjectPtr, true, "");

    DisconnectedDetails details;
    incomingCallNotification->CallDestroyed(details);
    incomingCallNotification->PublishIncomingCallNotification(emptyNumberInfocall);
    ASSERT_EQ(incomingCallNotification->CancelIncomingCallNotification(), TELEPHONY_SUCCESS);
    ASSERT_FALSE(incomingCallNotification->IsFullScreen());
}

/**
 * @tc.number   Telephony_RejectCallSms_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_RejectCallSms_001, TestSize.Level0)
{
    std::shared_ptr<RejectCallSms> rejectCallSms = std::make_shared<RejectCallSms>();
    sptr<CallBase> callObjectPtr = nullptr;
    rejectCallSms->IncomingCallHungUp(callObjectPtr, true, "");
    rejectCallSms->IncomingCallHungUp(callObjectPtr, false, "");
    DialParaInfo info;
    info.number = TEST_STR;
    callObjectPtr = new CSCall(info);
    rejectCallSms->IncomingCallHungUp(callObjectPtr, true, "");
    rejectCallSms->IncomingCallHungUp(callObjectPtr, false, "");
    rejectCallSms->IncomingCallHungUp(callObjectPtr, false, TEST_STR);

    std::u16string desAddr = u"123";
    std::u16string text = u"";
    rejectCallSms->SendMessage(INVALID_SLOTID, desAddr, text);
    rejectCallSms->NewCallCreated(callObjectPtr);

    DisconnectedDetails details;
    rejectCallSms->CallDestroyed(details);
    rejectCallSms->IncomingCallActivated(callObjectPtr);

    rejectCallSms->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);

    bool convertRet = rejectCallSms->ConvertToUtf16(TEST_STR) == desAddr;
    ASSERT_TRUE(convertRet);
}

/**
 * @tc.number   Telephony_MissedCallNotification_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_MissedCallNotification_001, TestSize.Level0)
{
    std::shared_ptr<MissedCallNotification> missedCallNotification = std::make_shared<MissedCallNotification>();
    sptr<CallBase> callObjectPtr = nullptr;

    missedCallNotification->NewCallCreated(callObjectPtr);
    missedCallNotification->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);

    missedCallNotification->IncomingCallActivated(callObjectPtr);
    missedCallNotification->IncomingCallHungUp(callObjectPtr, true, "");

    DisconnectedDetails details;
    missedCallNotification->CallDestroyed(details);

    DialParaInfo info;
    info.number = TEST_STR;
    callObjectPtr = new CSCall(info);
    missedCallNotification->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    missedCallNotification->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_DISCONNECTED, TelCallState::CALL_STATUS_INCOMING);

    missedCallNotification->PublishMissedCallEvent(callObjectPtr);
    std::map<std::string, int32_t> phoneNumAndUnReadCountMap;
    phoneNumAndUnReadCountMap.insert(pair<string, int>("000", 1));
    ASSERT_EQ(missedCallNotification->NotifyUnReadMissedCall(phoneNumAndUnReadCountMap), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallSettingManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_CallSettingManager_001, TestSize.Level0)
{
    std::shared_ptr<CallSettingManager> callSettingManager = std::make_shared<CallSettingManager>();
    ASSERT_NE(callSettingManager->SetCallWaiting(SIM1_SLOTID, true), TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->SetCallWaiting(INVALID_SLOTID, true), TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->GetCallWaiting(SIM1_SLOTID), TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->GetCallWaiting(INVALID_SLOTID), TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->GetCallRestriction(SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->GetCallRestriction(INVALID_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS),
        TELEPHONY_SUCCESS);

    CallRestrictionInfo callRestrictionInfo = {
        .password = "",
        .fac = CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING,
        .mode = CallRestrictionMode::RESTRICTION_MODE_DEACTIVATION,
    };
    callSettingManager->SetCallRestriction(SIM1_SLOTID, callRestrictionInfo);
    callSettingManager->SetCallRestriction(INVALID_SLOTID, callRestrictionInfo);

    CallTransferInfo callTransferInfo = {
        .transferNum = "",
        .settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE,
        .type = CallTransferType::TRANSFER_TYPE_BUSY,
        .startHour = 1,
        .startMinute = 1,
        .endHour = 1,
        .endMinute = 1,
    };
    ASSERT_NE(callSettingManager->SetCallTransferInfo(SIM1_SLOTID, callTransferInfo), TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->SetCallTransferInfo(INVALID_SLOTID, callTransferInfo), TELEPHONY_SUCCESS);
    bool result = false;
    ASSERT_NE(callSettingManager->CanSetCallTransferTime(SIM1_SLOTID, result), TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->SetCallPreferenceMode(SIM1_SLOTID, INVALID_MODE), TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->SetCallPreferenceMode(SIM1_SLOTID, CS_VOICE_ONLY), TELEPHONY_SUCCESS);
    ImsConfigItem item = ITEM_VIDEO_QUALITY;
    ASSERT_NE(callSettingManager->GetImsConfig(SIM1_SLOTID, item), TELEPHONY_SUCCESS);
    std::u16string value = u"";
    ASSERT_NE(callSettingManager->SetImsConfig(SIM1_SLOTID, item, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->GetImsFeatureValue(SIM1_SLOTID, FeatureType::TYPE_VOICE_OVER_LTE), TELEPHONY_SUCCESS);
    int32_t featureValue = 0;
    ASSERT_NE(callSettingManager->SetImsFeatureValue(SIM1_SLOTID, FeatureType::TYPE_VOICE_OVER_LTE, featureValue),
        TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->EnableImsSwitch(SIM1_SLOTID), TELEPHONY_SUCCESS);
    ASSERT_NE(callSettingManager->DisableImsSwitch(SIM1_SLOTID), TELEPHONY_SUCCESS);
    bool enabled = false;
    ASSERT_NE(callSettingManager->IsImsSwitchEnabled(SIM1_SLOTID, enabled), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallAbilityCallback_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_CallAbilityCallback_001, TestSize.Level0)
{
    std::shared_ptr<CallAbilityCallback> callAbilityCallback = std::make_shared<CallAbilityCallback>();
    ASSERT_EQ(callAbilityCallback->SetProcessCallback(nullptr), TELEPHONY_SUCCESS);
    CallAttributeInfo callAttributeInfo;
    ASSERT_EQ(callAbilityCallback->OnCallDetailsChange(callAttributeInfo), TELEPHONY_SUCCESS);
    ASSERT_EQ(callAbilityCallback->OnMeeTimeDetailsChange(callAttributeInfo), TELEPHONY_SUCCESS);
    CallEventInfo info;
    ASSERT_EQ(callAbilityCallback->OnCallEventChange(info), TELEPHONY_SUCCESS);
    DisconnectedDetails details;
    ASSERT_EQ(callAbilityCallback->OnCallDisconnectedCause(details), TELEPHONY_SUCCESS);
    AppExecFwk::PacMap resultInfo;
    ASSERT_EQ(
        callAbilityCallback->OnReportAsyncResults(CallResultReportId::GET_CALL_CLIP_ID, resultInfo), TELEPHONY_SUCCESS);
    MmiCodeInfo mmiCodeInfo;
    ASSERT_EQ(callAbilityCallback->OnReportMmiCodeResult(mmiCodeInfo), TELEPHONY_SUCCESS);
    ASSERT_EQ(
        callAbilityCallback->OnOttCallRequest(OttCallRequestId::OTT_REQUEST_ANSWER, resultInfo), TELEPHONY_SUCCESS);
    CallMediaModeInfo imsCallModeInfo;
    ASSERT_EQ(callAbilityCallback->OnReportImsCallModeChange(imsCallModeInfo), TELEPHONY_SUCCESS);
    CallSessionEvent callSessionEventOptions;
    ASSERT_EQ(callAbilityCallback->OnReportCallSessionEventChange(callSessionEventOptions), TELEPHONY_SUCCESS);
    PeerDimensionsDetail peerDimensionsDetail;
    ASSERT_EQ(callAbilityCallback->OnReportPeerDimensionsChange(peerDimensionsDetail), TELEPHONY_SUCCESS);
    int64_t dataUsage = 0;
    ASSERT_EQ(callAbilityCallback->OnReportCallDataUsageChange(dataUsage), TELEPHONY_SUCCESS);
    CameraCapabilities cameraCapabilities;
    ASSERT_EQ(callAbilityCallback->OnReportCameraCapabilities(cameraCapabilities), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallAbilityCallbackStub_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_CallAbilityCallbackStub_001, TestSize.Level0)
{
    std::shared_ptr<CallAbilityCallback> callAbilityCallback = std::make_shared<CallAbilityCallback>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(CallAbilityCallbackStub::GetDescriptor());
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_CALL_STATE_INFO), data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_EVENT),
        data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_CALL_DISCONNECTED_CAUSE), data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_CALL_ASYNC_RESULT_REQUEST), data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::REPORT_OTT_CALL_REQUEST),
        data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_MMI_CODE_RESULT_REQUEST),
        data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_AUDIO_DEVICE_CHANGE_RESULT_REQUEST),
        data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::REPORT_POST_DIAL_DELAY),
        data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_IMS_CALL_MODE_RECEIVE),
        data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::CALL_SESSION_EVENT_CHANGE),
        data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::PEERD_DIMENSIONS_CHANGE),
        data, reply, option);
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::CALL_DATA_USAGE_CHANGE), data, reply, option);
    int32_t res = callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::CAMERA_CAPABILITIES_CHANGE), data, reply, option);
    ASSERT_NE(res, TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_BluetoothCallClient_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_BluetoothCallClient_001, TestSize.Level0)
{
    BluetoothCallClient &bluetoothCallClient = DelayedRefSingleton<BluetoothCallClient>::GetInstance();
    bluetoothCallClient.UnInit();
    ASSERT_NE(bluetoothCallClient.RegisterCallBack(nullptr), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.UnRegisterCallBack(), TELEPHONY_SUCCESS);
    std::u16string value = u"";
    AppExecFwk::PacMap extras;
    bool enabled;
    bluetoothCallClient.IsNewCallAllowed(enabled);
    bluetoothCallClient.IsInEmergencyCall(enabled);
    bluetoothCallClient.SetMuted(false);
    bluetoothCallClient.MuteRinger();
    bluetoothCallClient.SetAudioDevice(AudioDeviceType::DEVICE_BLUETOOTH_SCO, "test");
    bluetoothCallClient.GetCurrentCallList(-1).size();
    ASSERT_NE(bluetoothCallClient.DialCall(value, extras), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.AnswerCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.RejectCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.HangUpCall(), TELEPHONY_SUCCESS);
    ASSERT_GE(bluetoothCallClient.GetCallState(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.HoldCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.UnHoldCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.SwitchCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.CombineConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.SeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.KickOutFromConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.StartDtmf('a'), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.StopDtmf(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.IsRinging(enabled), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerClient_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_CallManagerClient_001, TestSize.Level0)
{
    std::shared_ptr<CallManagerClient> callManagerClient = std::make_shared<CallManagerClient>();
    callManagerClient->UnInit();
    ASSERT_NE(callManagerClient->RegisterCallBack(nullptr), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->UnRegisterCallBack(), TELEPHONY_SUCCESS);
    std::u16string value = u"";
    AppExecFwk::PacMap extras;
    callManagerClient->HoldCall(0);
    callManagerClient->UnHoldCall(0);
    callManagerClient->SwitchCall(0);
    callManagerClient->CombineConference(0);
    callManagerClient->SeparateConference(0);
    callManagerClient->KickOutFromConference(0);
    int32_t intValue;
    callManagerClient->GetMainCallId(intValue, intValue);
    std::vector<std::u16string> callIdList;
    callManagerClient->GetSubCallIdList(0, callIdList);
    callManagerClient->GetCallIdListForConference(0, callIdList);
    callManagerClient->GetCallWaiting(0);
    callManagerClient->SetCallWaiting(0, false);
    callManagerClient->GetCallRestriction(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING);
    CallRestrictionInfo callRestrictionInfo;
    callManagerClient->SetCallRestriction(0, callRestrictionInfo);
    callManagerClient->GetCallTransferInfo(0, CallTransferType::TRANSFER_TYPE_BUSY);
    CallTransferInfo info;
    callManagerClient->SetCallTransferInfo(0, info);
    bool result;
    callManagerClient->CanSetCallTransferTime(0, result);
    callManagerClient->SetCallPreferenceMode(0, 0);
    callManagerClient->StartDtmf(0, 'a');
    callManagerClient->StopDtmf(0);
    callManagerClient->IsRinging(result);
    callManagerClient->HasCall();
    callManagerClient->IsNewCallAllowed(result);
    callManagerClient->IsInEmergencyCall(result);
    callManagerClient->ObserverOnCallDetailsChange();
    ASSERT_NE(callManagerClient->DialCall(value, extras), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->AnswerCall(0, 0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->RejectCall(0, false, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->HangUpCall(0), TELEPHONY_SUCCESS);
    ASSERT_GE(callManagerClient->GetCallState(), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->RemoveMissedIncomingCallNotification(), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerClient_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_CallManagerClient_002, TestSize.Level0)
{
    std::shared_ptr<CallManagerClient> callManagerClient = std::make_shared<CallManagerClient>();
    std::u16string value = u"";
    std::string value_ = "";
    bool enabled;
    int32_t callId = 1;
    ASSERT_NE(callManagerClient->IsEmergencyPhoneNumber(value, 0, enabled), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->FormatPhoneNumber(value, value, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->FormatPhoneNumberToE164(value, value, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->SetMuted(false), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->MuteRinger(), TELEPHONY_SUCCESS);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_EARPIECE,
        .address = { 0 },
    };
    callManagerClient->SetAudioDevice(audioDevice);
    ASSERT_NE(callManagerClient->ControlCamera(callId, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->SetPreviewWindow(callId, value_), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->SetDisplayWindow(callId, value_), TELEPHONY_SUCCESS);
    float zoomRatio = 1;
    callManagerClient->GetImsConfig(0, ImsConfigItem::ITEM_IMS_SWITCH_STATUS);
    callManagerClient->SetImsConfig(0, ImsConfigItem::ITEM_IMS_SWITCH_STATUS, value);
    callManagerClient->GetImsFeatureValue(0, FeatureType::TYPE_SS_OVER_UT);
    callManagerClient->SetImsFeatureValue(0, FeatureType::TYPE_SS_OVER_UT, 0);
    callManagerClient->UpdateImsCallMode(0, ImsCallMode::CALL_MODE_AUDIO_ONLY);
    callManagerClient->EnableImsSwitch(0);
    callManagerClient->DisableImsSwitch(0);
    callManagerClient->IsImsSwitchEnabled(0, enabled);
    callManagerClient->StartRtt(0, value);
    callManagerClient->StopRtt(0);
    std::vector<std::u16string> numberList;
    callManagerClient->JoinConference(0, numberList);
    std::vector<OttCallDetailsInfo> ottVec;
    callManagerClient->ReportOttCallDetailsInfo(ottVec);
    OttCallEventInfo eventInfo;
    callManagerClient->ReportOttCallEventInfo(eventInfo);
    callManagerClient->ReportOttCallEventInfo(eventInfo);
    ASSERT_GT(callManagerClient->SetCameraZoom(zoomRatio), TELEPHONY_ERROR);
    ASSERT_GT(callManagerClient->SetPausePicture(callId, value), TELEPHONY_ERROR);
    ASSERT_GT(callManagerClient->SetDeviceDirection(callId, 0), TELEPHONY_ERROR);
    ASSERT_GT(callManagerClient->CancelCallUpgrade(callId), TELEPHONY_ERROR);
    ASSERT_GT(callManagerClient->RequestCameraCapabilities(callId), TELEPHONY_ERROR);
    int32_t slotId = 0;
    std::string content = "1";
    ASSERT_GT(callManagerClient->SendUssdResponse(slotId, content), TELEPHONY_ERROR);
}

/**
 * @tc.number   Telephony_CallManagerHisysevent_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_CallManagerHisysevent_001, TestSize.Level0)
{
    std::shared_ptr<CallManagerHisysevent> callManagerHisysevent = std::make_shared<CallManagerHisysevent>();
    std::string desc;
    callManagerHisysevent->WriteCallStateBehaviorEvent(0, 0, 0);
    callManagerHisysevent->WriteIncomingCallBehaviorEvent(0, 1, 0);
    callManagerHisysevent->WriteIncomingCallBehaviorEvent(0, 1, 1);
    callManagerHisysevent->WriteIncomingCallBehaviorEvent(0, 0, 0);
    callManagerHisysevent->WriteIncomingCallBehaviorEvent(0, -1, -1);
    callManagerHisysevent->WriteIncomingCallFaultEvent(0, 0, 0, static_cast<int32_t>(TELEPHONY_ERR_MEMCPY_FAIL), desc);
    auto errCode = static_cast<int32_t>(TELEPHONY_ERR_VCARD_FILE_INVALID) + STEP_1;
    callManagerHisysevent->WriteIncomingCallFaultEvent(0, 0, 0, errCode, desc);
    callManagerHisysevent->WriteDialCallFaultEvent(0, 0, 0, static_cast<int32_t>(CALL_ERR_INVALID_SLOT_ID), desc);
    callManagerHisysevent->WriteDialCallFaultEvent(0, 0, 0, errCode, desc);
    callManagerHisysevent->WriteAnswerCallFaultEvent(0, 0, 0, static_cast<int32_t>(CALL_ERR_INVALID_SLOT_ID), desc);
    callManagerHisysevent->WriteAnswerCallFaultEvent(0, 0, 0, errCode, desc);
    callManagerHisysevent->WriteHangUpFaultEvent(0, 0, static_cast<int32_t>(CALL_ERR_INVALID_SLOT_ID), desc);
    callManagerHisysevent->WriteHangUpFaultEvent(0, 0, errCode, desc);
    CallErrorCode eventValue;
    callManagerHisysevent->CallDataErrorCodeConversion(-1, eventValue);
    callManagerHisysevent->CallDataErrorCodeConversion(static_cast<int32_t>(CALL_ERR_INVALID_SLOT_ID), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_INVALID_SLOT_ID);
    callManagerHisysevent->CallDataErrorCodeConversion(static_cast<int32_t>(CALL_ERR_INVALID_CALLID), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_INVALID_CALLID);
    callManagerHisysevent->CallDataErrorCodeConversion(static_cast<int32_t>(CALL_ERR_PHONE_NUMBER_EMPTY), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_PHONE_NUMBER_EMPTY);
    callManagerHisysevent->CallDataErrorCodeConversion(static_cast<int32_t>(CALL_ERR_NUMBER_OUT_OF_RANGE), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_NUMBER_OUT_OF_RANGE);
    callManagerHisysevent->CallDataErrorCodeConversion(
        static_cast<int32_t>(CALL_ERR_UNSUPPORTED_NETWORK_TYPE), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_UNSUPPORTED_NETWORK_TYPE);
    callManagerHisysevent->CallDataErrorCodeConversion(static_cast<int32_t>(CALL_ERR_INVALID_DIAL_SCENE), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_INVALID_DIAL_SCENE);
    callManagerHisysevent->CallDataErrorCodeConversion(static_cast<int32_t>(CALL_ERR_INVALID_VIDEO_STATE), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_INVALID_VIDEO_STATE);
    callManagerHisysevent->CallDataErrorCodeConversion(static_cast<int32_t>(CALL_ERR_UNKNOW_DIAL_TYPE), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_UNKNOW_DIAL_TYPE);
    callManagerHisysevent->CallDataErrorCodeConversion(static_cast<int32_t>(CALL_ERR_UNKNOW_CALL_TYPE), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_UNKNOW_CALL_TYPE);
    callManagerHisysevent->CallDataErrorCodeConversion(static_cast<int32_t>(CALL_ERR_CALL_OBJECT_IS_NULL), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_CALL_OBJECT_IS_NULL);
    callManagerHisysevent->CallInterfaceErrorCodeConversion(-1, eventValue);
    callManagerHisysevent->CallInterfaceErrorCodeConversion(static_cast<int32_t>(CALL_ERR_DIAL_IS_BUSY), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_DIAL_IS_BUSY);
    callManagerHisysevent->CallInterfaceErrorCodeConversion(
        static_cast<int32_t>(CALL_ERR_ILLEGAL_CALL_OPERATION), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_ILLEGAL_CALL_OPERATION);
    callManagerHisysevent->CallInterfaceErrorCodeConversion(
        static_cast<int32_t>(CALL_ERR_PHONE_CALLSTATE_NOTIFY_FAILED), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_PHONE_CALLSTATE_NOTIFY_FAILED);
}

/**
 * @tc.number   Telephony_CallManagerHisysevent_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_CallManagerHisysevent_002, TestSize.Level0)
{
    std::shared_ptr<CallManagerHisysevent> callManagerHisysevent = std::make_shared<CallManagerHisysevent>();
    std::string errordesc;
    callManagerHisysevent->GetErrorDescription(static_cast<int32_t>(CALL_ERR_PHONE_NUMBER_EMPTY), errordesc);
    callManagerHisysevent->GetErrorDescription(static_cast<int32_t>(CALL_ERR_UNKNOW_DIAL_TYPE), errordesc);
    callManagerHisysevent->GetErrorDescription(static_cast<int32_t>(TELEPHONY_ERR_LOCAL_PTR_NULL), errordesc);
    callManagerHisysevent->GetErrorDescription(static_cast<int32_t>(CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE), errordesc);
    callManagerHisysevent->SetDialStartTime();
    callManagerHisysevent->SetIncomingStartTime();
    callManagerHisysevent->SetAnswerStartTime();
    callManagerHisysevent->JudgingDialTimeOut(0, 0, 0);
    callManagerHisysevent->JudgingIncomingTimeOut(0, 0, 0);
    callManagerHisysevent->JudgingAnswerTimeOut(0, 0, 0);
    CallErrorCode eventValue;
    callManagerHisysevent->CallInterfaceErrorCodeConversion(
        static_cast<int32_t>(CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_SYSTEM_EVENT_HANDLE_FAILURE);
    callManagerHisysevent->CallInterfaceErrorCodeConversion(
        static_cast<int32_t>(CALL_ERR_CALL_COUNTS_EXCEED_LIMIT), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_CALL_COUNTS_EXCEED_LIMIT);
    callManagerHisysevent->CallInterfaceErrorCodeConversion(
        static_cast<int32_t>(CALL_ERR_GET_RADIO_STATE_FAILED), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_GET_RADIO_STATE_FAILED);
    callManagerHisysevent->TelephonyErrorCodeConversion(-1, eventValue);
    callManagerHisysevent->TelephonyErrorCodeConversion(static_cast<int32_t>(TELEPHONY_ERR_LOCAL_PTR_NULL), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_CALL_LOCAL_PTR_NULL);
    callManagerHisysevent->TelephonyErrorCodeConversion(
        static_cast<int32_t>(TELEPHONY_ERR_ARGUMENT_INVALID), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_ARGUMENT_INVALID);
    callManagerHisysevent->TelephonyErrorCodeConversion(
        static_cast<int32_t>(TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_IPC_CONNECT_STUB_FAIL);
    callManagerHisysevent->TelephonyErrorCodeConversion(
        static_cast<int32_t>(TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_IPC_WRITE_DESCRIPTOR_TOKEN_FAIL);
    callManagerHisysevent->TelephonyErrorCodeConversion(
        static_cast<int32_t>(TELEPHONY_ERR_WRITE_DATA_FAIL), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_IPC_WRITE_DATA_FAIL);
    callManagerHisysevent->TelephonyErrorCodeConversion(static_cast<int32_t>(TELEPHONY_ERR_PERMISSION_ERR), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_PERMISSION_ERR);
    callManagerHisysevent->TelephonyErrorCodeConversion(static_cast<int32_t>(TELEPHONY_ERR_MEMSET_FAIL), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_MEMSET_FAIL);
    callManagerHisysevent->TelephonyErrorCodeConversion(static_cast<int32_t>(TELEPHONY_ERR_MEMCPY_FAIL), eventValue);
    ASSERT_EQ(eventValue, CallErrorCode::CALL_ERROR_MEMCPY_FAIL);
    std::shared_ptr<NativeCallManagerHisysevent> nativeCallManagerHisysevent =
        std::make_shared<NativeCallManagerHisysevent>();
    nativeCallManagerHisysevent->WriteVoipCallEvent("001", "test", 0, 111, "testMsg", 1, 1, 1);
}

/**
 * @tc.number   Telephony_OTTCall_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_OTTCall_001, TestSize.Level0)
{
    DialParaInfo info;
    info.number = TEST_STR;
    std::shared_ptr<OTTCall> ottCall = std::make_shared<OTTCall>(info);
    CallAttributeInfo callAttributeInfo;
    ottCall->GetCallAttributeInfo(callAttributeInfo);
    ASSERT_EQ(ottCall->DialingProcess(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->AnswerCall(static_cast<int32_t>(VideoStateType::TYPE_VOICE)), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->RejectCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->HangUpCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->HoldCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->UnHoldCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->SwitchCall(), TELEPHONY_SUCCESS);
    ASSERT_FALSE(ottCall->GetEmergencyState());
    ASSERT_NE(ottCall->StartDtmf('a'), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->StopDtmf(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->GetSlotId(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->CombineConference(), TELEPHONY_SUCCESS);
    ASSERT_EQ(ottCall->CanCombineConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->SeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->KickOutFromConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->CanSeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->CanKickOutFromConference(), TELEPHONY_SUCCESS);
    ASSERT_EQ(ottCall->LaunchConference(), TELEPHONY_SUCCESS);
    ASSERT_EQ(ottCall->ExitConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->HoldConference(), TELEPHONY_SUCCESS);
    int32_t mainCallId = 1;
    ASSERT_EQ(ottCall->GetMainCallId(mainCallId), TELEPHONY_SUCCESS);
    std::vector<std::u16string> subCallIdList;
    ASSERT_NE(ottCall->GetSubCallIdList(subCallIdList), TELEPHONY_SUCCESS);
    std::vector<std::u16string> callIdList;
    ASSERT_NE(ottCall->GetCallIdListForConference(callIdList), TELEPHONY_SUCCESS);
    ASSERT_EQ(ottCall->IsSupportConferenceable(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->SetMute(0, SIM1_SLOTID), TELEPHONY_SUCCESS);
    OttCallRequestInfo requestInfo = {
        .phoneNum = "",
        .bundleName = "",
        .videoState = VideoStateType::TYPE_VOICE,
    };
    ASSERT_EQ(ottCall->PackOttCallRequestInfo(requestInfo), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->PostDialProceed(true), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_OTTCall_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch3Test, Telephony_OTTCall_002, TestSize.Level0)
{
    DialParaInfo info;
    info.number = TEST_STR;
    std::shared_ptr<OTTCall> ottCall = std::make_shared<OTTCall>(info);
    ottCall->HandleCombineConferenceFailEvent();
    ASSERT_NE(ottCall->InitVideoCall(), TELEPHONY_SUCCESS);
    ImsCallMode mode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_NE(ottCall->UpdateImsCallMode(mode), TELEPHONY_SUCCESS);
    CallMediaModeInfo callMediaModeInfo;
    ASSERT_NE(ottCall->ReportImsCallModeInfo(callMediaModeInfo), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->SendUpdateCallMediaModeRequest(mode), TELEPHONY_SUCCESS);
    CallModeReportInfo callModeReportInfo;
    ASSERT_NE(ottCall->RecieveUpdateCallMediaModeRequest(callModeReportInfo), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->SendUpdateCallMediaModeResponse(mode), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->ReceiveUpdateCallMediaModeResponse(callModeReportInfo), TELEPHONY_SUCCESS);
    std::string value = "123";
    ASSERT_NE(ottCall->ControlCamera(value, 1, 1), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->SetPausePicture(value), TELEPHONY_SUCCESS);
    uint64_t tempSurfaceId = std::stoull(value);
    auto surface = SurfaceUtils::GetInstance()->GetSurface(tempSurfaceId);
    if (surface == nullptr) {
        value = "";
    }
    ASSERT_NE(ottCall->SetPreviewWindow(value, surface), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->SetDisplayWindow(value, surface), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->SetDeviceDirection(1), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->CancelCallUpgrade(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->RequestCameraCapabilities(), TELEPHONY_SUCCESS);
}
} // namespace Telephony
} // namespace OHOS
