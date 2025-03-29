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

#include "antifraud_cloud_service.h"
#include "bluetooth_hfp_ag.h"
#include "call_manager_connect.h"
#include "call_manager_service.h"
#include "surface_utils.h"
#include "telephony_types.h"
#include "voip_call.h"
#include "anonymize_adapter.h"
#include "antifraud_adapter.h"
#include "antifraud_service.h"
#include "number_identity_service.h"

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

/********************************************* Test RegisterCallBack() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_RegisterCallBack_0100
 * @tc.name     test register callback
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_RegisterCallBack_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::unique_ptr<CallManagerCallback> callManagerCallbackPtr = std::make_unique<CallManagerCallBackStub>();
    if (callManagerCallbackPtr == nullptr) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::clientPtr_->RegisterCallBack(std::move(callManagerCallbackPtr)), RETURN_VALUE_IS_ZERO);
}

/*************************************** Test UnRegisterCallBack() ********************************************/
/**
 * @tc.number   Telephony_CallManager_UnRegisterCallBack_0100
 * @tc.name     test register callback
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UnRegisterCallBack_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->UnRegisterCallBack(), RETURN_VALUE_IS_ZERO);
}

/************************************ Test SetCallPreferenceMode() *****************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallPreferenceMode_0100
 * @tc.name     test set call preference mode cs voice only
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallPreferenceMode_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t mode = CS_VOICE_ONLY;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM1_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM2_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallPreferenceMode_0200
 * @tc.name     test set call preference mode with error mode
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallPreferenceMode_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t mode = 0;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM1_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM2_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallPreferenceMode_0300
 * @tc.name     test set call preference mode with cs voice prefered
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallPreferenceMode_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t mode = CS_VOICE_PREFERRED;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM1_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM2_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallPreferenceMode_0400
 * @tc.name     test set call preference mode with ims ps voice prefered
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallPreferenceMode_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t mode = IMS_PS_VOICE_PREFERRED;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM1_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM2_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallPreferenceMode_0500
 * @tc.name     test set call preference mode with ims ps voice only
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallPreferenceMode_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t mode = IMS_PS_VOICE_ONLY;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM1_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM2_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
}

/*************************************** Test IsInEmergencyCall() **************************************/
/**
 * @tc.number   Telephony_CallManager_IsInEmergencyCall_0100
 * @tc.name     test is in emergency call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsInEmergencyCall_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    bool enabled = false;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsInEmergencyCall(enabled), TELEPHONY_SUCCESS);
    EXPECT_EQ(enabled, false);
}

/**
 * @tc.number   Telephony_CallManager_IsInEmergencyCall_0200
 * @tc.name     test is in emergency call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsInEmergencyCall_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    bool enabled = false;
    EXPECT_EQ(bluetoothCallClient.IsInEmergencyCall(enabled), TELEPHONY_SUCCESS);
    EXPECT_EQ(enabled, false);
}

/**
 * @tc.number   Telephony_CallManager_IsInEmergencyCall_0300
 * @tc.name     test is in emergency call without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsInEmergencyCall_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    bool enabled = false;
    EXPECT_NE(CallManagerGtest::clientPtr_->IsInEmergencyCall(enabled), TELEPHONY_SUCCESS);
    EXPECT_EQ(enabled, false);
}

/****************************************** Test MuteRinger() *******************************************/
/**
 * @tc.number   Telephony_CallManager_MuteRinger_0100
 * @tc.name     test mute ringer
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_MuteRinger_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->MuteRinger(), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test JoinConference() *********************************************/
/**
 * @tc.number   Telephony_CallManager_JoinConference_0100
 * @tc.name     test join conference
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_JoinConference_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::vector<std::u16string> numberList { u"0000000000" };
    EXPECT_NE(CallManagerGtest::clientPtr_->JoinConference(callId, numberList), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_JoinConference_0200
 * @tc.name     test join conference with empty numberList
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_JoinConference_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::vector<std::u16string> numberList;
    EXPECT_NE(CallManagerGtest::clientPtr_->JoinConference(callId, numberList), CALL_ERR_PHONE_NUMBER_EMPTY);
}

/*********************************** Test ReportOttCallDetailsInfo() ***********************************/
/**
 * @tc.number   Telephony_CallManager_ReportOttCallDetailsInfo_0100
 * @tc.name     test report ott call details info
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ReportOttCallDetailsInfo_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    OttCallDetailsInfo info;
    const char *number = "000000";
    memcpy_s(info.phoneNum, kMaxNumberLen, number, strlen(number));
    const char *bundleName = "com.ohos.tddtest";
    memcpy_s(info.bundleName, kMaxNumberLen, bundleName, strlen(bundleName));
    info.callState = TelCallState::CALL_STATUS_DIALING;
    info.videoState = VideoStateType::TYPE_VOICE;
    std::vector<OttCallDetailsInfo> ottVec { info };
    EXPECT_EQ(CallManagerGtest::clientPtr_->ReportOttCallDetailsInfo(ottVec), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ReportOttCallDetailsInfo_0200
 * @tc.name     test report ott call details info
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ReportOttCallDetailsInfo_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::vector<OttCallDetailsInfo> ottVec;
    EXPECT_NE(CallManagerGtest::clientPtr_->ReportOttCallDetailsInfo(ottVec), RETURN_VALUE_IS_ZERO);
}

/************************************* Test ReportOttCallEventInfo() ************************************/
/**
 * @tc.number   Telephony_CallManager_ReportOttCallEventInfo_0100
 * @tc.name     test report ott call details info
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ReportOttCallEventInfo_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    OttCallEventInfo info;
    info.ottCallEventId = OttCallEventId::OTT_CALL_EVENT_FUNCTION_UNSUPPORTED;
    char bundlename[] = "com.ohos.tddtest";
    memcpy_s(info.bundleName, kMaxNumberLen, bundlename, strlen(bundlename));
    EXPECT_EQ(CallManagerGtest::clientPtr_->ReportOttCallEventInfo(info), RETURN_VALUE_IS_ZERO);
}

/*********************************** Test HasVoiceCapability() ***************************************/
/**
 * @tc.number   Telephony_CallManager_HasVoiceCapability_0100
 * @tc.name     test report ott call details info
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HasVoiceCapability_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->HasVoiceCapability(), true);
}

/**
 * @tc.number   Telephony_CallManager_InputDialerSpecialCode_0200
 * @tc.name     test Input Dialer Special Code with unsupport code
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_InputDialerSpecialCode_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string specialCode = "1234";
    EXPECT_NE(CallManagerGtest::clientPtr_->InputDialerSpecialCode(specialCode), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_InputDialerSpecialCode_0300
 * @tc.name     test Input Dialer Special Code without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_InputDialerSpecialCode_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string specialCode = "2846579";
    EXPECT_NE(CallManagerGtest::clientPtr_->InputDialerSpecialCode(specialCode), RETURN_VALUE_IS_ZERO);
}

/**************************** Test RemoveMissedIncomingCallNotification() ****************************/
/**
 * @tc.number   Telephony_CallManager_RemoveMissedIncomingCallNotification_0100
 * @tc.name     test remove missed incoming call and mark as read
 * @tc.desc     Function test
 */
