/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
// NOLINTBEGIN(readability-identifier-naming)
#define private public
#define protected public
// NOLINTEND(readability-identifier-naming)

#include <algorithm>
#include <gtest/gtest.h>

#include "call_ability_callback.h"
#include "call_ability_callback_proxy.h"
#include "call_ability_report_proxy.h"
#include "call_manager_info.h"
#include "call_object_manager.h"
#include "ims_call.h"
#include "telephony_errors.h"
#include "voip_call.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
constexpr int32_t TEST_CALL_ID = 100;
constexpr int32_t TEST_CALL_ID_SECOND = 101;
constexpr int32_t NOT_EXIST_CALL_ID = 9999;
const std::string TEST_ACCOUNT_NUMBER = "13800000000";

/**
 * Record every OnCallStateChange callback so that both the trigger times
 * and the reported parameters can be asserted.
 */
class MockAbilityCallback : public CallAbilityCallback {
public:
    int32_t OnCallStateChange(int32_t callId, int32_t numActive, int32_t numHeld, int32_t callState,
        const std::string &number) override
    {
        ++count_;
        lastCallId_ = callId;
        lastNumActive_ = numActive;
        lastNumHeld_ = numHeld;
        lastCallState_ = callState;
        lastNumber_ = number;
        return returnValue_;
    }

    void Reset()
    {
        count_ = 0;
        lastCallId_ = NOT_EXIST_CALL_ID;
        lastNumActive_ = 0;
        lastNumHeld_ = 0;
        lastCallState_ = static_cast<int32_t>(TelCallState::CALL_STATUS_UNKNOWN);
        lastNumber_ = "";
        returnValue_ = TELEPHONY_SUCCESS;
    }

    int32_t count_ = 0;
    int32_t lastCallId_ = NOT_EXIST_CALL_ID;
    int32_t lastNumActive_ = 0;
    int32_t lastNumHeld_ = 0;
    int32_t lastCallState_ = static_cast<int32_t>(TelCallState::CALL_STATUS_UNKNOWN);
    std::string lastNumber_ = "";
    int32_t returnValue_ = TELEPHONY_SUCCESS;
};

/**
 * Mock process callback used to verify CallAbilityCallback forwards the state
 * change to the registered manager callback.
 */
class MockManagerCallback : public CallManagerCallback {
public:
    int32_t OnCallDetailsChange(const CallAttributeInfo &info) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallEventChange(const CallEventInfo &info) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallDisconnectedCause(const DisconnectedDetails &details) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportMmiCodeResult(const MmiCodeInfo &info) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportAudioDeviceChange(const AudioDeviceInfo &info) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportPostDialDelay(const std::string &str) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnUpdateImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallSessionEventChange(const CallSessionEvent &callSessionEventOptions) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallDataUsageChange(const int64_t dataUsage) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnUpdateCameraCapabilities(const CameraCapabilities &cameraCapabilities) override
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallStateChange(int32_t callId, int32_t numActive, int32_t numHeld, int32_t callState,
        const std::string &number) override
    {
        ++count_;
        lastCallId_ = callId;
        lastNumActive_ = numActive;
        lastNumHeld_ = numHeld;
        lastCallState_ = callState;
        lastNumber_ = number;
        return TELEPHONY_ERR_FAIL;
    }

    int32_t count_ = 0;
    int32_t lastCallId_ = NOT_EXIST_CALL_ID;
    int32_t lastNumActive_ = 0;
    int32_t lastNumHeld_ = 0;
    int32_t lastCallState_ = static_cast<int32_t>(TelCallState::CALL_STATUS_UNKNOWN);
    std::string lastNumber_ = "";
};
}  // namespace

class ZeroCallStateReportTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<CallBase> CreateCall(CallType type, int32_t callId, TelCallState state,
        const std::string &number = TEST_ACCOUNT_NUMBER);
    static CallAttributeInfo BuildInfo(int32_t callId, TelCallState state);

    std::shared_ptr<CallAbilityReportProxy> proxy_ = nullptr;
    sptr<MockAbilityCallback> callback_ = nullptr;
};

void ZeroCallStateReportTest::SetUpTestCase() {}

void ZeroCallStateReportTest::TearDownTestCase() {}

void ZeroCallStateReportTest::SetUp()
{
    proxy_ = DelayedSingleton<CallAbilityReportProxy>::GetInstance();
    ASSERT_NE(proxy_, nullptr);
    callback_ = new (std::nothrow) MockAbilityCallback();
    ASSERT_NE(callback_, nullptr);
    callback_->Reset();
    proxy_->callbackPtrList_.clear();
    proxy_->callbackPtrList_.emplace_back(callback_);
}

