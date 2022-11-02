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

#ifndef BLUETOOTH_CALL_TEST_H
#define BLUETOOTH_CALL_TEST_H

#include <map>

#include "accesstoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace Telephony {
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

static HapInfoParams testInfoParams = {
    .userID = 1,
    .bundleName = "tel_call_manager_ui_test",
    .instIndex = 0,
    .appIDDesc = "test",
};

static PermissionDef testPermPlaceCallDef = {
    .permissionName = "ohos.permission.PLACE_CALL",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

static PermissionStateFull testPlaceCallState = {
    .permissionName = "ohos.permission.PLACE_CALL",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

static PermissionDef testPermSetTelephonyStateDef = {
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

static PermissionStateFull testSetTelephonyState = {
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

static PermissionDef testPermGetTelephonyStateDef = {
    .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

static PermissionStateFull testGetTelephonyState = {
    .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

static PermissionDef testPermAnswerCallDef = {
    .permissionName = "ohos.permission.ANSWER_CALL",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

static PermissionStateFull testAnswerCallState = {
    .permissionName = "ohos.permission.ANSWER_CALL",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

static HapPolicyParams testPolicyParams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testPermPlaceCallDef, testPermSetTelephonyStateDef, testPermGetTelephonyStateDef,
        testPermAnswerCallDef },
    .permStateList = { testPlaceCallState, testSetTelephonyState, testGetTelephonyState, testAnswerCallState },
};

class AccessToken {
public:
    AccessToken()
    {
        currentID_ = GetSelfTokenID();
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testInfoParams, testPolicyParams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(accessID_);
    }
    ~AccessToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }

private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};

enum class BluetoothCallFuncCode {
    TEST_DIAL = 0,
    TEST_ANSWER,
    TEST_REJECT,
    TEST_HANGUP,
    TEST_GET_CALL_STATE,
    TEST_HOLD,
    TEST_UNHOLD,
    TEST_SWITCH,
    TEST_COMBINE_CONFERENCE,
    TEST_SEPARATE_CONFERENCE,
    TEST_START_DTMF,
    TEST_STOP_DTMF,
    TEST_IS_RINGING,
    TEST_HAS_CALL,
    TEST_IS_NEW_CALL_ALLOWED,
    TEST_IS_IN_EMERGENCY_CALL,
    TEST_SET_MUTED,
    TEST_MUTE_RINGER,
    TEST_SET_AUDIO_DEVICE,
    TEST_GET_CURRENT_CALL_LIST,
};

class BluetoothCallTest {
public:
    BluetoothCallTest() = default;
    ~BluetoothCallTest() = default;
    int32_t Init();
    void RunBluetoothCallTest();

private:
    void PrintfUsage();
    void InitFunMap();
    using BluetoothCallFunc = void (BluetoothCallTest::*)();

    void DialCall();
    void AnswerCall();
    void RejectCall();
    void HangUpCall();
    void GetCallState();
    void HoldCall();
    void UnHoldCall();
    void SwitchCall();
    void CombineConference();
    void SeparateConference();
    void StartDtmf();
    void StopDtmf();
    void IsRinging();
    void HasCall();
    void IsNewCallAllowed();
    void IsInEmergencyCall();
    void SetMute();
    void MuteRinger();
    void SetAudioDevice();
    void GetCurrentCallList();

    std::map<uint32_t, BluetoothCallFunc> memberFuncMap_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_MANAGER_CALLBACK_TEST_H
