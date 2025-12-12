/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_IMS_RTT_ERRCODE_H
#define OHOS_IMS_RTT_ERRCODE_H
#include <stdint.h>

namespace OHOS {
namespace Telephony {
enum RttErrorCode {
    RTT_SUCCESS = 0,
    RTT_ERR_FAIL = -46010,
    RTT_ERR_PROXY_CLOSED = -46020
};
} // namespace Telephony
} // namespace OHOS
#endif // OHOS_IMS_RTT_ERRCODE_H