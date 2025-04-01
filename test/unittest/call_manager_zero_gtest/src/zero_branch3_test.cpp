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
#include "antifraud_service.h"
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
#include "bluetooth_call.h"
#include "datashare_helper.h"

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
constexpr int WAIT_TIME = 3;
} // namespace

class DemoHandler : public AppExecFwk::EventHandler {
public:
    explicit DemoHandler(std::shared_ptr<AppExecFwk::EventRunner> &eventRunner) : AppExecFwk::EventHandler(eventRunner)
    {}
    virtual ~DemoHandler() {}
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) {}
};

class ZeroBranch4Test : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void ZeroBranch4Test::SetUp() {}

void ZeroBranch4Test::TearDown() {}

void ZeroBranch4Test::SetUpTestCase()
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
        .processName = "ZeroBranch4Test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void ZeroBranch4Test::TearDownTestCase()
{
    sleep(WAIT_TIME);
}

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

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int systemAbilityId)
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("GetSystemAbilityManager failed.");
        return nullptr;
    }
    sptr<IRemoteObject> remote = saManager->GetSystemAbility(systemAbilityId);
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSystemAbility Service Failed.");
        return nullptr;
    }
    const std::string SETTINGS_DATASHARE_URI =
        "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
    const std::string SETTINGS_DATASHARE_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
    return DataShare::DataShareHelper::Creator(remote, SETTINGS_DATASHARE_URI, SETTINGS_DATASHARE_EXT_URI);
}

