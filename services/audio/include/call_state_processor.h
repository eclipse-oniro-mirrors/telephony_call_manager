/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_CALL_STATE_PROCESSOR_H
#define TELEPHONY_CALL_STATE_PROCESSOR_H

#include <mutex>
#include <set>
#include <string>

#include "call_manager_inner_type.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
constexpr uint16_t EMPTY_VALUE = 0;
constexpr uint16_t EXIST_ONLY_ONE_CALL = 1;

class CallStateProcessor : public std::enable_shared_from_this<CallStateProcessor> {
    DECLARE_DELAYED_SINGLETON(CallStateProcessor)
public:
    void AddCall(int32_t callId, TelCallState state);
    void DeleteCall(int32_t callId, TelCallState state);
    bool UpdateCurrentCallState();
    int32_t GetCurrentActiveCall();
    int32_t GetCallNumber(TelCallState state);
    bool ShouldSwitchState(TelCallState callState);
    int32_t GetAudioForegroundLiveCall();

private:
    std::set<int32_t> activeCalls_;
    std::set<int32_t> holdingCalls_;
    std::set<int32_t> alertingCalls_;
    std::set<int32_t> incomingCalls_;
    std::set<int32_t> dialingCalls_;
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_CALL_STATE_PROCESSOR_H