void ZeroCallStateReportTest::TearDown()
{
    if (proxy_ != nullptr) {
        proxy_->callbackPtrList_.clear();
    }
    CallObjectManager::callObjectPtrList_.clear();
    callback_ = nullptr;
    proxy_ = nullptr;
}

sptr<CallBase> ZeroCallStateReportTest::CreateCall(CallType type, int32_t callId, TelCallState state,
    const std::string &number)
{
    DialParaInfo dialInfo;
    dialInfo.callId = callId;
    dialInfo.callType = type;
    dialInfo.callState = state;
    dialInfo.accountId = 0;
    dialInfo.number = number;
    sptr<CallBase> call = nullptr;
    if (type == CallType::TYPE_VOIP) {
        call = new (std::nothrow) VoIPCall(dialInfo);
    } else {
        call = new (std::nothrow) IMSCall(dialInfo);
    }
    if (call != nullptr) {
        CallObjectManager::callObjectPtrList_.push_back(call);
    }
    return call;
}

CallAttributeInfo ZeroCallStateReportTest::BuildInfo(int32_t callId, TelCallState state)
{
    CallAttributeInfo info;
    info.callId = callId;
    info.callState = state;
    (void)std::copy(TEST_ACCOUNT_NUMBER.begin(), TEST_ACCOUNT_NUMBER.end(), info.accountNumber);
    return info;
}

