/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <cstring>
#include <string>

#include "napi_util.h"
#include "ims_rtt_errcode.h"
#include "ims_rtt_manager.h"
#include "napi_call_manager.h"
#include "call_manager_errors.h"
#include "call_manager_service.h"
#include "call_status_callback.h"
#include "call_manager_service_stub.h"
#include "napi_call_ability_callback.h"
#include "call_ability_callback_stub.h"
#include "rtt_call_listener.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class RttCallTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class MockCallAbilityCallbackStub : public CallAbilityCallbackStub {
    int32_t OnCallDetailsChange(const CallAttributeInfo &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnMeeTimeDetailsChange(const CallAttributeInfo &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallEventChange(const CallEventInfo &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallDisconnectedCause(const DisconnectedDetails &details)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportMmiCodeResult(const MmiCodeInfo &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportAudioDeviceChange(const AudioDeviceInfo &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportPostDialDelay(const std::string &str)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportCallSessionEventChange(const CallSessionEvent &callSessionEventOptions)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportCallDataUsageChange(const int64_t dataUsage)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportCameraCapabilities(const CameraCapabilities &cameraCapabilities)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnPhoneStateChange(int32_t numActive, int32_t numHeld, int32_t callState,
        const std::string &number)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportRttCallEvtChanged(const RttEvent &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportRttCallError(const RttError &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportRttCallMessage(AppExecFwk::PacMap &msgResult)
    {
        return TELEPHONY_SUCCESS;
    }
};

/**
 * @tc.number   Telephony_NapiCallAbilityCallbackTest001
 * @tc.name     test NapiCallAbilityCallback normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_NapiCallAbilityCallbackTest001, Function | MediumTest | Level1)
{
    NapiCallAbilityCallback callback;
    EventCallback eventCallback;
    (void)memset_s(&eventCallback, sizeof(EventCallback), 0, sizeof(EventCallback));
    EXPECT_NO_THROW(callback.RegisterRttModifyIndCallback(eventCallback));
    EXPECT_NO_THROW(callback.RegisterRttErrCauseCallback(eventCallback));
    EXPECT_NO_THROW(callback.RegisterRttCallMessageCallback(eventCallback));
    EXPECT_EQ(callback.rttModifyIndCallback_.callbackBeginTime_, eventCallback.callbackBeginTime_);
    EXPECT_EQ(callback.rttErrCauseCallback_.callbackBeginTime_, eventCallback.callbackBeginTime_);
    EXPECT_EQ(callback.rttCallMessageCallback_.callbackBeginTime_, eventCallback.callbackBeginTime_);
    EXPECT_NO_THROW(callback.UnRegisterRttModifyIndCallback());
    EXPECT_NO_THROW(callback.UnRegisterRttErrCauseCallback());
    EXPECT_NO_THROW(callback.UnRegisterRttCallMessageCallback());

    RttEvent eventInfo;
    EXPECT_EQ(callback.ReportRttCallEventInfo(eventInfo), CALL_ERR_CALLBACK_NOT_EXIST);
    EXPECT_EQ(callback.ReportRttCallEventInfo(eventInfo), CALL_ERR_CALLBACK_NOT_EXIST);

    uv_work_t *work = std::make_unique<uv_work_t>().release();
    int32_t status = 1;
    RttCallEvtWork *rttCallEvtwork = std::make_unique<RttCallEvtWork>().release();
    work->data = nullptr;
    callback.ReportRttCallEventInfoWork(work, status); // work->data is nullptr
    work->data = (void *)rttCallEvtwork;
    callback.ReportRttCallEventInfoWork(work, status);

    RttError errorInfo;
    EXPECT_EQ(eventCallback.env, nullptr);
    EXPECT_NE(callback.ReportRttCallErrorInfo(errorInfo, eventCallback), TELEPHONY_SUCCESS);

    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("info_one", 1);
    resultInfo.PutIntValue("info_two", 2);
    EXPECT_EQ(callback.ReportRttCallMessageInfo(resultInfo), CALL_ERR_CALLBACK_NOT_EXIST);
}

/**
 * @tc.number   Telephony_NapiCallAbilityCallbackTest002
 * @tc.name     test NapiCallAbilityCallback normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_NapiCallAbilityCallbackTest002, Function | MediumTest | Level1)
{
    NapiCallAbilityCallback callback;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    int32_t status = 0;
    work->data = nullptr;
    EXPECT_NO_THROW(callback.ReportRttCallMessageWork(work, status));

    CallSupplementWorker *callSupplementWorker = std::make_unique<CallSupplementWorker>().release();
    work->data = (void *)callSupplementWorker;
    EXPECT_NO_THROW(callback.ReportRttCallMessageWork(work, status));

    AppExecFwk::PacMap resultInfo;
    EventCallback eventCallback;
    resultInfo.PutIntValue("info_one", 1);
    EXPECT_EQ(callback.ReportRttCallMessage(resultInfo, eventCallback), TELEPHONY_ERROR);
}

/**
 * @tc.number   Telephony_NapiCallManagerTest001
 * @tc.name     test NapiCallManager normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_NapiCallManagerTest001, Function | MediumTest | Level1)
{
    NapiCallManager callManager;
    std::string tmpStr = "callDetailsChange";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "callEventChange";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "callOttRequest";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "callDisconnectedCause";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "mmiCodeResult";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "audioDeviceChange";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "postDialDelay";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "imsCallModeChange";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "callSessionEvent";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "peerDimensionsChange";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "cameraCapabilitiesChange";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "rttCallEvtChange";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));

    tmpStr = "rttCallErrCause";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));
    tmpStr = "sendRttMessageToInCall";
    EXPECT_NO_THROW(callManager.UnRegisterCallbackWithListenerType(tmpStr));
}

/**
 * @tc.number   Telephony_NapiCallManagerTest002
 * @tc.name     test NapiCallManager normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_NapiCallManagerTest002, Function | MediumTest | Level1)
{
    NapiCallManager callManager;
    napi_env env = nullptr;
    napi_value exports = nullptr;
    napi_callback_info info = nullptr;
    napi_value errorCode;
    napi_create_int32(env, -100, &errorCode);

#ifdef SUPPORT_RTT_CALL
    EXPECT_EQ(callManager.StartRtt(env, info), errorCode);
    EXPECT_EQ(callManager.StopRtt(env, info), errorCode);
#endif
    EXPECT_EQ(callManager.AnswerCall(env, info), errorCode);
    EXPECT_EQ(callManager.DeclareCallImsInterface(env, exports), errorCode);
    EXPECT_EQ(callManager.JoinConference(env, info), errorCode);

    DialAsyncContext *dialContext = std::make_unique<DialAsyncContext>().release();
    ASSERT_NE(dialContext, nullptr);
    dialContext->accountId = -1;
    EXPECT_NO_THROW(callManager.NativeDialCall(env, (void *)dialContext));
    dialContext->accountId = -2;
    EXPECT_NO_THROW(callManager.NativeDialCall(env, (void *)dialContext));

#ifdef SUPPORT_RTT_CALL
    SupplementAsyncContext *startRTTContext = std::make_unique<SupplementAsyncContext>().release();
    ASSERT_NE(startRTTContext, nullptr);
    EXPECT_NO_THROW(callManager.NativeStartRtt(env, (void *)startRTTContext));
    EXPECT_NO_THROW(callManager.NativeStopRtt(env, (void *)startRTTContext));
#endif

    EXPECT_EQ(callManager.SetRttCapability(env, info), errorCode);

    SetRttCapabilityAsyncContext *asyncContext = std::make_unique<SetRttCapabilityAsyncContext>().release();
    ASSERT_NE(asyncContext, nullptr);
    asyncContext->accountId = 1;
    EXPECT_NO_THROW(callManager.NativeSetRttCapability(env, (void *)asyncContext));
    asyncContext->accountId = -1;
    EXPECT_NO_THROW(callManager.NativeSetRttCapability(env, (void *)asyncContext));

    EXPECT_EQ(callManager.SendRttMessage(env, info), errorCode);

    RttAsyncContext *sendRttMsg = std::make_unique<RttAsyncContext>().release();
    ASSERT_NE(sendRttMsg, nullptr);
    EXPECT_NO_THROW(callManager.NativeSendRttMessage(env, (void *)sendRttMsg));
}

/**
 * @tc.number   Telephony_NapiCallManagerCallbackTest001
 * @tc.name     test NapiCallManagerCallback normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_NapiCallManagerCallbackTest001, Function | MediumTest | Level1)
{
    RttEvent info;
    RttError errorInfo;
    int32_t errorCode = -100;
    NapiCallManagerCallback callback;
    EXPECT_NE(callback.OnReportRttCallEvtChanged(info), errorCode);
    EXPECT_NE(callback.OnReportRttCallError(errorInfo), errorCode);

    AppExecFwk::PacMap msgResult;
    msgResult.PutIntValue("info_one", 1);
    EXPECT_NE(callback.OnReportRttCallMessage(msgResult), errorCode);
}

/**
 * @tc.number   Telephony_CallAbilityCallbackStubTest001
 * @tc.name     test CallAbilityCallbackStub normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_CallAbilityCallbackStubTest001, Function | MediumTest | Level1)
{
    std::shared_ptr<CallAbilityCallbackStub> callbackStub = std::make_shared<MockCallAbilityCallbackStub>();
    MessageParcel reply;

    MessageParcel data_one;
    data_one.WriteInt32(1); // callID
    data_one.WriteInt32(2); // evnetType
    data_one.WriteInt32(1); // reason
    EXPECT_EQ(callbackStub->OnUpdateRttCallEvent(data_one, reply), TELEPHONY_SUCCESS);

    MessageParcel data_two;
    data_two.WriteInt32(2); // callID
    data_two.WriteInt32(1); // causeCode
    data_two.WriteInt32(0); // opeartionType
    data_two.WriteString("reasonText"); // reasonText
    EXPECT_EQ(callbackStub->OnUpdateRttCallErrorReport(data_two, reply), TELEPHONY_SUCCESS);

    MessageParcel data_three;
    data_three.WriteInt32(0); // call id
    data_three.WriteString("rttMessage"); // rttMessage
    EXPECT_EQ(callbackStub->OnUpdateRttCallMessage(data_three, reply), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_ImsRttManagerTest001
 * @tc.name     test ImsRttManager normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_ImsRttManagerTest001, Function | MediumTest | Level1)
{
    int32_t ret = -100;
    ImsRttManager manager(1, 1);
    manager.devFd_ = 1;
    EXPECT_EQ(manager.InitRtt(), RTT_SUCCESS);
    manager.devFd_ = 0;
    EXPECT_NE(manager.InitRtt(), ret);

    int32_t len = 0;
    std::string sendMessage = "message";
    manager.sendStream_ = "message_message";
    EXPECT_NO_THROW(manager.ReadStreamData(len, sendMessage));
    len = static_cast<int32_t>(sendMessage.size());
    EXPECT_NO_THROW(manager.ReadStreamData(len, sendMessage));

    manager.devFd_ = PROXY_IS_OFF;
    EXPECT_EQ(manager.SendDataToProxy(sendMessage), RTT_ERR_PROXY_CLOSED);
    manager.devFd_ = PROXY_IS_OFF + 2;
    EXPECT_NE(manager.SendDataToProxy(sendMessage), ret);
}

/**
 * @tc.number   Telephony_ImsRttManagerTest002
 * @tc.name     test ImsRttManager normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_ImsRttManagerTest002, Function | MediumTest | Level1)
{
    ImsRttManager manager(1, 1);
    std::string reportMessage = "message";
    EXPECT_NO_THROW(manager.ReportRecvMessage(reportMessage));

    manager.devFd_ = PROXY_IS_OFF;
    EXPECT_NO_THROW(manager.RecvDataFromProxy(reportMessage));
    manager.devFd_ = PROXY_IS_OFF + 1;
    EXPECT_NO_THROW(manager.RecvDataFromProxy(reportMessage));

    manager.sendThread_ = std::make_unique<ffrt::thread>([]() {});
    manager.recvThread_ = std::make_unique<ffrt::thread>([]() {});
    manager.sendThreadActive_ = true;
    manager.recvThreadActive_ = true;
    EXPECT_NO_THROW(manager.DestroyRtt());
    EXPECT_FALSE(manager.sendThreadActive_);
    EXPECT_FALSE(manager.recvThreadActive_);
    EXPECT_EQ(manager.sendThread_, nullptr);
    EXPECT_EQ(manager.recvThread_, nullptr);

    manager.sendThread_ = std::make_unique<ffrt::thread>([]() {});
    manager.sendThread_->detach();
    manager.recvThread_ = std::make_unique<ffrt::thread>([]() {});
    manager.recvThread_->detach();
    EXPECT_NO_THROW(manager.DestroyRtt());
    EXPECT_EQ(manager.sendThread_, nullptr);
    EXPECT_EQ(manager.recvThread_, nullptr);

    uint16_t channelId = 1;
    int32_t callId = 1;
    EXPECT_NO_THROW(manager.SetChannelID(channelId));
    EXPECT_NO_THROW(manager.SetCallID(callId));
    EXPECT_NO_THROW(manager.SendRttMessage(reportMessage));
}

/**
 * @tc.number   Telephony_ImsRttManagerTest003
 * @tc.name     test Rtt data stream processing functions
 * @tc.desc     Test Rtt data stream conversion and special sequence processing
 */
HWTEST_F(RttCallTest, Telephony_ImsRttManagerTest003, Function | MediumTest | Level1)
{
    ImsRttManager manager(1, 1);

    uint8_t testData1[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};
    std::string result1 = manager.RttDataStreamToString(testData1, sizeof(testData1));
    EXPECT_EQ(result1, "Hello");

    uint8_t testData2[] = {0xE2, 0x80, 0xA8};
    std::string result2 = manager.RttDataStreamToString(testData2, sizeof(testData2));
    EXPECT_EQ(result2, "\r\n");

    uint8_t testData3[] = {0x07};
    std::string result3 = manager.RttDataStreamToString(testData3, sizeof(testData3));
    EXPECT_EQ(result3.size(), 0);

    uint8_t testData4[] = {0x1B, 0x61};
    std::string result4 = manager.RttDataStreamToString(testData4, sizeof(testData4));
    EXPECT_EQ(result4.size(), 0);

    uint8_t testData5[] = {0xC2, 0x98};
    std::string result5 = manager.RttDataStreamToString(testData5, sizeof(testData5));
    EXPECT_EQ(result5.size(), 0);

    uint8_t testData6[] = {0xEF, 0xBB, 0xBF};
    std::string result6 = manager.RttDataStreamToString(testData6, sizeof(testData6));
    EXPECT_EQ(result6.size(), 0);

    uint8_t testData7[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0xE2, 0x80, 0xA8, 0x07, 0x1B,
        0x61, 0xC2, 0x98, 0xEF, 0xBB, 0xBF};
    std::string result7 = manager.RttDataStreamToString(testData7, sizeof(testData7));
    EXPECT_EQ(result7, "Hello\r\n");
}

/**
 * @tc.number   Telephony_ImsRttManagerTest004
 * @tc.name     test special sequence processing functions
 * @tc.desc     Test various special sequence processing functions
 */
HWTEST_F(RttCallTest, Telephony_ImsRttManagerTest004, Function | MediumTest | Level1)
{
    ImsRttManager manager(1, 1);
    std::vector<uint8_t> output;

    uint8_t escapeData[] = {0xE2, 0x80, 0xA8};
    EXPECT_TRUE(manager.ProcEscapeSeq(escapeData, sizeof(escapeData), 0, output));
    EXPECT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], 0x0D);
    EXPECT_EQ(output[1], 0x0A);

    uint8_t invalidEscapeData[] = {0xE2, 0x80};
    output.clear();
    EXPECT_FALSE(manager.ProcEscapeSeq(invalidEscapeData, sizeof(invalidEscapeData), 0, output));
    EXPECT_TRUE(output.empty());

    uint8_t bellData[] = {0x07};
    EXPECT_TRUE(manager.ProcBellSeq(bellData, 0));

    uint8_t normalData[] = {0x41};
    EXPECT_FALSE(manager.ProcBellSeq(normalData, 0));

    uint8_t controlData[] = {0x1B, 0x61};
    EXPECT_TRUE(manager.ProcControlSeq(controlData, sizeof(controlData), 0));

    uint8_t invalidControlData[] = {0x1B};
    EXPECT_FALSE(manager.ProcControlSeq(invalidControlData, sizeof(invalidControlData), 0));

    uint8_t otherControlData1[] = {0xC2, 0x98};
    EXPECT_TRUE(manager.ProcOtherControlBytes(otherControlData1, sizeof(otherControlData1), 0));

    uint8_t otherControlData2[] = {0xC2, 0x9C};
    EXPECT_TRUE(manager.ProcOtherControlBytes(otherControlData2, sizeof(otherControlData2), 0));

    uint8_t invalidOtherControlData[] = {0xC2};
    EXPECT_FALSE(manager.ProcOtherControlBytes(invalidOtherControlData, sizeof(invalidOtherControlData), 0));

    uint8_t bomData[] = {0xEF, 0xBB, 0xBF};
    EXPECT_TRUE(manager.ProcessOrderMark(bomData, sizeof(bomData), 0));

    uint8_t invalidBomData[] = {0xEF, 0xBB};
    EXPECT_FALSE(manager.ProcessOrderMark(invalidBomData, sizeof(invalidBomData), 0));
}
/**
 * @tc.number   Telephony_CallManagerServiceStubTest001
 * @tc.name     test CallManagerServiceStub normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_CallManagerServiceStubTest001, Function | MediumTest | Level1)
{
    int32_t ret = -100;
    int32_t callId = -1;
    MessageParcel reply;
    MessageParcel data;
    data.WriteInt32(callId);
    std::shared_ptr<CallManagerServiceStub> serviceStub = std::make_shared<CallManagerService>();
    EXPECT_NE(serviceStub->OnUpdateImsRttCallMode(data, reply), ret);

    std::string rttMessage = "rttMessage";
    MessageParcel data2;
    data2.WriteInt32(callId);
    data2.WriteString(rttMessage);
    EXPECT_NE(serviceStub->OnSendRttMessage(data2, reply), ret);

    int32_t isEnable = 0;
    MessageParcel data3;
    data3.WriteInt32(callId);
    data3.WriteInt32(isEnable);
    EXPECT_NE(serviceStub->OnSetRttCapability(data3, reply), ret);
}

/**
 * @tc.number   Telephony_CallStatusCallbackTest001
 * @tc.name     test CallStatusCallback normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_CallStatusCallbackTest001, Function | MediumTest | Level1)
{
    int32_t ret = -100;
    int32_t callId = -1;
    MessageParcel reply;
    std::shared_ptr<CallStatusCallbackStub> statusStub = std::make_shared<CallStatusCallback>();

    MessageParcel data2;
    data2.WriteInt32(callId);
    data2.WriteInt32(2);
    data2.WriteInt32(3);
    data2.WriteInt32(1);
    EXPECT_NE(statusStub->OnHandleRttEvtChanged(data2, reply), ret);

    MessageParcel data3;
    data3.WriteInt32(callId);
    data3.WriteInt32(2);
    data3.WriteInt32(3);
    data3.WriteString("resonText");
    data3.WriteInt32(2);
    EXPECT_NE(statusStub->OnHandleRttErrReport(data3, reply), ret);
}

/**
 * @tc.number   Telephony_RttCallListenerTest001
 * @tc.name     test RttCallListener normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_RttCallListenerTest001, Function | MediumTest | Level1)
{
    DialParaInfo dialInfo;
    std::shared_ptr<RttCallListener> listener_ = std::make_shared<RttCallListener>();
    sptr<CallBase> callObjectPtr = new IMSCall(dialInfo);
    sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(callObjectPtr.GetRefPtr());
    imsCall->SetCallType(CallType::TYPE_IMS);
    imsCall->SetRttState(RttCallState::RTT_STATE_YES);
    listener_->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_NE(listener_->rttManager_, nullptr);
}

/**
 * @tc.number   Telephony_RttCallListenerTest002
 * @tc.name     test RttCallListener normal func
 * @tc.desc     Function test
 */
HWTEST_F(RttCallTest, Telephony_RttCallListenerTest002, Function | MediumTest | Level1)
{
    DialParaInfo dialInfo;
    std::shared_ptr<RttCallListener> listener_ = std::make_shared<RttCallListener>();
    sptr<CallBase> callObjectPtr = new IMSCall(dialInfo);
    sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(callObjectPtr.GetRefPtr());
    imsCall->SetCallType(CallType::TYPE_CS);
    imsCall->SetRttState(RttCallState::RTT_STATE_YES);
    listener_->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(listener_->rttManager_, nullptr);
}

/**
 * @tc.number   Telephony_RttCallListenerTest003
 * @tc.name     test InitRttManager normal func
 * @tc.desc     Function test for InitRttManager
 */
HWTEST_F(RttCallTest, Telephony_RttCallListenerTest003, Function | MediumTest | Level1)
{
    DialParaInfo dialInfo;
    std::shared_ptr<RttCallListener> listener_ = std::make_shared<RttCallListener>();
    sptr<CallBase> callObjectPtr = new IMSCall(dialInfo);
    sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(callObjectPtr.GetRefPtr());

    listener_->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(listener_->rttManager_, nullptr);

    imsCall->SetRttState(RttCallState::RTT_STATE_NO);
    listener_->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(listener_->rttManager_, nullptr);

    imsCall->SetRttState(RttCallState::RTT_STATE_YES);
    listener_->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(listener_->rttManager_, nullptr);
}

/**
 * @tc.number   Telephony_RttCallListenerTest004
 * @tc.name     test UnInitRttManager normal func
 * @tc.desc     Function test for UnInitRttManager
 */
HWTEST_F(RttCallTest, Telephony_RttCallListenerTest004, Function | MediumTest | Level1)
{
    DialParaInfo dialInfo;
    std::shared_ptr<RttCallListener> listener_ = std::make_shared<RttCallListener>();
    sptr<CallBase> callObjectPtr = new IMSCall(dialInfo);
    sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(callObjectPtr.GetRefPtr());
    imsCall->SetCallType(CallType::TYPE_IMS);
    imsCall->SetRttState(RttCallState::RTT_STATE_YES);

    listener_->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_NE(listener_->rttManager_, nullptr);

    listener_->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_ACTIVE, TelCallState::CALL_STATUS_DISCONNECTED);
    EXPECT_EQ(listener_->rttManager_, nullptr);

    listener_->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ACTIVE);
    listener_->rttManager_ = nullptr;
    listener_->CallStateUpdated(
        callObjectPtr, TelCallState::CALL_STATUS_ACTIVE, TelCallState::CALL_STATUS_DISCONNECTED);
    EXPECT_EQ(listener_->rttManager_, nullptr);
}

/**
 * @tc.number   Telephony_RttCallListenerTest005
 * @tc.name     test RefreshRttParam normal func
 * @tc.desc     Function test for RefreshRttParam
 */
HWTEST_F(RttCallTest, Telephony_RttCallListenerTest005, Function | MediumTest | Level1)
{
    DialParaInfo dialInfo;
    std::shared_ptr<RttCallListener> listener_ = std::make_shared<RttCallListener>();
    sptr<CallBase> callObjectPtr = new IMSCall(dialInfo);
    sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(callObjectPtr.GetRefPtr());
    imsCall->SetCallType(CallType::TYPE_IMS);
    imsCall->SetRttState(RttCallState::RTT_STATE_YES);

    listener_->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_NE(listener_->rttManager_, nullptr);

    listener_->rttManager_ = nullptr;
    listener_->RefreshRttParam(333, RttCallState::RTT_STATE_YES, 444);
    EXPECT_EQ(listener_->rttManager_, nullptr);
}

/**
 * @tc.number   Telephony_RttCallListenerTest006
 * @tc.name     test SendRttMessage normal func
 * @tc.desc     Function test for SendRttMessage
 */
HWTEST_F(RttCallTest, Telephony_RttCallListenerTest006, Function | MediumTest | Level1)
{
    DialParaInfo dialInfo;
    std::shared_ptr<RttCallListener> listener_ = std::make_shared<RttCallListener>();
    sptr<CallBase> callObjectPtr = new IMSCall(dialInfo);
    sptr<IMSCall> imsCall = reinterpret_cast<IMSCall *>(callObjectPtr.GetRefPtr());
    imsCall->SetCallType(CallType::TYPE_IMS);
    imsCall->SetRttState(RttCallState::RTT_STATE_YES);

    listener_->CallStateUpdated(callObjectPtr, TelCallState::CALL_STATUS_DIALING, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_NE(listener_->rttManager_, nullptr);

    std::string testMessage = "Hello RTT";
    EXPECT_EQ(listener_->SendRttMessage(testMessage), RTT_SUCCESS);

    listener_->rttManager_ = nullptr;
    EXPECT_EQ(listener_->SendRttMessage(testMessage), TELEPHONY_ERR_LOCAL_PTR_NULL);
}
} // namespace Telephony
} // namespace OHOS
