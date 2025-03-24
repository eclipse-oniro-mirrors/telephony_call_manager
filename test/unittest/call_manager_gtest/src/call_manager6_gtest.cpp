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
#include "interoperable_settings_handler.h"
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
constexpr const char *SYNERGY_INCOMING_MUTE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=synergy_incoming_mute";
constexpr const char *SYNERGY_MUTE_KEY = "synergy_incoming_mute";

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
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0900
 * @tc.name     input slotId out of count, CallTransferType TRANSFER_TYPE_NOT_REACHABLE,
 *              test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0900, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(slotId, CallTransferType::TRANSFER_TYPE_NOT_REACHABLE),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_1000
 * @tc.name     input slotId 0, CallTransferType invalid, test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_1000, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, (CallTransferType)FALSE_DEFAULT),
            CALL_ERR_INVALID_TRANSFER_TYPE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM2_SLOTID, (CallTransferType)FALSE_DEFAULT),
            CALL_ERR_INVALID_TRANSFER_TYPE);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_1100
 * @tc.name     test GetCallTransferInfo() without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_1100, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_BUSY),
            TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_BUSY),
            TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test SetCallTransferInfo() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0100
 * @tc.name     input slotId 0, CallTransferSettingType CALL_TRANSFER_ENABLE, CallTransferType TRANSFER_TYPE_BUSY,
 *              test SetCallTransferInfo()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "111") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM2_SLOTID, info), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0200
 * @tc.name     input invalid slotId, CallTransferSettingType CALL_TRANSFER_ENABLE, CallTransferType TRANSFER_TYPE_BUSY,
 *              test SetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "222") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(INVALID_SLOT_ID, info), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0300
 * @tc.name     input slotId was out of count, CallTransferSettingType CALL_TRANSFER_ENABLE,
 *              CallTransferType TRANSFER_TYPE_BUSY, test SetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "333") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(slotId, info), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0400
 * @tc.name     input slotId 0, CallTransferSettingType CALL_TRANSFER_ENABLE, CallTransferType invalid,
 *              test SetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = (CallTransferType)FALSE_DEFAULT;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info),
            CALL_ERR_INVALID_TRANSFER_TYPE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM2_SLOTID, info),
            CALL_ERR_INVALID_TRANSFER_TYPE);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0500
 * @tc.name     input slotId 0, CallTransferSettingType invalid, CallTransferType TRANSFER_TYPE_BUSY,
 *              test SetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = (CallTransferSettingType)FALSE_DEFAULT;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info),
            CALL_ERR_INVALID_TRANSFER_SETTING_TYPE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM2_SLOTID, info),
            CALL_ERR_INVALID_TRANSFER_SETTING_TYPE);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0600
 * @tc.name     input slotId 0, CallTransferSettingType invalid, CallTransferType TRANSFER_TYPE_BUSY, transferNum was
 *              too long, test SetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0600, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 2,
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
        "123456789012345678901234567890123456789012345678901234567890") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info), CALL_ERR_NUMBER_OUT_OF_RANGE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM2_SLOTID, info), CALL_ERR_NUMBER_OUT_OF_RANGE);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0700
 * @tc.name     input slotId 0, CallTransferSettingType CALL_TRANSFER_ENABLE, CallTransferType TRANSFER_TYPE_BUSY,
 *              test SetCallTransferInfo()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0700, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "444") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM2_SLOTID, info), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test EnableImsSwitch() ********************************************/
/**
 * @tc.number   Telephony_CallManager_EnableImsSwitch_0100
 * @tc.name     input slotId 0, test EnableImsSwitch
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_EnableImsSwitch_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(SIM2_SLOTID), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_EnableImsSwitch_0200
 * @tc.name     input invalid slotId, test EnableImsSwitch return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_EnableImsSwitch_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(INVALID_SLOT_ID), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_EnableImsSwitch_0300
 * @tc.name     input slotId was out of count, test EnableImsSwitch return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_EnableImsSwitch_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(slotId), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_EnableImsSwitch_0400
 * @tc.name     test EnableImsSwitch without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_EnableImsSwitch_0400, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(SIM1_SLOTID), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(SIM2_SLOTID), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test DisableImsSwitch() ********************************************/
