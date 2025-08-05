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

#include "incoming_flash_reminder_callback.h"

namespace OHOS {
namespace Telephony {
IncomingFlashReminderCallback::IncomingFlashReminderCallback(std::function<void()> startFlashRemindDone,
    std::function<void()> stopFlashRemindDone)
    : startFlashRemindDone_(std::move(startFlashRemindDone)), stopFlashRemindDone_(std::move(stopFlashRemindDone)) {}
void IncomingFlashReminderCallback::OnStartFlashRemindDone()
{
    if (startFlashRemindDone_ != nullptr) {
        startFlashRemindDone_();
    }
}

void IncomingFlashReminderCallback::OnStopFlashRemindDone()
{
    if (stopFlashRemindDone_ != nullptr) {
        stopFlashRemindDone_();
    }
}
}
}