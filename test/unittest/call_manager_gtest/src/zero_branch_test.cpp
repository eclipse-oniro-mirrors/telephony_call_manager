/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "call_ability_report_proxy.h"
#include "call_control_manager.h"
#include "call_manager_client.h"
#include "call_manager_hisysevent.h"
#include "call_number_utils.h"
#include "call_policy.h"
#include "call_records_manager.h"
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
#include "gtest/gtest.h"
#include "ims_call.h"
#include "ims_conference.h"
#include "incoming_call_notification.h"
#include "missed_call_notification.h"
#include "ott_call.h"
#include "ott_conference.h"
#include "reject_call_sms.h"
#include "report_call_info_handler.h"
#include "telephony_errors.h"
#include "telephony_hisysevent.h"
#include "telephony_log_wrapper.h"
#include "video_call_state.h"
#include "video_control_manager.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
const int32_t INVALID_SLOTID = 2;
const int32_t SIM1_SLOTID = 0;
const int16_t CAMERA_ROTATION_0 = 0;
const int16_t CAMERA_ROTATION_90 = 90;
const int16_t CAMERA_ROTATION_180 = 180;
const int16_t CAMERA_ROTATION_270 = 270;
const int32_t INVALID_MODE = 0;
const int32_t VALID_CALLID = 1;
const int32_t ERROR_CALLID = -1;
const int32_t ONE_TIME = 1;
constexpr int16_t DEFAULT_TIME = 0;
constexpr const char *TEST_STR = "123";
constexpr const char *LONG_STR =
    "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
    "111111111";
} // namespace

class DemoHandler : public AppExecFwk::EventHandler {
public:
    explicit DemoHandler(std::shared_ptr<AppExecFwk::EventRunner> &runner) : AppExecFwk::EventHandler(runner) {}
    virtual ~DemoHandler() {}
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) {}
};

class BranchTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void BranchTest::SetUpTestCase() {}

void BranchTest::TearDownTestCase() {}

void BranchTest::SetUp() {}

void BranchTest::TearDown() {}