/**
 * @tc.number   Telephony_CallManager_DisableImsSwitch_0100
 * @tc.name     input slotId 0, test DisableImsSwitch
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DisableImsSwitch_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->DisableImsSwitch(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->DisableImsSwitch(SIM2_SLOTID), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DisableImsSwitch_0200
 * @tc.name     input invalid slotId, test DisableImsSwitch return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DisableImsSwitch_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->DisableImsSwitch(INVALID_SLOT_ID), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_DisableImsSwitch_0300
 * @tc.name     input slotId was out of count, test DisableImsSwitch return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DisableImsSwitch_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->DisableImsSwitch(slotId), CALL_ERR_INVALID_SLOT_ID);
}

/******************************************* Test IsImsSwitchEnabled() ********************************************/
/**
 * @tc.number   Telephony_CallManager_IsImsSwitchEnabled_0100
 * @tc.name     input slotId 0, test IsImsSwitchEnabled
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsImsSwitchEnabled_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    bool enabled;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->IsImsSwitchEnabled(SIM1_SLOTID, enabled), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->IsImsSwitchEnabled(SIM2_SLOTID, enabled), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_IsImsSwitchEnabled_0200
 * @tc.name     input invalid slotId, test IsImsSwitchEnabled return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsImsSwitchEnabled_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    bool enabled;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsImsSwitchEnabled(INVALID_SLOT_ID, enabled), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_IsImsSwitchEnabled_0300
 * @tc.name     input slotId was out of count, test IsImsSwitchEnabled return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsImsSwitchEnabled_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    bool enabled;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsImsSwitchEnabled(slotId, enabled), CALL_ERR_INVALID_SLOT_ID);
}

/******************************************* Test SetVoNRState() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetVoNRState_0100
 * @tc.name     input slotId 0, VoNRstate ON(0) test SetVoNRState
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetVoNRState_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t state = 0;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoNRState(SIM1_SLOTID, state), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoNRState(SIM2_SLOTID, state), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetVoNRState_0200
 * @tc.name     input slotId 0, VoNRstate OFF(1) test SetVoNRState
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetVoNRState_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t state = 1;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoNRState(SIM1_SLOTID, state), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoNRState(SIM2_SLOTID, state), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetVoNRState_0300
 * @tc.name     input invalid slotId, test SetVoNRState return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetVoNRState_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t state = 0;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoNRState(INVALID_SLOT_ID, state), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetVoNRState_0400
 * @tc.name     input slotId was out of count, test SetVoNRState return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetVoNRState_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t state = 0;
    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoNRState(slotId, state), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetVoNRState_0500
 * @tc.name     input invalid state, test SetVoNRState return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetVoNRState_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t state = -1;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoNRState(SIM1_SLOTID, state), TELEPHONY_ERR_ARGUMENT_INVALID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoNRState(SIM2_SLOTID, state), TELEPHONY_ERR_ARGUMENT_INVALID);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetVoNRState_0600
 * @tc.name     test SetVoNRState without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetVoNRState_0600, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t state = 0;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoNRState(SIM1_SLOTID, state), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoNRState(SIM2_SLOTID, state), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test GetVoNRState() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetVoNRState_0100
 * @tc.name     input slotId 0, test GetVoNRState
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetVoNRState_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t state;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetVoNRState(SIM1_SLOTID, state), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetVoNRState(SIM2_SLOTID, state), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetVoNRState_0200
 * @tc.name     input invalid slotId, test GetVoNRState return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetVoNRState_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t state;
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetVoNRState(INVALID_SLOT_ID, state), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetVoNRState_0300
 * @tc.name     input slotId was out of count, test GetVoNRState return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetVoNRState_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    int32_t state;
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetVoNRState(slotId, state), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetVoNRState_0400
 * @tc.name     test GetVoNRState without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetVoNRState_0400, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t state;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetVoNRState(SIM1_SLOTID, state), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetVoNRState(SIM2_SLOTID, state), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test ControlCamera() *********************************************/
