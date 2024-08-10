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
#include "call_superprivacy_control_manager.h"
#include "bluetooth_hfp_ag.h"
#include "call_manager_connect.h"
#include "call_manager_service.h"
#include "surface_utils.h"
#include "telephony_types.h"
#include "voip_call.h"
#include "telephony_permission.h"

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
static constexpr const char *OHOS_PERMISSION_SET_TELEPHONY_STATE = "ohos.permission.SET_TELEPHONY_STATE";
static constexpr const char *OHOS_PERMISSION_PLACE_CALL = "ohos.permission.PLACE_CALL";
} // namespace

class DemoHandler : public AppExecFwk::EventHandler {
public:
    explicit DemoHandler(std::shared_ptr<AppExecFwk::EventRunner> &eventRunner) : AppExecFwk::EventHandler(eventRunner)
    {}
    virtual ~DemoHandler() {}
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) {}
};

class ZeroBranch8Test : public testing::Test {
public:
    void SetUp();
    void TearDown();
    void InitDialInfo(int32_t accountId, int32_t videoState, int32_t dialScene, int32_t dialType);
    static void SetUpTestCase();
    static void TearDownTestCase();
public:
    AppExecFwk::PacMap dialInfo_;
};

void ZeroBranch8Test::SetUp() {}

void ZeroBranch8Test::TearDown() {}

void ZeroBranch8Test::SetUpTestCase()
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
        .processName = "ZeroBranch8Test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void ZeroBranch8Test::TearDownTestCase() {}

void ZeroBranch8Test::InitDialInfo(int32_t accountId, int32_t videoState, int32_t dialScene, int32_t dialType)
{
    dialInfo_.PutIntValue("accountId", accountId);
    dialInfo_.PutIntValue("videoState", videoState);
    dialInfo_.PutIntValue("dialScene", dialScene);
    dialInfo_.PutIntValue("dialType", dialType);
}

HWTEST_F(ZeroBranch8Test, Telephony_CallPolicy_001, Function | MediumTest | Level1)
{
    AccessToken token;
    CallPolicy mCallPolicy;
    std::u16string testEmptyStr = u"";
    AppExecFwk::PacMap mPacMap;
    mPacMap.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_OTT_TYPE));
    mPacMap.PutIntValue("callType", static_cast<int32_t>(CallType::TYPE_BLUETOOTH));
    mPacMap.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_CARRIER_TYPE));
    EXPECT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_ARGUMENT_INVALID);
    mPacMap.PutIntValue("callType", static_cast<int32_t>(CallType::TYPE_CS));
    mPacMap.PutIntValue("dialScene", 3);
    EXPECT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_ARGUMENT_INVALID);
    mPacMap.PutIntValue("dialScene", static_cast<int32_t>(DialScene::CALL_EMERGENCY));
    mPacMap.PutIntValue("videoState", static_cast<int32_t>(VideoStateType::TYPE_VOICE));
    EXPECT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_ARGUMENT_INVALID);
    mPacMap.PutIntValue("videoState", static_cast<int32_t>(VideoStateType::TYPE_SEND_ONLY));
    EXPECT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_ARGUMENT_INVALID);
}

