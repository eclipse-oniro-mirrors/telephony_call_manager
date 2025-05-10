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

class ZeroBranch6Test : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void ZeroBranch6Test::SetUp() {}

void ZeroBranch6Test::TearDown() {}

void ZeroBranch6Test::SetUpTestCase()
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
        .processName = "ZeroBranch6Test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void ZeroBranch6Test::TearDownTestCase() {}

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
 * @tc.number   Telephony_CallStatusCallbackStub_004
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch6Test, Telephony_CallStatusCallbackStub_004, TestSize.Level0)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel data;
    data.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    MessageParcel reply;
    int32_t vecSize = 1;
    int32_t accountId = 0;
    int32_t defaultNumber = 0;
    std::string defaultString = "";
    std::vector<uint8_t> userProfile = {};
    char accountNum[kMaxNumberLen + 1] = { 0 };
    data.WriteInt32(vecSize);
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
    data.WriteInt32(SIM1_SLOTID);
    data.RewindRead(0);
    callStatusCallback->OnUpdateCallsReportInfo(data, reply);

    MessageParcel messageParcel;
    messageParcel.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    CellularCallEventInfo info;
    int32_t length = sizeof(CellularCallEventInfo);
    info.eventType = CellularCallEventType::EVENT_REQUEST_RESULT_TYPE;
    info.eventId = RequestResultEventId::INVALID_REQUEST_RESULT_EVENT_ID;
    messageParcel.WriteInt32(length);
    messageParcel.WriteRawData((const void *)&info, length);
    messageParcel.RewindRead(0);
    MessageParcel reply1;
    ASSERT_NE(callStatusCallback->OnUpdateEventReport(messageParcel, reply1), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStatusCallbackStub_005
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch6Test, Telephony_CallStatusCallbackStub_005, TestSize.Level0)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel data;
    data.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    int32_t defaultNumber = 0;
    MessageParcel reply;
    CallWaitResponse callWaitResponse;
    int32_t length = sizeof(CallWaitResponse);
    data.WriteInt32(length);
    callWaitResponse.result = defaultNumber;
    callWaitResponse.status = defaultNumber;
    callWaitResponse.classCw = defaultNumber;
    data.WriteRawData((const void *)&callWaitResponse, length);
    data.RewindRead(0);
    callStatusCallback->OnUpdateGetWaitingResult(data, reply);
    MessageParcel messageParcel;
    messageParcel.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    CallRestrictionResponse callRestrictionResult;
    length = sizeof(CallRestrictionResponse);
    messageParcel.WriteInt32(length);
    callRestrictionResult.result = 0;
    callRestrictionResult.status = 0;
    callRestrictionResult.classCw = 0;
    messageParcel.WriteRawData((const void *)&callRestrictionResult, length);
    messageParcel.RewindRead(0);
    MessageParcel reply1;
    callStatusCallback->OnUpdateGetRestrictionResult(messageParcel, reply1);
    MessageParcel dataParcel2;
    dataParcel2.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    CallTransferResponse callTransferResponse;
    length = sizeof(CallTransferResponse);
    messageParcel.WriteInt32(length);
    callTransferResponse.result = defaultNumber;
    callTransferResponse.status = defaultNumber;
    callTransferResponse.classx = defaultNumber;
    callTransferResponse.type = defaultNumber;
    std::string msg("hello");
    int32_t accountLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(callTransferResponse.number, kMaxNumberLen, msg.c_str(), accountLength);
    callTransferResponse.reason = defaultNumber;
    callTransferResponse.time = defaultNumber;
    callTransferResponse.startHour = defaultNumber;
    callTransferResponse.startMinute = defaultNumber;
    callTransferResponse.endHour = defaultNumber;
    callTransferResponse.endMinute = defaultNumber;
    dataParcel2.WriteRawData((const void *)&callTransferResponse, length);
    dataParcel2.RewindRead(0);
    MessageParcel reply2;
    ASSERT_NE(callStatusCallback->OnUpdateGetTransferResult(dataParcel2, reply2), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStatusCallbackStub_006
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch6Test, Telephony_CallStatusCallbackStub_006, TestSize.Level0)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel messageParcel;
    messageParcel.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    MessageParcel reply;
    ClipResponse clipResponse;
    int32_t length = sizeof(ClipResponse);
    messageParcel.WriteInt32(length);
    messageParcel.WriteRawData((const void *)&clipResponse, length);
    messageParcel.RewindRead(0);
    callStatusCallback->OnUpdateGetCallClipResult(messageParcel, reply);

    MessageParcel dataParcel2;
    dataParcel2.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    GetImsConfigResponse response;
    length = sizeof(GetImsConfigResponse);
    dataParcel2.WriteInt32(length);
    dataParcel2.WriteRawData((const void *)&response, length);
    dataParcel2.RewindRead(0);
    MessageParcel reply2;
    callStatusCallback->OnGetImsConfigResult(dataParcel2, reply2);

    MessageParcel dataParcel3;
    dataParcel3.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    GetImsFeatureValueResponse response1;
    length = sizeof(GetImsFeatureValueResponse);
    dataParcel3.WriteInt32(length);
    dataParcel3.WriteRawData((const void *)&response1, length);
    dataParcel3.RewindRead(0);
    MessageParcel reply3;
    callStatusCallback->OnGetImsFeatureValueResult(dataParcel3, reply3);

    MessageParcel dataParcel4;
    dataParcel4.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    MmiCodeInfo info;
    length = sizeof(MmiCodeInfo);
    dataParcel4.WriteInt32(length);
    info.result = 0;
    std::string msg("hello");
    int32_t msgLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(info.message, kMaxNumberLen, msg.c_str(), msgLength);
    dataParcel4.WriteRawData((const void *)&info, length);
    dataParcel4.RewindRead(0);
    ASSERT_NE(callStatusCallback->OnSendMmiCodeResult(dataParcel4, reply), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStatusCallbackStub_007
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch6Test, Telephony_CallStatusCallbackStub_007, TestSize.Level0)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel messageParcel;
    messageParcel.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    int32_t length = sizeof(CallModeReportInfo);
    messageParcel.WriteInt32(length);
    CallModeReportInfo callModeReportInfo;
    messageParcel.WriteRawData((const void *)&callModeReportInfo, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    callStatusCallback->OnReceiveImsCallModeRequest(messageParcel, reply);

    MessageParcel dataParcel2;
    dataParcel2.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    length = sizeof(CallSessionReportInfo);
    dataParcel2.WriteInt32(length);
    CallSessionReportInfo callSessionReportInfo;
    dataParcel2.WriteRawData((const void *)&callSessionReportInfo, length);
    dataParcel2.RewindRead(0);
    MessageParcel reply2;
    callStatusCallback->OnCallSessionEventChange(dataParcel2, reply2);

    MessageParcel dataParcel3;
    dataParcel3.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    length = sizeof(PeerDimensionsReportInfo);
    dataParcel3.WriteInt32(length);
    PeerDimensionsReportInfo dimensionsReportInfo;
    dataParcel3.WriteRawData((const void *)&dimensionsReportInfo, length);
    dataParcel3.RewindRead(0);
    MessageParcel reply3;
    callStatusCallback->OnPeerDimensionsChange(dataParcel3, reply3);

    MessageParcel dataParcel4;
    dataParcel4.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    int64_t reportInfo = static_cast<int64_t>(10);
    dataParcel4.WriteInt64(reportInfo);
    dataParcel4.RewindRead(0);
    MessageParcel reply4;
    callStatusCallback->OnCallDataUsageChange(dataParcel4, reply4);

    MessageParcel dataParcel5;
    dataParcel5.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    length = sizeof(CameraCapabilitiesReportInfo);
    dataParcel5.WriteInt32(length);
    CameraCapabilitiesReportInfo cameraCapabilitiesReportInfo;
    dataParcel5.WriteRawData((const void *)&cameraCapabilitiesReportInfo, length);
    dataParcel5.RewindRead(0);
    MessageParcel reply5;
    ASSERT_NE(callStatusCallback->OnCameraCapabilitiesChange(dataParcel5, reply5), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_SatelliteCallControl_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch6Test, Telephony_SatelliteCallControl_001, TestSize.Level0)
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
    ASSERT_EQ(isShowDialog, true);
}

