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
constexpr int16_t INVALID_NEGATIVE_ID = -100;
constexpr int16_t INVALID_POSITIVE_ID = 100;
constexpr int INVALID_DIAL_TYPE = 3;
constexpr int WAIT_TIME = 3;
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
BluetoothCallClient &bluetoothCallClient = DelayedRefSingleton<BluetoothCallClient>::GetInstance();

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
 * @tc.number   Telephony_CallManagerClient_001
 * @tc.name     test error nullptr branch
 * @tc.desc     Function test
 */
HWTEST_F(ClientErrorBranchTest, Telephony_CallManagerClient_001, TestSize.Level0)
{
    std::shared_ptr<CallManagerClient> client = std::make_shared<CallManagerClient>();
    std::u16string str = u"";
    client->UnInit();
    ASSERT_EQ(client->RegisterCallBack(nullptr), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->UnRegisterCallBack(), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->ObserverOnCallDetailsChange(), TELEPHONY_ERR_UNINIT);
    AppExecFwk::PacMap extras;
    ASSERT_EQ(client->DialCall(str, extras), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->AnswerCall(g_newCallId, 1), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->RejectCall(g_newCallId, false, str), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->HangUpCall(g_newCallId), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->GetCallState(), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->RegisterVoipCallManagerCallback(), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->UnRegisterVoipCallManagerCallback(), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->HoldCall(g_newCallId), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->UnHoldCall(g_newCallId), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SwitchCall(g_newCallId), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->CombineConference(g_newCallId), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SeparateConference(g_newCallId), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->KickOutFromConference(g_newCallId), TELEPHONY_ERR_UNINIT);
    int callId = 1;
    ASSERT_EQ(client->GetMainCallId(callId, callId), TELEPHONY_ERR_UNINIT);
    std::vector<std::u16string> callIdList;
    ASSERT_EQ(client->GetSubCallIdList(g_newCallId, callIdList), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->GetCallIdListForConference(g_newCallId, callIdList), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->GetCallWaiting(SIM1_SLOTID), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetCallWaiting(SIM1_SLOTID, false), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->GetCallRestriction(SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        TELEPHONY_ERR_UNINIT);
    CallRestrictionInfo callRestrictionInfo;
    ASSERT_EQ(client->SetCallRestriction(SIM1_SLOTID, callRestrictionInfo), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetCallRestrictionPassword(SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING,
        "", ""), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_BUSY), TELEPHONY_ERR_UNINIT);
    CallTransferInfo callTransferInfo;
    ASSERT_EQ(client->SetCallTransferInfo(SIM1_SLOTID, callTransferInfo), TELEPHONY_ERR_UNINIT);
    bool boolValue = false;
    ASSERT_EQ(client->CanSetCallTransferTime(SIM1_SLOTID, boolValue), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetCallPreferenceMode(SIM1_SLOTID, 0), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->StartDtmf(g_newCallId, 'c'), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->StopDtmf(g_newCallId), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->PostDialProceed(SIM1_SLOTID, true), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->IsRinging(boolValue), TELEPHONY_ERR_UNINIT);
    ASSERT_FALSE(client->HasCall());
    ASSERT_FALSE(client->HasCall(false));
    ASSERT_FALSE(client->HasCall(true));
    ASSERT_EQ(client->CancelCallUpgrade(g_newCallId), TELEPHONY_ERR_UNINIT);
}

/**
 * @tc.number   Telephony_CallManagerClient_002
 * @tc.name     test error nullptr branch
 * @tc.desc     Function test
 */
