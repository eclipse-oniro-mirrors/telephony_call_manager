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
constexpr int16_t RETURN_VALUE_IS_ZERO = 0;
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

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0200
 * @tc.name     Import countryCode KR, test FormatPhoneNumberToE164(), return +81338122111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0200, Function | MediumTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
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
 * @tc.name     Import phonyNumber 13888888888, test FormatPhoneNumberToE164(), return +8613888888888
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0300, Function | MediumTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
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
 * @tc.name     Import phonyNumber +81338122111, test FormatPhoneNumberToE164(), return +81338122111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0400, Function | MediumTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    std::string number = "+81338122111";
    std::string Code = "jp";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0500
 * @tc.name     Import phonyNumber 03-3812-2111, test FormatPhoneNumberToE164(), return +81338122111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0500, Function | MediumTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
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
 * @tc.name     Import phonyNumber 666666999999, test FormatPhoneNumberToE164(), return 83886082
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0600, Function | MediumTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    std::string number = "666666999999";
    std::string Code = "CN";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0700
 * @tc.name     Import countryCode abcdefg, test FormatPhoneNumberToE164(), return 83886082
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0700, TestSize.Level0)
{
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    std::string number = "13888888888";
    std::string Code = "abcdefg";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0800
 * @tc.name     Import countryCode abcdefg, test FormatPhoneNumberToE164(), return CALL_ERR_PHONE_NUMBER_EMPTY
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0800, TestSize.Level0)
{
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    std::string number = "";
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
 * @tc.name     input slotId 0, test SetCallWaiting() enable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0100, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM1_SLOTID, true), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM2_SLOTID, true), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0200
 * @tc.name     input invalid slotId, test SetCallWaiting() enable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0200, TestSize.Level1)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(INVALID_SLOT_ID, true), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0300
 * @tc.name     input slotId was out of count, test SetCallWaiting() enable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0300, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(slotId, true), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0400
 * @tc.name     input slotId 0, test SetCallWaiting() disable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0400, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM1_SLOTID, false), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM2_SLOTID, false), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0500
 * @tc.name     input invalid slotId, test SetCallWaiting() disable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0500, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(INVALID_SLOT_ID, false), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0600
 * @tc.name     input slotId was out of count, test SetCallWaiting() enable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0600, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(slotId, false), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0700
 * @tc.name     test SetCallWaiting without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0700, TestSize.Level0)
{


    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM1_SLOTID, true), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM2_SLOTID, true), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test GetCallRestriction() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0100
 * @tc.name     input slotId 0, CallRestrictionType RESTRICTION_TYPE_ALL_INCOMING, test GetCallRestriction()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0100, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM2_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0200
 * @tc.name     input invalid slotId, CallRestrictionType RESTRICTION_TYPE_ALL_INCOMING, test GetCallRestriction()
 *              return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0200, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                  INVALID_SLOT_ID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0300
 * @tc.name     input invalid was out of count, CallRestrictionType RESTRICTION_TYPE_ALL_INCOMING,
 *              test GetCallRestriction() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0300, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->GetCallRestriction(slotId, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0400
 * @tc.name     input slotId 0, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING, test GetCallRestriction()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0400, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM2_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0500
 * @tc.name     input invalid slotId, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING, test GetCallRestriction()
 *              return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0500, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                  INVALID_SLOT_ID, CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0600
 * @tc.name     input slotId was out of count, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING,
 *              test GetCallRestriction() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0600, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->GetCallRestriction(slotId, CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0700
 * @tc.name     input slotId 0, CallRestrictionType RESTRICTION_TYPE_ALL_INCOMING, test GetCallRestriction()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0700, TestSize.Level0)
{


    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
            TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM2_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
            TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test SetCallRestriction() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallRestriction_0100
 * @tc.name     input slotId 0, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING,
 *              CallRestrictionMode::RESTRICTION_MODE_ACTIVATION,
 *              test SetCallRestriction()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestriction_0100, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));

    CallRestrictionInfo info;
    info.fac = CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING;
    info.mode = CallRestrictionMode::RESTRICTION_MODE_ACTIVATION;
    strcpy_s(info.password, kMaxNumberLen + 1, "555");

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(SIM1_SLOTID, info), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(SIM2_SLOTID, info), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestriction_0200
 * @tc.name     input invalid slotId, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING,
 *              CallRestrictionMode::RESTRICTION_MODE_ACTIVATION,
 *              test SetCallRestriction() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestriction_0200, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));

    CallRestrictionInfo info;
    info.fac = CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING;
    info.mode = CallRestrictionMode::RESTRICTION_MODE_ACTIVATION;
    strcpy_s(info.password, kMaxNumberLen + 1, "666");


    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(INVALID_SLOT_ID, info), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestriction_0300
 * @tc.name     input slotId was out of count, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING,
 *              CallRestrictionMode::RESTRICTION_MODE_ACTIVATION,
 *              test SetCallRestriction() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestriction_0300, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));

    CallRestrictionInfo info;
    info.fac = CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING;
    info.mode = CallRestrictionMode::RESTRICTION_MODE_ACTIVATION;
    strcpy_s(info.password, kMaxNumberLen + 1, "777");

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(slotId, info), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestriction_0400
 * @tc.name     test SetCallRestriction() without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestriction_0400, TestSize.Level0)
{
    CallRestrictionInfo info;
    info.fac = CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING;
    info.mode = CallRestrictionMode::RESTRICTION_MODE_ACTIVATION;
    strcpy_s(info.password, kMaxNumberLen + 1, "888");

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(SIM1_SLOTID, info), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(SIM2_SLOTID, info), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/**************************************** Test SetCallRestrictionPassword() *****************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallRestrictionPassword_0100
 * @tc.name     input slotId 0, CallRestrictionType RESTRICTION_TYPE_ALL_CALLS,
 *              old pasword, new password, test SetCallRestrictionPassword()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestrictionPassword_0100, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));

    const char oldPassword[kMaxNumberLen + 1] = "1111";
    const char newPassword[kMaxNumberLen + 1] = "2222";
    CallRestrictionType fac = CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS;

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestrictionPassword(SIM1_SLOTID, fac, oldPassword, newPassword),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestrictionPassword(SIM2_SLOTID, fac, oldPassword, newPassword),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestrictionPassword_0200
 * @tc.name     input invalid slotId, CallRestrictionType RESTRICTION_TYPE_ALL_CALLS,
 *              old pasword, new password, test SetCallRestrictionPassword() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestrictionPassword_0200, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));

    const char oldPassword[kMaxNumberLen + 1] = "1111";
    const char newPassword[kMaxNumberLen + 1] = "2222";
    CallRestrictionType fac = CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS;

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestrictionPassword(INVALID_SLOT_ID, fac, oldPassword, newPassword),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestrictionPassword_0300
 * @tc.name     input slotId was out of count, CallRestrictionType RESTRICTION_TYPE_ALL_CALLS,
 *              old pasword, new password, test SetCallRestrictionPassword() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestrictionPassword_0300, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));

    const char oldPassword[kMaxNumberLen + 1] = "1111";
    const char newPassword[kMaxNumberLen + 1] = "2222";
    CallRestrictionType fac = CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS;

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestrictionPassword(slotId, fac, oldPassword, newPassword),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestrictionPassword_0400
 * @tc.name     input slotId 0, CallRestrictionType invalid, test SetCallRestrictionPassword() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestrictionPassword_0400, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));

    const char oldPassword[kMaxNumberLen + 1] = "1111";
    const char newPassword[kMaxNumberLen + 1] = "2222";
    CallRestrictionType fac = (CallRestrictionType)FALSE_DEFAULT;

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestrictionPassword(SIM1_SLOTID, fac, oldPassword, newPassword),
            CALL_ERR_INVALID_RESTRICTION_TYPE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestrictionPassword(SIM2_SLOTID, fac, oldPassword, newPassword),
            CALL_ERR_INVALID_RESTRICTION_TYPE);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestrictionPassword_0500
 * @tc.name     input slotId 0, CallRestrictionType RESTRICTION_TYPE_ALL_CALLS,
 *              pasword invalid, test SetCallRestrictionPassword() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestrictionPassword_0500, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));

    const char oldPassword[kMaxNumberLen + 1] = { 0 };
    const char newPassword[kMaxNumberLen + 1] = { 0 };
    CallRestrictionType fac = CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS;

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestrictionPassword(SIM1_SLOTID, fac, oldPassword, newPassword),
            TELEPHONY_ERR_ARGUMENT_INVALID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestrictionPassword(SIM2_SLOTID, fac, oldPassword, newPassword),
            TELEPHONY_ERR_ARGUMENT_INVALID);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestrictionPassword_0600
 * @tc.name     test SetCallRestrictionPassword() without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestrictionPassword_0600, TestSize.Level0)
{
    const char oldPassword[kMaxNumberLen + 1] = "1111";
    const char newPassword[kMaxNumberLen + 1] = "2222";
    CallRestrictionType fac = CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS;

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestrictionPassword(SIM1_SLOTID, fac, oldPassword, newPassword),
            TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestrictionPassword(SIM2_SLOTID, fac, oldPassword, newPassword),
            TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test GetCallTransferInfo() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0100
 * @tc.name     input slotId 0, CallTransferType TRANSFER_TYPE_BUSY, test GetCallTransferInfo()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0100, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_BUSY),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_BUSY),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0200
 * @tc.name     input invalid slotId, CallTransferType TRANSFER_TYPE_BUSY, test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0200, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(INVALID_SLOT_ID, CallTransferType::TRANSFER_TYPE_BUSY),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0300
 * @tc.name     input slotId was out of count, CallTransferType TRANSFER_TYPE_BUSY,
 *              test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0300, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(slotId, CallTransferType::TRANSFER_TYPE_BUSY),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0400
 * @tc.name     input slotId 0, CallTransferType TRANSFER_TYPE_NO_REPLY, test GetCallTransferInfo()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0400, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_NO_REPLY),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM2_SLOTID, CallTransferType::TRANSFER_TYPE_NO_REPLY),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0500
 * @tc.name     input invalid slotId, CallTransferType TRANSFER_TYPE_NO_REPLY, test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0500, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    EXPECT_EQ(
        CallManagerGtest::clientPtr_->GetCallTransferInfo(INVALID_SLOT_ID, CallTransferType::TRANSFER_TYPE_NO_REPLY),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0600
 * @tc.name     input slotId was out of count, CallTransferType TRANSFER_TYPE_NO_REPLY,
 *              test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0600, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(slotId, CallTransferType::TRANSFER_TYPE_NO_REPLY),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0700
 * @tc.name     input slotId 0, CallTransferType TRANSFER_TYPE_NOT_REACHABLE, test GetCallTransferInfo()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0700, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(
                      SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_NOT_REACHABLE),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(
                      SIM2_SLOTID, CallTransferType::TRANSFER_TYPE_NOT_REACHABLE),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0800
 * @tc.name     input invalid slotId, CallTransferType TRANSFER_TYPE_NOT_REACHABLE, test GetCallTransferInfo() return
 *              failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0800, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));


    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(
                  INVALID_SLOT_ID, CallTransferType::TRANSFER_TYPE_NOT_REACHABLE),
        CALL_ERR_INVALID_SLOT_ID);
}
} // namespace Telephony
} // namespace OHOS
