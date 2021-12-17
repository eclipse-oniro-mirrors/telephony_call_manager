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

#ifndef NAPI_CALL_MANAGER_CALLBACK_H
#define NAPI_CALL_MANAGER_CALLBACK_H

#include <cstdio>

#include "pac_map.h"

#include "call_manager_callback.h"

namespace OHOS {
namespace Telephony {
class NapiCallManagerCallback : public CallManagerCallback {
public:
    NapiCallManagerCallback() = default;
    ~NapiCallManagerCallback() = default;
    int32_t OnCallDetailsChange(const CallAttributeInfo &info) override;
    int32_t OnCallEventChange(const CallEventInfo &info) override;
    int32_t OnSupplementResult(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo) override;
};
} // namespace Telephony
} // namespace OHOS
#endif // NAPI_CALL_MANAGER_CALLBACK_H