HWTEST_F(ClientErrorBranchTest, Telephony_CallManagerClient_002, TestSize.Level0)
{
    std::shared_ptr<CallManagerClient> client = std::make_shared<CallManagerClient>();
    std::u16string str = u"";
    bool boolValue = false;
    int value = 0;
    ASSERT_EQ(client->IsNewCallAllowed(boolValue), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->IsInEmergencyCall(boolValue), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->IsEmergencyPhoneNumber(str, SIM1_SLOTID, boolValue), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->FormatPhoneNumber(str, str, str), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->FormatPhoneNumberToE164(str, str, str), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetMuted(false), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->MuteRinger(), TELEPHONY_ERR_UNINIT);
    AudioDevice audioDevice;
    ASSERT_EQ(client->SetAudioDevice(audioDevice), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->ControlCamera(g_newCallId, str), TELEPHONY_ERR_UNINIT);
    std::string surfaceId = "";
    ASSERT_EQ(client->SetPreviewWindow(g_newCallId, surfaceId), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetDisplayWindow(g_newCallId, surfaceId), TELEPHONY_ERR_UNINIT);
    float zoomRatio = 1.0;
    ASSERT_EQ(client->SetCameraZoom(zoomRatio), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetPausePicture(g_newCallId, str), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetDeviceDirection(g_newCallId, value), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->GetImsConfig(SIM1_SLOTID, ITEM_VIDEO_QUALITY), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetImsConfig(SIM1_SLOTID, ITEM_VIDEO_QUALITY, str), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->GetImsFeatureValue(SIM1_SLOTID, FeatureType::TYPE_SS_OVER_UT), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetImsFeatureValue(SIM1_SLOTID, FeatureType::TYPE_SS_OVER_UT, value), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->UpdateImsCallMode(SIM1_SLOTID, CALL_MODE_SEND_ONLY), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->EnableImsSwitch(SIM1_SLOTID), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->DisableImsSwitch(SIM1_SLOTID), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->IsImsSwitchEnabled(SIM1_SLOTID, boolValue), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetVoNRState(SIM1_SLOTID, value), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->GetVoNRState(SIM1_SLOTID, value), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->StartRtt(g_newCallId, str), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->StopRtt(g_newCallId), TELEPHONY_ERR_UNINIT);
    std::vector<std::u16string> numberList;
    ASSERT_EQ(client->JoinConference(g_newCallId, numberList), TELEPHONY_ERR_UNINIT);
    std::vector<OttCallDetailsInfo> ottVec;
    ASSERT_EQ(client->ReportOttCallDetailsInfo(ottVec), TELEPHONY_ERR_UNINIT);
    OttCallEventInfo eventInfo;
    ASSERT_EQ(client->ReportOttCallEventInfo(eventInfo), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->CloseUnFinishedUssd(SIM1_SLOTID), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->ObserverOnCallDetailsChange(), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->InputDialerSpecialCode(PHONE_NUMBER), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->RemoveMissedIncomingCallNotification(), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->SetVoIPCallState(value), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->GetVoIPCallState(value), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->ReportAudioDeviceInfo(), TELEPHONY_ERR_UNINIT);
    ASSERT_EQ(client->RequestCameraCapabilities(g_newCallId), TELEPHONY_ERR_UNINIT);
}

/**
 * @tc.number   Telephony_CallManagerClient_003
 * @tc.name     test error nullptr branch
 * @tc.desc     Function test
 */
 HWTEST_F(ClientErrorBranchTest, Telephony_CallManagerClient_003, TestSize.Level1)
 {
    std::shared_ptr<CallManagerClient> client = std::make_shared<CallManagerClient>();
    std::string content = "1";
    ASSERT_EQ(client->SendUssdResponse(SIM1_SLOTID, content), TELEPHONY_ERR_UNINIT);
    std::vector<std::string> dialingList;
    std::vector<std::string> incomingList;
    ASSERT_EQ(client->SetCallPolicyInfo(false, dialingList, false, incomingList), TELEPHONY_ERR_UNINIT);
}

