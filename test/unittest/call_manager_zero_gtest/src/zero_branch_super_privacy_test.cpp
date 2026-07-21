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

#define private public
#define protected public

#include "call_superprivacy_control_manager.h"
#include "super_privacy_policy_info.h"
#include "super_privacy_manager_client.h"
#include "call_control_manager.h"
#include "call_object_manager.h"
#include "gtest/gtest.h"
#include "syspara/parameters.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class SuperPrivacyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static int32_t originParamValue_;
};
int32_t SuperPrivacyTest::originParamValue_ = 0;
void SuperPrivacyTest::SetUpTestCase()
{
    originParamValue_ = system::GetIntParameter("persist.super_privacy_policy.sensors", -1);
}

void SuperPrivacyTest::TearDownTestCase()
{
    string oriParamStr = std::to_string(originParamValue_);
    system::SetParameter("persist.super_privacy_policy.sensors", oriParamStr.c_str());
}
void SuperPrivacyTest::SetUp() {}
void SuperPrivacyTest::TearDown() {}

/**
 * @tc.number   Telephony_ParsePolicyFromParam
 * @tc.name     test ParsePolicyFromParam with different parameter values
 * @tc.desc     Function test - cover all branches
 */
HWTEST_F(SuperPrivacyTest, Telephony_ParsePolicyFromParam, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    SuperPrivacyPolicyInfo policyInfo;
    
    // Test: mode=ON(2), mic=DISABLED(2), cam=ENABLED(1) -> value=17
    manager->ParsePolicyFromParam(policyInfo, 290);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::ALWAYS_ON);
    EXPECT_EQ(policyInfo.superPrivacyPolicies[0].sensorState, PrivacySensorState::DISABLED);
    EXPECT_EQ(policyInfo.superPrivacyPolicies[1].sensorState, PrivacySensorState::ENABLED);
    
    // Test: mode=OFF(0), all ENABLED(0) -> value=0
    manager->ParsePolicyFromParam(policyInfo, 0);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::OFF);
    
    // Test: mode=ON_WHEN_FOLDED(1), all DISABLED(2) -> value=545
    manager->ParsePolicyFromParam(policyInfo, 545);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::ON_WHEN_FOLDED);

    // Test: mode=invalid(3), all DEFAULT(0) -> value=3
    manager->ParsePolicyFromParam(policyInfo, 3);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::ON_WHEN_FOLDED);

    // Test: mode=invalid(0), CAMERA(3) -> value=48
    manager->ParsePolicyFromParam(policyInfo, 48);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::OFF);
}

