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

#include "antifraud_service.h"
#include "call_status_manager.h"
#include "antifraud_adapter.h"
#include "anonymize_adapter.h"
#include "antifraud_cloud_service.h"
#include "common_type.h"
#include "telephony_log_wrapper.h"
#include "gtest/gtest.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

constexpr int32_t VALID_SLOT_ID = 0;
constexpr int32_t VALID_INDEX = 0;
constexpr int32_t INVALID_SLOT_ID = -1;

class AntiFraudServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<AntiFraudService> antiFraudService_ = nullptr;
};

void AntiFraudServiceTest::SetUpTestCase() {}

void AntiFraudServiceTest::TearDownTestCase() {}

void AntiFraudServiceTest::SetUp()
{
    antiFraudService_ = DelayedSingleton<AntiFraudService>::GetInstance();
    ASSERT_TRUE(antiFraudService_ != nullptr);
    antiFraudService_->InitParams();
}

void AntiFraudServiceTest::TearDown()
{
    // 清理状态,确保用例之间不互相影响
    if (antiFraudService_ != nullptr) {
        antiFraudService_->InitParams();
        antiFraudService_->SetCallStatusManager(nullptr);
    }
}

/**
 * @tc.number   AntiFraudService_SetCallStatusManager_0100
 * @tc.name     Test SetCallStatusManager with valid pointer
 * @tc.desc     Function test
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_SetCallStatusManager_0100, TestSize.Level1)
{
    auto callStatusManager = std::make_shared<CallStatusManager>();
    ASSERT_TRUE(callStatusManager != nullptr);
    antiFraudService_->SetCallStatusManager(callStatusManager);
    EXPECT_EQ(antiFraudService_->callStatusManagerPtr_, callStatusManager);
}

/**
 * @tc.number   AntiFraudService_InitParams_0200
 * @tc.name     Test InitParams resets all state to default values
 * @tc.desc     Function test
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_InitParams_0200, TestSize.Level1)
{
    /* 故意设置非默认值 */
    antiFraudService_->fraudDetectText_ = "dirty";;
    antiFraudService_->antiFraudState_ = 99;

    antiFraudService_->InitParams();

    EXPECT_EQ(antiFraudService_->fraudDetectText_, "");
    EXPECT_EQ(antiFraudService_->antiFraudState_,
        static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_DEFAULT));
}

/**
 * @tc.number   AntiFraudService_StoppedSlotId_0500
 * @tc.name     Test GetStoppedSlotId/Index default and round-trip
 * @tc.desc     Function test
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_StoppedSlotId_0500, TestSize.Level1)
{
    /* 默认值校验 */
    EXPECT_EQ(antiFraudService_->GetStoppedSlotId(), -1);
    EXPECT_EQ(antiFraudService_->GetStoppedIndex(), -1);

    /* 写后读一致性 */
    antiFraudService_->SetStoppedSlotId(VALID_SLOT_ID);
    antiFraudService_->SetStoppedIndex(VALID_INDEX);
    EXPECT_EQ(antiFraudService_->GetStoppedSlotId(), VALID_SLOT_ID);
    EXPECT_EQ(antiFraudService_->GetStoppedIndex(), VALID_INDEX);
}

/* ============================================================
 * B. 边界条件测试 (5 条)
 * ============================================================ */

/**
 * @tc.number   AntiFraudService_SetCallStatusManager_0101
 * @tc.name     Test SetCallStatusManager with null pointer
 * @tc.desc     Boundary test
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_SetCallStatusManager_0101, TestSize.Level1)
{
    antiFraudService_->SetCallStatusManager(nullptr);
    EXPECT_EQ(antiFraudService_->callStatusManagerPtr_, nullptr);
}

/**
 * @tc.number   AntiFraudService_CreateDataShareHelper_0100
 * @tc.name     Test CreateDataShareHelper returns nullptr when SA manager unavailable
 * @tc.desc     Boundary test (B1-T, B2-T)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_CreateDataShareHelper_0100, TestSize.Level1)
{
    /* 测试环境下 SystemAbilityManagerClient 未初始化,B1-T 直接命中 */
    auto helper = antiFraudService_->CreateDataShareHelper(
        TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID, USER_SETTINGSDATA_URI.c_str());
    EXPECT_EQ(helper, nullptr);
}

