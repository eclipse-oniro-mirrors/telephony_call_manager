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

#include <memory>
#include <securec.h>
#include <string_ex.h>

#include "audio_system_manager.h"
#include "system_ability_definition.h"
#include "input/camera_manager.h"
#include "i_call_manager_service.h"
#include "call_manager_client.h"

#include "audio_player.h"
#include "bluetooth_call_test.h"
#include "call_manager_inner_type.h"
#include "call_manager_errors.h"
#include "call_manager_callback_test.h"
#include "call_manager_test_types.h"
#include "common_event_subscriber_test.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::Security::AccessToken;
using OHOS::Security::AccessToken::AccessTokenID;

enum class CallManagerInterfaceType {
    INTERFACE_CALL_MANAGER_TYPE = 1,
    INTERFACE_BLUETOOTH_CALL_TYPE,
};

std::shared_ptr<CallManagerClient> g_clientPtr = nullptr;
using CallManagerServiceFunc = void (*)();
std::map<uint32_t, CallManagerServiceFunc> g_memberFuncMap;

HapInfoParams testInfoParams = {
    .userID = 1,
    .bundleName = "tel_call_manager_ui_test",
    .instIndex = 0,
    .appIDDesc = "test",
};

PermissionDef testPermPlaceCallDef = {
    .permissionName = "ohos.permission.PLACE_CALL",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

PermissionStateFull testPlaceCallState = {
    .permissionName = "ohos.permission.PLACE_CALL",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

PermissionDef testPermSetTelephonyStateDef = {
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

PermissionStateFull testSetTelephonyState = {
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

PermissionDef testPermGetTelephonyStateDef = {
    .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

PermissionStateFull testGetTelephonyState = {
    .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

PermissionDef testPermAnswerCallDef = {
    .permissionName = "ohos.permission.ANSWER_CALL",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

PermissionStateFull testAnswerCallState = {
    .permissionName = "ohos.permission.ANSWER_CALL",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

PermissionDef testReadCallLogDef = {
    .permissionName = "ohos.permission.READ_CALL_LOG",
    .bundleName = "tel_call_manager_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
};

PermissionStateFull testPermReadCallLog = {
    .permissionName = "ohos.permission.READ_CALL_LOG",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

PermissionDef testWriteCallLogDef = {
    .permissionName = "ohos.permission.WRITE_CALL_LOG",
    .bundleName = "tel_call_manager_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
};

PermissionStateFull testPermWriteCallLog = {
    .permissionName = "ohos.permission.WRITE_CALL_LOG",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

PermissionDef testStartAbilityFromBGDef = {
    .permissionName = "ohos.permission.START_ABILITIES_FROM_BACKGROUND",
    .bundleName = "tel_call_manager_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
};

PermissionStateFull testPermStartAbilityFromBG = {
    .permissionName = "ohos.permission.START_ABILITIES_FROM_BACKGROUND",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

HapPolicyParams testPolicyParams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testPermPlaceCallDef, testPermSetTelephonyStateDef, testPermGetTelephonyStateDef,
        testPermAnswerCallDef, testReadCallLogDef, testWriteCallLogDef, testStartAbilityFromBGDef },
    .permStateList = { testPlaceCallState, testSetTelephonyState, testGetTelephonyState, testAnswerCallState,
        testPermReadCallLog, testPermWriteCallLog, testPermStartAbilityFromBG },
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

void DialCall()
{
    AccessToken token;
    int32_t accountId = DEFAULT_ACCOUNT_ID;
    int32_t videoState = DEFAULT_VIDEO_STATE;
    int32_t dialScene = DEFAULT_DIAL_SCENE;
    int32_t dialType = DEFAULT_DIAL_TYPE;
    int32_t callType = DEFAULT_CALL_TYPE;
    std::u16string phoneNumber;
    std::string tmpStr;
    AppExecFwk::PacMap dialInfo;
    std::cout << "------Dial------" << std::endl;
    std::cout << "please input phone number:" << std::endl;
    phoneNumber.clear();
    tmpStr.clear();
    std::cin >> tmpStr;
    phoneNumber = Str8ToStr16(tmpStr);
    std::cout << "you want to call " << tmpStr << std::endl;
    std::cout << "please input accountId:" << std::endl;
    std::cin >> accountId;
    std::cout << "please input videoState[0:audio,1:video]:" << std::endl;
    std::cin >> videoState;
    std::cout << "please input dialScene[0:normal,1:privileged,2:emergency]:" << std::endl;
    std::cin >> dialScene;
    std::cout << "please input dialType[0:carrier,1:voice mail,2:ott]:" << std::endl;
    std::cin >> dialType;
    std::cout << "please input callType[0:cs,1:ims,2:ott]:" << std::endl;
    std::cin >> callType;

    dialInfo.PutIntValue("accountId", accountId);
    dialInfo.PutIntValue("videoState", videoState);
    dialInfo.PutIntValue("dialScene", dialScene);
    dialInfo.PutIntValue("dialType", dialType);
    dialInfo.PutIntValue("callType", callType);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->DialCall(phoneNumber, dialInfo);
    std::cout << "return value:" << ret << std::endl;
}

void AnswerCall()
{
    AccessToken token;
    int32_t callId = DEFAULT_CALL_ID;
    int32_t videoState = DEFAULT_VIDEO_STATE;
    std::cout << "------Answer------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::cout << "please input videoState[0:audio,1:video]:" << std::endl;
    std::cin >> videoState;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->AnswerCall(callId, videoState);
    std::cout << "return value:" << ret << std::endl;
}

void RejectCall()
{
    AccessToken token;
    int32_t callId = DEFAULT_CALL_ID;
    int32_t boolValue = DEFAULT_VALUE;
    bool flag = false;
    std::u16string content;
    content.clear();
    std::cout << "------Reject------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::cout << "Whether to enter the reason for rejection?[0:no,1:yes]:" << std::endl;
    std::cin >> boolValue;
    if (boolValue != DEFAULT_VALUE) {
        flag = true;
        std::string tmpStr;
        tmpStr.clear();
        std::cout << "please input reject message:" << std::endl;
        std::cin >> tmpStr;
        content = Str8ToStr16(tmpStr);
    }
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->RejectCall(callId, flag, content);
    std::cout << "return value:" << ret << std::endl;
}

void HoldCall()
{
    AccessToken token;
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------HoldCall------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->HoldCall(callId);
    std::cout << "return value:" << ret << std::endl;
}

void UnHoldCall()
{
    AccessToken token;
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------UnHoldCall------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->UnHoldCall(callId);
    std::cout << "return value:" << ret << std::endl;
}

void HangUpCall()
{
    AccessToken token;
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------HangUpCall------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->HangUpCall(callId);
    std::cout << "return value:" << ret << std::endl;
}

void CombineConference()
{
    int32_t mainCallId = DEFAULT_CALL_ID;
    std::cout << "------CombineConference------" << std::endl;
    std::cout << "please input mainCallId:" << std::endl;
    std::cin >> mainCallId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->CombineConference(mainCallId);
    std::cout << "return value:" << ret << std::endl;
}

void SeparateConference()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------SeparateConference------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SeparateConference(callId);
    std::cout << "return value:" << ret << std::endl;
}

void GetCallState()
{
    std::cout << "------GetCallState------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->GetCallState();
    std::cout << "return value:" << ret << std::endl;
}

void SwitchCall()
{
    AccessToken token;
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------SwitchCall------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SwitchCall(callId);
    std::cout << "return value:" << ret << std::endl;
}

void HasCall()
{
    std::cout << "------HasCall------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->HasCall();
    std::cout << "return value:" << ret << std::endl;
}

void IsNewCallAllowed()
{
    std::cout << "------IsNewCallAllowed------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    bool enabled = false;
    int32_t ret = g_clientPtr->IsNewCallAllowed(enabled);
    std::cout << "return result:" << ret << std::endl;
    std::cout << "enabled value:" << enabled << std::endl;
}

void IsRinging()
{
    AccessToken token;
    std::cout << "------IsRinging------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    bool enabled = false;
    int32_t ret = g_clientPtr->IsRinging(enabled);
    std::cout << "return result:" << ret << std::endl;
    std::cout << "enabled value:" << enabled << std::endl;
}

void IsInEmergencyCall()
{
    AccessToken token;
    std::cout << "------IsInEmergencyCall------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    bool enabled = false;
    int32_t ret = g_clientPtr->IsInEmergencyCall(enabled);
    std::cout << "return result:" << ret << std::endl;
    std::cout << "enabled value:" << enabled << std::endl;
}

void StartDtmf()
{
    char c = DEFAULT_VALUE;
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "please input StartDtmf callId:" << std::endl;
    std::cin >> callId;
    std::cout << "Please enter to send dtmf characters:" << std::endl;
    std::cin >> c;
    int32_t ret = g_clientPtr->StartDtmf(callId, c);
    std::cout << "return value:" << ret << std::endl;
}

void StopDtmf()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "please input StopDtmf callId:" << std::endl;
    std::cin >> callId;
    int32_t ret = g_clientPtr->StopDtmf(callId);
    std::cout << "return value:" << ret << std::endl;
}

void GetCallWaiting()
{
    AccessToken token;
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------GetCallWaiting------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->GetCallWaiting(slotId);
    std::cout << "return value:" << ret << std::endl;
}

void SetCallWaiting()
{
    AccessToken token;
    int32_t slotId = SIM1_SLOTID;
    int32_t flag = DEFAULT_VALUE;
    std::cout << "------SetCallWaiting------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "whether open(0:no 1:yes):" << std::endl;
    std::cin >> flag;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetCallWaiting(slotId, (flag == 1) ? true : false);
    std::cout << "return value:" << ret << std::endl;
}

void GetCallRestriction()
{
    AccessToken token;
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = DEFAULT_VALUE;
    CallRestrictionType type;
    std::cout << "------GetCallRestriction------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input restriction type:" << std::endl;
    std::cin >> tmpType;
    type = static_cast<CallRestrictionType>(tmpType);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->GetCallRestriction(slotId, type);
    std::cout << "return value:" << ret << std::endl;
}

void SetCallRestriction()
{
    AccessToken token;
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = DEFAULT_VALUE;
    CallRestrictionInfo info;
    std::cout << "------SetCallRestriction------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input restriction type:" << std::endl;
    std::cin >> tmpType;
    info.fac = static_cast<CallRestrictionType>(tmpType);
    std::cout << "is open(1: open, 0: close):" << std::endl;
    std::cin >> tmpType;
    info.mode = static_cast<CallRestrictionMode>(tmpType);
    std::cout << "please input password:" << std::endl;
    std::cin >> info.password;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetCallRestriction(slotId, info);
    std::cout << "return value:" << ret << std::endl;
}

void SetCallPreferenceMode()
{
    AccessToken token;
    int32_t slotId = SIM1_SLOTID;
    int32_t mode = DEFAULT_PREFERENCEMODE;
    std::cout << "------CallPreferenceMode------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input PreferenceMode:" << std::endl;
    std::cout << "CS_VOICE_ONLY = 1" << std::endl;
    std::cout << "CS_VOICE_PREFERRED = 2" << std::endl;
    std::cout << "IMS_PS_VOICE_PREFERRED = 3" << std::endl;
    std::cout << "IMS_PS_VOICE_ONLY = 4" << std::endl;
    std::cin >> mode;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetCallPreferenceMode(slotId, mode);
    std::cout << "return value:" << ret << std::endl;
}

void GetCallTransferInfo()
{
    AccessToken token;
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = DEFAULT_VALUE;
    CallTransferType type;
    std::cout << "------GetCallTransferInfo------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input transfer type:" << std::endl;
    std::cin >> tmpType;
    type = static_cast<CallTransferType>(tmpType);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->GetCallTransferInfo(slotId, type);
    std::cout << "return value:" << ret << std::endl;
}

void SetCallTransferInfo()
{
    AccessToken token;
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = DEFAULT_VALUE;
    CallTransferInfo info;
    std::cout << "------SetCallTransferInfo------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input transfer type:" << std::endl;
    std::cin >> tmpType;
    info.type = static_cast<CallTransferType>(tmpType);
    std::cout << "please input transfer setting type:" << std::endl;
    std::cin >> tmpType;
    info.settingType = static_cast<CallTransferSettingType>(tmpType);
    std::cout << "please input phone number:" << std::endl;
    std::cin >> info.transferNum;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetCallTransferInfo(slotId, info);
    std::cout << "return value:" << ret << std::endl;
}

void CanSetCallTransferTime()
{
    AccessToken token;
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------CanSetCallTransferTime------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    bool result;
    int32_t ret = g_clientPtr->CanSetCallTransferTime(slotId, result);
    std::cout << "return value:" << ret << std::endl;
    std::cout << "return result:" << result << std::endl;
}

void IsEmergencyPhoneNumber()
{
    int32_t slotId = SIM1_SLOTID;
    std::u16string phoneNumber;
    std::string tmpStr;
    std::cout << "------IsEmergencyPhoneNumber------" << std::endl;
    std::cout << "please input phone number:" << std::endl;
    phoneNumber.clear();
    tmpStr.clear();
    std::cin >> tmpStr;
    phoneNumber = Str8ToStr16(tmpStr);
    std::cout << "The number is " << tmpStr << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    bool enabled = false;
    int32_t ret = g_clientPtr->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    std::cout << "return value:" << enabled << std::endl;
    std::cout << "return errorCode:" << ret << std::endl;
}

void FormatPhoneNumber()
{
    std::u16string phoneNumber;
    std::u16string countryCode;
    std::u16string formatNumber;
    std::string tmpStr;
    std::cout << "------FormatPhoneNumber------" << std::endl;
    std::cout << "please input phone number:" << std::endl;
    phoneNumber.clear();
    countryCode.clear();
    formatNumber.clear();
    tmpStr.clear();
    std::cin >> tmpStr;
    phoneNumber = Str8ToStr16(tmpStr);
    std::cout << "The number is " << tmpStr << std::endl;
    tmpStr.clear();
    std::cout << "please input countryCode:" << std::endl;
    std::cin >> tmpStr;
    countryCode = Str8ToStr16(tmpStr);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->FormatPhoneNumber(phoneNumber, countryCode, formatNumber);
    std::cout << "return value:" << ret << std::endl;
    std::cout << "return number:" << Str16ToStr8(formatNumber) << std::endl;
}

void FormatPhoneNumberToE164()
{
    std::u16string phoneNumber;
    std::u16string countryCode;
    std::u16string formatNumber;
    std::string tmpStr;
    std::cout << "------FormatPhoneNumberToE164------" << std::endl;
    std::cout << "please input phone number:" << std::endl;
    phoneNumber.clear();
    countryCode.clear();
    formatNumber.clear();
    tmpStr.clear();
    std::cin >> tmpStr;
    phoneNumber = Str8ToStr16(tmpStr);
    std::cout << "The number is " << tmpStr << std::endl;
    tmpStr.clear();
    std::cout << "please input countryCode:" << std::endl;
    std::cin >> tmpStr;
    countryCode = Str8ToStr16(tmpStr);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->FormatPhoneNumberToE164(phoneNumber, countryCode, formatNumber);
    std::cout << "return value:" << ret << std::endl;
    std::cout << "return number:" << Str16ToStr8(formatNumber) << std::endl;
}

void GetMainCallId()
{
    int callId = DEFAULT_CALL_ID;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    int mainCallId = DEFAULT_CALL_ID;
    int32_t ret = g_clientPtr->GetMainCallId(callId, mainCallId);
    std::cout << "return value:" << ret << std::endl;
    std::cout << "mainCallId value:" << mainCallId << std::endl;
}

void GetSubCallIdList()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::vector<std::u16string> result;
    result.clear();
    int32_t ret = g_clientPtr->GetSubCallIdList(callId, result);
    if (ret != TELEPHONY_SUCCESS) {
        std::cout << "GetSubCallIdList ret value:" << ret << std::endl;
    }
    std::vector<std::u16string>::iterator it = result.begin();
    for (; it != result.end(); ++it) {
        std::cout << "callId:" << Str16ToStr8(*it) << std::endl;
    }
}

void GetCallIdListForConference()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::vector<std::u16string> result;
    result.clear();
    int32_t ret = g_clientPtr->GetCallIdListForConference(callId, result);
    if (ret != TELEPHONY_SUCCESS) {
        std::cout << "GetCallIdListForConference ret value:" << ret << std::endl;
    }
    std::vector<std::u16string>::iterator it = result.begin();
    for (; it != result.end(); ++it) {
        std::cout << "callId:" << Str16ToStr8(*it) << std::endl;
    }
}

void InviteToConference()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::string number;
    std::vector<std::u16string> numberList;
    std::cout << "please input participate phone number:[-1]end" << std::endl;
    while (std::cin >> number) {
        numberList.push_back(Str8ToStr16(number));
        if (number == "-1") {
            break;
        }
    }
    int32_t ret = g_clientPtr->JoinConference(callId, numberList);
    std::cout << "return value:" << ret << std::endl;
}

void SetMute()
{
    int32_t isMute = DEFAULT_VALUE;
    std::cout << "------SetMute------" << std::endl;
    std::cout << "please input mute state(0:false 1:true):" << std::endl;
    std::cin >> isMute;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetMuted((isMute == 1) ? true : false);
    std::cout << "return value:" << ret << std::endl;
}

void MuteRinger()
{
    AccessToken token;
    std::cout << "------MuteRinger------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->MuteRinger();
    std::cout << "return value:" << ret << std::endl;
}

void SetAudioDevice()
{
    int32_t deviceType = DEFAULT_VALUE;
    AudioDevice audioDevice;
    std::cout << "------SetAudioDevice------" << std::endl;
    std::cout << "please input device type(0:earpiece 1:speaker 2:wired headset 3:bluetooth sco):" << std::endl;
    std::cin >> deviceType;
    if (deviceType == static_cast<int32_t>(AudioDeviceType::DEVICE_BLUETOOTH_SCO)) {
        std::cout << "please input bluetoothAddress:" << std::endl;
        std::cin >> audioDevice.address;
    }
    audioDevice.deviceType = static_cast<AudioDeviceType>(deviceType);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetAudioDevice(audioDevice);
    std::cout << "return value:" << ret << std::endl;
}

void GetVolume()
{
    int32_t type = DEFAULT_VALUE;
    std::cout << "------GetVolume------" << std::endl;
    std::cout << "please input volume type(3:ring 4:music)" << std::endl;
    std::cin >> type;
    AudioStandard::AudioVolumeType volumeType =
        AudioStandard::AudioVolumeType::STREAM_MUSIC;
    switch (type) {
        case AudioStandard::AudioVolumeType::STREAM_RING:
            volumeType = AudioStandard::AudioVolumeType::STREAM_RING;
            break;
        case AudioStandard::AudioVolumeType::STREAM_MUSIC:
            volumeType = AudioStandard::AudioVolumeType::STREAM_MUSIC;
            break;
        default:
            break;
    }
    AudioStandard::AudioSystemManager *audioSystemMgr = AudioStandard::AudioSystemManager::GetInstance();
    int32_t ret = audioSystemMgr->GetVolume(volumeType);
    std::cout << "return value:" << ret << std::endl;
}

void SetVolume()
{
    int32_t volume = DEFAULT_VALUE;
    int32_t type = DEFAULT_VALUE;
    std::cout << "------SetVolume------" << std::endl;
    std::cout << "please input volume value(0~15) :" << std::endl;
    std::cin >> volume;
    std::cout << "please input volume type(3:ring 4:music)" << std::endl;
    std::cin >> type;
    if (volume < MIN_VOLUME || volume > MAX_VOLUME) {
        std::cout << "volume value error" << std::endl;
        return;
    }
    AudioStandard::AudioVolumeType volumeType =
        AudioStandard::AudioVolumeType::STREAM_MUSIC;
    switch (type) {
        case AudioStandard::AudioVolumeType::STREAM_RING:
            volumeType = AudioStandard::AudioVolumeType::STREAM_RING;
            break;
        case AudioStandard::AudioVolumeType::STREAM_MUSIC:
            volumeType = AudioStandard::AudioVolumeType::STREAM_MUSIC;
            break;
        default:
            break;
    }
    AudioStandard::AudioSystemManager *audioSystemMgr = AudioStandard::AudioSystemManager::GetInstance();
    int32_t ret = audioSystemMgr->SetVolume(volumeType, volume);
    std::cout << "return value:" << ret << std::endl;
}

bool InitRenderer(const std::unique_ptr<AudioStandard::AudioRenderer> &audioRenderer, const wav_hdr &wavHeader)
{
    if (audioRenderer == nullptr) {
        std::cout << "audioRenderer is null" << std::endl;
        return false;
    }
    AudioStandard::AudioRendererParams rendererParams;
    rendererParams.sampleFormat = AudioStandard::AudioSampleFormat::SAMPLE_S16LE;
    rendererParams.sampleRate = static_cast<AudioStandard::AudioSamplingRate>(wavHeader.SamplesPerSec);
    rendererParams.channelCount = static_cast<AudioStandard::AudioChannel>(wavHeader.NumOfChan);
    rendererParams.encodingType = static_cast<AudioStandard::AudioEncodingType>(AudioStandard::ENCODING_PCM);
    if (audioRenderer->SetParams(rendererParams) != TELEPHONY_SUCCESS) {
        std::cout << "audio renderer set params error" << std::endl;
        if (!audioRenderer->Release()) {
            std::cout << "audio renderer release error" << std::endl;
        }
        return false;
    }
    if (!audioRenderer->Start()) {
        std::cout << "audio renderer start error" << std::endl;
        return false;
    }
    uint32_t frameCount;
    if (audioRenderer->GetFrameCount(frameCount)) {
        return false;
    }
    std::cout << "frame count : " << frameCount << std::endl;
    return true;
}

bool PlayRingtone()
{
    wav_hdr wavHeader;
    std::cout << "please input ringtone file path : " << std::endl;
    char path[RING_PATH_MAX_LENGTH];
    std::cin >> path;
    FILE *wavFile = fopen(path, "rb");
    if (wavFile == nullptr) {
        std::cout << "wav file nullptr" << std::endl;
        return false;
    }
    (void)fread(&wavHeader, READ_SIZE, sizeof(wav_hdr), wavFile);
    std::unique_ptr<AudioStandard::AudioRenderer> audioRenderer =
        AudioStandard::AudioRenderer::Create(AudioStandard::AudioStreamType::STREAM_MUSIC);
    if (!InitRenderer(audioRenderer, wavHeader)) {
        (void)fclose(wavFile);
        return false;
    }
    size_t bufferLen, bytesToWrite = DEFAULT_SIZE, bytesWritten = DEFAULT_SIZE;
    if (audioRenderer->GetBufferSize(bufferLen)) {
        (void)fclose(wavFile);
        return false;
    }
    std::unique_ptr<uint8_t> buffer = std::make_unique<uint8_t>(bufferLen + bufferLen);
    if (buffer == nullptr) {
        std::cout << "malloc memory nullptr" << std::endl;
        (void)fclose(wavFile);
        return false;
    }
    while (!feof(wavFile)) {
        bytesToWrite = fread(buffer.get(), READ_SIZE, bufferLen, wavFile);
        bytesWritten = DEFAULT_SIZE;
        while ((bytesWritten < bytesToWrite) && ((bytesToWrite - bytesWritten) > MIN_BYTES)) {
            bytesWritten += audioRenderer->Write(buffer.get() + bytesWritten, bytesToWrite - bytesWritten);
        }
    }
    audioRenderer->Flush();
    audioRenderer->Drain();
    audioRenderer->Stop();
    audioRenderer->Release();
    (void)fclose(wavFile);
    std::cout << "audio renderer plackback done" << std::endl;
    return true;
}

void ControlCamera()
{
    std::cout << "------ControlCamera test------" << std::endl;
    std::string tmpStr = "";
    sptr<CameraStandard::CameraManager> camManagerObj = CameraStandard::CameraManager::GetInstance();
    std::vector<sptr<CameraStandard::CameraDevice>> cameraObjList = camManagerObj->GetSupportedCameras();

    for (auto &it : cameraObjList) {
        tmpStr = it->GetID();
        std::cout << "camManagerObj->GetCameras Camera ID:" << tmpStr.c_str() << std::endl;
        if (!tmpStr.empty()) {
            break;
        }
    }

    std::u16string CameraID;
    CameraID.clear();
    CameraID = Str8ToStr16(tmpStr);
    int32_t ret = g_clientPtr->ControlCamera(CameraID);
    std::cout << "ok return value:" << ret << std::endl;

    std::cout << "ControlCamera done" << std::endl;
}

void SetPreviewWindow()
{
    std::cout << "------SetPreviewWindow test------" << std::endl;
    VideoWindow window;
    window.x = WINDOWS_X_START;
    window.y = WINDOWS_Y_START;
    window.z = WINDOWS_Z_ERROR;
    window.width = WINDOWS_WIDTH;
    window.height = WINDOWS_HEIGHT;
    int32_t ret = g_clientPtr->SetPreviewWindow(window);
    std::cout << "error return value:" << ret << std::endl;

    window.z = WINDOWS_Z_BOTTOM;
    ret = g_clientPtr->SetPreviewWindow(window);
    std::cout << "return value:" << ret << std::endl;

    window.z = WINDOWS_Z_TOP;
    ret = g_clientPtr->SetPreviewWindow(window);
    std::cout << "return value:" << ret << std::endl;

    std::cout << "SetPreviewWindow done" << std::endl;
}

void SetDisplayWindow()
{
    std::cout << "------SetDisplayWindow test------" << std::endl;
    VideoWindow window;
    window.x = WINDOWS_X_START;
    window.y = WINDOWS_Y_START;
    window.z = WINDOWS_WIDTH;
    window.width = WINDOWS_WIDTH;
    window.height = WINDOWS_HEIGHT;
    int32_t ret = g_clientPtr->SetDisplayWindow(window);
    std::cout << "error return value:" << ret << std::endl;

    window.z = WINDOWS_Z_TOP;
    ret = g_clientPtr->SetDisplayWindow(window);
    std::cout << "ok return value:" << ret << std::endl;

    window.z = WINDOWS_Z_BOTTOM;
    ret = g_clientPtr->SetDisplayWindow(window);
    std::cout << "ok return value:" << ret << std::endl;

    std::cout << "SetDisplayWindow done" << std::endl;
}

void SetCameraZoom()
{
    const float CameraZoomMax = 12.0;
    const float CameraZoomMin = -0.1;
    const float CameraZoom = 2.0;
    std::cout << "------SetCameraZoom test------" << std::endl;
    int32_t ret = g_clientPtr->SetCameraZoom(CameraZoomMax);
    std::cout << "return value:" << ret << std::endl;

    ret = g_clientPtr->SetCameraZoom(CameraZoomMin);
    std::cout << "return value:" << ret << std::endl;

    ret = g_clientPtr->SetCameraZoom(CameraZoom);
    std::cout << "return value:" << ret << std::endl;
    std::cout << "SetCameraZoom done" << std::endl;
}

void SetPausePicture()
{
    std::cout << "------SetPausePicture test------" << std::endl;
    std::u16string path;
    std::string tmpStr = "/system/bin/1.png";
    path.clear();
    path = Str8ToStr16(tmpStr);
    int32_t ret = g_clientPtr->SetPausePicture(path);
    std::cout << "\n return value:" << ret << std::endl;
    std::cout << "SetPausePicture done" << std::endl;
}

void SetDeviceDirection()
{
    const int32_t DeviceDirectionError1 = 50;
    const int32_t DeviceDirectionError2 = 350;
    const int32_t DeviceDirection90 = 90;
    std::cout << "------SetDeviceDirection test------" << std::endl;
    int32_t ret = g_clientPtr->SetDeviceDirection(DeviceDirectionError1);
    std::cout << "\n return value:" << ret << std::endl;

    ret = g_clientPtr->SetDeviceDirection(DeviceDirectionError2);
    std::cout << "\n return value:" << ret << std::endl;

    ret = g_clientPtr->SetDeviceDirection(DeviceDirection90);
    std::cout << "\n return value:" << ret << std::endl;
    std::cout << "SetDeviceDirection done" << std::endl;
}

void SubscribeCommonEvent()
{
    std::cout << "------SubscribeCommonEvent------" << std::endl;
    std::cout << "please input common event type : " << std::endl;
    char eventType[RING_PATH_MAX_LENGTH];
    std::cin >> eventType;
    OHOS::EventFwk::MatchingSkills matchingSkills;
    std::string event(eventType);
    matchingSkills.AddEvent(event);
    // make subcriber info
    OHOS::EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    // make a subcriber object
    std::shared_ptr<CommonEventSubscriberTest> subscriberTest =
        std::make_shared<CommonEventSubscriberTest>(subscriberInfo);
    if (subscriberTest == nullptr) {
        std::cout << "subscriber nullptr" << std::endl;
    }
    // subscribe a common event
    bool result = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberTest);
    std::cout << "subscribe common event : " << eventType << ", result : " << result << std::endl;
}

void GetImsConfig()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t item = DEFAULT_ITEM_VALUE;
    std::cout << "------GetImsConfig------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input item:" << std::endl;
    std::cin >> item;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->GetImsConfig(slotId, static_cast<ImsConfigItem>(item));
    std::cout << "return value:" << ret << std::endl;
}

void SetImsConfig()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t item = DEFAULT_ITEM_VALUE;
    std::string tmpValue;
    std::u16string value;
    std::cout << "------SetImsConfig------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input item:" << std::endl;
    std::cin >> item;
    std::cout << "please input item value:" << std::endl;
    std::cin >> tmpValue;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    value = Str8ToStr16(tmpValue);
    ret = g_clientPtr->SetImsConfig(slotId, static_cast<ImsConfigItem>(item), value);
    std::cout << "return value:" << ret << std::endl;
}

void GetImsFeatureValue()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = FeatureType::TYPE_VOICE_OVER_LTE;
    FeatureType type = FeatureType::TYPE_VOICE_OVER_LTE;
    std::cout << "------GetImsFeatureValue------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input feature type:" << std::endl;
    std::cin >> tmpType;
    type = static_cast<FeatureType>(tmpType);
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->GetImsFeatureValue(slotId, type);
    std::cout << "return value:" << ret << std::endl;
}

void SetImsFeatureValue()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = FeatureType::TYPE_VOICE_OVER_LTE;
    FeatureType type;
    int32_t value = DEFAULT_NET_TYPE;
    std::cout << "------SetImsNetworkValue------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input feature type:" << std::endl;
    std::cin >> tmpType;
    type = (FeatureType)tmpType;
    std::cout << "please input value:" << std::endl;
    std::cin >> value;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->SetImsFeatureValue(slotId, type, value);
    std::cout << "return value:" << ret << std::endl;
}

