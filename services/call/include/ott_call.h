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

#ifndef OTT_CALL_H
#define OTT_CALL_H
#include "call_base.h"
#include "net_call_base.h"

namespace OHOS {
namespace TelephonyCallManager {
class OTTCall : public CallBase, public NetCallBase {
public:
    OTTCall(const CallInfo &info);
    ~OTTCall();

    int32_t DialCall();
};
} // namespace TelephonyCallManager
} // namespace OHOS

#endif // OTT_CALL_H