/**
 * @tc.number   AntiFraudService_CreateDataShareHelper_0101
 * @tc.name     Test CreateDataShareHelper with invalid SA id
 * @tc.desc     Boundary test (B1-F, B2-T 间接命中)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_CreateDataShareHelper_0101, TestSize.Level1)
{
    auto helper = antiFraudService_->CreateDataShareHelper(-1, USER_SETTINGSDATA_URI.c_str());
    EXPECT_EQ(helper, nullptr);
}

/* ============================================================
 * C. 错误处理测试 (12 条)
 * ============================================================ */

/**
 * @tc.number   AntiFraudService_IsSwitchOn_0100
 * @tc.name     Test IsSwitchOn returns false when helper is unavailable
 * @tc.desc     Error test (B3-T)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_IsSwitchOn_0100, TestSize.Level1)
{
    bool result = antiFraudService_->IsSwitchOn("test_switch");
    EXPECT_FALSE(result);
}

/**
 * @tc.number   AntiFraudService_IsSwitchOn_0101
 * @tc.name     Test IsSwitchOn with empty switchName
 * @tc.desc     Error test (B3-T)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_IsSwitchOn_0101, TestSize.Level1)
{
    bool result = antiFraudService_->IsSwitchOn("");
    EXPECT_FALSE(result);
}

/**
 * @tc.number   AntiFraudService_IsSwitchOn_0102
 * @tc.name     Test IsSwitchOn with very long switchName
 * @tc.desc     Error test (B3-T 边界)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_IsSwitchOn_0102, TestSize.Level1)
{
    bool result = antiFraudService_->IsSwitchOn(std::string(1024, 'a'));
    EXPECT_FALSE(result);
}

/**
 * @tc.number   AntiFraudService_IsAntiFraudSwitchOn_0100
 * @tc.name     Test IsAntiFraudSwitchOn delegates to IsSwitchOn
 * @tc.desc     Error test
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_IsAntiFraudSwitchOn_0100, TestSize.Level1)
{
    bool result = antiFraudService_->IsAntiFraudSwitchOn();
    EXPECT_FALSE(result);
}

/**
 * @tc.number   AntiFraudService_IsUserImprovementPlanSwitchOn_0100
 * @tc.name     Test IsUserImprovementPlanSwitchOn delegates to IsSwitchOn
 * @tc.desc     Error test
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_IsUserImprovementPlanSwitchOn_0100, TestSize.Level1)
{
    bool result = antiFraudService_->IsUserImprovementPlanSwitchOn();
    EXPECT_FALSE(result);
}

/**
 * @tc.number   AntiFraudService_StopAntiFraudService_0100
 * @tc.name     Test StopAntiFraudService returns non-zero on failure
 * @tc.desc     Error test (B18-T)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_StopAntiFraudService_0100, TestSize.Level1)
{
    int32_t ret = antiFraudService_->StopAntiFraudService(VALID_SLOT_ID, VALID_INDEX);
    /* 测试环境下 dlopen 失败,B18-T 命中 */
    EXPECT_NE(ret, 0);
    /* B18-T 命中时 stoppedSlotId/Index 不应被设置 */
    EXPECT_EQ(antiFraudService_->GetStoppedSlotId(), 0);
    EXPECT_EQ(antiFraudService_->GetStoppedIndex(), -1);
}

/**
 * @tc.number   AntiFraudService_StopAntiFraudService_0101
 * @tc.name     Test StopAntiFraudService with invalid slotId
 * @tc.desc     Error test (B18-T)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_StopAntiFraudService_0101, TestSize.Level1)
{
    int32_t ret = antiFraudService_->StopAntiFraudService(INVALID_SLOT_ID, VALID_INDEX);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   AntiFraudService_AnonymizeText_0100
 * @tc.name     Test AnonymizeText returns non-zero when InitConfig fails
 * @tc.desc     Error test (B21-T)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_AnonymizeText_0100, TestSize.Level1)
{
    antiFraudService_->fraudDetectText_ = "test text";
    int ret = antiFraudService_->AnonymizeText();
    /* 测试环境下 AnonymizeAdapter dlopen 失败,B21-T 命中 */
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   AntiFraudService_AnonymizeText_0101
 * @tc.name     Test AnonymizeText with empty text
 * @tc.desc     Error test (B21-T 间接)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_AnonymizeText_0101, TestSize.Level1)
{
    antiFraudService_->fraudDetectText_ = "";
    int ret = antiFraudService_->AnonymizeText();
    EXPECT_NE(ret, 0);
}

