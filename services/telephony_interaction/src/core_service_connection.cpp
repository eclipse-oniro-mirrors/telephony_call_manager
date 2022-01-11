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

#include "core_service_connection.h"

#include "call_manager_errors.h"
#include "core_service_client.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
std::vector<std::u16string> CoreServiceConnection::GetFdnNumberList(int slotId)
{
    std::vector<std::u16string> numberVec;
    numberVec.clear();
    std::vector<std::shared_ptr<DiallingNumbersInfo>> diallingNumbers =
        DelayedRefSingleton<CoreServiceClient>::GetInstance().QueryIccDiallingNumbers(
            slotId, DiallingNumbersInfo::SIM_FDN);
    if (diallingNumbers.empty()) {
        TELEPHONY_LOGE("fdn number list is empty");
        return numberVec;
    }
    for (std::vector<std::shared_ptr<DiallingNumbersInfo>>::iterator it = diallingNumbers.begin();
         it != diallingNumbers.end(); it++) {
        numberVec.push_back((*it)->GetNumber());
    }
    return numberVec;
}
} // namespace Telephony
} // namespace OHOS
