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

#include "call_manager_info.h"
#include <memory>
#define PRIVATE public
#define PROTECTED public
#include "gtest/gtest.h"
#include "spam_call_adapter.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class SpamCallTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void SpamCallTest::SetUpTestCase() {}

void SpamCallTest::TearDownTestCase() {}

void SpamCallTest::SetUp() {}

void SpamCallTest::TearDown() {}

/**
 * @tc.number   Telephony_SpamCallAdapter_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_SpamCallAdapter_001, Function | MediumTest | Level1)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter_ = std::make_shared<SpamCallAdapter>();
    int32_t errCode = -1;
    std::string result = "";
    spamCallAdapter_->GetDetectResult(errCode, result);
    ASSERT_EQ(errCode, -1);
    ASSERT_EQ(result, "");
}

/**
 * @tc.number   Telephony_SpamCallAdapter_002
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_SpamCallAdapter_002, Function | MediumTest | Level1)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter_ = std::make_shared<SpamCallAdapter>();
    const std::string phoneNumber = "12345678900";
    const int32_t slotId = 0;
    bool ret = spamCallAdapter_->DetectSpamCall(phoneNumber, slotId);
    ASSERT_FALSE(ret);
}

/**
 * @tc.number   Telephony_SpamCallAdapter_003
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(SpamCallTest, Telephony_SpamCallAdapter_003, Function | MediumTest | Level1)
{
    std::shared_ptr<SpamCallAdapter> spamCallAdapter_ = std::make_shared<SpamCallAdapter>();
    std::string jsonData = "{\"detectResult\":1,\"decisionReason\":1}";
    NumberMarkInfo info = {
        .markType = MarkType::MARK_TYPE_NONE,
        .markContent = "",
        .markCount = 0,
        .markSource = "",
        .isCloud = false,
    };
    bool isBlock = false;
    int32_t blockReason = 0;
    spamCallAdapter_->ParseDetectResult(jsonData, isBlock, info, blockReason);
    ASSERT_TRUE(isBlock);
    ASSERT_EQ(blockReason, 1);
}
}
}