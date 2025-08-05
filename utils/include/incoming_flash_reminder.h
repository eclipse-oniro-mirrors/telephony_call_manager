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

#ifndef INCOMING_FLASH_REMINDER_H
#define INCOMING_FLASH_REMINDER_H

#include "event_handler.h"
#include "event_runner.h"
#include <functional>

namespace OHOS {
namespace Telephony {
class IncomingFlashReminder : public AppExecFwk::EventHandler {
public:
    IncomingFlashReminder(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
        std::function<void()> startFlashRemindDone, std::function<void()> stopFlashRemindDone);
    ~IncomingFlashReminder();
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    bool IsFlashRemindNecessary();
    void StartFlashRemind();
    void StopFlashRemind();
private:
    bool IsFlashReminderSwitchOn();
    bool IsScreenStatusSatisfied();
    bool IsTorchReady();
    void HandleSetTorchMode();
    void HandleStopFlashRemind();
    void HandleStartFlashRemind();
    bool isFlashRemindUsed_ = false;
    std::function<void()> startFlashRemindDone_;
    std::function<void()> stopFlashRemindDone_;
};
} // namespace Telephony
} // namespace OHOS
#endif