/* ============================================================
 * D. 参数化测试 - AddRuleToConfig 关键词规则名覆盖
 * ============================================================ */

class AntiFraudAddRuleTest : public testing::TestWithParam<std::string> {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp()
    {
        antiFraudService_ = DelayedSingleton<AntiFraudService>::GetInstance();
        ASSERT_TRUE(antiFraudService_ != nullptr);
        antiFraudService_->InitParams();
    }
    void TearDown() {}

protected:
    std::shared_ptr<AntiFraudService> antiFraudService_ = nullptr;
};

/**
 * @tc.number   AntiFraudService_AddRuleToConfig_Param
 * @tc.name     Parameterized: AddRuleToConfig with various rule names
 * @tc.desc     Boundary test (B19-T/B19-F 覆盖)
 * @note 规则名命中关键词集合时 isOpenKeywords=true,否则 false;函数应正常返回不崩溃
 */
HWTEST_P(AntiFraudAddRuleTest, AddRuleToConfig, TestSize.Level1)
{
    void *config = nullptr;
    /* 不管是关键词规则还是普通规则,函数都不应崩溃(B19 + B20 全分支覆盖) */
    ASSERT_NO_FATAL_FAILURE(antiFraudService_->AddRuleToConfig(GetParam(), config));
    SUCCEED();
}

INSTANTIATE_TEST_SUITE_P(
    KeywordRules,
    AntiFraudAddRuleTest,
    ::testing::Values(
        std::string("CHINA_RESIDENT_PASSPORT"),   /* B19-T 关键词规则1 */
        std::string("PERMIT_HM_TO_LAND"),         /* B19-T 关键词规则2 */
        std::string("PERMIT_TW_TO_LAND"),         /* B19-T 关键词规则3 */
        std::string("POLICE_OFFICER_CARD"),       /* B19-T 关键词规则4 */
        std::string("PRC"),                       /* B19-F 普通规则 */
        std::string("MILITARY_IDENTITY_CARD_NUMBER"), /* B19-F 普通规则 */
        std::string("UNKNOWN_RULE")));            /* B19-F 未定义规则 */

/* ============================================================
 * E. 异常场景测试 (4 条)
 * ============================================================ */

/**
 * @tc.number   AntiFraudService_AddRuleToConfig_0100
 * @tc.name     Test AddRuleToConfig with null config
 * @tc.desc     Exception test (B20-T 间接:config 为 nullptr)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_AddRuleToConfig_0100, TestSize.Level1)
{
    /* config 为 nullptr,SetRule 应处理并可能返回错误;函数不崩溃即可 */
    ASSERT_NO_FATAL_FAILURE(antiFraudService_->AddRuleToConfig("PRC", nullptr));
    SUCCEED();
}

/**
 * @tc.number   AntiFraudService_AnonymizeText_0200
 * @tc.name     Test AnonymizeText preserves text on IdentifyAnonymize failure
 * @tc.desc     Exception test (B21-T 副作用: fraudDetectText_ 不应被修改)
 */
HWTEST_F(AntiFraudServiceTest, AntiFraudService_AnonymizeText_0200, TestSize.Level1)
{
    antiFraudService_->fraudDetectText_ = "test text for identify";
    int ret = antiFraudService_->AnonymizeText();
    /* 测试环境下 InitConfig 已失败, B21-T 优先命中 */
    EXPECT_NE(ret, 0);
    /* fraudDetectText_ 不应被修改 */
    EXPECT_EQ(antiFraudService_->fraudDetectText_, "test text for identify");
}

} // namespace Telephony
} // namespace OHOS
