/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "motion_recognition.h"
#include "screen_sensor_plugin.h"
#include "audio_control_manager.h"
#include "audio_device_manager.h"
#include "audio_proxy.h"
#include "call_base.h"
#include "call_control_manager.h"
#include "call_object_manager.h"
#include "securec.h"
#include "telephony_log_wrapper.h"

#include "gtest/gtest.h"

#include <memory>

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
// 订阅者类型枚举,用于参数化测试
enum class SubscriberType : int32_t {
    PICKUP = 0,
    FLIP = 1,
    CLOSE_TO_EAR = 2,
};

// 工具:根据类型获取对应的订阅标志
struct SubscriberState {
    bool *subscribedFlag;
    void (*subscribe)();
    void (*unsubscribe)();
};

SubscriberState GetState(SubscriberType type)
{
    switch (type) {
        case SubscriberType::PICKUP:
            return {&MotionPickupSubscriber::isMotionPickupSubscribed_,
                &MotionRecogntion::SubscribePickupSensor,
                &MotionRecogntion::UnsubscribePickupSensor};
        case SubscriberType::FLIP:
            return {&MotionFlipSubscriber::isMotionFlipSubscribed_,
                &MotionRecogntion::SubscribeFlipSensor,
                &MotionRecogntion::UnsubscribeFlipSensor};
        case SubscriberType::CLOSE_TO_EAR:
            return {&MotionCloseToEarSubscriber::isMotionCloseToEarSubscribed_,
                &MotionRecogntion::SubscribeCloseToEarSensor,
                &MotionRecogntion::UnsubscribeCloseToEarSensor};
    }
    return {nullptr, nullptr, nullptr};
}

SubscriberState GetDirectState(SubscriberType type)
{
    switch (type) {
        case SubscriberType::PICKUP:
            return {&MotionPickupSubscriber::isMotionPickupSubscribed_,
                &MotionPickupSubscriber::SubscribePickupMotion,
                &MotionPickupSubscriber::UnsubscribePickupMotion};
        case SubscriberType::FLIP:
            return {&MotionFlipSubscriber::isMotionFlipSubscribed_,
                &MotionFlipSubscriber::SubscribeFlipMotion,
                &MotionFlipSubscriber::UnsubscribeFlipMotion};
        case SubscriberType::CLOSE_TO_EAR:
            return {&MotionCloseToEarSubscriber::isMotionCloseToEarSubscribed_,
                &MotionCloseToEarSubscriber::SubscribeCloseToEarMotion,
                &MotionCloseToEarSubscriber::UnsubscribeCloseToEarMotion};
    }
    return {nullptr, nullptr, nullptr};
}
} // namespace

class MotionRecognitionTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override;
    void TearDown() override;

protected:
    static void ResetSubscriberState();
};

void MotionRecognitionTest::SetUp()
{
    ResetSubscriberState();
}

void MotionRecognitionTest::TearDown()
{
    ResetSubscriberState();
}

void MotionRecognitionTest::ResetSubscriberState()
{
    MotionPickupSubscriber::isMotionPickupSubscribed_ = false;
    MotionFlipSubscriber::isMotionFlipSubscribed_ = false;
    MotionCloseToEarSubscriber::isMotionCloseToEarSubscribed_ = false;
}

class MotionRecogntionApiTest : public MotionRecognitionTest,
                                public testing::WithParamInterface<SubscriberType> {};

HWTEST_P(MotionRecogntionApiTest, SubscribeWhenNotSubscribed, TestSize.Level1)
{
    auto state = GetState(GetParam());
    ASSERT_NE(state.subscribedFlag, nullptr);
    *state.subscribedFlag = false;
    /* 测试环境下 SubscribeCallback 返回 false,标志位保持 false */
    state.subscribe();
    EXPECT_FALSE(*state.subscribedFlag);
}

HWTEST_P(MotionRecogntionApiTest, SubscribeWhenAlreadySubscribed, TestSize.Level1)
{
    auto state = GetState(GetParam());
    ASSERT_NE(state.subscribedFlag, nullptr);
    *state.subscribedFlag = true;
    /* 已订阅时早返回,不会回调 SubscribeCallback,标志位保持 true */
    state.subscribe();
    EXPECT_TRUE(*state.subscribedFlag);
}

HWTEST_P(MotionRecogntionApiTest, UnsubscribeWhenNotSubscribed, TestSize.Level1)
{
    auto state = GetState(GetParam());
    ASSERT_NE(state.subscribedFlag, nullptr);
    *state.subscribedFlag = false;
    /* 未订阅时早返回,标志位保持 false */
    state.unsubscribe();
    EXPECT_FALSE(*state.subscribedFlag);
}

HWTEST_P(MotionRecogntionApiTest, UnsubscribeWhenPreviouslySubscribed, TestSize.Level1)
{
    auto state = GetState(GetParam());
    ASSERT_NE(state.subscribedFlag, nullptr);
    *state.subscribedFlag = true;
    /* 测试环境下 UnsubscribeCallback 返回 false,标志位保持 true */
    state.unsubscribe();
    EXPECT_TRUE(*state.subscribedFlag);
}

HWTEST_P(MotionRecogntionApiTest, SubscribeUnsubscribeRoundTrip, TestSize.Level1)
{
    auto state = GetState(GetParam());
    ASSERT_NE(state.subscribedFlag, nullptr);
    *state.subscribedFlag = false;
    state.subscribe();
    EXPECT_FALSE(*state.subscribedFlag);
    state.unsubscribe();
    EXPECT_FALSE(*state.subscribedFlag);
}