/**
 * @tc.number   Telephony_Ott_Conference_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_Ott_Conference_001, Function | MediumTest | Level3)
{
    OttConference ottConference;
    ottConference.state_ = CONFERENCE_STATE_IDLE;
    ASSERT_EQ(ottConference.JoinToConference(1), CALL_ERR_ILLEGAL_CALL_OPERATION);
    ottConference.state_ = CONFERENCE_STATE_CREATING;
    ASSERT_EQ(ottConference.JoinToConference(1), TELEPHONY_SUCCESS);
    for (uint16_t i = 0; i <= CS_CONFERENCE_MAX_CALLS_CNT + 1; ++i) {
        ottConference.subCallIdSet_.insert(i);
    }
    ASSERT_EQ(ottConference.CanCombineConference(), CALL_ERR_CONFERENCE_CALL_EXCEED_LIMIT);
    ottConference.subCallIdSet_.clear();
    ottConference.subCallIdSet_.insert(1);
    ASSERT_EQ(ottConference.LeaveFromConference(-1), CALL_ERR_CONFERENCE_SEPERATE_FAILED);
    ASSERT_EQ(ottConference.LeaveFromConference(1), TELEPHONY_SUCCESS);
    ottConference.subCallIdSet_.clear();
    ottConference.subCallIdSet_.insert(1);
    ASSERT_EQ(ottConference.HoldConference(-1), CALL_ERR_CONFERENCE_SEPERATE_FAILED);
    ottConference.state_ = CONFERENCE_STATE_HOLDING;
    ASSERT_EQ(ottConference.HoldConference(-1), TELEPHONY_SUCCESS);
    ASSERT_EQ(ottConference.HoldConference(1), TELEPHONY_SUCCESS);
    ASSERT_EQ(ottConference.CanCombineConference(), TELEPHONY_SUCCESS);
    ottConference.subCallIdSet_.insert(1);
    ottConference.state_ = CONFERENCE_STATE_ACTIVE;
    ASSERT_EQ(ottConference.CanSeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_EQ(ottConference.CanKickOutFromConference(), TELEPHONY_SUCCESS);
    ottConference.subCallIdSet_.clear();
    ASSERT_EQ(ottConference.CanSeparateConference(), CALL_ERR_CONFERENCE_NOT_EXISTS);
    ASSERT_EQ(ottConference.CanKickOutFromConference(), CALL_ERR_CONFERENCE_NOT_EXISTS);
}

/**
 * @tc.number   Telephony_Cs_Conference_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_Cs_Conference_001, Function | MediumTest | Level3)
{
    CsConference csConference;
    csConference.state_ = CONFERENCE_STATE_IDLE;
    ASSERT_EQ(csConference.JoinToConference(1), CALL_ERR_ILLEGAL_CALL_OPERATION);
    csConference.state_ = CONFERENCE_STATE_CREATING;
    ASSERT_EQ(csConference.JoinToConference(1), TELEPHONY_SUCCESS);
    for (uint16_t i = 0; i <= CS_CONFERENCE_MAX_CALLS_CNT + 1; ++i) {
        csConference.subCallIdSet_.insert(i);
    }
    ASSERT_EQ(csConference.CanCombineConference(), CALL_ERR_CONFERENCE_CALL_EXCEED_LIMIT);
    csConference.subCallIdSet_.clear();
    csConference.subCallIdSet_.insert(1);
    ASSERT_EQ(csConference.LeaveFromConference(-1), CALL_ERR_CONFERENCE_SEPERATE_FAILED);
    ASSERT_EQ(csConference.LeaveFromConference(1), TELEPHONY_SUCCESS);
    csConference.subCallIdSet_.clear();
    csConference.subCallIdSet_.insert(1);
    ASSERT_EQ(csConference.HoldConference(-1), CALL_ERR_CONFERENCE_SEPERATE_FAILED);
    csConference.state_ = CONFERENCE_STATE_HOLDING;
    ASSERT_EQ(csConference.HoldConference(-1), TELEPHONY_SUCCESS);
    ASSERT_EQ(csConference.HoldConference(1), TELEPHONY_SUCCESS);
    ASSERT_EQ(csConference.CanCombineConference(), TELEPHONY_SUCCESS);
    csConference.subCallIdSet_.insert(1);
    csConference.state_ = CONFERENCE_STATE_ACTIVE;
    ASSERT_EQ(csConference.CanSeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_EQ(csConference.CanKickOutFromConference(), TELEPHONY_SUCCESS);
    csConference.subCallIdSet_.clear();
    ASSERT_EQ(csConference.CanSeparateConference(), CALL_ERR_CONFERENCE_NOT_EXISTS);
    ASSERT_EQ(csConference.CanKickOutFromConference(), CALL_ERR_CONFERENCE_NOT_EXISTS);
}

/**
 * @tc.number   Telephony_Ims_Conference_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_Ims_Conference_001, Function | MediumTest | Level3)
{
    ImsConference imsConference;
    imsConference.state_ = CONFERENCE_STATE_IDLE;
    ASSERT_EQ(imsConference.JoinToConference(1), CALL_ERR_ILLEGAL_CALL_OPERATION);
    imsConference.state_ = CONFERENCE_STATE_CREATING;
    ASSERT_EQ(imsConference.JoinToConference(1), TELEPHONY_SUCCESS);
    for (uint16_t i = 0; i <= CS_CONFERENCE_MAX_CALLS_CNT + 1; ++i) {
        imsConference.subCallIdSet_.insert(i);
    }
    ASSERT_EQ(imsConference.CanCombineConference(), CALL_ERR_CONFERENCE_CALL_EXCEED_LIMIT);
    imsConference.subCallIdSet_.clear();
    imsConference.subCallIdSet_.insert(1);
    ASSERT_EQ(imsConference.LeaveFromConference(-1), CALL_ERR_CONFERENCE_SEPERATE_FAILED);
    ASSERT_EQ(imsConference.LeaveFromConference(1), TELEPHONY_SUCCESS);
    imsConference.subCallIdSet_.clear();
    imsConference.subCallIdSet_.insert(1);
    ASSERT_EQ(imsConference.HoldConference(-1), CALL_ERR_CONFERENCE_SEPERATE_FAILED);
    imsConference.state_ = CONFERENCE_STATE_HOLDING;
    ASSERT_EQ(imsConference.HoldConference(-1), TELEPHONY_SUCCESS);
    ASSERT_EQ(imsConference.HoldConference(1), TELEPHONY_SUCCESS);
    ASSERT_EQ(imsConference.CanCombineConference(), TELEPHONY_SUCCESS);
    imsConference.subCallIdSet_.insert(1);
    imsConference.state_ = CONFERENCE_STATE_ACTIVE;
    ASSERT_EQ(imsConference.CanSeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_EQ(imsConference.CanKickOutFromConference(), TELEPHONY_SUCCESS);
    imsConference.subCallIdSet_.clear();
    ASSERT_EQ(imsConference.CanSeparateConference(), CALL_ERR_CONFERENCE_NOT_EXISTS);
    ASSERT_EQ(imsConference.CanKickOutFromConference(), CALL_ERR_CONFERENCE_NOT_EXISTS);
}

/**
 * @tc.number   Telephony_BluetoothCallService_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_BluetoothCallService_001, Function | MediumTest | Level3)
{
    BluetoothCallService bluetoothCallService;
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.AnswerCall());
    DialParaInfo dialParaInfo;
    sptr<OHOS::Telephony::CallBase> callBase1 = new OTTCall(dialParaInfo);
    bluetoothCallService.callObjectPtrList_.clear();
    callBase1->callState_ = TelCallState::CALL_STATUS_INCOMING;
    callBase1->callId_ = -1;
    bluetoothCallService.callObjectPtrList_.push_back(callBase1);
    bluetoothCallService.AnswerCall();
    bluetoothCallService.callControlManagerPtr_ = nullptr;
    ASSERT_EQ(TELEPHONY_ERR_PERMISSION_ERR, bluetoothCallService.AnswerCall());
    ASSERT_EQ(TELEPHONY_ERR_PERMISSION_ERR, bluetoothCallService.RejectCall());
    ASSERT_EQ(TELEPHONY_ERR_PERMISSION_ERR, bluetoothCallService.HangUpCall());
    ASSERT_EQ(TELEPHONY_ERR_PERMISSION_ERR, bluetoothCallService.HoldCall());
    ASSERT_EQ(TELEPHONY_ERR_PERMISSION_ERR, bluetoothCallService.UnHoldCall());
    ASSERT_EQ(TELEPHONY_ERR_PERMISSION_ERR, bluetoothCallService.SwitchCall());
    bluetoothCallService.StartDtmf('c');
    bluetoothCallService.callObjectPtrList_.clear();
    callBase1->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    callBase1->callId_ = -1;
    bluetoothCallService.callObjectPtrList_.push_back(callBase1);
    ASSERT_NE(TELEPHONY_ERR_LOCAL_PTR_NULL, bluetoothCallService.StartDtmf('c'));
    ASSERT_NE(TELEPHONY_ERR_LOCAL_PTR_NULL, bluetoothCallService.StopDtmf());
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.CombineConference());
    bluetoothCallService.callObjectPtrList_.clear();
    callBase1->callState_ = TelCallState::CALL_STATUS_HOLDING;
    callBase1->callId_ = -1;
    bluetoothCallService.callObjectPtrList_.push_back(callBase1);
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.CombineConference());
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.SeparateConference());
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.KickOutFromConference());
}

/**
 * @tc.number   Telephony_BluetoothCallService_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_BluetoothCallService_002, Function | MediumTest | Level3)
{
    BluetoothCallService bluetoothCallService;
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.AnswerCall());
    DialParaInfo dialParaInfo;
    sptr<OHOS::Telephony::CallBase> callBase1 = new OTTCall(dialParaInfo);
    bluetoothCallService.callObjectPtrList_.clear();
    callBase1->callState_ = TelCallState::CALL_STATUS_INCOMING;
    callBase1->callId_ = -1;
    bluetoothCallService.callObjectPtrList_.push_back(callBase1);
    bluetoothCallService.GetCurrentCallList(-1).size();
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.CombineConference());
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.SeparateConference());
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.KickOutFromConference());
}

/**
 * @tc.number   Telephony_BluetoothCallService_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_BluetoothCallService_003, Function | MediumTest | Level3)
{
    int32_t callId = 10020;
    int32_t numActive = 1;
    int32_t callState = (int32_t)TelCallState::CALL_STATUS_IDLE;
    std::string number = "";
    CallAttributeInfo callAttributeInfo;
    callAttributeInfo.callId = callId;
    CallObjectManager::AddOneVoipCallObject(callAttributeInfo);
    BluetoothCallService bluetoothCallService;
    bluetoothCallService.GetVoipCallState(numActive, callState, number);
    EXPECT_EQ(numActive, 1);
    CallObjectManager::UpdateOneVoipCallObjectByCallId(callId, TelCallState::CALL_STATUS_WAITING);
    bluetoothCallService.GetVoipCallState(numActive, callState, number);
    EXPECT_EQ(callState, (int32_t)TelCallState::CALL_STATUS_INCOMING);
    CallObjectManager::UpdateOneVoipCallObjectByCallId(callId, TelCallState::CALL_STATUS_INCOMING);
    bluetoothCallService.GetVoipCallState(numActive, callState, number);
    EXPECT_EQ(callState, (int32_t)TelCallState::CALL_STATUS_INCOMING);
    callState = (int32_t)TelCallState::CALL_STATUS_IDLE;
    CallObjectManager::UpdateOneVoipCallObjectByCallId(callId, TelCallState::CALL_STATUS_ALERTING);
    bluetoothCallService.GetVoipCallState(numActive, callState, number);
    EXPECT_EQ(callState, (int32_t)TelCallState::CALL_STATUS_ALERTING);
    numActive = 0;
    CallObjectManager::UpdateOneVoipCallObjectByCallId(callId, TelCallState::CALL_STATUS_ACTIVE);
    bluetoothCallService.GetVoipCallState(numActive, callState, number);
    EXPECT_EQ(numActive, 1);
}

/**
 * @tc.number   Telephony_BluetoothCallStub_001
 * @tc.name     test error nullptr branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_BluetoothCallStub_001, Function | MediumTest | Level3)
{
    auto bluetoothCallService = std::make_shared<BluetoothCallService>();
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    bluetoothCallService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerCallAbilityInterfaceCode::UPDATE_CALL_STATE_INFO), data1, reply, option);
    const uint8_t data = 1;
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(&data, 1);
    messageParcel.RewindRead(0);
    int32_t result = bluetoothCallService->OnAnswerCall(messageParcel, reply);
    ASSERT_EQ(result, TELEPHONY_ERR_PERMISSION_ERR);
    result = bluetoothCallService->OnRejectCall(messageParcel, reply);
    ASSERT_EQ(result, TELEPHONY_ERR_PERMISSION_ERR);
    bluetoothCallService->OnHangUpCall(messageParcel, reply);
    result = bluetoothCallService->OnGetBtCallState(messageParcel, reply);
    ASSERT_NE(result, TELEPHONY_ERR_PERMISSION_ERR);
    result = bluetoothCallService->OnHoldCall(messageParcel, reply);
    ASSERT_EQ(result, TELEPHONY_ERR_PERMISSION_ERR);
    result = bluetoothCallService->OnUnHoldCall(messageParcel, reply);
    ASSERT_EQ(result, TELEPHONY_ERR_PERMISSION_ERR);
    result = bluetoothCallService->OnSwitchCall(messageParcel, reply);
    ASSERT_EQ(result, TELEPHONY_ERR_PERMISSION_ERR);
    result = bluetoothCallService->OnCombineConference(messageParcel, reply);
    ASSERT_NE(result, TELEPHONY_ERR_PERMISSION_ERR);
    result = bluetoothCallService->OnSeparateConference(messageParcel, reply);
    ASSERT_NE(result, TELEPHONY_ERR_PERMISSION_ERR);
    result = bluetoothCallService->OnKickOutFromConference(messageParcel, reply);
    ASSERT_NE(result, TELEPHONY_ERR_PERMISSION_ERR);
    result = bluetoothCallService->OnStartDtmf(messageParcel, reply);
    ASSERT_EQ(result, TELEPHONY_SUCCESS);
    result = bluetoothCallService->OnStopDtmf(messageParcel, reply);
    ASSERT_EQ(result, TELEPHONY_SUCCESS);
    result = bluetoothCallService->OnGetCurrentCallList(messageParcel, reply);
    ASSERT_EQ(result, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_BluetoothConnection_001
 * @tc.name     test error nullptr branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_BluetoothConnection_001, Function | MediumTest | Level3)
{
    auto bluetoothConnection = std::make_shared<BluetoothConnection>();
    bluetoothConnection->IsAudioActivated();
#ifdef ABILITY_BLUETOOTH_SUPPORT
    bluetoothConnection->ResetBtConnection();
    bluetoothConnection->RegisterObserver();
    std::string address = "123";
    bluetoothConnection->RemoveBtDevice(address);
    Bluetooth::BluetoothRemoteDevice device;
    int32_t state = static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED);
    int32_t cause = 1;
    bluetoothConnection->OnConnectionStateChanged(device, state, cause);
    state = static_cast<int32_t>(Bluetooth::BTConnectState::DISCONNECTED);
    bluetoothConnection->OnConnectionStateChanged(device, state, cause);
    EXPECT_FALSE(bluetoothConnection->IsAudioActivated());
#endif
}

std::string GetTestNumber()
{
    std::string number =
        "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456";
    number += number;
    return number;
}

/**
 * @tc.number   Telephony_BluetoothCallPolicy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_BluetoothCallPolicy_001, Function | MediumTest | Level3)
{
    BluetoothCallPolicy callPolicy;
    DialParaInfo dialParaInfo;
    sptr<OHOS::Telephony::CallBase> callBase1 = new OTTCall(dialParaInfo);
    callBase1->callId_ = 0;
    callPolicy.callObjectPtrList_.push_back(callBase1);
    callBase1->callState_ = TelCallState::CALL_STATUS_INCOMING;
    int32_t policy = 0;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.AnswerCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_WAITING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.AnswerCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.AnswerCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_INCOMING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.RejectCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_WAITING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.RejectCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.RejectCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.HoldCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.HoldCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_HOLDING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.UnHoldCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.UnHoldCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_INCOMING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.HangUpPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_WAITING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.HangUpPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.HangUpPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_ALERTING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.HangUpPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_DIALING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.HangUpPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_HOLDING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.HangUpPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.HangUpPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_EQ(CALL_ERR_ILLEGAL_CALL_OPERATION, callPolicy.SwitchCallPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.SwitchCallPolicy(policy));
}

/**
 * @tc.number   Telephony_BluetoothCallPolicy_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_BluetoothCallPolicy_002, Function | MediumTest | Level3)
{
    BluetoothCallPolicy callPolicy;
    DialParaInfo dialParaInfo;
    sptr<OHOS::Telephony::CallBase> callBase1 = new OTTCall(dialParaInfo);
    callBase1->callId_ = 0;
    callPolicy.callObjectPtrList_.push_back(callBase1);
    int32_t policy = 0;
    callBase1->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.StartDtmfPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_ALERTING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.StartDtmfPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_DIALING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.StartDtmfPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_HOLDING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.StartDtmfPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.StartDtmfPolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.CombineConferencePolicy(policy));
    callBase1->callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.CombineConferencePolicy(policy));
    callBase1->conferenceState_ = TelConferenceState::TEL_CONFERENCE_ACTIVE;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.SeparateConferencePolicy(policy));
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.KickOutFromConferencePolicy(policy));
    callBase1->conferenceState_ = TelConferenceState::TEL_CONFERENCE_HOLDING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.SeparateConferencePolicy(policy));
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.KickOutFromConferencePolicy(policy));
    callBase1->conferenceState_ = TelConferenceState::TEL_CONFERENCE_IDLE;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.SeparateConferencePolicy(policy));
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.KickOutFromConferencePolicy(policy));
}

/**
 * @tc.number   Telephony_ImsCall_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_ImsCall_002, Function | MediumTest | Level3)
{
    DialParaInfo dialParaInfo;
    IMSCall call { dialParaInfo };
    call.isInitialized_ = true;
    ImsCallMode mode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    call.UpdateImsCallMode(mode);
    call.SendUpdateCallMediaModeRequest(mode);
    CallModeReportInfo callModeRequestInfo;
    call.RecieveUpdateCallMediaModeRequest(callModeRequestInfo);
    call.SendUpdateCallMediaModeResponse(mode);
    CallModeReportInfo callModeResponseInfo;
    call.ReceiveUpdateCallMediaModeResponse(callModeResponseInfo);
    CallMediaModeInfo callMediaModeInfo;
    call.ReportImsCallModeInfo(callMediaModeInfo);
    call.SwitchVideoState(mode);
    call.IsSupportVideoCall();
    call.GetCallVideoState(mode);
    std::string value = "123";
    call.ControlCamera(value, 1, 1);
    call.SetPausePicture(value);
    uint64_t tempSurfaceId = std::stoull(value);
    auto surface = SurfaceUtils::GetInstance()->GetSurface(tempSurfaceId);
    if (surface == nullptr) {
        value = "";
    }
    call.SetPreviewWindow(value, surface);
    call.SetDisplayWindow(value, surface);
    call.SetDeviceDirection(1);
    call.CancelCallUpgrade();
    call.RequestCameraCapabilities();
    call.videoCallState_ = nullptr;
    ASSERT_NE(TELEPHONY_ERR_LOCAL_PTR_NULL, call.UpdateImsCallMode(mode));
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.RecieveUpdateCallMediaModeRequest(callModeRequestInfo));
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.ReceiveUpdateCallMediaModeResponse(callModeResponseInfo));
}

/**
 * @tc.number   Telephony_CSCall_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CSCall_001, Function | MediumTest | Level3)
{
    DialParaInfo dialParaInfo;
    CSCall call { dialParaInfo };
    call.AnswerCall(0);
    call.RejectCall();
    call.HoldCall();
    call.SwitchCall();
    call.SetMute(0, 0);
    call.CombineConference();
    call.SeparateConference();
    call.KickOutFromConference();
    call.CanCombineConference();
    call.CanSeparateConference();
    call.LaunchConference();
    call.HoldConference();
    int32_t mainCallId = 1;
    ASSERT_EQ(TELEPHONY_SUCCESS, call.GetMainCallId(mainCallId));
    call.HandleCombineConferenceFailEvent();
}

/**
 * @tc.number   Telephony_CallRecordsManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallRecordsManager_001, Function | MediumTest | Level3)
{
    CallRecordsManager callRecordsManager;
    callRecordsManager.Init();

    sptr<CallBase> callObjectPtr = nullptr;
    TelCallState priorState = TelCallState::CALL_STATUS_HOLDING;
    TelCallState nextState = TelCallState::CALL_STATUS_HOLDING;
    callRecordsManager.CallStateUpdated(callObjectPtr, priorState, nextState);
    nextState = TelCallState::CALL_STATUS_DISCONNECTED;
    callRecordsManager.CallStateUpdated(callObjectPtr, priorState, nextState);
    DialParaInfo dialParaInfo;
    callObjectPtr = new CSCall(dialParaInfo);
    ASSERT_TRUE(callObjectPtr != nullptr);
    callRecordsManager.CallStateUpdated(callObjectPtr, priorState, nextState);
    CallAttributeInfo info;
    info.callBeginTime = DEFAULT_TIME;
    info.callEndTime = ONE_TIME;
    callRecordsManager.AddOneCallRecord(info);
    info.callBeginTime = ONE_TIME;
    info.callEndTime = DEFAULT_TIME;
    callRecordsManager.AddOneCallRecord(info);
    info.callBeginTime = DEFAULT_TIME;
    info.callEndTime = DEFAULT_TIME;
    callRecordsManager.AddOneCallRecord(info);
    info.callBeginTime = ONE_TIME;
    info.callEndTime = ONE_TIME;
    callRecordsManager.AddOneCallRecord(info);
    info.ringBeginTime = DEFAULT_TIME;
    info.ringEndTime = ONE_TIME;
    callRecordsManager.AddOneCallRecord(info);
    info.ringBeginTime = ONE_TIME;
    info.ringEndTime = DEFAULT_TIME;
    callRecordsManager.AddOneCallRecord(info);
    info.ringBeginTime = DEFAULT_TIME;
    info.ringEndTime = DEFAULT_TIME;
    callRecordsManager.AddOneCallRecord(info);
    info.ringBeginTime = ONE_TIME;
    info.ringEndTime = ONE_TIME;
    callRecordsManager.AddOneCallRecord(info);
    callRecordsManager.RemoveMissedIncomingCallNotification();
    int32_t videoState = static_cast<int32_t>(VideoStateType::TYPE_VIDEO);
    callRecordsManager.GetCallFeatures(videoState);
}

/**
 * @tc.number   Telephony_CallControlManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_001, Function | MediumTest | Level3)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    int32_t videoState = 0;
    ASSERT_NE(callControlManager->AnswerCall(INVALID_CALLID, videoState), TELEPHONY_SUCCESS);
    videoState = 2;
    ASSERT_NE(callControlManager->AnswerCall(VALID_CALLID, videoState), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->RejectCall(VALID_CALLID, false, u""), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->HangUpCall(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->HangUpCall(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_EQ(callControlManager->GetCallState(), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->HoldCall(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->UnHoldCall(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SwitchCall(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_FALSE(callControlManager->HasCall());
    sptr<CallBase> callObjectPtr = nullptr;
    TelCallState priorState = TelCallState::CALL_STATUS_HOLDING;
    TelCallState nextState = TelCallState::CALL_STATUS_HOLDING;
    ASSERT_FALSE(callControlManager->NotifyNewCallCreated(callObjectPtr));
    callObjectPtr = nullptr;
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, callControlManager->AddCallLogAndNotification(callObjectPtr));
    callObjectPtr = nullptr;
    ASSERT_FALSE(callControlManager->NotifyCallStateUpdated(callObjectPtr, priorState, nextState));
    DisconnectedDetails details;
    ASSERT_FALSE(callControlManager->NotifyCallDestroyed(details));
    ASSERT_FALSE(callControlManager->NotifyIncomingCallAnswered(callObjectPtr));
    ASSERT_FALSE(callControlManager->NotifyIncomingCallRejected(callObjectPtr, false, ""));
    ASSERT_NE(callControlManager->PostDialProceed(VALID_CALLID, true), TELEPHONY_SUCCESS);
    CallEventInfo info;
    ASSERT_FALSE(callControlManager->NotifyCallEventUpdated(info));
    DialParaInfo dialParaInfo;
    callObjectPtr = new CSCall(dialParaInfo);
    ASSERT_TRUE(callControlManager->NotifyNewCallCreated(callObjectPtr));
    callControlManager->AddCallLogAndNotification(callObjectPtr);
    ASSERT_FALSE(callControlManager->NotifyCallStateUpdated(callObjectPtr, priorState, nextState));
    ASSERT_FALSE(callControlManager->NotifyIncomingCallAnswered(callObjectPtr));
    ASSERT_FALSE(callControlManager->NotifyIncomingCallRejected(callObjectPtr, false, ""));
    callObjectPtr->SetCallCreateTime(time(nullptr));

    callControlManager->Init();
    ASSERT_NE(callControlManager->RejectCall(VALID_CALLID, false, u""), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->RejectCall(INVALID_CALLID, false, u""), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->HangUpCall(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_TRUE(callControlManager->NotifyNewCallCreated(callObjectPtr));
    callControlManager->AddCallLogAndNotification(callObjectPtr);
    ASSERT_TRUE(callControlManager->NotifyCallStateUpdated(callObjectPtr, priorState, nextState));
    ASSERT_TRUE(callControlManager->NotifyIncomingCallAnswered(callObjectPtr));
    ASSERT_TRUE(callControlManager->NotifyIncomingCallRejected(callObjectPtr, false, ""));
    callControlManager->ConnectCallUiService(true);
}

/**
 * @tc.number   Telephony_CallControlManager_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_002, Function | MediumTest | Level3)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    ASSERT_NE(callControlManager->StartDtmf(INVALID_CALLID, 'a'), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->StopDtmf(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetCallWaiting(INVALID_CALLID), TELEPHONY_SUCCESS);
    int32_t slotId = 1;
    ASSERT_NE(callControlManager->GetCallWaiting(slotId), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetCallWaiting(INVALID_CALLID, true), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetCallWaiting(slotId, true), TELEPHONY_SUCCESS);
    CallRestrictionType callRestrictionType = CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS;
    ASSERT_NE(callControlManager->GetCallRestriction(INVALID_CALLID, callRestrictionType), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetCallRestriction(slotId, callRestrictionType), TELEPHONY_SUCCESS);
    CallRestrictionInfo callRestrictionInfo;
    ASSERT_NE(callControlManager->SetCallRestriction(INVALID_CALLID, callRestrictionInfo), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetCallRestriction(slotId, callRestrictionInfo), TELEPHONY_SUCCESS);
    CallTransferType callTransferType = CallTransferType::TRANSFER_TYPE_BUSY;
    ASSERT_NE(callControlManager->GetCallTransferInfo(INVALID_CALLID, callTransferType), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetCallTransferInfo(slotId, callTransferType), TELEPHONY_SUCCESS);
    CallTransferInfo info;
    ASSERT_NE(callControlManager->SetCallTransferInfo(INVALID_CALLID, info), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetCallTransferInfo(slotId, info), TELEPHONY_SUCCESS);
    bool result = true;
    ASSERT_NE(callControlManager->CanSetCallTransferTime(INVALID_CALLID, result), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->CanSetCallTransferTime(slotId, result), TELEPHONY_SUCCESS);
    int32_t mode = 1;
    ASSERT_NE(callControlManager->SetCallPreferenceMode(INVALID_CALLID, mode), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetCallPreferenceMode(slotId, mode), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->CombineConference(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SeparateConference(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->KickOutFromConference(INVALID_CALLID), TELEPHONY_SUCCESS);
    int32_t mainCallId = VALID_CALLID;
    ASSERT_NE(callControlManager->GetMainCallId(INVALID_CALLID, mainCallId), TELEPHONY_SUCCESS);
    std::vector<std::u16string> callIdList;
    ASSERT_NE(callControlManager->GetSubCallIdList(INVALID_CALLID, callIdList), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetCallIdListForConference(INVALID_CALLID, callIdList), TELEPHONY_SUCCESS);
    ImsConfigItem item = ITEM_VIDEO_QUALITY;
    ASSERT_NE(callControlManager->GetImsConfig(INVALID_CALLID, item), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetImsConfig(slotId, item), TELEPHONY_SUCCESS);
    std::u16string value = u"";
    ASSERT_NE(callControlManager->SetImsConfig(INVALID_CALLID, item, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetImsConfig(slotId, item, value), TELEPHONY_SUCCESS);
    FeatureType featureType = TYPE_VOICE_OVER_LTE;
    ASSERT_NE(callControlManager->GetImsFeatureValue(INVALID_CALLID, featureType), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetImsFeatureValue(slotId, featureType), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetImsFeatureValue(INVALID_CALLID, featureType, mode), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetImsFeatureValue(slotId, featureType, mode), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->DisableImsSwitch(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->DisableImsSwitch(slotId), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallControlManager_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_003, Function | MediumTest | Level3)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    callControlManager->UnInit();
    callControlManager->CallStateObserve();
    callControlManager->Init();
    callControlManager->CallStateObserve();
    ASSERT_NE(callControlManager->GetCallWaiting(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetCallWaiting(SIM1_SLOTID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetCallWaiting(INVALID_CALLID, true), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetCallWaiting(SIM1_SLOTID, true), TELEPHONY_SUCCESS);
    CallRestrictionType callRestrictionType = CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING;
    ASSERT_NE(callControlManager->GetCallRestriction(INVALID_CALLID, callRestrictionType), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetCallRestriction(SIM1_SLOTID, callRestrictionType), TELEPHONY_SUCCESS);
    CallRestrictionInfo callRestrictionInfo;
    ASSERT_NE(callControlManager->SetCallRestriction(INVALID_CALLID, callRestrictionInfo), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetCallRestriction(SIM1_SLOTID, callRestrictionInfo), TELEPHONY_SUCCESS);
    CallTransferType callTransferType = CallTransferType::TRANSFER_TYPE_BUSY;
    ASSERT_NE(callControlManager->GetCallTransferInfo(INVALID_CALLID, callTransferType), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetCallTransferInfo(SIM1_SLOTID, callTransferType), TELEPHONY_SUCCESS);
    CallTransferInfo callTransferInfo;
    ASSERT_NE(callControlManager->SetCallTransferInfo(INVALID_CALLID, callTransferInfo), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetCallTransferInfo(SIM1_SLOTID, callTransferInfo), TELEPHONY_SUCCESS);
    bool result = true;
    ASSERT_NE(callControlManager->CanSetCallTransferTime(INVALID_CALLID, result), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->CanSetCallTransferTime(SIM1_SLOTID, result), TELEPHONY_SUCCESS);
    int32_t mode = 1;
    ASSERT_NE(callControlManager->SetCallPreferenceMode(INVALID_CALLID, mode), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetCallPreferenceMode(SIM1_SLOTID, mode), TELEPHONY_SUCCESS);
    ImsConfigItem item = ITEM_VIDEO_QUALITY;
    ASSERT_NE(callControlManager->GetImsConfig(INVALID_CALLID, item), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetImsConfig(SIM1_SLOTID, item), TELEPHONY_SUCCESS);
    std::u16string value = u"";
    ASSERT_NE(callControlManager->SetImsConfig(INVALID_CALLID, item, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetImsConfig(SIM1_SLOTID, item, value), TELEPHONY_SUCCESS);
    FeatureType featureType = TYPE_VOICE_OVER_LTE;
    ASSERT_NE(callControlManager->GetImsFeatureValue(INVALID_CALLID, featureType), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->GetImsFeatureValue(SIM1_SLOTID, featureType), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetImsFeatureValue(INVALID_CALLID, featureType, mode), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetImsFeatureValue(SIM1_SLOTID, featureType, mode), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->DisableImsSwitch(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->DisableImsSwitch(SIM1_SLOTID), TELEPHONY_SUCCESS);
    bool enaled = false;
    ASSERT_NE(callControlManager->IsImsSwitchEnabled(INVALID_CALLID, enaled), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->IsImsSwitchEnabled(SIM1_SLOTID, enaled), TELEPHONY_SUCCESS);
    std::vector<std::u16string> numberList = { u"123", u"124" };
    ASSERT_NE(callControlManager->JoinConference(INVALID_CALLID, numberList), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->JoinConference(SIM1_SLOTID, numberList), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallControlManager_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_004, Function | MediumTest | Level3)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    std::vector<std::u16string> numberList = { u"123", u"124" };
    ASSERT_NE(callControlManager->JoinConference(INVALID_CALLID, numberList), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->JoinConference(SIM1_SLOTID, numberList), TELEPHONY_SUCCESS);
    ImsCallMode mode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_EQ(callControlManager->UpdateImsCallMode(INVALID_CALLID, mode), TELEPHONY_SUCCESS);
    std::u16string str = u"";
    ASSERT_NE(callControlManager->StartRtt(INVALID_CALLID, str), TELEPHONY_SUCCESS);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO,
        .address = { 0 },
    };
    callControlManager->SetAudioDevice(audioDevice);
    audioDevice.deviceType = AudioDeviceType::DEVICE_SPEAKER;
    callControlManager->SetAudioDevice(audioDevice);
    bool enabled = false;
    callControlManager->IsEmergencyPhoneNumber(str, SIM1_SLOTID, enabled);
    callControlManager->IsEmergencyPhoneNumber(str, INVALID_SLOTID, enabled);
    std::string number = "";
    callControlManager->NumberLegalityCheck(number);
    number = LONG_STR;
    callControlManager->NumberLegalityCheck(number);
    number = "1234567";
    callControlManager->NumberLegalityCheck(number);
    std::shared_ptr<CallBroadcastSubscriber> subscriberPtr = nullptr;
    CallControlManager::SystemAbilityListener listen;
    int32_t systemAbilityId = 1;
    std::string deviceId = "123";
    listen.OnAddSystemAbility(systemAbilityId, deviceId);
    listen.OnRemoveSystemAbility(systemAbilityId, deviceId);
    listen.OnAddSystemAbility(COMMON_EVENT_SERVICE_ID, deviceId);
    listen.OnRemoveSystemAbility(COMMON_EVENT_SERVICE_ID, deviceId);
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SIM_STATE_CHANGED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    subscriberPtr = std::make_shared<CallBroadcastSubscriber>(subscriberInfo);
    listen.OnAddSystemAbility(systemAbilityId, deviceId);
    listen.OnRemoveSystemAbility(systemAbilityId, deviceId);
    listen.OnAddSystemAbility(COMMON_EVENT_SERVICE_ID, deviceId);
    listen.OnRemoveSystemAbility(COMMON_EVENT_SERVICE_ID, deviceId);
    ASSERT_NE(callControlManager->StopRtt(INVALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->SetMuted(false), TELEPHONY_SUCCESS);
    ASSERT_EQ(callControlManager->MuteRinger(), TELEPHONY_SUCCESS);
    bool enaled = false;
    int32_t slotId = 1;
    ASSERT_NE(callControlManager->IsImsSwitchEnabled(INVALID_CALLID, enaled), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->IsImsSwitchEnabled(slotId, enaled), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallControlManager_005
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_005, Function | MediumTest | Level3)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    std::u16string number = u"";
    AppExecFwk::PacMap extras;
    bool isEcc = false;
    callControlManager->CanDial(number, extras, isEcc);
    std::string accountNumber = "1234567";
    callControlManager->PackageDialInformation(extras, accountNumber, isEcc);
    int32_t videoState = 0;
    callControlManager->CurrentIsSuperPrivacyMode(VALID_CALLID, videoState);
    sptr<CallBase> callObjectPtr = nullptr;
    callControlManager->AnswerHandlerForSatelliteOrVideoCall(callObjectPtr, videoState);
    DialParaInfo dialParaInfo;
    callObjectPtr = new CSCall(dialParaInfo);
    callControlManager->AnswerHandlerForSatelliteOrVideoCall(callObjectPtr, videoState);
    TelCallState callState = TelCallState::CALL_STATUS_ACTIVE;
    callControlManager->CarrierAndVoipConflictProcess(VALID_CALLID, callState);
    CallRestrictionType fac = CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS;
    int32_t slotId = 1;
    const char oldPassword[kMaxNumberLen + 1] = "1111";
    const char newPassword[kMaxNumberLen + 1] = "2222";
    callControlManager->SetCallRestrictionPassword(slotId, fac, oldPassword, newPassword);
    callControlManager->SeparateConference(VALID_CALLID);
    callControlManager->KickOutFromConference(VALID_CALLID);
    callControlManager->EnableImsSwitch(slotId);
    int32_t state = 0;
    callControlManager->SetVoNRState(slotId, state);
    callControlManager->GetVoNRState(slotId, state);
    callControlManager->IsEmergencyPhoneNumber(number, slotId, isEcc);
    callControlManager->CloseUnFinishedUssd(slotId);
    sptr<CallBase> callObjectPtr1 = nullptr;
    callControlManager->AddBlockLogAndNotification(callObjectPtr1);
    callControlManager->AddBlockLogAndNotification(callObjectPtr);
    callControlManager->HangUpVoipCall();
    callControlManager->SetVoIPCallState(3);
    callControlManager->SetVoIPCallState(2);
    callControlManager->DisconnectAllCalls();
    ASSERT_EQ(callControlManager->SetVoIPCallState(0), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallControlManager_006
 * @tc.name     test CallControlManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_006, Function | MediumTest | Level1)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    callControlManager->ReportPhoneUEInSuperPrivacy("");
    if (callControlManager->CallRequestHandlerPtr_ == nullptr) {
        callControlManager->CallRequestHandlerPtr_ = std::make_unique<CallRequestHandler>();
        callControlManager->CallRequestHandlerPtr_->callRequestProcessPtr_ = nullptr;
    }
    AppExecFwk::PacMap extras;
    std::u16string longNum = u"11111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
        "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
        "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
    EXPECT_NE(callControlManager->DialCall(longNum, extras), TELEPHONY_SUCCESS);
    std::u16string normalNum = u"11111";
    callControlManager->SetCallTypeExtras(extras);

    extras.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_CARRIER_TYPE));
    callControlManager->SetCallTypeExtras(extras);
    extras.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_VOICE_MAIL_TYPE));
    extras.PutIntValue("videoState", static_cast<int32_t>(VideoStateType::TYPE_VIDEO));
    callControlManager->SetCallTypeExtras(extras);
    EXPECT_NE(callControlManager->DialCall(normalNum, extras), TELEPHONY_SUCCESS);
    extras.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_BLUETOOTH_TYPE));
    EXPECT_NE(callControlManager->DialCall(normalNum, extras), TELEPHONY_SUCCESS);
    callControlManager->CallRequestHandlerPtr_ = nullptr;
    EXPECT_NE(callControlManager->DialCall(normalNum, extras), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallControlManager_007
 * @tc.name     test CallControlManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_007, Function | MediumTest | Level1)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    if (callControlManager->CallRequestHandlerPtr_ == nullptr) {
        callControlManager->CallRequestHandlerPtr_ = std::make_unique<CallRequestHandler>();
        callControlManager->CallRequestHandlerPtr_->callRequestProcessPtr_ = nullptr;
    }
    EXPECT_EQ(callControlManager->CarrierAndVoipConflictProcess(-1, TelCallState::CALL_STATUS_ANSWERED),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callControlManager->GetCallState(), static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN));
    callControlManager->VoIPCallState_ = CallStateToApp::CALL_STATE_IDLE;
    EXPECT_NE(callControlManager->GetCallState(), static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN));
    callControlManager->VoIPCallState_ = CallStateToApp::CALL_STATE_ANSWERED;
    EXPECT_NE(callControlManager->GetCallState(), static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN));
    DialParaInfo info;
    sptr<CallBase> call = new CSCall(info);
    CallObjectManager::AddOneCallObject(call);
    callControlManager->VoIPCallState_ = CallStateToApp::CALL_STATE_UNKNOWN;
    EXPECT_NE(callControlManager->GetCallState(), static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN));
    callControlManager->VoIPCallState_ = CallStateToApp::CALL_STATE_RINGING;
    EXPECT_NE(callControlManager->GetCallState(), static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN));
    callControlManager->VoIPCallState_ = CallStateToApp::CALL_STATE_OFFHOOK;
    EXPECT_NE(callControlManager->GetCallState(), static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN));
    EXPECT_TRUE(callControlManager->HasCall());
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    call->SetCallType(CallType::TYPE_CS);
    call->SetCrsType(0);
    call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    callControlManager->VoIPCallState_ = CallStateToApp::CALL_STATE_IDLE;
    EXPECT_NE(callControlManager->GetCallState(), static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN));
    EXPECT_NE(callControlManager->AnswerCall(-1, static_cast<int32_t>(VideoStateType::TYPE_VIDEO)), TELEPHONY_SUCCESS);
    EXPECT_NE(callControlManager->AnswerCall(1, static_cast<int32_t>(VideoStateType::TYPE_VIDEO)), TELEPHONY_SUCCESS);
    EXPECT_NE(callControlManager->AnswerCall(0, static_cast<int32_t>(VideoStateType::TYPE_VIDEO)), TELEPHONY_SUCCESS);
    int32_t crsType = 2;
    call->SetCrsType(crsType);
    EXPECT_NE(callControlManager->AnswerCall(0, static_cast<int32_t>(VideoStateType::TYPE_VIDEO)), TELEPHONY_SUCCESS);
    call->SetCallType(CallType::TYPE_VOIP);
    EXPECT_NE(callControlManager->AnswerCall(0, static_cast<int32_t>(VideoStateType::TYPE_VOICE)), TELEPHONY_SUCCESS);
    EXPECT_NE(callControlManager->AnswerCall(0, static_cast<int32_t>(VideoStateType::TYPE_SEND_ONLY)),
        TELEPHONY_SUCCESS);
    callControlManager->VoIPCallState_ = CallStateToApp::CALL_STATE_UNKNOWN;
    EXPECT_NE(callControlManager->AnswerCall(0, static_cast<int32_t>(VideoStateType::TYPE_VOICE)), TELEPHONY_SUCCESS);
    callControlManager->CallRequestHandlerPtr_ = nullptr;
    EXPECT_EQ(callControlManager->HandlerAnswerCall(0, static_cast<int32_t>(VideoStateType::TYPE_VOICE)),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    call->SetCallType(CallType::TYPE_CS);
    EXPECT_EQ(callControlManager->AnswerCall(0, static_cast<int32_t>(VideoStateType::TYPE_VOICE)), TELEPHONY_SUCCESS);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_NE(callControlManager->SetMuted(false), CALL_ERR_AUDIO_SETTING_MUTE_FAILED);
}

/**
 * @tc.number   Telephony_CallControlManager_008
 * @tc.name     test CallControlManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_008, Function | MediumTest | Level1)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    if (callControlManager->CallRequestHandlerPtr_ == nullptr) {
        callControlManager->CallRequestHandlerPtr_ = std::make_unique<CallRequestHandler>();
        callControlManager->CallRequestHandlerPtr_->callRequestProcessPtr_ = nullptr;
    }
    DialParaInfo info;
    sptr<CallBase> call = new CSCall(info);
    call->SetCallType(CallType::TYPE_CS);
    CallObjectManager::callObjectPtrList_.clear();
    CallObjectManager::AddOneCallObject(call);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    EXPECT_NE(callControlManager->HoldCall(0), TELEPHONY_SUCCESS);
    EXPECT_NE(callControlManager->HangUpCall(0), TELEPHONY_SUCCESS);
    EXPECT_NE(callControlManager->RejectCall(0, false, u""), TELEPHONY_SUCCESS);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_HOLD);
    EXPECT_NE(callControlManager->UnHoldCall(0), TELEPHONY_SUCCESS);
    callControlManager->CallRequestHandlerPtr_ = nullptr;
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    EXPECT_NE(callControlManager->HoldCall(0), TELEPHONY_SUCCESS);
    EXPECT_NE(callControlManager->HangUpCall(0), TELEPHONY_SUCCESS);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_HOLD);
    EXPECT_NE(callControlManager->UnHoldCall(0), TELEPHONY_SUCCESS);
    EXPECT_NE(callControlManager->StartDtmf(0, '1'), TELEPHONY_ERR_ARGUMENT_INVALID);
    sleep(1);
    EXPECT_NE(callControlManager->StopDtmf(0), TELEPHONY_ERR_ARGUMENT_INVALID);
    call->SetTelCallState(TelCallState::CALL_STATUS_IDLE);
    EXPECT_NE(callControlManager->StartDtmf(0, '1'), TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_NE(callControlManager->StopDtmf(0), TELEPHONY_ERR_ARGUMENT_INVALID);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_NE(callControlManager->SetMuted(false), CALL_ERR_AUDIO_SETTING_MUTE_FAILED);
    sptr<CallBase> voipCall = (std::make_unique<VoIPCall>(info)).release();
    voipCall->SetCallType(CallType::TYPE_VOIP);
    CallObjectManager::AddOneCallObject(voipCall);
    call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    EXPECT_NE(callControlManager->HangUpCall(0), TELEPHONY_SUCCESS);
    EXPECT_NE(callControlManager->RejectCall(0, false, u""), TELEPHONY_SUCCESS);
    call->SetCallId(VOIP_CALL_MINIMUM);
    EXPECT_NE(callControlManager->HangUpCall(VOIP_CALL_MINIMUM), TELEPHONY_SUCCESS);
    EXPECT_NE(callControlManager->RejectCall(VOIP_CALL_MINIMUM, false, u""), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallControlManager_009
 * @tc.name     test CallControlManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_009, Function | MediumTest | Level1)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    if (callControlManager->CallRequestHandlerPtr_ == nullptr) {
        callControlManager->CallRequestHandlerPtr_ = std::make_unique<CallRequestHandler>();
        callControlManager->CallRequestHandlerPtr_->callRequestProcessPtr_ = nullptr;
    }
    DialParaInfo info;
    sptr<CallBase> call = (std::make_unique<IMSCall>(info)).release();
    call->SetVideoStateType(VideoStateType::TYPE_VOICE);
    call->SetCallType(CallType::TYPE_IMS);
    callControlManager->AnswerHandlerForSatelliteOrVideoCall(call, static_cast<int32_t>(VideoStateType::TYPE_VOICE));
    callControlManager->AnswerHandlerForSatelliteOrVideoCall(call, static_cast<int32_t>(VideoStateType::TYPE_VIDEO));
    EXPECT_EQ(call->GetVideoStateType(), VideoStateType::TYPE_VIDEO);
    CallObjectManager::callObjectPtrList_.clear();
    CallObjectManager::AddOneCallObject(call);
    sptr<CallBase> satelliteCall = (std::make_unique<SatelliteCall>(info)).release();
    satelliteCall->SetCallType(CallType::TYPE_SATELLITE);
    satelliteCall->SetCallId(1);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    callControlManager->AnswerHandlerForSatelliteOrVideoCall(satelliteCall,
        static_cast<int32_t>(VideoStateType::TYPE_VIDEO));
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    callControlManager->AnswerHandlerForSatelliteOrVideoCall(satelliteCall,
        static_cast<int32_t>(VideoStateType::TYPE_VIDEO));
    sptr<CallBase> voipCall = (std::make_unique<VoIPCall>(info)).release();
    voipCall->SetCallType(CallType::TYPE_VOIP);
    voipCall->SetCallId(2);
    sptr<CallBase> csCall = (std::make_unique<CSCall>(info)).release();
    csCall->SetCallId(3);
    csCall->SetCallType(CallType::TYPE_CS);
    CallObjectManager::AddOneCallObject(satelliteCall);
    CallObjectManager::AddOneCallObject(voipCall);
    CallObjectManager::AddOneCallObject(csCall);
    voipCall->SetTelCallState(TelCallState::CALL_STATUS_UNKNOWN);
    EXPECT_EQ(callControlManager->CarrierAndVoipConflictProcess(0, TelCallState::CALL_STATUS_ANSWERED),
        TELEPHONY_SUCCESS);
    EXPECT_EQ(callControlManager->CarrierAndVoipConflictProcess(1, TelCallState::CALL_STATUS_ANSWERED),
        TELEPHONY_SUCCESS);
    EXPECT_EQ(callControlManager->CarrierAndVoipConflictProcess(2, TelCallState::CALL_STATUS_ANSWERED),
        TELEPHONY_SUCCESS);
    EXPECT_EQ(callControlManager->CarrierAndVoipConflictProcess(3, TelCallState::CALL_STATUS_ANSWERED),
        TELEPHONY_SUCCESS);
    voipCall->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    EXPECT_GT(callControlManager->CarrierAndVoipConflictProcess(0, TelCallState::CALL_STATUS_ANSWERED),
        TELEPHONY_ERROR);
    voipCall->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
    EXPECT_GT(callControlManager->CarrierAndVoipConflictProcess(0, TelCallState::CALL_STATUS_ANSWERED),
        TELEPHONY_ERROR);
    EXPECT_GT(callControlManager->HangUpVoipCall(), TELEPHONY_ERROR);
}

/**
 * @tc.number   Telephony_CallControlManager_010
 * @tc.name     test CallControlManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_010, Function | MediumTest | Level1)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    callControlManager->Init();
    DialParaInfo info;
    sptr<CallBase> call = (std::make_unique<IMSCall>(info)).release();
    call->SetCallType(CallType::TYPE_IMS);
    TelCallState priorState = TelCallState::CALL_STATUS_DIALING;
    TelCallState nextState = TelCallState::CALL_STATUS_HOLDING;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    nextState = TelCallState::CALL_STATUS_ALERTING;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    nextState = TelCallState::CALL_STATUS_ACTIVE;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    priorState = TelCallState::CALL_STATUS_INCOMING;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    nextState = TelCallState::CALL_STATUS_DISCONNECTED;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    priorState = TelCallState::CALL_STATUS_ACTIVE;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    nextState = TelCallState::CALL_STATUS_INCOMING;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    priorState = TelCallState::CALL_STATUS_DISCONNECTING;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    nextState = TelCallState::CALL_STATUS_DISCONNECTED;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    priorState = TelCallState::CALL_STATUS_WAITING;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    nextState = TelCallState::CALL_STATUS_ACTIVE;
    EXPECT_TRUE(callControlManager->NotifyCallStateUpdated(call, priorState, nextState));
    CallObjectManager::callObjectPtrList_.clear();
    CallObjectManager::AddOneCallObject(call);
    call->SetTelCallState(TelCallState::CALL_STATUS_IDLE);
    call->SetCallId(0);
    EXPECT_EQ(callControlManager->PostDialProceed(0, false), CALL_ERR_CALL_STATE_MISMATCH_OPERATION);
    call->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTED);
    EXPECT_EQ(callControlManager->PostDialProceed(0, false), CALL_ERR_CALL_STATE_MISMATCH_OPERATION);
    call->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTING);
    EXPECT_EQ(callControlManager->PostDialProceed(0, false), CALL_ERR_CALL_STATE_MISMATCH_OPERATION);
    call->SetTelCallState(TelCallState::CALL_STATUS_ANSWERED);
    EXPECT_NE(callControlManager->PostDialProceed(0, false), CALL_ERR_CALL_STATE_MISMATCH_OPERATION);
    call->SetTelCallState(TelCallState::CALL_STATUS_WAITING);
    EXPECT_NE(callControlManager->PostDialProceed(0, false), CALL_ERR_CALL_STATE_MISMATCH_OPERATION);
    call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    EXPECT_NE(callControlManager->PostDialProceed(0, false), CALL_ERR_CALL_STATE_MISMATCH_OPERATION);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_NE(callControlManager->SetMuted(false), CALL_ERR_AUDIO_SETTING_MUTE_FAILED);
    callControlManager->DisconnectAllCalls();
}

/**
 * @tc.number   Telephony_CallControlManager_011
 * @tc.name     test CallControlManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_011, Function | MediumTest | Level1)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    DialParaInfo info;
    sptr<CallBase> call = (std::make_unique<IMSCall>(info)).release();
    call->SetTelCallState(TelCallState::CALL_STATUS_IDLE);
    call->SetCallId(0);
    sptr<CallBase> call2 = (std::make_unique<IMSCall>(info)).release();
    call2->SetTelCallState(TelCallState::CALL_STATUS_HOLDING);
    call2->SetCallId(1);
    CallObjectManager::callObjectPtrList_.clear();
    CallObjectManager::AddOneCallObject(call);
    EXPECT_EQ(callControlManager->CombineConference(0), CALL_ERR_CALL_STATE_MISMATCH_OPERATION);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(callControlManager->CombineConference(0), CALL_ERR_CALL_STATE_MISMATCH_OPERATION);
    CallObjectManager::AddOneCallObject(call2);
    EXPECT_NE(callControlManager->CombineConference(0), CALL_ERR_CALL_STATE_MISMATCH_OPERATION);
    EXPECT_NE(callControlManager->SeparateConference(0), TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_NE(callControlManager->KickOutFromConference(0), TELEPHONY_ERR_ARGUMENT_INVALID);
    int32_t mainCallId = 0;
    EXPECT_NE(callControlManager->GetMainCallId(0, mainCallId), TELEPHONY_ERR_ARGUMENT_INVALID);
    std::vector<std::u16string> callIdList;
    EXPECT_NE(callControlManager->GetSubCallIdList(0, callIdList), TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_NE(callControlManager->GetCallIdListForConference(0, callIdList), TELEPHONY_ERR_ARGUMENT_INVALID);
}

/**
 * @tc.number   Telephony_CallControlManager_012
 * @tc.name     test CallControlManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallControlManager_012, Function | MediumTest | Level1)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    EXPECT_EQ(callControlManager->GetCallWaiting(-1), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->SetCallWaiting(-1, false), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->GetCallRestriction(-1, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        CALL_ERR_INVALID_SLOT_ID);
    CallRestrictionInfo restrictionInfo;
    EXPECT_EQ(callControlManager->SetCallRestriction(-1, restrictionInfo), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->SetCallRestrictionPassword(-1, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING,
        "", ""), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->GetCallTransferInfo(-1, CallTransferType::TRANSFER_TYPE_UNCONDITIONAL),
        CALL_ERR_INVALID_SLOT_ID);
    CallTransferInfo callTransferInfo;
    EXPECT_EQ(callControlManager->SetCallTransferInfo(-1, callTransferInfo), CALL_ERR_INVALID_SLOT_ID);
    bool result = false;
    EXPECT_EQ(callControlManager->CanSetCallTransferTime(-1, result), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->SetCallPreferenceMode(-1, 0), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->GetImsConfig(-1, ImsConfigItem::ITEM_VIDEO_QUALITY), CALL_ERR_INVALID_SLOT_ID);
    std::u16string value = u"";
    EXPECT_EQ(callControlManager->SetImsConfig(-1, ImsConfigItem::ITEM_VIDEO_QUALITY, value),
        CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->GetImsFeatureValue(-1, FeatureType::TYPE_VOICE_OVER_LTE), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->SetImsFeatureValue(-1, FeatureType::TYPE_VOICE_OVER_LTE, 0),
        CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->EnableImsSwitch(-1), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->DisableImsSwitch(-1), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->IsImsSwitchEnabled(-1, result), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->SetVoNRState(-1, 0), CALL_ERR_INVALID_SLOT_ID);
    int32_t state = 0;
    EXPECT_EQ(callControlManager->GetVoNRState(-1, state), CALL_ERR_INVALID_SLOT_ID);
    EXPECT_EQ(callControlManager->CloseUnFinishedUssd(-1), CALL_ERR_INVALID_SLOT_ID);
    CallControlManager::alarmSeted = true;
    callControlManager->ConnectCallUiService(true);
    CallControlManager::alarmSeted = true;
    callControlManager->ConnectCallUiService(false);
    CallControlManager::alarmSeted = true;
    callControlManager->ConnectCallUiService(true);
    EXPECT_NE(callControlManager->RemoveMissedIncomingCallNotification(), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStatusManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_001, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    CallDetailInfo info;
    std::string number = "123";
    memcpy_s(&info.phoneNum, kMaxNumberLen, number.c_str(), number.length());
    info.state = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_HOLDING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_DIALING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_ALERTING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_INCOMING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_WAITING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_DISCONNECTED;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_DISCONNECTING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    DisconnectedDetails details;
    ASSERT_GT(callStatusManager->HandleDisconnectedCause(details), TELEPHONY_ERROR);
    CellularCallEventInfo cellularCallEventInfo;
    cellularCallEventInfo.eventType = static_cast<CellularCallEventType>(1);
    ASSERT_GT(callStatusManager->HandleEventResultReportInfo(cellularCallEventInfo), TELEPHONY_ERROR);
    cellularCallEventInfo.eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;
    cellularCallEventInfo.eventId = RequestResultEventId::RESULT_DIAL_NO_CARRIER;
    ASSERT_GT(callStatusManager->HandleEventResultReportInfo(cellularCallEventInfo), TELEPHONY_ERROR);
    OttCallEventInfo ottCallEventInfo;
    (void)memset_s(&ottCallEventInfo, sizeof(OttCallEventInfo), 0, sizeof(OttCallEventInfo));
    ottCallEventInfo.ottCallEventId = OttCallEventId::OTT_CALL_EVENT_FUNCTION_UNSUPPORTED;
    (void)memcpy_s(ottCallEventInfo.bundleName, kMaxBundleNameLen + 1, LONG_STR, strlen(LONG_STR));
    ASSERT_GT(callStatusManager->HandleOttEventReportInfo(ottCallEventInfo), TELEPHONY_ERROR);
    (void)memset_s(&ottCallEventInfo, sizeof(OttCallEventInfo), 0, sizeof(OttCallEventInfo));
    ottCallEventInfo.ottCallEventId = OttCallEventId::OTT_CALL_EVENT_FUNCTION_UNSUPPORTED;
    (void)memcpy_s(ottCallEventInfo.bundleName, kMaxBundleNameLen + 1, TEST_STR, strlen(TEST_STR));
    ASSERT_GT(callStatusManager->HandleOttEventReportInfo(ottCallEventInfo), TELEPHONY_ERROR);
    info.voipCallInfo.voipCallId = "123456789";
    info.callType = CallType::TYPE_VOIP;
    info.state = TelCallState::CALL_STATUS_INCOMING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_DISCONNECTED;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
}

/**
 * @tc.number   Telephony_CallStatusManager_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_002, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    CallDetailInfo callDetailInfo;
    std::string number = "";
    memcpy_s(&callDetailInfo.phoneNum, kMaxNumberLen, number.c_str(), number.length());
    callDetailInfo.state = TelCallState::CALL_STATUS_INCOMING;
    callDetailInfo.callType = CallType::TYPE_CS;
    ASSERT_GT(callStatusManager->IncomingHandle(callDetailInfo), TELEPHONY_ERROR);
    ASSERT_GT(callStatusManager->IncomingVoipCallHandle(callDetailInfo), TELEPHONY_ERROR);
    callDetailInfo.state = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_GT(callStatusManager->IncomingHandle(callDetailInfo), TELEPHONY_ERROR);
    ASSERT_GT(callStatusManager->IncomingVoipCallHandle(callDetailInfo), TELEPHONY_ERROR);
    ASSERT_GT(callStatusManager->IncomingFilterPolicy(callDetailInfo), TELEPHONY_ERROR);
    sptr<CallBase> callObjectPtr = nullptr;
    TelCallState nextState = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_GT(callStatusManager->UpdateCallState(callObjectPtr, nextState), TELEPHONY_ERROR);
    ASSERT_GT(callStatusManager->ToSpeakerPhone(callObjectPtr), TELEPHONY_ERROR);
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_CS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_INCOMING;
    callObjectPtr = new CSCall(dialParaInfo);
    ASSERT_GT(callStatusManager->UpdateCallState(callObjectPtr, nextState), TELEPHONY_ERROR);
    nextState = TelCallState::CALL_STATUS_INCOMING;
    ASSERT_GT(callStatusManager->UpdateCallState(callObjectPtr, nextState), TELEPHONY_ERROR);
    callStatusManager->RefreshCallIfNecessary(callObjectPtr, callDetailInfo);
    ASSERT_GT(callStatusManager->ToSpeakerPhone(callObjectPtr), TELEPHONY_ERROR);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
    ASSERT_GT(callStatusManager->ToSpeakerPhone(callObjectPtr), TELEPHONY_ERROR);
    ASSERT_GT(callStatusManager->TurnOffMute(callObjectPtr), TELEPHONY_ERROR);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_ALERTING);
    callStatusManager->SetOriginalCallTypeForActiveState(callObjectPtr);
    callStatusManager->SetOriginalCallTypeForDisconnectState(callObjectPtr);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    callStatusManager->SetOriginalCallTypeForActiveState(callObjectPtr);
    int32_t activeCallNum = 0;
    int32_t waitingCallNum = 0;
    int32_t slotId = 0;
    callStatusManager->AutoAnswer(activeCallNum, waitingCallNum);
    bool canSwitchCallState = 1;
    TelCallState priorState = TelCallState::CALL_STATUS_DISCONNECTING;
    callStatusManager->AutoHandleForDsda(canSwitchCallState, priorState, activeCallNum, slotId, true);
    callStatusManager->AutoUnHoldForDsda(canSwitchCallState, priorState, activeCallNum, slotId);
    callStatusManager->AutoAnswerForVideoCall(activeCallNum);
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_003, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    // scene-0: null input
    ASSERT_EQ(callStatusManager->IsFromTheSameNumberAtTheSameTime(nullptr), false);
    
    // init
    const std::string phoneNumber = "12345678911";
    time_t oldCallCreateTime = time(nullptr);
    if (oldCallCreateTime < 0) {
        oldCallCreateTime = 0;
    }
    int32_t incomingMaxDuration = 10 * 1000;
    
    //old call
    DialParaInfo dialParaInfoOld;
    sptr<OHOS::Telephony::CallBase> oldCall = new BluetoothCall(dialParaInfoOld, "");
    oldCall->SetAccountNumber(phoneNumber);
    oldCall->SetCallId(1);
    oldCall->SetCallCreateTime(oldCallCreateTime);
    oldCall->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    oldCall->SetCallType(CallType::TYPE_BLUETOOTH);
    //new call
    DialParaInfo dialParaInfoNew;
    sptr<OHOS::Telephony::CallBase> newCall = new IMSCall(dialParaInfoNew);
    newCall->SetAccountNumber(phoneNumber);
    newCall->SetCallId(2);
    newCall->SetCallCreateTime(oldCallCreateTime + incomingMaxDuration - 1000);
    newCall->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    newCall->SetCallType(CallType::TYPE_IMS);
    
    // scene-1: do not exist old call with the same phone number
    ASSERT_EQ(callStatusManager->IsFromTheSameNumberAtTheSameTime(newCall), false);
    ASSERT_NO_THROW(callStatusManager->ModifyEsimType());
    
    // scene-2: invalid diff-CallID
    callStatusManager->AddOneCallObject(oldCall);
    newCall->SetCallId(9);
    ASSERT_EQ(callStatusManager->IsFromTheSameNumberAtTheSameTime(newCall), false);
    ASSERT_NO_THROW(callStatusManager->ModifyEsimType());
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_004, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    
    // init
    const std::string phoneNumber = "12345678911";
    time_t oldCallCreateTime = time(nullptr);
    if (oldCallCreateTime < 0) {
        oldCallCreateTime = 0;
    }
    int32_t incomingMaxDuration = 10 * 1000;
    
    //old call
    DialParaInfo dialParaInfoOld;
    sptr<OHOS::Telephony::CallBase> oldCall = new BluetoothCall(dialParaInfoOld, "");
    oldCall->SetAccountNumber(phoneNumber);
    oldCall->SetCallId(1);
    oldCall->SetCallCreateTime(oldCallCreateTime);
    oldCall->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    oldCall->SetCallType(CallType::TYPE_BLUETOOTH);
    //new call
    DialParaInfo dialParaInfoNew;
    sptr<OHOS::Telephony::CallBase> newCall = new IMSCall(dialParaInfoNew);
    newCall->SetAccountNumber(phoneNumber);
    newCall->SetCallId(2);
    newCall->SetCallCreateTime(oldCallCreateTime + incomingMaxDuration + 1000);
    newCall->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    newCall->SetCallType(CallType::TYPE_IMS);
    
    // scene-3: invalid diff-CreateTime
    ASSERT_EQ(callStatusManager->IsFromTheSameNumberAtTheSameTime(newCall), false);
    ASSERT_NO_THROW(callStatusManager->ModifyEsimType());

    // scene-4: invalid old call status
    callStatusManager->DeleteOneCallObject(oldCall->GetCallID());
    oldCall->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
    callStatusManager->AddOneCallObject(oldCall);
    newCall->SetCallCreateTime(oldCallCreateTime + incomingMaxDuration - 1000);
    ASSERT_EQ(callStatusManager->IsFromTheSameNumberAtTheSameTime(newCall), false);
    ASSERT_NO_THROW(callStatusManager->ModifyEsimType());
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_005, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    
    // init
    const std::string phoneNumber = "12345678911";
    time_t oldCallCreateTime = time(nullptr);
    if (oldCallCreateTime < 0) {
        oldCallCreateTime = 0;
    }
    int32_t incomingMaxDuration = 10 * 1000;
    
    // old call
    DialParaInfo dialParaInfoOld;
    sptr<OHOS::Telephony::CallBase> oldCall = new BluetoothCall(dialParaInfoOld, "");
    oldCall->SetAccountNumber(phoneNumber);
    oldCall->SetCallId(1);
    oldCall->SetCallCreateTime(oldCallCreateTime);
    oldCall->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    oldCall->SetCallType(CallType::TYPE_BLUETOOTH);
    // new call
    DialParaInfo dialParaInfoNew;
    sptr<OHOS::Telephony::CallBase> newCall = new IMSCall(dialParaInfoNew);
    newCall->SetAccountNumber(phoneNumber);
    newCall->SetCallId(2);
    newCall->SetCallCreateTime(oldCallCreateTime + incomingMaxDuration - 1000);
    newCall->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    newCall->SetCallType(CallType::TYPE_IMS);
    // scene-5: invalid call type
    callStatusManager->AddOneCallObject(oldCall);
    newCall->SetCallType(CallType::TYPE_CS);
    ASSERT_EQ(callStatusManager->IsFromTheSameNumberAtTheSameTime(newCall), true);
    ASSERT_NO_THROW(callStatusManager->ModifyEsimType());
    // scene-6: valid on-number-dual-call scene
    newCall->SetCallType(CallType::TYPE_IMS);
    ASSERT_EQ(callStatusManager->IsFromTheSameNumberAtTheSameTime(newCall), true);
    ASSERT_NO_THROW(callStatusManager->ModifyEsimType());
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_006, Function | MediumTest | Level3)
{
    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper =
            CreateDataShareHelper(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    if (dataShareHelper != nullptr) {
        std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
        ASSERT_NO_THROW(callStatusManager->ModifyEsimType());
        
        OHOS::Uri uri("datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true");
        DataShare::DataShareValueObject keyObj("key_esim_card_type");
        DataShare::DataShareValueObject valueObj("2");
        DataShare::DataShareValuesBucket bucket;
        bucket.Put("KEYWORD", keyObj);
        bucket.Put("VALUE", valueObj);
        dataShareHelper->Insert(uri, bucket);
        dataShareHelper->Release();
        ASSERT_NO_THROW(callStatusManager->ModifyEsimType());
    }
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_007, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    CallDetailInfo info;
    std::string number = "123456789";
    memcpy_s(&info.phoneNum, kMaxNumberLen, number.c_str(), number.length());
    info.voipCallInfo.voipCallId = "123456789";
    info.callType = CallType::TYPE_VOIP;
    info.state = TelCallState::CALL_STATUS_INCOMING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.callType = CallType::TYPE_BLUETOOTH;
    info.index = 1;
    info.state = TelCallState::CALL_STATUS_INCOMING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_WAITING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_DISCONNECTED;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_DIALING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_ALERTING;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    callStatusManager->HandleConnectingCallReportInfo(info);
    info.index = 0;
    ASSERT_GT(callStatusManager->HandleCallReportInfo(info), TELEPHONY_ERROR);
    sptr<CallBase> call = callStatusManager->GetOneCallObjectByVoipCallId(
        info.voipCallInfo.voipCallId, info.voipCallInfo.voipBundleName, info.voipCallInfo.uid);
    ASSERT_TRUE(call != nullptr);
    EXPECT_EQ(callStatusManager->AnsweredVoipCallHandle(info), TELEPHONY_ERROR);
    EXPECT_GT(callStatusManager->DisconnectingVoipCallHandle(info), TELEPHONY_ERROR);
    callStatusManager->DeleteOneCallObject(call);
    EXPECT_EQ(callStatusManager->AnsweredVoipCallHandle(info), TELEPHONY_ERROR);
    EXPECT_EQ(callStatusManager->DisconnectingVoipCallHandle(info), TELEPHONY_ERROR);
    EXPECT_FALSE(callStatusManager->IsContactPhoneNum(number));
    EXPECT_FALSE(callStatusManager->IsDistributeCallSourceStatus());
    EXPECT_GT(callStatusManager->UpdateCallStateAndHandleDsdsMode(info, call), TELEPHONY_ERROR);
    EXPECT_FALSE(callStatusManager->ShouldBlockIncomingCall(call, info));
    call = nullptr;
    EXPECT_EQ(callStatusManager->UpdateCallStateAndHandleDsdsMode(info, call), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callStatusManager->HandleCallReportInfoEx(info), TELEPHONY_ERR_FAIL);
    VoipCallEventInfo voipCallInfo;
    callStatusManager->BtCallDialingHandleFirst(nullptr, info);
    std::vector<sptr<CallBase>> conferenceCallList;
    callStatusManager->SetConferenceCall(conferenceCallList);
    EXPECT_GT(callStatusManager->HandleVoipEventReportInfo(voipCallInfo), TELEPHONY_ERROR);
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_008, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    CallDetailsInfo infos;
    infos.slotId = -1;
    EXPECT_EQ(callStatusManager->HandleCallsReportInfo(infos), CALL_ERR_INVALID_SLOT_ID);
    infos.slotId = 0;
    CallDetailInfo info;
    infos.callVec.push_back(info);
    EXPECT_EQ(callStatusManager->HandleCallsReportInfo(infos), TELEPHONY_SUCCESS);
    callStatusManager->callDetailsInfo_[0].callVec.push_back(info);
    EXPECT_EQ(callStatusManager->HandleCallsReportInfo(infos), TELEPHONY_SUCCESS);
    auto &info0 = infos.callVec[0];
    info0.state = TelCallState::CALL_STATUS_ACTIVE;
    EXPECT_EQ(callStatusManager->HandleCallsReportInfo(infos), TELEPHONY_SUCCESS);
    info0.state = TelCallState::CALL_STATUS_UNKNOWN;
    info0.mpty = 1;
    EXPECT_EQ(callStatusManager->HandleCallsReportInfo(infos), TELEPHONY_SUCCESS);
    info0.mpty = 0;
    info0.callType = CallType::TYPE_CS;
    EXPECT_EQ(callStatusManager->HandleCallsReportInfo(infos), TELEPHONY_SUCCESS);
    info0.callType = CallType::TYPE_ERR_CALL;
    info0.callMode = VideoStateType::TYPE_VIDEO;
    EXPECT_EQ(callStatusManager->HandleCallsReportInfo(infos), TELEPHONY_SUCCESS);
    info0.callMode = VideoStateType::TYPE_VOICE;
    info0.state = TelCallState::CALL_STATUS_ALERTING;
    EXPECT_EQ(callStatusManager->HandleCallsReportInfo(infos), TELEPHONY_SUCCESS);
    auto &info1 = callStatusManager->callDetailsInfo_[0].callVec[0];
    info1.index = 1;
    EXPECT_EQ(callStatusManager->HandleCallsReportInfo(infos), TELEPHONY_SUCCESS);
    info.state = TelCallState::CALL_STATUS_DIALING;
    EXPECT_GT(callStatusManager->HandleVoipCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_ANSWERED;
    EXPECT_EQ(callStatusManager->HandleVoipCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_DISCONNECTING;
    EXPECT_EQ(callStatusManager->HandleVoipCallReportInfo(info), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_UNKNOWN;
    EXPECT_GT(callStatusManager->HandleVoipCallReportInfo(info), TELEPHONY_ERROR);
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_009, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    CallObjectManager::callObjectPtrList_.clear();
    ContactInfo contactInfo;
    callStatusManager->QueryCallerInfo(contactInfo, "110");
    VoipCallEventInfo voipCallEventInfo;
    EXPECT_EQ(callStatusManager->HandleVoipEventReportInfo(voipCallEventInfo), TELEPHONY_ERR_LOCAL_PTR_NULL);
    CallDetailInfo info;
    callStatusManager->CallFilterCompleteResult(info);
    EXPECT_EQ(callStatusManager->UpdateDialingCallInfo(info), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callStatusManager->IncomingVoipCallHandle(info), CALL_ERR_CALL_OBJECT_IS_NULL);
    EXPECT_EQ(callStatusManager->OutgoingVoipCallHandle(info), CALL_ERR_CALL_OBJECT_IS_NULL);
    bool isDistributedDeviceDialing = false;
    EXPECT_FALSE(callStatusManager->UpdateDialingHandle(info, isDistributedDeviceDialing));
    EXPECT_EQ(callStatusManager->ActiveHandle(info), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callStatusManager->ActiveVoipCallHandle(info), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callStatusManager->HoldingHandle(info), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callStatusManager->AlertHandle(info), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(callStatusManager->DisconnectedVoipCallHandle(info), TELEPHONY_ERR_LOCAL_PTR_NULL);

    info.state = TelCallState::CALL_STATUS_ACTIVE;
    info.callType = CallType::TYPE_VOIP;
    sptr<CallBase> voipCall = callStatusManager->CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    ASSERT_TRUE(voipCall != nullptr);
    callStatusManager->antiFraudSlotId_ = 0;
    callStatusManager->antiFraudIndex_ = 0;
    CallObjectManager::AddOneCallObject(voipCall);
    callStatusManager->SetupAntiFraudService(voipCall, info);
    callStatusManager->StopAntiFraudDetect(voipcall, info);
    callStatusManager->HandleCeliaCall(voipcall);
    callStatusManager->SetConferenceCall({voipCall});
    EXPECT_EQ(CallObjectManager::callObjectPtrList_.size(), 1);
    voipCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_CREATE);
    EXPECT_EQ(callStatusManager->HandleVoipEventReportInfo(voipCallEventInfo), TELEPHONY_ERR_FAIL);
    voipCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    callStatusManager->StartInComingCallMotionRecognition();
    EXPECT_EQ(callStatusManager->HandleVoipEventReportInfo(voipCallEventInfo), TELEPHONY_SUCCESS);
    voipCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_DIALING);
    callStatusManager->StartInComingCallMotionRecognition();
    voipCallEventInfo.voipCallEvent = VoipCallEvent::VOIP_CALL_EVENT_MUTED;
    EXPECT_EQ(callStatusManager->HandleVoipEventReportInfo(voipCallEventInfo), TELEPHONY_SUCCESS);
    voipCallEventInfo.voipCallEvent = VoipCallEvent::VOIP_CALL_EVENT_UNMUTED;
    EXPECT_EQ(callStatusManager->HandleVoipEventReportInfo(voipCallEventInfo), TELEPHONY_SUCCESS);
    EXPECT_EQ(callStatusManager->IncomingVoipCallHandle(info), TELEPHONY_SUCCESS);
    EXPECT_EQ(callStatusManager->OutgoingVoipCallHandle(info), TELEPHONY_SUCCESS);
    info.callMode = VideoStateType::TYPE_VIDEO;
    EXPECT_NE(callStatusManager->OutgoingVoipCallHandle(info), TELEPHONY_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_010, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    CallObjectManager::callObjectPtrList_.clear();
    CallDetailInfo info;
    info.state = TelCallState::CALL_STATUS_ACTIVE;
    info.callType = CallType::TYPE_SATELLITE;
    sptr<CallBase> satelliteCall = callStatusManager->CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    ASSERT_TRUE(satelliteCall != nullptr);
    CallObjectManager::AddOneCallObject(satelliteCall);
    EXPECT_EQ(CallObjectManager::callObjectPtrList_.size(), 1);
    EXPECT_EQ(callStatusManager->UpdateDialingCallInfo(info), TELEPHONY_SUCCESS);
    EXPECT_NE(callStatusManager->HoldingHandle(info), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callStatusManager->DisconnectingHandle(info), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_NE(callStatusManager->DialingHandle(info), TELEPHONY_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_011, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    callStatusManager->StopCallMotionRecognition(TelCallState::CALL_STATUS_ALERTING);
    sptr<CallBase> call = nullptr;
    callStatusManager->SetOriginalCallTypeForActiveState(call);
    callStatusManager->SetOriginalCallTypeForDisconnectState(call);
    callStatusManager->SetVideoCallState(call, TelCallState::CALL_STATUS_DISCONNECTED);
    callStatusManager->SetBtCallDialByPhone(call, false);
    CallDetailInfo info;
    callStatusManager->CreateAndSaveNewCall(info, CallDirection::CALL_DIRECTION_IN);
    info.state = TelCallState::CALL_STATUS_ACTIVE;
    info.callType = CallType::TYPE_IMS;
    call = callStatusManager->CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    ASSERT_TRUE(call != nullptr);
    info.callType = CallType::TYPE_ERR_CALL;
    EXPECT_NE(callStatusManager->RefreshCallIfNecessary(call, info), nullptr);
    info.callType = CallType::TYPE_CS;
    NumberMarkInfo numberMarkInfo;
    numberMarkInfo.markType = MarkType::MARK_TYPE_NONE;
    call->SetNumberLocation("default");
    EXPECT_NE(callStatusManager->RefreshCallIfNecessary(call, info), nullptr);
    call->SetNumberLocation("not_default");
    numberMarkInfo.markType = MarkType::MARK_TYPE_CRANK;
    call->SetNumberMarkInfo(numberMarkInfo);
    callStatusManager->HandleDialWhenHolding(0, call);
    callStatusManager->RefreshCallDisconnectReason(call,
        static_cast<int32_t>(RilDisconnectedReason::DISCONNECTED_REASON_ANSWERED_ELSEWHER));
    callStatusManager->RefreshCallDisconnectReason(call,
        static_cast<int32_t>(RilDisconnectedReason::DISCONNECTED_REASON_NORMAL));
    EXPECT_NE(callStatusManager->RefreshCallIfNecessary(call, info), nullptr);
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_012, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    CallDetailInfo info;
    callStatusManager->callDetailsInfo_[0].callVec.push_back(info);
    callStatusManager->antiFraudSlotId_ = 0;
    callStatusManager->TriggerAntiFraud(static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_DEFAULT));
    callStatusManager->antiFraudIndex_ = -1;
    callStatusManager->antiFraudSlotId_ = 0;
    callStatusManager->TriggerAntiFraud(static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_RISK));
    EXPECT_EQ(callStatusManager->antiFraudSlotId_, -1);
    callStatusManager->antiFraudSlotId_ = 0;
    callStatusManager->TriggerAntiFraud(static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_FINISHED));
    EXPECT_EQ(callStatusManager->antiFraudSlotId_, -1);
    sleep(WAIT_TIME);
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_013, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    CallDetailInfo info;
    bool isExistedOldCall = false;
    CallObjectManager::callObjectPtrList_.clear();
    EXPECT_EQ(callStatusManager->RefreshOldCall(info, isExistedOldCall), TELEPHONY_SUCCESS);
    info.state = TelCallState::CALL_STATUS_ACTIVE;
    info.callType = CallType::TYPE_IMS;
    info.callMode = VideoStateType::TYPE_VOICE;
    callStatusManager->tmpCallDetailsInfo_[0].callVec.push_back(info);
    EXPECT_TRUE(callStatusManager->GetConferenceCallList(0).empty());
    auto &tmpInfo = callStatusManager->tmpCallDetailsInfo_[0].callVec[0];
    tmpInfo.mpty = 1;
    EXPECT_TRUE(callStatusManager->GetConferenceCallList(0).empty());
    sptr<CallBase> call = callStatusManager->CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    ASSERT_TRUE(call != nullptr);
    DialParaInfo paraInfo;
    AppExecFwk::PacMap extras;
    EXPECT_TRUE(callStatusManager->CreateNewCallByCallTypeEx(paraInfo, info, CallDirection::CALL_DIRECTION_IN,
        extras) == nullptr);
    CallObjectManager::AddOneCallObject(call);
    callStatusManager->StartInComingCallMotionRecognition();
    EXPECT_FALSE(callStatusManager->GetConferenceCallList(0).empty());
    callStatusManager->StopCallMotionRecognition(TelCallState::CALL_STATUS_DISCONNECTED);
    EXPECT_GT(callStatusManager->RefreshOldCall(info, isExistedOldCall), TELEPHONY_ERROR);
    info.callType = CallType::TYPE_CS;
    EXPECT_GT(callStatusManager->RefreshOldCall(info, isExistedOldCall), TELEPHONY_ERROR);
    info.state = TelCallState::CALL_STATUS_HOLDING;
    EXPECT_GT(callStatusManager->RefreshOldCall(info, isExistedOldCall), TELEPHONY_ERROR);
    info.callMode = VideoStateType::TYPE_VIDEO;
    EXPECT_GT(callStatusManager->RefreshOldCall(info, isExistedOldCall), TELEPHONY_ERROR);
    info.callType = CallType::TYPE_IMS;
    info.state = TelCallState::CALL_STATUS_ACTIVE;
    EXPECT_GT(callStatusManager->RefreshOldCall(info, isExistedOldCall), TELEPHONY_ERROR);
    info.callType = CallType::TYPE_CS;
    info.callMode = VideoStateType::TYPE_VOICE;
    EXPECT_GT(callStatusManager->RefreshOldCall(info, isExistedOldCall), TELEPHONY_ERROR);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_HOLD);
    callStatusManager->StartInComingCallMotionRecognition();
    callStatusManager->BtCallDialingHandleFirst(call, info);
    info.state = TelCallState::CALL_STATUS_ALERTING;
    callStatusManager->BtCallDialingHandleFirst(call, info);
    call->SetTelCallState(TelCallState::CALL_STATUS_ALERTING);
    callStatusManager->BtCallDialingHandleFirst(call, info);
    EXPECT_EQ(call->GetTelCallState(), TelCallState::CALL_STATUS_DIALING);
    callStatusManager->BtCallDialingHandle(call, info);
    EXPECT_EQ(call->phoneOrWatch_, static_cast<int32_t>(PhoneOrWatchDial::WATCH_DIAL));
}

HWTEST_F(ZeroBranch4Test, Telephony_CallStatusManager_013, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> manager = std::make_shared<CallStatusManager>();
    CallDetailInfo info;
    info.state = TelCallState::CALL_STATUS_ACTIVE;
    info.callType = CallType::TYPE_IMS;
    sptr<CallBase> call = manager->CreateNewCall(info, CallDirection::CALL_DIRECTION_IN);
    ASSERT_TRUE(call != nullptr);
    manager->antiFraudSlotId_ = 0;
    manager->antiFraudIndex_ = 0;
    call->SetSlotId(1);
    manager->StopAntiFraudDetect(call, info);
    manager->HandleCeliaCall(call);
    info.index = 1;
    call->SetCallIndex(1);
    manager->StopAntiFraudDetect(call, info);
    manager->HandleCeliaCall(call);
    call->SetSlotId(0);
    manager->StopAntiFraudDetect(call, info);
    manager->HandleCeliaCall(call);
    EXPECT_EQ(manager->antiFraudIndex_, 0);

    NumberMarkInfo markInfo;
    markInfo.markType = MarkType::MARK_TYPE_FRAUD;
    call->SetNumberMarkInfo(markInfo);
    manager->SetupAntiFraudService(call, info);
    markInfo.markType = MarkType::MARK_TYPE_YELLOW_PAGE;
    call->SetNumberMarkInfo(markInfo);
    manager->SetupAntiFraudService(call, info);
    markInfo.markType = MarkType::MARK_TYPE_TAXI;
    call->SetNumberMarkInfo(markInfo);
    manager->SetupAntiFraudService(call, info);
    EXPECT_EQ(manager->antiFraudIndex_, 0);
}
} // namespace Telephony
} // namespace OHOS
