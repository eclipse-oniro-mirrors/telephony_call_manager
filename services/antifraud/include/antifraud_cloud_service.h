/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef ANTIFRAUD_CLOUD_SERVICE_H
#define ANTIFRAUD_CLOUD_SERVICE_H

#include <map>
#include <mutex>
#include <string>

#include "anti_fraud_service_client_type.h"
#include "antifraud_hsdr_helper.h"

namespace OHOS {
namespace Telephony {
class AntiFraudCloudService : public std::enable_shared_from_this<AntiFraudCloudService> {
public:
    explicit AntiFraudCloudService(const std::string &phoneNum);
    bool UploadPostRequest(const OHOS::AntiFraudService::AntiFraudResult &antiFraudResult);
private:
    bool isSettled_ = false;
    std::string phoneNum_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::pair<std::string, std::string> EncryptSync(const std::string &metaData, sptr<IRemoteObject> remoteObject);
    std::pair<std::string, std::string> ProcessEncryptResult(const std::string &encryptResult);
    std::string GenerateRequestJson(const std::map<std::string, std::string> &headersMap, const std::string &body);
    std::string GetAuthSync(const std::string &metaData, const std::string &ak, sptr<IRemoteObject> remoteObject);
    std::string GenerateUcsRequestBody(UcsMethod code, const std::string &requestData);
    std::string EncodeBase64(const std::string &src);
    std::string GeneratePayload(const OHOS::AntiFraudService::AntiFraudResult &antiFraudResult);
    std::string GetRomVersion();
    std::string GetSubstringBeforeSymbol(const std::string &str, const std::string &symbol);
    std::string GetDeviceSerial();
    std::string CalculateDigest(const std::string &payload);
    std::string GetOsVersion();
    std::string GenerateRandomString(size_t length);
    std::string ProcessSignResult(const std::string &signResult);
    uint64_t GenerateRandomLong();
    bool ConnectCloudAsync(const std::string &metaData, const std::string &auth,
        const OHOS::AntiFraudService::AntiFraudResult &antiFraudResult, sptr<IRemoteObject> remoteObject);
    std::map<std::string, std::string> GenerateHeadersMap(const std::string &auth, const std::string &fileContent,
        const std::string &boundary);
};
} // namespace Telephony
} // namespace OHOS
#endif