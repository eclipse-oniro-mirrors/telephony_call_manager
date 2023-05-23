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

#include "bluetooth_call_test.h"

#include <iostream>

#include "call_manager_errors.h"

#include "bluetooth_call_client.h"
#include "call_manager_test_types.h"
#include "call_manager_callback_test.h"

namespace OHOS {
namespace Telephony {
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
std::shared_ptr<BluetoothCallClient> g_bluetoothCallPtr = nullptr;

HapInfoParams testInfoParamsCase = {
    .userID = 1,
    .bundleName = "tel_call_manager_ui_test",
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true,
};

PermissionDef testPermPlaceCallDefCase = {
    .permissionName = "ohos.permission.PLACE_CALL",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

PermissionStateFull testPlaceCallStateCase = {
    .permissionName = "ohos.permission.PLACE_CALL",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

PermissionDef testPermSetTelephonyStateDefCase = {
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

PermissionStateFull testSetTelephonyStateCase = {
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

PermissionDef testPermGetTelephonyStateDefCase = {
    .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

PermissionStateFull testGetTelephonyStateCase = {
    .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

PermissionDef testPermAnswerCallDefCase = {
    .permissionName = "ohos.permission.ANSWER_CALL",
    .bundleName = "tel_call_manager_ui_test",
    .grantMode = 1, // SYSTEM_GRANT
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test call maneger",
    .descriptionId = 1,
};

PermissionStateFull testAnswerCallStateCase = {
    .permissionName = "ohos.permission.ANSWER_CALL",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 }, // PERMISSION_USER_SET
};

HapPolicyParams testPolicyParamsCase = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testPermPlaceCallDefCase, testPermSetTelephonyStateDefCase, testPermGetTelephonyStateDefCase,
        testPermAnswerCallDefCase },
    .permStateList = { testPlaceCallStateCase, testSetTelephonyStateCase, testGetTelephonyStateCase,
        testAnswerCallStateCase },
};

class AccessToken {
public:
    AccessToken()
    {
        currentID_ = GetSelfTokenID();
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testInfoParamsCase, testPolicyParamsCase);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenIdEx.tokenIDEx);
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

int32_t BluetoothCallTest::Init()
{
    AccessToken token;
    g_bluetoothCallPtr = DelayedSingleton<BluetoothCallClient>::GetInstance();
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return TELEPHONY_ERROR;
    }
    g_bluetoothCallPtr->Init();
    std::unique_ptr<CallManagerCallbackTest> callbackPtr = std::make_unique<CallManagerCallbackTest>();
    if (callbackPtr == nullptr) {
        std::cout << "make_unique CallManagerCallbackTest failed!" << std::endl;
        return TELEPHONY_ERROR;
    }
    int32_t ret = g_bluetoothCallPtr->RegisterCallBack(std::move(callbackPtr));
    if (ret != TELEPHONY_SUCCESS) {
        std::cout << "RegisterCallBack failed!" << std::endl;
        return TELEPHONY_ERROR;
    }
    std::cout << "RegisterCallBack success!" << std::endl;
    InitFunMap();
    return TELEPHONY_SUCCESS;
}

void BluetoothCallTest::RunBluetoothCallTest()
{
    int32_t interfaceNum = OHOS::Telephony::DEFAULT_VALUE;
    const int32_t exitNumber = 1000;
    while (true) {
        PrintfUsage();
        std::cin >> interfaceNum;
        if (interfaceNum == exitNumber) {
            std::cout << "start to exit now...." << std::endl;
            break;
        }
        auto itFunc = memberFuncMap_.find(interfaceNum);
        if (itFunc != memberFuncMap_.end() && itFunc->second != nullptr) {
            auto BluetoothCallFunc = itFunc->second;
            (this->*BluetoothCallFunc)();
            continue;
        }
        std::cout << "err: invalid input!" << std::endl;
    }
    memberFuncMap_.clear();
}

void BluetoothCallTest::PrintfUsage()
{
    std::cout << "\n\n-----------bluetooth call test start--------------\n";
    std::cout << "usage:please input a cmd num:\n";
    std::cout << "0:dial\n";
    std::cout << "1:answer\n";
    std::cout << "2:reject\n";
    std::cout << "3:hangup\n";
    std::cout << "4:getCallState\n";
    std::cout << "5:hold\n";
    std::cout << "6:unhold\n";
    std::cout << "7:switchCall\n";
    std::cout << "8:combineConference\n";
    std::cout << "9:separateConference\n";
    std::cout << "10:startDtmf\n";
    std::cout << "11:stopRtt\n";
    std::cout << "12:isRinging\n";
    std::cout << "13:hasCall\n";
    std::cout << "14:isNewCallAllowed\n";
    std::cout << "15:isInEmergencyCall\n";
    std::cout << "16:setMuted\n";
    std::cout << "17:muteRinger\n";
    std::cout << "18:setAudioDevice\n";
    std::cout << "19:GetCurrentCallList\n";
    std::cout << "1000:exit\n";
}

void BluetoothCallTest::InitFunMap()
{
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_DIAL)] = &BluetoothCallTest::DialCall;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_ANSWER)] = &BluetoothCallTest::AnswerCall;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_REJECT)] = &BluetoothCallTest::RejectCall;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_HANGUP)] = &BluetoothCallTest::HangUpCall;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_GET_CALL_STATE)] =
        &BluetoothCallTest::GetCallState;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_HOLD)] = &BluetoothCallTest::HoldCall;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_UNHOLD)] = &BluetoothCallTest::UnHoldCall;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_SWITCH)] = &BluetoothCallTest::SwitchCall;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_COMBINE_CONFERENCE)] =
        &BluetoothCallTest::CombineConference;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_SEPARATE_CONFERENCE)] =
        &BluetoothCallTest::SeparateConference;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_START_DTMF)] = &BluetoothCallTest::StartDtmf;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_STOP_DTMF)] = &BluetoothCallTest::StopDtmf;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_IS_RINGING)] = &BluetoothCallTest::IsRinging;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_HAS_CALL)] = &BluetoothCallTest::HasCall;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_IS_NEW_CALL_ALLOWED)] =
        &BluetoothCallTest::IsNewCallAllowed;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_IS_IN_EMERGENCY_CALL)] =
        &BluetoothCallTest::IsInEmergencyCall;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_SET_MUTED)] = &BluetoothCallTest::SetMute;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_MUTE_RINGER)] = &BluetoothCallTest::MuteRinger;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_SET_AUDIO_DEVICE)] =
        &BluetoothCallTest::SetAudioDevice;
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallFuncCode::TEST_GET_CURRENT_CALL_LIST)] =
        &BluetoothCallTest::GetCurrentCallList;
}

