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
#include "call_setting_manager.h"
#include "voip_call_connection.h"

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

HWTEST_F(ZeroBranch8Test, Telephony_BluetoothCallService_001, Function | MediumTest | Level3)
{
    AccessToken token;
    BluetoothCallService bluetoothCallService;
    EXPECT_NE(bluetoothCallService.HangUpCall(), TELEPHONY_SUCCESS);
    EXPECT_NE(bluetoothCallService.HoldCall(), TELEPHONY_SUCCESS);
    EXPECT_NE(bluetoothCallService.UnHoldCall(), TELEPHONY_SUCCESS);
    EXPECT_NE(bluetoothCallService.SwitchCall(), TELEPHONY_SUCCESS);
    EXPECT_NE(bluetoothCallService.CombineConference(), TELEPHONY_SUCCESS);
    EXPECT_NE(bluetoothCallService.SeparateConference(), TELEPHONY_SUCCESS);
    EXPECT_NE(bluetoothCallService.KickOutFromConference(), TELEPHONY_SUCCESS);
}

HWTEST_F(ZeroBranch8Test, Telephony_BluetoothCallService_002, Function | MediumTest | Level3)
{
    AccessToken token;
    BluetoothCallService bluetoothCallService;
    bluetoothCallService.callControlManagerPtr_ = DelayedSingleton<CallControlManager>::GetInstance();
    EXPECT_NE(bluetoothCallService.SeparateConference(), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(bluetoothCallService.KickOutFromConference(), TELEPHONY_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(ZeroBranch8Test, Telephony_CallSettingManager_001, Function | MediumTest | Level3)
{
    std::shared_ptr<CallSettingManager> callSettingManager = std::make_shared<CallSettingManager>();
    callSettingManager->cellularCallConnectionPtr_ = nullptr;
    EXPECT_EQ(callSettingManager->GetCallRestriction(SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    CallRestrictionInfo callRestrictionInfo = {
        .password = "",
        .fac = CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING,
        .mode = CallRestrictionMode::RESTRICTION_MODE_DEACTIVATION,
    };
    EXPECT_EQ(callSettingManager->SetCallRestriction(SIM1_SLOTID, callRestrictionInfo), TELEPHONY_ERR_LOCAL_PTR_NULL);
    CallRestrictionType fac = CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING;
    const char oldPassword[kMaxNumberLen + 1] = "1111";
    const char newPassword[kMaxNumberLen + 1] = "2222";
    EXPECT_EQ(callSettingManager->SetCallRestrictionPassword(INVALID_SLOTID, fac, oldPassword, newPassword),
        CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callSettingManager->SetCallRestrictionPassword(SIM1_SLOTID, fac, oldPassword, newPassword),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    CallTransferType type = CallTransferType::TRANSFER_TYPE_UNCONDITIONAL;
    EXPECT_EQ(callSettingManager->GetCallTransferInfo(INVALID_SLOTID, type), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callSettingManager->GetCallTransferInfo(SIM1_SLOTID, type), TELEPHONY_ERR_LOCAL_PTR_NULL);
    CallTransferInfo callTransferInfo = {
        .transferNum = "",
        .settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE,
        .type = CallTransferType::TRANSFER_TYPE_BUSY,
        .startHour = 1,
        .startMinute = 1,
        .endHour = 1,
        .endMinute = 1,
    };
    EXPECT_EQ(callSettingManager->SetCallTransferInfo(SIM1_SLOTID, callTransferInfo), TELEPHONY_ERR_LOCAL_PTR_NULL);
    bool result = false;
    EXPECT_EQ(callSettingManager->CanSetCallTransferTime(SIM1_SLOTID, result), TELEPHONY_ERR_LOCAL_PTR_NULL);
    int32_t featureValue = 0;
    EXPECT_EQ(callSettingManager->SetImsFeatureValue(SIM1_SLOTID, FeatureType::TYPE_VOICE_OVER_LTE, featureValue),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callSettingManager->EnableImsSwitch(SIM1_SLOTID), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callSettingManager->DisableImsSwitch(SIM1_SLOTID), TELEPHONY_ERR_LOCAL_PTR_NULL);
    bool enabled = false;
    EXPECT_EQ(callSettingManager->IsImsSwitchEnabled(SIM1_SLOTID, enabled), TELEPHONY_ERR_LOCAL_PTR_NULL);
    int32_t state = 0;
    EXPECT_EQ(callSettingManager->SetVoNRState(SIM1_SLOTID, state), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callSettingManager->GetVoNRState(SIM1_SLOTID, state), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callSettingManager->CloseUnFinishedUssd(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callSettingManager->CloseUnFinishedUssd(SIM1_SLOTID), TELEPHONY_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(ZeroBranch8Test, Telephony_CallSettingManager_002, Function | MediumTest | Level3)
{
    std::shared_ptr<CallSettingManager> callSettingManager = std::make_shared<CallSettingManager>();
    CallRestrictionType fac = CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING;
    const char oldPassword[kMaxNumberLen + 1] = "1111";
    const char newPassword[kMaxNumberLen + 1] = "2222";
    EXPECT_NE(callSettingManager->SetCallRestrictionPassword(SIM1_SLOTID, fac, oldPassword, newPassword),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    int32_t state = 0;
    EXPECT_NE(callSettingManager->SetVoNRState(SIM1_SLOTID, state), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callSettingManager->GetVoNRState(SIM1_SLOTID, state), TELEPHONY_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(ZeroBranch8Test, Telephony_CallRequestHandler_001, Function | MediumTest | Level1)
{
    std::unique_ptr<CallRequestHandler> callRequestHandler = std::make_unique<CallRequestHandler>();
    callRequestHandler->Init();
    EXPECT_NE(callRequestHandler->DialCall(), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callRequestHandler->AnswerCall(1, 1), TELEPHONY_ERR_LOCAL_PTR_NULL);
    std::string content = "";
    EXPECT_NE(callRequestHandler->RejectCall(1, true, content), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callRequestHandler->HangUpCall(1), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callRequestHandler->HoldCall(1), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callRequestHandler->UnHoldCall(1), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callRequestHandler->SwitchCall(1), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callRequestHandler->CombineConference(1), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callRequestHandler->SeparateConference(1), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callRequestHandler->KickOutFromConference(1), TELEPHONY_ERR_LOCAL_PTR_NULL);
    std::u16string test = u"";
    EXPECT_NE(callRequestHandler->StartRtt(1, test), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callRequestHandler->StopRtt(1), TELEPHONY_ERR_LOCAL_PTR_NULL);
    std::vector<std::string> emptyRecords = {};
    EXPECT_NE(callRequestHandler->JoinConference(1, emptyRecords), TELEPHONY_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(ZeroBranch8Test, Telephony_CallManagerClient_001, Function | MediumTest | Level1)
{
    std::u16string test = u"";
    int32_t callId = 1;
    int32_t systemAbilityId = 1;
    std::shared_ptr<CallManagerClient> callManagerClient = std::make_shared<CallManagerClient>();
    callManagerClient->Init(systemAbilityId);
    EXPECT_NE(callManagerClient->ObserverOnCallDetailsChange(), TELEPHONY_ERR_UNINIT);
    std::string number = "12345678";
    EXPECT_NE(callManagerClient->MakeCall(number), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->UnRegisterVoipCallManagerCallback(), TELEPHONY_ERR_UNINIT);
    bool result = false;
    EXPECT_NE(callManagerClient->CanSetCallTransferTime(SIM1_SLOTID, result), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->IsRinging(result), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->IsNewCallAllowed(result), TELEPHONY_ERR_UNINIT);
    float zoomRatio = 1.0;
    EXPECT_NE(callManagerClient->SetCameraZoom(zoomRatio), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->SetPausePicture(callId, test), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->GetImsConfig(SIM1_SLOTID, ImsConfigItem::ITEM_IMS_SWITCH_STATUS),
        TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->SetImsConfig(SIM1_SLOTID, ImsConfigItem::ITEM_IMS_SWITCH_STATUS, test),
        TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->GetImsFeatureValue(SIM1_SLOTID, FeatureType::TYPE_SS_OVER_UT), TELEPHONY_ERR_UNINIT);
    int32_t value = 0;
    EXPECT_NE(callManagerClient->SetImsFeatureValue(SIM1_SLOTID, FeatureType::TYPE_SS_OVER_UT, value),
        TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->UpdateImsCallMode(callId, ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->StartRtt(callId, test), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->StopRtt(callId), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->CancelCallUpgrade(callId), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(callManagerClient->RequestCameraCapabilities(callId), TELEPHONY_ERR_UNINIT);
    std::string eventName = "abc";
    EXPECT_NE(callManagerClient->SendCallUiEvent(callId, eventName), TELEPHONY_ERR_UNINIT);
}

HWTEST_F(ZeroBranch8Test, Telephony_CallStatusCallback_001, Function | MediumTest | Level1)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    CallReportInfo callReportInfo;
    callReportInfo.callType = CallType::TYPE_VOIP;
    EXPECT_EQ(callStatusCallback->UpdateCallReportInfo(callReportInfo), TELEPHONY_SUCCESS);
    CallsReportInfo info;
    callReportInfo.state = TelCallState::CALL_STATUS_INCOMING;
    info.callVec.push_back(callReportInfo);
    EXPECT_EQ(callStatusCallback->UpdateCallsReportInfo(info), TELEPHONY_SUCCESS);
}

HWTEST_F(ZeroBranch8Test, Telephony_VoipCallConnection_001, Function | MediumTest | Level1)
{
    std::shared_ptr<VoipCallConnection> voipCallConnection = std::make_shared<VoipCallConnection>();
    voipCallConnection->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    EXPECT_NE(voipCallConnection->voipCallManagerInterfacePtr_, nullptr);
    EXPECT_EQ(voipCallConnection->GetCallManagerProxy(), TELEPHONY_SUCCESS);
    EXPECT_TRUE(voipCallConnection->connectCallManagerState_);
    voipCallConnection->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    VoipCallEventInfo voipCallEventInfo;
    voipCallEventInfo.voipCallId = "123";
    EXPECT_NE(voipCallConnection->AnswerCall(voipCallEventInfo, static_cast<int32_t>(VideoStateType::TYPE_VOICE)),
        TELEPHONY_ERROR);
    EXPECT_NE(voipCallConnection->RejectCall(voipCallEventInfo), TELEPHONY_ERROR);
    EXPECT_NE(voipCallConnection->HangUpCall(voipCallEventInfo), TELEPHONY_ERROR);
    sptr<ICallStatusCallback> callStatusCallback = nullptr;
    EXPECT_NE(voipCallConnection->RegisterCallManagerCallBack(callStatusCallback), TELEPHONY_ERROR);
    EXPECT_NE(voipCallConnection->UnRegisterCallManagerCallBack(), TELEPHONY_ERROR);
    CallAudioEvent callAudioEvent = CallAudioEvent::AUDIO_EVENT_MUTED;
    std::string voipCallId = "123";
    EXPECT_NE(voipCallConnection->SendCallUiEvent(voipCallId, callAudioEvent), TELEPHONY_ERROR);
}

HWTEST_F(ZeroBranch8Test, Telephony_ReportCallInfoHandler_001, Function | MediumTest | Level1)
{
    ReportCallInfoHandler reportCallInfoHandler;
    reportCallInfoHandler.Init();
    VoipCallEventInfo info;
    EXPECT_NE(reportCallInfoHandler.UpdateVoipEventInfo(info), TELEPHONY_ERR_LOCAL_PTR_NULL);
    DialParaInfo mDialParaInfo;
    sptr<CallBase> csCall = new CSCall(mDialParaInfo);
    csCall->callType_ =  CallType::TYPE_IMS;
    EXPECT_EQ(CallObjectManager::AddOneCallObject(csCall), TELEPHONY_SUCCESS);
    CallModeReportInfo response;
    reportCallInfoHandler.ReceiveImsCallModeRequest(response);
    EXPECT_NE(CallObjectManager::GetOneCallObjectByIndex(response.callIndex), nullptr);
}

HWTEST_F(ZeroBranch8Test, Telephony_CallSuperPrivacyControlManager_001, Function | MediumTest | Level1)
{
    int32_t callId = 0;
    int32_t videoState = 0;
    std::u16string phoneNumber = u"";
    int32_t accountId = 1;
    int32_t dialType = 0;
    int32_t dialScene = 0;
    int32_t callType = 0;
    auto controlManager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    controlManager->RestoreSuperPrivacyMode();
    EXPECT_FALSE(controlManager->GetIsChangeSuperPrivacyMode());
    controlManager->SetIsChangeSuperPrivacyMode(true);
    controlManager->RestoreSuperPrivacyMode();
    EXPECT_FALSE(controlManager->GetIsChangeSuperPrivacyMode());
    controlManager->CloseAnswerSuperPrivacyMode(callId, videoState);
    controlManager->CloseCallSuperPrivacyMode(phoneNumber, accountId, videoState, dialType, dialScene, callType);
}
} // namespace Telephony
} // namespace OHOS