void UpdateImsCallMode()
{
    int32_t callId = DEFAULT_CALL_ID;
    uint32_t mediaMode = DEFAULT_VIDEO_STATE;
    std::cout << "------UpdateImsCallMode------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::cout << "please input media mode[0:voice, 3:video]:" << std::endl;
    std::cin >> mediaMode;
    ImsCallMode mode = static_cast<ImsCallMode>(mediaMode);
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->UpdateImsCallMode(callId, mode);
    std::cout << "return value:" << ret << std::endl;
}

void EnableImsSwitch()
{
    AccessToken token;
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------EnableImsSwitch------" << std::endl;
    std::cout << "please input slot id:" << std::endl;
    std::cin >> slotId;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->EnableImsSwitch(slotId);
    std::cout << "return value:" << ret << std::endl;
}

void DisableImsSwitch()
{
    AccessToken token;
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------DisableImsSwitch------" << std::endl;
    std::cout << "please input slot id:" << std::endl;
    std::cin >> slotId;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->DisableImsSwitch(slotId);
    std::cout << "return value:" << ret << std::endl;
}

void IsImsSwitchEnabled()
{
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------IsImsSwitchEnabled------" << std::endl;
    std::cout << "please input slot id:" << std::endl;
    std::cin >> slotId;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    bool enabled;
    ret = g_clientPtr->IsImsSwitchEnabled(slotId, enabled);
    std::cout << "enabled:" << enabled << std::endl;
    std::cout << "return value:" << ret << std::endl;
}

