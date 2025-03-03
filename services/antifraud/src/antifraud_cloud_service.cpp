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

#include "antifraud_cloud_service.h"

#include <chrono>
#include <cJSON.h>
#include <limits>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <parameter.h>
#include <parameters.h>
#include <random>
#include <sstream>
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr size_t BOUNDARY_LENGTH = 32;
constexpr size_t REQUEST_NO_LENGTH = 10;
constexpr size_t SERIAL_NUM_LEN = 16;
constexpr int BASE64_NUMBER2 = 2;
constexpr int BASE64_NUMBER3 = 3;
constexpr int BASE64_NUMBER4 = 4;
constexpr int BASE64_NUMBER6 = 6;
constexpr int COMMON_TIME_OUT = 5000;
const std::string BASE_64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const std::string RANDOM_CHAR_SET = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string UPLOAD_MODEL_PATH = "/rms/v1/antiFraud/upload-model-result";
const std::string CLOUD_CONNECT_SERVICE_NAME = "com.huawei.cloud.afs.ROOT";
const std::string UCS_SERVICE_NAME = "com.huawei.hmos.hsdr.antifraud";

AntiFraudCloudService::AntiFraudCloudService(const std::string &phoneNum) : phoneNum_(phoneNum) {}

bool AntiFraudCloudService::UploadPostRequest(const OHOS::AntiFraudService::AntiFraudResult &antiFraudResult)
{
    auto &helper = DelayedRefSingleton<HsdrHelper>().GetInstance();
    std::string metaData = GeneratePayload(antiFraudResult);
    std::weak_ptr<AntiFraudCloudService> weakPtr = shared_from_this();
    std::unique_lock<std::mutex> lock(mutex_);
    isSettled_ = false;
    std::string auth;
    helper.ConnectHsdr([weakPtr, metaData, &auth](sptr<IRemoteObject> remoteObject) {
        auto ptr = weakPtr.lock();
        if (ptr == nullptr) {
            auto &helper = DelayedRefSingleton<HsdrHelper>().GetInstance();
            helper.DisconnectHsdr();
            return;
        }
        std::lock_guard<std::mutex> lock(ptr->mutex_);
        auto pair = ptr->EncryptSync(metaData, remoteObject);
        auto ak = pair.first;
        if (ak.empty()) {
            TELEPHONY_LOGE("Failed to get ak.");
            ptr->isSettled_ = true;
            ptr->cv_.notify_all();
            return;
        }
        auth = ptr->GetAuthSync(metaData, ak, remoteObject);
        ptr->isSettled_ = true;
        ptr->cv_.notify_all();
    });

    while (!isSettled_) {
        if (cv_.wait_for(lock, std::chrono::milliseconds(COMMON_TIME_OUT)) == std::cv_status::timeout) {
            TELEPHONY_LOGE("get auth timeout.");
            helper.DisconnectHsdr();
            return false;
        }
    }

    if (auth.empty()) {
        TELEPHONY_LOGE("Failed to get auth.");
        helper.DisconnectHsdr();
        return false;
    }

    helper.ConnectHsdr([metaData, auth, antiFraudResult, weakPtr](sptr<IRemoteObject> remoteObject) {
        auto ptr = weakPtr.lock();
        if (ptr == nullptr) {
            auto &helper = DelayedRefSingleton<HsdrHelper>().GetInstance();
            helper.DisconnectHsdr();
            return;
        }
        ptr->ConnectCloudAsync(metaData, auth, antiFraudResult, remoteObject);
    });
    return true;
}

std::string AntiFraudCloudService::GeneratePayload(const OHOS::AntiFraudService::AntiFraudResult &antiFraudResult)
{
    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        TELEPHONY_LOGE("Failed to create json object");
        return "";
    }
    cJSON_AddStringToObject(root, "modelCaller", "com.hsdr");
    cJSON_AddStringToObject(root, "reqNo", GenerateRandomString(REQUEST_NO_LENGTH).c_str());
    cJSON_AddStringToObject(root, "calleeId", "15564984");
    cJSON_AddStringToObject(root, "osType", "1");
    cJSON_AddStringToObject(root, "serviceType", "00000002");
    cJSON_AddStringToObject(root, "callerNum", phoneNum_.c_str());
    cJSON_AddStringToObject(root, "markerId", "11");
    cJSON_AddStringToObject(root, "romVer", GetRomVersion().c_str());
    cJSON_AddStringToObject(root, "modelVer", std::to_string(antiFraudResult.modelVersion).c_str());
    cJSON_AddNumberToObject(root, "modelScore", antiFraudResult.pvalue);
    cJSON_AddStringToObject(root, "modelDecision", std::to_string(antiFraudResult.fraudType).c_str());
    char *jsonString = cJSON_Print(root);
    if (jsonString == nullptr) {
        TELEPHONY_LOGE("Failed to generate json string.");
        cJSON_Delete(root);
        return "";
    }
    std::string payload(jsonString);
    cJSON_Delete(root);
    free(jsonString);
    return payload;
}