/**
 * @tc.number   Telephony_ProximitySensor_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch6Test, Telephony_ProximitySensor_001, TestSize.Level0)
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
    ASSERT_NE(proximitySensor, nullptr);
}

/**
 * @tc.number   Telephony_StatusBar_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch6Test, Telephony_StatusBar_001, TestSize.Level0)
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
    ASSERT_NE(statusBar, nullptr);
}

/**
 * @tc.number   Telephony_WiredHeadset_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch6Test, Telephony_WiredHeadset_001, TestSize.Level0)
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
    ASSERT_NE(wiredHeadset, nullptr);
}

/**
 * @tc.number   Telephony_CallStatusPolicy_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch6Test, Telephony_CallStatusPolicy_001, TestSize.Level0)
{
    CallStatusPolicy callStatusPolicy;
    CallDetailInfo info;
    callStatusPolicy.DialingHandlePolicy(info);
    int32_t res = callStatusPolicy.FilterResultsDispose(nullptr);
    ASSERT_NE(res, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallAbilityCallback_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch6Test, Telephony_CallAbilityCallback_002, TestSize.Level0)
{
    auto callAbilityCallback = std::make_shared<CallAbilityCallback>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    callAbilityCallback->OnRemoteRequest(static_cast<uint32_t>(CallStatusInterfaceCode::UPDATE_VOIP_EVENT_INFO),
        data, reply, option);
    callAbilityCallback->OnUpdateCallStateInfo(data, reply);
    callAbilityCallback->OnUpdateMeeTimeStateInfo(data, reply);
    callAbilityCallback->OnUpdateCallEvent(data, reply);
    callAbilityCallback->OnUpdateCallDisconnectedCause(data, reply);
    callAbilityCallback->OnUpdateAysncResults(data, reply);
    callAbilityCallback->OnUpdateMmiCodeResults(data, reply);
    callAbilityCallback->OnUpdateAudioDeviceChange(data, reply);
    callAbilityCallback->OnUpdateOttCallRequest(data, reply);
    callAbilityCallback->OnUpdatePostDialDelay(data, reply);
    callAbilityCallback->OnUpdateImsCallModeChange(data, reply);
    callAbilityCallback->OnUpdateCallSessionEventChange(data, reply);
    callAbilityCallback->OnUpdatePeerDimensionsChange(data, reply);
    callAbilityCallback->OnUpdateCallDataUsageChange(data, reply);
    ASSERT_NE(callAbilityCallback->OnUpdateCameraCapabilities(data, reply), TELEPHONY_SUCCESS);
}
} // namespace Telephony
} // namespace OHOS
