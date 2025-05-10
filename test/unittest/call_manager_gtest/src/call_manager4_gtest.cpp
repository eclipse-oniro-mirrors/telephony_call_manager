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
    if (eq) {
        EXPECT_EQ(callState, targetState);
    } else {
        EXPECT_NE(callState, targetState);
    }
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

/**
 * @tc.number   Telephony_CallManager_KickOutFromConference_0400
 * @tc.name     Import callId normal, test KickOutFromConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_KickOutFromConference_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    std::string number = "10086";
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    EXPECT_GE(CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(number), dialInfo_), RETURN_VALUE_IS_ZERO);
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_NE(CallManagerGtest::clientPtr_->KickOutFromConference(g_newCallId), RETURN_VALUE_IS_ZERO);
    }
    sleep(WAIT_TIME);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/********************************************* Test GetMainCallId() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_GetMainCallId_0100
 * @tc.name     Import callId "abcd", test GetMainCallId(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetMainCallId_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    int32_t mainCallId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->GetMainCallId(callId, mainCallId), TELEPHONY_ERR_SUCCESS);
    EXPECT_NE(mainCallId, RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_GetMainCallId_0200
 * @tc.name     Import callId "100", test GetMainCallId(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetMainCallId_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    int32_t mainCallId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->GetMainCallId(callId, mainCallId), TELEPHONY_ERR_SUCCESS);
    EXPECT_NE(mainCallId, RETURN_VALUE_IS_ZERO);
}

/***************************************** Test GetSubCallIdList() ******************************************/

/**
 * @tc.number   Telephony_CallManager_GetSubCallIdList_0100
 * @tc.name     Import callId "abcd", test GetSubCallIdList(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetSubCallIdList_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    std::vector<std::u16string> list;
    list.clear();
    int32_t result = CallManagerGtest::clientPtr_->GetSubCallIdList(callId, list);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    bool isEmpty = list.empty();
    EXPECT_EQ(isEmpty, true);
    if (!list.empty()) {
        list.clear();
    }
}

/**
 * @tc.number   Telephony_CallManager_GetSubCallIdList_0200
 * @tc.name     Import callId "100", test GetSubCallIdList(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetSubCallIdList_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    std::vector<std::u16string> ans;
    ans.clear();
    int32_t result = CallManagerGtest::clientPtr_->GetSubCallIdList(callId, ans);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    bool isEmpty = ans.empty();
    EXPECT_EQ(isEmpty, true);
    if (!ans.empty()) {
        ans.clear();
    }
}

/************************************ Test GetCallIdListForConference() ***************************************/