HWTEST_F(ZeroBranch8Test, Telephony_CallManagerService_001, Function | MediumTest | Level1)
{
    AccessToken token;
    auto callManagerService = new CallManagerService();
    callManagerService->state_ =  CallManagerService::ServiceRunningState::STATE_RUNNING;
    callManagerService->OnStart();
    std::string deviceId("123");
    int32_t systemAbilityId = 100;
    std::u16string test = u"";
    callManagerService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_NE(systemAbilityId, AUDIO_POLICY_SERVICE_ID);
    int32_t slotId = 0;
    EXPECT_EQ(callManagerService->CloseUnFinishedUssd(slotId), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callManagerService->SetVoIPCallState(0), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_TRUE(TelephonyPermission::CheckCallerIsSystemApp());
    EXPECT_TRUE(TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE));
    std::string eventName = "ABC";
    int32_t callId = 0;
    EXPECT_EQ(callManagerService->SendCallUiEvent(callId, eventName), TELEPHONY_SUCCESS);
    std::string number = "123456";
    EXPECT_NE(callManagerService->MakeCall(number), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
}

HWTEST_F(ZeroBranch8Test, Telephony_CallManagerService_002, Function | MediumTest | Level3)
{
    AccessToken token;
    std::u16string test = u"";
    std::string test_ = "";
    bool enabled;
    auto callManagerService = new CallManagerService();
    callManagerService->OnStart();
    EXPECT_NE(callManagerService->callControlManagerPtr_, nullptr);
    EXPECT_NE(callManagerService->DialCall(test, dialInfo_), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->AnswerCall(0, 0), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->RejectCall(0, false, test), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->HangUpCall(0), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->GetCallState(), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->HoldCall(0), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->SwitchCall(0), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_FALSE(callManagerService->HasCall());
    EXPECT_NE(callManagerService->IsNewCallAllowed(enabled), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->IsRinging(enabled), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->IsInEmergencyCall(enabled), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->StartDtmf(0, 'a'), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->StopDtmf(0), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->callControlManagerPtr_, nullptr);
    EXPECT_NE(callManagerService->GetCallWaiting(0), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->SetCallWaiting(0, false), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->GetCallRestriction(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    CallRestrictionInfo callRestrictionInfo;
    EXPECT_NE(callManagerService->SetCallRestriction(0, callRestrictionInfo), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(
        callManagerService->SetCallRestrictionPassword(0, CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS, "11", "22"),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(
        callManagerService->GetCallTransferInfo(0, CallTransferType::TRANSFER_TYPE_UNCONDITIONAL),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    CallTransferInfo callTransferInfo;
    EXPECT_NE(callManagerService->SetCallTransferInfo(0, callTransferInfo), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->CanSetCallTransferTime(0, enabled), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->SetCallPreferenceMode(0, 0), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->StartRtt(0, test), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->StopRtt(0), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->CombineConference(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_NE(callManagerService->SeparateConference(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_NE(callManagerService->KickOutFromConference(0), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->SetMuted(false), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->MuteRinger(), TELEPHONY_ERR_LOCAL_PTR_NULL);
    callManagerService->OnStop();
}

/**
 * @tc.number   Telephony_CallManagerService_003
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch8Test, Telephony_CallManagerService_003, Function | MediumTest | Level3)
{
    AccessToken token;
    float zoomRatio = 1;
    std::u16string test = u"";
    std::string test_ = "";
    int32_t callId = 1;
    int32_t value = 1;
    bool enabled;
    auto callManagerService = new CallManagerService();
    callManagerService->OnStart();
    EXPECT_NE(callManagerService->callControlManagerPtr_, nullptr);
    EXPECT_TRUE(TelephonyPermission::CheckCallerIsSystemApp());
    EXPECT_TRUE(TelephonyPermission::CheckPermission(OHOS_PERMISSION_PLACE_CALL));
    EXPECT_NE(callManagerService->SetCameraZoom(zoomRatio), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->SetPausePicture(callId, test), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->SetDeviceDirection(callId, value), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_NE(callManagerService->CancelCallUpgrade(callId), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_NE(callManagerService->RequestCameraCapabilities(callId), TELEPHONY_ERR_PERMISSION_ERR);
    EXPECT_NE(callManagerService->IsEmergencyPhoneNumber(test, 0, enabled), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->FormatPhoneNumber(test, test, test), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->FormatPhoneNumberToE164(test, test, test), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->GetMainCallId(value, value), TELEPHONY_ERR_PERMISSION_ERR);
    std::vector<std::u16string> callIdList;
    EXPECT_NE(callManagerService->GetSubCallIdList(0, callIdList), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->GetCallIdListForConference(0, callIdList), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_NE(callManagerService->GetImsConfig(0, ImsConfigItem::ITEM_VIDEO_QUALITY), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->SetImsConfig(0, ImsConfigItem::ITEM_VIDEO_QUALITY, test),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->GetImsFeatureValue(0, FeatureType::TYPE_VOICE_OVER_LTE),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->SetImsFeatureValue(0, FeatureType::TYPE_VOICE_OVER_LTE, 0),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->UpdateImsCallMode(0, ImsCallMode::CALL_MODE_AUDIO_ONLY),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->EnableImsSwitch(false), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->DisableImsSwitch(false), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->IsImsSwitchEnabled(0, enabled), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->SetVoNRState(0, value), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->GetVoNRState(0, value), TELEPHONY_ERR_LOCAL_PTR_NULL);
    std::vector<std::u16string> numberList;
    EXPECT_NE(callManagerService->JoinConference(false, numberList), TELEPHONY_ERR_LOCAL_PTR_NULL);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO,
        .address = { 0 },
    };
    EXPECT_NE(callManagerService->SetAudioDevice(audioDevice), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->ControlCamera(1, test), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->SetPreviewWindow(1, test_, nullptr), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callManagerService->SetDisplayWindow(1, test_, nullptr), TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_004
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch8Test, Telephony_CallManagerService_004, Function | MediumTest | Level3)
{
    AccessFalseToken token;
    std::u16string test = u"";
    std::string test_ = "";
    bool enabled;
    auto callManagerService = new CallManagerService();
    EXPECT_FALSE(TelephonyPermission::CheckCallerIsSystemApp());
    EXPECT_EQ(callManagerService->DialCall(test, dialInfo_), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->AnswerCall(0, 0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->RejectCall(0, false, test), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->HangUpCall(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->HoldCall(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->UnHoldCall(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->SwitchCall(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->IsNewCallAllowed(enabled), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->IsRinging(enabled), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->IsInEmergencyCall(enabled), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->StartDtmf(0, 'a'), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->StopDtmf(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->GetCallWaiting(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->SetCallWaiting(0, false), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->GetCallRestriction(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    CallRestrictionInfo callRestrictionInfo;
    EXPECT_EQ(callManagerService->SetCallRestriction(0, callRestrictionInfo), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(
        callManagerService->SetCallRestrictionPassword(0, CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS, "11", "22"),
        TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(
        callManagerService->GetCallTransferInfo(0, CallTransferType::TRANSFER_TYPE_UNCONDITIONAL),
        TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    CallTransferInfo callTransferInfo;
    EXPECT_EQ(callManagerService->SetCallTransferInfo(0, callTransferInfo), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->CanSetCallTransferTime(0, enabled), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->CombineConference(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->SeparateConference(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->KickOutFromConference(0), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->SetMuted(false), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->MuteRinger(), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO,
        .address = { 0 },
    };
    EXPECT_EQ(callManagerService->SetAudioDevice(audioDevice), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->ControlCamera(1, test), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->SetPreviewWindow(1, test_, nullptr), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->SetDisplayWindow(1, test_, nullptr), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
}

/**
 * @tc.number   Telephony_CallManagerService_005
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch8Test, Telephony_CallManagerService_005, Function | MediumTest | Level3)
{
    AccessFalseToken token;
    float zoomRatio = 1;
    std::u16string test = u"";
    int32_t callId = 1;
    int32_t value = 1;
    bool enabled;
    auto callManagerService = new CallManagerService();
    EXPECT_FALSE(TelephonyPermission::CheckCallerIsSystemApp());
    EXPECT_EQ(callManagerService->SetPausePicture(callId, test), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->SetDeviceDirection(callId, value), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->CancelCallUpgrade(callId), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->RequestCameraCapabilities(callId), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->GetMainCallId(value, value), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    std::vector<std::u16string> callIdList;
    EXPECT_EQ(callManagerService->GetSubCallIdList(0, callIdList), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->GetCallIdListForConference(0, callIdList), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->UpdateImsCallMode(0, ImsCallMode::CALL_MODE_AUDIO_ONLY),
        TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->EnableImsSwitch(false), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->DisableImsSwitch(false), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->IsImsSwitchEnabled(0, enabled), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->SetVoNRState(0, value), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    EXPECT_EQ(callManagerService->GetVoNRState(0, value), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    std::vector<std::u16string> numberList;
    EXPECT_EQ(callManagerService->JoinConference(false, numberList), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
    std::string number = "123456";
    EXPECT_EQ(callManagerService->MakeCall(number), TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
}
} // namespace Telephony
} // namespace OHOS