void StartRtt()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::u16string msg;
    std::string tmpMsg;
    std::cout << "------StartRtt------" << std::endl;
    std::cout << "please input call id:" << std::endl;
    std::cin >> callId;
    std::cout << "please input Rtt msg:" << std::endl;
    msg.clear();
    tmpMsg.clear();
    std::cin >> tmpMsg;
    msg = Str8ToStr16(tmpMsg);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->StartRtt(callId, msg);
    std::cout << "return value:" << ret << std::endl;
}

void StopRtt()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------StopRtt------" << std::endl;
    std::cout << "please input call id:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->StopRtt(callId);
    std::cout << "return value:" << ret << std::endl;
}

void CancelMissedIncomingCallNotification()
{
    AccessToken token;
    std::cout << "------CancelMissedIncomingCallNotification------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->CancelMissedIncomingCallNotification();
    std::cout << "return value:" << ret << std::endl;
}

void AddPermission()
{
    return;
}

void InitPermission()
{
    return;
}

void VerifyPermission()
{
    return;
}

void InitCallBasicPower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_DIAL_CALL] = &OHOS::Telephony::DialCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_ANSWER_CALL] = &OHOS::Telephony::AnswerCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_REJECT_CALL] = &OHOS::Telephony::RejectCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_HOLD_CALL] = &OHOS::Telephony::HoldCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_UNHOLD_CALL] = &OHOS::Telephony::UnHoldCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_DISCONNECT_CALL] = &OHOS::Telephony::HangUpCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_CALL_STATE] = &OHOS::Telephony::GetCallState;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SWAP_CALL] = &OHOS::Telephony::SwitchCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_START_RTT] = &OHOS::Telephony::StartRtt;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_STOP_RTT] = &OHOS::Telephony::StopRtt;
}

