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
