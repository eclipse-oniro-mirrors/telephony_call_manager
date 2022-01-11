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

#include "call_manager_gtest.h"

#include <cstring>
#include <gtest/gtest.h>
#include <string>

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
constexpr int16_t SIM1_SLOTID = 0;
constexpr int16_t SIM1_SLOTID_NO_CARD = 0;
constexpr int16_t RETURN_VALUE_IS_ZERO = 0;
constexpr int16_t INVALID_NEGATIVE_ID = -100;
constexpr int16_t INVALID_POSITIVE_ID = 100;
constexpr int16_t CALL_MANAGER_ERROR = -1;
constexpr int16_t CAMERA_ROTATION_90 = 90;
constexpr int16_t CAMERA_ROTATION_ERROR = 50;
constexpr int16_t SLEEP_1000_MS = 1000;
const std::string PHONE_NUMBER = "xxxxx";

int32_t CallInfoManager::CallDetailsChange(const CallAttributeInfo &info)
{
    TELEPHONY_LOGI("CallDetailsChange Start");
    std::lock_guard<std::mutex> lock(mutex_);
    updateCallInfo_ = info;
    if (callIdSet_.find(updateCallInfo_.callId) == callIdSet_.end()) {
        TELEPHONY_LOGI("CallDetailsChange new call");
        callIdSet_.insert(updateCallInfo_.callId);
        newCallId_ = updateCallInfo_.callId;
        newCallState_ = (int32_t)updateCallInfo_.callState;
        std::unordered_set<int32_t> newSet;
        newSet.clear();
        callStateMap_.insert(std::pair<int32_t, std::unordered_set<int32_t>>(newCallId_, newSet));
    }
    callStateMap_[updateCallInfo_.callId].insert((int32_t)updateCallInfo_.callState);
    return TELEPHONY_SUCCESS;
}

int32_t CallInfoManager::CallEventChange(const CallEventInfo &info)
{
    return TELEPHONY_SUCCESS;
}

void CallInfoManager::Init()
{
    newCallId_ = -1;
    updateCallInfo_.speakerphoneOn = -1;
    updateCallInfo_.startTime = -1;
    updateCallInfo_.isEcc = -1;
    updateCallInfo_.accountId = -1;
    updateCallInfo_.callType = CallType::TYPE_CS;
    updateCallInfo_.callId = -1;
    updateCallInfo_.callState = TelCallState::CALL_STATUS_IDLE;
    updateCallInfo_.videoState = VideoStateType::TYPE_VOICE;
}

void CallInfoManager::LockCallId(bool eq, int32_t originVal, int32_t slipMs, int32_t timeoutMs)
{
    int32_t useTimeMs = 0;
    while ((updateCallInfo_.callId != originVal) && (useTimeMs < timeoutMs)) {
        usleep(slipMs * SLEEP_1000_MS);
        std::cout << "  sleep:" << slipMs * SLEEP_1000_MS;
        useTimeMs += slipMs;
    }
    std::cout << "\n===========wait " << useTimeMs << " ms target:" << updateCallInfo_.callId << std::endl;
    if (eq) {
        EXPECT_NE(updateCallInfo_.callId, originVal);
    } else {
        EXPECT_EQ(updateCallInfo_.callId, originVal);
    }
}
void CallInfoManager::LockCallState(bool eq, int32_t originVal, int32_t slipMs, int32_t timeoutMs)
{
    int32_t useTimeMs = 0;
    while ((CallManagerGtest::clientPtr_->GetCallState() != originVal) && (useTimeMs < timeoutMs)) {
        usleep(slipMs * SLEEP_1000_MS);
        std::cout << "  sleep:" << slipMs * SLEEP_1000_MS;
        useTimeMs += slipMs;
    }
    int32_t callState = CallManagerGtest::clientPtr_->GetCallState();
    std::cout << "\n===========wait " << useTimeMs << " ms target:" << callState;
    std::cout << " originVal:" << originVal << std::endl;
    if (eq) {
        EXPECT_NE(callState, originVal);
    } else {
        EXPECT_EQ(callState, originVal);
    }
}

