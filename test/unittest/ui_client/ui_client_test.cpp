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

#include <securec.h>
#include <memory>
#include <iostream>
#include <string_ex.h>

#include "system_ability_definition.h"

#include "call_manager_errors.h"
#include "call_manager_ipc_client.h"
#include "call_manager_type.h"

namespace OHOS {
using namespace OHOS::TelephonyCallManager;

std::unique_ptr<CallManagerIpcClient> g_clientPtr = nullptr;

void DialCall()
{
    int32_t accountId = 0;
    int32_t videoState = 0;
    int32_t dialScene = 0;
    int32_t callId = 0;
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
    dialInfo.PutIntValue("accountId", accountId);
    dialInfo.PutIntValue("videoState", videoState);
    dialInfo.PutIntValue("dialScene", dialScene);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    g_clientPtr->DialCall(phoneNumber, dialInfo, callId);
    std::cout << "return callId:" << callId << std::endl;
}

void AcceptCall()
{
    int callId = 0;
    int videoState = 0;
    std::cout << "------Answer------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::cout << "please input videoState[0:audio,1:video]:" << std::endl;
    std::cin >> videoState;
    int ret = 0;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->AcceptCall(callId, videoState);
    std::cout << "return value:" << ret << std::endl;
}

void RejectCall()
{
    int callId = 0;
    int boolValue = 0;
    bool flag = false;
    std::u16string content;
    std::string tmpStr;
    content.clear();
    std::cout << "------Reject------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::cout << "Whether to enter the reason for rejection?[0:no,1:yes]:" << std::endl;
    std::cin >> boolValue;
    if (boolValue != 0) {
        flag = true;
        std::cout << "please input reject message:" << std::endl;
        tmpStr.clear();
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

void HangUpCall()
{
    int callId = 0;
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

void Init()
{
    g_clientPtr = std::make_unique<CallManagerIpcClient>();
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    if (g_clientPtr == nullptr) {
        printf("new CallManagerIpcClient failed!\n");
        return;
    }
    g_clientPtr->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
}

void PrintfUsage()
{
    printf(
        "\n-----------start--------------\n"
        "usage:please input a cmd num:\n"
        "0:dial\n"
        "1:answer\n"
        "2:reject\n"
        "5:hangUpCall\n"
        "32:getCallState\n"
        "1000:exit\n");
}
} // namespace OHOS

int main()
{
    printf("callManager test start....\n");
    int interfaceNum = 0;
    bool loopFlag = true;
    const int exitNum = 1000;
    OHOS::Init();

    while (loopFlag) {
        OHOS::PrintfUsage();
        std::cin >> interfaceNum;
        switch (interfaceNum) {
            case OHOS::INTERFACE_DIAL_CALL:
                OHOS::DialCall();
                break;
            case OHOS::INTERFACE_ANSWER_CALL:
                OHOS::AcceptCall();
                break;
            case OHOS::INTERFACE_REJECT_CALL:
                OHOS::RejectCall();
                break;
            case OHOS::INTERFACE_DISCONNECT_CALL:
                OHOS::HangUpCall();
                break;
            case OHOS::INTERFACE_GET_CALL_STATE:
                OHOS::GetCallState();
                break;
            case exitNum:
                loopFlag = false;
                std::cout << "exit..." << std::endl;
                break;
            default:
                std::cout << "please input correct cmd number..." << std::endl;
                break;
        }
    }
    if (OHOS::g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return -1;
    }
    OHOS::g_clientPtr->UnInit();
    return OHOS::TELEPHONY_NO_ERROR;
}