/**
 * @tc.number   Telephony_CallManager_ControlCamera_0100
 * @tc.name     input camera id lcam001, Test ControlCamera, returns 0 if success
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0100, Function | MediumTest | Level0)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    // 3516DV300 camera id string is "lcam001"
    std::string id = "lcam001";

    std::u16string cameraID = Str8ToStr16(id);
    EXPECT_NE(CallManagerGtest::clientPtr_->ControlCamera(callId, cameraID), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ControlCamera_0200
 * @tc.name     input error camera id xtsautotest123456, Test ControlCamera, return err code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string id = "xtsautotest123456";

    std::u16string cameraID = Str8ToStr16(id);
    EXPECT_NE(CallManagerGtest::clientPtr_->ControlCamera(callId, cameraID), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ControlCamera_0300
 * @tc.name     input error camera id xtsautotest123456, Test ControlCamera,
 *              then input empty camera id, Test ControlCamera
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string id = "lcam001";

    std::u16string cameraID = Str8ToStr16(id);
    EXPECT_NE(CallManagerGtest::clientPtr_->ControlCamera(callId, cameraID), RETURN_VALUE_IS_ZERO);

    id = "";
    cameraID = Str8ToStr16(id);
    EXPECT_NE(CallManagerGtest::clientPtr_->ControlCamera(callId, cameraID), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ControlCamera_0600
 * @tc.name     input empty camera id, Test ControlCamera, returns error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0600, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string id = "";

    std::u16string cameraID = Str8ToStr16(id);
    EXPECT_NE(CallManagerGtest::clientPtr_->ControlCamera(callId, cameraID), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetPreviewWindow() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetPreviewWindow_0100
 * @tc.name     input previewSurfaceId:"", Test set preview window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPreviewWindow_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string previewSurfaceId = "";
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPreviewWindow(callId, previewSurfaceId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPreviewWindow_0200
 * @tc.name     input previewSurfaceId:123, Test set preview window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPreviewWindow_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string previewSurfaceId = "123";
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPreviewWindow(callId, previewSurfaceId), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetDisplayWindow() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetDisplayWindow_0100
 * @tc.name     input displaySurfaceId:"", Test set display window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDisplayWindow_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string displaySurfaceId = "";
    EXPECT_NE(CallManagerGtest::clientPtr_->SetDisplayWindow(callId, displaySurfaceId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDisplayWindow_0200
 * @tc.name     input displaySurfaceId:123, Test set display window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDisplayWindow_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string displaySurfaceId = "123";
    EXPECT_NE(CallManagerGtest::clientPtr_->SetDisplayWindow(callId, displaySurfaceId), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetDeviceDirection() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0100
 * @tc.name     input invalid rotation, Test SetDeviceDirection, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDeviceDirection_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    int32_t rotation = CAMERA_ROTATION_ERROR;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetDeviceDirection(callId, rotation), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0200
 * @tc.name     input rotation 0, Test SetDeviceDirection, return 0 if success
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDeviceDirection_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    int32_t rotation = 0;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetDeviceDirection(callId, rotation), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0300
 * @tc.name     input rotation 90, Test SetDeviceDirection
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDeviceDirection_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    int32_t rotation = CAMERA_ROTATION_90;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetDeviceDirection(callId, rotation), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_InteroperableSettingsHandlerTest_001
 * @tc.name     test interoperable settings handler
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_InteroperableSettingsHandlerTest_001, Function | MediumTest | Level2)
{
    Uri uri(SYNERGY_INCOMING_MUTE_URI);
    auto helper = DelayedSingleton<SettingsDataShareHelper>().GetInstance();
    InteroperableSettingsHandler::RegisterObserver();
    EXPECT_NE(InteroperableSettingsHandler::recvObserver_, nullptr);
    InteroperableSettingsHandler::RegisterObserver();
    EXPECT_NE(InteroperableSettingsHandler::recvObserver_, nullptr);
    auto recvObserver = std::make_shared<InteroperableRecvObserver>();
    recvObserver->OnChange();
    EXPECT_EQ(InteroperableSettingsHandler::QueryMuteRinger(), "0");
    InteroperableSettingsHandler::RecoverMuteRinger("0");
    InteroperableSettingsHandler::RecoverMuteRinger("3");
    InteroperableSettingsHandler::SendMuteRinger();
    EXPECT_EQ(helper->Insert(uri, SYNERGY_MUTE_KEY, "0"), -1);
    EXPECT_EQ(helper->Update(uri, "nokey", "0"), -1);
}
} // namespace Telephony
} // namespace OHOS
