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

#ifndef CALL_FILTER_MANAGER_H
#define CALL_FILTER_MANAGER_H
#include "ffrt.h"
#include <set>
#include "event_handler.h"
#include "event_runner.h"

#include "call_base.h"
#include "call_status_policy.h"
#include "call_data_base_helper.h"

namespace OHOS {
namespace Telephony {
class CallIncomingFilterManager : public RefBase {
public:
    CallIncomingFilterManager();
    ~CallIncomingFilterManager();
    int32_t PackCellularCallInfo(CellularCallInfo &callInfo, const CallDetailInfo &info);
    int32_t DoIncomingFilter(const CallDetailInfo &info);

private:
    std::vector<std::string> phones_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_FILTER_MANAGER_H