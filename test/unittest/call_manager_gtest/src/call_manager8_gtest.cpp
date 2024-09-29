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
 * @tc.number   Telephony_CallManager_SetAudioDevice_0200
 * @tc.name     make EARPIECE device type, set active EARPIECE device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string phoneNumber = "77777777777";
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_EARPIECE,
        .address = { 0 },
    };
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_EQ(clientPtr_->SetAudioDevice(audioDevice), RETURN_VALUE_IS_ZERO);
    sleep(WAIT_TIME);

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0300
 * @tc.name     make SPEAKER device type, set active SPEAKER device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string phoneNumber = "88888888888";
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_SPEAKER,
        .address = { 0 },
    };
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_EQ(clientPtr_->SetAudioDevice(audioDevice), RETURN_VALUE_IS_ZERO);
    sleep(WAIT_TIME);

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0400
 * @tc.name     make DEVICE_WIRED_HEADSET device type, set active DEVICE_WIRED_HEADSET device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    std::string phoneNumber = "99999999999";
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_WIRED_HEADSET,
        .address = { 0 },
    };
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_NE(clientPtr_->SetAudioDevice(audioDevice), RETURN_VALUE_IS_ZERO);
    sleep(WAIT_TIME);

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0500
 * @tc.name     make a empty buletoothAddress, set active bluetooth device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0500, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string phoneNumber = "00000001111";
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO,
        .address = { 0 },
    };
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_NE(clientPtr_->SetAudioDevice(audioDevice), RETURN_VALUE_IS_ZERO);
    sleep(WAIT_TIME);

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0600
 * @tc.name     make SPEAKER device type, set disable device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0600, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string phoneNumber = "00000002222";
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_DISABLE,
        .address = { 0 },
    };
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_NE(clientPtr_->SetAudioDevice(audioDevice), RETURN_VALUE_IS_ZERO);
    sleep(WAIT_TIME);

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0700
 * @tc.name     make SPEAKER device type, set unknown device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0700, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string phoneNumber = "00000003333";
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_UNKNOWN,
        .address = { 0 },
    };
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_NE(clientPtr_->SetAudioDevice(audioDevice), RETURN_VALUE_IS_ZERO);
    sleep(WAIT_TIME);

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}

/********************************************* Test SetMuted()***********************************************/
/**
 * @tc.number   Telephony_CallManager_SetMuted_0100
 * @tc.name     set muted true
 * @tc.desc     Function test
 * @tc.require: issueI5K59I
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetMuted_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string phoneNumber = "00000004444";
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    bool muted = true;
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetMuted(muted), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        sleep(WAIT_TIME);
        HangUpCall();
    }
}

/**
 * @tc.number   Telephony_CallManager_SetMuted_0200
 * @tc.name     without call, set muted failed
 * @tc.desc     Function test
 * @tc.require: issueI5K59I
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetMuted_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    bool muted = true;

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetMuted(muted), CALL_ERR_AUDIO_SETTING_MUTE_FAILED);
}

/**
 * @tc.number   Telephony_CallManager_SetMuted_0300
 * @tc.name     set muted true
 * @tc.desc     Function test
 * @tc.require: issueI5K59I
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetMuted_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    bool muted = false;
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetMuted(muted), RETURN_VALUE_IS_ZERO);
    }
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        sleep(WAIT_TIME);
        HangUpCall();
    }
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

/************************************* Test CloseUnFinishedUssd() ************************************/
/**
 * @tc.number   Telephony_CallManager_CloseUnFinishedUssd_0100
 * @tc.name     test Close Unfinished ussd
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_CloseUnFinishedUssd_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->CloseUnFinishedUssd(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
    }

    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->CloseUnFinishedUssd(SIM2_SLOTID), RETURN_VALUE_IS_ZERO);
    }
}

/************************************* Test InputDialerSpecialCode() ************************************/
/**
 * @tc.number   Telephony_CallManager_InputDialerSpecialCode_0100
 * @tc.name     test Input Dialer Special Code with support code
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_InputDialerSpecialCode_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string specialCode = "2846579";
    EXPECT_EQ(CallManagerGtest::clientPtr_->InputDialerSpecialCode(specialCode), RETURN_VALUE_IS_ZERO);
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
} // namespace Telephony
} // namespace OHOS