void InitCallUtils()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_HAS_CALL] = &OHOS::Telephony::HasCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_NEW_CALL_ALLOWED] = &OHOS::Telephony::IsNewCallAllowed;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_RINGING] = &OHOS::Telephony::IsRinging;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_EMERGENCY_CALL] = &OHOS::Telephony::IsInEmergencyCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_EMERGENCY_NUMBER] = &OHOS::Telephony::IsEmergencyPhoneNumber;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_FORMAT_NUMBER] = &OHOS::Telephony::FormatPhoneNumber;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_FORMAT_NUMBER_E164] = &OHOS::Telephony::FormatPhoneNumberToE164;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_CANCEL_MISSED_INCOMING_CALL_NOTIFICATION] =
        &OHOS::Telephony::CancelMissedIncomingCallNotification;
}

void InitCallConferencePower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_COMBINE_CONFERENCE] = &OHOS::Telephony::CombineConference;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SEPARATE_CONFERENCE] = &OHOS::Telephony::SeparateConference;
}

void InitCallDtmfPower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_START_DTMF] = &OHOS::Telephony::StartDtmf;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_STOP_DTMF] = &OHOS::Telephony::StopDtmf;
}

void InitCallSupplementPower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_CALL_WAITING] = &OHOS::Telephony::GetCallWaiting;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_CALL_WAITING] = &OHOS::Telephony::SetCallWaiting;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_CALL_RESTRICTION] = &OHOS::Telephony::GetCallRestriction;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_CALL_RESTRICTION] = &OHOS::Telephony::SetCallRestriction;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_CALL_TRANSFER] = &OHOS::Telephony::GetCallTransferInfo;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_CALL_TRANSFER] = &OHOS::Telephony::SetCallTransferInfo;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_CAN_SET_CALL_TRANSFER_TIME] = &OHOS::Telephony::CanSetCallTransferTime;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SETCALL_PREFERENCEMODE] = &OHOS::Telephony::SetCallPreferenceMode;
}

