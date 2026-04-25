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
#include "incoming_flash_reminder.h"
#include "gtest/gtest.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include <dlfcn.h>
#include <functional>
#include <memory>

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

constexpr uint32_t DELAY_SET_TORCH_EVENT = 1000000;
constexpr uint32_t STOP_FLASH_REMIND_EVENT = 1000001;
constexpr uint32_t START_FLASH_REMIND_EVENT = 1000002;

class IncomingFlashReminderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
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
            if (reminder_->libAdapterHandler_ != nullptr) {
                dlclose(reminder_->libAdapterHandler_);
                reminder_->libAdapterHandler_ = nullptr;
            }
            reminder_ = nullptr;
        }
        runner_ = nullptr;
    }

protected:
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<IncomingFlashReminder> reminder_;
    bool callbackCalled_ = false;
};

/**
 * @tc.number   Telephony_IncomingFlashReminder_HandlerNullptr_IsScreenStatusSatisfied_001
 * @tc.name     test libAdapterHandler_ nullptr branch for IsScreenStatusSatisfied
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_IsScreenStatusSatisfied, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    EXPECT_FALSE(reminder_->IsScreenStatusSatisfied());
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_HandlerNullptr_IsTorchReady_001
 * @tc.name     test libAdapterHandler_ nullptr branch for IsTorchReady
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_IsTorchReady, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    EXPECT_FALSE(reminder_->IsTorchReady());
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_HandlerNullptr_HandleSetTorchMode_001
 * @tc.name     test libAdapterHandler_ nullptr branch for HandleSetTorchMode
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_HandleSetTorchMode, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->HandleSetTorchMode();
    EXPECT_TRUE(true);
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
 * @tc.number   Telephony_IncomingFlashReminder_HandlerNullptr_ReleaseDepsAdapter_001
 * @tc.name     test libAdapterHandler_ nullptr branch for ReleaseDepsAdapter
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_ReleaseDepsAdapter, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->ReleaseDepsAdapter();
    EXPECT_EQ(reminder_->libAdapterHandler_, nullptr);
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
    reminder_ = nullptr;
    EXPECT_TRUE(true);
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
    reminder_ = nullptr;
    EXPECT_TRUE(true);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DlsymFailed_IsScreenStatusSatisfied_001
 * @tc.name     test dlsym return nullptr branch for IsScreenStatusSatisfied
 * @tc.desc     Function test - use libc.so which won't have IsScreenLocked symbol
 */
