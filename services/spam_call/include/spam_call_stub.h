/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_SPAM_CALL_STUB_H
#define TELEPHONY_SPAM_CALL_STUB_H

#include <map>

#include "iremote_object.h"
#include "iremote_stub.h"

#include "spam_call_callback.h"

namespace OHOS {
namespace Telephony {
class SpamCallStub : public IRemoteStub<SpamCallCallback> {
public:
    SpamCallStub();
    ~SpamCallStub();
    int32_t OnRemoteRequest(
            uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    static constexpr int COMMAND_DETECT_SPAM_CALL_RESULT = 1;
    static constexpr int COMMAND_DETECT_NEED_NOTIFY_RESULT = 3;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_SPAM_CALL_STUB_H