void initCallConferenceExPower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_MAINID] = &OHOS::Telephony::GetMainCallId;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_SUBCALL_LIST_ID] = &OHOS::Telephony::GetSubCallIdList;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_CALL_LIST_ID_FOR_CONFERENCE] =
        &OHOS::Telephony::GetCallIdListForConference;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_JOIN_CONFERENCE] = &OHOS::Telephony::InviteToConference;
}

void InitCallMultimediaPower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_MUTE] = &OHOS::Telephony::SetMute;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_MUTE_RINGER] = &OHOS::Telephony::MuteRinger;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_AUDIO_DEVICE] = &OHOS::Telephony::SetAudioDevice;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_CTRL_CAMERA] = &OHOS::Telephony::ControlCamera;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_PREVIEW_WINDOW] = &OHOS::Telephony::SetPreviewWindow;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_DISPLAY_WINDOW] = &OHOS::Telephony::SetDisplayWindow;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_CAMERA_ZOOM] = &OHOS::Telephony::SetCameraZoom;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_PAUSE_IMAGE] = &OHOS::Telephony::SetPausePicture;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_DEVICE_DIRECTION] = &OHOS::Telephony::SetDeviceDirection;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_UPDATE_CALL_MEDIA_MODE] = &OHOS::Telephony::UpdateImsCallMode;

    g_memberFuncMap[DEFINE_INIT_PERMISSIONS] = &OHOS::Telephony::InitPermission;
    g_memberFuncMap[DEFINE_VERIFY_PERMISSIONS] = &OHOS::Telephony::VerifyPermission;
}