std::pair<std::string, std::string> AntiFraudCloudService::EncryptSync(const std::string &metaData,
    sptr<IRemoteObject> remoteObject)
{
    if (remoteObject == nullptr) {
        TELEPHONY_LOGE("remoteObject is nullptr.");
        return {"", ""};
    }
    HsdrProxy service(remoteObject);
    std::string encryptResult;
    HsdrRequest encryptReq = { std::to_string(GenerateRandomLong()), UCS_SERVICE_NAME,
        HsdrCommands::COMMAND_UCS_REQUEST, GenerateUcsRequestBody(UcsMethod::Encrypt, metaData) };
    auto ret = service.RequestHsdrServiceSync(encryptReq, encryptResult);
    TELEPHONY_LOGI("RequestHsdrServiceSync ret = %{public}d", ret);

    auto pair = ProcessEncryptResult(encryptResult);
    return pair;
}

std::pair<std::string, std::string> AntiFraudCloudService::ProcessEncryptResult(const std::string &encryptResult)
{
    cJSON *root = cJSON_Parse(encryptResult.c_str());
    if (root == nullptr) {
        TELEPHONY_LOGE("cJSON_Parse failed");
        return {"", ""};
    }
    cJSON *ak = cJSON_GetObjectItem(root, "ak");
    if (ak == nullptr || ak->type != cJSON_String) {
        TELEPHONY_LOGE("ak is not string.");
        cJSON_Delete(root);
        return {"", ""};
    }
    std::string akString(ak->valuestring);
    cJSON *encrypt = cJSON_GetObjectItem(root, "data");
    if (encrypt == nullptr || encrypt->type != cJSON_String) {
        TELEPHONY_LOGE("encrypt is not string.");
        cJSON_Delete(root);
        return {akString, ""};
    }
    std::string encryptString(encrypt->valuestring);
    cJSON_Delete(root);
    return {akString, encryptString};
}

std::string AntiFraudCloudService::GetAuthSync(const std::string &metaData, const std::string &ak,
    sptr<IRemoteObject> remoteObject)
{
    if (remoteObject == nullptr) {
        TELEPHONY_LOGE("remoteObject is nullptr.");
        return "";
    }
    HsdrProxy service(remoteObject);
    auto timestamp = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    std::string toSign = "POST&" + UPLOAD_MODEL_PATH + "&&";
    toSign += metaData + "&";
    toSign += "ak=" + ak + "&";
    toSign += "timestamp=" + timestamp;
    std::string signResult;
    HsdrRequest signReq = { std::to_string(GenerateRandomLong()), UCS_SERVICE_NAME, HsdrCommands::COMMAND_UCS_REQUEST,
        GenerateUcsRequestBody(UcsMethod::Sign, toSign) };
    auto ret = service.RequestHsdrServiceSync(signReq, signResult);
    TELEPHONY_LOGI("RequestHsdrServiceSync ret = %{public}d", ret);
    std::string signature = ProcessSignResult(signResult);
    std::string auth = "EXT-CLOUDSOA-HMAC-SHA256 ak=\"" + ak + "\",timestamp=" + timestamp +
        ",signature=\"" + signature + "\"";
    return auth;
}

std::string AntiFraudCloudService::ProcessSignResult(const std::string &signResult)
{
    cJSON *root = cJSON_Parse(signResult.c_str());
    if (root == nullptr) {
        TELEPHONY_LOGE("cJSON_Parse failed");
        return "";
    }
    cJSON *data = cJSON_GetObjectItem(root, "data");
    if (data == nullptr || data->type != cJSON_String) {
        TELEPHONY_LOGE("signature data is not string.");
        cJSON_Delete(root);
        return "";
    }
    std::string signature(data->valuestring);
    cJSON_Delete(root);
    return signature;
}

std::string AntiFraudCloudService::GenerateUcsRequestBody(UcsMethod code, const std::string &requestData)
{
    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        TELEPHONY_LOGE("Failed to create json object.");
        return "";
    }
    cJSON_AddNumberToObject(root, "method", static_cast<int>(code));
    cJSON_AddStringToObject(root, "data", EncodeBase64(requestData.c_str(), requestData.size()).c_str());
    char *jsonString = cJSON_Print(root);
    if (jsonString == nullptr) {
        TELEPHONY_LOGE("Failed to generate json string.");
        cJSON_Delete(root);
        return "";
    }
    std::string requestBody(jsonString);
    free(jsonString);
    cJSON_Delete(root);
    return requestBody;
}