/**
 * @tc.number   Telephony_TryReportCallStateChange_AnsweredWithCallId
 * @tc.name     test answered state is reported with real callId
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_TryReportCallStateChange_AnsweredWithCallId, TestSize.Level0)
{
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ANSWERED);
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    proxy_->TryReportCallStateChange(info);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(callback_->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ANSWERED));
}

/**
 * @tc.number   Telephony_TryReportCallStateChange_CallObjectNotExist
 * @tc.name     test call object not exist
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_TryReportCallStateChange_CallObjectNotExist, TestSize.Level0)
{
    CallAttributeInfo info = BuildInfo(NOT_EXIST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportCallStateChange(info);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_TryReportCallStateChange_VoipWithoutRemoteControl
 * @tc.name     test voip call without remote control is filtered
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_TryReportCallStateChange_VoipWithoutRemoteControl, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_VOIP, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    call->SetRemoteControl(false);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportCallStateChange(info);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_TryReportCallStateChange_VoipWithRemoteControl
 * @tc.name     test voip call with remote control is reported
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_TryReportCallStateChange_VoipWithRemoteControl, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_VOIP, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    call->SetRemoteControl(true);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportCallStateChange(info);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(callback_->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
    EXPECT_EQ(callback_->lastNumber_, TEST_ACCOUNT_NUMBER);
}

/**
 * @tc.number   Telephony_TryReportCallStateChange_CellularWithCallId
 * @tc.name     test cellular call is reported with real callId
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_TryReportCallStateChange_CellularWithCallId, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportCallStateChange(info);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(callback_->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
}

/**
 * @tc.number   Telephony_CallStateUpdated_TriggersReport
 * @tc.name     test CallStateUpdated triggers bt call state report
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallStateUpdated_TriggersReport, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    sptr<CallBase> callPtr = call;
    proxy_->CallStateUpdated(callPtr, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(callback_->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
    EXPECT_EQ(callback_->lastNumber_, TEST_ACCOUNT_NUMBER);
}

/**
 * @tc.number   Telephony_CallStateUpdated_AnsweredState
 * @tc.name     test CallStateUpdated with answered state reports
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallStateUpdated_AnsweredState, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    sptr<CallBase> callPtr = call;
    proxy_->CallStateUpdated(callPtr, TelCallState::CALL_STATUS_ACTIVE, TelCallState::CALL_STATUS_ANSWERED);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(callback_->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ANSWERED));
}

/**
 * @tc.number   Telephony_ReportCallStateInfo_NotTriggerReport
 * @tc.name     test ReportCallStateInfo no longer triggers bt call state report
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_ReportCallStateInfo_NotTriggerReport, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->ReportCallStateInfo(info);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_TryReportCallStateChange_NumActiveAndNumHeld
 * @tc.name     test numActive and numHeld are counted correctly
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_TryReportCallStateChange_NumActiveAndNumHeld, TestSize.Level0)
{
    sptr<CallBase> activeCall = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(activeCall, nullptr);
    sptr<CallBase> holdCall =
        CreateCall(CallType::TYPE_IMS, TEST_CALL_ID_SECOND, TelCallState::CALL_STATUS_HOLDING);
    ASSERT_NE(holdCall, nullptr);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportCallStateChange(info);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastNumActive_, 1);
    EXPECT_EQ(callback_->lastNumHeld_, 1);
}

/**
 * @tc.number   Telephony_TryReportCallStateChange_EmptyCallbackList
 * @tc.name     test report when callback list is empty
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_TryReportCallStateChange_EmptyCallbackList, TestSize.Level0)
{
    proxy_->callbackPtrList_.clear();
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportCallStateChange(info);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_ReportCallStateChange_SkipNullptrCallback
 * @tc.name     test report skips nullptr callback and keeps valid callback
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_ReportCallStateChange_SkipNullptrCallback, TestSize.Level0)
{
    proxy_->callbackPtrList_.emplace_back(nullptr);
    int32_t ret = proxy_->ReportCallStateChange(TEST_CALL_ID, 1, 0,
        static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE), TEST_ACCOUNT_NUMBER);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(callback_->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
}

/**
 * @tc.number   Telephony_ReportCallStateChange_OnlyNullptrCallback
 * @tc.name     test report with only nullptr callback returns fail
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_ReportCallStateChange_OnlyNullptrCallback, TestSize.Level0)
{
    proxy_->callbackPtrList_.clear();
    proxy_->callbackPtrList_.emplace_back(nullptr);
    int32_t ret = proxy_->ReportCallStateChange(TEST_CALL_ID, 1, 0,
        static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE), TEST_ACCOUNT_NUMBER);
    EXPECT_EQ(ret, TELEPHONY_ERR_FAIL);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_CallAbilityCallback_ForwardToProcessCallback
 * @tc.name     test ability callback forwards state change to process callback
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallAbilityCallback_ForwardToProcessCallback, TestSize.Level0)
{
    CallAbilityCallback abilityCallback;
    auto managerCallback = std::make_unique<MockManagerCallback>();
    MockManagerCallback *managerPtr = managerCallback.get();
    ASSERT_EQ(abilityCallback.SetProcessCallback(std::move(managerCallback)), TELEPHONY_SUCCESS);
    int32_t ret = abilityCallback.OnCallStateChange(TEST_CALL_ID, 1, 0,
        static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE), TEST_ACCOUNT_NUMBER);
    EXPECT_EQ(ret, TELEPHONY_ERR_FAIL);
    EXPECT_EQ(managerPtr->count_, 1);
    EXPECT_EQ(managerPtr->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(managerPtr->lastNumActive_, 1);
    EXPECT_EQ(managerPtr->lastNumHeld_, 0);
    EXPECT_EQ(managerPtr->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
    EXPECT_EQ(managerPtr->lastNumber_, TEST_ACCOUNT_NUMBER);
}

/**
 * @tc.number   Telephony_CallAbilityCallback_NoProcessCallback
 * @tc.name     test ability callback without process callback returns success
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallAbilityCallback_NoProcessCallback, TestSize.Level0)
{
    CallAbilityCallback abilityCallback;
    int32_t ret = abilityCallback.OnCallStateChange(TEST_CALL_ID, 1, 0,
        static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE), TEST_ACCOUNT_NUMBER);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallAbilityCallbackStub_WithoutFileDescriptor
 * @tc.name     test stub dispatch without file descriptor writes result to reply
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallAbilityCallbackStub_WithoutFileDescriptor, TestSize.Level0)
{
    MockAbilityCallback stub;
    MessageParcel data;
    MessageParcel reply;
    ASSERT_TRUE(data.WriteInt32(TEST_CALL_ID));
    ASSERT_TRUE(data.WriteInt32(1));
    ASSERT_TRUE(data.WriteInt32(0));
    ASSERT_TRUE(data.WriteInt32(static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE)));
    ASSERT_TRUE(data.WriteString(TEST_ACCOUNT_NUMBER));
    int32_t ret = stub.OnUpdateBtCallState(data, reply);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    EXPECT_EQ(stub.count_, 1);
    EXPECT_EQ(stub.lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(stub.lastNumActive_, 1);
    EXPECT_EQ(stub.lastNumHeld_, 0);
    EXPECT_EQ(stub.lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
    EXPECT_EQ(stub.lastNumber_, TEST_ACCOUNT_NUMBER);
    EXPECT_EQ(reply.ReadInt32(), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallAbilityCallbackStub_WithFileDescriptor
 * @tc.name     test stub dispatch with file descriptor
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallAbilityCallbackStub_WithFileDescriptor, TestSize.Level0)
{
    MockAbilityCallback stub;
    MessageParcel data;
    MessageParcel reply;
    ASSERT_TRUE(data.WriteInt32(TEST_CALL_ID));
    ASSERT_TRUE(data.WriteInt32(0));
    ASSERT_TRUE(data.WriteInt32(0));
    ASSERT_TRUE(data.WriteInt32(static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING)));
    ASSERT_TRUE(data.WriteString(TEST_ACCOUNT_NUMBER));
    ASSERT_TRUE(data.WriteFileDescriptor(0));
    int32_t ret = stub.OnUpdateBtCallState(data, reply);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    EXPECT_EQ(stub.count_, 1);
    EXPECT_EQ(stub.lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(stub.lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING));
    EXPECT_EQ(reply.ReadInt32(), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallAbilityCallbackProxy_NullRemote
 * @tc.name     test proxy returns ipc fail when remote is nullptr
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallAbilityCallbackProxy_NullRemote, TestSize.Level0)
{
    sptr<IRemoteObject> impl;
    CallAbilityCallbackProxy callbackProxy(impl);
    int32_t ret = callbackProxy.OnCallStateChange(TEST_CALL_ID, 1, 0,
        static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE), TEST_ACCOUNT_NUMBER);
    EXPECT_EQ(ret, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_CallAbilityCallbackProxy_ForwardToRemoteStub
 * @tc.name     test proxy forwards state change to remote stub
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallAbilityCallbackProxy_ForwardToRemoteStub, TestSize.Level0)
{
    sptr<MockAbilityCallback> stub = new (std::nothrow) MockAbilityCallback();
    ASSERT_NE(stub, nullptr);
    stub->Reset();
    CallAbilityCallbackProxy callbackProxy(stub);
    int32_t ret = callbackProxy.OnCallStateChange(TEST_CALL_ID, 1, 0,
        static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE), TEST_ACCOUNT_NUMBER);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    EXPECT_EQ(stub->count_, 1);
    EXPECT_EQ(stub->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(stub->lastNumActive_, 1);
    EXPECT_EQ(stub->lastNumHeld_, 0);
    EXPECT_EQ(stub->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
    EXPECT_EQ(stub->lastNumber_, TEST_ACCOUNT_NUMBER);
}

/**
 * @tc.number   Telephony_ReportCallStateChange_ContinueAfterFail
 * @tc.name     test report continues after a callback returns fail
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_ReportCallStateChange_ContinueAfterFail, TestSize.Level0)
{
    callback_->returnValue_ = TELEPHONY_ERR_FAIL;
    sptr<MockAbilityCallback> secondCallback = new (std::nothrow) MockAbilityCallback();
    ASSERT_NE(secondCallback, nullptr);
    secondCallback->Reset();
    proxy_->callbackPtrList_.emplace_back(secondCallback);
    int32_t ret = proxy_->ReportCallStateChange(TEST_CALL_ID, 1, 0,
        static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE), TEST_ACCOUNT_NUMBER);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(secondCallback->count_, 1);
}

/**
 * @tc.number   Telephony_CallStateUpdated_NullCallObject
 * @tc.name     test CallStateUpdated with nullptr call object does not report
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallStateUpdated_NullCallObject, TestSize.Level0)
{
    sptr<CallBase> call = nullptr;
    proxy_->CallStateUpdated(call, TelCallState::CALL_STATUS_ACTIVE, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_CallStateUpdated_TruncateNumberAtComma
 * @tc.name     test CallStateUpdated truncates number at comma
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallStateUpdated_TruncateNumberAtComma, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE,
        "138,0000");
    ASSERT_NE(call, nullptr);
    proxy_->CallStateUpdated(call, TelCallState::CALL_STATUS_ACTIVE, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastNumber_, "138");
}

/**
 * @tc.number   Telephony_CallStateUpdated_TruncateNumberAtSemicolon
 * @tc.name     test CallStateUpdated truncates number at semicolon
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallStateUpdated_TruncateNumberAtSemicolon, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE,
        "139;0000");
    ASSERT_NE(call, nullptr);
    proxy_->CallStateUpdated(call, TelCallState::CALL_STATUS_ACTIVE, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastNumber_, "139");
}

/**
 * @tc.number   Telephony_CallAbilityCallbackStub_WriteReplyFail
 * @tc.name     test OnUpdateBtCallState returns write reply fail when reply rejects writes
 * @tc.desc     Function test
 */
HWTEST_F(ZeroCallStateReportTest, Telephony_CallAbilityCallbackStub_WriteReplyFail, TestSize.Level0)
{
    MockAbilityCallback stub;
    MessageParcel data;
    MessageParcel reply;
    ASSERT_TRUE(data.WriteInt32(TEST_CALL_ID));
    ASSERT_TRUE(data.WriteInt32(1));
    ASSERT_TRUE(data.WriteInt32(0));
    ASSERT_TRUE(data.WriteInt32(static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE)));
    ASSERT_TRUE(data.WriteString(TEST_ACCOUNT_NUMBER));
    // Max capacity 0 makes the reply parcel reject writes, so WriteInt32 fails.
    ASSERT_TRUE(reply.SetMaxCapacity(0));
    int32_t ret = stub.OnUpdateBtCallState(data, reply);
    EXPECT_EQ(ret, TELEPHONY_ERR_WRITE_REPLY_FAIL);
    EXPECT_EQ(stub.count_, 1);
}
}  // namespace Telephony
}  // namespace OHOS
