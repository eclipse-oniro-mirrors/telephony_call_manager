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
    RttEvent event;
    MessageParcel reply;
    int32_t length = 0;
    std::shared_ptr<CallAbilityCallbackStub> callbackStub = std::make_shared<MockCallAbilityCallbackStub>();

    MessageParcel data_one;
    data_one.WriteInt32(length);
    data_one.WriteRawData((void *)&event, sizeof(RttEvent));
    EXPECT_EQ(callbackStub->OnUpdateRttCallEvent(data_one, reply), TELEPHONY_ERR_ARGUMENT_INVALID);

    length = sizeof(RttEvent);
    MessageParcel data_two;
    data_two.WriteInt32(length);
    data_two.WriteRawData((void *)&event, length);
    EXPECT_EQ(callbackStub->OnUpdateRttCallEvent(data_two, reply), TELEPHONY_SUCCESS);

    RttError rttError;
    length = 0;
    MessageParcel data_three;
    data_three.WriteInt32(length);
    data_three.WriteRawData((void *)&rttError, sizeof(RttError));
    EXPECT_EQ(callbackStub->OnUpdateRttCallErrorReport(data_three, reply), TELEPHONY_ERR_ARGUMENT_INVALID);

    length = sizeof(RttError);
    MessageParcel data_four;
    data_four.WriteInt32(length);
    data_four.WriteRawData((void *)&rttError, sizeof(RttError));
    EXPECT_EQ(callbackStub->OnUpdateRttCallErrorReport(data_four, reply), TELEPHONY_SUCCESS);

    MessageParcel data_five;
    data_five.WriteInt32(0); // call id
    data_five.WriteString("rttMessage"); // rttMessage
    EXPECT_EQ(callbackStub->OnUpdateRttCallMessage(data_five, reply), TELEPHONY_SUCCESS);
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

    manager.devFd_ = -1;
    EXPECT_EQ(manager.CloseProxy(), RTT_SUCCESS);
    manager.devFd_ = 1;
    EXPECT_NE(manager.CloseProxy(), ret);

    manager.devFd_ = 0;
    EXPECT_NO_THROW(manager.WakeUpKernelRead());

    EXPECT_NE(manager.CreateRttThread(), ret);
    EXPECT_NE(manager.CreateSendThread(), ret);

    RttCallEvtWork *rttCallEvtwork = std::make_unique<RttCallEvtWork>().release();
    EXPECT_NO_THROW(manager.SendThreadLoop((void *)rttCallEvtwork));

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
    int32_t ret = -100;
    ImsRttManager manager(1, 1);
    EXPECT_NE(manager.CreateRecvThread(), ret);

    RttCallEvtWork *rttCallEvtwork = std::make_unique<RttCallEvtWork>().release();
    EXPECT_NO_THROW(manager.RecvThreadLoop((void *)rttCallEvtwork));

    std::string reportMessage = "message";
    EXPECT_NO_THROW(manager.ReportRecvMessage(reportMessage));

    manager.devFd_ = PROXY_IS_OFF;
    EXPECT_NO_THROW(manager.RecvDataFromProxy(reportMessage));
    manager.devFd_ = PROXY_IS_OFF + 1;
    EXPECT_NO_THROW(manager.RecvDataFromProxy(reportMessage));

    EXPECT_NO_THROW(manager.DestroyRtt());

    uint16_t channelId = 1;
    int32_t callId = 1;
    EXPECT_NO_THROW(manager.SetChannelID(channelId));
    EXPECT_NO_THROW(manager.SetCallID(callId));
    EXPECT_NO_THROW(manager.SendRttMessage(reportMessage));
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
    int32_t len = 0;
    MessageParcel reply;
    MessageParcel data;
    data.WriteInt32(len);
    std::shared_ptr<CallStatusCallbackStub> statusStub = std::make_shared<CallStatusCallback>();
    EXPECT_EQ(statusStub->OnHandleRttEvtChanged(data, reply), TELEPHONY_ERR_ARGUMENT_INVALID);

    RttEventInfo eventInfo;
    len = sizeof(RttEventInfo);
    MessageParcel data2;
    data2.WriteInt32(len);
    data2.WriteRawData((void *)&eventInfo, sizeof(RttEventInfo));
    EXPECT_NE(statusStub->OnHandleRttEvtChanged(data2, reply), ret);

    RttErrorInfo errorInfo;
    len = sizeof(RttErrorInfo);
    MessageParcel data3;
    data3.WriteInt32(len);
    data3.WriteRawData((void *)&errorInfo, sizeof(RttErrorInfo));
    EXPECT_NE(statusStub->OnHandleRttErrReport(data3, reply), ret);
}
} // namespace Telephony
} // namespace OHOS