/********************************************* Test DialCall()***********************************************/
/**
 * @tc.number   Telephony_CallManager_DialCall_0100
 * @tc.name     make a normal phone call with card1, TYPE_VOICE
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string phoneNumber = "00000000000";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0200
 * @tc.name     make a normal phone call with null telephone numbers,
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string phoneNumber = "";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0300
 * @tc.name     make a normal phone call with telephone numbers is negative number
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_50_MS, SLEEP_30000_MS);
    std::string phoneNumber = "-12354785268";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0400
 * @tc.name     make a normal phone call with telephone numbers is too long
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0400, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    std::string phoneNumber = "19119080646435437102938190283912471651865810514786470168818468143768714648";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0500
 * @tc.name     If an invalid number is dialed, the DialCall() interface succeeds, but callId is not generated
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0500, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_50_MS, SLEEP_30000_MS);
    std::string phoneNumber = "19!@#$%^&*:";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1000 to do ...
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, import phonynumber 10086
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1000, Function | MediumTest | Level0)
{
    if (!HasSimCard()) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_50_MS, SLEEP_30000_MS);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1100
 * @tc.name     make a normal phone call with card1, TYPE_VOICE import phoneNumber 112
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1100, Function | MediumTest | Level0)
{
    if (!HasSimCard()) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_50_MS, SLEEP_30000_MS);
    std::string phoneNumber = "112";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
}
/********************************************* Test AnswerCall() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_AnswerCall_0100
 * @tc.name     test AnswerCall with the callId does not exist
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_AnswerCall_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    int32_t videoState = (int32_t)VideoStateType::TYPE_VOICE;
    EXPECT_NE(CallManagerGtest::clientPtr_->AnswerCall(callId, videoState), RETURN_VALUE_IS_ZERO)
        << "callId = " << callId;
}

/**
 * @tc.number   Telephony_CallManager_AnswerCall_0200
 * @tc.name     test AnswerCall with the videoState does not exist
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_AnswerCall_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    int32_t videoState = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->AnswerCall(callId, videoState), RETURN_VALUE_IS_ZERO)
        << "callId = " << callId;
}

/********************************************* Test RejectCall() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_RejectCall_0100
 * @tc.name     test RejectCall with the callId does not exist
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_RejectCall_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    std::u16string textMessage = Str8ToStr16("this is a test message");
    EXPECT_NE(CallManagerGtest::clientPtr_->RejectCall(callId, true, textMessage), RETURN_VALUE_IS_ZERO)
        << "callId = " << callId;
}

/******************************************* Test HangUpCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_HangUpCall_0100
 * @tc.name     test disconnect call with wrong callId
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HangUpCall_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->HangUpCall(callId), RETURN_VALUE_IS_ZERO) << "callId = " << callId;
}

/**
 * @tc.number   Telephony_CallManager_HangUpCall_0200  to do ...
 * @tc.name     test ring disconnect call after DialCall,
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HangUpCall_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string phoneNumber = "00000000000";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}

/******************************************* Test GetCallState() *********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallState_0100
 * @tc.name     test GetCallState() without  call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallState_0100, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallState(), (int32_t)CallStateToApp::CALL_STATE_IDLE);
}

/**
 * @tc.number   Telephony_CallManager_GetCallState_0200 to do ...
 * @tc.name     test GetCallState() after call
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallState_0200, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    std::string phoneNumber = "00000000000";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}

/******************************************* Test HoldCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_HoldCall_0100
 * @tc.name     Hold calls for non-existent call ID
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HoldCall_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->HoldCall(callId), RETURN_VALUE_IS_ZERO) << "callId = " << callId;
}

/**
 * @tc.number   Telephony_CallManager_HoldCall_0200 to do ...
 * @tc.name     coming call test hold call,return non 0
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HoldCall_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string phoneNumber = "00000000000";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HoldCall_0600 to do ...
 * @tc.name     coming a active call test hold call, return 0
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HoldCall_0600, Function | MediumTest | Level0)
{
    if (!HasSimCard()) {
        return;
    }
    std::string phoneNumber = PHONE_NUMBER;
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}
/******************************************* Test UnHoldCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0100
 * @tc.name     Replies calls to a call ID that does not exist , return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UnHoldCall_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->UnHoldCall(callId), RETURN_VALUE_IS_ZERO) << "callId = " << callId;
}

/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0200 to do ...
 * @tc.name     Passing in the suspended call ID, test UnHoldCall() return non 0
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UnHoldCall_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string phoneNumber = "00000000000";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0600 to do ...
 * @tc.name     coming a active call , holding the call ,and test unhold call , return 0
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UnHoldCall_0600, Function | MediumTest | Level0)
{
    if (!HasSimCard()) {
        return;
    }
    std::string phoneNumber = PHONE_NUMBER;
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SwitchCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SwitchCall_0100
 * @tc.name     Test returns 0 after switching call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SwitchCall_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->SwitchCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SwitchCall_0200 to do ...
 * @tc.name     Test returns 0 after switching call, and DialCall() ,return true
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SwitchCall_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }

    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_50_MS, SLEEP_30000_MS);
    std::string phoneNumber = "00000000000";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    CallInfoManager::LockCallId(false, FALSE_DEFAULT, SLEEP_50_MS, SLEEP_1000_MS);
    ret = CallManagerGtest::clientPtr_->SwitchCall(newCallId_);
    EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
}

/********************************************* Test HasCall() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_HasCall_0100
 * @tc.name     in CALL_STATE_IDLE status,than test Hascall() , return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HasCall_0100, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callState = CallManagerGtest::clientPtr_->GetCallState();
    int32_t idleState = (int32_t)CallStateToApp::CALL_STATE_IDLE;
    ASSERT_EQ(callState, idleState);
    EXPECT_NE(CallManagerGtest::clientPtr_->HasCall(), true);
}

/**
 * @tc.number   Telephony_CallManager_HasCall_0200 to do ...
 * @tc.name     The wrong number is not on call,return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HasCall_0200, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }

    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_50_MS, SLEEP_30000_MS);
    std::string phoneNumber = "";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);

    bool isRet = CallManagerGtest::clientPtr_->HasCall();
    EXPECT_NE(isRet, true);
}

/**
 * @tc.number   Telephony_CallManager_HasCall_0300 to do ...
 * @tc.name     Free time test Hascall() , return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HasCall_0300, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }

    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_50_MS, SLEEP_30000_MS);
    bool isRet = CallManagerGtest::clientPtr_->HasCall();
    EXPECT_NE(isRet, true);
}

/********************************* Test IsNewCallAllowed() ***************************************/
/**
 * @tc.number   Telephony_CallManager_IsNewCallAllowed_0100
 * @tc.name     The call failed after making a call without the card
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsNewCallAllowed_0100, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    std::string phoneNumber = "00000000000";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    ASSERT_EQ(ret, RETURN_VALUE_IS_ZERO);

    CallInfoManager::LockCallState(false, (int32_t)TelCallState::CALL_STATUS_ACTIVE, SLEEP_50_MS, SLEEP_12000_MS);
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsNewCallAllowed(), true);
}

/********************************************* Test IsRinging() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_IsRinging_0100
 * @tc.name     There is no call,return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsRinging_0100, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callState = CallManagerGtest::clientPtr_->GetCallState();
    int32_t idleState = (int32_t)CallStateToApp::CALL_STATE_IDLE;
    ASSERT_EQ(callState, idleState);
    EXPECT_NE(CallManagerGtest::clientPtr_->IsRinging(), true);
}

/***************************************** Test CombineConference() ********************************************/
/**
 * @tc.number   Telephony_CallManager_CombineConference_0100
 * @tc.name     Import callId "@&%￥",test CombineConference(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_CombineConference_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->CombineConference(callId), RETURN_VALUE_IS_ZERO)
        << "callId = " << callId;
}

/**
 * @tc.number   Telephony_CallManager_CombineConference_0200
 * @tc.name     Import callId "100",test CombineConference(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_CombineConference_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->CombineConference(callId), RETURN_VALUE_IS_ZERO)
        << "callId = " << callId;
}

/********************************************* Test GetMainCallId() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_GetMainCallId_0100
 * @tc.name     Import callId "abcd",test GetMainCallId(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetMainCallId_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->GetMainCallId(callId), RETURN_VALUE_IS_ZERO) << "callId = " << callId;
}

/**
 * @tc.number   Telephony_CallManager_GetMainCallId_0200
 * @tc.name     Import callId "100",test GetMainCallId(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetMainCallId_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->GetMainCallId(callId), RETURN_VALUE_IS_ZERO) << "callId = " << callId;
}

/***************************************** Test GetSubCallIdList() ******************************************/