HWTEST_P(MotionRecogntionApiTest, PreSubscribedStateIsPreserved, TestSize.Level1)
{
    auto state = GetState(GetParam());
    ASSERT_NE(state.subscribedFlag, nullptr);
    *state.subscribedFlag = true;
    /* 模拟已订阅状态,后续 Subscribe/Unsubscribe 调用均应为 no-op */
    state.subscribe();
    EXPECT_TRUE(*state.subscribedFlag);
    state.unsubscribe();
    EXPECT_TRUE(*state.subscribedFlag);
}

INSTANTIATE_TEST_SUITE_P(
    AllSubscribers,
    MotionRecogntionApiTest,
    ::testing::Values(
        SubscriberType::PICKUP,
        SubscriberType::FLIP,
        SubscriberType::CLOSE_TO_EAR));

class MotionSubscriberDirectTest : public MotionRecognitionTest,
                                   public testing::WithParamInterface<SubscriberType> {};

HWTEST_P(MotionSubscriberDirectTest, SubscribeEarlyReturnWhenSubscribed, TestSize.Level1)
{
    auto state = GetDirectState(GetParam());
    ASSERT_NE(state.subscribedFlag, nullptr);
    *state.subscribedFlag = true;
    state.subscribe();
    EXPECT_TRUE(*state.subscribedFlag);
}

HWTEST_P(MotionSubscriberDirectTest, SubscribeKeepsFlagWhenCallbackFails, TestSize.Level1)
{
    auto state = GetDirectState(GetParam());
    ASSERT_NE(state.subscribedFlag, nullptr);
    *state.subscribedFlag = false;
    state.subscribe();
    EXPECT_FALSE(*state.subscribedFlag);
}

HWTEST_P(MotionSubscriberDirectTest, UnsubscribeEarlyReturnWhenNotSubscribed, TestSize.Level1)
{
    auto state = GetDirectState(GetParam());
    ASSERT_NE(state.subscribedFlag, nullptr);
    *state.subscribedFlag = false;
    state.unsubscribe();
    EXPECT_FALSE(*state.subscribedFlag);
}

HWTEST_P(MotionSubscriberDirectTest, UnsubscribeKeepsFlagWhenCallbackFails, TestSize.Level1)
{
    auto state = GetDirectState(GetParam());
    ASSERT_NE(state.subscribedFlag, nullptr);
    *state.subscribedFlag = true;
    state.unsubscribe();
    EXPECT_TRUE(*state.subscribedFlag);
}

INSTANTIATE_TEST_SUITE_P(
    AllSubscribers,
    MotionSubscriberDirectTest,
    ::testing::Values(
        SubscriberType::PICKUP,
        SubscriberType::FLIP,
        SubscriberType::CLOSE_TO_EAR));

/**
 * @tc.number   MotionRecognition_ReduceRingToneVolume_0100
 * @tc.name     ReduceRingToneVolume invocation is safe
 * @tc.desc     Function test - 不崩溃即可,真实行为依赖 audio 服务
 */
HWTEST_F(MotionRecognitionTest, MotionRecognition_ReduceRingToneVolume_0100, TestSize.Level1)
{
    /* AudioProxy 在测试构建中可返回 null,函数需在两种情形下都不崩溃 */
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    if (audioProxy != nullptr) {
        int32_t currentVolume = audioProxy->GetVolume(AudioStandard::AudioVolumeType::STREAM_RING);
        EXPECT_GE(currentVolume, 0);
    }
    ASSERT_NO_FATAL_FAILURE(MotionRecogntion::ReduceRingToneVolume());
    SUCCEED();
}

/**
 * @tc.number   MotionRecognition_MultipleSubscribersIsolation_0100
 * @tc.name     Multiple subscribe/unsubscribe cycles preserve independent state
 * @tc.desc     Function test - 各订阅者状态相互独立
 */
HWTEST_F(MotionRecognitionTest, MotionRecognition_MultipleSubscribersIsolation_0100, TestSize.Level1)
{
    /* 三个订阅者反复切换,标志位应保持初始值(Subscribe/Unsubscribe 在测试环境下均失败) */
    for (int32_t i = 0; i < 3; ++i) {
        MotionRecogntion::SubscribePickupSensor();
        MotionRecogntion::UnsubscribePickupSensor();
        MotionRecogntion::SubscribeFlipSensor();
        MotionRecogntion::UnsubscribeFlipSensor();
        MotionRecogntion::SubscribeCloseToEarSensor();
        MotionRecogntion::UnsubscribeCloseToEarSensor();
    }
    EXPECT_FALSE(MotionPickupSubscriber::isMotionPickupSubscribed_);
    EXPECT_FALSE(MotionFlipSubscriber::isMotionFlipSubscribed_);
    EXPECT_FALSE(MotionCloseToEarSubscriber::isMotionCloseToEarSubscribed_);
}

/**
 * @tc.number   MotionRecognition_StaticStateObservable_0100
 * @tc.name     Subscriber flags are shared static state
 * @tc.desc     Function test - 静态状态在多次调用间保持一致
 */
HWTEST_F(MotionRecognitionTest, MotionRecognition_StaticStateObservable_0100, TestSize.Level1)
{
    /* 直接修改静态标志位,通过 public API 验证其可被观测 */
    MotionPickupSubscriber::isMotionPickupSubscribed_ = true;
    MotionRecogntion::SubscribePickupSensor();
    EXPECT_TRUE(MotionPickupSubscriber::isMotionPickupSubscribed_);

    MotionPickupSubscriber::isMotionPickupSubscribed_ = false;
    MotionRecogntion::SubscribePickupSensor();
    EXPECT_FALSE(MotionPickupSubscriber::isMotionPickupSubscribed_);
}
} // namespace Telephony
} // namespace OHOS
