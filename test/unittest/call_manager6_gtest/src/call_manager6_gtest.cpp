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
#include "call_manager6_gtest.h"

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
#ifndef TEL_TEST_UNSUPPORT
constexpr int16_t SIM2_SLOTID = 1;
constexpr int16_t SIM1_SLOTID_NO_CARD = 0;
constexpr int16_t RETURN_VALUE_IS_ZERO = 0;
constexpr int16_t INVALID_NEGATIVE_ID = -100;
constexpr int16_t INVALID_POSITIVE_ID = 100;
constexpr int INVALID_DIAL_TYPE = 3;
constexpr int WAIT_TIME = 3;
#ifndef CALL_MANAGER_IMS_LITE_UNSUPPORT
constexpr int16_t CAMERA_ROTATION_90 = 90;
constexpr int16_t CAMERA_ROTATION_ERROR = 50;
#endif // CALL_MANAGER_IMS_LITE_UNSUPPORT
#endif // TEL_TEST_UNSUPPORT
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
    if (CallManager6Gtest::clientPtr_->GetCallState() == targetState) {
        return;
    }
    int32_t usedTimeMs = 0;
    std::cout << "wait for a few seconds......" << std::endl;
    while ((CallManager6Gtest::clientPtr_->GetCallState() != targetState) && (usedTimeMs < timeoutMs)) {
        usleep(slipMs * SLEEP_1000_MS);
        usedTimeMs += slipMs;
    }
    int32_t callState = CallManager6Gtest::clientPtr_->GetCallState();
    std::cout << "waited " << usedTimeMs << " seconds" << std::endl;
    std::cout << "target call state:" << targetState << std::endl;
    EXPECT_EQ(callState, targetState);
}

void CallManager6Gtest::HangUpCall()
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

/******************************************* Test SetCameraZoom() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetCameraZoom_0100
 * @tc.name     input zoomRatio 5.0, Test SetCameraZoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetCameraZoom_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 5.0;
    EXPECT_EQ(CallManager6Gtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetCameraZoom_0200
 * @tc.name     input zoomRatio 0.1, Test SetCameraZoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetCameraZoom_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 0.1;
    EXPECT_EQ(CallManager6Gtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetCameraZoom_0300
 * @tc.name     input zoomRatio 10.0, Test SetCameraZoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetCameraZoom_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 10.0;
    EXPECT_EQ(CallManager6Gtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetCameraZoom_0400
 * @tc.name     input invalid para zoomRatio 15.0, Test SetCameraZoom, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetCameraZoom_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 15.0;
    EXPECT_EQ(CallManager6Gtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetCameraZoom_0500
 * @tc.name     input invalid para zoomRatio 0.0, Test SetCameraZoom, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetCameraZoom_0500, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 0.0;
    EXPECT_EQ(CallManager6Gtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetCameraZoom_0600
 * @tc.name     input invalid para zoomRatio -1.0, Test SetCameraZoom, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetCameraZoom_0600, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = -1.0;
    EXPECT_EQ(CallManager6Gtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetCameraZoom_0700
 * @tc.name     input invalid para zoomRatio 10.1, Test SetCameraZoom, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetCameraZoom_0700, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 10.1;
    EXPECT_EQ(CallManager6Gtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}
/******************************************* Test SetPausePicture() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0100
 * @tc.name     input locale picture path /system/bin/test.png that does not exist, Test SetPausePicture,
 *              return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetPausePicture_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string pathTemp = "/system/bin/test.png";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManager6Gtest::clientPtr_->SetPausePicture(callId, path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0200
 * @tc.name     input an invalid image format file path, Test SetPausePicture, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetPausePicture_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string pathTemp = "/system/bin/test.jpg";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManager6Gtest::clientPtr_->SetPausePicture(callId, path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0300
 * @tc.name     input an invalid image format file path, Test SetPausePicture, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetPausePicture_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string pathTemp = "/system/bin/test.bmp";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManager6Gtest::clientPtr_->SetPausePicture(callId, path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0400
 * @tc.name     input an invalid image format file path, Test SetPausePicture, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetPausePicture_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string pathTemp = "/system/bin/test.webp";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManager6Gtest::clientPtr_->SetPausePicture(callId, path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0500
 * @tc.name     input remote picture path http://callManager.jpg that does not exist, Test SetPausePicture,
 *              return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetPausePicture_0500, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string pathTemp = "http://callManager.jpg";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManager6Gtest::clientPtr_->SetPausePicture(callId, path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0600
 * @tc.name     input empty picture path, Test SetPausePicture, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetPausePicture_0600, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::string pathTemp = "";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManager6Gtest::clientPtr_->SetPausePicture(callId, path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_CancelCallUpgrade_0100
 * @tc.name     input callId:1, Test CancelCallUpgrade, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_CancelCallUpgrade_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    EXPECT_NE(CallManager6Gtest::clientPtr_->CancelCallUpgrade(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_RequestCameraCapabilities_0100
 * @tc.name     input callId:1, Test RequestCameraCapabilities, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_RequestCameraCapabilities_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    EXPECT_NE(CallManager6Gtest::clientPtr_->RequestCameraCapabilities(callId), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test GetImsConfig() ************************************************/
