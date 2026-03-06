/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "sms_service_manager_client.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

#ifdef __cplusplus
extern "C" {
#endif

int SendMessage(int slotId, const char16_t* desAddr, const char16_t* text) // 此处是否需要调整接口，增加两个传入长度。
{
    return Singleton<SmsServiceManagerClient>::GetInstance()
        .SendMessage(slotId, std::u16string(desAddr), std::u16string(u""), std::u16string(text), nullptr, nullptr);
}

#ifdef __cplusplus
}
#endif
} // TELEPHONY
} // OHOS