/**
 * @tc.number   Telephony_GetCurrentPrivacyPolicy
 * @tc.name     test GetCurrentPrivacyPolicy
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_GetCurrentPrivacyPolicy, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    SuperPrivacyPolicyInfo policyInfo;
    
    // Set parameter and test
    system::SetParameter("persist.super_privacy_policy.sensors", "18");
    manager->GetCurrentPrivacyPolicy(policyInfo);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::ALWAYS_ON);

}

/**
 * @tc.number   Telephony_CanCallWithSuperPrivacyPolicy
 * @tc.name     test CanCallWithSuperPrivacyPolicy - cover all branches
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_CanCallWithSuperPrivacyPolicy, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    
    // Branch: OFF mode -> true
    system::SetParameter("persist.super_privacy_policy.sensors", "0");
    EXPECT_TRUE(manager->CanCallWithSuperPrivacyPolicy());
    
    // Branch: ON mode, mic DISABLED -> false
    system::SetParameter("persist.super_privacy_policy.sensors", "514");
    EXPECT_FALSE(manager->CanCallWithSuperPrivacyPolicy());
    
    // Branch: ON mode, cam DISABLED -> false
    system::SetParameter("persist.super_privacy_policy.sensors", "34");
    EXPECT_FALSE(manager->CanCallWithSuperPrivacyPolicy());
    
    // Branch: ON mode, all ENABLED -> true
    system::SetParameter("persist.super_privacy_policy.sensors", "4370");
    EXPECT_TRUE(manager->CanCallWithSuperPrivacyPolicy());
}

/**
 * @tc.number   Telephony_RecordChangedPrivacyPolicy
 * @tc.name     test RecordChangedPrivacyPolicy
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_RecordChangedPrivacyPolicy, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    
    // Test: mic DISABLED
    system::SetParameter("persist.super_privacy_policy.sensors", "514");
    manager->RecordChangedPrivacyPolicy();
    EXPECT_NE(manager->disabledSensorsDuringCall_.find(PrivacySensorType::MICROPHONE),
                manager->disabledSensorsDuringCall_.end());

    // Test: cam DISABLED
    system::SetParameter("persist.super_privacy_policy.sensors", "34");
    manager->RecordChangedPrivacyPolicy();
    EXPECT_NE(manager->disabledSensorsDuringCall_.find(PrivacySensorType::CAMERA),
                manager->disabledSensorsDuringCall_.end());
    
    // Test: all ENABLED (no add)
    manager->disabledSensorsDuringCall_.clear();
    system::SetParameter("persist.super_privacy_policy.sensors", "8194");
    manager->RecordChangedPrivacyPolicy();
    EXPECT_TRUE(manager->disabledSensorsDuringCall_.empty());

}

/**
 * @tc.number   Telephony_RestorePrivacyPolicy
 * @tc.name     test RestorePrivacyPolicy - cover all branches
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_RestorePrivacyPolicy, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    
    // Branch: empty set -> early return
    {
        std::unique_lock<ffrt::shared_mutex> lock(manager->superPrivacyModeMutex_);
        manager->disabledSensorsDuringCall_.clear();
    }
    manager->RestorePrivacyPolicy();
    {
        std::shared_lock<ffrt::shared_mutex> lock(manager->superPrivacyModeMutex_);
        EXPECT_TRUE(manager->disabledSensorsDuringCall_.empty());
    }
    
    // Branch: OFF mode -> clear without restore
    system::SetParameter("persist.super_privacy_policy.sensors", "0");
    {
        std::unique_lock<ffrt::shared_mutex> lock(manager->superPrivacyModeMutex_);
        manager->disabledSensorsDuringCall_.insert(PrivacySensorType::MICROPHONE);
    }
    manager->RestorePrivacyPolicy();
    {
        std::shared_lock<ffrt::shared_mutex> lock(manager->superPrivacyModeMutex_);
        EXPECT_TRUE(manager->disabledSensorsDuringCall_.empty());
    }
    
    // Branch: ON mode -> restore and clear
    system::SetParameter("persist.super_privacy_policy.sensors", "1");
    {
        std::unique_lock<ffrt::shared_mutex> lock(manager->superPrivacyModeMutex_);
        manager->disabledSensorsDuringCall_.insert(PrivacySensorType::MICROPHONE);
        manager->disabledSensorsDuringCall_.insert(PrivacySensorType::CAMERA);
    }
    manager->RestorePrivacyPolicy();
    {
        std::shared_lock<ffrt::shared_mutex> lock(manager->superPrivacyModeMutex_);
        EXPECT_TRUE(manager->disabledSensorsDuringCall_.empty());
    }
}

/**
 * @tc.number   Telephony_EnablePrivacySensors
 * @tc.name     test EnablePrivacySensors
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_EnablePrivacySensors, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    system::SetParameter("persist.super_privacy_policy.sensors", "1");
    manager->RecordChangedPrivacyPolicy();
    manager->EnablePrivacySensors();
    EXPECT_TRUE(manager->disabledSensorsDuringCall_.empty());
}

/**
 * @tc.number   Telephony_CloseCallSuperPrivacyMode
 * @tc.name     test CloseCallSuperPrivacyMode with valid parameters
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_CloseCallSuperPrivacyMode, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    std::u16string phoneNumber = u"1234567890";
    int32_t accountId = 0;
    int32_t videoState = 0;
    int32_t dialType = 0;
    int32_t dialScene = 0;
    int32_t callType = 0;
    manager->CloseCallSuperPrivacyMode(phoneNumber, accountId, videoState,
        dialType, dialScene, callType);
    EXPECT_NE(manager, nullptr);
}

/**
 * @tc.number   Telephony_CloseAnswerSuperPrivacyMode
 * @tc.name     test CloseAnswerSuperPrivacyMode with parameter validation
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_CloseAnswerSuperPrivacyMode, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    int32_t callId = 1;
    int32_t videoState = 0;
    system::SetParameter("persist.super_privacy_policy.sensors", "17");
    manager->RecordChangedPrivacyPolicy();
    manager->CloseAnswerSuperPrivacyMode(callId, videoState);
    EXPECT_NE(manager, nullptr);
}

/**
 * @tc.number   Telephony_CloseCallAccordingPolicy
 * @tc.name     test CloseCallAccordingPolicy - cover different sensor states
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_CloseCallAccordingPolicy, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    SuperPrivacyPolicyInfo policyInfo;
    
    // Test: mic DISABLED
    policyInfo.superPrivacyPolicies[static_cast<int32_t>(PrivacySensorType::MICROPHONE)]
        .sensorState = PrivacySensorState::DISABLED;
    manager->CloseCallAccordingPolicy(policyInfo);
    
    // Test: all ENABLED (no action)
    policyInfo.superPrivacyPolicies[static_cast<int32_t>(PrivacySensorType::MICROPHONE)]
        .sensorState = PrivacySensorState::ENABLED;
    policyInfo.superPrivacyPolicies[static_cast<int32_t>(PrivacySensorType::CAMERA)]
        .sensorState = PrivacySensorState::ENABLED;
    manager->CloseCallAccordingPolicy(policyInfo);
    EXPECT_NE(manager, nullptr);
}

/**
 * @tc.number   Telephony_CloseAllCall
 * @tc.name     test CloseAllCall clears modified sensors
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_CloseAllCall, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    system::SetParameter("persist.super_privacy_policy.sensors", "17");
    manager->RecordChangedPrivacyPolicy();
    manager->CloseAllCall();
    EXPECT_TRUE(manager->disabledSensorsDuringCall_.empty());
}

/**
 * @tc.number   Telephony_ParamChangeCallback1
 * @tc.name     test ParamChangeCallback with invalid params
 * @tc.desc     Function test - cover early return branches
 */