void InitImsServicePower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_IMS_CONFIG] = &OHOS::Telephony::GetImsConfig;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_IMS_CONFIG] = &OHOS::Telephony::SetImsConfig;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_IMS_FEATURE_VALUE] = &OHOS::Telephony::GetImsFeatureValue;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_IMS_FEATURE_VALUE] = &OHOS::Telephony::SetImsFeatureValue;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_ENABLE_VOLTE] = &OHOS::Telephony::EnableImsSwitch;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_DISABLE_VOLTE] = &OHOS::Telephony::DisableImsSwitch;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_VOLTE_ENABLED] = &OHOS::Telephony::IsImsSwitchEnabled;
}

int32_t Init()
{
    AccessToken token;
    g_clientPtr = DelayedSingleton<CallManagerClient>::GetInstance();
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return TELEPHONY_ERROR;
    }
    g_clientPtr->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    std::unique_ptr<CallManagerCallbackTest> callbackPtr = std::make_unique<CallManagerCallbackTest>();
    if (callbackPtr == nullptr) {
        std::cout << "make_unique CallManagerCallbackTest failed!" << std::endl;
        return TELEPHONY_ERROR;
    }
    int32_t ret = g_clientPtr->RegisterCallBack(std::move(callbackPtr));
    if (ret != TELEPHONY_SUCCESS) {
        std::cout << "RegisterCallBack failed!" << std::endl;
        return TELEPHONY_ERROR;
    }
    std::cout << "RegisterCallBack success!" << std::endl;
    InitCallBasicPower();
    InitCallUtils();
    InitCallConferencePower();
    InitCallDtmfPower();
    InitCallSupplementPower();
    initCallConferenceExPower();
    InitCallMultimediaPower();
    InitImsServicePower();
    return TELEPHONY_SUCCESS;
}

