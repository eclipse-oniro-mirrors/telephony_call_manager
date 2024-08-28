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
 * @tc.number   Telephony_CallManagerService_001
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerService_001, Function | MediumTest | Level3)
{
    AccessToken token;
    std::u16string test = u"";
    std::string test_ = "";
    bool enabled;
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_NE(callManagerService->DialCall(test, dialInfo_), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->AnswerCall(0, 0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->RejectCall(0, false, test), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->HangUpCall(0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->GetCallState(), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->HoldCall(0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->UnHoldCall(0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SwitchCall(0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->IsNewCallAllowed(enabled), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->IsRinging(enabled), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->IsInEmergencyCall(enabled), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->StartDtmf(0, 'a'), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->StopDtmf(0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->GetCallWaiting(0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SetCallWaiting(0, false), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->GetCallRestriction(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        TELEPHONY_SUCCESS);
    CallRestrictionInfo callRestrictionInfo;
    ASSERT_NE(callManagerService->SetCallRestriction(0, callRestrictionInfo), TELEPHONY_SUCCESS);
    ASSERT_NE(
        callManagerService->SetCallRestrictionPassword(0, CallRestrictionType::RESTRICTION_TYPE_ALL_CALLS, "11", "22"),
        TELEPHONY_SUCCESS);
    ASSERT_NE(
        callManagerService->GetCallTransferInfo(0, CallTransferType::TRANSFER_TYPE_UNCONDITIONAL), TELEPHONY_SUCCESS);
    CallTransferInfo callTransferInfo;
    ASSERT_NE(callManagerService->SetCallTransferInfo(0, callTransferInfo), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->CanSetCallTransferTime(0, enabled), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SetCallPreferenceMode(0, 0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->StartRtt(0, test), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->StopRtt(0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->CombineConference(0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SeparateConference(0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->KickOutFromConference(0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SetMuted(false), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->MuteRinger(), TELEPHONY_SUCCESS);
    AudioDevice audioDevice = {
        .deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO,
        .address = { 0 },
    };
    ASSERT_NE(callManagerService->SetAudioDevice(audioDevice), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->ControlCamera(1, test), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SetPreviewWindow(1, test_, nullptr), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SetDisplayWindow(1, test_, nullptr), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerService_002
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerService_002, Function | MediumTest | Level3)
{
    AccessToken token;
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    float zoomRatio = 1;
    std::u16string test = u"";
    int32_t callId = 1;
    int32_t value = 1;
    bool enabled;
    ASSERT_NE(callManagerService->SetCameraZoom(zoomRatio), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SetPausePicture(callId, test), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SetDeviceDirection(callId, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->CancelCallUpgrade(callId), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->RequestCameraCapabilities(callId), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->IsEmergencyPhoneNumber(test, 0, enabled), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->FormatPhoneNumber(test, test, test), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->FormatPhoneNumberToE164(test, test, test), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->GetMainCallId(value, value), TELEPHONY_SUCCESS);
    std::vector<std::u16string> callIdList;
    ASSERT_NE(callManagerService->GetSubCallIdList(0, callIdList), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->GetCallIdListForConference(0, callIdList), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->GetImsConfig(0, ImsConfigItem::ITEM_VIDEO_QUALITY), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SetImsConfig(0, ImsConfigItem::ITEM_VIDEO_QUALITY, test), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->GetImsFeatureValue(0, FeatureType::TYPE_VOICE_OVER_LTE), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SetImsFeatureValue(0, FeatureType::TYPE_VOICE_OVER_LTE, 0), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->UpdateImsCallMode(0, ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->EnableImsSwitch(false), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->DisableImsSwitch(false), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->IsImsSwitchEnabled(0, enabled), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->SetVoNRState(0, value), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->GetVoNRState(0, value), TELEPHONY_SUCCESS);
    std::vector<std::u16string> numberList;
    ASSERT_NE(callManagerService->JoinConference(false, numberList), TELEPHONY_SUCCESS);
    std::vector<OttCallDetailsInfo> ottVec;
    ASSERT_NE(callManagerService->ReportOttCallDetailsInfo(ottVec), TELEPHONY_SUCCESS);
    ASSERT_NE(callManagerService->GetMainCallId(value, value), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerService_003
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerService_003, Function | MediumTest | Level3)
{
    AccessToken token;
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    callManagerService->Init();
    callManagerService->RegisterCallBack(nullptr);
    callManagerService->UnRegisterCallBack();
    callManagerService->ObserverOnCallDetailsChange();
    callManagerService->RegisterVoipCallManagerCallback();
    callManagerService->UnRegisterVoipCallManagerCallback();
    int32_t callId = 0;
    bool proceed = false;
    callManagerService->PostDialProceed(callId, proceed);
    OttCallDetailsInfo info;
    const char *number = "000000";
    memcpy_s(info.phoneNum, kMaxNumberLen, number, strlen(number));
    const char *bundleName = "com.ohos.tddtest";
    memcpy_s(info.bundleName, kMaxNumberLen, bundleName, strlen(bundleName));
    info.callState = TelCallState::CALL_STATUS_DIALING;
    info.videoState = VideoStateType::TYPE_VOICE;
    std::vector<OttCallDetailsInfo> ottVec { info };
    callManagerService->ReportOttCallDetailsInfo(ottVec);
    OttCallEventInfo eventInfo;
    callManagerService->ReportOttCallEventInfo(eventInfo);
    int32_t slotId = 0;
    callManagerService->CloseUnFinishedUssd(slotId);
    std::string specialCode;
    callManagerService->InputDialerSpecialCode(specialCode);
    callManagerService->RemoveMissedIncomingCallNotification();
    callManagerService->SetVoIPCallState(0);
    int32_t state = 1;
    callManagerService->GetVoIPCallState(state);
    CallManagerProxyType proxyType = CallManagerProxyType::PROXY_BLUETOOTH_CALL;
    callManagerService->GetProxyObjectPtr(proxyType);
    callManagerService->GetBundleInfo();
    callManagerService->ReportAudioDeviceInfo();
    std::string eventName = "EVENT_IS_CELIA_CALL";
    ASSERT_NE(callManagerService->SendCallUiEvent(callId, eventName), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_001
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerServiceStub_001, Function | MediumTest | Level3)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    int32_t res = -1;
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_DIAL_CALL),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_ANSWER_CALL),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_REJECT_CALL),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_HOLD_CALL),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_UNHOLD_CALL),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_DISCONNECT_CALL),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_STATE),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SWAP_CALL),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_HAS_CALL),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_IS_NEW_CALL_ALLOWED),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_IS_RINGING),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_IS_EMERGENCY_CALL),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_IS_EMERGENCY_NUMBER),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_IS_FORMAT_NUMBER),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerInterfaceCode::INTERFACE_IS_FORMAT_NUMBER_E164), data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_COMBINE_CONFERENCE),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SEPARATE_CONFERENCE),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_START_DTMF),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_STOP_DTMF),
        data, reply, option);
    res = callManagerService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerInterfaceCode::INTERFACE_POST_DIAL_PROCEED), data, reply, option);
    ASSERT_NE(res, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_002
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerServiceStub_002, Function | MediumTest | Level3)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_WAITING),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_WAITING),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_RESTRICTION),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_RESTRICTION),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerInterfaceCode::INTERFACE_SET_CALL_RESTRICTION_PASSWORD), data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_TRANSFER),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_TRANSFER),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerInterfaceCode::INTERFACE_CAN_SET_CALL_TRANSFER_TIME), data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_MAINID),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_SUBCALL_LIST_ID),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerInterfaceCode::INTERFACE_GET_CALL_LIST_ID_FOR_CONFERENCE), data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_MUTE),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_MUTE_RINGER),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_AUDIO_DEVICE),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_CTRL_CAMERA),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_PREVIEW_WINDOW),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_DISPLAY_WINDOW),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_CAMERA_ZOOM),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_PAUSE_IMAGE),
        data, reply, option);
    int32_t res = callManagerService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerInterfaceCode::INTERFACE_SET_DEVICE_DIRECTION), data, reply, option);
    ASSERT_NE(res, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_003
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerServiceStub_003, Function | MediumTest | Level3)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SETCALL_PREFERENCEMODE),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_IMS_CONFIG),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_IMS_CONFIG),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_IMS_FEATURE_VALUE),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_IMS_FEATURE_VALUE),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_UPDATE_CALL_MEDIA_MODE),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_ENABLE_VOLTE),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_DISABLE_VOLTE),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_IS_VOLTE_ENABLED),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_START_RTT),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_STOP_RTT),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_JOIN_CONFERENCE),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(
    CallManagerInterfaceCode::INTERFACE_REPORT_OTT_CALL_DETAIL_INFO), data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(
    CallManagerInterfaceCode::INTERFACE_REPORT_OTT_CALL_EVENT_INFO), data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_PROXY_OBJECT_PTR),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_CLOSE_UNFINISHED_USSD),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_REPORT_AUDIO_DEVICE_INFO),
    data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(
    CallManagerInterfaceCode::INTERFACE_INPUT_DIALER_SPECIAL_CODE), data, reply, option);