/**
 * @tc.number   Telephony_CallManager_GetCallIdListForConference_0100
 * @tc.name     Import callId "abcd", test GetCallIdListForConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallIdListForConference_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    std::vector<std::u16string> callIdList;
    callIdList.clear();
    int32_t result = CallManagerGtest::clientPtr_->GetCallIdListForConference(callId, callIdList);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    bool isEmpty = callIdList.empty();
    EXPECT_EQ(isEmpty, true);
    if (!callIdList.empty()) {
        callIdList.clear();
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallIdListForConference_0200
 * @tc.name     Import callId "100", test GetCallIdListForConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallIdListForConference_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    std::vector<std::u16string> ans;
    ans.clear();
    int32_t result = CallManagerGtest::clientPtr_->GetCallIdListForConference(callId, ans);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    bool isEmpty = ans.empty();
    EXPECT_EQ(isEmpty, true);
    if (!ans.empty()) {
        ans.clear();
    }
}
/************************************* Test IsEmergencyPhoneNumber() ***************************************/
/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0100
 * @tc.name     Call one phonynumber "0-0-0", test IsEmergencyPhoneNumber(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "0-0-0";
    std::u16string phoneNumber = Str8ToStr16(number);
    if (HasSimCard(SIM1_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM1_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_NE(enabled, true);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM2_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_NE(enabled, true);
    }
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0200
 * @tc.name     Call one phonynumber "112", test IsEmergencyPhoneNumber(), return true
 * @tc.desc     Function test
 * @tc.require: I5O1OQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::u16string phoneNumber = Str8ToStr16("112");
    if (HasSimCard(SIM2_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM2_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
    if (HasSimCard(SIM1_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM1_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0300
 * @tc.name     Call one phonynumber "911", test IsEmergencyPhoneNumber(), return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::u16string number = Str8ToStr16("911");
    if (HasSimCard(SIM1_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(number, SIM1_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(number, SIM2_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0400
 * @tc.name     Call one phonynumber "119", test IsEmergencyPhoneNumber(), return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0400, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "119";
    std::u16string phoneNumber = Str8ToStr16(number);
    if (HasSimCard(SIM1_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM1_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM2_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0500
 * @tc.name     Call one phonynumber "999", test IsEmergencyPhoneNumber(), return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0500, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "999";
    std::u16string phoneNumber = Str8ToStr16(number);
    bool enabled = false;
    int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0600
 * @tc.name     Call one phonynumber "", test IsEmergencyPhoneNumber(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0600, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "";
    std::u16string phoneNumber = Str8ToStr16(number);
    bool enabled = false;
    int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    EXPECT_NE(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0700
 * @tc.name     Call one phonynumber "", test IsEmergencyPhoneNumber(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0700, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "@123";
    std::u16string phoneNumber = Str8ToStr16(number);
    bool enabled = false;
    int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    EXPECT_NE(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_1000
 * @tc.name     Call one phonynumber "", test IsEmergencyPhoneNumber(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_1000, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "+1+1+9";
    std::u16string phoneNumber = Str8ToStr16(number);
    bool enabled = false;
    int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    EXPECT_NE(enabled, true);
}

/********************************************* Test GetCallWaiting() ***********************************************/

/**
 * @tc.number   Telephony_CallManager_GetCallWaiting_0100
 * @tc.name     Import slotId 1, test GetCallWaiting(), return 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallWaiting_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallWaiting(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallWaiting(SIM2_SLOTID), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallWaiting_0200
 * @tc.name     test GetCallWaiting without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallWaiting_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallWaiting(SIM1_SLOTID), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallWaiting(SIM2_SLOTID), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/********************************************* Test StartDtmf() ***********************************************/

/**
 * @tc.number   Telephony_CallManager_StartDtmf_0100
 * @tc.name     Import callId abcd, test StartDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StartDtmf_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    char str = '1';
    EXPECT_NE(CallManagerGtest::clientPtr_->StartDtmf(callId, str), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StartDtmf_0200
 * @tc.name     Import callId 100, test StartDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StartDtmf_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    char str = '1';
    EXPECT_NE(CallManagerGtest::clientPtr_->StartDtmf(callId, str), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StartDtmf_0300
 * @tc.name     Import callId 100, test StartDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StartDtmf_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    char str = '1';
    EXPECT_NE(bluetoothCallClient.StartDtmf(str), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StartDtmf_0400
 * @tc.name     Import callId abcd, test StartDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StartDtmf_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    std::string phoneNumber = "10086";
    InitDialInfo(SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_GE(ret, RETURN_VALUE_IS_ZERO);
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_GE(CallManagerGtest::clientPtr_->StartDtmf(g_newCallId, '1'), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        sleep(WAIT_TIME);
        HangUpCall();
    }
}

/********************************************* Test StopDtmf() ***********************************************/

/**
 * @tc.number   Telephony_CallManager_StopDtmf_0100
 * @tc.name     Import callId abcd, test StopDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StopDtmf_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->StopDtmf(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StopDtmf_0200
 * @tc.name     Import callId 100, test StopDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StopDtmf_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->StopDtmf(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StopDtmf_0300
 * @tc.name     Import callId, test StopDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StopDtmf_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_NE(bluetoothCallClient.StopDtmf(), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StopDtmf_0400
 * @tc.name     Import callId abcd, test StopDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StopDtmf_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    std::string phoneNumber = "10086";
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_GE(ret, RETURN_VALUE_IS_ZERO);
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    if (CallInfoManager::HasActiveStatus()) {
        sleep(WAIT_TIME);
        EXPECT_EQ(CallManagerGtest::clientPtr_->StopDtmf(g_newCallId), RETURN_VALUE_IS_ZERO);
    }
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        sleep(WAIT_TIME);
        HangUpCall();
    }
}

/**
 * @tc.number   Telephony_CallManager_PostDialProceed_0100
 * @tc.name     test post dial continue with proceed false
 * @tc.desc     Function test
 */
HWTEST_F(
    CallManagerGtest, Telephony_CallManager_PostDialProceed_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 0;
    bool proceed = false;
    EXPECT_NE(CallManagerGtest::clientPtr_->PostDialProceed(callId, proceed), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_PostDialProceed_0200
 * @tc.name     test post dial continue without permission
 * @tc.desc     Function test
 */
HWTEST_F(
    CallManagerGtest, Telephony_CallManager_PostDialProceed_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 0;
    bool proceed = false;
    EXPECT_NE(CallManagerGtest::clientPtr_->PostDialProceed(callId, proceed), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_PostDialProceed_0300
 * @tc.name     test post dial in normal scene
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_PostDialProceed_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
        InitDialInfo(SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
            (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16("10086;123"), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS,
            SLEEP_30000_MS);
        if (CallInfoManager::HasActiveStatus()) {
            sleep(1);
            EXPECT_EQ(CallManagerGtest::clientPtr_->PostDialProceed(g_newCallId, true), RETURN_VALUE_IS_ZERO);
        }
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            sleep(1);
            HangUpCall();
        }
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
        InitDialInfo(SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
            (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16("10086;456"), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS,
            SLEEP_30000_MS);
        if (CallInfoManager::HasActiveStatus()) {
            sleep(1);
            EXPECT_EQ(CallManagerGtest::clientPtr_->PostDialProceed(g_newCallId, true), RETURN_VALUE_IS_ZERO);
        }
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            sleep(1);
            HangUpCall();
        }
    }
}

/******************************** Test FormatPhoneNumber() * **************************************/

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0100
 * @tc.name     Import phonyNumber 01085198749, test FormatPhoneNumber(), return 010-8519-8748
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "01085198749";
    std::string Code = "Kr";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0200
 * @tc.name     Import countryCode KR, test FormatPhoneNumber(), return 010-8519-8748
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "010-8519-8748";
    std::string Code = "KR";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0300
 * @tc.name     Import phonyNumber (03)38122112, test FormatPhoneNumber(), return 03-3812-2112
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "(03)38122112";
    std::string Code = "JP";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0400
 * @tc.name     Import phonyNumber 13888888888, test FormatPhoneNumber(), return 138 8888 8888
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0400, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "13888888888";
    std::string Code = "CN";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0500
 * @tc.name     Import phonyNumber +81338122121, test FormatPhoneNumber(), return 03-3812-2121
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0500, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "+81338122121";
    std::string Code = "jp";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0600
 * @tc.name     Import phonyNumber 666666999989, test FormatPhoneNumber(), return 666666999989
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0600, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "666666999989";
    std::string Code = "CN";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0700
 * @tc.name     Import countryCode abcdefg, test FormatPhoneNumber(), return 83886082
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0700, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "13888888889";
    std::string Code = "abcdefg";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/******************************* Test FormatPhoneNumberToE164() ***************************************/

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0100
 * @tc.name     Import phonyNumber 01085198748, test FormatPhoneNumberToE164(), return +821085198748
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
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
} // namespace Telephony
} // namespace OHOS
