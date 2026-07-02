/*
 * Copyright (C) 2026-2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_I_WATCH_LITE_CALL_MANAGER_H
#define OHOS_I_WATCH_LITE_CALL_MANAGER_H

#include <string>

namespace OHOS {
namespace Telephony {
class IWatchTelephonyNode {
public:
    virtual ~IWatchTelephonyNode() = default;

    virtual int32_t GetCallerStatus(const std::string &phoneNumber, std::string &retResult) = 0;
};

using CreateWatchTelephonyNodeFuncPtr = IWatchTelephonyNode *(*)(void);

} // namespace IWatchTelephonyNode
} // namespace OHOS
#endif // OHOS_I_WATCH_LITE_CALL_MANAGER_H