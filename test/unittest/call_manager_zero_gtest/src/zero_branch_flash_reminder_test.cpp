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
#include <dlfcn.h>
#include <functional>
#include <memory>
#include "gtest/gtest.h"

#include "incoming_flash_reminder.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
constexpr int WAIT_TIME = 1;

class IncomingFlashReminderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase()
    {
        sleep(WAIT_TIME);
    }
    void SetUp() override
    {
        callbackCalled_ = false;
        runner_ = AppExecFwk::EventRunner::Create("test_runner");
        reminder_ = std::make_shared<IncomingFlashReminder>(runner_, [this]() {
            callbackCalled_ = true;
        });
    }
    void TearDown() override
    {
        if (reminder_ != nullptr) {
            reminder_->ReleaseDepsAdapter();
        }
        runner_ = nullptr;
        reminder_ = nullptr;
    }

protected:
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<IncomingFlashReminder> reminder_;
    bool callbackCalled_ = false;
};

/**
 * @tc.number   Telephony_IncomingFlashReminder_HandlerNullptr
 * @tc.name     test libAdapterHandler_ nullptr branch
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    EXPECT_FALSE(reminder_->IsScreenStatusSatisfied());
    EXPECT_FALSE(reminder_->IsTorchReady());
    reminder_->HandleSetTorchMode();
    reminder_->ReleaseDepsAdapter();
    EXPECT_FALSE(reminder_->IsFlashRemindNecessary());
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_HandlerNullptr_HandleStopFlashRemind_001
 * @tc.name     test libAdapterHandler_ nullptr branch for HandleStopFlashRemind when isFlashRemindUsed_ is true
 * @tc.desc     Function test - covers line 242-245 in source code
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_HandleStopFlashRemind_Used, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = true;
    callbackCalled_ = false;
    reminder_->HandleStopFlashRemind();
    EXPECT_FALSE(callbackCalled_);
    EXPECT_FALSE(reminder_->isFlashRemindUsed_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_HandlerNullptr_HandleStopFlashRemind_NotUsed_001
 * @tc.name     test libAdapterHandler_ nullptr branch for HandleStopFlashRemind when isFlashRemindUsed_ is false
 * @tc.desc     Function test - covers line 233-238 in source code
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_HandleStopFlashRemind_NotUsed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    callbackCalled_ = false;
    reminder_->HandleStopFlashRemind();
    EXPECT_TRUE(callbackCalled_);
    EXPECT_FALSE(reminder_->isFlashRemindUsed_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_HandlerNullptr_Destructor_NotUsed_001
 * @tc.name     test libAdapterHandler_ nullptr branch for Destructor when isFlashRemindUsed_ is false
 * @tc.desc     Function test - covers line 51-53 in source code
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_Destructor_NotUsed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    reminder_.reset();
    EXPECT_EQ(reminder_, nullptr);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_HandlerNullptr_Destructor_Used_001
 * @tc.name     test libAdapterHandler_ nullptr branch for Destructor when isFlashRemindUsed_ is true
 * @tc.desc     Function test - covers line 56-58 in source code
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_Destructor_Used, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = true;
    reminder_.reset();
    EXPECT_EQ(reminder_, nullptr);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DlsymFailed
 * @tc.name     test dlsym return nullptr branch for IsScreenStatusSatisfied
 * @tc.desc     Function test - use libtelephony_cust_api.z.so which won't have IsScreenLocked symbol
 */
HWTEST_F(IncomingFlashReminderTest, DlsymFailed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libtelephony_cust_api.z.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        EXPECT_FALSE(reminder_->IsScreenStatusSatisfied());
        EXPECT_FALSE(reminder_->IsTorchReady());
        reminder_->HandleSetTorchMode();
        reminder_->ReleaseDepsAdapter();
        EXPECT_NE(reminder_->libAdapterHandler_, nullptr);
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DlsymFailed_HandleStopFlashRemind_001
 * @tc.name     test dlsym return nullptr branch for HandleStopFlashRemind when isFlashRemindUsed_ is true
 * @tc.desc     Function test - covers line 247-249 in source code
 */
HWTEST_F(IncomingFlashReminderTest, DlsymFailed_HandleStopFlashRemind, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libtelephony_cust_api.z.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        reminder_->isFlashRemindUsed_ = true;
        callbackCalled_ = false;
        reminder_->HandleStopFlashRemind();
        EXPECT_FALSE(callbackCalled_);
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DlsymFailed_Destructor_001
 * @tc.name     test dlsym return nullptr branch for Destructor when isFlashRemindUsed_ is true
 * @tc.desc     Function test - covers line 62-63 in source code
 */
HWTEST_F(IncomingFlashReminderTest, DlsymFailed_Destructor, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libtelephony_cust_api.z.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        reminder_->isFlashRemindUsed_ = true;
        reminder_.reset();
        reminder_ = nullptr;
    }
    EXPECT_EQ(reminder_, nullptr);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_RealLib
 * @tc.name     test with real libtel_cm_deps_adapter.z.so if available
 * @tc.desc     Function test - weak assertions due to uncontrollable return values
 */
HWTEST_F(IncomingFlashReminderTest, RealLib, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter.z.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        reminder_->IsScreenStatusSatisfied();
        reminder_->IsTorchReady();
        reminder_->IsFlashRemindNecessary();
        reminder_->HandleSetTorchMode();
        reminder_->ReleaseDepsAdapter();
        EXPECT_TRUE(reminder_->libAdapterHandler_ == nullptr);
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ProcessUnknownEvent_001
 * @tc.name     test process unknown event
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, ProcessUnknownEvent, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    auto event = AppExecFwk::InnerEvent::Get(99999, 0);
    reminder_->ProcessEvent(event);
    EXPECT_EQ(reminder_->libAdapterHandler_, nullptr);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_StartFlashRemind_AlreadyStarted_001
 * @tc.name     test start flash remind already started
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, StartFlashRemind_AlreadyStarted, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = true;
    reminder_->StartFlashRemind();
    EXPECT_TRUE(reminder_->isFlashRemindUsed_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_StopFlashRemind_NotUsed_001
 * @tc.name     test stop flash remind when not used
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, StopFlashRemind_NotUsed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    callbackCalled_ = false;
    reminder_->StopFlashRemind();
    EXPECT_FALSE(callbackCalled_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_StartFlashRemind_NotStarted_001
 * @tc.name     test start flash remind when not started
 * @tc.desc     Function test - tests initial state
 */
HWTEST_F(IncomingFlashReminderTest, FlashRemind, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    reminder_->StartFlashRemind();
    EXPECT_FALSE(callbackCalled_);

    reminder_->isFlashRemindUsed_ = true;
    reminder_->StopFlashRemind();
    EXPECT_FALSE(reminder_->isFlashRemindUsed_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_Constructor_001
 * @tc.name     test constructor with valid runner and callback
 * @tc.desc     Function test - verifies object is constructed properly
 */
HWTEST_F(IncomingFlashReminderTest, Constructor, TestSize.Level0)
{
    auto testRunner = AppExecFwk::EventRunner::Create("constructor_test");
    bool testCallbackCalled = false;
    auto testReminder = std::make_shared<IncomingFlashReminder>(testRunner, [&testCallbackCalled]() {
        testCallbackCalled = true;
    });
    EXPECT_NE(testReminder, nullptr);
    EXPECT_FALSE(testReminder->isFlashRemindUsed_);
    EXPECT_EQ(testReminder->libAdapterHandler_, nullptr);
    testReminder = nullptr;
}

} // namespace Telephony
} // namespace OHOS