HWTEST_F(
    CallManagerGtest, Telephony_CallManager_RemoveMissedIncomingCallNotification_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::clientPtr_->RemoveMissedIncomingCallNotification(), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_RemoveMissedIncomingCallNotification_0200
 * @tc.name     test cancel missed incoming call and mark as read without permission
 * @tc.desc     Function test
 */
HWTEST_F(
    CallManagerGtest, Telephony_CallManager_RemoveMissedIncomingCallNotification_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_NE(CallManagerGtest::clientPtr_->RemoveMissedIncomingCallNotification(), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetVoIPCallState() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetVoIPCallState_0100
 * @tc.name     input VoIP call state 0 test SetVoIPCallState
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetVoIPCallState_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    int32_t state = 0;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoIPCallState(state), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetVoIPCallState() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetVoIPCallState_0200
 * @tc.name     input VoIP call state 1 test SetVoIPCallState
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetVoIPCallState_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    int32_t state = 1;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoIPCallState(state), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetVoIPCallState() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetVoIPCallState_0300
 * @tc.name     input VoIP call state 2 test SetVoIPCallState
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetVoIPCallState_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    int32_t state = 2;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoIPCallState(state), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetVoIPCallState() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetVoIPCallState_0400
 * @tc.name     input VoIP call state 3 test SetVoIPCallState
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetVoIPCallState_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    int32_t state = 3;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetVoIPCallState(state), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test GetVoIPCallState() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetVoIPCallState_0100
 * @tc.name     test GetVoIPCallState
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetVoIPCallState_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    int32_t state = -1;
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetVoIPCallState(state), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_GetVoIPCallState_0200
 * @tc.name     test GetVoIPCallState no permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetVoIPCallState_0200, Function | MediumTest | Level3)
{
    int32_t state = -1;
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetVoIPCallState(state), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallControlManager_SetVoIPCallInfo_0300
 * @tc.name     test SetVoIPCallInfo
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallControlManager_SetVoIPCallInfo_0300, Function | MediumTest | Level3)
{
    std::shared_ptr<CallControlManager> callControlManager = std::make_shared<CallControlManager>();
    int32_t callId = 10020;
    std::string phoneNumber = "12345678901";
    ASSERT_EQ(callControlManager->SetVoIPCallInfo(callId, static_cast<int32_t>(TelCallState::CALL_STATUS_IDLE),
                                                    phoneNumber), TELEPHONY_SUCCESS);
    ASSERT_EQ(callControlManager->SetVoIPCallInfo(callId, static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING),
                                                    phoneNumber), TELEPHONY_SUCCESS);
    ASSERT_EQ(callControlManager->SetVoIPCallInfo(callId, static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED),
                                                    phoneNumber), TELEPHONY_SUCCESS);
    ASSERT_EQ(callControlManager->SetVoIPCallInfo(callId, static_cast<int32_t>(TelCallState::CALL_STATUS_ALERTING),
                                                    phoneNumber), TELEPHONY_SUCCESS);
    int32_t state = 5;
    ASSERT_EQ(callControlManager->GetVoIPCallInfo(callId, state, phoneNumber), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_SetVoIPCallInfo_0400
 * @tc.name     test SetVoIPCallInfo
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerServiceStub_SetVoIPCallInfo_0400, Function | MediumTest | Level3)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    MessageParcel data;
    MessageParcel reply;
    int32_t callId = 0;
    data.WriteInt32(callId);
    data.RewindRead(0);
    ASSERT_EQ(callManagerService->OnSetVoIPCallInfo(data, reply), TELEPHONY_SUCCESS);
    ASSERT_EQ(callManagerService->OnGetVoIPCallInfo(data, reply), TELEPHONY_SUCCESS);
}

