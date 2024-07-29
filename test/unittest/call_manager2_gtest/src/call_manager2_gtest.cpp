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
#include "call_manager2_gtest.h"

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
    if (CallManager2Gtest::clientPtr_->GetCallState() == targetState) {
        return;
    }
    int32_t usedTimeMs = 0;
    std::cout << "wait for a few seconds......" << std::endl;
    while ((CallManager2Gtest::clientPtr_->GetCallState() != targetState) && (usedTimeMs < timeoutMs)) {
        usleep(slipMs * SLEEP_1000_MS);
        usedTimeMs += slipMs;
    }
    int32_t callState = CallManager2Gtest::clientPtr_->GetCallState();
    std::cout << "waited " << usedTimeMs << " seconds" << std::endl;
    std::cout << "target call state:" << targetState << std::endl;
    EXPECT_EQ(callState, targetState);
}

void CallManager2Gtest::HangUpCall()
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

/********************************************* Test RejectCall() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_RejectCall_0100
 * @tc.name     test RejectCall with the callId does not exist
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_RejectCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    std::u16string textMessage = Str8ToStr16("this is a test message");
    EXPECT_NE(CallManager2Gtest::clientPtr_->RejectCall(callId, true, textMessage), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_RejectCall_0200
 * @tc.name     test RejectCall without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_RejectCall_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    std::u16string textMessage = Str8ToStr16("this is a test message");
    EXPECT_NE(CallManager2Gtest::clientPtr_->RejectCall(callId, true, textMessage), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_RejectCall_0300
 * @tc.name     test RejectCall with the callId does not exist
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_RejectCall_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManager2Gtest::blueToothClientPtr_->RejectCall(), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test HangUpCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_HangUpCall_0100
 * @tc.name     test disconnect call with wrong callId
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HangUpCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->HangUpCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HangUpCall_0200  to do ...
 * @tc.name     test ring disconnect call after DialCall,
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HangUpCall_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->HangUpCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HangUpCall_0300  to do ...
 * @tc.name     test ring disconnect call after DialCall without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HangUpCall_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->HangUpCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HangUpCall_0400  to do ...
 * @tc.name     test ring disconnect call after DialCall
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HangUpCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManager2Gtest::blueToothClientPtr_->HangUpCall(), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test GetCallState() *********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallState_0100
 * @tc.name     test GetCallState() without call
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_GetCallState_0100, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_EQ(CallManager2Gtest::clientPtr_->GetCallState(), (int32_t)CallStateToApp::CALL_STATE_IDLE);
}

/**
 * @tc.number   Telephony_CallManager_GetCallState_0200 to do ...
 * @tc.name     test GetCallState() after call
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_GetCallState_0200, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    EXPECT_EQ(CallManager2Gtest::clientPtr_->GetCallState(), (int32_t)CallStateToApp::CALL_STATE_IDLE);
}

/**
 * @tc.number   Telephony_CallManager_GetCallState_0300
 * @tc.name     test GetCallState()
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_GetCallState_0300, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_EQ(CallManager2Gtest::blueToothClientPtr_->GetCallState(), TELEPHONY_SUCCESS);
}

/**************************** Test ReportAudioDeviceInfo() ****************************/
/**
 * @tc.number   Telephony_CallManager_ReportAudioDeviceInfo_0100
 * @tc.name     test report audio device info
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_ReportAudioDeviceInfo_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string phoneNumber = "10086";
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_EQ(CallManager2Gtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_), RETURN_VALUE_IS_ZERO);
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_GE(CallManager2Gtest::clientPtr_->ReportAudioDeviceInfo(), RETURN_VALUE_IS_ZERO);
    sleep(1);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/**
 * @tc.number   Telephony_CallManager_ReportAudioDeviceInfo_0200
 * @tc.name     test report audio device info without active call
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_ReportAudioDeviceInfo_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    EXPECT_GE(CallManager2Gtest::clientPtr_->ReportAudioDeviceInfo(), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ReportAudioDeviceInfo_0300
 * @tc.name     test report audio device info without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_ReportAudioDeviceInfo_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_NE(CallManager2Gtest::clientPtr_->ReportAudioDeviceInfo(), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test HoldCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_HoldCall_0100
 * @tc.name     Hold calls for non-existent call ID
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HoldCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->HoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HoldCall_0200 to do ...
 * @tc.name     coming call test hold call, return non 0
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HoldCall_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->HoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HoldCall_0300 to do ...
 * @tc.name     coming call test hold call without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HoldCall_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->HoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HoldCall_0400
 * @tc.name     Hold calls for non-existent call ID
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HoldCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManager2Gtest::blueToothClientPtr_->HoldCall(), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test UnHoldCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0100
 * @tc.name     Replies calls to a call ID that does not exist, return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_UnHoldCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->UnHoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0200 to do ...
 * @tc.name     Passing in the suspended call ID, test UnHoldCall() return non 0
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_UnHoldCall_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->UnHoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0300
 * @tc.name     Replies calls to a call ID that does not exist without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_UnHoldCall_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->UnHoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0400
 * @tc.name     Replies calls to a call ID that does not exist, return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_UnHoldCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManager2Gtest::blueToothClientPtr_->UnHoldCall(), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SwitchCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SwitchCall_0100
 * @tc.name     Test returns 0 after switching call
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_SwitchCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->SwitchCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SwitchCall_0200
 * @tc.name     Test returns 0 after switching call without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_SwitchCall_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->SwitchCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SwitchCall_0300
 * @tc.name     Test returns 0 after switching call
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_SwitchCall_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManager2Gtest::blueToothClientPtr_->SwitchCall(), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SwitchCall_0400
 * @tc.name     Test returns 0 after switching call, and DialCall(), return true
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_SwitchCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    std::string phoneNumber = "44444444444";
    int32_t callId = INVALID_NEGATIVE_ID;
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManager2Gtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_NE(CallManager2Gtest::clientPtr_->SwitchCall(callId), RETURN_VALUE_IS_ZERO);
    sleep(WAIT_TIME);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/********************************************* Test HasCall() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_HasCall_0100
 * @tc.name     in CALL_STATE_IDLE status, than test Hascall(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HasCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callState = CallManager2Gtest::clientPtr_->GetCallState();
    int32_t idleState = (int32_t)CallStateToApp::CALL_STATE_IDLE;
    ASSERT_EQ(callState, idleState);
    EXPECT_EQ(CallManager2Gtest::clientPtr_->HasCall(), false);
}

/**
 * @tc.number   Telephony_CallManager_HasCall_0200 to do ...
 * @tc.name     The wrong number is not on call, return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HasCall_0200, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    std::string phoneNumber = "";
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManager2Gtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    bool isRet = CallManager2Gtest::clientPtr_->HasCall();
    EXPECT_NE(isRet, true);
}

/**
 * @tc.number   Telephony_CallManager_HasCall_0300 to do ...
 * @tc.name     Free time test Hascall(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HasCall_0300, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    bool isRet = CallManager2Gtest::clientPtr_->HasCall();
    EXPECT_NE(isRet, true);
}

/**
 * @tc.number   Telephony_CallManager_HasCall_0400
 * @tc.name     in CALL_STATE_IDLE status, than test Hascall(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_HasCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_EQ(CallManager2Gtest::clientPtr_->GetCallState(), (int32_t)CallStateToApp::CALL_STATE_IDLE);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_EQ(CallManager2Gtest::blueToothClientPtr_->HasCall(), false);
}

/********************************* Test IsNewCallAllowed() ***************************************/
/**
 * @tc.number   Telephony_CallManager_IsNewCallAllowed_0100
 * @tc.name     The call failed after making a call without the card
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_IsNewCallAllowed_0100, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    std::string phoneNumber = "55555555555";
    InitDialInfo(SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManager2Gtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    ASSERT_EQ(ret, RETURN_VALUE_IS_ZERO);
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    bool enabled = false;
    EXPECT_EQ(CallManager2Gtest::clientPtr_->IsNewCallAllowed(enabled), TELEPHONY_SUCCESS);
    EXPECT_EQ(enabled, false);
    sleep(WAIT_TIME);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/**
 * @tc.number   Telephony_CallManager_IsNewCallAllowed_0200
 * @tc.name     The call failed after making a call without the card
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_IsNewCallAllowed_0200, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    bool enabled = false;
    EXPECT_EQ(CallManager2Gtest::blueToothClientPtr_->IsNewCallAllowed(enabled), TELEPHONY_SUCCESS);
    EXPECT_EQ(enabled, true);
}

/********************************************* Test IsRinging() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_IsRinging_0100
 * @tc.name     There is no call, return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_IsRinging_0100, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callState = CallManager2Gtest::clientPtr_->GetCallState();
    int32_t idleState = (int32_t)CallStateToApp::CALL_STATE_IDLE;
    ASSERT_EQ(callState, idleState);
    bool enabled = false;
    EXPECT_EQ(CallManager2Gtest::clientPtr_->IsRinging(enabled), TELEPHONY_SUCCESS);
    EXPECT_NE(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsRinging_0200
 * @tc.name     There is no call, return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_IsRinging_0200, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callState = CallManager2Gtest::clientPtr_->GetCallState();
    int32_t idleState = (int32_t)CallStateToApp::CALL_STATE_IDLE;
    ASSERT_EQ(callState, idleState);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    bool enabled = false;
    EXPECT_EQ(CallManager2Gtest::blueToothClientPtr_->IsRinging(enabled), TELEPHONY_SUCCESS);
    EXPECT_NE(enabled, true);
}

/***************************************** Test CombineConference() ********************************************/
/**
 * @tc.number   Telephony_CallManager_CombineConference_0100
 * @tc.name     Import callId "@&%￥", test CombineConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_CombineConference_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->CombineConference(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_CombineConference_0200
 * @tc.name     Import callId "100", test CombineConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_CombineConference_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->CombineConference(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_CombineConference_0300
 * @tc.name     Import callId "@&%￥", test CombineConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_CombineConference_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManager2Gtest::blueToothClientPtr_->CombineConference(), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_CombineConference_0400
 * @tc.name     Import callId "@&%￥", test CombineConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_CombineConference_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    std::string phoneNumber = "10086";
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    EXPECT_EQ(CallManager2Gtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_), RETURN_VALUE_IS_ZERO);
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_NE(CallManager2Gtest::clientPtr_->CombineConference(g_newCallId), RETURN_VALUE_IS_ZERO);
    }
    sleep(WAIT_TIME);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/***************************************** Test SeparateConference() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SeparateConference_0100
 * @tc.name     Import callId " -100", test SeparateConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_SeparateConference_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->SeparateConference(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SeparateConference_0200
 * @tc.name     Import callId "100", test SeparateConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_SeparateConference_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->SeparateConference(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SeparateConference_0300
 * @tc.name     Import callId " -100", test SeparateConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_SeparateConference_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManager2Gtest::blueToothClientPtr_->SeparateConference(), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SeparateConference_0400
 * @tc.name     Import callId normal, test SeparateConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_SeparateConference_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    EXPECT_EQ(CallManager2Gtest::clientPtr_->DialCall(Str8ToStr16("10086"), dialInfo_), RETURN_VALUE_IS_ZERO);
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_NE(CallManager2Gtest::clientPtr_->SeparateConference(g_newCallId), RETURN_VALUE_IS_ZERO);
    }
    sleep(WAIT_TIME);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/***************************************** Test KickOutFromConference() ********************************************/
/**
 * @tc.number   Telephony_CallManager_KickOutFromConference_0100
 * @tc.name     Import callId " -100", test KickOutFromConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_KickOutFromConference_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->KickOutFromConference(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_KickOutFromConference_0200
 * @tc.name     Import callId "100", test KickOutFromConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_KickOutFromConference_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    EXPECT_NE(CallManager2Gtest::clientPtr_->KickOutFromConference(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_KickOutFromConference_0300
 * @tc.name     Import callId " -100", test KickOutFromConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManager2Gtest, Telephony_CallManager_KickOutFromConference_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManager2Gtest::blueToothClientPtr_->KickOutFromConference(), RETURN_VALUE_IS_ZERO);
}
} // namespace Telephony
} // namespace OHOS
