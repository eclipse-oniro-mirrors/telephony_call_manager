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

namespace OHOS {
namespace Telephony {
class IncomingFlashReminder : public AppExecFwk::EventHandler {
public:
    IncomingFlashReminder(const std::shared<AppExecFwk::EventRunner> &runner);
    ~IncomingFlashReminder();
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    bool IsFlashRemindNecessary();
    void StartFlashRemind();
    void StopFlashRemind();
private:
    bool IsFlashReminderSwicthOn();
    bool IsScreenStatusSatisfied();
    bool IsTorchReady();
    void HandleSetTorchMode();
    void HandleStopFlashRemind();
    void HandleStartFlashRemind();
    bool isFlashRemindUsed_ = false;
};
} // namespace Telephony
} // namespace OHOS
#endif