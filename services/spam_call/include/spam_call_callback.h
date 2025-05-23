/*
 * Copyright (C) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_SPAM_CALL_CALLBACK_H
#define TELEPHONY_SPAM_CALL_CALLBACK_H

#include <iremote_broker.h>

namespace OHOS {
namespace Telephony {
class SpamCallCallback : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"idl.ISpamResultCallback");

    virtual int32_t OnResult(int32_t &errCode, std::string &result) = 0;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_SPAM_CALL_CALLBACK_H