/**
 * @tc.number   Telephony_CallManager_GetSubCallIdList_0100
 * @tc.name     Import callId "abcd",test GetSubCallIdList(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetSubCallIdList_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    std::vector<std::u16string> ans = CallManagerGtest::clientPtr_->GetSubCallIdList(callId);
    bool isEmpty = ans.empty();
    EXPECT_EQ(isEmpty, true);
    if (!ans.empty()) {
        ans.clear();
    }
}

/**
 * @tc.number   Telephony_CallManager_GetSubCallIdList_0200
 * @tc.name     Import callId "100",test GetSubCallIdList(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetSubCallIdList_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    std::vector<std::u16string> ans = CallManagerGtest::clientPtr_->GetSubCallIdList(callId);
    bool isEmpty = ans.empty();
    EXPECT_EQ(isEmpty, true);
    if (!ans.empty()) {
        ans.clear();
    }
}

/************************************ Test GetCallIdListForConference() ***************************************/

/**
 * @tc.number   Telephony_CallManager_GetCallIdListForConference_0100
 * @tc.name     Import callId "abcd",test GetCallIdListForConference(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallIdListForConference_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    std::vector<std::u16string> ans = CallManagerGtest::clientPtr_->GetCallIdListForConference(callId);
    bool isEmpty = ans.empty();
    EXPECT_EQ(isEmpty, true);
    if (!ans.empty()) {
        ans.clear();
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallIdListForConference_0200
 * @tc.name     Import callId "100",test GetCallIdListForConference(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallIdListForConference_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    std::vector<std::u16string> ans = CallManagerGtest::clientPtr_->GetCallIdListForConference(callId);
    bool isEmpty = ans.empty();
    EXPECT_EQ(isEmpty, true);
    if (!ans.empty()) {
        ans.clear();
    }
}
/************************************* Test IsEmergencyPhoneNumber() ***************************************/
/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0100
 * @tc.name     Call one phonynumber "0-0-0",test IsEmergencyPhoneNumber(),return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t slotId = SIM1_SLOTID;
    std::string number = "0-0-0";
    std::u16string phoneNumber = Str8ToStr16(number);
    int32_t error = CALL_MANAGER_ERROR;
    EXPECT_NE(CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, error), true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0200
 * @tc.name     Call one phonynumber "112",test IsEmergencyPhoneNumber(),return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t slotId = SIM1_SLOTID;
    std::string number = "112";
    std::u16string phoneNumber = Str8ToStr16(number);
    int32_t error = CALL_MANAGER_ERROR;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, error), true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0300
 * @tc.name     Call one phonynumber "911",test IsEmergencyPhoneNumber(),return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t slotId = SIM1_SLOTID;
    std::string number = "911";
    std::u16string phoneNumber = Str8ToStr16(number);
    int32_t error = CALL_MANAGER_ERROR;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, error), true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0400
 * @tc.name     Call one phonynumber "08",test IsEmergencyPhoneNumber(),return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0400, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "08";
    std::u16string phoneNumber = Str8ToStr16(number);
    int32_t error = CALL_MANAGER_ERROR;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, error), true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0500
 * @tc.name     Call one phonynumber "118",test IsEmergencyPhoneNumber(),return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0500, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "118";
    std::u16string phoneNumber = Str8ToStr16(number);
    int32_t error = CALL_MANAGER_ERROR;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, error), true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0600
 * @tc.name     Call one phonynumber "119",test IsEmergencyPhoneNumber(),return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0600, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t slotId = SIM1_SLOTID;
    std::string number = "119";
    std::u16string phoneNumber = Str8ToStr16(number);
    int32_t error = CALL_MANAGER_ERROR;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, error), true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0700
 * @tc.name     Call one phonynumber "999",test IsEmergencyPhoneNumber(),return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0700, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "999";
    std::u16string phoneNumber = Str8ToStr16(number);
    int32_t error = CALL_MANAGER_ERROR;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, error), true);
}