callManagerService->OnRemoteRequest(static_cast<uint32_t>(
    CallManagerInterfaceCode::INTERFACE_CANCEL_MISSED_INCOMING_CALL_NOTIFICATION), data, reply, option);
int32_t res = callManagerService->OnRemoteRequest(static_cast<uint32_t>(
    CallManagerInterfaceCode::INTERFACE_SET_VONR_STATE), data, reply, option);
ASSERT_NE(res, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_004
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerServiceStub_004, Function | MediumTest | Level3)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.RewindRead(0);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_VONR_STATE),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_KICK_OUT_CONFERENCE),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SET_VOIP_CALL_STATE),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_GET_VOIP_CALL_STATE),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_CANCEL_CALL_UPGRADE),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerInterfaceCode::INTERFACE_REQUEST_CAMERA_CAPABILITIES), data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_REGISTER_CALLBACK),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_UNREGISTER_CALLBACK),
        data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerInterfaceCode::INTERFACE_VOIP_REGISTER_CALLBACK), data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerInterfaceCode::INTERFACE_VOIP_UNREGISTER_CALLBACK), data, reply, option);
    callManagerService->OnRemoteRequest(static_cast<uint32_t>(
        CallManagerInterfaceCode::INTERFACE_OBSERVER_ON_CALL_DETAILS_CHANGE), data, reply, option);
    ASSERT_NE(callManagerService->OnRemoteRequest(
        static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_SEND_CALLUI_EVENT),
        data, reply, option), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_005
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerServiceStub_005, Function | MediumTest | Level3)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    MessageParcel data;
    MessageParcel reply;
    int32_t callId = 0;
    data.WriteInt32(callId);
    data.RewindRead(0);
    callManagerService->OnHangUpCall(data, reply);
    callManagerService->OnGetCallState(data, reply);
    callManagerService->OnHoldCall(data, reply);
    callManagerService->OnUnHoldCall(data, reply);
    callManagerService->OnSwitchCall(data, reply);
    callManagerService->OnHasCall(data, reply);
    callManagerService->OnIsNewCallAllowed(data, reply);
    callManagerService->OnMuteRinger(data, reply);
    callManagerService->OnIsRinging(data, reply);
    callManagerService->OnIsInEmergencyCall(data, reply);
    callManagerService->OnStopDtmf(data, reply);
    callManagerService->OnGetCallWaiting(data, reply);
    callManagerService->OnCombineConference(data, reply);
    callManagerService->OnSeparateConference(data, reply);
    callManagerService->OnKickOutFromConference(data, reply);
    callManagerService->OnGetMainCallId(data, reply);
    callManagerService->OnGetSubCallIdList(data, reply);
    callManagerService->OnGetCallIdListForConference(data, reply);
    callManagerService->OnEnableVoLte(data, reply);
    callManagerService->OnDisableVoLte(data, reply);
    callManagerService->OnIsVoLteEnabled(data, reply);
    callManagerService->OnGetVoNRState(data, reply);
    callManagerService->OnStopRtt(data, reply);
    callManagerService->OnCloseUnFinishedUssd(data, reply);
    callManagerService->OnInputDialerSpecialCode(data, reply);
    callManagerService->OnRemoveMissedIncomingCallNotification(data, reply);
    callManagerService->OnSetVoIPCallState(data, reply);
    callManagerService->OnGetVoIPCallState(data, reply);
    callManagerService->OnReportAudioDeviceInfo(data, reply);
    callManagerService->OnCancelCallUpgrade(data, reply);
    callManagerService->OnRequestCameraCapabilities(data, reply);
    callManagerService->OnSetAudioDevice(data, reply);
    callManagerService->OnRegisterVoipCallManagerCallback(data, reply);
    callManagerService->OnUnRegisterVoipCallManagerCallback(data, reply);
    ASSERT_EQ(callManagerService->OnGetProxyObjectPtr(data, reply), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_006
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerServiceStub_006, Function | MediumTest | Level3)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    MessageParcel data;
    MessageParcel reply;
    int32_t callId = 0;
    int32_t videoState = 1;
    data.WriteInt32(callId);
    data.WriteInt32(videoState);
    data.RewindRead(0);
    callManagerService->OnAcceptCall(data, reply);
    callManagerService->OnGetCallRestriction(data, reply);
    callManagerService->OnGetTransferNumber(data, reply);
    callManagerService->OnSetCallPreferenceMode(data, reply);
    callManagerService->OnSetDeviceDirection(data, reply);
    callManagerService->OnGetImsConfig(data, reply);
    callManagerService->OnGetImsFeatureValue(data, reply);
    callManagerService->OnSetVoNRState(data, reply);

    MessageParcel data1;
    MessageParcel reply1;
    std::string message("hello");
    data1.WriteInt32(callId);
    data1.WriteBool(false);
    data1.WriteString16(Str8ToStr16(message));
    data1.RewindRead(0);
    callManagerService->OnRejectCall(data1, reply1);

    MessageParcel data2;
    MessageParcel reply2;
    data2.WriteBool(false);
    data2.RewindRead(0);
    callManagerService->OnSetMute(data2, reply2);

    MessageParcel data3;
    data3.WriteInt32(callId);
    data3.WriteInt8('c');
    data3.RewindRead(0);
    callManagerService->OnStartDtmf(data3, reply);

    MessageParcel data4;
    data4.WriteInt32(callId);
    data4.WriteBool(false);
    data4.RewindRead(0);
    callManagerService->OnPostDialProceed(data4, reply);
    callManagerService->OnSetCallWaiting(data4, reply);

    MessageParcel data5;
    data5.WriteInt32(callId);
    std::vector<std::u16string> numberList;
    data5.WriteString16Vector(numberList);
    data5.RewindRead(0);
    ASSERT_EQ(callManagerService->OnJoinConference(data5, reply), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_007
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerServiceStub_007, Function | MediumTest | Level3)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    MessageParcel data;
    MessageParcel reply;
    int32_t callId = 0;
    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data.WriteInt32(callId);
    CallRestrictionInfo callRestrictionInfo;
    int length = sizeof(CallRestrictionInfo);
    data.WriteRawData((const void *)&callRestrictionInfo, length);
    data.RewindRead(0);
    callManagerService->OnSetCallRestriction(data, reply);

    MessageParcel data1;
    char accountNum[kMaxNumberLen + 1] = { 0 };
    data1.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data1.WriteInt32(callId);
    data1.WriteInt32(0);
    data1.WriteCString(accountNum);
    data1.WriteCString(accountNum);
    data1.RewindRead(0);
    callManagerService->OnSetCallRestrictionPassword(data1, reply);

    MessageParcel data2;
    data2.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data2.WriteInt32(callId);
    CallTransferInfo callTransferInfo;
    length = sizeof(CallTransferInfo);
    data2.WriteRawData((const void *)&callRestrictionInfo, length);
    data2.RewindRead(0);
    callManagerService->OnSetTransferNumber(data2, reply);

    MessageParcel data3;
    data3.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data3.WriteInt32(callId);
    data3.WriteBool(true);
    data3.RewindRead(0);
    callManagerService->OnCanSetCallTransferTime(data3, reply);
    
    MessageParcel data4;
    std::string message("hello");
    data4.WriteInt32(callId);
    data4.WriteString16(Str8ToStr16(message));
    data4.RewindRead(0);
    callManagerService->OnControlCamera(data4, reply);
    callManagerService->OnSetPausePicture(data4, reply);
    callManagerService->OnStartRtt(data4, reply);

    MessageParcel data5;
    float fnum = 0.0;
    data5.WriteFloat(fnum);
    data5.RewindRead(0);
    ASSERT_EQ(callManagerService->OnSetCameraZoom(data5, reply), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_008
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerServiceStub_008, Function | MediumTest | Level3)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    MessageParcel data6;
    MessageParcel reply;
    int32_t callId = 0;
    std::string message("12345678");
    data6.WriteString16(Str8ToStr16(message));
    data6.WriteInt32(callId);
    data6.RewindRead(0);
    callManagerService->OnIsEmergencyPhoneNumber(data6, reply);

    MessageParcel data7;
    std::string callNumber("12345678");
    std::string countryCode("101");
    data7.WriteString16(Str8ToStr16(callNumber));
    data7.WriteString16(Str8ToStr16(countryCode));
    data7.RewindRead(0);
    callManagerService->OnFormatPhoneNumber(data7, reply);
    callManagerService->OnFormatPhoneNumberToE164(data7, reply);

    MessageParcel data8;
    data8.WriteInt32(callId);
    data8.WriteInt32(0);
    data8.WriteString16(Str8ToStr16(message));
    data8.RewindRead(0);
    callManagerService->OnSetImsConfig(data8, reply);

    MessageParcel data9;
    data9.WriteInt32(callId);
    data9.WriteInt32(0);
    data9.WriteInt32(0);
    data9.RewindRead(0);
    callManagerService->OnSetImsFeatureValue(data9, reply);

    MessageParcel data11;
    data11.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    data11.WriteInt32(callId);
    ImsCallMode imsCallMode;
    int length = sizeof(ImsCallMode);
    data11.WriteRawData((const void *)&imsCallMode, length);
    data11.RewindRead(0);
    ASSERT_EQ(callManagerService->OnUpdateCallMediaMode(data11, reply), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallManagerServiceStub_009
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManagerServiceStub_009, Function | MediumTest | Level3)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    MessageParcel data;
    MessageParcel reply;
    std::string message("12345678");
    std::string bundleName("abc");
    int32_t defaultNumber = 0;
    data.WriteString16(Str8ToStr16(message));
    data.WriteInt32(defaultNumber);
    data.WriteInt32(defaultNumber);
    data.WriteInt32(defaultNumber);
    data.WriteInt32(defaultNumber);
    data.WriteInt32(defaultNumber);
    data.WriteString(bundleName);
    callManagerService->OnDialCall(data, reply);

    MessageParcel data12;
    data12.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    OttCallDetailsInfo ottCallDetailsInfo;
    int length = sizeof(OttCallDetailsInfo);
    data12.WriteInt32(length);
    data12.WriteRawData((const void *)&ottCallDetailsInfo, length);
    data12.RewindRead(0);
    callManagerService->OnReportOttCallDetailsInfo(data12, reply);

    MessageParcel data13;
    data13.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());
    OttCallEventInfo ottCallEventInfo;
    length = sizeof(OttCallEventInfo);
    data13.WriteRawData((const void *)&ottCallEventInfo, length);
    data13.RewindRead(0);
    callManagerService->OnReportOttCallEventInfo(data13, reply);

    MessageParcel data15;
    data15.WriteInt32(defaultNumber);
    data15.WriteString("hello");
    callManagerService->OnGetProxyObjectPtr(data15, reply);
    ASSERT_NE(callManagerService->OnSendCallUiEvent(data15, reply), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_VoipCall_001
 * @tc.name     test error nullptr branch with permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_VoipCall_001, Function | MediumTest | Level3)
{
    AccessToken token;
    DialParaInfo dialInfo;
    std::shared_ptr<VoIPCall> voIPCall = std::make_shared<VoIPCall>(dialInfo);
    EXPECT_EQ(voIPCall->DialingProcess(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->AnswerCall(0), TELEPHONY_SUCCESS);
    VoipCallEventInfo voipcallInfo;
    EXPECT_EQ(voIPCall->PackVoipCallInfo(voipcallInfo), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->RejectCall(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->HangUpCall(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->HoldCall(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->UnHoldCall(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->SwitchCall(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->SetMute(0, 0), TELEPHONY_SUCCESS);
    CallAttributeInfo info;
    voIPCall->GetCallAttributeInfo(info);
    EXPECT_EQ(voIPCall->CombineConference(), TELEPHONY_SUCCESS);
    voIPCall->HandleCombineConferenceFailEvent();
    EXPECT_EQ(voIPCall->SeparateConference(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->KickOutFromConference(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->CanCombineConference(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->CanSeparateConference(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->CanKickOutFromConference(), TELEPHONY_SUCCESS);
    int32_t mainCallId = 0;
    EXPECT_EQ(voIPCall->GetMainCallId(mainCallId), TELEPHONY_SUCCESS);
    std::vector<std::u16string> callIdList;
    EXPECT_EQ(voIPCall->GetSubCallIdList(callIdList), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->GetCallIdListForConference(callIdList), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->IsSupportConferenceable(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->LaunchConference(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->ExitConference(), TELEPHONY_SUCCESS);
    EXPECT_EQ(voIPCall->HoldConference(), TELEPHONY_SUCCESS);
}
} // namespace Telephony
} // namespace OHOS