/**
 * @tc.number   Telephony_CallManager_GetImsConfig_0100
 * @tc.name     test get ims config item video quality
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_GetImsConfig_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->GetImsConfig(SIM1_SLOTID, ITEM_VIDEO_QUALITY), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->GetImsConfig(SIM2_SLOTID, ITEM_VIDEO_QUALITY), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetImsConfig_0200
 * @tc.name     test get ims config item ims switch status
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_GetImsConfig_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManager6Gtest::clientPtr_->GetImsConfig(SIM1_SLOTID, ITEM_IMS_SWITCH_STATUS), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManager6Gtest::clientPtr_->GetImsConfig(SIM2_SLOTID, ITEM_IMS_SWITCH_STATUS), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetImsConfig_0300
 * @tc.name     test get ims config item with invalid item
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_GetImsConfig_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    ImsConfigItem item = static_cast<ImsConfigItem>(2);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->GetImsConfig(SIM1_SLOTID, item), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->GetImsConfig(SIM2_SLOTID, item), RETURN_VALUE_IS_ZERO);
    }
}

/********************************************* Test SetImsConfig() ************************************************/
/**
 * @tc.number   Telephony_CallManager_SetImsConfig_0100
 * @tc.name     test set ims config item video quality
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetImsConfig_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::u16string value = u"123";
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManager6Gtest::clientPtr_->SetImsConfig(SIM1_SLOTID, ITEM_VIDEO_QUALITY, value), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManager6Gtest::clientPtr_->SetImsConfig(SIM2_SLOTID, ITEM_VIDEO_QUALITY, value), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetImsConfig_0200
 * @tc.name     test set ims config item ims switch status
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetImsConfig_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::u16string value = u"123";
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsConfig(SIM1_SLOTID, ITEM_IMS_SWITCH_STATUS, value),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsConfig(SIM2_SLOTID, ITEM_IMS_SWITCH_STATUS, value),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetImsConfig_0300
 * @tc.name     test set ims config item with invalid item
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetImsConfig_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::u16string value = u"123";
    ImsConfigItem item = static_cast<ImsConfigItem>(2);
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsConfig(SIM1_SLOTID, item, value), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsConfig(SIM2_SLOTID, item, value), RETURN_VALUE_IS_ZERO);
    }
}

/*********************************** Test GetImsFeatureValue() ****************************************/
/**
 * @tc.number   Telephony_CallManager_GetImsFeatureValue_0100
 * @tc.name     test get ims feature value type voice over lte
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_GetImsFeatureValue_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManager6Gtest::clientPtr_->GetImsFeatureValue(SIM1_SLOTID, TYPE_VOICE_OVER_LTE), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManager6Gtest::clientPtr_->GetImsFeatureValue(SIM2_SLOTID, TYPE_VOICE_OVER_LTE), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetImsFeatureValue_0200
 * @tc.name     test get ims feature value type video over lte
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_GetImsFeatureValue_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManager6Gtest::clientPtr_->GetImsFeatureValue(SIM1_SLOTID, TYPE_VIDEO_OVER_LTE), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManager6Gtest::clientPtr_->GetImsFeatureValue(SIM2_SLOTID, TYPE_VIDEO_OVER_LTE), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetImsFeatureValue_0300
 * @tc.name     test get ims feature value type ss over ut
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_GetImsFeatureValue_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->GetImsFeatureValue(SIM1_SLOTID, TYPE_SS_OVER_UT), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->GetImsFeatureValue(SIM2_SLOTID, TYPE_SS_OVER_UT), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetImsFeatureValue_0400
 * @tc.name     test get ims feature value with invalid type
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_GetImsFeatureValue_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    FeatureType type = static_cast<FeatureType>(3);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->GetImsFeatureValue(SIM1_SLOTID, type), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->GetImsFeatureValue(SIM2_SLOTID, type), RETURN_VALUE_IS_ZERO);
    }
}

/***************************************** Test SetImsFeatureValue() ***************************************/
/**
 * @tc.number   Telephony_CallManager_SetImsFeatureValue_0100
 * @tc.name     test set ims feature value type voice over lte
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetImsFeatureValue_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t value = 1;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsFeatureValue(SIM1_SLOTID, TYPE_VOICE_OVER_LTE, value),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsFeatureValue(SIM2_SLOTID, TYPE_VOICE_OVER_LTE, value),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetImsFeatureValue_0200
 * @tc.name     test set ims feature value type video over lte
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetImsFeatureValue_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t value = 1;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsFeatureValue(SIM1_SLOTID, TYPE_VIDEO_OVER_LTE, value),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsFeatureValue(SIM2_SLOTID, TYPE_VIDEO_OVER_LTE, value),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetImsFeatureValue_0300
 * @tc.name     test set ims feature value type ss over ut
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetImsFeatureValue_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t value = 1;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsFeatureValue(SIM1_SLOTID, TYPE_SS_OVER_UT, value),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsFeatureValue(SIM2_SLOTID, TYPE_SS_OVER_UT, value),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetImsFeatureValue_0400
 * @tc.name     test set ims feature value with invalid type
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetImsFeatureValue_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t value = 1;
    FeatureType type = static_cast<FeatureType>(3);
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsFeatureValue(SIM1_SLOTID, type, value), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->SetImsFeatureValue(SIM2_SLOTID, type, value), RETURN_VALUE_IS_ZERO);
    }
}

/************************************** Test UpdateImsCallMode() ****************************************/
/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0100
 * @tc.name     test update ims call mode audio only
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_UpdateImsCallMode_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    EXPECT_NE(CallManager6Gtest::clientPtr_->UpdateImsCallMode(callId, CALL_MODE_AUDIO_ONLY), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0200
 * @tc.name     test update ims call mode send only
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_UpdateImsCallMode_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    EXPECT_NE(CallManager6Gtest::clientPtr_->UpdateImsCallMode(callId, CALL_MODE_SEND_ONLY), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0300
 * @tc.name     test update ims call mode recerve only
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_UpdateImsCallMode_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    EXPECT_NE(CallManager6Gtest::clientPtr_->UpdateImsCallMode(callId, CALL_MODE_RECEIVE_ONLY), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0400
 * @tc.name     test update ims call mode send receive
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_UpdateImsCallMode_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    EXPECT_NE(CallManager6Gtest::clientPtr_->UpdateImsCallMode(callId, CALL_MODE_SEND_RECEIVE), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0500
 * @tc.name     test update ims call mode video paused
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_UpdateImsCallMode_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    EXPECT_NE(CallManager6Gtest::clientPtr_->UpdateImsCallMode(callId, CALL_MODE_VIDEO_PAUSED), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0600
 * @tc.name     test update ims call with invalid mode
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_UpdateImsCallMode_0600, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    ImsCallMode mode = static_cast<ImsCallMode>(5);
    EXPECT_NE(CallManager6Gtest::clientPtr_->UpdateImsCallMode(callId, mode), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test StartRtt() ************************************************/
/**
 * @tc.number   Telephony_CallManager_StartRtt_0100
 * @tc.name     test start rtt
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_StartRtt_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::u16string msg = u"123";
    int32_t callId = 1;
    EXPECT_NE(CallManager6Gtest::clientPtr_->StartRtt(callId, msg), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test StopRtt() ************************************************/
/**
 * @tc.number   Telephony_CallManager_StopRtt_0100
 * @tc.name     test stop rtt
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_StopRtt_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    EXPECT_NE(CallManager6Gtest::clientPtr_->StopRtt(callId), RETURN_VALUE_IS_ZERO);
}

/**************************************** Test CanSetCallTransferTime() ******************************************/
/**
 * @tc.number   Telephony_CallManager_CanSetCallTransferTime_0100
 * @tc.name     input invalid slotId, test CanSetCallTransferTime() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_CanSetCallTransferTime_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    bool result;
    EXPECT_EQ(CallManager6Gtest::clientPtr_->CanSetCallTransferTime(INVALID_SLOT_ID, result), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_CanSetCallTransferTime_0200
 * @tc.name     input slotId was out of count
 *              test CanSetCallTransferTime() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_CanSetCallTransferTime_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    bool result;
    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManager6Gtest::clientPtr_->CanSetCallTransferTime(slotId, result), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_CanSetCallTransferTime_0300
 * @tc.name     test CanSetCallTransferTime() without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_CanSetCallTransferTime_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    bool result;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManager6Gtest::clientPtr_->CanSetCallTransferTime(SIM1_SLOTID, result), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManager6Gtest::clientPtr_->CanSetCallTransferTime(SIM2_SLOTID, result), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/**
 * @tc.number   Telephony_CallManager_CanSetCallTransferTime_0400
 * @tc.name     test CanSetCallTransferTime() with normal situation
 * @tc.desc     Function test
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_CanSetCallTransferTime_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    bool result;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->CanSetCallTransferTime(SIM1_SLOTID, result), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManager6Gtest::clientPtr_->CanSetCallTransferTime(SIM2_SLOTID, result), RETURN_VALUE_IS_ZERO);
    }
}

/********************************************* Test SetAudioDevice()***********************************************/
/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0100
 * @tc.name     make a normal buletoothAddress, set active bluetooth device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManager6Gtest, Telephony_CallManager_SetAudioDevice_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string phoneNumber = "66666666666";
    EXPECT_EQ(CallManager6Gtest::IsServiceConnected(), true);
    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t ret = CallManager6Gtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);

    HandsFreeAudioGateway *profile = HandsFreeAudioGateway::GetProfile();
    vector<BluetoothRemoteDevice> devices = profile->GetConnectedDevices();
    int deviceNum = devices.size();
    std::string address = "";
    const int32_t zeroDevice = 0;
    if (deviceNum > zeroDevice) {
        address = devices[0].GetDeviceAddr();
    }

    AudioDevice audioDevice;
    if (memset_s(&audioDevice, sizeof(AudioDevice), 0, sizeof(AudioDevice)) != EOK) {
        TELEPHONY_LOGE("memset_s fail");
        return;
    }
    audioDevice.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
    if (address.length() > kMaxAddressLen) {
        TELEPHONY_LOGE("address is not too long");
        return;
    }
    if (memcpy_s(audioDevice.address, kMaxAddressLen, address.c_str(), address.length()) != EOK) {
        TELEPHONY_LOGE("memcpy_s address fail");
        return;
    }

    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_OFFHOOK, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_NE(clientPtr_->SetAudioDevice(audioDevice), RETURN_VALUE_IS_ZERO);
    sleep(WAIT_TIME);

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
    }
}
} // namespace Telephony
} // namespace OHOS
