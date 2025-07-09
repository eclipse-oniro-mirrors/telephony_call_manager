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
#include "call_dialog.h"
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
#include "edm_call_policy.h"

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

class ZeroBranch2Test : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void ZeroBranch2Test::SetUp() {}

void ZeroBranch2Test::TearDown() {}

void ZeroBranch2Test::SetUpTestCase()
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
        .processName = "ZeroBranch2Test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void ZeroBranch2Test::TearDownTestCase() {}

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
 * @tc.number   Telephony_CallRequestHandler_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallRequestHandler_001, Function | MediumTest | Level1)
{
    std::unique_ptr<CallRequestHandler> callRequestHandler = std::make_unique<CallRequestHandler>();
    ASSERT_NE(callRequestHandler->DialCall(), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandler->AnswerCall(1, 1), TELEPHONY_ERR_SUCCESS);
    std::string content = "";
    ASSERT_NE(callRequestHandler->RejectCall(1, true, content), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandler->HangUpCall(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandler->HoldCall(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandler->UnHoldCall(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandler->SwitchCall(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandler->CombineConference(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandler->SeparateConference(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandler->KickOutFromConference(1), TELEPHONY_ERR_SUCCESS);
    std::u16string test = u"";
    ASSERT_NE(callRequestHandler->StartRtt(1, test), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandler->StopRtt(1), TELEPHONY_ERR_SUCCESS);
    std::vector<std::string> emptyRecords = {};
    ASSERT_NE(callRequestHandler->JoinConference(1, emptyRecords), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_CallRequestProcess_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallRequestProcess_001, Function | MediumTest | Level1)
{
    std::unique_ptr<CallRequestProcess> callRequestProcess = std::make_unique<CallRequestProcess>();
    callRequestProcess->DialRequest();
    callRequestProcess->AnswerRequest(1, 1);
    std::string content = "";
    callRequestProcess->RejectRequest(1, true, content);
    callRequestProcess->HangUpRequest(1);
    callRequestProcess->HoldRequest(1);
    callRequestProcess->UnHoldRequest(1);
    callRequestProcess->SwitchRequest(1);
    callRequestProcess->CombineConferenceRequest(1);
    callRequestProcess->SeparateConferenceRequest(1);
    callRequestProcess->KickOutFromConferenceRequest(1);
    std::u16string test = u"";
    callRequestProcess->StartRttRequest(1, test);
    callRequestProcess->StopRttRequest(1);
    std::vector<std::string> numberList = {};
    callRequestProcess->JoinConference(1, numberList);
    DialParaInfo mDialParaInfo;
    callRequestProcess->UpdateCallReportInfo(mDialParaInfo, TelCallState::CALL_STATUS_INCOMING);
    callRequestProcess->HandleDialFail();
    callRequestProcess->GetOtherRingingCall(1);
    callRequestProcess->CarrierDialProcess(mDialParaInfo);
    callRequestProcess->IsDialCallForDsda(mDialParaInfo);
    bool isEcc = false;
    std::string phoneNumber = "123456789012";
    callRequestProcess->HandleEccCallForDsda(phoneNumber, mDialParaInfo, isEcc);
    callRequestProcess->VoiceMailDialProcess(mDialParaInfo);
    callRequestProcess->OttDialProcess(mDialParaInfo);
    CellularCallInfo mCellularCallInfo;
    callRequestProcess->PackCellularCallInfo(mDialParaInfo, mCellularCallInfo);
    std::vector<std::u16string> testList = {};
    callRequestProcess->IsFdnNumber(testList, content);
    callRequestProcess->IsDsdsMode3();
    callRequestProcess->DisconnectOtherSubIdCall(1, 0, 0);
    callRequestProcess->DisconnectOtherCallForVideoCall(1);
    mDialParaInfo.number = "*#21#";
    callRequestProcess->CarrierDialProcess(mDialParaInfo);
    ASSERT_FALSE(callRequestProcess->IsDsdsMode5());
}

/**
 * @tc.number   Telephony_CallRequestProcess_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallRequestProcess_002, Function | MediumTest | Level1)
{
    std::unique_ptr<CallRequestProcess> callRequestProcess = std::make_unique<CallRequestProcess>();
    callRequestProcess->HoldOrDisconnectedCall(VALID_CALLID, SIM1_SLOTID, 1);
    DialParaInfo mDialParaInfo;
    sptr<CallBase> call = new CSCall(mDialParaInfo);
    call->SetCallId(VALID_CALLID);
    call->SetCallType(CallType::TYPE_VOIP);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    callRequestProcess->AddOneCallObject(call);
    callRequestProcess->AnswerRequest(VALID_CALLID, static_cast<int>(CallType::TYPE_VOIP));
    callRequestProcess->AnswerRequest(VALID_CALLID, static_cast<int>(CallType::TYPE_CS));
    callRequestProcess->NeedAnswerVTAndEndActiveVO(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VIDEO));
    callRequestProcess->NeedAnswerVTAndEndActiveVO(ERROR_CALLID, static_cast<int>(VideoStateType::TYPE_VIDEO));
    callRequestProcess->NeedAnswerVOAndEndActiveVT(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VOICE));
    callRequestProcess->NeedAnswerVOAndEndActiveVT(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VIDEO));
    sptr<CallBase> voipCall = new VoIPCall(mDialParaInfo);
    voipCall->SetCallId(VALID_CALLID);
    voipCall->SetCallType(CallType::TYPE_VOIP);
    voipCall->SetTelCallState(TelCallState::CALL_STATUS_HOLDING);
    voipCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_HOLD);
    callRequestProcess->AddOneCallObject(voipCall);
    callRequestProcess->NeedAnswerVTAndEndActiveVO(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VIDEO));
    callRequestProcess->NeedAnswerVTAndEndActiveVO(ERROR_CALLID, static_cast<int>(VideoStateType::TYPE_VIDEO));
    callRequestProcess->NeedAnswerVOAndEndActiveVT(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VOICE));
    callRequestProcess->NeedAnswerVOAndEndActiveVT(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VIDEO));
    callRequestProcess->GetOtherRingingCall(VALID_CALLID);
    callRequestProcess->GetOtherRingingCall(ERROR_CALLID);
    callRequestProcess->HoldOrDisconnectedCall(VALID_CALLID, SIM1_SLOTID,
        static_cast<int>(VideoStateType::TYPE_VIDEO));
    callRequestProcess->HoldOrDisconnectedCall(VALID_CALLID, SIM1_SLOTID,
        static_cast<int>(VideoStateType::TYPE_VOICE));
    std::list<int32_t> list = {1, 2, -1, 0};
    bool noOtherCall = false;
    callRequestProcess->IsExistCallOtherSlot(list, SIM1_SLOTID, noOtherCall);
    sptr<CallBase> dialCall = new IMSCall(mDialParaInfo);
    dialCall->SetCallId(VALID_CALLID);
    dialCall->SetCallType(CallType::TYPE_VOIP);
    dialCall->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
    dialCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_DIALING);
    callRequestProcess->AddOneCallObject(dialCall);
    sptr<CallBase> incomingCall = new CSCall(mDialParaInfo);
    incomingCall->SetCallType(CallType::TYPE_CS);
    bool flagForConference = false;
    callRequestProcess->HandleCallWaitingNumZero(incomingCall, voipCall, SIM1_SLOTID, 2, flagForConference);
    callRequestProcess->HandleCallWaitingNumZero(incomingCall, dialCall, SIM1_SLOTID, 2, flagForConference);
    callRequestProcess->DisconnectOtherCallForVideoCall(VALID_CALLID);
    ASSERT_FALSE(flagForConference);
}

/**
 * @tc.number   Telephony_CallRequestProcess_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallRequestProcess_003, Function | MediumTest | Level1)
{
    std::unique_ptr<CallRequestProcess> callRequestProcess = std::make_unique<CallRequestProcess>();
    DialParaInfo mDialParaInfo;
    sptr<CallBase> call = nullptr;
    callRequestProcess->DeleteOneCallObject(call);
    call = new CSCall(mDialParaInfo);
    call->SetCallId(VALID_CALLID);
    call->SetCallType(CallType::TYPE_VOIP);
    call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    callRequestProcess->AddOneCallObject(call);
    callRequestProcess->DisconnectOtherSubIdCall(VALID_CALLID, SIM1_SLOTID,
        static_cast<int>(VideoStateType::TYPE_VIDEO));
    sptr<CallBase> dialCall = new IMSCall(mDialParaInfo);
    dialCall->SetCallId(2);
    dialCall->SetCallType(CallType::TYPE_VOIP);
    dialCall->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
    dialCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_DIALING);
    callRequestProcess->AddOneCallObject(dialCall);
    callRequestProcess->DisconnectOtherSubIdCall(VALID_CALLID, SIM1_SLOTID,
        static_cast<int>(VideoStateType::TYPE_VIDEO));
    callRequestProcess->DisconnectOtherCallForVideoCall(VALID_CALLID);
    callRequestProcess->DisconnectOtherCallForVideoCall(ERROR_CALLID);
    std::string content = "";
    callRequestProcess->RejectRequest(VALID_CALLID, true, content);
    callRequestProcess->RejectRequest(2, true, content);
    callRequestProcess->HoldRequest(VALID_CALLID);
    callRequestProcess->HoldRequest(2);
    callRequestProcess->CombineConferenceRequest(VALID_CALLID);
    callRequestProcess->SeparateConferenceRequest(VALID_CALLID);
    callRequestProcess->KickOutFromConferenceRequest(VALID_CALLID);
    std::u16string msg = u"";
    callRequestProcess->StartRttRequest(VALID_CALLID, msg);
    callRequestProcess->StartRttRequest(2, msg);
    callRequestProcess->StopRttRequest(VALID_CALLID);
    callRequestProcess->StopRttRequest(2);
    std::vector<std::string> numberList;
    callRequestProcess->JoinConference(VALID_CALLID, numberList);
    callRequestProcess->JoinConference(2, numberList);
    callRequestProcess->isFirstDialCallAdded_ = true;
    callRequestProcess->HandleDialFail();
    callRequestProcess->DeleteOneCallObject(call);
    callRequestProcess->DeleteOneCallObject(dialCall);
    ASSERT_TRUE(callRequestProcess->isFirstDialCallAdded_);
}

/**
 * @tc.number   Telephony_CallRequestProcess_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallRequestProcess_004, Function | MediumTest | Level1)
{
    std::unique_ptr<CallRequestProcess> callRequestProcess = std::make_unique<CallRequestProcess>();
    DialParaInfo info;
    info.dialType = DialType::DIAL_CARRIER_TYPE;
    EXPECT_GT(callRequestProcess->HandleDialRequest(info), TELEPHONY_ERROR);
    info.dialType = DialType::DIAL_VOICE_MAIL_TYPE;
    EXPECT_GT(callRequestProcess->HandleDialRequest(info), TELEPHONY_ERROR);
    info.dialType = DialType::DIAL_OTT_TYPE;
    EXPECT_GT(callRequestProcess->HandleDialRequest(info), TELEPHONY_ERROR);
    info.dialType = DialType::DIAL_BLUETOOTH_TYPE;
    EXPECT_GT(callRequestProcess->HandleDialRequest(info), TELEPHONY_ERROR);
    sleep(1);
    std::vector<std::u16string> fdnNumberList = { u"11111", u"22222" };
    EXPECT_TRUE(callRequestProcess->IsFdnNumber(fdnNumberList, "22222"));
    EXPECT_GT(callRequestProcess->BluetoothDialProcess(info), TELEPHONY_ERROR);
}

/**
 * @tc.number   Telephony_CallRequestProcess_005
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallRequestProcess_005, Function | MediumTest | Level1)
{
    std::unique_ptr<CallRequestProcess> callRequestProcess = std::make_unique<CallRequestProcess>();
    callRequestProcess->isFirstDialCallAdded_ = false;
    CallObjectManager::callObjectPtrList_.clear();
    DialParaInfo info;
    sptr<CallBase> imsCall = new IMSCall(info);
    imsCall->SetCallType(CallType::TYPE_IMS);
    imsCall->SetCallIndex(0);
    imsCall->SetCallId(VALID_CALLID);
    imsCall->SetSlotId(0);
    imsCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_CONNECTING);
    CallObjectManager::AddOneCallObject(imsCall);
    callRequestProcess->HandleDialFail();
    CallDetailInfo callDetailInfo;
    callDetailInfo.callType = imsCall->GetCallType();
    callDetailInfo.accountId = imsCall->GetSlotId();
    callDetailInfo.state = TelCallState::CALL_STATUS_DISCONNECTED;
    callDetailInfo.voiceDomain = static_cast<int32_t>(imsCall->GetCallType());
    std::shared_ptr<CallStatusManager> callStatusManagerPtr = std::make_shared<CallStatusManager>();
    callStatusManagerPtr->DisconnectedHandle(callDetailInfo);
    EXPECT_FALSE(CallObjectManager::HasCellularCallExist());
}

/**
 * @tc.number   Telephony_CallObjectManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallObjectManager_001, Function | MediumTest | Level1)
{
    CallObjectManager::HasNewCall();
    DialParaInfo mDialParaInfo;
    sptr<CallBase> csCall = new CSCall(mDialParaInfo);
    csCall->SetCallType(CallType::TYPE_CS);
    csCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_DIALING);
    CallObjectManager::AddOneCallObject(csCall);
    CallObjectManager::HasNewCall();
    csCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_CREATE);
    CallObjectManager::HasNewCall();
    csCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_CONNECTING);
    CallObjectManager::HasNewCall();
    CallObjectManager::HasRingingMaximum();
    csCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    CallObjectManager::HasRingingMaximum();
    CallObjectManager::HasDialingMaximum();
    csCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    CallObjectManager::HasDialingMaximum();
    std::string number = "";
    CallObjectManager::IsCallExist(number);
    number = "test";
    bool res = CallObjectManager::IsCallExist(number);
    CallObjectManager::HasVideoCall();
    csCall->SetVideoStateType(VideoStateType::TYPE_VIDEO);
    CallObjectManager::HasVideoCall();
    csCall->SetCallType(CallType::TYPE_VOIP);
    CallObjectManager::HasVideoCall();
    CallObjectManager::GetCallInfoList(DEFAULT_INDEX);
    csCall->SetCallType(CallType::TYPE_OTT);
    CallObjectManager::GetCallInfoList(SIM1_SLOTID);
    CallObjectManager::GetCallInfoList(DEFAULT_INDEX);
    CallObjectManager::GetCallInfoList(DEFAULT_INDEX, false);
    CallObjectManager::GetAllCallInfoList(false);
    ASSERT_FALSE(res);
}

/**
 * @tc.number   Telephony_CallObjectManager_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallObjectManager_002, Function | MediumTest | Level1)
{
    DialParaInfo mDialParaInfo;
    sptr<CallBase> csCall = new CSCall(mDialParaInfo);
    csCall->callId_ = 0;
    csCall->callType_ = CallType::TYPE_VOIP;
    csCall->callRunningState_ = CallRunningState::CALL_RUNNING_STATE_RINGING;
    csCall->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    CallObjectManager::AddOneCallObject(csCall);
    CallObjectManager::GetOneCallObject(0);
    std::list<int32_t> list;
    CallObjectManager::GetVoipCallList(list);
    CallObjectManager::GetVoipCallNum();
    CallObjectManager::GetAllCallList();
    bool hasRingingCall = false;
    CallObjectManager::HasRingingCall(hasRingingCall);
    csCall->callRunningState_ = CallRunningState::CALL_RUNNING_STATE_HOLD;
    CallObjectManager::HasHoldCall(hasRingingCall);
    ASSERT_FALSE(CallObjectManager::IsCallExist(CallType::TYPE_VOIP, TelCallState::CALL_STATUS_ACTIVE));
    std::list<int32_t> satelliteCallList;
    EXPECT_EQ(CallObjectManager::GetSatelliteCallList(satelliteCallList), TELEPHONY_SUCCESS);
    EXPECT_EQ(CallObjectManager::GetForegroundCall(false), nullptr);
    EXPECT_EQ(CallObjectManager::GetForegroundCall(true), nullptr);
    EXPECT_EQ(CallObjectManager::GetCallNumByRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE), 1);
    EXPECT_EQ(CallObjectManager::GetCallNumByRunningState(CallRunningState::CALL_RUNNING_STATE_DIALING), 0);
    EXPECT_EQ(CallObjectManager::GetForegroundLiveCallByCallId(0), nullptr);
    EXPECT_FALSE(CallObjectManager::HasIncomingCallCrsType());
    CallObjectManager::DeleteOneCallObject(0);
}

/**
 * @tc.number   Telephony_CallObjectManager_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallObjectManager_003, Function | MediumTest | Level1)
{
    DialParaInfo dialInfo;
    sptr<CallBase> call = new VoIPCall(dialInfo);
    call->callId_ = 0;
    call->callType_ = CallType::TYPE_VOIP;
    call->callRunningState_ = CallRunningState::CALL_RUNNING_STATE_RINGING;
    call->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    CallObjectManager::AddOneCallObject(call);
    call->SetCallIndex(0);
    call->SetSlotId(0);
    EXPECT_TRUE(CallObjectManager::GetOneCallObjectByIndexAndSlotId(0, 0) == nullptr);
    EXPECT_TRUE(CallObjectManager::GetOneCallObjectByIndexAndSlotId(0, 1) == nullptr);
    EXPECT_TRUE(CallObjectManager::GetOneCallObjectByIndexAndSlotId(1, 0) == nullptr);
    EXPECT_TRUE(CallObjectManager::GetOneCallObjectByIndexAndSlotId(1, 1) == nullptr);
    EXPECT_TRUE(CallObjectManager::HasVoipCallExist());
    int32_t callId = 0;
    EXPECT_TRUE(CallObjectManager::IsCallExist(0));
    EXPECT_FALSE(CallObjectManager::IsCallExist(1));
    CallAttributeInfo info;
    CallObjectManager::AddOneVoipCallObject(info);
    CallObjectManager::GetActiveVoipCallInfo();
    info.callState = TelCallState::CALL_STATUS_ACTIVE;
    CallObjectManager::AddOneVoipCallObject(info);
    CallAttributeInfo info2;
    info2.callId = 1;
    info2.callState = TelCallState::CALL_STATUS_ACTIVE;
    CallObjectManager::AddOneVoipCallObject(info2);
    CallObjectManager::GetActiveVoipCallInfo();
    EXPECT_FALSE(CallObjectManager::IsVoipCallExist(TelCallState::CALL_STATUS_DIALING, callId));
    EXPECT_TRUE(CallObjectManager::IsVoipCallExist(TelCallState::CALL_STATUS_ACTIVE, callId));
    std::string phoneNum = "";
    EXPECT_EQ(CallObjectManager::GetOneCallObject(phoneNum), nullptr);
    bool enabled = false;
    EXPECT_GT(CallObjectManager::IsNewCallAllowedCreate(enabled), TELEPHONY_ERROR);
    CallObjectManager::DeleteOneCallObject(0);
}

/**
 * @tc.number   Telephony_CallObjectManager_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallObjectManager_004, Function | MediumTest | Level1)
{
    DialParaInfo dialInfo;
    sptr<CallBase> call = new CSCall(dialInfo);
    call->callId_ = 1;
    call->SetCallIndex(0);
    call->SetSlotId(0);
    call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    call->SetCallType(CallType::TYPE_CS);
    CallObjectManager::AddOneCallObject(call);
    EXPECT_TRUE(CallObjectManager::HasCellularCallExist());
    call->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTED);
    EXPECT_FALSE(CallObjectManager::HasCellularCallExist());
    call->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTING);
    EXPECT_FALSE(CallObjectManager::HasCellularCallExist());
    EXPECT_TRUE(CallObjectManager::GetOneCallObjectByIndexAndSlotId(0, 0) != nullptr);
    EXPECT_TRUE(CallObjectManager::GetOneCallObjectByIndexAndSlotId(0, 1) == nullptr);
    EXPECT_TRUE(CallObjectManager::GetOneCallObjectByIndexAndSlotId(1, 0) == nullptr);
    EXPECT_TRUE(CallObjectManager::GetOneCallObjectByIndexAndSlotId(1, 1) == nullptr);
    bool enabled = false;
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_CREATE);
    EXPECT_GT(CallObjectManager::IsNewCallAllowedCreate(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasRingingCall(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasHoldCall(enabled), TELEPHONY_ERROR);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_CONNECTING);
    EXPECT_GT(CallObjectManager::IsNewCallAllowedCreate(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasRingingCall(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasHoldCall(enabled), TELEPHONY_ERROR);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_DIALING);
    EXPECT_GT(CallObjectManager::IsNewCallAllowedCreate(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasRingingCall(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasHoldCall(enabled), TELEPHONY_ERROR);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    EXPECT_GT(CallObjectManager::IsNewCallAllowedCreate(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasRingingCall(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasHoldCall(enabled), TELEPHONY_ERROR);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ENDED);
    EXPECT_GT(CallObjectManager::IsNewCallAllowedCreate(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasRingingCall(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasHoldCall(enabled), TELEPHONY_ERROR);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_HOLD);
    EXPECT_GT(CallObjectManager::HasRingingCall(enabled), TELEPHONY_ERROR);
    EXPECT_GT(CallObjectManager::HasHoldCall(enabled), TELEPHONY_ERROR);
    CallObjectManager::DeleteOneCallObject(1);
}

/**
 * @tc.number   Telephony_CallNumberUtils_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallNumberUtils_001, Function | MediumTest | Level1)
{
    std::string emptyStr = "";
    std::string phoneNumber = "123456789012";
    std::string countryCode = "gr";
    std::string formatNumber = "";
    NumberMarkInfo numberMarkInfo;
    ASSERT_NE(DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumber(emptyStr, emptyStr, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumber(phoneNumber, emptyStr, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(
        DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumber(phoneNumber, countryCode, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_GE(
        DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(emptyStr, emptyStr, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(
        DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(phoneNumber, emptyStr, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(
        phoneNumber, countryCode, formatNumber), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToNational(
        emptyStr, emptyStr, formatNumber), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToInternational(
        emptyStr, emptyStr, formatNumber), TELEPHONY_ERR_SUCCESS);
    ASSERT_FALSE(DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(INVALID_SLOTID));
    ASSERT_TRUE(DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(0));
    EXPECT_NE(DelayedSingleton<CallNumberUtils>::GetInstance()->QueryYellowPageAndMarkInfo(
        numberMarkInfo, phoneNumber), TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallNumberUtils_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallNumberUtils_002, Function | MediumTest | Level1)
{
    std::string emptyStr = "";
    std::string phoneNumber = "123456789012";
    std::string numberLocation = "";
    EXPECT_NE(DelayedSingleton<CallNumberUtils>::GetInstance()->QueryNumberLocationInfo(numberLocation, emptyStr),
        TELEPHONY_ERR_SUCCESS);
    std::shared_ptr<NumberIdentityDataBaseHelper> callDataPtr =
        DelayedSingleton<NumberIdentityDataBaseHelper>::GetInstance();
    printf("NUMBER_IDENTITY_URI: %s\n", callDataPtr->NUMBER_IDENTITY_URI);
    auto helper = CreateDataShareHelper(callDataPtr->NUMBER_IDENTITY_URI);
    if (helper != nullptr) {
        helper->Release();
        EXPECT_EQ(DelayedSingleton<CallNumberUtils>::GetInstance()->
            QueryNumberLocationInfo(numberLocation, phoneNumber), TELEPHONY_ERR_SUCCESS);
    } else {
        printf("helper is null");
        EXPECT_NE(DelayedSingleton<CallNumberUtils>::GetInstance()->
            QueryNumberLocationInfo(numberLocation, phoneNumber), TELEPHONY_ERR_SUCCESS);
    }
}

/**
 * @tc.number   Telephony_CellularCallConnection_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CellularCallConnection_001, Function | MediumTest | Level1)
{
    std::shared_ptr<CellularCallConnection> cellularCallConnection =
        DelayedSingleton<CellularCallConnection>::GetInstance();
    CellularCallInfo mCellularCallInfo;
    ASSERT_NE(cellularCallConnection->Dial(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->HangUp(
        mCellularCallInfo, CallSupplementType::TYPE_DEFAULT), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->Reject(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->Answer(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->HoldCall(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->UnHoldCall(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SwitchCall(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    bool enabled = false;
    ASSERT_NE(cellularCallConnection->IsEmergencyPhoneNumber("", 0, enabled), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->CombineConference(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SeparateConference(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->KickOutFromConference(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->StartDtmf('a', mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->StopDtmf(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->GetCallTransferInfo(
        CallTransferType::TRANSFER_TYPE_BUSY, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SetCallWaiting(true, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->GetCallWaiting(0), TELEPHONY_ERR_SUCCESS);
    CallRestrictionInfo mCallRestrictionInfo;
    ASSERT_NE(cellularCallConnection->SetCallRestriction(mCallRestrictionInfo, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->GetCallRestriction(
        CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SetCallPreferenceMode(0, 1), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_CellularCallConnection_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CellularCallConnection_002, Function | MediumTest | Level1)
{
    CallTransferInfo mCallTransferInfo;
    std::shared_ptr<CellularCallConnection> cellularCallConnection =
        DelayedSingleton<CellularCallConnection>::GetInstance();
    ASSERT_NE(cellularCallConnection->SetCallTransferInfo(mCallTransferInfo, 0), TELEPHONY_ERR_SUCCESS);
    bool result;
    ASSERT_NE(cellularCallConnection->CanSetCallTransferTime(0, result), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->GetImsSwitchStatus(0, result), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SetImsConfig(ImsConfigItem::ITEM_VIDEO_QUALITY, "", 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SetImsConfig(ImsConfigItem::ITEM_VIDEO_QUALITY, 1, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->GetImsConfig(ImsConfigItem::ITEM_VIDEO_QUALITY, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        cellularCallConnection->SetImsFeatureValue(FeatureType::TYPE_VOICE_OVER_LTE, 1, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->GetImsFeatureValue(FeatureType::TYPE_VOICE_OVER_LTE, 0), TELEPHONY_ERR_SUCCESS);
    std::vector<std::string> numberList = {};
    ASSERT_NE(cellularCallConnection->InviteToConference(numberList, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SetMute(0, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SendDtmf('a', ""), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        cellularCallConnection->SendDtmfString("", "", PhoneNetType::PHONE_TYPE_GSM, 1, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->RegisterCallBack(nullptr), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SetImsSwitchStatus(0, true), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->ConnectService(), TELEPHONY_ERR_SUCCESS);
    cellularCallConnection->ClearAllCalls();
    std::string testStr = "";
    ASSERT_NE(cellularCallConnection->ControlCamera(SIM1_SLOTID, DEFAULT_INDEX, testStr, 1, 1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        cellularCallConnection->SetPreviewWindow(SIM1_SLOTID, DEFAULT_INDEX, testStr, nullptr), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        cellularCallConnection->SetDisplayWindow(SIM1_SLOTID, DEFAULT_INDEX, testStr, nullptr), TELEPHONY_ERR_SUCCESS);
    float zoomRatio = 1;
    ASSERT_NE(cellularCallConnection->SetCameraZoom(zoomRatio), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SetPausePicture(SIM1_SLOTID, DEFAULT_INDEX, testStr), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SetDeviceDirection(SIM1_SLOTID, DEFAULT_INDEX, 1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->CancelCallUpgrade(SIM1_SLOTID, DEFAULT_INDEX), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->RequestCameraCapabilities(SIM1_SLOTID, DEFAULT_INDEX), TELEPHONY_ERR_SUCCESS);
    int32_t slotId = 0;
    std::string content = "1";
    ASSERT_EQ(cellularCallConnection->SendUssdResponse(slotId, content), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_CellularCallConnection_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CellularCallConnection_003, Function | MediumTest | Level1)
{
    CellularCallInfo mCellularCallInfo;
    std::shared_ptr<CellularCallConnection> cellularCallConnection =
        DelayedSingleton<CellularCallConnection>::GetInstance();
    std::u16string test = u"";
    ASSERT_NE(cellularCallConnection->StartRtt(mCellularCallInfo, test), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->StopRtt(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->SendUpdateCallMediaModeRequest(
        mCellularCallInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->RegisterCallBackFun(), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->ReConnectService(), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(cellularCallConnection->PostDialProceed(mCellularCallInfo, true), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_CellularCallConnection_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CellularCallConnection_004, Function | MediumTest | Level1)
{
    CellularCallInfo mCellularCallInfo;
    std::shared_ptr<CellularCallConnection> cellularCallConnection =
        DelayedSingleton<CellularCallConnection>::GetInstance();
    std::u16string test = u"";
    int32_t state = 0;
    cellularCallConnection->SetVoNRState(SIM1_SLOTID, state);
    cellularCallConnection->GetVoNRState(SIM1_SLOTID, state);
    int res = cellularCallConnection->CloseUnFinishedUssd(SIM1_SLOTID);
    CellularCallConnection::SystemAbilityListener listen;
    int32_t systemAbilityId = 1;
    std::string deviceId = "123";
    listen.OnAddSystemAbility(systemAbilityId, deviceId);
    listen.OnRemoveSystemAbility(systemAbilityId, deviceId);
    ASSERT_NE(res, TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_CallPolicy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallPolicy_001, Function | MediumTest | Level1)
{
    CallPolicy mCallPolicy;
    std::u16string testEmptyStr = u"";
    AppExecFwk::PacMap mPacMap;
    mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true);
    mPacMap.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_CARRIER_TYPE));
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_VOICE_MAIL_TYPE));
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("callType", static_cast<int32_t>(CallType::TYPE_CS));
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialScene", static_cast<int32_t>(DialScene::CALL_NORMAL));
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, false), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialScene", static_cast<int32_t>(DialScene::CALL_PRIVILEGED));
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, false), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialScene", static_cast<int32_t>(DialScene::CALL_EMERGENCY));
    mPacMap.PutIntValue("videoState", static_cast<int32_t>(VideoStateType::TYPE_VOICE));
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, false), TELEPHONY_ERR_SUCCESS);
    system::SetParameter("persist.edm.telephony_call_disable", "true");
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_POLICY_DISABLED);
    system::SetParameter("persist.edm.telephony_call_disable", "false");
    mPacMap.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_VOICE_MAIL_TYPE));
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, false), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.AnswerCallPolicy(0, INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.AnswerCallPolicy(0, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.RejectCallPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_GE(mCallPolicy.HoldCallPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.UnHoldCallPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.HangUpPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.SwitchCallPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_GE(mCallPolicy.VideoCallPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.StartRttPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.StopRttPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.IsValidSlotId(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_FALSE(mCallPolicy.IsSupportVideoCall(mPacMap));
    ASSERT_GE(mCallPolicy.CanDialMulityCall(mPacMap, false), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_CallPolicy_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallPolicy_002, Function | MediumTest | Level1)
{
    CallPolicy mCallPolicy;
    ASSERT_EQ(mCallPolicy.IsValidSlotId(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.EnableVoLtePolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.EnableVoLtePolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.DisableVoLtePolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.DisableVoLtePolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.IsVoLteEnabledPolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.IsVoLteEnabledPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.GetCallWaitingPolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.GetCallWaitingPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.SetCallWaitingPolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.SetCallWaitingPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.GetCallRestrictionPolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.GetCallRestrictionPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.SetCallRestrictionPolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.SetCallRestrictionPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.GetCallTransferInfoPolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.GetCallTransferInfoPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.SetCallTransferInfoPolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.SetCallTransferInfoPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.SetCallPreferenceModePolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.SetCallPreferenceModePolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.GetImsConfigPolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.GetImsConfigPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.SetImsConfigPolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.SetImsConfigPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.GetImsFeatureValuePolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.GetImsFeatureValuePolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.SetImsFeatureValuePolicy(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.SetImsFeatureValuePolicy(0), TELEPHONY_ERR_SUCCESS);
    std::vector<std::string> numberList = {};
    ASSERT_NE(mCallPolicy.InviteToConferencePolicy(0, numberList), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_CallPolicy_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallPolicy_003, Function | MediumTest | Level1)
{
    CallPolicy callPolicy;
    AppExecFwk::PacMap videoExtras;
    videoExtras.PutIntValue("videoState", static_cast<int>(VideoStateType::TYPE_VIDEO));
    callPolicy.CanDialMulityCall(videoExtras, false);
    AppExecFwk::PacMap voiceExtras;
    voiceExtras.PutIntValue("videoState", static_cast<int>(VideoStateType::TYPE_VOICE));
    callPolicy.CanDialMulityCall(voiceExtras, false);
    DialParaInfo info;
    sptr<CallBase> call = new CSCall(info);
    call->SetCallType(CallType::TYPE_CS);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    callPolicy.AddOneCallObject(call);
    callPolicy.IsVoiceCallValid(VideoStateType::TYPE_VIDEO);
    callPolicy.IsVoiceCallValid(VideoStateType::TYPE_VOICE);
    call->SetVideoStateType(VideoStateType::TYPE_VIDEO);
    callPolicy.IsVoiceCallValid(VideoStateType::TYPE_VOICE);
    call->SetCallType(CallType::TYPE_VOIP);
    callPolicy.IsVoiceCallValid(VideoStateType::TYPE_VOICE);
    call->SetVideoStateType(VideoStateType::TYPE_VOICE);
    call->SetCallType(CallType::TYPE_CS);
    callPolicy.IsVoiceCallValid(VideoStateType::TYPE_VOICE);
    callPolicy.IsValidCallType(CallType::TYPE_CS);
    callPolicy.IsValidCallType(CallType::TYPE_IMS);
    callPolicy.IsValidCallType(CallType::TYPE_OTT);
    callPolicy.IsValidCallType(CallType::TYPE_SATELLITE);
    callPolicy.IsValidCallType(CallType::TYPE_VOIP);
    callPolicy.CanDialMulityCall(videoExtras, false);
    callPolicy.CanDialMulityCall(voiceExtras, false);
    call->SetVideoStateType(VideoStateType::TYPE_VIDEO);
    callPolicy.CanDialMulityCall(videoExtras, false);
    callPolicy.CanDialMulityCall(voiceExtras, false);
    call->SetCallId(VALID_CALLID);
    call->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
    callPolicy.RejectCallPolicy(VALID_CALLID);
    callPolicy.AnswerCallPolicy(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VOICE));
    callPolicy.AnswerCallPolicy(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VIDEO));
    call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    callPolicy.RejectCallPolicy(VALID_CALLID);
    callPolicy.AnswerCallPolicy(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VOICE));
    callPolicy.AnswerCallPolicy(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VIDEO));
    call->SetTelCallState(TelCallState::CALL_STATUS_WAITING);
    callPolicy.RejectCallPolicy(VALID_CALLID);
    callPolicy.AnswerCallPolicy(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VOICE));
    callPolicy.AnswerCallPolicy(VALID_CALLID, static_cast<int>(VideoStateType::TYPE_VIDEO));
    ASSERT_GE(callPolicy.RejectCallPolicy(VALID_CALLID), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_CallPolicy_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallPolicy_004, Function | MediumTest | Level1)
{
    CallPolicy callPolicy;
    DialParaInfo info;
    sptr<CallBase> call = new CSCall(info);
    call->SetCallType(CallType::TYPE_CS);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    call->SetCallId(VALID_CALLID);
    callPolicy.AddOneCallObject(call);
    callPolicy.HoldCallPolicy(VALID_CALLID);
    callPolicy.UnHoldCallPolicy(VALID_CALLID);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_HOLD);
    callPolicy.HoldCallPolicy(VALID_CALLID);
    callPolicy.UnHoldCallPolicy(VALID_CALLID);
    call->SetTelCallState(TelCallState::CALL_STATUS_IDLE);
    callPolicy.HangUpPolicy(VALID_CALLID);
    call->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTING);
    callPolicy.HangUpPolicy(VALID_CALLID);
    call->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTED);
    callPolicy.HangUpPolicy(VALID_CALLID);
    callPolicy.SwitchCallPolicy(VALID_CALLID);
    sptr<CallBase> imsCall = new IMSCall(info);
    imsCall->SetCallType(CallType::TYPE_IMS);
    imsCall->SetCallId(2);
    callPolicy.AddOneCallObject(imsCall);
    imsCall->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
    ASSERT_EQ(callPolicy.SwitchCallPolicy(VALID_CALLID), CALL_ERR_ILLEGAL_CALL_OPERATION);
    imsCall->SetTelCallState(TelCallState::CALL_STATUS_ALERTING);
    ASSERT_EQ(callPolicy.SwitchCallPolicy(VALID_CALLID), CALL_ERR_ILLEGAL_CALL_OPERATION);
    call->SetTelCallState(TelCallState::CALL_STATUS_HOLDING);
    ASSERT_EQ(callPolicy.SwitchCallPolicy(VALID_CALLID), CALL_ERR_ILLEGAL_CALL_OPERATION);
    imsCall->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_EQ(callPolicy.SwitchCallPolicy(VALID_CALLID), TELEPHONY_ERR_SUCCESS);
    callPolicy.VideoCallPolicy(VALID_CALLID);
    callPolicy.StartRttPolicy(VALID_CALLID);
    callPolicy.StartRttPolicy(2);
    callPolicy.StopRttPolicy(VALID_CALLID);
    callPolicy.StopRttPolicy(2);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    callPolicy.StartRttPolicy(VALID_CALLID);
    callPolicy.StopRttPolicy(VALID_CALLID);
    std::vector<std::string> numberList;
    callPolicy.InviteToConferencePolicy(VALID_CALLID, numberList);
    numberList.push_back("");
    numberList.push_back("123");
    numberList.push_back(
        "19119080646435437102938190283912471651865851478647016881846814376871464810514786470168818468143768714648");
    ASSERT_GE(callPolicy.InviteToConferencePolicy(VALID_CALLID, numberList), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_CallPolicy_005
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallPolicy_005, Function | MediumTest | Level1)
{
    CallPolicy callPolicy;
    ASSERT_EQ(callPolicy.IsValidSlotId(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.EnableVoLtePolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.DisableVoLtePolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.IsVoLteEnabledPolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.VoNRStatePolicy(INVALID_SLOTID, -1), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.GetCallWaitingPolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.SetCallWaitingPolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.GetCallRestrictionPolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.SetCallRestrictionPolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.GetCallTransferInfoPolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.SetCallTransferInfoPolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.SetCallPreferenceModePolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.GetImsConfigPolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.SetImsConfigPolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.GetImsFeatureValuePolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.SetImsFeatureValuePolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
    ASSERT_EQ(callPolicy.CloseUnFinishedUssdPolicy(INVALID_SLOTID), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallPolicy_006
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallPolicy_006, Function | MediumTest | Level1)
{
    CallPolicy callPolicy;
    bool isEcc = false;
    int32_t slotId = 0;
    std::string number = "111111";
    callPolicy.HasNormalCall(isEcc, slotId, CallType::TYPE_IMS);
    bool isAirplaneModeOn = false;
    callPolicy.GetAirplaneMode(isAirplaneModeOn);
    ASSERT_EQ(isAirplaneModeOn, false);
    std::vector<std::string> dialingList;
    std::vector<std::string> incomingList;
    ASSERT_EQ(callPolicy.IsDialingEnable(number), true);
    ASSERT_EQ(callPolicy.IsIncomingEnable(number), true);
    dialingList.push_back(number);
    incomingList.push_back(number);
    ASSERT_EQ(callPolicy.SetEdmPolicy(0, dialingList, 0, incomingList), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(callPolicy.IsDialingEnable(number), false);
    ASSERT_EQ(callPolicy.IsIncomingEnable(number), false);
}

/**
 * @tc.number   Telephony_ReportCallInfoHandler_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_ReportCallInfoHandler_001, Function | MediumTest | Level1)
{
    CallDetailInfo mCallDetailInfo;
    DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallReportInfo(mCallDetailInfo);
    CallDetailsInfo mCallDetailsInfo;
    ASSERT_NE(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallsReportInfo(mCallDetailsInfo),
        TELEPHONY_ERR_SUCCESS);
    DisconnectedDetails mDisconnectedDetails;
    ASSERT_NE(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateDisconnectedCause(mDisconnectedDetails),
        TELEPHONY_ERR_SUCCESS);
    CellularCallEventInfo mCellularCallEventInfo;
    ASSERT_NE(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateEventResultInfo(mCellularCallEventInfo),
        TELEPHONY_ERR_SUCCESS);
    OttCallEventInfo mOttCallEventInfo;
    ASSERT_NE(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateOttEventInfo(mOttCallEventInfo),
        TELEPHONY_ERR_SUCCESS);
    DelayedSingleton<ReportCallInfoHandler>::GetInstance()->Init();
    ASSERT_EQ(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallReportInfo(mCallDetailInfo),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallsReportInfo(mCallDetailsInfo),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateDisconnectedCause(mDisconnectedDetails),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateEventResultInfo(mCellularCallEventInfo),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateOttEventInfo(mOttCallEventInfo),
        TELEPHONY_ERR_SUCCESS);
    CallModeReportInfo mCallModeRequestInfo;
    ASSERT_EQ(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->ReceiveImsCallModeRequest(mCallModeRequestInfo),
        TELEPHONY_ERR_SUCCESS);
    CallModeReportInfo mCallModeResponseInfo;
    ASSERT_EQ(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->ReceiveImsCallModeResponse(mCallModeResponseInfo),
        TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_VideoControlManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_VideoControlManager_001, Function | MediumTest | Level1)
{
    std::u16string testEmptyStr = u"";
    std::u16string testStr = u"123";
    std::string testEmptyStr_ = "";
    std::string testStr_ = "123";
    int32_t callId = 1;
    uint64_t tempSurfaceId = std::stoull(testStr_);
    auto surface = SurfaceUtils::GetInstance()->GetSurface(tempSurfaceId);
    if (surface == nullptr) {
        testStr_ = "";
    }
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->ControlCamera(callId, testEmptyStr, 1, 1),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->ControlCamera(callId, testStr, 1, 1),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetPreviewWindow(callId, testEmptyStr_, nullptr),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetPreviewWindow(callId, testStr_, surface),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetDisplayWindow(callId, testEmptyStr_, nullptr),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetDisplayWindow(callId, testStr_, surface),
        TELEPHONY_ERR_SUCCESS);
    float zoomRatio = 11;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetCameraZoom(zoomRatio), TELEPHONY_ERR_SUCCESS);
    zoomRatio = 0.01;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetCameraZoom(zoomRatio), TELEPHONY_ERR_SUCCESS);
    zoomRatio = 1;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetCameraZoom(zoomRatio), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<VideoControlManager>::GetInstance()->SetPausePicture(callId, testStr), TELEPHONY_ERR_SUCCESS);
    int32_t rotation = CAMERA_ROTATION_0;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(callId, rotation),
        TELEPHONY_ERR_SUCCESS);
    rotation = CAMERA_ROTATION_90;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(callId, rotation),
        TELEPHONY_ERR_SUCCESS);
    rotation = CAMERA_ROTATION_180;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(callId, rotation),
        TELEPHONY_ERR_SUCCESS);
    rotation = CAMERA_ROTATION_270;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(callId, rotation),
        TELEPHONY_ERR_SUCCESS);
    rotation = 1;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(callId, rotation),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->CancelCallUpgrade(callId), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->RequestCameraCapabilities(callId),
        TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_VideoControlManager_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_VideoControlManager_002, Function | MediumTest | Level1)
{
    int32_t callId = 1;
    std::u16string testStr = u"123";
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->OpenCamera(callId, testStr, 0, 0),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->CloseCamera(callId, testStr, 0, 0),
        TELEPHONY_ERR_SUCCESS);
    ImsCallMode mode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    CallMediaModeInfo imsCallModeInfo;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->ReportImsCallModeInfo(imsCallModeInfo),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<VideoControlManager>::GetInstance()->UpdateImsCallMode(callId, mode), TELEPHONY_ERR_SUCCESS);
    ASSERT_FALSE(DelayedSingleton<VideoControlManager>::GetInstance()->ContainCameraID(""));
    VideoWindow mVideoWindow;
    mVideoWindow.width = -1;
    mVideoWindow.height = 1;
    ASSERT_FALSE(DelayedSingleton<VideoControlManager>::GetInstance()->CheckWindow(mVideoWindow));
    mVideoWindow.width = 1;
    mVideoWindow.height = -1;
    ASSERT_FALSE(DelayedSingleton<VideoControlManager>::GetInstance()->CheckWindow(mVideoWindow));
    mVideoWindow.width = -1;
    mVideoWindow.height = -1;
    ASSERT_FALSE(DelayedSingleton<VideoControlManager>::GetInstance()->CheckWindow(mVideoWindow));
    mVideoWindow.width = 1;
    mVideoWindow.height = 1;
    mVideoWindow.z = 2;
    ASSERT_FALSE(DelayedSingleton<VideoControlManager>::GetInstance()->CheckWindow(mVideoWindow));
    mVideoWindow.z = 0;
    ASSERT_TRUE(DelayedSingleton<VideoControlManager>::GetInstance()->CheckWindow(mVideoWindow));
    mVideoWindow.z = 1;
    ASSERT_TRUE(DelayedSingleton<VideoControlManager>::GetInstance()->CheckWindow(mVideoWindow));
}

/**
 * @tc.number   Telephony_CallDialog_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallDialog_001, Function | MediumTest | Level1)
{
    auto callDialog = DelayedSingleton<CallDialog>::GetInstance();
    ASSERT_NE(callDialog, nullptr);
    callDialog->DialogProcessMMICodeExtension();
    std::string diallogReason = "SATELLITE";
    int32_t slotId = 0;
    ASSERT_TRUE(callDialog->DialogConnectExtension(diallogReason, slotId));
    ASSERT_TRUE(callDialog->DialogConnectExtension(diallogReason));
    std::u16string number = u"13333333333";
    int32_t videoState = 0;
    int32_t dialType = 0;
    int32_t dialScene = 0;
    int32_t callType = 1;
    bool isVideo = false;
    ASSERT_TRUE(callDialog->DialogConnectPrivpacyModeExtension(
        diallogReason, number, slotId, videoState, dialType, dialScene, callType, isVideo));
    ASSERT_TRUE(callDialog->DialogConnectAnswerPrivpacyModeExtension(diallogReason, slotId, videoState, isVideo));
    callDialog->DialogCallingPrivacyModeExtension(Rosen::FoldStatus::FOLDED);
    callDialog->DialogCallingPrivacyModeExtension(Rosen::FoldStatus::EXPAND);
}

/**
 * @tc.number   Telephony_CallNumberUtils_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_CallNumberUtils_003, Function | MediumTest | Level1)
{
    auto cellularCallConnection = DelayedSingleton<CellularCallConnection>::GetInstance();
    EXPECT_NE(cellularCallConnection, nullptr);
    std::string dialStr = "";
    ASSERT_FALSE(cellularCallConnection->IsMmiCode(0, dialStr));
    DialParaInfo dialInfo;
    sptr<CallBase> call = new IMSCall(dialInfo);
    call->callId_ = 1;
    call->SetCallIndex(0);
    call->SetSlotId(0);
    call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    call->SetCallType(CallType::TYPE_IMS);
    CallObjectManager::AddOneCallObject(call);
    dialStr = "333";
    ASSERT_FALSE(cellularCallConnection->IsMmiCode(0,dialStr));
    dialStr = "33";
    ASSERT_TRUE(cellularCallConnection->IsMmiCode(0, dialStr));
    CallObjectManager::DeleteOneCallObject(call);
    dialStr = "333";
    ASSERT_FALSE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "12";
    ASSERT_FALSE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "1*";
    ASSERT_TRUE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "*1";
    ASSERT_TRUE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "**";
    ASSERT_TRUE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "33";
    ASSERT_TRUE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "*21*10086#";
    ASSERT_TRUE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "10086";
    ASSERT_FALSE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "*30#10086";
    ASSERT_FALSE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "*33##123#";
    ASSERT_FALSE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "*10086#";
    ASSERT_TRUE(cellularCallConnection->IsMmiCode(0, dialStr));
    dialStr = "#10086#";
    ASSERT_TRUE(cellularCallConnection->IsMmiCode(0, dialStr));
}

/**
 * @tc.number   Telephony_EdmCallPolicy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_EdmCallPolicy_001, Function | MediumTest | Level1)
{
    std::vector<std::string> dialingList;
    std::vector<std::string> incomingList;
    std::string number = "11111111";
    bool isModifyParameter = false;
    std::shared_ptr<EdmCallPolicy> edmCallPolicy = std::make_shared<EdmCallPolicy>();
    if (system::GetBoolParameter("persist.edm.telephony_call_disable", false)) {
        system::SetParameter("persist.edm.telephony_call_disable", "false");
        isModifyParameter = true;
    }
    edmCallPolicy->SetCallPolicy(0, dialingList, 0, incomingList);
    EXPECT_EQ(edmCallPolicy->IsDialingEnable(number), true);
    dialingList.push_back(number);
    edmCallPolicy->SetCallPolicy(0, dialingList, 0, incomingList);
    EXPECT_EQ(edmCallPolicy->IsDialingEnable(number), false);
    edmCallPolicy->SetCallPolicy(1, dialingList, 0, incomingList);
    EXPECT_EQ(edmCallPolicy->IsDialingEnable(number), true);
    dialingList.insert(dialingList.end(), 1000, "22222222");
    EXPECT_EQ(edmCallPolicy->SetCallPolicy(0, dialingList, 0, incomingList), TELEPHONY_ERR_ARGUMENT_INVALID);
    dialingList.clear();
    edmCallPolicy->SetCallPolicy(1, dialingList, 0, incomingList);
    EXPECT_EQ(edmCallPolicy->IsDialingEnable(number), true);
    if (isModifyParameter) {
        system::SetParameter("persist.edm.telephony_call_disable", "true");
    }
}

/**
 * @tc.number   Telephony_EdmCallPolicy_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch2Test, Telephony_EdmCallPolicy_002, Function | MediumTest | Level1)
{
    std::vector<std::string> dialingList;
    std::vector<std::string> incomingList;
    std::string number = "11111111";
    bool isModifyParameter = false;
    std::shared_ptr<EdmCallPolicy> edmCallPolicy = std::make_shared<EdmCallPolicy>();
    if (system::GetBoolParameter("persist.edm.telephony_call_disable", false)) {
        system::SetParameter("persist.edm.telephony_call_disable", "false");
        isModifyParameter = true;
    }
    edmCallPolicy->SetCallPolicy(0, dialingList, 0, incomingList);
    EXPECT_EQ(edmCallPolicy->IsIncomingEnable(number), true);
    incomingList.push_back(number);
    edmCallPolicy->SetCallPolicy(0, dialingList, 0, incomingList);
    EXPECT_EQ(edmCallPolicy->IsIncomingEnable(number), false);
    edmCallPolicy->SetCallPolicy(0, dialingList, 1, incomingList);
    EXPECT_EQ(edmCallPolicy->IsIncomingEnable(number), true);
    incomingList.insert(incomingList.end(), 1000, "22222222");
    EXPECT_EQ(edmCallPolicy->SetCallPolicy(0, dialingList, 0, incomingList), TELEPHONY_ERR_ARGUMENT_INVALID);
    incomingList.clear();
    edmCallPolicy->SetCallPolicy(0, dialingList, 1, incomingList);
    EXPECT_EQ(edmCallPolicy->IsIncomingEnable(number), true);

    if (isModifyParameter) {
        system::SetParameter("persist.edm.telephony_call_disable", "true");
    }
}
} // namespace Telephony
} // namespace OHOS
