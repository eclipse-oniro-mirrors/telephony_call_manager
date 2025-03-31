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

#define private public
#define protected public
#include "call_manager_gtest.h"

#include <gtest/gtest.h>
#include <cstring>
#include <string>

#include "bluetooth_hfp_ag.h"
#include "call_manager_connect.h"
#include "call_manager_service.h"
#include "surface_utils.h"
#include "telephony_types.h"
#include "voip_call.h"

using namespace OHOS::Bluetooth;
namespace OHOS {
namespace Telephony {
using namespace testing::ext;
constexpr int16_t SIM2_SLOTID = 1;
constexpr int16_t SIM1_SLOTID_NO_CARD = 0;
constexpr int16_t RETURN_VALUE_IS_ZERO = 0;
constexpr int16_t INVALID_NEGATIVE_ID = -100;
constexpr int16_t INVALID_POSITIVE_ID = 100;
constexpr int INVALID_DIAL_TYPE = 3;
constexpr int WAIT_TIME = 3;
constexpr int16_t CAMERA_ROTATION_90 = 90;
constexpr int16_t CAMERA_ROTATION_ERROR = 50;
constexpr int16_t SLEEP_1000_MS = 1000;
constexpr int BASE_TIME_MS = 1000;
constexpr int SLEEP_TIME_MS = 50;
constexpr int MAX_LIMIT_TIME = 18000;
constexpr int16_t SIM1_SLOTID = 0;
const std::string PHONE_NUMBER = "0000000000";

BluetoothCallClient &bluetoothCallClient = DelayedRefSingleton<BluetoothCallClient>::GetInstance();
std::unordered_map<int32_t, std::unordered_set<int32_t>> g_callStateMap;
int32_t g_newCallId = -1;
std::mutex CallInfoManager::mutex_;
int16_t CallInfoManager::newCallState_;
CallAttributeInfo CallInfoManager::updateCallInfo_;
std::unordered_set<int32_t> CallInfoManager::callIdSet_;

int32_t CallInfoManager::CallDetailsChange(const CallAttributeInfo &info)
{
    TELEPHONY_LOGI("CallDetailsChange Start");
    std::lock_guard<std::mutex> lock(mutex_);
    updateCallInfo_ = info;
    if (callIdSet_.find(updateCallInfo_.callId) == callIdSet_.end()) {
        TELEPHONY_LOGI("CallDetailsChange new call");
        callIdSet_.insert(updateCallInfo_.callId);
        g_newCallId = updateCallInfo_.callId;
        newCallState_ = (int32_t)updateCallInfo_.callState;
        std::unordered_set<int32_t> newSet;
        newSet.clear();
        g_callStateMap.insert(std::pair<int32_t, std::unordered_set<int32_t>>(g_newCallId, newSet));
    }
    g_callStateMap[updateCallInfo_.callId].insert((int32_t)updateCallInfo_.callState);
    return TELEPHONY_SUCCESS;
}

int32_t CallInfoManager::MeeTimeDetailsChange(const CallAttributeInfo &info)
{
    TELEPHONY_LOGI("MeeTimeDetailsChange Start");
    std::lock_guard<std::mutex> lock(mutex_);
    updateCallInfo_ = info;
    if (callIdSet_.find(updateCallInfo_.callId) == callIdSet_.end()) {
        TELEPHONY_LOGI("MeeTimeDetailsChange new call");
        callIdSet_.insert(updateCallInfo_.callId);
        g_newCallId = updateCallInfo_.callId;
        newCallState_ = (int32_t)updateCallInfo_.callState;
        std::unordered_set<int32_t> newSet;
        newSet.clear();
        g_callStateMap.insert(std::pair<int32_t, std::unordered_set<int32_t>>(g_newCallId, newSet));
    }
    g_callStateMap[updateCallInfo_.callId].insert((int32_t)updateCallInfo_.callState);
    return TELEPHONY_SUCCESS;
}

bool CallInfoManager::HasActiveStatus()
{
    TELEPHONY_LOGI("Waiting for activation !");
    int sumUseTime = 0;
    int slipMs = SLEEP_TIME_MS;
    do {
        if (!(HasState(g_newCallId, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE)))) {
            usleep(slipMs * BASE_TIME_MS);
            sumUseTime += slipMs;
        } else {
            TELEPHONY_LOGI("===========wait %d ms callStatus:%d==============", sumUseTime, newCallState_);
            return true;
        }
    } while (sumUseTime < MAX_LIMIT_TIME);
    TELEPHONY_LOGI("===========wait %d ms callStatus:%d=====not active=========", sumUseTime, newCallState_);
    return false;
}

bool CallInfoManager::HasState(int callId, int32_t callState)
{
    if (g_callStateMap.find(callId) == g_callStateMap.end()) {
        return false;
    }
    if (g_callStateMap[callId].find(callState) == g_callStateMap[callId].end()) {
        return false;
    }
    return true;
}

int32_t CallInfoManager::CallEventChange(const CallEventInfo &info)
{
    return TELEPHONY_SUCCESS;
}

void CallInfoManager::Init()
{
    g_newCallId = -1;
    updateCallInfo_.speakerphoneOn = -1;
    updateCallInfo_.startTime = -1;
    updateCallInfo_.isEcc = -1;
    updateCallInfo_.accountId = -1;
    updateCallInfo_.callType = CallType::TYPE_CS;
    updateCallInfo_.callId = -1;
    updateCallInfo_.callState = TelCallState::CALL_STATUS_IDLE;
    updateCallInfo_.videoState = VideoStateType::TYPE_VOICE;
}

void CallInfoManager::LockCallId(bool eq, int32_t targetId, int32_t slipMs, int32_t timeoutMs)
{
    int32_t useTimeMs = 0;
    std::cout << "wait for a few seconds......" << std::endl;
    while ((updateCallInfo_.callId != targetId) && (useTimeMs < timeoutMs)) {
        usleep(slipMs * SLEEP_1000_MS);
        useTimeMs += slipMs;
    }
    std::cout << "\n===========wait " << useTimeMs << " ms target:" << updateCallInfo_.callId << std::endl;
    if (eq) {
        EXPECT_EQ(updateCallInfo_.callId, targetId);
    } else {
        EXPECT_NE(updateCallInfo_.callId, targetId);
    }
}

void CallInfoManager::LockCallState(bool eq, int32_t targetState, int32_t slipMs, int32_t timeoutMs)
{
    if (CallManagerGtest::clientPtr_->GetCallState() == targetState) {
        return;
    }
    int32_t usedTimeMs = 0;
    std::cout << "wait for a few seconds......" << std::endl;
    while ((CallManagerGtest::clientPtr_->GetCallState() != targetState) && (usedTimeMs < timeoutMs)) {
        usleep(slipMs * SLEEP_1000_MS);
        usedTimeMs += slipMs;
    }
    int32_t callState = CallManagerGtest::clientPtr_->GetCallState();
    std::cout << "waited " << usedTimeMs << " seconds" << std::endl;
    std::cout << "target call state:" << targetState << std::endl;
    EXPECT_EQ(callState, targetState);
}

void CallManagerGtest::HangUpCall()
{
    clientPtr_->HangUpCall(INVALID_CALLID);
}

class ClientErrorBranchTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**************************** Test ReportAudioDeviceInfo() ****************************/
/**
 * @tc.number   Telephony_CallManager_ReportAudioDeviceInfo_0200
 * @tc.name     test report audio device info without active call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ReportAudioDeviceInfo_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    EXPECT_GE(CallManagerGtest::clientPtr_->ReportAudioDeviceInfo(), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ReportAudioDeviceInfo_0300
 * @tc.name     test report audio device info without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ReportAudioDeviceInfo_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_NE(CallManagerGtest::clientPtr_->ReportAudioDeviceInfo(), RETURN_VALUE_IS_ZERO);
}
} // namespace Telephony
} // namespace OHOS