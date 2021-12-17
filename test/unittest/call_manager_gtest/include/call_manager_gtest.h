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
constexpr int32_t SLEEP_SECONDS = 2;
constexpr int32_t EVENT_BLUETOOTH_SCO_CONNECTED_CODE = 0;
constexpr int32_t EVENT_BLUETOOTH_SCO_DISCONNECTED_CODE = 1;
const std::string EVENT_BLUETOOTH_SCO_CONNECTED = "usual.event.BLUETOOTH_SCO_CONNECTED";
const std::string EVENT_BLUETOOTH_SCO_DISCONNECTED = "usual.event.BLUETOOTH_SCO_DISCONNECTED";

class CallManagerGtest : public testing::Test {
public:
    // execute before first testcase
    static void SetUpTestCase()
    {
        std::cout << "---------- gtest start ------------" << std::endl;
        clientPtr_ = std::make_unique<CallManagerConnect>();
        if (clientPtr_ == nullptr) {
            std::cout << "make_unique CallManagerConnect failed!" << std::endl;
            return;
        }
        if (clientPtr_->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID) != TELEPHONY_SUCCESS) {
            std::cout << "connect callManager server failed!" << std::endl;
            return;
        }
        std::cout << "connect callManager server success!!!" << std::endl;

        if (CoreServiceClient::GetInstance().GetProxy() == nullptr) {
            std::cout << "connect coreService server failed!" << std::endl;
            return;
        }
        std::cout << "connect coreService server success!!!" << std::endl;
        CallInfoManager::Init();
    }

    bool HasSimCard()
    {
        if (CoreServiceClient::GetInstance().GetProxy() == nullptr) {
            return false;
        }
        return CoreServiceClient::GetInstance().HasSimCard(DEFAULT_SLOT_ID);
    }

    inline void SleepForSeconds()
    {
        std::this_thread::sleep_for(std::chrono::seconds(SLEEP_SECONDS));
    }

    bool SendConnectBtScoBroadcast()
    {
        AAFwk::Want want;
        want.SetAction(EVENT_BLUETOOTH_SCO_CONNECTED);
        EventFwk::CommonEventData data;
        data.SetWant(want);
        data.SetCode(EVENT_BLUETOOTH_SCO_CONNECTED_CODE);
        OHOS::EventFwk::CommonEventPublishInfo publishInfo;
        publishInfo.SetOrdered(true);
        return EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    }

    bool SendDisconnectBtScoBroadcast()
    {
        AAFwk::Want want;
        want.SetAction(EVENT_BLUETOOTH_SCO_DISCONNECTED);
        EventFwk::CommonEventData data;
        data.SetWant(want);
        data.SetCode(EVENT_BLUETOOTH_SCO_DISCONNECTED_CODE);
        OHOS::EventFwk::CommonEventPublishInfo publishInfo;
        publishInfo.SetOrdered(true);
        return EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    }

    void InitDialInfo(int32_t accountId, int32_t videoState, int32_t dialScene, int32_t dialType)
    {
        dialInfo_.PutIntValue("accountId", accountId);
        dialInfo_.PutIntValue("videoState", videoState);
        dialInfo_.PutIntValue("dialScene", dialScene);
        dialInfo_.PutIntValue("dialType", dialType);
    }

    // execute before each testcase
    void SetUp() {}

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

    bool HasState(int callId, int callState)
    {
        if (g_callStateMap.find(callId) == g_callStateMap.end()) {
            return false;
        }
        if (g_callStateMap[callId].find(callState) == g_callStateMap[callId].end()) {
            return false;
        }
        return true;
    }

public:
    static std::unique_ptr<CallManagerConnect> clientPtr_;
    AppExecFwk::PacMap dialInfo_;

    const int SLEEP_50_MS = 50;
    const int SLEEP_1000_MS = 1000;
    const int SLEEP_12000_MS = 12000;
    const int SLEEP_30000_MS = 30000;
    const std::string EMPTY_DEFAULT = "";
    const int FALSE_DEFAULT = -1;
    std::unordered_map<int, std::unordered_set<int>> g_callStateMap;
    int newCallId_ = -1;
};

std::unique_ptr<CallManagerConnect> CallManagerGtest::clientPtr_ = nullptr;
} // namespace Telephony
} // namespace OHOS

#endif