/********************************************* Test GetCallWaiting() ***********************************************/

/**
 * @tc.number   Telephony_CallManager_GetCallWaiting_0100
 * @tc.name     Import slotId 1,test GetCallWaiting(),return 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallWaiting_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t slotId = SIM1_SLOTID;
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallWaiting(slotId), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test StartDtmf() ***********************************************/

/**
 * @tc.number   Telephony_CallManager_StartDtmf_0100
 * @tc.name     Import callId abcd,test StartDtmf(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StartDtmf_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    char str = '1';
    EXPECT_NE(CallManagerGtest::clientPtr_->StartDtmf(callId, str), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StartDtmf_0200
 * @tc.name     Import callId 100, test StartDtmf(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StartDtmf_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    char str = '1';
    EXPECT_NE(CallManagerGtest::clientPtr_->StartDtmf(callId, str), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test StopDtmf() ***********************************************/

/**
 * @tc.number   Telephony_CallManager_StopDtmf_0100
 * @tc.name     Import callId abcd,test StopDtmf(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StopDtmf_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->StopDtmf(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StopDtmf_0200
 * @tc.name     Import callId 100, test StopDtmf(),return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StopDtmf_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->StopDtmf(callId), RETURN_VALUE_IS_ZERO);
}

/******************************** Test FormatPhoneNumber() * **************************************/

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0100
 * @tc.name     Import phonyNumber 01085198748,test FormatPhoneNumber(),return 010-8519-8748
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "01085198748";
    std::string Code = "Kr";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0200
 * @tc.name     Import countryCode KR,test FormatPhoneNumber(),return 010-8519-8748
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "010-8519-8748";
    std::string Code = "KR";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0300
 * @tc.name     Import phonyNumber (03)38122111,test FormatPhoneNumber(),return 03-3812-2111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "(03)38122111";
    std::string Code = "JP";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0400
 * @tc.name     Import phonyNumber 13888888888,test FormatPhoneNumber(),return 138 8888 8888
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0400, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "13888888888";
    std::string Code = "CN";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0500
 * @tc.name     Import phonyNumber +81338122111,test FormatPhoneNumber(),return 03-3812-2111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0500, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "+81338122111";
    std::string Code = "jp";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0600
 * @tc.name     Import phonyNumber 666666999999,test FormatPhoneNumber(),return 666666999999
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0600, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "666666999999";
    std::string Code = "CN";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0700
 * @tc.name     Import countryCode abcdefg,test FormatPhoneNumber(),return 83886082
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0700, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "13888888888";
    std::string Code = "abcdefg";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/******************************* Test FormatPhoneNumberToE164() ***************************************/

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0100
 * @tc.name     Import phonyNumber 01085198748,test FormatPhoneNumberToE164(),return +821085198748
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "01085198748";
    std::string Code = "Kr";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0200
 * @tc.name     Import countryCode KR,test FormatPhoneNumberToE164(),return +81338122111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "(03)38122111";
    std::string Code = "JP";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0300
 * @tc.name     Import phonyNumber 13888888888,test FormatPhoneNumberToE164(),return +8613888888888
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "13888888888";
    std::string Code = "cn";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0400
 * @tc.name     Import phonyNumber +81338122111,test FormatPhoneNumberToE164(),return +81338122111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0400, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "+81338122111";
    std::string Code = "jp";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0500
 * @tc.name     Import phonyNumber 03-3812-2111,test FormatPhoneNumberToE164(),return +81338122111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0500, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "03-3812-2111";
    std::string Code = "JP";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0600
 * @tc.name     Import phonyNumber 666666999999,test FormatPhoneNumberToE164(),return 83886082
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0600, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "666666999999";
    std::string Code = "CN";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0700
 * @tc.name     Import countryCode abcdefg,test FormatPhoneNumberToE164(),return 83886082
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0700, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    std::string number = "13888888888";
    std::string Code = "abcdefg";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/********************************************* Test SetCallWaiting() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0100
 * @tc.name     test SetCallWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM1_SLOTID, true), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test SetCallWaiting() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0200
 * @tc.name     test SetCallWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM1_SLOTID, false), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test GetCallRestriction() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0100
 * @tc.name     test GetCallRestriction
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
        SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        RETURN_VALUE_IS_ZERO);
}

/******************************************* Test GetCallRestriction() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0200
 * @tc.name     test GetCallRestriction
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
        SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING),
        RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetCallRestriction() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallRestriction_0100
 * @tc.name     test SetCallRestriction
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestriction_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    CallRestrictionInfo info;
    info.fac = CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING;
    info.mode = CallRestrictionMode::RESTRICTION_MODE_ACTIVATION;
    strcpy(info.password, "123");

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(SIM1_SLOTID, info), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test GetCallTransferInfo() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0100
 * @tc.name     test GetCallTransferInfo
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_BUSY),
        RETURN_VALUE_IS_ZERO);
}

/******************************************* Test GetCallTransferInfo() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0200
 * @tc.name     test GetCallTransferInfo
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(
        CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_NO_REPLY),
        RETURN_VALUE_IS_ZERO);
}

/******************************************* Test GetCallTransferInfo() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0300
 * @tc.name     test GetCallTransferInfo
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(
        SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_NOT_REACHABLE),
        RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetCallTransferInfo() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0100
 * @tc.name     test SetCallTransferInfo
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    strcpy(info.transferNum, "123");

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test EnableVoLte() ********************************************/
/**
 * @tc.number   Telephony_CallManager_EnableVoLte_0100
 * @tc.name     test EnableVoLte
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_EnableVoLte_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->EnableVoLte(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test DisableVoLte() ********************************************/
/**
 * @tc.number   Telephony_CallManager_DisableVoLte_0100
 * @tc.name     test DisableVoLte
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DisableVoLte_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->DisableVoLte(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test IsVoLteEnabled() ********************************************/
/**
 * @tc.number   Telephony_CallManager_IsVoLteEnabled_0100
 * @tc.name     test IsVoLteEnabled
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsVoLteEnabled_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->IsVoLteEnabled(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetLteEnhanceMode() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetLteEnhanceMode_0100
 * @tc.name     test SetLteEnhanceMode
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetLteEnhanceMode_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetLteEnhanceMode(SIM1_SLOTID, true), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetLteEnhanceMode() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetLteEnhanceMode_0200
 * @tc.name     test SetLteEnhanceMode
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetLteEnhanceMode_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetLteEnhanceMode(SIM1_SLOTID, false), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test GetLteEnhanceMode() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetLteEnhanceMode_0100
 * @tc.name     test GetLteEnhanceMode
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetLteEnhanceMode_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->GetLteEnhanceMode(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test ControlCamera() *********************************************/
/**
 * @tc.number   Telephony_CallManager_ControlCamera_0100
 * @tc.name     Test returns 0 after ControlCamera
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0100, Function | MediumTest | Level0)
{
    if (!HasSimCard()) {
        return;
    }
    // 3516DV300 camera id string is "lcam001"
    std::string id = "lcam001";
    std::string pc = "com.example.myapplication";

    std::u16string cameraID = Str8ToStr16(id);
    std::u16string callingPackage = Str8ToStr16(pc);
    EXPECT_EQ(CallManagerGtest::clientPtr_->ControlCamera(cameraID, callingPackage), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ControlCamera_0200
 * @tc.name     Test returns error after ControlCamera
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string id = "xtsautotest123456";
    std::string pc = "com.example.myapplication";

    std::u16string cameraID = Str8ToStr16(id);
    std::u16string callingPackage = Str8ToStr16(pc);
    EXPECT_NE(CallManagerGtest::clientPtr_->ControlCamera(cameraID, callingPackage), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ControlCamera_0300
 * @tc.name     Test returns error after ControlCamera
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string id = "lcam001";
    std::string pc = "com.ohos.errorpkg";

    std::u16string cameraID = Str8ToStr16(id);
    std::u16string callingPackage = Str8ToStr16(pc);
    EXPECT_EQ(CallManagerGtest::clientPtr_->ControlCamera(cameraID, callingPackage), RETURN_VALUE_IS_ZERO);

    id = "";
    cameraID = Str8ToStr16(id);
    EXPECT_EQ(CallManagerGtest::clientPtr_->ControlCamera(cameraID, callingPackage), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ControlCamera_0600
 * @tc.name     Test returns error after ControlCamera
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0600, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string id = "";
    std::string pc = "com.example.myapplication";

    std::u16string cameraID = Str8ToStr16(id);
    std::u16string callingPackage = Str8ToStr16(pc);
    EXPECT_NE(CallManagerGtest::clientPtr_->ControlCamera(cameraID, callingPackage), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ControlCamera_0700
 * @tc.name     Test returns error after ControlCamera
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0700, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    std::string id = "lcam001";
    std::string pc = "com.example.myapplication";

    std::u16string cameraID = Str8ToStr16(id);
    std::u16string callingPackage = Str8ToStr16(pc);
    EXPECT_EQ(CallManagerGtest::clientPtr_->ControlCamera(cameraID, callingPackage), RETURN_VALUE_IS_ZERO);

    id = "";
    cameraID = Str8ToStr16(id);
    EXPECT_EQ(CallManagerGtest::clientPtr_->ControlCamera(cameraID, callingPackage), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetPreviewWindow() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetPreviewWindow_0100
 * @tc.name     Test returns error after set preview window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPreviewWindow_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    VideoWindow window;
    window.x = 0;
    window.y = 0;
    window.z = -1;
    window.width = 200;
    window.height = 200;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPreviewWindow(window), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPreviewWindow_0200
 * @tc.name     Test returns 0 after set preview window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPreviewWindow_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    VideoWindow window;
    window.x = 0;
    window.y = 0;
    window.z = 0;
    window.width = 200;
    window.height = 200;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetPreviewWindow(window), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetDisplayWindow() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetDisplayWindow_0100
 * @tc.name     Test returns error after set display window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDisplayWindow_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    VideoWindow window;
    window.x = 10;
    window.y = 10;
    window.z = 10;
    window.width = 100;
    window.height = 200;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetDisplayWindow(window), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDisplayWindow_0200
 * @tc.name     Test returns 0 after set display window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDisplayWindow_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    VideoWindow window;
    window.x = 10;
    window.y = 10;
    window.z = 0;
    window.width = 200;
    window.height = 200;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetDisplayWindow(window), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetDeviceDirection() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0100
 * @tc.name     Test returns error after set device direction
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDeviceDirection_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t rotation = CAMERA_ROTATION_ERROR;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetDeviceDirection(rotation), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0200
 * @tc.name     Test returns error after set device direction
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDeviceDirection_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t rotation = 0;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetDeviceDirection(rotation), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0300
 * @tc.name     Test returns 0 after set device direction
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDeviceDirection_0300, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    int32_t rotation = CAMERA_ROTATION_90;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetDeviceDirection(rotation), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetCameraZoom() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0100
 * @tc.name     Test returns error after set camera zoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard()) {
        return;
    }
    float zoomRatio = 5.0;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0200
 * @tc.name     Test returns 0 after set camera zoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0200, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    float zoomRatio = 0.1;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0300
 * @tc.name     Test returns 0 after set camera zoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0300, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    float zoomRatio = 10.0;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0400
 * @tc.name     Test returns 0 after set camera zoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0400, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    float zoomRatio = 15.0;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0500
 * @tc.name     Test returns 0 after set camera zoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0500, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    float zoomRatio = 0.0;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0600
 * @tc.name     Test returns 0 after set camera zoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0600, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    float zoomRatio = -1.0;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0700
 * @tc.name     Test returns 0 after set camera zoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0700, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    float zoomRatio = 10.1;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}
/******************************************* Test SetPausePicture() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0100
 * @tc.name     Test returns 0 after set pause picture
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0100, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    std::string pathTemp = "/system/bin/test.png";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0200
 * @tc.name     Test returns 0 after set pause picture
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0200, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    std::string pathTemp = "/system/bin/test.jpg";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0300
 * @tc.name     Test returns 0 after set pause picture
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0300, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    std::string pathTemp = "/system/bin/test.bmp";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0400
 * @tc.name     Test returns 0 after set pause picture
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0400, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    std::string pathTemp = "/system/bin/test.webp";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0500
 * @tc.name     Test returns 0 after set pause picture
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0500, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    std::string pathTemp = "http://callManager.jpg";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0600
 * @tc.name     Test returns 0 after set pause picture
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0600, Function | MediumTest | Level1)
{
    if (!HasSimCard()) {
        return;
    }
    std::string pathTemp = "";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test RegisterCallBack() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_RegisterCallBack_0100
 * @tc.name     test register callback
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_RegisterCallBack_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard()) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->RegisterCallBack(), RETURN_VALUE_IS_ZERO);
}
} // namespace Telephony
} // namespace OHOS