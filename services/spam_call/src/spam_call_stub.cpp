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

#include "spam_call_stub.h"

#include <string_ex.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
namespace Telephony {
SpamCallStub::SpamCallStub() {}

SpamCallStub::~SpamCallStub() {}

int32_t SpamCallStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::u16string myDescriptor = SpamCallStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        TELEPHONY_LOGE("descriptor checked fail !");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }
    int32_t errCodeVar = data.ReadInt32();
    std::string resultVar = Str16ToStr8(data.ReadString16());
    TELEPHONY_LOGI("OnReceived, cmd = %{public}u", code);
    switch (code) {
        case COMMAND_DETECT_SPAM_CALL_RESULT:
            OnResult(errCodeVar, resultVar);
            break;
        default:
            TELEPHONY_LOGE("callback failed, default = %d", code);
            break;
    }
    return ERR_NONE;
}
} // namespace Telephony
} // namespace OHOS
