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

#define private public
#define protected public

#include "antifraud_cloud_service.h"
#include "antifraud_hsdr_helper.h"
#include "anti_fraud_service_client_type.h"
#include "telephony_log_wrapper.h"
#include "gtest/gtest.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class AntiFraudCloudServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<AntiFraudCloudService> cloudService_ = nullptr;
};

void AntiFraudCloudServiceTest::SetUpTestCase() {}

void AntiFraudCloudServiceTest::TearDownTestCase() {}

void AntiFraudCloudServiceTest::SetUp()
{
    cloudService_ = std::make_shared<AntiFraudCloudService>("13800138000");
    ASSERT_TRUE(cloudService_ != nullptr);
}

void AntiFraudCloudServiceTest::TearDown()
{
    cloudService_ = nullptr;
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GetSubstringBeforeSymbol_SymbolExists_0100, TestSize.Level1)
{
    std::string result = cloudService_->GetSubstringBeforeSymbol("version(1.0.0)", "(");
    EXPECT_EQ(result, "version");
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GetSubstringBeforeSymbol_SymbolNotExists_0101,
        TestSize.Level1)
{
    std::string result = cloudService_->GetSubstringBeforeSymbol("version1.0.0", "(");
    EXPECT_EQ(result, "version1.0.0");
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GetSubstringBeforeSymbol_EmptyString_0102, TestSize.Level1)
{
    std::string result = cloudService_->GetSubstringBeforeSymbol("", "(");
    EXPECT_EQ(result, "");
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GetSubstringBeforeSymbol_SymbolAtEnd_0103, TestSize.Level1)
{
    std::string result = cloudService_->GetSubstringBeforeSymbol("version(", "(");
    EXPECT_EQ(result, "version");
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ProcessEncryptResult_ParseFailed_0200, TestSize.Level1)
{
    auto pair = cloudService_->ProcessEncryptResult("{invalid json");
    EXPECT_TRUE(pair.first.empty());
    EXPECT_TRUE(pair.second.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ProcessEncryptResult_AkNull_0201, TestSize.Level1)
{
    auto pair = cloudService_->ProcessEncryptResult("{\"data\": \"test\"}");
    EXPECT_TRUE(pair.first.empty());
    EXPECT_TRUE(pair.second.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ProcessEncryptResult_AkNotString_0202, TestSize.Level1)
{
    auto pair = cloudService_->ProcessEncryptResult("{\"ak\": 123}");
    EXPECT_TRUE(pair.first.empty());
    EXPECT_TRUE(pair.second.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ProcessEncryptResult_DataNull_0203, TestSize.Level1)
{
    auto pair = cloudService_->ProcessEncryptResult("{\"ak\": \"test_ak\"}");
    EXPECT_EQ(pair.first, "test_ak");
    EXPECT_TRUE(pair.second.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ProcessEncryptResult_DataNotString_0204, TestSize.Level1)
{
    auto pair = cloudService_->ProcessEncryptResult("{\"ak\": \"test_ak\", \"data\": 123}");
    EXPECT_EQ(pair.first, "test_ak");
    EXPECT_TRUE(pair.second.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ProcessEncryptResult_Success_0205, TestSize.Level1)
{
    auto pair = cloudService_->ProcessEncryptResult("{\"ak\": \"test_ak\", \"data\": \"test_data\"}");
    EXPECT_EQ(pair.first, "test_ak");
    EXPECT_EQ(pair.second, "test_data");
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ProcessSignResult_ParseFailed_0300, TestSize.Level1)
{
    std::string result = cloudService_->ProcessSignResult("{invalid json");
    EXPECT_TRUE(result.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ProcessSignResult_DataNull_0301, TestSize.Level1)
{
    std::string result = cloudService_->ProcessSignResult("{\"other\": \"value\"}");
    EXPECT_TRUE(result.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ProcessSignResult_DataNotString_0302, TestSize.Level1)
{
    std::string result = cloudService_->ProcessSignResult("{\"data\": 123}");
    EXPECT_TRUE(result.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ProcessSignResult_Success_0303, TestSize.Level1)
{
    std::string result = cloudService_->ProcessSignResult("{\"data\": \"test_signature\"}");
    EXPECT_EQ(result, "test_signature");
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_EncryptSync_NullRemoteObject_0400, TestSize.Level1)
{
    auto pair = cloudService_->EncryptSync("test_data", nullptr);
    EXPECT_TRUE(pair.first.empty());
    EXPECT_TRUE(pair.second.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GetAuthSync_NullRemoteObject_0500, TestSize.Level1)
{
    std::string result = cloudService_->GetAuthSync("test_data", "test_ak", nullptr);
    EXPECT_TRUE(result.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_ConnectCloudAsync_NullRemoteObject_0600, TestSize.Level1)
{
    OHOS::AntiFraudService::AntiFraudResult antiFraudResult = {0, true, 1, 0, "test_text", 0.5f};
    bool result = cloudService_->ConnectCloudAsync("test_data", "test_auth", antiFraudResult, nullptr);
    EXPECT_FALSE(result);
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_CalculateDigest_EmptyString_0700, TestSize.Level1)
{
    std::string result = cloudService_->CalculateDigest("");
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 64);
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_CalculateDigest_NormalString_0701, TestSize.Level1)
{
    std::string result = cloudService_->CalculateDigest("test_string");
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 64);
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_CalculateDigest_LongString_0702, TestSize.Level1)
{
    std::string longStr(1024, 'a');
    std::string result = cloudService_->CalculateDigest(longStr);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 64);
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_EncodeBase64_EmptyString_0800, TestSize.Level1)
{
    std::string result = cloudService_->EncodeBase64("");
    EXPECT_TRUE(result.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_EncodeBase64_NormalString_0801, TestSize.Level1)
{
    std::string result = cloudService_->EncodeBase64("test");
    EXPECT_FALSE(result.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GenerateRandomString_ZeroLength_0900, TestSize.Level1)
{
    std::string result = cloudService_->GenerateRandomString(0);
    EXPECT_TRUE(result.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GenerateRandomString_NormalLength_0901, TestSize.Level1)
{
    std::string result = cloudService_->GenerateRandomString(16);
    EXPECT_EQ(result.size(), 16);
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GenerateRandomLong_0903, TestSize.Level1)
{
    uint64_t result = cloudService_->GenerateRandomLong();
    EXPECT_TRUE(result > 0);
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GetOsVersion_1000, TestSize.Level1)
{
    std::string result = cloudService_->GetOsVersion();
    SUCCEED();
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GetRomVersion_1100, TestSize.Level1)
{
    std::string result = cloudService_->GetRomVersion();
    SUCCEED();
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GetDeviceSerial_1200, TestSize.Level1)
{
    std::string result = cloudService_->GetDeviceSerial();
    SUCCEED();
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GenerateHeadersMap_1300, TestSize.Level1)
{
    std::map<std::string, std::string> headers =
        cloudService_->GenerateHeadersMap("test_auth", "test_content", "test_boundary");
    EXPECT_FALSE(headers.empty());
    EXPECT_TRUE(headers.find("Authorization") != headers.end());
    EXPECT_TRUE(headers.find("Content-Type") != headers.end());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GeneratePayload_Success_1400, TestSize.Level1)
{
    OHOS::AntiFraudService::AntiFraudResult antiFraudResult = {0, true, 1, 0, "test_text", 0.5f};
    std::string result = cloudService_->GeneratePayload(antiFraudResult);
    EXPECT_FALSE(result.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GenerateUcsRequestBody_Success_1500, TestSize.Level1)
{
    std::string result = cloudService_->GenerateUcsRequestBody(UcsMethod::Encrypt, "test_data");
    EXPECT_FALSE(result.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_GenerateRequestJson_Success_1600, TestSize.Level1)
{
    std::map<std::string, std::string> headers = {{"key1", "value1"}, {"key2", "value2"}};
    std::string result = cloudService_->GenerateRequestJson(headers, "test_body");
    EXPECT_FALSE(result.empty());
}

HWTEST_F(AntiFraudCloudServiceTest, AntiFraudCloudService_UploadPostRequest_BasicCall_1700, TestSize.Level1)
{
    OHOS::AntiFraudService::AntiFraudResult antiFraudResult = {0, true, 1, 0, "test_text", 0.5f};
    bool result = cloudService_->UploadPostRequest(antiFraudResult);
    EXPECT_FALSE(result);
}
} // namespace Telephony
} // namespace OHOS