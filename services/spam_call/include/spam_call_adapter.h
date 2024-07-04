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

#include "call_manager_info.h"
#include "singleton.h"
#include "cJSON.h"
#include "time_wait_helper.h"
#include "ability_connect_callback_stub.h"
#include <memory>
#include "ffrt.h"

namespace OHOS {
namespace Telephony {
class SpamCallAdapter : public std::enable_shared_from_this<SpamCallAdapter> {
public:
    SpamCallAdapter();
    ~SpamCallAdapter();
    bool DetectSpamCall(const std::string &phoneNumber, const int32_t &slotId);
    void GetDetectResult(int32_t &errCode, std::string &result);
    void SetDetectResult(int32_t &errCode, std::string &result);
    void GetParseResult(bool &isBlock, NumberMarkInfo &info, int32_t &blockReason);
    void SetParseResult(bool &isBlock, NumberMarkInfo &info, int32_t &blockReason);
    std::string GetDetectPhoneNum();
    void NotifyAll();
    bool WaitForDetectResult();
    void ParseDetectResult(const std::string &jsonData, bool &isBlock, NumberMarkInfo &info, int32_t &blockReason);

private:
    bool ConnectSpamCallAbility(const AAFwk::Want &want, const std::string &phoneNumber, const int32_t &slotId);
    void DisconnectSpamCallAbility();
    bool JsonGetNumberValue(cJSON *json, const std::string key, int32_t &out);
    bool JsonGetStringValue(cJSON *json, const std::string key, std::string &out);
    bool JsonGetBoolValue(cJSON *json, const std::string key);
    int32_t errCode_ = -1;
    std::string result_ = "";
    std::string phoneNumber_ = "";
    NumberMarkInfo info_ = {
        .markType = MarkType::MARK_TYPE_NONE,
        .markContent = "",
        .markCount = -1,
        .markSource = "",
        .isCloud = false,
    };
    bool isBlock_ = false;
    int32_t blockReason_;
    std::unique_ptr<TimeWaitHelper> timeWaitHelper_ {nullptr};
    ffrt::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_SPAM_CALL_ADAPTER_MANAGER_H