void PrintfCallBasisInterface()
{
    std::cout << "\n\n-----------start--------------\n"
              << "usage:please input a cmd num:\n"
              << "2:dial\n"
              << "3:answer\n"
              << "4:reject\n"
              << "5:hold\n"
              << "6:unhold\n"
              << "7:hangUpCall\n"
              << "8:getCallState\n"
              << "9:switchCall\n";
}

void PrintfCallUtilsInterface()
{
    std::cout << "10:hasCall\n"
              << "11:isNewCallAllowed\n"
              << "12:isRinging\n"
              << "13:isInEmergencyCall\n"
              << "14:isEmergencyPhoneNumber\n"
              << "15:formatPhoneNumber\n"
              << "16:formatPhoneNumberToE164\n"
              << "58:CancelMissedIncomingCallNotification\n";
}

void PrintfCallConferenceInterface()
{
    std::cout << "17:combine conference\n"
              << "18:separate conference\n";
}

void PrintfCallDtmfInterface()
{
    std::cout << "19:StartDtmf\n"
              << "20:StopDtmf\n";
}

void PrintfCallSupplementInterface()
{
    std::cout << "21:getCallWaiting\n"
              << "22:setCallWaiting\n"
              << "23:getCallRestriction\n"
              << "24:setCallRestriction\n"
              << "25:getCallTransferInfo\n"
              << "26:setCallTransferInfo\n"
              << "27:CanSetCallTransferTime\n";
}

