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
std::shared_ptr<BluetoothCallClient> g_bluetoothCallPtr = nullptr;

int32_t BluetoothCallTest::Init()
{
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
    std::cout << "\n\n-----------bluetooth call test start--------------\n"
              << "usage:please input a cmd num:\n"
              << "0:dial\n"
              << "1:answer\n"
              << "2:reject\n"
              << "3:hangup\n"
              << "4:getCallState\n"
              << "5:hold\n"
              << "6:unhold\n"
              << "7:switchCall\n"
              << "8:combineConference\n"
              << "9:separateConference\n"
              << "10:startDtmf\n"
              << "11:stopRtt\n"
              << "12:isRinging\n"
              << "13:hasCall\n"
              << "14:isNewCallAllowed\n"
              << "15:isInEmergencyCall\n"
              << "16:setMuted\n"
              << "17:muteRinger\n"
              << "18:setAudioDevice\n"
              << "19:GetCurrentCallList\n"
              << "1000:exit\n";
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
    int32_t ret = g_bluetoothCallPtr->IsNewCallAllowed();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::IsRinging()
{
    std::cout << "------IsRinging------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->IsRinging();
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::IsInEmergencyCall()
{
    std::cout << "------IsInEmergencyCall------" << std::endl;
    if (g_bluetoothCallPtr == nullptr) {
        std::cout << "g_bluetoothCallPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_bluetoothCallPtr->IsInEmergencyCall();
    std::cout << "return value:" << ret << std::endl;
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
    AudioDevice device = AudioDevice::DEVICE_UNKNOWN;
    device = static_cast<AudioDevice>(deviceType);
    int32_t ret = g_bluetoothCallPtr->SetAudioDevice(device);
    std::cout << "return value:" << ret << std::endl;
}

void BluetoothCallTest::GetCurrentCallList()
{
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
