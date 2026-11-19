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
#define private public
#define protected public

#include <algorithm>
#include <gtest/gtest.h>

#include "call_ability_callback.h"
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
 * Record every OnBtCallStateChange callback so that both the trigger times
 * and the reported parameters can be asserted.
 */
class MockBtAbilityCallback : public CallAbilityCallback {
public:
    int32_t OnBtCallStateChange(int32_t callId, int32_t numActive, int32_t numHeld, int32_t callState,
        const std::string &number) override
    {
        ++count_;
        lastCallId_ = callId;
        lastNumActive_ = numActive;
        lastNumHeld_ = numHeld;
        lastCallState_ = callState;
        lastNumber_ = number;
        return TELEPHONY_SUCCESS;
    }

    void Reset()
    {
        count_ = 0;
        lastCallId_ = NOT_EXIST_CALL_ID;
        lastNumActive_ = 0;
        lastNumHeld_ = 0;
        lastCallState_ = static_cast<int32_t>(TelCallState::CALL_STATUS_UNKNOWN);
        lastNumber_ = "";
    }

    int32_t count_ = 0;
    int32_t lastCallId_ = NOT_EXIST_CALL_ID;
    int32_t lastNumActive_ = 0;
    int32_t lastNumHeld_ = 0;
    int32_t lastCallState_ = static_cast<int32_t>(TelCallState::CALL_STATUS_UNKNOWN);
    std::string lastNumber_ = "";
};
}  // namespace

class ZeroBtCallStateReportTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<CallBase> CreateCall(CallType type, int32_t callId, TelCallState state);
    static CallAttributeInfo BuildInfo(int32_t callId, TelCallState state);

    std::shared_ptr<CallAbilityReportProxy> proxy_ = nullptr;
    sptr<MockBtAbilityCallback> callback_ = nullptr;
};

void ZeroBtCallStateReportTest::SetUpTestCase() {}

void ZeroBtCallStateReportTest::TearDownTestCase() {}

void ZeroBtCallStateReportTest::SetUp()
{
    proxy_ = DelayedSingleton<CallAbilityReportProxy>::GetInstance();
    ASSERT_NE(proxy_, nullptr);
    callback_ = new (std::nothrow) MockBtAbilityCallback();
    ASSERT_NE(callback_, nullptr);
    callback_->Reset();
    proxy_->callbackPtrList_.clear();
    proxy_->callbackPtrList_.emplace_back(callback_);
}

void ZeroBtCallStateReportTest::TearDown()
{
    if (proxy_ != nullptr) {
        proxy_->callbackPtrList_.clear();
    }
    CallObjectManager::callObjectPtrList_.clear();
    callback_ = nullptr;
    proxy_ = nullptr;
}