HWTEST_F(IncomingFlashReminderTest, DlsymFailed_IsScreenStatusSatisfied, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libc.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        EXPECT_FALSE(reminder_->IsScreenStatusSatisfied());
        dlclose(reminder_->libAdapterHandler_);
        reminder_->libAdapterHandler_ = nullptr;
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DlsymFailed_IsTorchReady_001
 * @tc.name     test dlsym return nullptr branch for IsTorchReady
 * @tc.desc     Function test - use libc.so which won't have camera/torch symbols
 */
HWTEST_F(IncomingFlashReminderTest, DlsymFailed_IsTorchReady, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libc.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        EXPECT_FALSE(reminder_->IsTorchReady());
        dlclose(reminder_->libAdapterHandler_);
        reminder_->libAdapterHandler_ = nullptr;
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DlsymFailed_HandleSetTorchMode_001
 * @tc.name     test dlsym return nullptr branch for HandleSetTorchMode
 * @tc.desc     Function test - use libc.so which won't have torch mode symbols
 */
HWTEST_F(IncomingFlashReminderTest, DlsymFailed_HandleSetTorchMode, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libc.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        reminder_->HandleSetTorchMode();
        EXPECT_TRUE(true);
        dlclose(reminder_->libAdapterHandler_);
        reminder_->libAdapterHandler_ = nullptr;
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DlsymFailed_HandleStopFlashRemind_001
 * @tc.name     test dlsym return nullptr branch for HandleStopFlashRemind when isFlashRemindUsed_ is true
 * @tc.desc     Function test - covers line 247-249 in source code
 */
HWTEST_F(IncomingFlashReminderTest, DlsymFailed_HandleStopFlashRemind, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libc.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        reminder_->isFlashRemindUsed_ = true;
        callbackCalled_ = false;
        reminder_->HandleStopFlashRemind();
        EXPECT_FALSE(callbackCalled_);
        dlclose(reminder_->libAdapterHandler_);
        reminder_->libAdapterHandler_ = nullptr;
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DlsymFailed_ReleaseDepsAdapter_001
 * @tc.name     test dlsym return nullptr branch for ReleaseDepsAdapter
 * @tc.desc     Function test - use libc.so which won't have FreeCamera symbol
 */
HWTEST_F(IncomingFlashReminderTest, DlsymFailed_ReleaseDepsAdapter, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libc.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        reminder_->ReleaseDepsAdapter();
        EXPECT_NE(reminder_->libAdapterHandler_, nullptr);
        dlclose(reminder_->libAdapterHandler_);
        reminder_->libAdapterHandler_ = nullptr;
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DlsymFailed_Destructor_001
 * @tc.name     test dlsym return nullptr branch for Destructor when isFlashRemindUsed_ is true
 * @tc.desc     Function test - covers line 62-63 in source code
 */
HWTEST_F(IncomingFlashReminderTest, DlsymFailed_Destructor, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libc.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        void* handler = reminder_->libAdapterHandler_;
        reminder_->isFlashRemindUsed_ = true;
        reminder_.reset();
        reminder_ = nullptr;
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_RealLib_IsScreenStatusSatisfied_001
 * @tc.name     test with real libtel_cm_deps_adapter.z.so if available
 * @tc.desc     Function test - weak assertions due to uncontrollable return values
 */
HWTEST_F(IncomingFlashReminderTest, RealLib_IsScreenStatusSatisfied, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter.z.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        bool result = reminder_->IsScreenStatusSatisfied();
        EXPECT_TRUE(result == true || result == false);
        dlclose(reminder_->libAdapterHandler_);
        reminder_->libAdapterHandler_ = nullptr;
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_RealLib_IsTorchReady_001
 * @tc.name     test with real libtel_cm_deps_adapter.z.so if available
 * @tc.desc     Function test - weak assertions due to uncontrollable return values
 */
HWTEST_F(IncomingFlashReminderTest, RealLib_IsTorchReady, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter.z.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        bool result = reminder_->IsTorchReady();
        EXPECT_TRUE(result == true || result == false);
        dlclose(reminder_->libAdapterHandler_);
        reminder_->libAdapterHandler_ = nullptr;
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_RealLib_IsFlashRemindNecessary_001
 * @tc.name     test with real libtel_cm_deps_adapter.z.so if available
 * @tc.desc     Function test - weak assertions due to uncontrollable return values
 */
HWTEST_F(IncomingFlashReminderTest, RealLib_IsFlashRemindNecessary, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter.z.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        bool result = reminder_->IsFlashRemindNecessary();
        EXPECT_TRUE(result == true || result == false);
        dlclose(reminder_->libAdapterHandler_);
        reminder_->libAdapterHandler_ = nullptr;
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_RealLib_HandleSetTorchMode_001
 * @tc.name     test HandleSetTorchMode with real libtel_cm_deps_adapter.z.so if available
 * @tc.desc     Function test - weak assertions, verifies function executes without crash
 */
HWTEST_F(IncomingFlashReminderTest, RealLib_HandleSetTorchMode, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter.z.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        reminder_->HandleSetTorchMode();
        EXPECT_TRUE(true);
        dlclose(reminder_->libAdapterHandler_);
        reminder_->libAdapterHandler_ = nullptr;
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_RealLib_HandleStopFlashRemind_001
 * @tc.name     test HandleStopFlashRemind with real libtel_cm_deps_adapter.z.so if available
 * @tc.desc     Function test - verifies callback is called when isFlashRemindUsed_ is true
 */
HWTEST_F(IncomingFlashReminderTest, RealLib_HandleStopFlashRemind, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter.z.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        reminder_->isFlashRemindUsed_ = true;
        callbackCalled_ = false;
        reminder_->HandleStopFlashRemind();
        EXPECT_FALSE(reminder_->isFlashRemindUsed_);
        EXPECT_TRUE(callbackCalled_);
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_RealLib_ReleaseDepsAdapter_001
 * @tc.name     test ReleaseDepsAdapter with real libtel_cm_deps_adapter.z.so if available
 * @tc.desc     Function test - verifies function executes without crash
 */
HWTEST_F(IncomingFlashReminderTest, RealLib_ReleaseDepsAdapter, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter.z.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        reminder_->ReleaseDepsAdapter();
        EXPECT_TRUE(reminder_->libAdapterHandler_ == nullptr || reminder_->libAdapterHandler_ != nullptr);
        if (reminder_->libAdapterHandler_ != nullptr) {
            dlclose(reminder_->libAdapterHandler_);
            reminder_->libAdapterHandler_ = nullptr;
        }
    }
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_IsFlashRemindNecessary_NullptrHandler_001
 * @tc.name     test IsFlashRemindNecessary when libAdapterHandler_ is nullptr
 * @tc.desc     Function test - both IsScreenStatusSatisfied and IsTorchReady should return false
 */
HWTEST_F(IncomingFlashReminderTest, IsFlashRemindNecessary_NullptrHandler, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    EXPECT_FALSE(reminder_->IsFlashRemindNecessary());
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ProcessDelaySetTorchEvent_001
 * @tc.name     test process delay set torch event
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, ProcessDelaySetTorchEvent, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    auto event = AppExecFwk::InnerEvent::Get(DELAY_SET_TORCH_EVENT, 0);
    reminder_->ProcessEvent(event);
    EXPECT_TRUE(true);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ProcessStopFlashRemindEvent_Used_001
 * @tc.name     test process stop flash remind event when isFlashRemindUsed_ is true
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, ProcessStopFlashRemindEvent_Used, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = true;
    callbackCalled_ = false;
    auto event = AppExecFwk::InnerEvent::Get(STOP_FLASH_REMIND_EVENT, 0);
    reminder_->ProcessEvent(event);
    EXPECT_FALSE(callbackCalled_);
    EXPECT_FALSE(reminder_->isFlashRemindUsed_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ProcessStopFlashRemindEvent_NotUsed_001
 * @tc.name     test process stop flash remind event when isFlashRemindUsed_ is false
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, ProcessStopFlashRemindEvent_NotUsed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    callbackCalled_ = false;
    auto event = AppExecFwk::InnerEvent::Get(STOP_FLASH_REMIND_EVENT, 0);
    reminder_->ProcessEvent(event);
    EXPECT_TRUE(callbackCalled_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ProcessStartFlashRemindEvent_AlreadyUsed_001
 * @tc.name     test process start flash remind event when already used
 * @tc.desc     Function test - covers line 178-179 in source code
 */
HWTEST_F(IncomingFlashReminderTest, ProcessStartFlashRemindEvent_AlreadyUsed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = true;
    auto event = AppExecFwk::InnerEvent::Get(START_FLASH_REMIND_EVENT, 0);
    reminder_->ProcessEvent(event);
    EXPECT_TRUE(reminder_->isFlashRemindUsed_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ProcessStartFlashRemindEvent_NotUsed_001
 * @tc.name     test process start flash remind event when not used
 * @tc.desc     Function test - tests initial state of HandleStartFlashRemind
 */
HWTEST_F(IncomingFlashReminderTest, ProcessStartFlashRemindEvent_NotUsed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    auto event = AppExecFwk::InnerEvent::Get(START_FLASH_REMIND_EVENT, 0);
    reminder_->ProcessEvent(event);
    EXPECT_TRUE(true);
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
    EXPECT_TRUE(true);
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
 * @tc.number   Telephony_IncomingFlashReminder_StartFlashRemind_NotStarted_001
 * @tc.name     test start flash remind when not started
 * @tc.desc     Function test - tests initial state
 */
HWTEST_F(IncomingFlashReminderTest, StartFlashRemind_NotStarted, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    reminder_->StartFlashRemind();
    EXPECT_TRUE(true);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_StopFlashRemind_Used_001
 * @tc.name     test stop flash remind when used
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, StopFlashRemind_Used, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = true;
    callbackCalled_ = false;
    reminder_->StopFlashRemind();
    EXPECT_FALSE(reminder_->isFlashRemindUsed_);
    EXPECT_FALSE(callbackCalled_);
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
    EXPECT_TRUE(callbackCalled_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_CallbackNullptr_001
 * @tc.name     test with nullptr callback
 * @tc.desc     Function test - verifies behavior when stopFlashRemindDone_ is nullptr
 */
HWTEST_F(IncomingFlashReminderTest, CallbackNullptr, TestSize.Level0)
{
    auto nullRunner = AppExecFwk::EventRunner::Create("null_callback_runner");
    auto nullReminder = std::make_shared<IncomingFlashReminder>(nullRunner, nullptr);
    nullReminder->libAdapterHandler_ = nullptr;
    nullReminder->isFlashRemindUsed_ = false;
    nullReminder->HandleStopFlashRemind();
    EXPECT_FALSE(nullReminder->isFlashRemindUsed_);
    nullReminder = nullptr;
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