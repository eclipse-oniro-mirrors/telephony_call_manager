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

#ifndef TELEPHONY_SPAM_CALL_PROXY_H
#define TELEPHONY_SPAM_CALL_PROXY_H

#include <iremote_proxy.h>
#include "i_spam_call.h"
#include "spam_call_adapter.h"
#include <memory>

namespace OHOS {
namespace Telephony {
class SpamCallProxy : public OHOS::IRemoteProxy<ISpamCall> {
public:
    explicit SpamCallProxy(const sptr<IRemoteObject> &impl);

    virtual ~SpamCallProxy() = default;

    int32_t DetectSpamCall(const std::string &phoneNumber, const int32_t &slotId,
        std::shared_ptr<SpamCallAdapter> spamCallAdapter) override;

private:
    static constexpr int COMMAND_DETECT_SPAM_CALL = 1;
    std::shared_ptr<SpamCallAdapter> spamCallAdapter_ {nullptr};
};
} // namespace Telephony
} // namespace OHOS

#endif //TELEPHONY_SPAM_CALL_PROXY_H
