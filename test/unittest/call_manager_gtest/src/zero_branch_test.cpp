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

class BranchTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void BranchTest::SetUp() {}

void BranchTest::TearDown() {}

void BranchTest::SetUpTestCase()
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
        .processName = "BranchTest",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void BranchTest::TearDownTestCase() {}

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
HWTEST_F(BranchTest, Telephony_CallRequestHandler_001, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_CallRequestProcess_001, Function | MediumTest | Level1)
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
    callRequestProcess->IsDsdsMode5();
}

/**
 * @tc.number   Telephony_CallRequestProcess_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallRequestProcess_002, Function | MediumTest | Level1)
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
    callRequestProcess->HandleCallWaitingNumZero(incomingCall, call, SIM1_SLOTID, 1, flagForConference);
    callRequestProcess->HandleCallWaitingNumZero(incomingCall, voipCall, SIM1_SLOTID, 2, flagForConference);
    callRequestProcess->HandleCallWaitingNumZero(incomingCall, dialCall, SIM1_SLOTID, 2, flagForConference);
    callRequestProcess->DisconnectOtherCallForVideoCall(VALID_CALLID);
}

/**
 * @tc.number   Telephony_CallRequestProcess_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallRequestProcess_003, Function | MediumTest | Level1)
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
}

/**
 * @tc.number   Telephony_CallObjectManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallObjectManager_001, Function | MediumTest | Level1)
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
    CallObjectManager::IsCallExist(number);
    CallObjectManager::HasVideoCall();
    csCall->SetVideoStateType(VideoStateType::TYPE_VIDEO);
    CallObjectManager::HasVideoCall();
    csCall->SetCallType(CallType::TYPE_VOIP);
    CallObjectManager::HasVideoCall();
    CallObjectManager::GetCallInfoList(DEFAULT_INDEX);
    csCall->SetCallType(CallType::TYPE_OTT);
    CallObjectManager::GetCallInfoList(SIM1_SLOTID);
    CallObjectManager::GetCallInfoList(DEFAULT_INDEX);
}

/**
 * @tc.number   Telephony_CallNumberUtils_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallNumberUtils_001, Function | MediumTest | Level1)
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
    EXPECT_EQ(DelayedSingleton<CallNumberUtils>::GetInstance()->QueryYellowPageAndMarkInfo(
        numberMarkInfo, phoneNumber), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_CallNumberUtils_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallNumberUtils_002, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_CellularCallConnection_001, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_CellularCallConnection_002, Function | MediumTest | Level1)
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
    ASSERT_NE(cellularCallConnection->ClearAllCalls(), TELEPHONY_ERR_SUCCESS);
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
}

/**
 * @tc.number   Telephony_CellularCallConnection_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CellularCallConnection_003, Function | MediumTest | Level1)
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
 * @tc.number   Telephony_CallPolicy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallPolicy_001, Function | MediumTest | Level1)
{
    CallPolicy mCallPolicy;
    std::u16string testEmptyStr = u"";
    AppExecFwk::PacMap mPacMap;
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_CARRIER_TYPE));
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_VOICE_MAIL_TYPE));
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("callType", static_cast<int32_t>(CallType::TYPE_CS));
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialScene", static_cast<int32_t>(DialScene::CALL_NORMAL));
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, false), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialScene", static_cast<int32_t>(DialScene::CALL_PRIVILEGED));
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, false), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialScene", static_cast<int32_t>(DialScene::CALL_EMERGENCY));
    mPacMap.PutIntValue("videoState", static_cast<int32_t>(VideoStateType::TYPE_VOICE));
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, false), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialType", static_cast<int32_t>(DialType::DIAL_VOICE_MAIL_TYPE));
    ASSERT_NE(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
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
HWTEST_F(BranchTest, Telephony_CallPolicy_002, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_CallPolicy_003, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_CallPolicy_004, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_CallPolicy_005, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_CallPolicy_006, Function | MediumTest | Level1)
{
    CallPolicy callPolicy;
    bool isEcc = false;
    int32_t slotId = 0;
    callPolicy.HasNormalCall(isEcc, slotId, CallType::TYPE_IMS);
    bool isAirplaneModeOn = false;
    callPolicy.GetAirplaneMode(isAirplaneModeOn);
    ASSERT_EQ(isAirplaneModeOn, false);
}

/**
 * @tc.number   Telephony_ReportCallInfoHandler_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_ReportCallInfoHandler_001, Function | MediumTest | Level1)
{
    CallDetailInfo mCallDetailInfo;
    ASSERT_NE(DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallReportInfo(mCallDetailInfo),
        TELEPHONY_ERR_SUCCESS);
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
HWTEST_F(BranchTest, Telephony_VideoControlManager_001, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_VideoControlManager_002, Function | MediumTest | Level1)
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
    ASSERT_FALSE(DelayedSingleton<VideoControlManager>::GetInstance()->IsPngFile(""));
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
 * @tc.number   Telephony_VideoCallState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_VideoCallState_001, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_VideoCallState_002, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_VideoCallState_003, Function | MediumTest | Level1)
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
    ASSERT_NE(
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
HWTEST_F(BranchTest, Telephony_VideoCallState_004, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_VideoCallState_005, Function | MediumTest | Level1)
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
HWTEST_F(BranchTest, Telephony_IncomingCallNotification_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_RejectCallSms_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_MissedCallNotification_001, Function | MediumTest | Level3)
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
    missedCallNotification->PublishMissedCallNotification(callObjectPtr);
    int32_t id = 1;
    ASSERT_EQ(missedCallNotification->CancelMissedCallsNotification(id), TELEPHONY_SUCCESS);
    std::map<std::string, int32_t> phoneNumAndUnReadCountMap;
    phoneNumAndUnReadCountMap.insert(pair<string, int>("000", 1));
    ASSERT_EQ(missedCallNotification->NotifyUnReadMissedCall(phoneNumAndUnReadCountMap), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallSettingManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallSettingManager_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_CallAbilityCallback_001, Function | MediumTest | Level3)
{
    std::shared_ptr<CallAbilityCallback> callAbilityCallback = std::make_shared<CallAbilityCallback>();
    ASSERT_EQ(callAbilityCallback->SetProcessCallback(nullptr), TELEPHONY_SUCCESS);
    CallAttributeInfo callAttributeInfo;
    ASSERT_EQ(callAbilityCallback->OnCallDetailsChange(callAttributeInfo), TELEPHONY_SUCCESS);
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
 * @tc.number   Telephony_BluetoothCallClient_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_BluetoothCallClient_001, Function | MediumTest | Level3)
{
    std::shared_ptr<BluetoothCallClient> bluetoothCallClient = std::make_shared<BluetoothCallClient>();
    bluetoothCallClient->UnInit();
    ASSERT_NE(bluetoothCallClient->RegisterCallBack(nullptr), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->UnRegisterCallBack(), TELEPHONY_SUCCESS);
    std::u16string value = u"";
    AppExecFwk::PacMap extras;
    bool enabled;
    bluetoothCallClient->IsNewCallAllowed(enabled);
    bluetoothCallClient->IsInEmergencyCall(enabled);
    bluetoothCallClient->SetMuted(false);
    bluetoothCallClient->MuteRinger();
    bluetoothCallClient->SetAudioDevice(AudioDeviceType::DEVICE_BLUETOOTH_SCO, "test");
    bluetoothCallClient->GetCurrentCallList(-1).size();
    ASSERT_NE(bluetoothCallClient->DialCall(value, extras), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->AnswerCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->RejectCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->HangUpCall(), TELEPHONY_SUCCESS);
    ASSERT_GE(bluetoothCallClient->GetCallState(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->HoldCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->UnHoldCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->SwitchCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->CombineConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->SeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->KickOutFromConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->StartDtmf('a'), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->StopDtmf(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient->IsRinging(enabled), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerClient_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallManagerClient_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_CallManagerClient_002, Function | MediumTest | Level3)
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
}

/**
 * @tc.number   Telephony_CallManagerHisysevent_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallManagerHisysevent_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_CallManagerHisysevent_002, Function | MediumTest | Level3)
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
}

/**
 * @tc.number   Telephony_OTTCall_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_OTTCall_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_OTTCall_002, Function | MediumTest | Level3)
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

/**
 * @tc.number   Telephony_Ott_Conference_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_Ott_Conference_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_Cs_Conference_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_Ims_Conference_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_BluetoothCallService_001, Function | MediumTest | Level3)
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
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.StartDtmf('c'));
    bluetoothCallService.callObjectPtrList_.clear();
    callBase1->callState_ = TelCallState::CALL_STATUS_ACTIVE;
    callBase1->callId_ = -1;
    bluetoothCallService.callObjectPtrList_.push_back(callBase1);
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, bluetoothCallService.StartDtmf('c'));
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, bluetoothCallService.StopDtmf());
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.CombineConference());
    bluetoothCallService.callObjectPtrList_.clear();
    callBase1->callState_ = TelCallState::CALL_STATUS_HOLDING;
    callBase1->callId_ = -1;
    bluetoothCallService.callObjectPtrList_.push_back(callBase1);
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.CombineConference());
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.SeparateConference());
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, bluetoothCallService.KickOutFromConference());
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
HWTEST_F(BranchTest, Telephony_BluetoothCallPolicy_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_BluetoothCallPolicy_002, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_ImsCall_002, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_CSCall_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_CallRecordsManager_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_CallControlManager_001, Function | MediumTest | Level3)
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
    ASSERT_NE(callControlManager->RemoveMissedIncomingCallNotification(), TELEPHONY_SUCCESS);
    callControlManager->ConnectCallUiService(true);
}

/**
 * @tc.number   Telephony_CallControlManager_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallControlManager_002, Function | MediumTest | Level3)
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
    bool enaled = false;
    ASSERT_NE(callControlManager->IsImsSwitchEnabled(INVALID_CALLID, enaled), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->IsImsSwitchEnabled(slotId, enaled), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallControlManager_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallControlManager_003, Function | MediumTest | Level3)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
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
HWTEST_F(BranchTest, Telephony_CallControlManager_004, Function | MediumTest | Level3)
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
    CallControlManager::SystemAbilityListener listen(subscriberPtr);
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
}

/**
 * @tc.number   Telephony_CallStatusManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallStatusManager_001, Function | MediumTest | Level3)
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
HWTEST_F(BranchTest, Telephony_CallStatusManager_002, Function | MediumTest | Level3)
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
    ContactInfo contactInfo;
    std::string phoneNum;
    callStatusManager->QueryCallerInfo(contactInfo, phoneNum);
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

/**
 * @tc.number   Telephony_CallStatusManager_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallStatusManager_003, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    callStatusManager->Init();
    CellularCallEventInfo cellularCallEventInfo;
    ASSERT_EQ(callStatusManager->HandleEventResultReportInfo(cellularCallEventInfo), TELEPHONY_SUCCESS);
    cellularCallEventInfo.eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;
    cellularCallEventInfo.eventId = RequestResultEventId::RESULT_DIAL_NO_CARRIER;
    ASSERT_EQ(callStatusManager->HandleEventResultReportInfo(cellularCallEventInfo), TELEPHONY_SUCCESS);
    OttCallEventInfo ottCallEventInfo;
    (void)memset_s(&ottCallEventInfo, sizeof(OttCallEventInfo), 0, sizeof(OttCallEventInfo));
    ottCallEventInfo.ottCallEventId = OttCallEventId::OTT_CALL_EVENT_FUNCTION_UNSUPPORTED;
    (void)memcpy_s(ottCallEventInfo.bundleName, kMaxBundleNameLen + 1, LONG_STR, strlen(LONG_STR));
    ASSERT_EQ(callStatusManager->HandleOttEventReportInfo(ottCallEventInfo), TELEPHONY_SUCCESS);
    (void)memset_s(&ottCallEventInfo, sizeof(OttCallEventInfo), 0, sizeof(OttCallEventInfo));
    ottCallEventInfo.ottCallEventId = OttCallEventId::OTT_CALL_EVENT_FUNCTION_UNSUPPORTED;
    (void)memcpy_s(ottCallEventInfo.bundleName, kMaxBundleNameLen + 1, TEST_STR, strlen(TEST_STR));
    ASSERT_EQ(callStatusManager->HandleOttEventReportInfo(ottCallEventInfo), TELEPHONY_SUCCESS);
    CallDetailInfo callDetailInfo;
    std::string number = "";
    memcpy_s(&callDetailInfo.phoneNum, kMaxNumberLen, number.c_str(), number.length());
    callDetailInfo.state = TelCallState::CALL_STATUS_INCOMING;
    callDetailInfo.callType = CallType::TYPE_CS;
    ASSERT_EQ(callStatusManager->IncomingFilterPolicy(callDetailInfo), TELEPHONY_SUCCESS);
    callStatusManager->CallFilterCompleteResult(callDetailInfo);
    CallDirection dir = CallDirection::CALL_DIRECTION_OUT;
    ASSERT_FALSE(callStatusManager->CreateNewCall(callDetailInfo, dir) == nullptr);
    dir = CallDirection::CALL_DIRECTION_IN;
    ASSERT_FALSE(callStatusManager->CreateNewCall(callDetailInfo, dir) == nullptr);
    callDetailInfo.callType = CallType::TYPE_IMS;
    dir = CallDirection::CALL_DIRECTION_OUT;
    ASSERT_FALSE(callStatusManager->CreateNewCall(callDetailInfo, dir) == nullptr);
    dir = CallDirection::CALL_DIRECTION_IN;
    ASSERT_FALSE(callStatusManager->CreateNewCall(callDetailInfo, dir) == nullptr);
    callDetailInfo.callType = CallType::TYPE_OTT;
    dir = CallDirection::CALL_DIRECTION_OUT;
    ASSERT_FALSE(callStatusManager->CreateNewCall(callDetailInfo, dir) == nullptr);
    dir = CallDirection::CALL_DIRECTION_IN;
    ASSERT_FALSE(callStatusManager->CreateNewCall(callDetailInfo, dir) == nullptr);
    callDetailInfo.callType = CallType::TYPE_ERR_CALL;
    ASSERT_TRUE(callStatusManager->CreateNewCall(callDetailInfo, dir) == nullptr);
}

/**
 * @tc.number   Telephony_IncomingCallWakeup_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_IncomingCallWakeup_001, Function | MediumTest | Level3)
{
    IncomingCallWakeup incomingCallWakeup;
    sptr<CallBase> callObjectPtr = nullptr;
    incomingCallWakeup.NewCallCreated(callObjectPtr);
}

/**
 * @tc.number   Telephony_CallStateReportProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallStateReportProxy_001, Function | MediumTest | Level3)
{
    CallStateReportProxy callStateReportPtr;
    sptr<CallBase> callObjectPtr = nullptr;
    callStateReportPtr.CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_CS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_INCOMING;
    callObjectPtr = new CSCall(dialParaInfo);
    ASSERT_TRUE(callObjectPtr != nullptr);
    callStateReportPtr.CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    callObjectPtr->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    callStateReportPtr.CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    callStateReportPtr.UpdateCallState(callObjectPtr, TelCallState::CALL_STATUS_INCOMING);
    callStateReportPtr.UpdateCallStateForSlotId(callObjectPtr, TelCallState::CALL_STATUS_INCOMING);
    std::string number = "123456789012";
    std::u16string phoneNumber = Str8ToStr16(number);
    callStateReportPtr.ReportCallState(0, phoneNumber);
    callStateReportPtr.ReportCallStateForCallId(0, 0, phoneNumber);
}

/**
 * @tc.number   Telephony_CallStateListener_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallStateListener_001, Function | MediumTest | Level3)
{
    CallStateListener callStateListener;
    sptr<CallBase> callObjectPtr = nullptr;
    callStateListener.NewCallCreated(callObjectPtr);
    callStateListener.CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    callStateListener.IncomingCallHungUp(callObjectPtr, true, "");
    callStateListener.IncomingCallActivated(callObjectPtr);
    DialParaInfo dialParaInfo;
    callObjectPtr = new CSCall(dialParaInfo);
    callStateListener.NewCallCreated(callObjectPtr);
    callStateListener.CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    callStateListener.IncomingCallHungUp(callObjectPtr, true, "");
    callStateListener.IncomingCallActivated(callObjectPtr);
    std::shared_ptr<CallStateReportProxy> callStateReportPtr = nullptr;
    ASSERT_FALSE(callStateListener.AddOneObserver(callStateReportPtr));
    ASSERT_FALSE(callStateListener.RemoveOneObserver(callStateReportPtr));
    callStateReportPtr = std::make_shared<CallStateReportProxy>();
    ASSERT_TRUE(callStateListener.AddOneObserver(callStateReportPtr));
    ASSERT_TRUE(callStateListener.AddOneObserver(callStateReportPtr));
}

/**
 * @tc.number   Telephony_ConferenceBase_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_ConferenceBase_001, Function | MediumTest | Level3)
{
    std::shared_ptr<ConferenceBase> conference = std::make_shared<CsConference>();
    std::vector<std::u16string> callIdList;
    ASSERT_NE(conference->GetSubCallIdList(VALID_CALLID, callIdList), TELEPHONY_SUCCESS);
    ASSERT_NE(conference->GetCallIdListForConference(VALID_CALLID, callIdList), TELEPHONY_SUCCESS);
    ASSERT_NE(conference->JoinToConference(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(conference->LeaveFromConference(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(conference->HoldConference(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(conference->CanSeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(conference->CanKickOutFromConference(), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->SetMainCall(ERROR_CALLID), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->SetMainCall(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(conference->CanSeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(conference->CanKickOutFromConference(), TELEPHONY_SUCCESS);
    conference->SetConferenceState(ConferenceState::CONFERENCE_STATE_CREATING);
    ASSERT_EQ(conference->JoinToConference(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->GetSubCallIdList(VALID_CALLID, callIdList), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->GetCallIdListForConference(VALID_CALLID, callIdList), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->CanSeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->CanKickOutFromConference(), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->HoldConference(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->SetMainCall(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->JoinToConference(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->LeaveFromConference(VALID_CALLID), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallAbilityReportProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallAbilityReportProxy_001, Function | MediumTest | Level3)
{
    CallAbilityConnectCallback callAbilityConnectCallback;
    callAbilityConnectCallback.ReConnectAbility();
    if (CallObjectManager::HasCallExist()) {
        callAbilityConnectCallback.ReConnectAbility();
    }
    CallConnectAbility callConnectCalluiAbility;
    ASSERT_FALSE(callConnectCalluiAbility.WaitForConnectResult());
    std::shared_ptr<CallAbilityReportProxy> callAbilityReportProxy = std::make_shared<CallAbilityReportProxy>();
    sptr<ICallAbilityCallback> callAbilityCallbackPtr = nullptr;
    std::string pidName = "123";
    ASSERT_NE(callAbilityReportProxy->RegisterCallBack(callAbilityCallbackPtr, pidName), TELEPHONY_SUCCESS);
    ASSERT_NE(callAbilityReportProxy->UnRegisterCallBack(pidName), TELEPHONY_SUCCESS);
    sptr<CallBase> callObjectPtr = nullptr;
    callAbilityReportProxy->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    callAbilityCallbackPtr = new CallAbilityCallback();
    ASSERT_EQ(callAbilityReportProxy->RegisterCallBack(callAbilityCallbackPtr, pidName), TELEPHONY_SUCCESS);
    DisconnectedDetails disconnectedDetails;
    callAbilityReportProxy->CallDestroyed(disconnectedDetails);
    CallAttributeInfo callAttributeInfo;
    callAbilityReportProxy->ReportCallStateInfo(callAttributeInfo);
    CallEventInfo callEventInfo;
    callAbilityReportProxy->ReportCallEvent(callEventInfo);
    CallResultReportId reportId = CallResultReportId::GET_CALL_CLIP_ID;
    AppExecFwk::PacMap resultInfo;
    callAbilityReportProxy->ReportAsyncResults(reportId, resultInfo);
    MmiCodeInfo mmiCodeInfo;
    callAbilityReportProxy->ReportMmiCodeResult(mmiCodeInfo);
    OttCallRequestId ottReportId = OttCallRequestId::OTT_REQUEST_ANSWER;
    callAbilityReportProxy->OttCallRequest(ottReportId, resultInfo);
    callAbilityReportProxy->ReportPostDialDelay(TEST_STR);
    std::string ottBundleName = "com.ohos.callservice";
    sptr<CallAbilityCallback> ottCallAbilityCallbackPtr = new CallAbilityCallback();
    ASSERT_EQ(callAbilityReportProxy->RegisterCallBack(ottCallAbilityCallbackPtr, ottBundleName), TELEPHONY_SUCCESS);
    callAbilityReportProxy->OttCallRequest(ottReportId, resultInfo);
    ASSERT_EQ(callAbilityReportProxy->UnRegisterCallBack(ottBundleName), TELEPHONY_SUCCESS);
    CallMediaModeInfo imsCallModeInfo;
    callAbilityReportProxy->ReportImsCallModeChange(imsCallModeInfo);
    CallSessionEvent callSessionEventOptions;
    callAbilityReportProxy->ReportCallSessionEventChange(callSessionEventOptions);
    PeerDimensionsDetail peerDimensionsDetail;
    callAbilityReportProxy->ReportPeerDimensionsChange(peerDimensionsDetail);
    int64_t dataUsage = 0;
    callAbilityReportProxy->ReportCallDataUsageChange(dataUsage);
    CameraCapabilities cameraCapabilities;
    callAbilityReportProxy->ReportCameraCapabilities(cameraCapabilities);
    ASSERT_EQ(callAbilityReportProxy->UnRegisterCallBack(pidName), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_AutoAnswerState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_AutoAnswerState_001, Function | MediumTest | Level3)
{
    bool flag = true;
    sptr<CallBase> callObjectPtr = nullptr;
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_IMS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_INCOMING;
    callObjectPtr = new IMSCall(dialParaInfo);
    ASSERT_TRUE(callObjectPtr != nullptr);
    callObjectPtr->SetAutoAnswerState(flag);
    ASSERT_EQ(callObjectPtr->GetAutoAnswerState(), true);
}

/**
 * @tc.number   Telephony_CanUnHoldState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CanUnHoldState_001, Function | MediumTest | Level3)
{
    bool flag = true;
    sptr<CallBase> callObjectPtr = nullptr;
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_IMS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_INCOMING;
    callObjectPtr = new IMSCall(dialParaInfo);
    ASSERT_TRUE(callObjectPtr != nullptr);
    callObjectPtr->SetCanUnHoldState(flag);
    ASSERT_EQ(callObjectPtr->GetCanUnHoldState(), true);
}

/**
 * @tc.number   Telephony_SatelliteCall_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_SatelliteCall_001, Function | MediumTest | Level3)
{
    DialParaInfo dialParaInfo;
    SatelliteCall call { dialParaInfo };
    call.DialingProcess();
    call.AnswerCall(0);
    call.RejectCall();
    call.HangUpCall();
    CallAttributeInfo callAttributeInfo;
    call.GetCallAttributeInfo(callAttributeInfo);
}

/**
 * @tc.number   Telephony_VoipCallManagerProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_VoipCallManagerProxy_001, Function | MediumTest | Level3)
{
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IVoipCallManagerService> voipCallManagerInterfacePtr = nullptr;
    if (managerPtr != nullptr) {
        sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
        if (iRemoteObjectPtr != nullptr) {
            voipCallManagerInterfacePtr = iface_cast<IVoipCallManagerService>(iRemoteObjectPtr);
        }
    }
    AppExecFwk::PacMap mPacMap;
    ErrorReason error = ErrorReason::VOIP_CALL_EXISTS;
    if (voipCallManagerInterfacePtr != nullptr) {
        std::vector<uint8_t> userProfile = { 0 };
        voipCallManagerInterfacePtr->ReportIncomingCall(mPacMap, userProfile, error);
        voipCallManagerInterfacePtr->ReportIncomingCallError(mPacMap);
        VoipCallState voipCallState = VoipCallState::VOIP_CALL_STATE_ACTIVE;
        std::string callId = "123";
        voipCallManagerInterfacePtr->ReportCallStateChange(callId, voipCallState);
        voipCallManagerInterfacePtr->UnRegisterCallBack();
        std::string bundleName = " ";
        std::string processMode = "0";
        voipCallManagerInterfacePtr->ReportVoipIncomingCall(callId, bundleName, processMode);
        std::string extensionId = " ";
        voipCallManagerInterfacePtr->ReportVoipCallExtensionId(callId, bundleName, extensionId);
        VoipCallEventInfo voipCallEventInfo;
        voipCallEventInfo.voipCallId = "123";
        voipCallManagerInterfacePtr->Answer(voipCallEventInfo, static_cast<int32_t>(VideoStateType::TYPE_VOICE));
        voipCallManagerInterfacePtr->HangUp(voipCallEventInfo);
        voipCallManagerInterfacePtr->Reject(voipCallEventInfo);
        voipCallManagerInterfacePtr->UnloadVoipSa();
        sptr<ICallStatusCallback> statusCallback = (std::make_unique<CallStatusCallback>()).release();
        voipCallManagerInterfacePtr->RegisterCallManagerCallBack(statusCallback);
        voipCallManagerInterfacePtr->UnRegisterCallManagerCallBack();
    }
}

/**
 * @tc.number   Telephony_CallRequestEventHandlerHelper_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallRequestEventHandlerHelper_001, Function | MediumTest | Level3)
{
    bool flag = false;
    DelayedSingleton<CallRequestEventHandlerHelper>::GetInstance()->RestoreDialingFlag(flag);
    ASSERT_NE(DelayedSingleton<CallRequestEventHandlerHelper>::GetInstance()->IsDialingCallProcessing(),
        true);
}

/**
 * @tc.number   Telephony_DistributedCallManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_DistributedCallManager_001, Function | MediumTest | Level3)
{
    DistributedCallManager manager;
    AudioDevice device;
    device.deviceType = AudioDeviceType::DEVICE_EARPIECE;
    manager.GetDevIdFromAudioDevice(device);
    manager.IsSelectVirtualModem();
    manager.SwitchOnDCallDeviceSync(device);
    device.deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    std::string devId = "";
    manager.CreateDAudioDevice(devId, device);
    manager.CreateDAudioDevice(TEST_STR, device);
    manager.dcallProxy_ = std::make_shared<DistributedCallProxy>();
    manager.CreateDAudioDevice(TEST_STR, device);
    manager.AddDCallDevice(TEST_STR);
    manager.RemoveDCallDevice(TEST_STR);
    manager.onlineDCallDevices_.insert(std::make_pair(TEST_STR, device));
    manager.AddDCallDevice(TEST_STR);
    manager.RemoveDCallDevice(TEST_STR);
    manager.GetConnectedDCallDeviceId();
    manager.SwitchOffDCallDeviceSync();
    manager.dCallDeviceSwitchedOn_.store(true);
    manager.SwitchOffDCallDeviceSync();
    manager.GetConnectedDCallDeviceId();
    manager.GetConnectedDCallDevice(device);
    manager.SetConnectedDCallDevice(device);
    manager.ClearConnectedDCallDevice();
    manager.IsSelectVirtualModem();
    manager.SwitchOnDCallDeviceSync(device);
    std::unique_ptr<AudioDevice> devicePtr = std::make_unique<AudioDevice>();
    devicePtr->deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    manager.IsSelectVirtualModem();
    manager.SwitchOnDCallDevice(std::move(devicePtr));
    std::unique_ptr<AudioDevice> deviceRarpiecePtr = std::make_unique<AudioDevice>();
    deviceRarpiecePtr->deviceType = AudioDeviceType::DEVICE_EARPIECE;
    manager.SwitchOnDCallDevice(std::move(deviceRarpiecePtr));
    manager.OnDCallSystemAbilityAdded(TEST_STR);
}

/**
 * @tc.number   Telephony_DistributedCallProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_DistributedCallProxy_001, Function | MediumTest | Level3)
{
    std::unique_ptr<DistributedCallProxy> proxy = std::make_unique<DistributedCallProxy>();
    proxy->GetDCallClient();
    proxy->Init();
    proxy->GetDCallClient();
    proxy->dcallClient_ = nullptr;
    proxy->UnInit();
    proxy->IsSelectVirtualModem();
    proxy->SwitchDevice(TEST_STR, 1);
    std::vector<std::string> devList;
    proxy->GetOnlineDeviceList(devList);
    OHOS::DistributedHardware::DCallDeviceInfo devInfo;
    proxy->GetDCallDeviceInfo(TEST_STR, devInfo);
    proxy->GetDCallClient();
    proxy->IsSelectVirtualModem();
    proxy->SwitchDevice(TEST_STR, 1);
    proxy->GetOnlineDeviceList(devList);
    proxy->GetDCallDeviceInfo(TEST_STR, devInfo);
    proxy->UnInit();
}

/**
 * @tc.number   Telephony_BluetoothCallManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_BluetoothCallManager_001, Function | MediumTest | Level3)
{
    int32_t state = 0;
    int32_t numActive = CallObjectManager::GetCallNum(TelCallState::CALL_STATUS_ACTIVE);
    int32_t numHeld = CallObjectManager::GetCallNum(TelCallState::CALL_STATUS_HOLDING);
    int32_t numDial = CallObjectManager::GetCallNum(TelCallState::CALL_STATUS_DIALING);
    int32_t callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
    std::string number = CallObjectManager::GetCallNumber(TelCallState::CALL_STATUS_HOLDING);
    BluetoothCallManager bluetoothCallManager;
    bluetoothCallManager.SendBtCallState(numActive, numHeld, callState, number);
    bluetoothCallManager.SendCallDetailsChange(1, 1);
    bluetoothCallManager.GetBtScoState();
    bluetoothCallManager.IsBtAvailble();
    bluetoothCallManager.GetConnectedScoAddr();
    bluetoothCallManager.GetConnectedScoName();
    bluetoothCallManager.IsBtScoConnected();
}

/**
 * @tc.number   Telephony_CallAbilityCallbackDeathRecipient_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallAbilityCallbackDeathRecipient_001, Function | MediumTest | Level3)
{
    CallAbilityCallbackDeathRecipient recipient;
    OHOS::wptr<OHOS::IRemoteObject> object;
    recipient.OnRemoteDied(object);
}

/**
 * @tc.number   Telephony_ApplicationStateObserver_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_ApplicationStateObserver_001, Function | MediumTest | Level3)
{
    ApplicationStateObserver applicationStateObserver;
    AppExecFwk::ProcessData processData;
    applicationStateObserver.OnProcessDied(processData);
}

/**
 * @tc.number   Telephony_CallAbilityCallbackProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallAbilityCallbackProxy, Function | MediumTest | Level3)
{
    sptr<IRemoteObject> impl;
    CallAbilityCallbackProxy callAbilityCallbackProxy(impl);
    CallAttributeInfo callAttributeInfo;
    callAbilityCallbackProxy.OnCallDetailsChange(callAttributeInfo);
    CallEventInfo info;
    callAbilityCallbackProxy.OnCallEventChange(info);
    DisconnectedDetails details;
    callAbilityCallbackProxy.OnCallDisconnectedCause(details);
    AppExecFwk::PacMap resultInfo;
    
    callAbilityCallbackProxy.OnReportAsyncResults(CallResultReportId::GET_CALL_CLIP_ID, resultInfo);
    MmiCodeInfo mmiCodeInfo;
    callAbilityCallbackProxy.OnReportMmiCodeResult(mmiCodeInfo);
    callAbilityCallbackProxy.OnOttCallRequest(OttCallRequestId::OTT_REQUEST_ANSWER, resultInfo);
    CallMediaModeInfo imsCallModeInfo;
    callAbilityCallbackProxy.OnReportImsCallModeChange(imsCallModeInfo);
    CallSessionEvent callSessionEventOptions;
    callAbilityCallbackProxy.OnReportCallSessionEventChange(callSessionEventOptions);
    PeerDimensionsDetail peerDimensionsDetail;
    callAbilityCallbackProxy.OnReportPeerDimensionsChange(peerDimensionsDetail);
    int64_t dataUsage = 0;
    callAbilityCallbackProxy.OnReportCallDataUsageChange(dataUsage);
    CameraCapabilities cameraCapabilities;
    callAbilityCallbackProxy.OnReportCameraCapabilities(cameraCapabilities);
}

/**
 * @tc.number   Telephony_CallBroadcastSubscriber_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallBroadcastSubscriber_001, Function | MediumTest | Level3)
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SIM_STATE_CHANGED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    CallBroadcastSubscriber subscriber(subscriberInfo);
    EventFwk::CommonEventData eventData;
    subscriber.OnReceiveEvent(eventData);
    subscriber.UnknownBroadcast(eventData);
    subscriber.SimStateBroadcast(eventData);
    subscriber.ConnectCallUiServiceBroadcast(eventData);
    subscriber.HighTempLevelChangedBroadcast(eventData);
    subscriber.ConnectCallUiSuperPrivacyModeBroadcast(eventData);
}

/**
 * @tc.number   Telephony_OTTCallConnection_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_OTTCallConnection_001, Function | MediumTest | Level3)
{
    OTTCallConnection ott;
    OttCallRequestInfo requestInfo;
    ott.Answer(requestInfo);
    ott.HoldCall(requestInfo);
    ott.HangUp(requestInfo);
    ott.Reject(requestInfo);
    ott.UnHoldCall(requestInfo);
    ott.SwitchCall(requestInfo);
    ott.CombineConference(requestInfo);
    ott.SeparateConference(requestInfo);
    ott.KickOutFromConference(requestInfo);
    std::vector<std::string> numberList;
    ott.InviteToConference(requestInfo, numberList);
    ott.UpdateImsCallMode(requestInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY);
    AppExecFwk::PacMap info;
    ott.PackCellularCallInfo(requestInfo, info);
}

/**
 * @tc.number   Telephony_SuperPrivacyManagerClient_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_SuperPrivacyManagerClient_001, Function | MediumTest | Level3)
{
    int32_t privacy = SuperPrivacyManagerClient::GetInstance().
    SetSuperPrivacyMode(static_cast<int32_t>(CallSuperPrivacyModeType::OFF), SOURCE_CALL);
}

/**
 * @tc.number   Telephony_CallStatusCallback_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallStatusCallback_001, Function | MediumTest | Level3)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    CallReportInfo callReportInfo;
    callStatusCallback->UpdateCallReportInfo(callReportInfo);
    CallsReportInfo callsReportInfo;
    callStatusCallback->UpdateCallsReportInfo(callsReportInfo);
    DisconnectedDetails disconnectedDetails;
    callStatusCallback->UpdateDisconnectedCause(disconnectedDetails);
    CellularCallEventInfo cellularCallEventIndo;
    callStatusCallback->UpdateEventResultInfo(cellularCallEventIndo);
    RBTPlayInfo rbtpPlyaInfo = RBTPlayInfo::LOCAL_ALERTING;
    callStatusCallback->UpdateRBTPlayInfo(rbtpPlyaInfo);
    int32_t result = 0;
    callStatusCallback->StartDtmfResult(result);
    callStatusCallback->StopDtmfResult(result);
    callStatusCallback->SendUssdResult(result);
    callStatusCallback->GetImsCallDataResult(result);
    CallWaitResponse callWaitResponse;
    callStatusCallback->UpdateGetWaitingResult(callWaitResponse);
    callStatusCallback->UpdateSetWaitingResult(result);
    CallRestrictionResponse callRestrictionResponse;
    callStatusCallback->UpdateGetRestrictionResult(callRestrictionResponse);
    callStatusCallback->UpdateSetRestrictionResult(result);
    callStatusCallback->UpdateSetRestrictionPasswordResult(result);
    CallTransferResponse callTransferResponse;
    callStatusCallback->UpdateGetTransferResult(callTransferResponse);
    callStatusCallback->UpdateSetTransferResult(result);
    ClipResponse clipResponse;
    callStatusCallback->UpdateGetCallClipResult(clipResponse);
    ClirResponse clirResponse;
    callStatusCallback->UpdateGetCallClirResult(clirResponse);
    callStatusCallback->UpdateSetCallClirResult(result);
    callStatusCallback->StartRttResult(result);
    callStatusCallback->StopRttResult(result);
    GetImsConfigResponse getImsConfigResponse;
    callStatusCallback->GetImsConfigResult(getImsConfigResponse);
    callStatusCallback->SetImsConfigResult(result);
    GetImsFeatureValueResponse getImsFeatureValueResopnse;
    callStatusCallback->GetImsFeatureValueResult(getImsFeatureValueResopnse);
    callStatusCallback->SetImsFeatureValueResult(result);
    CallModeReportInfo callModeReportInfo;
    callStatusCallback->ReceiveUpdateCallMediaModeRequest(callModeReportInfo);
    callStatusCallback->ReceiveUpdateCallMediaModeResponse(callModeReportInfo);
    callStatusCallback->InviteToConferenceResult(result);
    MmiCodeInfo mmiCodeInfo;
    callStatusCallback->SendMmiCodeResult(mmiCodeInfo);
    callStatusCallback->CloseUnFinishedUssdResult(result);
}

/**
 * @tc.number   Telephony_CallStatusCallback_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallStatusCallback_002, Function | MediumTest | Level3)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    std::string c("a");
    callStatusCallback->ReportPostDialChar(c);
    std::string str("abc");
    callStatusCallback->ReportPostDialDelay(str);
    CallSessionReportInfo callSessionReportInfo;
    callStatusCallback->HandleCallSessionEventChanged(callSessionReportInfo);
    PeerDimensionsReportInfo peerDimensionReportInfo;
    callStatusCallback->HandlePeerDimensionsChanged(peerDimensionReportInfo);
    int64_t res = 0;
    callStatusCallback->HandleCallDataUsageChanged(res);
    CameraCapabilitiesReportInfo cameraCapabilities;
    callStatusCallback->HandleCameraCapabilitiesChanged(cameraCapabilities);
    VoipCallEventInfo voipCallEventInfo;
    callStatusCallback->UpdateVoipEventInfo(voipCallEventInfo);
}

/**
 * @tc.number   Telephony_SatelliteCallControl_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_SatelliteCallControl_001, Function | MediumTest | Level3)
{
    auto satelliteCallControl = DelayedSingleton<SatelliteCallControl>::GetInstance();
    int32_t level = static_cast<int32_t>(SatCommTempLevel::TEMP_LEVEL_HIGH);
    satelliteCallControl->SetSatcommTempLevel(level);
    satelliteCallControl->IsAllowedSatelliteDialCall();
    satelliteCallControl->IsSatelliteSwitchEnable();
    satelliteCallControl->GetSatcommTempLevel();
    sptr<CallBase> call = nullptr;
    TelCallState priorState = TelCallState::CALL_STATUS_DIALING;
    TelCallState nextState = TelCallState::CALL_STATUS_ALERTING;
    satelliteCallControl->HandleSatelliteCallStateUpdate(call, priorState, nextState);
    satelliteCallControl->IsShowDialog();
    bool isShowDialog = true;
    satelliteCallControl->SetShowDialog(isShowDialog);
}

/**
 * @tc.number   Telephony_ProximitySensor_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_ProximitySensor_001, Function | MediumTest | Level3)
{
    auto proximitySensor = DelayedSingleton<ProximitySensor>::GetInstance();
    sptr<CallBase> callObjectPtr = nullptr;
    proximitySensor->NewCallCreated(callObjectPtr);
    proximitySensor->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE,
        TelCallState::CALL_STATUS_INCOMING);
    proximitySensor->IncomingCallHungUp(callObjectPtr, false, "");
    proximitySensor->IncomingCallActivated(callObjectPtr);
    DisconnectedDetails details;
    proximitySensor->CallDestroyed(details);
}

/**
 * @tc.number   Telephony_StatusBar_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_StatusBar_001, Function | MediumTest | Level3)
{
    auto statusBar = DelayedSingleton<StatusBar>::GetInstance();
    sptr<CallBase> callObjectPtr = nullptr;
    statusBar->NewCallCreated(callObjectPtr);
    bool isDisplayMute = false;
    statusBar->UpdateMuteIcon(isDisplayMute);
    statusBar->UpdateSpeakerphoneIcon(isDisplayMute);
    statusBar->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
    statusBar->IncomingCallHungUp(callObjectPtr, false, "");
    statusBar->IncomingCallActivated(callObjectPtr);
    DisconnectedDetails details;
    statusBar->CallDestroyed(details);
}

/**
 * @tc.number   Telephony_WiredHeadset_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_WiredHeadset_001, Function | MediumTest | Level3)
{
    auto wiredHeadset = DelayedSingleton<WiredHeadset>::GetInstance();
    sptr<CallBase> callObjectPtr = nullptr;
    wiredHeadset->Init();
    wiredHeadset->NewCallCreated(callObjectPtr);
    wiredHeadset->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_IDLE, TelCallState::CALL_STATUS_INCOMING);
    wiredHeadset->IncomingCallHungUp(callObjectPtr, false, "");
    wiredHeadset->IncomingCallActivated(callObjectPtr);
    DisconnectedDetails details;
    wiredHeadset->CallDestroyed(details);
}

/**
 * @tc.number   Telephony_CallStatusPolicy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallStatusPolicy_001, Function | MediumTest | Level3)
{
    CallStatusPolicy callStatusPolicy;
    CallDetailInfo info;
    callStatusPolicy.DialingHandlePolicy(info);
    callStatusPolicy.FilterResultsDispose(nullptr);
}

} // namespace Telephony
} // namespace OHOS
