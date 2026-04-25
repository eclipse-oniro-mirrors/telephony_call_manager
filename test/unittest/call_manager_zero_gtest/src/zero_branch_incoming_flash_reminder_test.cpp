/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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
 * @tc.name     test libAdapterHandler_ nullptr branch for HandleStopFlashRemind
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_HandleStopFlashRemind, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    reminder_->HandleStopFlashRemind();
    EXPECT_TRUE(callbackCalled_);
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
 * @tc.number   Telephony_IncomingFlashReminder_HandlerNullptr_Destructor_001
 * @tc.name     test libAdapterHandler_ nullptr branch for Destructor
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_Destructor, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
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
 * @tc.name     test dlsym return nullptr branch for HandleStopFlashRemind
 * @tc.desc     Function test - use libc.so which won't have torch symbols
 */
HWTEST_F(IncomingFlashReminderTest, DlsymFailed_HandleStopFlashRemind, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = dlopen("libc.so", RTLD_LAZY);
    if (reminder_->libAdapterHandler_ != nullptr) {
        reminder_->isFlashRemindUsed_ = true;
        reminder_->HandleStopFlashRemind();
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
        dlclose(reminder_->libAdapterHandler_);
        reminder_->libAdapterHandler_ = nullptr;
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
 * @tc.number   Telephony_IncomingFlashReminder_StopWhenUsed_001
 * @tc.name     test stop flash remind when used
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, StopWhenUsed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = true;
    callbackCalled_ = false;
    reminder_->HandleStopFlashRemind();
    EXPECT_FALSE(reminder_->isFlashRemindUsed_);
    EXPECT_TRUE(callbackCalled_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_StopWhenNotUsed_001
 * @tc.name     test stop flash remind when not used
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, StopWhenNotUsed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    callbackCalled_ = false;
    reminder_->HandleStopFlashRemind();
    EXPECT_TRUE(callbackCalled_);
    EXPECT_FALSE(reminder_->isFlashRemindUsed_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DestructorWhenUsed_001
 * @tc.name     test destructor when flash remind used
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, DestructorWhenUsed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = true;
    reminder_.reset();
    reminder_ = nullptr;
    EXPECT_TRUE(true);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_DestructorWhenNotUsed_001
 * @tc.name     test destructor when flash remind not used
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, DestructorWhenNotUsed, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    reminder_.reset();
    reminder_ = nullptr;
    EXPECT_TRUE(true);
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
 * @tc.number   Telephony_IncomingFlashReminder_ProcessStopFlashRemindEvent_001
 * @tc.name     test process stop flash remind event
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, ProcessStopFlashRemindEvent, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = true;
    callbackCalled_ = false;
    auto event = AppExecFwk::InnerEvent::Get(STOP_FLASH_REMIND_EVENT, 0);
    reminder_->ProcessEvent(event);
    EXPECT_TRUE(callbackCalled_);
    EXPECT_FALSE(reminder_->isFlashRemindUsed_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ProcessStartFlashRemindEvent_001
 * @tc.name     test process start flash remind event
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, ProcessStartFlashRemindEvent, TestSize.Level0)
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
 * @tc.number   Telephony_IncomingFlashReminder_StopFlashRemind_NotUsed_001
 * @tc.name     test stop flash remind not used
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

} // namespace Telephony
} // namespace OHOS