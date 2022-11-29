/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <string_ex.h>
#include <thread>
#include <unordered_set>

#include "bluetooth_call_client.h"
#include "call_manager_client.h"
#include "call_manager_connect.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "core_service_client.h"

namespace OHOS {
namespace Telephony {
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
        clientPtr_ = DelayedSingleton<CallManagerClient>::GetInstance();
        if (clientPtr_ == nullptr) {
            std::cout << "clientPtr_ is nullptr!" << std::endl;
            return;
        }
        clientPtr_->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
        servicePtr_ = std::make_unique<CallManagerConnect>();
        if (servicePtr_ == nullptr) {
            std::cout << "make_unique CallManagerConnect failed!" << std::endl;
            return;
        }
        if (servicePtr_->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID) != TELEPHONY_SUCCESS) {
            std::cout << "connect callManager server failed!" << std::endl;
            return;
        }
        blueToothClientPtr_ = DelayedSingleton<BluetoothCallClient>::GetInstance();
        if (blueToothClientPtr_ == nullptr) {
            std::cout << "make_unique CallManagerConnect failed!" << std::endl;
            return;
        }
        blueToothClientPtr_->Init();
        isConnected_ = true;
        std::cout << "connect callManager server success!!!" << std::endl;

        CallInfoManager::Init();
    }

    bool HasSimCard(int32_t slotId)
    {
        return DelayedRefSingleton<CoreServiceClient>::GetInstance().HasSimCard(slotId);
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
        if (servicePtr_ != nullptr) {
            servicePtr_->UnInit();
        }
        if (blueToothClientPtr_ != nullptr) {
            blueToothClientPtr_->UnInit();
        }
        std::cout << "---------- gtest end ------------" << std::endl;
    }

    void HangUpCall();

public:
    static bool isConnected_;
    static std::shared_ptr<CallManagerClient> clientPtr_;
    static std::shared_ptr<BluetoothCallClient> blueToothClientPtr_;
    static std::unique_ptr<CallManagerConnect> servicePtr_;
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
    const int32_t INVALID_VIDEO_STATE = -1;
    const int32_t INVALID_SLOT_ID = -1;
};

bool CallManagerGtest::isConnected_ = false;
std::shared_ptr<CallManagerClient> CallManagerGtest::clientPtr_ = nullptr;
std::shared_ptr<BluetoothCallClient> CallManagerGtest::blueToothClientPtr_ = nullptr;
std::unique_ptr<CallManagerConnect> CallManagerGtest::servicePtr_ = nullptr;
} // namespace Telephony
} // namespace OHOS

#endif