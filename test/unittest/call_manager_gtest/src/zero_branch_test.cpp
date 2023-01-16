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
#include "call_number_utils.h"
#include "call_policy.h"
#include "call_request_handler.h"
#include "call_request_process.h"
#include "call_setting_manager.h"
#include "cellular_call_connection.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "cs_call.h"
#include "gtest/gtest.h"
#include "incoming_call_notification.h"
#include "missed_call_notification.h"
#include "ott_call.h"
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
constexpr const char *TEST_STR = "123";
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
    ASSERT_NE(
        DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumber(phoneNumber, countryCode, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(emptyStr, emptyStr, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(
        DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(phoneNumber, emptyStr, formatNumber),
        TELEPHONY_ERR_SUCCESS);
    ASSERT_NE(DelayedSingleton<CallNumberUtils>::GetInstance()->FormatPhoneNumberToE164(
                  phoneNumber, countryCode, formatNumber),
        TELEPHONY_ERR_SUCCESS);
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
    event = nullptr;
    report_callinfo_handler->ProcessEvent(event);
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
} // namespace Telephony
} // namespace OHOS
