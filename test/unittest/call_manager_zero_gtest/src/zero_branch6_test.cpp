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

class ZeroBranch5Test : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void ZeroBranch5Test::SetUp() {}

void ZeroBranch5Test::TearDown() {}

void ZeroBranch5Test::SetUpTestCase()
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
        .processName = "ZeroBranch5Test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void ZeroBranch5Test::TearDownTestCase() {}

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
 * @tc.number   Telephony_CallStatusManager_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallStatusManager_003, Function | MediumTest | Level3)
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
 * @tc.number   Telephony_CallStatusManager_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallStatusManager_004, Function | MediumTest | Level3)
{
    std::shared_ptr<CallStatusManager> callStatusManager = std::make_shared<CallStatusManager>();
    callStatusManager->Init();
    sptr<CallBase> callObjectPtr = nullptr;
    callStatusManager->HandleRejectCall(callObjectPtr, true);
    DialParaInfo dialParaInfo;
    dialParaInfo.callType = CallType::TYPE_CS;
    dialParaInfo.callState = TelCallState::CALL_STATUS_INCOMING;
    callObjectPtr = new CSCall(dialParaInfo);
    callStatusManager->HandleRejectCall(callObjectPtr, true);
    CallDetailInfo callDetailInfo;
    std::string number = "";
    memcpy_s(&callDetailInfo.phoneNum, kMaxNumberLen, number.c_str(), number.length());
    callDetailInfo.state = TelCallState::CALL_STATUS_INCOMING;
    callDetailInfo.callType = CallType::TYPE_CS;
    callDetailInfo.voipCallInfo.voipCallId = "123456789";
    callStatusManager->OutgoingVoipCallHandle(callDetailInfo);
    std::vector<std::u16string> callIdList;
    CallRunningState previousState = CallRunningState::CALL_RUNNING_STATE_CREATE;
    TelCallState priorState = TelCallState::CALL_STATUS_INCOMING;
    callStatusManager->HandleHoldCallOrAutoAnswerCall(callObjectPtr, callIdList, previousState, priorState);
    callStatusManager->AutoAnswerForVoiceCall(callObjectPtr, SIM1_SLOTID, true);
    callStatusManager->SetVideoCallState(callObjectPtr, TelCallState::CALL_STATUS_ACTIVE);
    callStatusManager->ShouldRejectIncomingCall();
    callStatusManager->IsRingOnceCall(callObjectPtr, callDetailInfo);
    sptr<CallBase> callObjectPtr1 = nullptr;
    int32_t res = callStatusManager->HandleRingOnceCall(callObjectPtr1);
    callStatusManager->HandleRingOnceCall(callObjectPtr);
    ASSERT_NE(res, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_IncomingCallWakeup_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_IncomingCallWakeup_001, Function | MediumTest | Level3)
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
HWTEST_F(ZeroBranch5Test, Telephony_CallStateReportProxy_001, Function | MediumTest | Level3)
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
    ASSERT_TRUE(!number.empty());
}