void PrintfCallConferenceExInterface()
{
    std::cout << "28:GetMainCallId\n"
              << "29:GetSubCallIdList\n"
              << "30:GetCallIdListForConference\n";
}

void PrintfCallMultimediaInterface()
{
    std::cout << "31:SetMute\n"
              << "32:MuteRinger\n"
              << "33:SetAudioDevice\n"
              << "34:ControlCamera\n"
              << "35:SetPreviewWindow\n"
              << "36:SetDisplayWindow\n"
              << "37:SetCameraZoom\n"
              << "38:SetPausePicture\n"
              << "39:SetDeviceDirection\n"
              << "40:SetCallPreferenceMode\n"
              << "41:GetImsConfig\n"
              << "42:SetImsConfig\n"
              << "43:GetImsNetworkValue\n"
              << "44:SetImsNetworkValue\n"
              << "45:UpdateImsCallMode\n"
              << "46:EnableImsSwitch\n"
              << "47:DisableImsSwitch\n"
              << "48:IsImsSwitchEnabled\n"
              << "49:StartRtt\n"
              << "50:StopRtt\n"
              << "93:InitPermission\n"
              << "94:VerifyPermission\n"
              << "95:SendConnectBluetoothScoBroadcast\n"
              << "96:SendDisconnectBluetoothScoBroadcast\n"
              << "97:SubscribeCommonEvent\n"
              << "98:GetVolume\n"
              << "99:SetVolume\n"
              << "100:PlayRintone\n";
}

void PrintfUsage()
{
    PrintfCallBasisInterface();
    PrintfCallUtilsInterface();
    PrintfCallConferenceInterface();
    PrintfCallDtmfInterface();
    PrintfCallSupplementInterface();
    PrintfCallConferenceExInterface();
    PrintfCallMultimediaInterface();
    std::cout << "1000:exit\n";
}

int32_t mainExit()
{
    if (OHOS::Telephony::g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return OHOS::Telephony::TELEPHONY_ERR_FAIL;
    }
    OHOS::Telephony::g_memberFuncMap.clear();
    OHOS::Telephony::g_clientPtr->UnInit();
    std::cout << "exit success" << std::endl;
    return OHOS::Telephony::TELEPHONY_SUCCESS;
}

int32_t RunTest()
{
    std::cout << "callManager test start...." << std::endl;
    int32_t interfaceNum = DEFAULT_VALUE;
    const int32_t exitNumber = 1000;
    const int32_t getVolumeNumber = 98;
    const int32_t setVolumeNumber = 99;
    const int32_t playRingtoneNumber = 100;
    if (Init() != TELEPHONY_SUCCESS) {
        std::cout << "callManager test init failed!" << std::endl;
        return TELEPHONY_SUCCESS;
    }
    while (true) {
        PrintfUsage();
        std::cin >> interfaceNum;
        if (interfaceNum == exitNumber) {
            std::cout << "start to exit now...." << std::endl;
            break;
        } else if (interfaceNum == playRingtoneNumber) {
            PlayRingtone();
            continue;
        } else if (interfaceNum == setVolumeNumber) {
            SetVolume();
            continue;
        } else if (interfaceNum == getVolumeNumber) {
            GetVolume();
            continue;
        }
        auto itFunc = g_memberFuncMap.find(interfaceNum);
        if (itFunc != g_memberFuncMap.end() && itFunc->second != nullptr) {
            auto memberFunc = itFunc->second;
            (*memberFunc)();
            continue;
        }
        std::cout << "err: invalid input!" << std::endl;
    }
    return mainExit();
}
} // namespace Telephony
} // namespace OHOS

int32_t main()
{
    int32_t code = OHOS::Telephony::DEFAULT_VALUE;
    const int32_t exitCode = 1000;
    std::cout << "Please select interface type...." << std::endl;
    while (true) {
        std::cout << "1:callManager interface\n"
            << "2:bluetooth call interface\n"
            << "1000:exit\n";
        std::cin >> code;
        switch (code) {
            case static_cast<int32_t>(OHOS::Telephony::CallManagerInterfaceType::INTERFACE_CALL_MANAGER_TYPE):
                OHOS::Telephony::RunTest();
                OHOS::Telephony::mainExit();
                break;
            case static_cast<int32_t>(OHOS::Telephony::CallManagerInterfaceType::INTERFACE_BLUETOOTH_CALL_TYPE): {
                std::unique_ptr<OHOS::Telephony::BluetoothCallTest> testPtr =
                    std::make_unique<OHOS::Telephony::BluetoothCallTest>();
                if (testPtr != nullptr) {
                    testPtr->Init();
                    testPtr->RunBluetoothCallTest();
                }
                break;
            }
            default:
                std::cout << "invalid input" << std::endl;
                break;
        }
        if (code == exitCode) {
            break;
        }
    }
    return OHOS::Telephony::TELEPHONY_SUCCESS;
}