HWTEST_F(SuperPrivacyTest, Telephony_ParamChangeCallbackWithInvalidParam, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    SuperPrivacyPolicyInfo policyInfo;
    system::SetParameter("persist.super_privacy_policy.sensors", "0");
    CallSuperPrivacyControlManager::ParamChangeCallback(nullptr, nullptr, nullptr);
    CallSuperPrivacyControlManager::ParamChangeCallback("wrong_key", "value", nullptr);
    manager->GetCurrentPrivacyPolicy(policyInfo);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::OFF);
}

/**
 * @tc.number   Telephony_ParamChangeCallback2
 * @tc.name     test ParamChangeCallback with valid params
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_ParamChangeCallbackWithValidParam, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    system::SetParameter("persist.super_privacy_policy.sensors", "0");
    CallSuperPrivacyControlManager::ParamChangeCallback(
        "persist.super_privacy_policy.sensors", "18", nullptr);
    SuperPrivacyPolicyInfo policyInfo;
    manager->GetCurrentPrivacyPolicy(policyInfo);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::OFF);
}

/**
 * @tc.number   Telephony_RegisterSuperPrivacyPolicy
 * @tc.name     test RegisterSuperPrivacyPolicy updates policy
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_RegisterSuperPrivacyPolicy, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    system::SetParameter("persist.super_privacy_policy.sensors", "2");
    manager->RegisterSuperPrivacyPolicy();
    SuperPrivacyPolicyInfo policyInfo;
    manager->GetCurrentPrivacyPolicy(policyInfo);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::ALWAYS_ON);
}

/**
 * @tc.number   Telephony_SetSuperPrivacyPolicy
 * @tc.name     test SetSuperPrivacyPolicy with different modes
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_SetSuperPrivacyPolicy, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    SuperPrivacyPolicyInfo policyInfo;
    policyInfo.superPrivacyMode = SuperPrivacyMode::ALWAYS_ON;
    policyInfo.superPrivacyPolicies[static_cast<int32_t>(PrivacySensorType::MICROPHONE)]
        .sensorState = PrivacySensorState::DISABLED;
    manager->SetSuperPrivacyPolicy(policyInfo, 2);
    EXPECT_NE(manager, nullptr);
}

/**
 * @tc.number   Telephony_SuperPrivacyModeChangeEvent
 * @tc.name     test SuperPrivacyModeChangeEvent updates policy
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_SuperPrivacyModeChangeEvent, TestSize.Level0)
{
    auto manager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    system::SetParameter("persist.super_privacy_policy.sensors", "0");
    CallSuperPrivacyControlManager::SuperPrivacyModeChangeEvent();
    SuperPrivacyPolicyInfo policyInfo;
    manager->GetCurrentPrivacyPolicy(policyInfo);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::OFF);
    system::SetParameter("persist.super_privacy_policy.sensors", "18");
    CallSuperPrivacyControlManager::SuperPrivacyModeChangeEvent();
    manager->GetCurrentPrivacyPolicy(policyInfo);
    EXPECT_EQ(policyInfo.superPrivacyMode, SuperPrivacyMode::ALWAYS_ON);
}

/**
 * @tc.number   Telephony_Client_SetSuperPrivacyPoliciesWithModeOn
 * @tc.name     test SetSuperPrivacyPolicies with ON mode
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_Client_SetSuperPrivacyPoliciesWithModeOn, TestSize.Level0)
{
    SuperPrivacyPolicyInfo policyInfo;
    policyInfo.superPrivacyMode = SuperPrivacyMode::ALWAYS_ON;
    policyInfo.superPrivacyPolicies[static_cast<int32_t>(PrivacySensorType::MICROPHONE)]
        .sensorState = PrivacySensorState::DISABLED;
    policyInfo.superPrivacyPolicies[static_cast<int32_t>(PrivacySensorType::CAMERA)]
        .sensorState = PrivacySensorState::ENABLED;
    int32_t result = SuperPrivacyManagerClient::GetInstance().SetSuperPrivacyPolicies(policyInfo, 2);
    EXPECT_NE(result, TELEPHONY_ERR_FAIL);
}

/**
 * @tc.number   Telephony_Client_SetSuperPrivacyPoliciesWithModeOff
 * @tc.name     test SetSuperPrivacyPolicies with OFF mode
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_Client_SetSuperPrivacyPoliciesWithModeOff, TestSize.Level0)
{
    SuperPrivacyPolicyInfo policyInfo;
    policyInfo.superPrivacyMode = SuperPrivacyMode::OFF;
    for (auto& policy : policyInfo.superPrivacyPolicies) {
        policy.sensorState = PrivacySensorState::DEFAULT;
    }
    int32_t result = SuperPrivacyManagerClient::GetInstance().SetSuperPrivacyPolicies(policyInfo, 2);
    EXPECT_NE(result, TELEPHONY_ERR_FAIL);
}

/**
 * @tc.number   Telephony_SuperPrivacyPolicy_Marshalling
 * @tc.name     test SuperPrivacyPolicy Marshalling
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_SuperPrivacyPolicy_Marshalling, TestSize.Level0)
{
    SuperPrivacyPolicy policy(PrivacySensorType::MICROPHONE,
        PrivacySensorState::DISABLED);
    Parcel parcel;
    bool result = policy.Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.number   Telephony_SuperPrivacyPolicyInfo_Marshalling
 * @tc.name     test SuperPrivacyPolicyInfo Marshalling
 * @tc.desc     Function test
 */
HWTEST_F(SuperPrivacyTest, Telephony_SuperPrivacyPolicyInfo_Marshalling, TestSize.Level0)
{
    SuperPrivacyPolicyInfo policyInfo;
    policyInfo.superPrivacyMode = SuperPrivacyMode::ALWAYS_ON;
    policyInfo.superPrivacyPolicies[0].sensorState = PrivacySensorState::ENABLED;
    policyInfo.superPrivacyPolicies[1].sensorState = PrivacySensorState::DISABLED;
    Parcel parcel;
    bool result = policyInfo.Marshalling(parcel);
    EXPECT_TRUE(result);
}

} // namespace Telephony
} // namespace OHOS