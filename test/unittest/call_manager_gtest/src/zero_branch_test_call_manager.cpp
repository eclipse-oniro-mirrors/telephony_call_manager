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

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
namespace {
    const int32_t VALID_CALLID = 1;
} // namespace

class CallManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void CallManagerTest::SetUpTestCase() {}

void CallManagerTest::TearDownTestCase() {}

void CallManagerTest::SetUp() {}

void CallManagerTest::TearDown() {}

/**
 * @tc.number   Telephony_CallRequestProcess_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerTest, Telephony_CallRequestProcess_001, Function | MediumTest | Level1)
{
    std::unique_ptr<CallRequestProcess> callRequestProcess = std::make_unique<CallRequestProcess>();
    DialParaInfo mDialParaInfo;
    sptr<CallBase> call = new CSCall(mDialParaInfo);
    call->SetCallId(VALID_CALLID);
    call->SetCallType(CallType::TYPE_SATELLITE);
    sptr<CallBase> incomingCall = new CSCall(mDialParaInfo);
    call->SetTelCallState(TelCallState::CALL_STATUS_HOLDING);
    int32_t slotId = 1;
    callRequestProcess->HandleDsdaIncomingCall(call, 0, slotId, 0, incomingCall);
    call->SetTelCallState(TelCallState ::CALL_STATUS_DIALING);
    sptr<CallBase> holdCall = new CSCall(mDialParaInfo);
    slotId = 0;
    bool flagForConference = false;
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    callRequestProcess->HandleCallWaitingNumOneNext(incomingCall, call, holdCall, slotId, flagForConference);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    callRequestProcess->AddOneCallObject(call);
    callRequestProcess->HandleCallWaitingNumOneNext(incomingCall, call, holdCall, slotId, flagForConference);
    sptr<CallBase> listCall = new CSCall(mDialParaInfo);
    listCall->SetCallType(CallType::TYPE_IMS);
    listCall->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    listCall->SetSlotId(1);
    listCall->SetCallId(1);
    callRequestProcess->AddOneCallObject(listCall);
    callRequestProcess->HandleCallWaitingNumOneNext(incomingCall, call, holdCall, slotId, flagForConference);
    callRequestProcess->AnswerRequestForDsda(call, 1, 0);
    sptr<CallBase> listCall2 = new CSCall(mDialParaInfo);
    listCall2->SetCallType(CallType::TYPE_IMS);
    listCall2->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    listCall2->SetSlotId(1);
    listCall2->SetCallId(1);
    listCall2->SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
    callRequestProcess->AddOneCallObject(listCall2);
    sptr<CallBase> listCall3 = new CSCall(mDialParaInfo);
    listCall3->SetCallType(CallType::TYPE_VOIP);
    listCall3->SetTelCallState(TelCallState::CALL_STATUS_WAITING);
    listCall3->SetSlotId(1);
    listCall3->SetCallId(1);
    listCall3->SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
    callRequestProcess->AddOneCallObject(listCall3);
    callRequestProcess->HangUpRequest(1);
    callRequestProcess->HangUpForDsdaRequest(holdCall);
    std::vector<std::u16string> testList = {};
    std::u16string formatNumber = u"123455667";
    testList.emplace_back(formatNumber);
    callRequestProcess->IsFdnNumber(testList, "1234567890");
    callRequestProcess->UnHoldRequest(1);
    callRequestProcess->EccDialPolicy();
}

/**
 * @tc.number   Telephony_CallControlManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerTest, Telephony_CallControlManager_001, Function | MediumTest | Level3)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    callControlManager->Init();
    std::u16string number = u"123456789";
    AppExecFwk::PacMap extras;
    callControlManager->DialCall(number, extras);
    DialParaInfo mDialParaInfo;
    sptr<CallBase> call = new CSCall(mDialParaInfo);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    call->SetCallId(1);
    call->SetCallType(CallType::TYPE_IMS);
    callControlManager->AddOneCallObject(call);
    callControlManager->AnswerCall(INVALID_CALLID, 0);
    sptr<CallBase> call2 = new CSCall(mDialParaInfo);
    call2->SetCallType(CallType::TYPE_IMS);
    call2->SetVideoStateType(VideoStateType::TYPE_SEND_ONLY);
    callControlManager->AnswerHandlerForSatelliteOrVideoCall(call2, 0);
    sptr<CallBase> call3 = new CSCall(mDialParaInfo);
    call3->SetCallType(CallType::TYPE_SATELLITE);
    callControlManager->AnswerHandlerForSatelliteOrVideoCall(call3, 0);
    call2->SetCallType(CallType::TYPE_VOIP);
    call3->SetCallType(CallType::TYPE_VOIP);
    call2->SetCallId(2);
    call3->SetCallId(3);
    call2->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    call2->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    callControlManager->AddOneCallObject(call2);
    callControlManager->AddOneCallObject(call3);
    callControlManager->CarrierAndVoipConflictProcess(1, TelCallState::CALL_STATUS_ANSWERED);
    callControlManager->HoldCall(2);
    auto ba = callControlManager->GetOneCallObject(2);
    ba->SetTelCallState(TelCallState::CALL_STATUS_HOLDING);
    callControlManager->SwitchCall(2);
    ba->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_HOLD);
    callControlManager->UnHoldCall(2);
    ba->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    auto ba3 = callControlManager->GetOneCallObject(3);
    ba3->SetTelCallState(TelCallState::CALL_STATUS_HOLDING);
    callControlManager->CombineConference(2);
    callControlManager->SeparateConference(2);
    callControlManager->KickOutFromConference(2);
}

/**
 * @tc.number   Telephony_CallStatusManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerTest, Telephony_CallStatusManager_001, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    callStatusManager->Init();
    DialParaInfo paraInfo;
    AppExecFwk::PacMap extras;
    sptr<CallBase> call = (std::make_unique<CSCall>(paraInfo, extras)).release();
    std::string phoneNum;
    callStatusManager->SetContactInfo(call, phoneNum);
    CallDetailInfo info;
    callStatusManager->UpdateDialingCallInfo(info);
    DialParaInfo mDialParaInfo;
    mDialParaInfo.accountId = 0;
    mDialParaInfo.callType = CallType::TYPE_IMS;
    mDialParaInfo.videoState = VideoStateType::TYPE_VIDEO;
    sptr<OHOS::Telephony::CallBase> call1 = new IMSCall(mDialParaInfo);
    callStatusManager->AddOneCallObject(call1);
    callStatusManager->UpdateDialingCallInfo(info);
    callStatusManager->ShouldBlockIncomingCall(call1, info);
    CallDetailInfo detail;
    CallDetailsInfo isInfo;
    isInfo.callVec.emplace_back(detail);
    callStatusManager->HandleCallsReportInfo(isInfo);
    callStatusManager->ActiveHandle(info);
    callStatusManager->HoldingHandle(info);
    callStatusManager->AlertHandle(info);
    callStatusManager->RefreshCallIfNecessary(call1, info);
    DialParaInfo mDialParaInfo2;
    mDialParaInfo2.accountId = 0;
    mDialParaInfo2.callType = CallType::TYPE_VOIP;
    mDialParaInfo2.videoState = VideoStateType::TYPE_VIDEO;
    mDialParaInfo2.voipCallInfo.voipCallId = "123";
    sptr<OHOS::Telephony::CallBase> call2 = new IMSCall(mDialParaInfo2);
    VoipCallEventInfo voipInfo;
    voipInfo.voipCallId = "123";
    callStatusManager->AddOneCallObject(call2);
    callStatusManager->HandleVoipEventReportInfo(voipInfo);
    callStatusManager->DisconnectingHandle(info);
    callStatusManager->DisconnectedHandle(info);
}
}
}