void BluetoothCallTest::DialCall()
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
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->DialCall(phoneNumber, dialInfo);
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::AnswerCall()
{
    AccessToken token;
    std::cout << "------Answer------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->AnswerCall();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::RejectCall()
{
    AccessToken token;
    std::cout << "------Reject------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->RejectCall();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::HangUpCall()
{
    AccessToken token;
    std::cout << "------HangUpCall------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->HangUpCall();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::GetCallState()
{
    std::cout << "------GetCallState------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->GetCallState();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::HoldCall()
{
    AccessToken token;
    std::cout << "------HoldCall------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->HoldCall();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::UnHoldCall()
{
    AccessToken token;
    std::cout << "------UnHoldCall------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->UnHoldCall();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::CombineConference()
{
    std::cout << "------CombineConference------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->CombineConference();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::SeparateConference()
{
    std::cout << "------SeparateConference------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->SeparateConference();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::SwitchCall()
{
    AccessToken token;
    std::cout << "------SwitchCall------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->SwitchCall();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::HasCall()
{
    std::cout << "------HasCall------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->HasCall();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::IsNewCallAllowed()
{
    std::cout << "------IsNewCallAllowed------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    bool enabled = false;
    int32_t ret = g_bluetoothCallPtr->IsNewCallAllowed(enabled);
    std::cout << "ret:" << ret << std::endl;
    std::cout << "enabled value:" << enabled << std::endl;
}

void BluetoothCallTest::IsRinging()
{
    AccessToken token;
    std::cout << "------IsRinging------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    bool enabled = false;
    int32_t ret = g_bluetoothCallPtr->IsRinging(enabled);
    std::cout << "ret:" << ret << std::endl;
    std::cout << "enabled value:" << enabled << std::endl;
}

void BluetoothCallTest::IsInEmergencyCall()
{
    AccessToken token;
    std::cout << "------IsInEmergencyCall------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    bool enabled = false;
    int32_t ret = g_bluetoothCallPtr->IsInEmergencyCall(enabled);
    std::cout << "ret:" << ret << std::endl;
    std::cout << "enabled value:" << enabled << std::endl;
}

void BluetoothCallTest::StartDtmf()
{
    char c = DEFAULT_VALUE;
    std::cout << "------StartDtmf------" << std::endl;
    std::cout << "Please enter to send dtmf characters:" << std::endl;
    std::cin >> c;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->StartDtmf(c);
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::StopDtmf()
{
    std::cout << "------StopDtmf------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->StopDtmf();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::SetMute()
{
    int32_t isMute = DEFAULT_VALUE;
    std::cout << "------SetMute------" << std::endl;
    std::cout << "please input mute state(0:false 1:true):" << std::endl;
    std::cin >> isMute;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->SetMuted((isMute == 1) ? true : false);
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::MuteRinger()
{
    AccessToken token;
    std::cout << "------MuteRinger------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->MuteRinger();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::SetAudioDevice()
{
    int32_t deviceType = DEFAULT_VALUE;
    std::cout << "------SetAudioDevice------" << std::endl;
    std::cout << "please input device type(0:earpiece 1:speaker 2:wired headset 3:bluetooth sco):" << std::endl;
    std::cin >> deviceType;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    AudioDeviceType device = static_cast<AudioDeviceType>(deviceType);
    std::string address = "0C:D7:46:14:AA:33";
    int32_t ret = g_bluetoothCallPtr->SetAudioDevice(device, address);
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::GetCurrentCallList()
{
    AccessToken token;
    int32_t slotId = DEFAULT_VALUE;
    std::cout << "------GetCurrentCallList------" << std::endl;
    std::cout << "Please enter slotId:" << std::endl;
    std::cin >> slotId;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    std::vector<CallAttributeInfo> callVec = g_bluetoothCallPtr->GetCurrentCallList(slotId);
    std::cout << "call list count:" << callVec.size() << std::endl;
    std::vector<CallAttributeInfo>::iterator it = callVec.begin();
    for (; it != callVec.end(); ++it) {
        std::cout << "number:" << (*it).accountNumber << ", callState:"
            << static_cast<int32_t>((*it).callState) << std::endl;
    }
}
} // namespace Telephony
} // namespace OHOS