sptr<CallBase> ZeroBtCallStateReportTest::CreateCall(CallType type, int32_t callId, TelCallState state)
{
    DialParaInfo dialInfo;
    dialInfo.callId = callId;
    dialInfo.callType = type;
    dialInfo.callState = state;
    dialInfo.accountId = 0;
    dialInfo.number = TEST_ACCOUNT_NUMBER;
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

CallAttributeInfo ZeroBtCallStateReportTest::BuildInfo(int32_t callId, TelCallState state)
{
    CallAttributeInfo info;
    info.callId = callId;
    info.callState = state;
    (void)std::copy(TEST_ACCOUNT_NUMBER.begin(), TEST_ACCOUNT_NUMBER.end(), info.accountNumber);
    return info;
}

/**
 * @tc.number   Telephony_BtCallStateReport_001
 * @tc.name     test answered state is filtered
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_001, TestSize.Level0)
{
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ANSWERED);
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    proxy_->TryReportBtCallStateChange(info);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_BtCallStateReport_002
 * @tc.name     test call object not exist
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_002, TestSize.Level0)
{
    CallAttributeInfo info = BuildInfo(NOT_EXIST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportBtCallStateChange(info);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_BtCallStateReport_003
 * @tc.name     test voip call without remote control is filtered
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_003, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_VOIP, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    call->SetRemoteControl(false);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportBtCallStateChange(info);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_BtCallStateReport_004
 * @tc.name     test voip call with remote control is reported
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_004, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_VOIP, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    call->SetRemoteControl(true);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportBtCallStateChange(info);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(callback_->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
    EXPECT_EQ(callback_->lastNumber_, TEST_ACCOUNT_NUMBER);
}

/**
 * @tc.number   Telephony_BtCallStateReport_005
 * @tc.name     test cellular call is reported with real callId
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_005, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportBtCallStateChange(info);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(callback_->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
}

/**
 * @tc.number   Telephony_BtCallStateReport_006
 * @tc.name     test CallStateUpdated triggers bt call state report
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_006, TestSize.Level0)
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
 * @tc.number   Telephony_BtCallStateReport_007
 * @tc.name     test CallStateUpdated with answered state does not report
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_007, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    sptr<CallBase> callPtr = call;
    proxy_->CallStateUpdated(callPtr, TelCallState::CALL_STATUS_ACTIVE, TelCallState::CALL_STATUS_ANSWERED);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_BtCallStateReport_008
 * @tc.name     test ReportCallStateInfo no longer triggers bt call state report
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_008, TestSize.Level0)
{
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->ReportCallStateInfo(info);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_BtCallStateReport_009
 * @tc.name     test numActive and numHeld are counted correctly
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_009, TestSize.Level0)
{
    sptr<CallBase> activeCall = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(activeCall, nullptr);
    sptr<CallBase> holdCall =
        CreateCall(CallType::TYPE_IMS, TEST_CALL_ID_SECOND, TelCallState::CALL_STATUS_HOLDING);
    ASSERT_NE(holdCall, nullptr);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportBtCallStateChange(info);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastNumActive_, 1);
    EXPECT_EQ(callback_->lastNumHeld_, 1);
}

/**
 * @tc.number   Telephony_BtCallStateReport_010
 * @tc.name     test report when callback list is empty
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_010, TestSize.Level0)
{
    proxy_->callbackPtrList_.clear();
    sptr<CallBase> call = CreateCall(CallType::TYPE_IMS, TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    ASSERT_NE(call, nullptr);
    CallAttributeInfo info = BuildInfo(TEST_CALL_ID, TelCallState::CALL_STATUS_ACTIVE);
    proxy_->TryReportBtCallStateChange(info);
    EXPECT_EQ(callback_->count_, 0);
}

/**
 * @tc.number   Telephony_BtCallStateReport_011
 * @tc.name     test report skips nullptr callback and keeps valid callback
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_011, TestSize.Level0)
{
    proxy_->callbackPtrList_.emplace_back(nullptr);
    int32_t ret = proxy_->ReportBtCallStateChange(TEST_CALL_ID, 1, 0,
        static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE), TEST_ACCOUNT_NUMBER);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    EXPECT_EQ(callback_->count_, 1);
    EXPECT_EQ(callback_->lastCallId_, TEST_CALL_ID);
    EXPECT_EQ(callback_->lastCallState_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE));
}

/**
 * @tc.number   Telephony_BtCallStateReport_012
 * @tc.name     test report with only nullptr callback returns fail
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBtCallStateReportTest, Telephony_BtCallStateReport_012, TestSize.Level0)
{
    proxy_->callbackPtrList_.clear();
    proxy_->callbackPtrList_.emplace_back(nullptr);
    int32_t ret = proxy_->ReportBtCallStateChange(TEST_CALL_ID, 1, 0,
        static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE), TEST_ACCOUNT_NUMBER);
    EXPECT_EQ(ret, TELEPHONY_ERR_FAIL);
    EXPECT_EQ(callback_->count_, 0);
}
}  // namespace Telephony
}  // namespace OHOS