std::string AntiFraudCloudService::EncodeBase64(const char *bytes, unsigned int size)
{
    std::string result;
    int i = 0;
    int j = 0;
    uint8_t charArray3[BASE64_NUMBER3] = { 0 }; // store 3 byte of bytes_to_encode
    uint8_t charArray4[BASE64_NUMBER4] = { 0 }; // store encoded character to 4 bytes

    while (size--) {
        charArray3[i++] = *(bytes++); // get three bytes (24 bits)
        if (i == BASE64_NUMBER3) {
            // eg. we have 3 bytes as ( 0100 1101, 0110 0001, 0110 1110) --> (010011, 010110, 000101, 101110)
            charArray4[0] = (charArray3[0] & 0xfc) >> BASE64_NUMBER2; // get first 6 bits of first byte
            // get last 2 bits of first byte and first 4 bit of second byte
            charArray4[1] = ((charArray3[0] & 0x03) << BASE64_NUMBER4) + ((charArray3[1] & 0xf0) >> BASE64_NUMBER4);
            // get last 4 bits of second byte and first 2 bits of third byte
            charArray4[BASE64_NUMBER2] = ((charArray3[1] & 0x0f) << BASE64_NUMBER2) +
                ((charArray3[BASE64_NUMBER2] & 0xc0) >> BASE64_NUMBER6);
            charArray4[BASE64_NUMBER3] = charArray3[BASE64_NUMBER2] & 0x3f; // get last 6 bits of third byte

            for (i = 0; (i < BASE64_NUMBER4); i++) {
                result += BASE_64_CHARS[charArray4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < BASE64_NUMBER3; j++) {
            charArray3[j] = '\0';
        }

        charArray4[0] = (charArray3[0] & 0xfc) >> BASE64_NUMBER2;
        charArray4[1] = ((charArray3[0] & 0x03) << BASE64_NUMBER4) + ((charArray3[1] & 0xf0) >> BASE64_NUMBER4);
        charArray4[BASE64_NUMBER2] = ((charArray3[1] & 0x0f) << BASE64_NUMBER2) +
            ((charArray3[BASE64_NUMBER2] & 0xc0) >> BASE64_NUMBER6);

        for (j = 0; (j < i + 1); j++) {
            result += BASE_64_CHARS[charArray4[j]];
        }

        while ((i++ < BASE64_NUMBER3)) {
            result += '=';
        }
    }

    return result;
}

bool AntiFraudCloudService::ConnectCloudAsync(const std::string &metaData, const std::string &auth,
    const OHOS::AntiFraudService::AntiFraudResult &antiFraudResult, sptr<IRemoteObject> remoteObject)
{
    if (remoteObject == nullptr) {
        TELEPHONY_LOGE("remoteObject is nullptr.");
        auto &helper = DelayedRefSingleton<HsdrHelper>().GetInstance();
        helper.DisconnectHsdr();
        return false;
    }
    HsdrProxy service(remoteObject);
    std::string boundary = GenerateRandomString(BOUNDARY_LENGTH);
    std::map<std::string, std::string> headers = GenerateHeadersMap(auth, antiFraudResult.text, boundary);
    std::ostringstream bodyStream;
    bodyStream << "--" << boundary << "\r\n" <<
        "Content-Disposition: form-data; name=\"meta-data\"\r\n" <<
        "Content-Type: application/json\r\n" <<
        "\r\n" <<
        metaData << "\r\n" <<
        "--" << boundary << "\r\n" <<
        "Content-Disposition: form-data; name=\"file-data\"; filename=\"file.txt\"\r\n" <<
        "Content-Type: text/plain\r\n" <<
        "\r\n" <<
        antiFraudResult.text << "\r\n" <<
        "--" << boundary << "--";
    std::string body = bodyStream.str();
    std::string requestJson = GenerateRequestJson(headers, body);
    OnResponse onResponse = [](const HsdrResponse &response) {
        TELEPHONY_LOGI("Onresponse, body: %{public}s", response.body_.c_str());
        auto &helper = DelayedRefSingleton<HsdrHelper>().GetInstance();
        helper.DisconnectHsdr();
    };
    OnError onError = [](int errCode) {
        TELEPHONY_LOGE("error code = %{public}d", errCode);
        auto &helper = DelayedRefSingleton<HsdrHelper>().GetInstance();
        helper.DisconnectHsdr();
    };
    std::string requestId = std::to_string(GenerateRandomLong());
    sptr<HsdrCallbackStub> callbackStub =
        new (std::nothrow) HsdrCallbackStub(requestId, onResponse, onError);
    if (callbackStub == nullptr) {
        TELEPHONY_LOGE("callbackStub is nullptr.");
        auto &helper = DelayedRefSingleton<HsdrHelper>().GetInstance();
        helper.DisconnectHsdr();
        return false;
    }
    HsdrRequest request = { requestId, CLOUD_CONNECT_SERVICE_NAME, HsdrCommands::COMMAND_CLOUD_CONNECT,
        requestJson };
    int ret = service.RequestHsdrServiceAsync(callbackStub, request);
    TELEPHONY_LOGI("RequestHsdrServiceAsync ret = %{public}d", ret);
    return ret == 0;
}

std::map<std::string, std::string> AntiFraudCloudService::GenerateHeadersMap(const std::string &auth,
    const std::string &fileContent, const std::string &boundary)
{
    std::map<std::string, std::string> headers;
    headers["Authorization"] = auth;
    headers["X-Request-ID"] = GetDeviceSerial();
    headers["Content-Digest"] = CalculateDigest(fileContent);
    headers["down-algo"] = "AES-GCM";
    headers["X-HarmonyOS-Version"] = GetHarmonyOsVersion();
    std::string contentType = "multipart/form-data; boundary=" + boundary;
    headers["Content-Type"] = contentType;
    return headers;
}

std::string AntiFraudCloudService::CalculateDigest(const std::string &payload)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, payload.c_str(), payload.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(BASE64_NUMBER2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string AntiFraudCloudService::GenerateRequestJson(const std::map<std::string, std::string> &headersMap,
    const std::string &body)
{
    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        TELEPHONY_LOGE("Failed to create json object.");
        return "";
    }
    cJSON_AddStringToObject(root, "method", "POST");
    cJSON_AddStringToObject(root, "path", UPLOAD_MODEL_PATH.c_str());

    cJSON *headers = cJSON_CreateObject();
    if (headers == nullptr) {
        TELEPHONY_LOGE("Failed to create json object.");
        cJSON_Delete(root);
        return "";
    }
    cJSON *params = cJSON_CreateArray();
    if (params == nullptr) {
        TELEPHONY_LOGE("Failed to create json object.");
        cJSON_Delete(root);
        cJSON_Delete(headers);
        return "";
    }
    for (auto header : headersMap) {
        cJSON *param = cJSON_CreateObject();
        if (param == nullptr) {
            TELEPHONY_LOGE("Failed to create json object.");
            cJSON_Delete(root);
            cJSON_Delete(headers);
            cJSON_Delete(params);
            return "";
        }
        cJSON_AddStringToObject(param, "name", header.first.c_str());
        cJSON_AddStringToObject(param, "value", header.second.c_str());
        cJSON_AddItemToArray(params, param);
    }
    cJSON_AddItemToObject(headers, "params", params);
    cJSON_AddItemToObject(root, "headers", headers);

    cJSON_AddStringToObject(root, "body", body.c_str());
    cJSON_AddNumberToObject(root, "connectTimeout", COMMON_TIME_OUT);
    cJSON_AddNumberToObject(root, "readTimeout", COMMON_TIME_OUT);
    char *str = cJSON_Print(root);
    if (str == nullptr) {
        TELEPHONY_LOGE("Failed to generate json string.");
        cJSON_Delete(root);
        return "";
    }
    std::string requestJson(str);
    cJSON_Delete(root);
    free(str);
    return requestJson;
}

std::string AntiFraudCloudService::GenerateRandomString(size_t length)
{
    std::string randomString;
    randomString.reserve(length);
    for (size_t i = 0; i < length; i++) {
        std::random_device rd;
        std::uniform_int_distribution<size_t> dist(0, RANDOM_CHAR_SET.size() - 1);
        size_t randomNum = dist(rd);
        randomString += RANDOM_CHAR_SET[randomNum];
    }

    return randomString;
}

uint64_t AntiFraudCloudService::GenerateRandomLong()
{
    std::random_device seed;
    std::mt19937_64 gen(seed());
    std::uniform_int_distribution<uint64_t> dis(std::numeric_limits<uint64_t>::min(),
        std::numeric_limits<uint64_t>::max());
    return dis(gen);
}

std::string AntiFraudCloudService::GetHarmonyOsVersion()
{
    return system::GetParameter("const.ohos.fullname", "");
}

std::string AntiFraudCloudService::GetDeviceSerial()
{
    const char *serial = GetSerial();
    if (serial == nullptr) {
        TELEPHONY_LOGE("GetSerial failed.");
        return GenerateRandomString(SERIAL_NUM_LEN);
    }
    std::string deviceSerial(serial);
    return deviceSerial;
}

std::string AntiFraudCloudService::GetRomVersion()
{
    std::string fullVersion = system::GetParameter("const.product.software.version", "");
    return GetSubstringBeforeSymbol(fullVersion, "(");
}

std::string AntiFraudCloudService::GetSubstringBeforeSymbol(const std::string &str, const std::string &symbol)
{
    size_t pos = str.rfind(symbol);
    if (pos != std::string::npos) {
        return str.substr(0, pos);
    } else {
        return str;
    }
}
} // namespace Telephony
} // namsespace OHOS