/*********************************** Test Dump() ***************************************/
/**
 * @tc.number   Telephony_CallManager_TestDump_0100
 * @tc.name     Test Dump
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_TestDump_0100, Function | MediumTest | Level3)
{
    std::vector<std::u16string> emptyArgs = {};
    std::vector<std::u16string> args = { u"test", u"test1" };
    EXPECT_GE(DelayedSingleton<CallManagerService>::GetInstance()->Dump(-1, args), 0);
    EXPECT_GE(DelayedSingleton<CallManagerService>::GetInstance()->Dump(0, emptyArgs), 0);
    EXPECT_GE(DelayedSingleton<CallManagerService>::GetInstance()->Dump(0, args), 0);
}

/**
 * @tc.number   Telephony_VoipCallObject_0200
 * @tc.name     Test VoipCallObject
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_VoipCallObject_0200, Function | MediumTest | Level3)
{
    CallObjectManager::ClearVoipList();
    int32_t callId = 10010;
    int32_t secondCallId = 10011;
    CallAttributeInfo callAttrInfo;
    callAttrInfo.callId = callId;
    CallAttributeInfo secondCallAttrInfo;
    secondCallAttrInfo.callId = secondCallId;
    EXPECT_EQ(CallObjectManager::AddOneVoipCallObject(callAttrInfo), TELEPHONY_SUCCESS);
    
    EXPECT_EQ(CallObjectManager::AddOneVoipCallObject(secondCallAttrInfo), TELEPHONY_SUCCESS);
    
    EXPECT_EQ(CallObjectManager::AddOneVoipCallObject(callAttrInfo), CALL_ERR_PHONE_CALL_ALREADY_EXISTS);

    TelCallState nextState = TelCallState::CALL_STATUS_DISCONNECTED;
    EXPECT_EQ(CallObjectManager::UpdateOneVoipCallObjectByCallId(secondCallId, nextState), TELEPHONY_SUCCESS);

    int32_t newCallId = -1;
    EXPECT_EQ(CallObjectManager::UpdateOneVoipCallObjectByCallId(newCallId, nextState), TELEPHONY_ERROR);

    EXPECT_EQ(CallObjectManager::DeleteOneVoipCallObject(newCallId), TELEPHONY_ERROR);

    EXPECT_TRUE(CallObjectManager::IsVoipCallExist());

    CallObjectManager::ClearVoipList();

    EXPECT_FALSE(CallObjectManager::IsVoipCallExist());
}

/**
 * @tc.number   Telephony_AntiFraudCloudService_0001
 * @tc.name     Test antiFraud cloud service
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_AntiFraudCloudService_0001, Function | MediumTest | Level3)
{
    auto service = std::make_shared<AntiFraudCloudService>("10000");
    std::string fileContent = "abcdefghij";
    OHOS::AntiFraudService::AntiFraudResult antiFraudResult = {0, true, 0, 0, fileContent, 1.0f};
    EXPECT_FALSE(service->UploadPostRequest(antiFraudResult));
    EXPECT_TRUE(service->EncryptSync("", nullptr).first.empty());
    EXPECT_TRUE(service->ProcessEncryptResult("{\"key\":\"value\"}").first.empty());
    EXPECT_TRUE(service->ProcessEncryptResult("{\"ak\":0}").first.empty());
    EXPECT_EQ(service->ProcessEncryptResult("{\"ak\":\"ak\"}").first, "ak");
    EXPECT_TRUE(service->ProcessEncryptResult("{\"ak\":\"ak\"}").second.empty());
    EXPECT_TRUE(service->ProcessEncryptResult("{\"ak\":\"ak\",\"data\":0}").second.empty());
    EXPECT_EQ(service->ProcessEncryptResult("{\"ak\":\"ak\",\"data\":\"data\"}").second, "data");
    EXPECT_TRUE(service->GetAuthSync("", "", nullptr).empty());
    EXPECT_TRUE(service->ProcessSignResult("").empty());
    EXPECT_TRUE(service->ProcessSignResult("{\"key\":\"value\"}").empty());
    EXPECT_TRUE(service->ProcessSignResult("{\"data\":0}").empty());
    EXPECT_EQ(service->ProcessSignResult("{\"data\":\"data\"}"), "data");
    auto headers = service->GenerateHeadersMap("", "", "");
    EXPECT_FALSE(headers.empty());
    EXPECT_EQ(service->CalculateDigest("abcdefghij"),
        "72399361da6a7754fec986dca5b7cbaf1c810a28ded4abaf56b2106d06cb78b0");
    EXPECT_FALSE(service->GenerateRequestJson(headers, "").empty());
    EXPECT_FALSE(service->GetDeviceSerial().empty());
    EXPECT_EQ(service->GetSubstringBeforeSymbol("1234", "."), "1234");
}

constexpr int UCS_SERVICE_COMMAND = 0;
constexpr int CLOUD_CONNECT_SERVICE_COMMAND = 1;
class MockRemoteObject : public IRemoteObject {
public:
    explicit MockRemoteObject(int command) : IRemoteObject(u"default")
    {
        command_ = command;
    }

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        if (command_ == -1) {
            return -1;
        }
        reply.WriteInt32(0);
        if (command_ == UCS_SERVICE_COMMAND) {
            std::vector<std::u16string> responseBuffer{u"0"};
            reply.WriteString16Vector(responseBuffer);
        }
        return 0;
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    bool IsObjectDead() const
    {
        return command_ != UCS_SERVICE_COMMAND && command_ != CLOUD_CONNECT_SERVICE_COMMAND;
    }

    int Dump(int fd, const std::vector<std::u16string> &args)
    {
        return 0;
    }
private:
    int command_;
};

/**
 * @tc.number   Telephony_AntiFraudCloudService_0002
 * @tc.name     Test antiFraud cloud service
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_AntiFraudCloudService_0002, Function | MediumTest | Level3)
{
    auto service = std::make_shared<AntiFraudCloudService>("10000");
    sptr<OHOS::IRemoteObject> failRemoteObj = new MockRemoteObject(-1);
    EXPECT_FALSE(service->GetAuthSync("", "", failRemoteObj).empty());
    sptr<OHOS::IRemoteObject> ucsRemoteObj = new MockRemoteObject(UCS_SERVICE_COMMAND);
    EXPECT_FALSE(service->GetAuthSync("", "", ucsRemoteObj).empty());
    sptr<OHOS::IRemoteObject> cloudConnectRemoteObj = new MockRemoteObject(CLOUD_CONNECT_SERVICE_COMMAND);
    OHOS::AntiFraudService::AntiFraudResult antiFraudResult = {0, true, 0, 0, "fileContent", 1.0f};
    EXPECT_FALSE(service->ConnectCloudAsync("", "", antiFraudResult, nullptr));
    EXPECT_FALSE(service->ConnectCloudAsync("", "", antiFraudResult, failRemoteObj));
    EXPECT_TRUE(service->ConnectCloudAsync("", "", antiFraudResult, cloudConnectRemoteObj));
}

/**
 * @tc.number   Telephony_AntiFraudHsdrHelper_0001
 * @tc.name     Test antiFraud hsdr helper
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_AntiFraudHsdrHelper_0001, Function | MediumTest | Level3)
{
    sptr<HsdrConnection> connection = new (std::nothrow) HsdrConnection(
        [](const sptr<IRemoteObject> &remoteObject) {});
    if (connection == nullptr) {
        std::cout << "connection is nullptr" << std::endl;
        return;
    }
    AppExecFwk::ElementName element;
    sptr<OHOS::IRemoteObject> failRemoteObj = new MockRemoteObject(-1);
    connection->OnAbilityConnectDone(element, failRemoteObj, -1);
    EXPECT_EQ(connection->remoteObject_, nullptr);
    connection->OnAbilityConnectDone(element, nullptr, 0);
    EXPECT_EQ(connection->remoteObject_, nullptr);
    EXPECT_FALSE(connection->IsAlive());
    connection->OnAbilityConnectDone(element, failRemoteObj, 0);
    EXPECT_NE(connection->remoteObject_, nullptr);
    EXPECT_FALSE(connection->IsAlive());
    connection->OnAbilityDisconnectDone(element, 0);
    EXPECT_EQ(connection->remoteObject_, nullptr);
    sptr<OHOS::IRemoteObject> ucsRemoteObj = new MockRemoteObject(UCS_SERVICE_COMMAND);
    connection->remoteObject_ = ucsRemoteObj;
    EXPECT_TRUE(connection->IsAlive());
    connection->OnAbilityDisconnectDone(element, -1);
    EXPECT_EQ(connection->remoteObject_, nullptr);
    connection->connectedCallback_ = nullptr;
    connection->OnAbilityConnectDone(element, failRemoteObj, 0);
    EXPECT_EQ(connection->remoteObject_, nullptr);
}

/**
 * @tc.number   Telephony_AntiFraudHsdrHelper_0002
 * @tc.name     Test antiFraud hsdr helper
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_AntiFraudHsdrHelper_0002, Function | MediumTest | Level3)
{
    OnResponse onResponse = [](const HsdrResponse &response) {};
    OnError onError = [](int errCode) {};
    sptr<HsdrCallbackStub> callbackStub =
        new (std::nothrow) HsdrCallbackStub("123456", onResponse, onError);
    if (callbackStub == nullptr) {
        std::cout << "callbackStub is nullptr" << std::endl;
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    EXPECT_NE(callbackStub->OnRemoteRequest(0, data, reply, option), 0);
    data.WriteInterfaceToken(u"");
    EXPECT_NE(callbackStub->OnRemoteRequest(0, data, reply, option), 0);

    data.WriteInterfaceToken(u"OHOS.Security.CloudConnectCallback");
    EXPECT_NE(callbackStub->OnRemoteRequest(0, data, reply, option), 0);

    data.WriteInterfaceToken(u"OHOS.Security.CloudConnectCallback");
    data.WriteString16(u"123456");
    std::vector<std::u16string> responseBuffer{u"0"};
    data.WriteString16Vector(responseBuffer);
    EXPECT_EQ(callbackStub->OnRemoteRequest(0, data, reply, option), 0);

    MessageParcel data2;
    data2.WriteInterfaceToken(u"OHOS.Security.CloudConnectCallback");
    data2.WriteString16(u"1234567");
    data2.WriteString16Vector(responseBuffer);
    EXPECT_EQ(callbackStub->OnRemoteRequest(0, data2, reply, option), 0);
}

/**
 * @tc.number   Telephony_AntiFraud_0100
 * @tc.name     Test AntiFraud
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_AntiFraud_0100, Function | MediumTest | Level3)
{
    auto antiFraudService = DelayedSingleton<AntiFraudService>::GetInstance();
    EXPECT_EQ(antiFraudService->CreateDataShareHelper(-1, USER_SETTINGSDATA_URI.c_str()), nullptr);
    EXPECT_NE(antiFraudService->CreateDataShareHelper(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID,
        USER_SETTINGSDATA_URI.c_str()), nullptr);

    std::string switchName = "noswitch";
    EXPECT_FALSE(antiFraudService->IsSwitchOn(switchName));
    EXPECT_FALSE(antiFraudService->IsSwitchOn(ANTIFRAUD_SWITCH));

    OHOS::AntiFraudService::AntiFraudResult fraudResult;
    std::string phoneNum = "123456";
    antiFraudService->InitAntiFraudService(phoneNum, 0, 1);
    antiFraudService->StopAntiFraudService(0, 1);
    antiFraudService->RecordDetectResult(fraudResult, -1, -1);

    antiFraudService->InitAntiFraudService(phoneNum, 0, 1);
    antiFraudService->StopAntiFraudService(0, 1);
    antiFraudService->RecordDetectResult(fraudResult, -1, 0);

    fraudResult.result = true;
    antiFraudService->InitAntiFraudService(phoneNum, 0, 1);
    antiFraudService->RecordDetectResult(fraudResult, 0, -1);

    antiFraudService->InitAntiFraudService(phoneNum, 0, 1);
    antiFraudService->RecordDetectResult(fraudResult, 0, 0);

    antiFraudService->RecordDetectResult(fraudResult, 0, 0);
    
    auto callStatusManager = std::make_shared<CallStatusManager>();
    antiFraudService->SetCallStatusManager(callStatusManager);
    antiFraudService->InitAntiFraudService(phoneNum, 0, 3);
    antiFraudService->RecordDetectResult(fraudResult, 0, 3);

    auto antiFraudAdapter = DelayedSingleton<AntiFraudAdapter>::GetInstance();
    antiFraudAdapter->ReleaseAntiFraud();
    antiFraudAdapter->GetLibAntiFraud();
    antiFraudAdapter->ReleaseAntiFraud();
}
} // namespace Telephony
} // namespace OHOS
