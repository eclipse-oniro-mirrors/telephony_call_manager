/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef CALL_MANAGER_GTEST
#define CALL_MANAGER_GTEST

#include <unordered_set>
#include <gtest/gtest.h>
#include <iostream>
#include <string_ex.h>

#include <chrono>
#include <thread>

#include "common_event.h"
#include "common_event_manager.h"

#include "call_manager_connect.h"
#include "core_service_client.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t DEFAULT_SLOT_ID = 0;
constexpr int16_t SLEEP_THREE_SECONDS = 3;

class CallManagerGtest : public testing::Test {
public:
    // execute before first testcase
    static void SetUpTestCase()
    {
        std::cout << "---------- warning ------------" << std::endl;
        std::cout << "---Please modify PHONE_NUMBER first in the file call_manager_gtest.cpp---" << std::endl;
        std::cout << "---------- gtest start ------------" << std::endl;
        isConnected_ = false;
        clientPtr_ = std::make_unique<CallManagerConnect>();
        if (clientPtr_ == nullptr) {
            std::cout << "make_unique CallManagerConnect failed!" << std::endl;
            return;
        }
        if (clientPtr_->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID) != TELEPHONY_SUCCESS) {
            std::cout << "connect callManager server failed!" << std::endl;
            return;
        }
        isConnected_ = true;
        std::cout << "connect callManager server success!!!" << std::endl;

        CallInfoManager::Init();
    }

    bool HasSimCard()
    {
        return DelayedRefSingleton<CoreServiceClient>::GetInstance().HasSimCard(DEFAULT_SLOT_ID);
    }

    static bool IsServiceConnected()
    {
        if (!isConnected_) {
            std::cout << "call manager service not connected" << std::endl;
        }
        return isConnected_;
    }

    inline void SleepForSeconds(int32_t seconds)
    {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    }

    void InitDialInfo(int32_t accountId, int32_t videoState, int32_t dialScene, int32_t dialType)
    {
        dialInfo_.PutIntValue("accountId", accountId);
        dialInfo_.PutIntValue("videoState", videoState);
        dialInfo_.PutIntValue("dialScene", dialScene);
        dialInfo_.PutIntValue("dialType", dialType);
    }

    // execute before each testcase
    void SetUp()
    {
        std::this_thread::sleep_for(std::chrono::seconds(SLEEP_THREE_SECONDS));
    }

    // execute after each testcase
    void TearDown() {}

    // execute after last testcase
    static void TearDownTestCase()
    {
        if (clientPtr_ != nullptr) {
            clientPtr_->UnInit();
        }
        std::cout << "---------- gtest end ------------" << std::endl;
    }

    void HangUpCall();
public:
    static bool isConnected_;
    static std::unique_ptr<CallManagerConnect> clientPtr_;
    AppExecFwk::PacMap dialInfo_;

    const int32_t SLEEP_50_MS = 50;
    const int32_t SLEEP_100_MS = 100;
    const int32_t SLEEP_200_MS = 200;
    const int32_t SLEEP_1000_MS = 1000;
    const int32_t SLEEP_12000_MS = 12000;
    const int32_t SLEEP_30000_MS = 30000;
    const std::string EMPTY_DEFAULT = "";
    const int32_t FALSE_DEFAULT = -1;
    const int32_t DIAL_SCENE_TEST = 100;
};

bool CallManagerGtest::isConnected_ = false;
std::unique_ptr<CallManagerConnect> CallManagerGtest::clientPtr_ = nullptr;
} // namespace Telephony
} // namespace OHOS

#endif
