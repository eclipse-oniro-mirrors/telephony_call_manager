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

#ifndef CALL_MANAGER_CONFIG_H
#define CALL_MANAGER_CONFIG_H

#include <string>
#include <unordered_set>
#include <cJSON.h>
#include <atomic>
#include <mutex>
#include <memory>
#include "ffrt.h"

namespace OHOS {
namespace Telephony {

class CallManagerConfig {
public:
    static bool ShouldHangUpCellularCallBeforeAnswer(int32_t uid);
    static bool ShouldConvertUsernameToPhoneNum(int32_t uid);

private:
    static void Init();
    static int32_t ParserConfigJson();
    static int32_t LoaderJsonFile(std::unique_ptr<char[]> &content, const std::string &path);
    static std::string ParseString(const cJSON *value);
    static void ParseFeatureList(
        const cJSON *root, const std::string &itemName, std::unordered_set<std::string> &featureList);
    static std::string BuildMatchKey(int32_t uid, const std::string &methodName);

    static std::unordered_set<std::string> hangUpCellularCallBeforeAnswerList_;
    static std::unordered_set<std::string> convertUsernameToPhoneNumList_;
    static std::atomic<bool> isInit_;
    static ffrt::mutex mutex_;
};

} // namespace Telephony
} // namespace OHOS
#endif