/**
 * @tc.number   Telephony_CallStateListener_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallStateListener_001, Function | MediumTest | Level3)
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
HWTEST_F(ZeroBranch5Test, Telephony_ConferenceBase_001, Function | MediumTest | Level3)
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
HWTEST_F(ZeroBranch5Test, Telephony_CallAbilityReportProxy_001, Function | MediumTest | Level3)
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
    sptr<IRemoteObject> object = nullptr;
    ASSERT_NE(callAbilityReportProxy->UnRegisterCallBack(object), TELEPHONY_SUCCESS);
    sptr<CallBase> callObjectPtr = nullptr;
    callAbilityReportProxy->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    callAbilityCallbackPtr = new CallAbilityCallback();
    ASSERT_EQ(callAbilityReportProxy->RegisterCallBack(callAbilityCallbackPtr, pidName), TELEPHONY_SUCCESS);
    DisconnectedDetails disconnectedDetails;
    callAbilityReportProxy->CallDestroyed(disconnectedDetails);
    CallAttributeInfo callAttributeInfo;
    callAbilityReportProxy->ReportCallStateInfo(callAttributeInfo);
    callAbilityReportProxy->ReportCallStateInfo(callAttributeInfo, pidName);
    CallEventInfo callEventInfo;
    callAbilityReportProxy->CallEventUpdated(callEventInfo);
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
}

/**
 * @tc.number   Telephony_CallAbilityReportProxy_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallAbilityReportProxy_002, Function | MediumTest | Level3)
{
    CallAbilityConnectCallback callAbilityConnectCallback;
    callAbilityConnectCallback.ReConnectAbility();
    if (CallObjectManager::HasCallExist()) {
        callAbilityConnectCallback.ReConnectAbility();
    }
    CallConnectAbility callConnectCalluiAbility;
    callConnectCalluiAbility.WaitForConnectResult();
    std::shared_ptr<CallAbilityReportProxy> callAbilityReportProxy = std::make_shared<CallAbilityReportProxy>();
    ASSERT_NE(callAbilityReportProxy, nullptr);
    int64_t dataUsage = 0;
    std::string pidName = "123";
    ASSERT_NE(callAbilityReportProxy->ReportCallDataUsageChange(dataUsage), TELEPHONY_SUCCESS);
    CameraCapabilities cameraCapabilities;
    ASSERT_NE(callAbilityReportProxy->ReportCameraCapabilities(cameraCapabilities), TELEPHONY_SUCCESS);
    ASSERT_NE(callAbilityReportProxy->UnRegisterCallBack(pidName), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallAbilityConnectCallback_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallAbilityConnectCallback_001, Function | MediumTest | Level3)
{
    CallAbilityConnectCallback callAbilityConnectCallback;
    callAbilityConnectCallback.ReConnectAbility();
    if (CallObjectManager::HasCallExist()) {
        callAbilityConnectCallback.ReConnectAbility();
    }
    std::string bundle = "111";
    std::string ability = "222";
    AppExecFwk::ElementName element("", bundle, ability);
    int resultCode = 0;
    callAbilityConnectCallback.OnAbilityConnectDone(element, nullptr, resultCode);
    ASSERT_EQ(resultCode, 0);
    callAbilityConnectCallback.OnAbilityDisconnectDone(element, resultCode);
    ASSERT_EQ(resultCode, 0);
}

/**
 * @tc.number   Telephony_AutoAnswerState_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_AutoAnswerState_001, Function | MediumTest | Level3)
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
HWTEST_F(ZeroBranch5Test, Telephony_CanUnHoldState_001, Function | MediumTest | Level3)
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
HWTEST_F(ZeroBranch5Test, Telephony_SatelliteCall_001, Function | MediumTest | Level3)
{
    DialParaInfo dialParaInfo;
    SatelliteCall call { dialParaInfo };
    int32_t ret1 = call.DialingProcess();
    int32_t ret2 = call.AnswerCall(0);
    int32_t ret3 = call.RejectCall();
    int32_t ret4 = call.HangUpCall();
    CallAttributeInfo callAttributeInfo;
    call.GetCallAttributeInfo(callAttributeInfo);
    EXPECT_EQ(ret1, TELEPHONY_SUCCESS);
    EXPECT_NE(ret2, TELEPHONY_SUCCESS);
    EXPECT_NE(ret3, TELEPHONY_SUCCESS);
    EXPECT_NE(ret4, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_VoipCallManagerProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_VoipCallManagerProxy_001, Function | MediumTest | Level3)
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
        int32_t ret1 = voipCallManagerInterfacePtr->ReportIncomingCall(mPacMap, userProfile, error);
        int32_t ret2 = voipCallManagerInterfacePtr->ReportIncomingCallError(mPacMap);
        VoipCallState voipCallState = VoipCallState::VOIP_CALL_STATE_ACTIVE;
        std::string callId = "123";
        int32_t ret3 = voipCallManagerInterfacePtr->ReportCallStateChange(callId, voipCallState);
        int32_t ret4 = voipCallManagerInterfacePtr->UnRegisterCallBack();
        std::string bundleName = " ";
        std::string processMode = "0";
        int32_t uid = 0;
        int32_t ret5 = voipCallManagerInterfacePtr->ReportVoipIncomingCall(callId, bundleName, processMode, uid);
        std::string extensionId = " ";
        int32_t ret6 = voipCallManagerInterfacePtr->ReportVoipCallExtensionId(callId, bundleName, extensionId, uid);
        VoipCallEventInfo voipCallEventInfo;
        voipCallEventInfo.voipCallId = "123";
        int32_t ret7 = voipCallManagerInterfacePtr->Answer(voipCallEventInfo,
            static_cast<int32_t>(VideoStateType::TYPE_VOICE));
        int32_t ret8 = voipCallManagerInterfacePtr->HangUp(voipCallEventInfo);
        int32_t ret9 = voipCallManagerInterfacePtr->Reject(voipCallEventInfo);
        int32_t ret10 = voipCallManagerInterfacePtr->UnloadVoipSa();
        sptr<ICallStatusCallback> statusCallback = (std::make_unique<CallStatusCallback>()).release();
        int32_t ret11 = voipCallManagerInterfacePtr->RegisterCallManagerCallBack(statusCallback);
        int32_t ret12 = voipCallManagerInterfacePtr->UnRegisterCallManagerCallBack();
        EXPECT_NE(ret1, TELEPHONY_SUCCESS);
        EXPECT_NE(ret2, TELEPHONY_SUCCESS);
        EXPECT_NE(ret3, TELEPHONY_SUCCESS);
        EXPECT_NE(ret4, TELEPHONY_SUCCESS);
        EXPECT_NE(ret5, TELEPHONY_SUCCESS);
        EXPECT_NE(ret6, TELEPHONY_SUCCESS);
        EXPECT_NE(ret7, TELEPHONY_SUCCESS);
        EXPECT_NE(ret8, TELEPHONY_SUCCESS);
        EXPECT_NE(ret9, TELEPHONY_SUCCESS);
        EXPECT_NE(ret10, TELEPHONY_SUCCESS);
        EXPECT_NE(ret11, TELEPHONY_SUCCESS);
        EXPECT_NE(ret12, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   Telephony_CallRequestEventHandlerHelper_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallRequestEventHandlerHelper_001, Function | MediumTest | Level3)
{
    bool flag = false;
    DelayedSingleton<CallRequestEventHandlerHelper>::GetInstance()->RestoreDialingFlag(flag);
    EXPECT_FALSE(DelayedSingleton<CallRequestEventHandlerHelper>::GetInstance()->IsDialingCallProcessing());
}

/**
 * @tc.number   Telephony_DistributedCallManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_DistributedCallManager_001, Function | MediumTest | Level3)
{
    DistributedCallManager manager;
    AudioDevice device;
    device.deviceType = AudioDeviceType::DEVICE_EARPIECE;
    std::string restr = manager.GetDevIdFromAudioDevice(device);
    bool res1 = manager.SwitchOnDCallDeviceSync(device);
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
    manager.SwitchOnDCallDeviceSync(device);
    manager.OnDCallSystemAbilityAdded(TEST_STR);
    EXPECT_TRUE(restr.empty());
    EXPECT_FALSE(res1);
}

/**
 * @tc.number   Telephony_DistributedCallProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_DistributedCallProxy_001, Function | MediumTest | Level3)
{
    std::unique_ptr<DistributedCallProxy> proxy = std::make_unique<DistributedCallProxy>();
    proxy->GetDCallClient();
    int32_t res = proxy->Init();
    proxy->GetDCallClient();
    proxy->dcallClient_ = nullptr;
    int32_t res1 = proxy->UnInit();
    int32_t res3 = proxy->SwitchDevice(TEST_STR, 1);
    std::vector<std::string> devList;
    int32_t res4 = proxy->GetOnlineDeviceList(devList);
    OHOS::DistributedHardware::DCallDeviceInfo devInfo;
    int32_t res5 = proxy->GetDCallDeviceInfo(TEST_STR, devInfo);
    proxy->GetDCallClient();
    int32_t res6 = proxy->SwitchDevice(TEST_STR, 1);
    int32_t res7 = proxy->GetOnlineDeviceList(devList);
    proxy->GetDCallDeviceInfo(TEST_STR, devInfo);
    int32_t res8 = proxy->UnInit();
    EXPECT_NE(res, TELEPHONY_SUCCESS);
    EXPECT_NE(res1, TELEPHONY_SUCCESS);
    EXPECT_NE(res3, TELEPHONY_SUCCESS);
    EXPECT_NE(res4, TELEPHONY_SUCCESS);
    EXPECT_NE(res5, TELEPHONY_SUCCESS);
    EXPECT_NE(res6, TELEPHONY_SUCCESS);
    EXPECT_NE(res7, TELEPHONY_SUCCESS);
    EXPECT_NE(res8, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_BluetoothCallManager_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_BluetoothCallManager_001, Function | MediumTest | Level3)
{
    int32_t state = 0;
    int32_t numActive = CallObjectManager::GetCallNum(TelCallState::CALL_STATUS_ACTIVE);
    int32_t numHeld = CallObjectManager::GetCallNum(TelCallState::CALL_STATUS_HOLDING);
    int32_t numDial = CallObjectManager::GetCallNum(TelCallState::CALL_STATUS_DIALING);
    int32_t callState = static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE);
    std::string number = CallObjectManager::GetCallNumber(TelCallState::CALL_STATUS_HOLDING);
    BluetoothCallManager bluetoothCallManager;
    ASSERT_EQ(bluetoothCallManager.SendBtCallState(numActive, numHeld, callState, number), TELEPHONY_SUCCESS);
    ASSERT_EQ(bluetoothCallManager.SendCallDetailsChange(1, 1), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallManager.IsBtAvailble(), true);
}

/**
 * @tc.number   Telephony_CallAbilityCallbackDeathRecipient_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallAbilityCallbackDeathRecipient_001, Function | MediumTest | Level3)
{
    CallAbilityCallbackDeathRecipient recipient;
    OHOS::wptr<OHOS::IRemoteObject> object;
    recipient.OnRemoteDied(object);
    ASSERT_EQ(object, nullptr);
}

/**
 * @tc.number   Telephony_ApplicationStateObserver_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_ApplicationStateObserver_001, Function | MediumTest | Level3)
{
    ApplicationStateObserver applicationStateObserver;
    AppExecFwk::ProcessData processData;
    applicationStateObserver.OnProcessDied(processData);
    ASSERT_NE(sizeof(processData), 0);
}

/**
 * @tc.number   Telephony_CallAbilityCallbackProxy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallAbilityCallbackProxy_001, Function | MediumTest | Level3)
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

    AudioDeviceInfo audioDeviceInfo;
    callAbilityCallbackProxy.OnReportAudioDeviceChange(audioDeviceInfo);
    const std::string str {"123"};
    callAbilityCallbackProxy.OnReportPostDialDelay(str);
    ASSERT_TRUE(!str.empty());
}

/**
 * @tc.number   Telephony_OTTCallConnection_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_OTTCallConnection_001, Function | MediumTest | Level3)
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
    int32_t result = ott.InviteToConference(requestInfo, numberList);
    ott.UpdateImsCallMode(requestInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY);
    AppExecFwk::PacMap info;
    ott.PackCellularCallInfo(requestInfo, info);
    ASSERT_NE(result, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_SuperPrivacyManagerClient_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_SuperPrivacyManagerClient_001, Function | MediumTest | Level3)
{
    int32_t privacy = SuperPrivacyManagerClient::GetInstance().
        SetSuperPrivacyMode(static_cast<int32_t>(CallSuperPrivacyModeType::OFF), SOURCE_CALL);
#ifdef CALL_MANAGER_AUTO_START_OPTIMIZE
    ASSERT_EQ(privacy, 22);
#else
    ASSERT_EQ(privacy, 0);
#endif
}

/**
 * @tc.number   Telephony_CallStatusCallback_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallStatusCallback_001, Function | MediumTest | Level3)
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
    int32_t res = callStatusCallback->CloseUnFinishedUssdResult(result);
    ASSERT_NE(res, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStatusCallback_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallStatusCallback_002, Function | MediumTest | Level3)
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
    res = callStatusCallback->UpdateVoipEventInfo(voipCallEventInfo);
    ASSERT_NE(res, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStatusCallbackStub_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallStatusCallbackStub_001, Function | MediumTest | Level3)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_CALL_INFO),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_CALLS_INFO),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_DISCONNECTED_CAUSE),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_EVENT_RESULT_INFO),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_RBT_PLAY_INFO),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_GET_WAITING),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_SET_WAITING),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_GET_RESTRICTION),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_SET_RESTRICTION),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_GET_TRANSFER),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_SET_TRANSFER),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_GET_CALL_CLIP),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_GET_CALL_CLIR),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_SET_CALL_CLIR),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::GET_IMS_SWITCH_STATUS),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::SET_IMS_SWITCH_STATUS),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_STARTRTT_STATUS),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_STOPRTT_STATUS),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::GET_IMS_CONFIG),
        data, reply, option);
    int32_t res =  callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::SET_IMS_CONFIG),
        data, reply, option);
    ASSERT_NE(res, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStatusCallbackStub_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallStatusCallbackStub_002, Function | MediumTest | Level3)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::GET_IMS_FEATURE_VALUE),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::SET_IMS_FEATURE_VALUE),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::GET_LTE_ENHANCE_MODE),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::SET_LTE_ENHANCE_MODE),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallStatusInterfaceCode::RECEIVE_UPDATE_MEDIA_MODE_RESPONSE), data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::INVITE_TO_CONFERENCE),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::START_DTMF),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::STOP_DTMF),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::SEND_USSD),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::GET_IMS_CALL_DATA),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::MMI_CODE_INFO_RESPONSE),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::CLOSE_UNFINISHED_USSD),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_SET_RESTRICTION_PWD),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::POST_DIAL_CHAR),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::POST_DIAL_DELAY),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::CALL_SESSION_EVENT),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::PEER_DIMENSION_CHANGE),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::CALL_DATA_USAGE),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::CAMERA_CAPBILITIES_CHANGE),
        data, reply, option);
    callStatusCallback->OnRemoteRequest(static_cast<uint32_t>(
        CallStatusInterfaceCode::RECEIVE_UPDATE_MEDIA_MODE_REQUEST), data, reply, option);
    int32_t res = callStatusCallback->OnRemoteRequest(
        static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_VOIP_EVENT_INFO), data, reply, option);
    ASSERT_NE(res, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStatusCallbackStub_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch5Test, Telephony_CallStatusCallbackStub_003, Function | MediumTest | Level3)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel data;
    MessageParcel reply;
    int32_t accountId = 0;
    int32_t defaultNumber = 0;
    std::string defaultString = "";
    std::vector<uint8_t> userProfile = {};
    char accountNum[kMaxNumberLen + 1] = { 0 };
    data.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    data.WriteInt32(0);
    data.WriteCString(accountNum);
    data.WriteInt32(accountId);
    data.WriteInt32(static_cast<int32_t>(CallType::TYPE_VOIP));
    data.WriteInt32(static_cast<int32_t>(VideoStateType::TYPE_VOICE));
    data.WriteInt32(static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
    data.WriteInt32(defaultNumber);
    data.WriteInt32(defaultNumber);
    data.WriteInt32(defaultNumber);
    data.WriteInt32(defaultNumber);
    data.WriteString(defaultString);
    data.WriteString(defaultString);
    data.WriteString(defaultString);
    data.WriteString(defaultString);
    data.WriteString(defaultString);
    data.WriteUInt8Vector(userProfile);
    data.RewindRead(0);
    ASSERT_EQ(callStatusCallback->OnUpdateCallReportInfo(data, reply), TELEPHONY_SUCCESS);
}
} // namespace Telephony
} // namespace OHOS
