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
        if (reminder_ != nullptr && reminder_->libAdapterHandler_ != nullptr) {
            dlclose(reminder_->libAdapterHandler_);
            reminder_->libAdapterHandler_ = nullptr;
        }
        reminder_ = nullptr;
        runner_ = nullptr;
    }

protected:
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<IncomingFlashReminder> reminder_;
    bool callbackCalled_ = false;
};

HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_IsScreenStatusSatisfied, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    EXPECT_FALSE(reminder_->IsScreenStatusSatisfied());
}

HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_IsTorchReady, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    EXPECT_FALSE(reminder_->IsTorchReady());
}

HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_HandleSetTorchMode, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->HandleSetTorchMode();
    EXPECT_EQ(Mock_GetTorchMode(), 0);
}

HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_HandleStopFlashRemind, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    reminder_->HandleStopFlashRemind();
    EXPECT_TRUE(callbackCalled_);
}

HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_ReleaseDepsAdapter, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->ReleaseDepsAdapter();
    EXPECT_EQ(reminder_->libAdapterHandler_, nullptr);
}

HWTEST_F(IncomingFlashReminderTest, HandlerNullptr_Destructor, TestSize.Level0)
{
    reminder_->libAdapterHandler_ = nullptr;
    reminder_->isFlashRemindUsed_ = false;
    reminder_.reset();
    reminder_ = nullptr;
    EXPECT_TRUE(true);
}

} // namespace Telephony
} // namespace OHOS