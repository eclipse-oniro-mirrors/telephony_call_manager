/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_RTT_CALL_LISTENER_H
#define TELEPHONY_RTT_CALL_LISTENER_H

#include <cstdint>

#include "call_state_listener_base.h"
#include "ims_call.h"

namespace OHOS {
namespace Telephony {
class RttCallListener : public CallStateListenerBase {
public:
    RttCallListener() = default;
    ~RttCallListener() = default;
    void CallStateUpdated(sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState) override;
    void InitRttManager(sptr<IMSCall> &imsCall);
    void UnInitRttManager();
    int32_t SendRttMessage(const std::string &rttMessage);

private:
    std::shared_ptr<ImsRttManager> rttManager_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_RTT_CALL_LISTENER_H