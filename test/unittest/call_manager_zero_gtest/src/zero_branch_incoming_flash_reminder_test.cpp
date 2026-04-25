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

extern void Mock_Reset();
extern void Mock_SetIsTorchSupported(bool);
extern void Mock_SetTorchMode(int);
extern void Mock_SetIsScreenLocked(bool);
extern void Mock_SetSetTorchModeResult(int);
extern void Mock_SetFreeCameraResult(int);
extern int Mock_GetTorchMode();

constexpr uint32_t DELAY_SET_TORCH_EVENT = 1000000;
constexpr uint32_t STOP_FLASH_REMIND_EVENT = 1000001;
constexpr uint32_t START_FLASH_REMIND_EVENT = 1000002;

class IncomingFlashReminderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override
    {
        Mock_Reset();
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
    EXPECT_EQ(Mock_GetTorchMode(), 0);
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
 * @tc.number   Telephony_IncomingFlashReminder_TorchNotSupported_001
 * @tc.name     test torch not supported branch
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, TorchNotSupported, TestSize.Level0)
{
    Mock_SetIsTorchSupported(false);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    EXPECT_FALSE(reminder_->IsTorchReady());
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_TorchAlreadyOn_001
 * @tc.name     test torch already on branch
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, TorchAlreadyOn, TestSize.Level0)
{
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(1);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    EXPECT_FALSE(reminder_->IsTorchReady());
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_TorchReadyOff_001
 * @tc.name     test torch ready with mode off
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, TorchReadyOff, TestSize.Level0)
{
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    EXPECT_TRUE(reminder_->IsTorchReady());
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_TorchReadyAuto_001
 * @tc.name     test torch ready with mode auto
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, TorchReadyAuto, TestSize.Level0)
{
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(2);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    EXPECT_TRUE(reminder_->IsTorchReady());
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ScreenLocked_001
 * @tc.name     test screen locked branch
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, ScreenLocked, TestSize.Level0)
{
    Mock_SetIsScreenLocked(true);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    EXPECT_TRUE(reminder_->IsScreenStatusSatisfied());
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ScreenNotLocked_001
 * @tc.name     test screen not locked branch
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, ScreenNotLocked, TestSize.Level0)
{
    Mock_SetIsScreenLocked(false);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    EXPECT_FALSE(reminder_->IsScreenStatusSatisfied());
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_Necessary_ScreenLocked_TorchReady_001
 * @tc.name     test necessary condition satisfied
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, Necessary_ScreenLocked_TorchReady, TestSize.Level0)
{
    Mock_SetIsScreenLocked(true);
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    EXPECT_TRUE(reminder_->IsFlashRemindNecessary());
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_NotNecessary_ScreenNotLocked_001
 * @tc.name     test not necessary when screen not locked
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, NotNecessary_ScreenNotLocked, TestSize.Level0)
{
    Mock_SetIsScreenLocked(false);
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    EXPECT_FALSE(reminder_->IsFlashRemindNecessary());
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_NotNecessary_TorchNotSupported_001
 * @tc.name     test not necessary when torch not supported
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, NotNecessary_TorchNotSupported, TestSize.Level0)
{
    Mock_SetIsScreenLocked(true);
    Mock_SetIsTorchSupported(false);
    Mock_SetTorchMode(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    EXPECT_FALSE(reminder_->IsFlashRemindNecessary());
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_NotNecessary_TorchAlreadyOn_001
 * @tc.name     test not necessary when torch already on
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, NotNecessary_TorchAlreadyOn, TestSize.Level0)
{
    Mock_SetIsScreenLocked(true);
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(1);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    EXPECT_FALSE(reminder_->IsFlashRemindNecessary());
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_SwitchTorchFromOff_001
 * @tc.name     test switch torch from off to on
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, SwitchTorchFromOff, TestSize.Level0)
{
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(0);
    Mock_SetSetTorchModeResult(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    reminder_->HandleSetTorchMode();
    EXPECT_EQ(Mock_GetTorchMode(), 1);
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_SwitchTorchFromOn_001
 * @tc.name     test switch torch from on to off
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, SwitchTorchFromOn, TestSize.Level0)
{
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(1);
    Mock_SetSetTorchModeResult(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    reminder_->HandleSetTorchMode();
    EXPECT_EQ(Mock_GetTorchMode(), 0);
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_StopWhenUsed_001
 * @tc.name     test stop flash remind when used
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, StopWhenUsed, TestSize.Level0)
{
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(1);
    Mock_SetSetTorchModeResult(0);
    Mock_SetFreeCameraResult(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    reminder_->isFlashRemindUsed_ = true;
    callbackCalled_ = false;
    reminder_->HandleStopFlashRemind();
    EXPECT_EQ(Mock_GetTorchMode(), 0);
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
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(1);
    Mock_SetSetTorchModeResult(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    reminder_->isFlashRemindUsed_ = true;
    reminder_.reset();
    reminder_ = nullptr;
    EXPECT_EQ(Mock_GetTorchMode(), 0);
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
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(0);
    Mock_SetSetTorchModeResult(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    auto event = AppExecFwk::InnerEvent::Get(DELAY_SET_TORCH_EVENT, 0);
    reminder_->ProcessEvent(event);
    EXPECT_EQ(Mock_GetTorchMode(), 1);
    dlclose(reminder_->libAdapterHandler_);
    reminder_->libAdapterHandler_ = nullptr;
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ProcessStopFlashRemindEvent_001
 * @tc.name     test process stop flash remind event
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, ProcessStopFlashRemindEvent, TestSize.Level0)
{
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(1);
    Mock_SetSetTorchModeResult(0);
    Mock_SetFreeCameraResult(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
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
    Mock_SetIsScreenLocked(true);
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(0);
    reminder_->isFlashRemindUsed_ = false;
    auto event = AppExecFwk::InnerEvent::Get(START_FLASH_REMIND_EVENT, 0);
    reminder_->ProcessEvent(event);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_ProcessUnknownEvent_001
 * @tc.name     test process unknown event
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, ProcessUnknownEvent, TestSize.Level0)
{
    auto event = AppExecFwk::InnerEvent::Get(99999, 0);
    reminder_->ProcessEvent(event);
    EXPECT_TRUE(true);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_StartFlashRemind_Normal_001
 * @tc.name     test start flash remind normal flow
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, StartFlashRemind_Normal, TestSize.Level0)
{
    Mock_SetIsScreenLocked(true);
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(0);
    reminder_->isFlashRemindUsed_ = false;
    reminder_->StartFlashRemind();
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_StartFlashRemind_AlreadyStarted_001
 * @tc.name     test start flash remind already started
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, StartFlashRemind_AlreadyStarted, TestSize.Level0)
{
    reminder_->isFlashRemindUsed_ = true;
    reminder_->StartFlashRemind();
    EXPECT_TRUE(reminder_->isFlashRemindUsed_);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_StopFlashRemind_Normal_001
 * @tc.name     test stop flash remind normal flow
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, StopFlashRemind_Normal, TestSize.Level0)
{
    Mock_SetIsTorchSupported(true);
    Mock_SetTorchMode(1);
    Mock_SetSetTorchModeResult(0);
    Mock_SetFreeCameraResult(0);
    reminder_->libAdapterHandler_ = dlopen("libtel_cm_deps_adapter_test.z.so", RTLD_LAZY);
    ASSERT_NE(reminder_->libAdapterHandler_, nullptr);
    reminder_->isFlashRemindUsed_ = true;
    callbackCalled_ = false;
    reminder_->StopFlashRemind();
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_StopFlashRemind_NotUsed_001
 * @tc.name     test stop flash remind not used
 * @tc.desc     Function test
 */
HWTEST_F(IncomingFlashReminderTest, StopFlashRemind_NotUsed, TestSize.Level0)
{
    reminder_->isFlashRemindUsed_ = false;
    callbackCalled_ = false;
    reminder_->StopFlashRemind();
    EXPECT_TRUE(callbackCalled_);
}

} // namespace Telephony
} // namespace OHOS