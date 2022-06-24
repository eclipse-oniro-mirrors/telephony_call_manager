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

#ifndef CALL_HISYSEVENT_H
#define CALL_HISYSEVENT_H

#include <string>

namespace OHOS {
namespace Telephony {
static const std::string DOMAIN_CALL_MANAGER = "CALL_MANAGER";
// EVENT
static const std::string CALL_STATE_CHANGED_EVENT = "PHONE_STATE";

static const std::string CALL_STATE = "state";
static const std::string SLOT_ID = "slotId";

class CallHisysevent {
public:
    template<typename... Types>
    static void CallWriteEvent(const std::string &eventName, Types... args);
    static void HiSysEventWriteCallState(const int32_t slotId, const int32_t state);
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_HISYSEVENT_H