/**
 * @tc.number   Telephony_BluetoothCallClient_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ClientErrorBranchTest, Telephony_BluetoothCallClient_001, TestSize.Level0)
{
    bluetoothCallClient.UnInit();
    ASSERT_NE(bluetoothCallClient.RegisterCallBack(nullptr), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.UnRegisterCallBack(), TELEPHONY_SUCCESS);
    std::u16string value = u"";
    AppExecFwk::PacMap extras;
    bool enabled;
    bluetoothCallClient.IsNewCallAllowed(enabled);
    bluetoothCallClient.IsInEmergencyCall(enabled);
    bluetoothCallClient.SetMuted(false);
    bluetoothCallClient.MuteRinger();
    bluetoothCallClient.SetAudioDevice(AudioDeviceType::DEVICE_BLUETOOTH_SCO, "test");
    bluetoothCallClient.GetCurrentCallList(-1).size();
    ASSERT_NE(bluetoothCallClient.DialCall(value, extras), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.AnswerCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.RejectCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.HangUpCall(), TELEPHONY_SUCCESS);
    ASSERT_GE(bluetoothCallClient.GetCallState(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.HoldCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.UnHoldCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.SwitchCall(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.CombineConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.SeparateConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.KickOutFromConference(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.StartDtmf('a'), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.StopDtmf(), TELEPHONY_SUCCESS);
    ASSERT_NE(bluetoothCallClient.IsRinging(enabled), TELEPHONY_SUCCESS);
}

/********************************************* Test DialCall()***********************************************/
/**
 * @tc.number   Telephony_CallManager_DialCall_0100
 * @tc.name     make a normal phone call with card1, TYPE_VOICE
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0100, Function | MediumTest | Level1)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "11111111111";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0200
 * @tc.name     make a normal phone call with null telephone numbers,
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0200, Function | MediumTest | Level1)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(""), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(""), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0300
 * @tc.name     make a normal phone call with telephone numbers is negative number
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "-12354785268";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0400
 * @tc.name     make a normal phone call with telephone numbers is too long
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "19119080646435437102938190283912471651865810514786470168818468143768714648";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0500
 * @tc.name     If an invalid number is dialed, the DialCall() interface succeeds, but callId is not generated
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0500, TestSize.Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16("19!@#$%^&*:"), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16("19!@#$%^&*:"), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1000 to do ...
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, import phonynumber 10086
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1000, Function | MediumTest | Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16("10086"), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16("10086"), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1100
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, slot id was invalid
 * @tc.desc     Function test
 * @tc.require: I5P2WA
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1100, Function | MediumTest | Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    InitDialInfo(INVALID_SLOT_ID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1200
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, slot id out of count
 * @tc.desc     Function test
 * @tc.require: I5P2WA
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1200, Function | MediumTest | Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    InitDialInfo(slotId, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1300
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, video state was invalid
 * @tc.desc     Function test
 * @tc.require: I5P2WA
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1300, Function | MediumTest | Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, INVALID_VIDEO_STATE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_INVALID_VIDEO_STATE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, INVALID_VIDEO_STATE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_INVALID_VIDEO_STATE);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1400
 * @tc.name     make a normal phone call without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1400, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16("22222222222"), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16("22222222222"), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1500
 * @tc.name     make a normal phone call with error dial type
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1500, Function | MediumTest | Level1)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "33333333333";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, INVALID_DIAL_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_UNKNOW_DIAL_TYPE);
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, INVALID_DIAL_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_UNKNOW_DIAL_TYPE);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1600
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, video state was invalid
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1600, Function | MediumTest | Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, INVALID_VIDEO_STATE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = bluetoothCallClient.DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_INVALID_VIDEO_STATE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, INVALID_VIDEO_STATE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = bluetoothCallClient.DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_INVALID_VIDEO_STATE);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1700
 * @tc.name     make a normal phone call with telephone numbers is too long
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1700, Function | MediumTest | Level2)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber =
        "19119080646435437102938190283912471651865851478647016881846814376871464810514786470168818468143768714648";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
            (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = bluetoothCallClient.DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
            (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = bluetoothCallClient.DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1800
 * @tc.name     make a normal phone call with null telephone numbers,
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1800, Function | MediumTest | Level1)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = bluetoothCallClient.DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, TELEPHONY_ERR_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = bluetoothCallClient.DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, TELEPHONY_ERR_SUCCESS);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1900
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, slot id out of count
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1900, Function | MediumTest | Level0)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    InitDialInfo(slotId, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = bluetoothCallClient.DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_2000
 * @tc.name     make a normal phone call with card1, TYPE_VOICE
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_2000, Function | MediumTest | Level1)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    std::string phoneNumber = "10086";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        EXPECT_EQ(CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_), RETURN_VALUE_IS_ZERO);
        sleep(WAIT_TIME);
        if (CallInfoManager::HasActiveStatus()) {
            EXPECT_EQ(CallManagerGtest::clientPtr_->HoldCall(g_newCallId), RETURN_VALUE_IS_ZERO);
            sleep(WAIT_TIME);
            EXPECT_EQ(CallManagerGtest::clientPtr_->UnHoldCall(g_newCallId), RETURN_VALUE_IS_ZERO);
        }
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        EXPECT_EQ(CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_), RETURN_VALUE_IS_ZERO);
        if (CallInfoManager::HasActiveStatus()) {
            EXPECT_EQ(CallManagerGtest::clientPtr_->HoldCall(g_newCallId), RETURN_VALUE_IS_ZERO);
            sleep(WAIT_TIME);
            EXPECT_EQ(CallManagerGtest::clientPtr_->UnHoldCall(g_newCallId), RETURN_VALUE_IS_ZERO);
        }
        sleep(WAIT_TIME);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_2100
 * @tc.name     make post dial call with pause
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_2100, Function | MediumTest | Level1)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (!CanDialCall(SIM1_SLOTID, SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    std::string phoneNumber = "10086,123";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        EXPECT_EQ(CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_), RETURN_VALUE_IS_ZERO);
        sleep(1);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        EXPECT_EQ(CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_), RETURN_VALUE_IS_ZERO);
        sleep(1);
        if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
            HangUpCall();
        }
    }
}

/********************************************* Test AnswerCall() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_AnswerCall_0100
 * @tc.name     test AnswerCall with the callId does not exist
 * @tc.desc     Function test
 * @tc.require: I5P2WA
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_AnswerCall_0100, Function | MediumTest | Level1)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    int32_t videoState = (int32_t)VideoStateType::TYPE_VOICE;
    EXPECT_NE(CallManagerGtest::clientPtr_->AnswerCall(callId, videoState), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_AnswerCall_0200
 * @tc.name     test AnswerCall with the videoState does not exist
 * @tc.desc     Function test
 * @tc.require: I5P2WA
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_AnswerCall_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    int32_t videoState = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->AnswerCall(callId, videoState), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_AnswerCall_0300
 * @tc.name     test AnswerCall without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_AnswerCall_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    int32_t videoState = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->AnswerCall(callId, videoState), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_AnswerCall_0400
 * @tc.name     test AnswerCall with the callId does not exist
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_AnswerCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    sptr<IRemoteObject> remoteObject = new MockRemoteObject1();
    EXPECT_CALL(*samgr, GetSystemAbility(testing::_)).WillRepeatedly(testing::Return(remoteObject));
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_NE(bluetoothCallClient.AnswerCall(), RETURN_VALUE_IS_ZERO);
}
} // namespace Telephony
} // namespace OHOS
