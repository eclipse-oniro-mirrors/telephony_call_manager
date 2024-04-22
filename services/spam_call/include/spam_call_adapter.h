/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_SPAM_CALL_ADAPTER_MANAGER_H
#define TELEPHONY_SPAM_CALL_ADAPTER_MANAGER_H

#include <string>

#include "spam_call_connection.h"
#include "call_manager_info.h"
#include "singleton.h"
#include "cJSON.h"

#define JSON_GET_NUMBER_VALUE(json, key, cursor, out, finally)                                                         \
    do {                                                                                                               \
        (cursor) = cJSON_GetObjectItem(json, key.c_str());                                                             \
        if (!cJSON_IsNumber(cursor)) {                                                                                 \
            TELEPHONY_LOGE("ParseToResponsePart failed to get %{public}s", key.c_str());                               \
            if (key == "detectResult" || key == "decisionReason") {                                                    \
                finally;                                                                                               \
            }                                                                                                          \
        }                                                                                                              \
        (out) = static_cast<int32_t>(cJSON_GetNumberValue(cursor));                                                    \
    } while (0)

#define JSON_GET_STRING_VALUE(json, key, cursor, out, finally)                                                         \
    do {                                                                                                               \
        (cursor) = cJSON_GetObjectItem(json, key.c_str());                                                             \
        if (!cJSON_IsString(cursor)) {                                                                                 \
            TELEPHONY_LOGE("ParseToResponsePart failed to get %{public}s", key.c_str());                               \
        }                                                                                                              \
        (out) = cJSON_GetStringValue(cursor);                                                                          \
    } while (0)

namespace OHOS {
namespace Telephony {
class SpamCallAdapter {
    DECLARE_DELAYED_SINGLETON(SpamCallAdapter)
public:
    bool DetectSpamCall(const std::string &phoneNumber, const int32_t &slotId);
    bool GetDetectFlag();
    void SetDetectFlag(bool isDetected);
    void GetDetectResult(int32_t &errCode, std::string &result);
    void SetDetectResult(int32_t &errCode, std::string &result);
    void NotifyAll();
    bool WaitForDetectResult();
    void ParseDetectResult(const std::string &jsonData, bool &isBlock, NumberMarkInfo &info, int32_t &blockReason);

private:
    bool ConnectSpamCallAbility(const AAFwk::Want &want, const std::string &phoneNumber, const int32_t &slotId);
    sptr<SpamCallConnection> connection_ {nullptr};
    bool isDetected_ = false;
    static std::condition_variable cv_;
    std::mutex mutex_;
    int32_t errCode_ = -1;
    std::string result_ = "";
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_SPAM_CALL_ADAPTER_MANAGER_H