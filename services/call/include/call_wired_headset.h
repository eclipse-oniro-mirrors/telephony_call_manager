/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_CALL_WIRED_HEADSET_H
#define TELEPHONY_CALL_WIRED_HEADSET_H

#include <memory>
#include "key_event.h"
#include "singleton.h"
#include "key_option.h"

namespace OHOS {
namespace Telephony {
using WiredHeadSetCallback = std::function<void(const std::shared_ptr<OHOS::MMI::KeyEvent>)>;

class CallWiredHeadSet : public std::enable_shared_from_this<CallWiredHeadSet> {
    DECLARE_DELAYED_SINGLETON(CallWiredHeadSet)
public:
    bool Init();
    void DeInit();

    std::shared_ptr<MMI::KeyOption> InitOption(
        const std::set<int32_t> &preKeys, int32_t finalKey, bool isFinalKeyDown, int32_t duration);
    int32_t RegistKeyEvent(int32_t keyCode, bool isFinalKeyDown, const WiredHeadSetCallback &callback);
    void UnRegistKeyEvent(int32_t &subscribeId);
    void DealKeyPressedUp(const std::shared_ptr<OHOS::MMI::KeyEvent>);
    void DealKeyPressedDown(const std::shared_ptr<OHOS::MMI::KeyEvent>);
    void DealKeyShortPressed();
    void DealKeyLongPressed();

private:
    time_t GetCurrentTimeMS();
private:
    bool isProcessed_;
    time_t downFirstTime_;
    int32_t subscribeIdForPressedUp_;
    int32_t subscribeIdForPressedDown_;
    int32_t subscribeIdForMediaPausedUp_;
    int32_t subscribeIdForMediaPauseDown_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_CALL_WIRED_HEADSET_H
