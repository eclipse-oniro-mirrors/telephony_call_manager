/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "call_hisysevent.h"

#include "call_manager_errors.h"
#include "hisysevent.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
template<typename... Types>
void CallHisysevent::CallWriteEvent(const std::string &eventName, Types... args)
{
    OHOS::HiviewDFX::HiSysEvent::EventType type;
    if (eventName == CALL_STATE_CHANGED_EVENT) {
        type = HiviewDFX::HiSysEvent::EventType::STATISTIC;
    } else {
        TELEPHONY_LOGE("CallHisysevent::CallWriteEvent the event name is not in the processing scope!");
        return;
    }
    OHOS::HiviewDFX::HiSysEvent::Write(DOMAIN_CALL_MANAGER, eventName, type, args...);
}

void CallHisysevent::HiSysEventWriteCallState(const int32_t slotId, const int32_t state, const int32_t index)
{
    CallWriteEvent(CALL_STATE_CHANGED_EVENT, SLOT_ID_KEY, slotId, CALL_STATE_KEY, state, INDEX_ID_KEY, index);
}
} // namespace Telephony
} // namespace OHOS
