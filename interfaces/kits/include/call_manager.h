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

#ifndef CALL_MANAGER_H
#define CALL_MANAGER_H

#include <string>
#include <vector>

#include "iremote_object.h"
#include "pac_map.h"
#include "refbase.h"

#include "call_manager_type.h"
#include "i_call_manager_service.h"
#include "iremote_object.h"
#include "refbase.h"

namespace OHOS {
namespace TelephonyApi {
constexpr int32_t DEFAULT_ERROR = -1;
class CallManager {
public:
    CallManager();
    ~CallManager();
    int32_t DialCall(const std::u16string number, AppExecFwk::PacMap &extras, int32_t &callId);
    int32_t AcceptCall(int32_t callId, int32_t videoState);
    int32_t RejectCall(int32_t callId, bool isSendSms, std::u16string content);
    int32_t HangUpCall(int32_t callId);
    int32_t GetCallState();

private:
    sptr<OHOS::TelephonyCallManager::ICallManagerService> callManagerService_;
    int32_t ConnectService();
};
} // namespace TelephonyApi
} // namespace OHOS
#endif