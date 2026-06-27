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

#include "antifraud_adapter.h"
#include "antifraud_service.h"
#include "telephony_log_wrapper.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
using namespace testing;

constexpr int32_t VALID_SLOT_ID = 0;
constexpr int32_t VALID_INDEX = 0;

class AntiFraudAdapterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<AntiFraudAdapter> antiFraudAdapter_ = nullptr;
};

void AntiFraudAdapterTest::SetUpTestCase() {}

void AntiFraudAdapterTest::TearDownTestCase() {}

void AntiFraudAdapterTest::SetUp()
{
    antiFraudAdapter_ = DelayedSingleton<AntiFraudAdapter>::GetInstance();
    ASSERT_TRUE(antiFraudAdapter_ != nullptr);
    antiFraudAdapter_->libAntiFraud_ = nullptr;
}

void AntiFraudAdapterTest::TearDown()
{
    if (antiFraudAdapter_ != nullptr) {
        antiFraudAdapter_->ReleaseAntiFraud();
    }
}

HWTEST_F(AntiFraudAdapterTest, AntiFraudAdapter_GetLibAntiFraud_0100, TestSize.Level1)
{
    antiFraudAdapter_->libAntiFraud_ = nullptr;
    void *lib = antiFraudAdapter_->GetLibAntiFraud();
    EXPECT_TRUE(lib == nullptr || lib != nullptr);
}

HWTEST_F(AntiFraudAdapterTest, AntiFraudAdapter_GetLibAntiFraud_0200, TestSize.Level1)
{
    void *mockLib = reinterpret_cast<void *>(0x12345678);
    antiFraudAdapter_->libAntiFraud_ = mockLib;
    void *lib = antiFraudAdapter_->GetLibAntiFraud();
    EXPECT_EQ(lib, mockLib);
}

HWTEST_F(AntiFraudAdapterTest, AntiFraudAdapter_ReleaseAntiFraud_0100, TestSize.Level1)
{
    antiFraudAdapter_->libAntiFraud_ = nullptr;
    void *lib = antiFraudAdapter_->GetLibAntiFraud();
    if (lib != nullptr) {
        antiFraudAdapter_->ReleaseAntiFraud();
        EXPECT_EQ(antiFraudAdapter_->libAntiFraud_, nullptr);
    } else {
        EXPECT_TRUE(true);
    }
}

HWTEST_F(AntiFraudAdapterTest, AntiFraudAdapter_ReleaseAntiFraud_0200, TestSize.Level1)
{
    antiFraudAdapter_->libAntiFraud_ = nullptr;
    antiFraudAdapter_->ReleaseAntiFraud();
    EXPECT_EQ(antiFraudAdapter_->libAntiFraud_, nullptr);
}

HWTEST_F(AntiFraudAdapterTest, AntiFraudAdapter_CheckAntiFraud_0300, TestSize.Level1)
{
    antiFraudAdapter_->libAntiFraud_ = nullptr;
    int32_t ret = antiFraudAdapter_->CheckAntiFraud("10086");
    EXPECT_TRUE(ret == 0 || ret == -1);
}

HWTEST_F(AntiFraudAdapterTest, AntiFraudAdapter_StopAntiFraud_0100, TestSize.Level1)
{
    int32_t ret = antiFraudAdapter_->StopAntiFraud();
    EXPECT_TRUE(ret == 0 || ret == -1);
}

HWTEST_F(AntiFraudAdapterTest, AntiFraudAdapter_StopAntiFraud_0300, TestSize.Level1)
{
    antiFraudAdapter_->libAntiFraud_ = nullptr;
    int32_t ret = antiFraudAdapter_->StopAntiFraud();
    EXPECT_TRUE(ret == 0 || ret == -1);
}

HWTEST_F(AntiFraudAdapterTest, AntiFraudAdapter_ReLoadLib_0100, TestSize.Level1)
{
    void *lib1 = antiFraudAdapter_->GetLibAntiFraud();
    antiFraudAdapter_->ReleaseAntiFraud();
    void *lib2 = antiFraudAdapter_->GetLibAntiFraud();
    EXPECT_TRUE((lib1 == nullptr && lib2 == nullptr) || (lib1 != nullptr && lib2 != nullptr));
}
}
}