/**
 * @tc.number   Telephony_CallRequestHandler_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallRequestHandler_001, Function | MediumTest | Level1)
{
    auto runner = AppExecFwk::EventRunner::Create("test");
    auto call_request_handler = std::make_shared<CallRequestHandler>(runner);
    auto event = AppExecFwk::InnerEvent::Get(0);
    call_request_handler->DialCallEvent(event);
    call_request_handler->AcceptCallEvent(event);
    call_request_handler->RejectCallEvent(event);
    call_request_handler->HangUpCallEvent(event);
    call_request_handler->HoldCallEvent(event);
    call_request_handler->UnHoldCallEvent(event);
    call_request_handler->SwitchCallEvent(event);
    call_request_handler->CombineConferenceEvent(event);
    call_request_handler->SeparateConferenceEvent(event);
    call_request_handler->UpdateCallMediaModeEvent(event);
    call_request_handler->StartRttEvent(event);
    call_request_handler->StopRttEvent(event);
    call_request_handler->JoinConferenceEvent(event);
    event = nullptr;
    call_request_handler->ProcessEvent(event);
    call_request_handler->DialCallEvent(event);
    call_request_handler->AcceptCallEvent(event);
    call_request_handler->RejectCallEvent(event);
    call_request_handler->HangUpCallEvent(event);
    call_request_handler->HoldCallEvent(event);
    call_request_handler->UnHoldCallEvent(event);
    call_request_handler->SwitchCallEvent(event);
    call_request_handler->CombineConferenceEvent(event);
    call_request_handler->SeparateConferenceEvent(event);
    call_request_handler->UpdateCallMediaModeEvent(event);
    call_request_handler->StartRttEvent(event);
    call_request_handler->StopRttEvent(event);
    call_request_handler->JoinConferenceEvent(event);
    call_request_handler->DialCall();
    std::unique_ptr<CallRequestHandlerService> callRequestHandlerService =
        std::make_unique<CallRequestHandlerService>();
    ASSERT_NE(callRequestHandlerService->DialCall(), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandlerService->AnswerCall(1, 1), TELEPHONY_ERR_SUCCESS);
    std::string content = "";
    ASSERT_NE(callRequestHandlerService->RejectCall(1, true, content), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandlerService->HangUpCall(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandlerService->HoldCall(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandlerService->UnHoldCall(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandlerService->SwitchCall(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandlerService->CombineConference(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandlerService->SeparateConference(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        callRequestHandlerService->UpdateImsCallMode(1, ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    std::u16string test = u"";
    ASSERT_NE(callRequestHandlerService->StartRtt(1, test), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(callRequestHandlerService->StopRtt(1), TELEPHONY_ERR_SUCCESS);
    std::vector<std::string> emptyRecords = {};
    ASSERT_NE(callRequestHandlerService->JoinConference(1, emptyRecords), TELEPHONY_ERR_SUCCESS);
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
    callRequestProcess->UpdateCallMediaModeRequest(1, ImsCallMode::CALL_MODE_AUDIO_ONLY);
    std::u16string test = u"";
    callRequestProcess->StartRttRequest(1, test);
    callRequestProcess->StopRttRequest(1);
    std::vector<std::string> numberList = {};
    callRequestProcess->JoinConference(1, numberList);
    DialParaInfo mDialParaInfo;
    callRequestProcess->CarrierDialProcess(mDialParaInfo);
    callRequestProcess->VoiceMailDialProcess(mDialParaInfo);
    callRequestProcess->OttDialProcess(mDialParaInfo);
    callRequestProcess->UpdateImsCallMode(1, ImsCallMode::CALL_MODE_AUDIO_ONLY);
    CellularCallInfo mCellularCallInfo;
    callRequestProcess->PackCellularCallInfo(mDialParaInfo, mCellularCallInfo);
    std::vector<std::u16string> testList = {};
    callRequestProcess->IsFdnNumber(testList, content);
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
    ASSERT_NE(DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumber(emptyStr, emptyStr, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumber(phoneNumber, emptyStr, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_GE(
        DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumber(phoneNumber, countryCode, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_GE(
        DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(emptyStr, emptyStr, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_GT(
        DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(phoneNumber, emptyStr, formatNumber),
        TELEPHONY_ERROR);
    ASSERT_GT(DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(
                  phoneNumber, countryCode, formatNumber),
        TELEPHONY_ERROR);
    ASSERT_FALSE(DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(INVALID_SLOTID));
    ASSERT_TRUE(DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(0));
}

/**
 * @tc.number   Telephony_CellularCallConnection_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CellularCallConnection_001, Function | MediumTest | Level1)
{
    CellularCallInfo mCellularCallInfo;
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->Dial(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->HangUp(
                  mCellularCallInfo, CallSupplementType::TYPE_DEFAULT),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->Reject(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->Answer(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->HoldCall(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->UnHoldCall(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->SwitchCall(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    bool enabled = false;
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->IsEmergencyPhoneNumber("", 0, enabled),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->CombineConference(mCellularCallInfo),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SeparateConference(mCellularCallInfo),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->StartDtmf('a', mCellularCallInfo),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->StopDtmf(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->GetCallTransferInfo(
                  CallTransferType::TRANSFER_TYPE_BUSY, 0),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SetCallWaiting(true, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->GetCallWaiting(0), TELEPHONY_ERR_SUCCESS);
    CallRestrictionInfo mCallRestrictionInfo;
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SetCallRestriction(mCallRestrictionInfo, 0),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->GetCallRestriction(
                  CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING, 0),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->SetCallPreferenceMode(0, 1), TELEPHONY_ERR_SUCCESS);
    std::u16string test = u"";
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->StartRtt(mCellularCallInfo, test),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->StopRtt(mCellularCallInfo), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SendUpdateCallMediaModeRequest(
                  mCellularCallInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->RegisterCallBackFun(), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->ReConnectService(), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_CellularCallConnection_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CellularCallConnection_002, Function | MediumTest | Level1)
{
    CallTransferInfo mCallTransferInfo;
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SetCallTransferInfo(mCallTransferInfo, 0),
        TELEPHONY_ERR_SUCCESS);
    bool result;
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->CanSetCallTransferTime(0, result),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->GetImsSwitchStatus(0, result), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->SetImsConfig(ImsConfigItem::ITEM_VIDEO_QUALITY, "", 0),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->SetImsConfig(ImsConfigItem::ITEM_VIDEO_QUALITY, 1, 0),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->GetImsConfig(ImsConfigItem::ITEM_VIDEO_QUALITY, 0),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SetImsFeatureValue(
                  FeatureType::TYPE_VOICE_OVER_LTE, 1, 0),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->GetImsFeatureValue(
                  FeatureType::TYPE_VOICE_OVER_LTE, 0),
        TELEPHONY_ERR_SUCCESS);
    std::vector<std::string> numberList = {};
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->InviteToConference(numberList, 0),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SetMute(0, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SendDtmf('a', ""), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SendDtmfString(
                  "", "", PhoneNetType::PHONE_TYPE_GSM, 1, 0),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->RegisterCallBack(nullptr), TELEPHONY_ERR_SUCCESS);
    std::u16string testStr = u"";
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->ControlCamera(testStr, 1, 1), TELEPHONY_ERR_SUCCESS);
    VideoWindow mVideoWindow;
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->SetPreviewWindow(mVideoWindow), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->SetDisplayWindow(mVideoWindow), TELEPHONY_ERR_SUCCESS);
    float zoomRatio = 1;
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SetCameraZoom(zoomRatio), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SetPausePicture(testStr), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->SetDeviceDirection(1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CellularCallConnection>::GetInstance()->SetImsSwitchStatus(0, true), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CellularCallConnection>::GetInstance()->ConnectService(), TELEPHONY_ERR_SUCCESS);
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
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, false), TELEPHONY_ERR_SUCCESS);
    mPacMap.PutIntValue("dialScene", static_cast<int32_t>(DialScene::CALL_PRIVILEGED));
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, true), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(mCallPolicy.DialPolicy(testEmptyStr, mPacMap, false), TELEPHONY_ERR_SUCCESS);
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
    ASSERT_NE(mCallPolicy.HoldCallPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.UnHoldCallPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.HangUpPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.SwitchCallPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.UpdateCallMediaModePolicy(0, ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.StartRttPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.StopRttPolicy(0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(mCallPolicy.IsValidSlotId(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
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
 * @tc.number   Telephony_ReportCallInfoHandler_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_ReportCallInfoHandler_001, Function | MediumTest | Level1)
{
    auto runner = AppExecFwk::EventRunner::Create("test");
    auto report_callinfo_handler = std::make_shared<ReportCallInfoHandler>(runner);
    auto event = AppExecFwk::InnerEvent::Get(0);
    report_callinfo_handler->ProcessEvent(event);
    report_callinfo_handler->ReportCallInfo(event);
    report_callinfo_handler->ReportCallsInfo(event);
    report_callinfo_handler->ReportDisconnectedCause(event);
    report_callinfo_handler->ReportEventInfo(event);
    report_callinfo_handler->ReportOttEvent(event);
    report_callinfo_handler->OnUpdateMediaModeResponse(event);
    event = nullptr;
    report_callinfo_handler->ProcessEvent(event);
    report_callinfo_handler->ReportCallInfo(event);
    report_callinfo_handler->ReportCallsInfo(event);
    report_callinfo_handler->ReportDisconnectedCause(event);
    report_callinfo_handler->ReportEventInfo(event);
    report_callinfo_handler->ReportOttEvent(event);
    report_callinfo_handler->OnUpdateMediaModeResponse(event);
    DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->Start();
    CallDetailInfo mCallDetailInfo;
    ASSERT_EQ(DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateCallReportInfo(mCallDetailInfo),
        TELEPHONY_ERR_SUCCESS);
    CallDetailsInfo mCallDetailsInfo;
    ASSERT_EQ(DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateCallsReportInfo(mCallDetailsInfo),
        TELEPHONY_ERR_SUCCESS);
    DisconnectedDetails mDisconnectedDetails;
    ASSERT_EQ(
        DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateDisconnectedCause(mDisconnectedDetails),
        TELEPHONY_ERR_SUCCESS);
    CellularCallEventInfo mCellularCallEventInfo;
    ASSERT_EQ(
        DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateEventResultInfo(mCellularCallEventInfo),
        TELEPHONY_ERR_SUCCESS);
    OttCallEventInfo mOttCallEventInfo;
    ASSERT_NE(DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateOttEventInfo(mOttCallEventInfo),
        TELEPHONY_ERR_SUCCESS);
    CallMediaModeResponse mCallMediaModeResponse;
    ASSERT_NE(
        DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateMediaModeResponse(mCallMediaModeResponse),
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
    ASSERT_NE(
        DelayedSingleton<VideoControlManager>::GetInstance()->ControlCamera(testEmptyStr, 1, 1), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<VideoControlManager>::GetInstance()->ControlCamera(testStr, 1, 1), TELEPHONY_ERR_SUCCESS);
    float zoomRatio = 11;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetCameraZoom(zoomRatio), TELEPHONY_ERR_SUCCESS);
    zoomRatio = 0.01;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetCameraZoom(zoomRatio), TELEPHONY_ERR_SUCCESS);
    zoomRatio = 1;
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetCameraZoom(zoomRatio), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->SetPausePicture(testStr), TELEPHONY_ERR_SUCCESS);
    int32_t rotation = CAMERA_ROTATION_0;
    ASSERT_NE(
        DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(rotation), TELEPHONY_ERR_SUCCESS);
    rotation = CAMERA_ROTATION_90;
    ASSERT_NE(
        DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(rotation), TELEPHONY_ERR_SUCCESS);
    rotation = CAMERA_ROTATION_180;
    ASSERT_NE(
        DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(rotation), TELEPHONY_ERR_SUCCESS);
    rotation = CAMERA_ROTATION_270;
    ASSERT_NE(
        DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(rotation), TELEPHONY_ERR_SUCCESS);
    rotation = 1;
    ASSERT_NE(
        DelayedSingleton<VideoControlManager>::GetInstance()->SetDeviceDirection(rotation), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->OpenCamera(testStr, 0, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<VideoControlManager>::GetInstance()->CloseCamera(testStr, 0, 0), TELEPHONY_ERR_SUCCESS);
    ASSERT_FALSE(DelayedSingleton<VideoControlManager>::GetInstance()->ContainCameraID(""));
    ASSERT_FALSE(DelayedSingleton<VideoControlManager>::GetInstance()->IsPngFile(""));
}

/**
 * @tc.number   Telephony_VideoControlManager_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_VideoControlManager_002, Function | MediumTest | Level1)
{
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
    ASSERT_NE(audioOnlyState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        audioOnlyState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        audioOnlyState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        audioOnlyState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        audioOnlyState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        audioOnlyState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        audioOnlyState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        audioOnlyState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        audioOnlyState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
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
    ASSERT_NE(
        videoSendState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoSendState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoSendState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoSendState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoSendState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoSendState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoSendState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoSendState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoSendState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoSendState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
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
    ASSERT_NE(
        videoReceiveState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoReceiveState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoReceiveState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoReceiveState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoReceiveState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoReceiveState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoReceiveState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoReceiveState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_RECEIVE_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoReceiveState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoReceiveState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_VIDEO_PAUSED),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
    ASSERT_GE(
        videoReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_GE(
        videoReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_GE(
        videoReceiveState.SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_VideoCallState_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_VideoCallState_004, Function | MediumTest | Level1)
{
    VideoSendReceiveState videoSendReceiveState = VideoSendReceiveState(nullptr);
    ASSERT_NE(
        videoSendReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoSendReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoSendReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoSendReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoSendReceiveState.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoSendReceiveState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoSendReceiveState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoSendReceiveState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoSendReceiveState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(videoSendReceiveState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoSendReceiveState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoSendReceiveState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoSendReceiveState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_RECEIVE_ONLY),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoSendReceiveState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_SEND_RECEIVE),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(videoSendReceiveState.ReceiveUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_VIDEO_PAUSED),
        TELEPHONY_ERR_SUCCESS);
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
        videoPauseState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoPauseState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_EQ(
        videoPauseState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_RECEIVE_ONLY), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoPauseState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_SEND_RECEIVE), TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        videoPauseState.RecieveUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_VIDEO_PAUSED), TELEPHONY_ERR_SUCCESS);
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
    bluetoothCallClient->GetCurrentCallList(0).size();
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
    ASSERT_NE(callManagerClient->DialCall(value, extras), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->AnswerCall(0, 0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->RejectCall(0, false, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->HangUpCall(0), TELEPHONY_SUCCESS);
    ASSERT_GE(callManagerClient->GetCallState(), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->CancelMissedIncomingCallNotification(), TELEPHONY_SUCCESS);
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
    bool enabled;
    ASSERT_NE(callManagerClient->IsEmergencyPhoneNumber(value, 0, enabled), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->FormatPhoneNumber(value, value, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->FormatPhoneNumberToE164(value, value, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->SetMuted(false), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->MuteRinger(), TELEPHONY_SUCCESS);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_EARPIECE,
        .address = { 0 },
    };
    ASSERT_NE(callManagerClient->SetAudioDevice(audioDevice), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->ControlCamera(value), TELEPHONY_SUCCESS);
    VideoWindow window;
    ASSERT_NE(callManagerClient->SetPreviewWindow(window), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerClient->SetDisplayWindow(window), TELEPHONY_SUCCESS);
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
    ASSERT_GT(callManagerClient->SetPausePicture(value), TELEPHONY_ERROR);
    ASSERT_GT(callManagerClient->SetDeviceDirection(0), TELEPHONY_ERROR);
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
    callManagerHisysevent->WriteIncomingCallFaultEvent(0, 0, 0, -1, desc);
    callManagerHisysevent->WriteDialCallFaultEvent(0, 0, 0, static_cast<int32_t>(CALL_ERR_INVALID_SLOT_ID), desc);
    callManagerHisysevent->WriteDialCallFaultEvent(0, 0, 0, -1, desc);
    callManagerHisysevent->WriteAnswerCallFaultEvent(0, 0, 0, static_cast<int32_t>(CALL_ERR_INVALID_SLOT_ID), desc);
    callManagerHisysevent->WriteAnswerCallFaultEvent(0, 0, 0, -1, desc);
    callManagerHisysevent->WriteHangUpFaultEvent(0, 0, static_cast<int32_t>(CALL_ERR_INVALID_SLOT_ID), desc);
    callManagerHisysevent->WriteHangUpFaultEvent(0, 0, -1, desc);
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
    int32_t videoState = static_cast<int32_t>(VideoStateType::TYPE_VOICE);
    CallAttributeInfo callAttributeInfo;
    ottCall->GetCallAttributeInfo(callAttributeInfo);
    ASSERT_EQ(ottCall->DialingProcess(), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->AnswerCall(videoState), TELEPHONY_SUCCESS);
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
    ASSERT_NE(ottCall->CanSeparateConference(), TELEPHONY_SUCCESS);
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
    ImsCallMode mode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
    ASSERT_NE(ottCall->SendUpdateCallMediaModeRequest(mode), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->RecieveUpdateCallMediaModeRequest(mode), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->SendUpdateCallMediaModeResponse(mode), TELEPHONY_SUCCESS);
    CallMediaModeResponse response;
    ASSERT_NE(ottCall->ReceiveUpdateCallMediaModeResponse(response), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->DispatchUpdateVideoRequest(mode), TELEPHONY_SUCCESS);
    ASSERT_NE(ottCall->DispatchUpdateVideoResponse(mode), TELEPHONY_SUCCESS);
    int32_t mute = 0;
    ASSERT_NE(ottCall->SetMute(mute, SIM1_SLOTID), TELEPHONY_SUCCESS);
    OttCallRequestInfo requestInfo = {
        .phoneNum = "",
        .bundleName = "",
        .videoState = VideoStateType::TYPE_VOICE,
    };
    ASSERT_EQ(ottCall->PackOttCallRequestInfo(requestInfo), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_Bluetooth_Call_Manager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_Bluetooth_Call_Manager_001, Function | MediumTest | Level3)
{
    BluetoothCallManager bluetoothCallManager = BluetoothCallManager();
    bluetoothCallManager.btConnection_ = nullptr;
    std::string bluetoothAddress = "test";
    ASSERT_EQ(bluetoothCallManager.ConnectBtSco(bluetoothAddress), false);
    ASSERT_EQ(bluetoothCallManager.DisconnectBtSco(), false);
    ASSERT_EQ(bluetoothCallManager.SendBtCallState(0, 0, 0, bluetoothAddress), false);
    ASSERT_EQ(bluetoothCallManager.GetBtScoState(), false);
    ASSERT_EQ(bluetoothCallManager.IsBtScoConnected(), false);
    ASSERT_EQ(bluetoothCallManager.IsBtAvailble(), false);
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
    ASSERT_EQ(ottConference.JoinToConference(1), CALL_ERR_CONFERENCE_CALL_EXCEED_LIMIT);
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
    ottConference.subCallIdSet_.clear();
    ASSERT_EQ(ottConference.CanSeparateConference(), CALL_ERR_CONFERENCE_NOT_EXISTS);
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
    ASSERT_EQ(csConference.JoinToConference(1), CALL_ERR_CONFERENCE_CALL_EXCEED_LIMIT);
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
    csConference.subCallIdSet_.clear();
    ASSERT_EQ(csConference.CanSeparateConference(), CALL_ERR_CONFERENCE_NOT_EXISTS);
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
    ASSERT_EQ(imsConference.JoinToConference(1), CALL_ERR_CONFERENCE_CALL_EXCEED_LIMIT);
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
    imsConference.subCallIdSet_.clear();
    ASSERT_EQ(imsConference.CanSeparateConference(), CALL_ERR_CONFERENCE_NOT_EXISTS);
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
}

/**
 * @tc.number   Telephony_BluetoothConnection_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_BluetoothConnection_001, Function | MediumTest | Level3)
{
    BluetoothConnection bluetoothConnection;
    bluetoothConnection.SetBtScoState(BtScoState::SCO_STATE_CONNECTED);
    ASSERT_EQ(BtScoState::SCO_STATE_CONNECTED, bluetoothConnection.GetBtScoState());
    ASSERT_EQ(true, bluetoothConnection.ConnectBtSco());
    ASSERT_EQ(true, bluetoothConnection.IsBtScoConnected());
    bluetoothConnection.SetBtScoState(BtScoState::SCO_STATE_DISCONNECTED);
    ASSERT_EQ(true, bluetoothConnection.DisconnectBtSco());
#ifdef ABILITY_BLUETOOTH_SUPPORT
    bluetoothConnection.Init();
    bluetoothConnection.SetBtScoState(BtScoState::SCO_STATE_UNKNOWN);
    bluetoothConnection.connectedScoAddr_ = "123456";
    ASSERT_EQ(false, bluetoothConnection.ConnectBtSco());
    bluetoothConnection.connectedScoAddr_ = "";
    Bluetooth::BluetoothRemoteDevice device;
    bluetoothConnection.mapConnectedBtDevices_["test"] = device;
    ASSERT_EQ(false, bluetoothConnection.ConnectBtSco());
    bluetoothConnection.mapConnectedBtDevices_.clear();
    ASSERT_EQ(false, bluetoothConnection.ConnectBtSco());
    bluetoothConnection.SetBtScoState(BtScoState::SCO_STATE_CONNECTED);
    bluetoothConnection.connectedScoAddr_ = "test";
    ASSERT_EQ(true, bluetoothConnection.ConnectBtSco("test"));
    ASSERT_EQ(false, bluetoothConnection.ConnectBtSco(device));
    bluetoothConnection.DisconnectBtSco(device);
    bluetoothConnection.mapConnectedBtDevices_.clear();
    bluetoothConnection.IsBtAvailble();
    bluetoothConnection.OnScoStateChanged(device, (int32_t)Bluetooth::HfpScoConnectState::SCO_CONNECTED);
    bluetoothConnection.connectedScoAddr_ = device.GetDeviceAddr();
    bluetoothConnection.OnScoStateChanged(device, (int32_t)Bluetooth::HfpScoConnectState::SCO_DISCONNECTED);
    bluetoothConnection.OnScoStateChanged(device, -1);
    bluetoothConnection.AddBtDevice("test", device);
    bluetoothConnection.AddBtDevice("test", device);
    bluetoothConnection.RemoveBtDevice("test");
    bluetoothConnection.RemoveBtDevice("test");
    bluetoothConnection.OnConnectionStateChanged(device, (int32_t)Bluetooth::HfpScoConnectState::SCO_CONNECTED);
    bluetoothConnection.OnConnectionStateChanged(device, (int32_t)Bluetooth::HfpScoConnectState::SCO_DISCONNECTED);
    ASSERT_EQ(nullptr, bluetoothConnection.GetBtDevice("test"));
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
 * @tc.number   Telephony_CarrierCall_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CarrierCall_001, Function | MediumTest | Level3)
{
    DialParaInfo dialParaInfo;
    CSCall call { dialParaInfo };
    call.callRunningState_ = CallRunningState::CALL_RUNNING_STATE_CREATE;
    call.accountNumber_ = GetTestNumber();
    ASSERT_EQ(CALL_ERR_ANSWER_FAILED, call.CarrierAnswerCall(0));
    call.callRunningState_ = CallRunningState::CALL_RUNNING_STATE_RINGING;
    call.cellularCallConnectionPtr_ = nullptr;
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.CarrierAnswerCall(0));
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.CarrierRejectCall());
    call.cellularCallConnectionPtr_ = DelayedSingleton<CellularCallConnection>::GetInstance();
    call.callState_ = TelCallState::CALL_STATUS_INCOMING;
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, call.CarrierRejectCall());
    call.callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, call.CarrierRejectCall());
    call.cellularCallConnectionPtr_ = nullptr;
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.CarrierHangUpCall());
    call.cellularCallConnectionPtr_ = DelayedSingleton<CellularCallConnection>::GetInstance();
    call.policyFlag_ = POLICY_FLAG_HANG_UP_ACTIVE;
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, call.CarrierHangUpCall());
    call.policyFlag_ = 0;
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, call.CarrierHangUpCall());
    call.cellularCallConnectionPtr_ = nullptr;
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.CarrierHoldCall());
    call.cellularCallConnectionPtr_ = DelayedSingleton<CellularCallConnection>::GetInstance();
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, call.CarrierHoldCall());
    call.cellularCallConnectionPtr_ = nullptr;
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.CarrierUnHoldCall());
    call.cellularCallConnectionPtr_ = DelayedSingleton<CellularCallConnection>::GetInstance();
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, call.CarrierUnHoldCall());
    call.cellularCallConnectionPtr_ = nullptr;
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.CarrierSetMute(0, 0));
    call.cellularCallConnectionPtr_ = DelayedSingleton<CellularCallConnection>::GetInstance();
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, call.CarrierSetMute(0, 0));
    call.cellularCallConnectionPtr_ = nullptr;
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.CarrierSwitchCall());
    call.cellularCallConnectionPtr_ = DelayedSingleton<CellularCallConnection>::GetInstance();
    ASSERT_NE(TELEPHONY_ERR_SUCCESS, call.CarrierSwitchCall());
    call.cellularCallConnectionPtr_ = nullptr;
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.CarrierCombineConference());
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.CarrierSeparateConference());
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.StartDtmf('c'));
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.StopDtmf());
}

/**
 * @tc.number   Telephony_Callbase_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_Callbase_001, Function | MediumTest | Level3)
{
    DialParaInfo dialParaInfo;
    CSCall call { dialParaInfo };
    call.callRunningState_ = CallRunningState::CALL_RUNNING_STATE_CREATE;
    ASSERT_EQ(CALL_ERR_PHONE_ANSWER_IS_BUSY, call.AnswerCallBase());
    call.callRunningState_ = CallRunningState::CALL_RUNNING_STATE_RINGING;
    call.callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    call.callBeginTime_ = 0;
    call.StateChangesToActive();
    call.conferenceState_ = TelConferenceState::TEL_CONFERENCE_ACTIVE;
    call.ringEndTime_ = 0;
    call.conferenceState_ = TelConferenceState::TEL_CONFERENCE_DISCONNECTING;
    call.StateChangesToDisconnected();
    call.StateChangesToDisconnecting();
    call.conferenceState_ = TelConferenceState::TEL_CONFERENCE_ACTIVE;
    call.StateChangesToDisconnected();
    call.StateChangesToDisconnecting();
    call.isSpeakerphoneOn_ = true;
    call.SetAudio();
    call.isSpeakerphoneOn_ = false;
    call.SetAudio();
    ASSERT_EQ(CALL_ERR_NOT_NEW_STATE, call.SetTelCallState(TelCallState::CALL_STATUS_UNKNOWN));
    ASSERT_EQ(TELEPHONY_SUCCESS, call.SetTelCallState(TelCallState::CALL_STATUS_DIALING));
    ASSERT_EQ(TELEPHONY_SUCCESS, call.SetTelCallState(TelCallState::CALL_STATUS_INCOMING));
    ASSERT_EQ(TELEPHONY_SUCCESS, call.SetTelCallState(TelCallState::CALL_STATUS_WAITING));
    ASSERT_EQ(TELEPHONY_SUCCESS, call.SetTelCallState(TelCallState::CALL_STATUS_ACTIVE));
    ASSERT_EQ(TELEPHONY_SUCCESS, call.SetTelCallState(TelCallState::CALL_STATUS_HOLDING));
    ASSERT_EQ(TELEPHONY_SUCCESS, call.SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTED));
    ASSERT_EQ(TELEPHONY_SUCCESS, call.SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTING));
    ASSERT_EQ(TELEPHONY_SUCCESS, call.SetTelCallState(TelCallState::CALL_STATUS_ALERTING));
    ASSERT_EQ(TELEPHONY_SUCCESS, call.SetTelCallState(TelCallState::CALL_STATUS_UNKNOWN));
    call.IsSpeakerphoneEnabled();
    call.callState_ = TelCallState::CALL_STATUS_IDLE;
    ASSERT_EQ(false, call.IsAliveState());
    call.callState_ = TelCallState::CALL_STATUS_DISCONNECTED;
    ASSERT_EQ(false, call.IsAliveState());
    call.callState_ = TelCallState::CALL_STATUS_DISCONNECTING;
    ASSERT_EQ(false, call.IsAliveState());
    call.callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_EQ(true, call.IsAliveState());
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
    callBase1->conferenceState_ = TelConferenceState::TEL_CONFERENCE_HOLDING;
    ASSERT_EQ(TELEPHONY_SUCCESS, callPolicy.SeparateConferencePolicy(policy));
    callBase1->conferenceState_ = TelConferenceState::TEL_CONFERENCE_IDLE;
    ASSERT_NE(TELEPHONY_SUCCESS, callPolicy.SeparateConferencePolicy(policy));
}

/**
 * @tc.number   Telephony_ImsCall_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_ImsCall_001, Function | MediumTest | Level3)
{
    DialParaInfo dialParaInfo;
    IMSCall call { dialParaInfo };
    call.isInitialized_ = true;
    call.DialingProcess();
    call.AnswerCall(0);
    call.RejectCall();
    call.HoldCall();
    call.UnHoldCall();
    call.SwitchCall();
    call.RejectCall();
    call.RejectCall();
    ASSERT_EQ(TELEPHONY_SUCCESS, call.InitVideoCall());
    call.accountNumber_ = GetTestNumber();
    std::u16string msg;
    call.SetMute(0, 0);
    call.SeparateConference();
    call.CanCombineConference();
    call.CanSeparateConference();
    call.LaunchConference();
    call.ExitConference();
    call.HoldConference();
    ASSERT_NE(TELEPHONY_SUCCESS, call.StartRtt(msg));
    call.AcceptVideoCall();
    call.RefuseVideoCall();
    ASSERT_NE(TELEPHONY_SUCCESS, call.StopRtt());
    call.callState_ = TelCallState::CALL_STATUS_UNKNOWN;
    ASSERT_EQ(CALL_ERR_CALL_STATE, call.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY));
    call.callState_ = TelCallState::CALL_STATUS_ACTIVE;
    call.videoCallState_ = nullptr;
    ASSERT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, call.SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY));
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
    ASSERT_NE(callRecordsManager.CancelMissedIncomingCallNotification(), TELEPHONY_SUCCESS);
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
    ASSERT_FALSE(callControlManager->NotifyCallStateUpdated(callObjectPtr, priorState, nextState));
    DisconnectedDetails details;
    ASSERT_FALSE(callControlManager->NotifyCallDestroyed(details));
    ASSERT_FALSE(callControlManager->NotifyIncomingCallAnswered(callObjectPtr));
    ASSERT_FALSE(callControlManager->NotifyIncomingCallRejected(callObjectPtr, false, ""));
    CallEventInfo info;
    ASSERT_FALSE(callControlManager->NotifyCallEventUpdated(info));
    DialParaInfo dialParaInfo;
    callObjectPtr = new CSCall(dialParaInfo);
    ASSERT_TRUE(callControlManager->NotifyNewCallCreated(callObjectPtr));
    ASSERT_FALSE(callControlManager->NotifyCallStateUpdated(callObjectPtr, priorState, nextState));
    ASSERT_FALSE(callControlManager->NotifyIncomingCallAnswered(callObjectPtr));
    ASSERT_FALSE(callControlManager->NotifyIncomingCallRejected(callObjectPtr, false, ""));

    callControlManager->Init();
    ASSERT_NE(callControlManager->RejectCall(VALID_CALLID, false, u""), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->RejectCall(INVALID_CALLID, false, u""), TELEPHONY_SUCCESS);
    ASSERT_NE(callControlManager->HangUpCall(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_TRUE(callControlManager->NotifyNewCallCreated(callObjectPtr));
    ASSERT_TRUE(callControlManager->NotifyCallStateUpdated(callObjectPtr, priorState, nextState));
    ASSERT_TRUE(callControlManager->NotifyIncomingCallAnswered(callObjectPtr));
    ASSERT_TRUE(callControlManager->NotifyIncomingCallRejected(callObjectPtr, false, ""));
    ASSERT_NE(callControlManager->CancelMissedIncomingCallNotification(), TELEPHONY_SUCCESS);
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
    ASSERT_NE(callControlManager->UpdateImsCallMode(INVALID_CALLID, mode), TELEPHONY_SUCCESS);
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
    CallDetailInfo info = {
        .phoneNum = "123",
    };
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
}

/**
 * @tc.number   Telephony_CallStatusManager_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, Telephony_CallStatusManager_002, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    CallDetailInfo callDetailInfo = {
        .phoneNum = "",
    };
    callDetailInfo.state = TelCallState::CALL_STATUS_INCOMING;
    callDetailInfo.callType = CallType::TYPE_CS;
    ASSERT_GT(callStatusManager->IncomingHandle(callDetailInfo), TELEPHONY_ERROR);
    callDetailInfo.state = TelCallState::CALL_STATUS_ACTIVE;
    ASSERT_GT(callStatusManager->IncomingHandle(callDetailInfo), TELEPHONY_ERROR);
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
    CallDetailInfo callDetailInfo = {
        .phoneNum = "",
    };
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
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_CS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_INCOMING;
    callObjectPtr = new CSCall(dialParaInfo);
    ASSERT_TRUE(callObjectPtr != nullptr);
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
    ASSERT_NE(conference->SetMainCall(ERROR_CALLID), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->SetMainCall(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_NE(conference->CanSeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->JoinToConference(VALID_CALLID), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->GetSubCallIdList(VALID_CALLID, callIdList), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->GetCallIdListForConference(VALID_CALLID, callIdList), TELEPHONY_SUCCESS);
    ASSERT_EQ(conference->CanSeparateConference(), TELEPHONY_SUCCESS);
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
    std::shared_ptr<CallAbilityReportProxy> callAbilityReportProxy = std::make_shared<CallAbilityReportProxy>();
    sptr<ICallAbilityCallback> callAbilityCallbackPtr = nullptr;
    std::string bundleName = "test";
    ASSERT_NE(callAbilityReportProxy->RegisterCallBack(callAbilityCallbackPtr, bundleName), TELEPHONY_SUCCESS);
    ASSERT_NE(callAbilityReportProxy->UnRegisterCallBack(bundleName), TELEPHONY_SUCCESS);
    sptr<CallBase> callObjectPtr = nullptr;
    callAbilityReportProxy->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    callAbilityCallbackPtr = new CallAbilityCallback();
    ASSERT_EQ(callAbilityReportProxy->RegisterCallBack(callAbilityCallbackPtr, bundleName), TELEPHONY_SUCCESS);
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
    std::string ottBundleName = "com.ohos.callservice";
    sptr<CallAbilityCallback> ottCallAbilityCallbackPtr = new CallAbilityCallback();
    ASSERT_EQ(callAbilityReportProxy->RegisterCallBack(ottCallAbilityCallbackPtr, ottBundleName), TELEPHONY_SUCCESS);
    callAbilityReportProxy->OttCallRequest(ottReportId, resultInfo);
    ASSERT_EQ(callAbilityReportProxy->UnRegisterCallBack(bundleName), TELEPHONY_SUCCESS);
}
} // namespace Telephony
} // namespace OHOS
