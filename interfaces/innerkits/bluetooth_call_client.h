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

#ifndef BLUETOOTH_CALL_CLIENT_H
#define BLUETOOTH_CALL_CLIENT_H

#include "singleton.h"
#include "pac_map.h"

#include "call_manager_callback.h"

namespace OHOS {
namespace Telephony {
class BluetoothCallClient : public std::enable_shared_from_this<BluetoothCallClient> {
    DECLARE_DELAYED_SINGLETON(BluetoothCallClient)
public:
    void Init();
    void UnInit();
    int32_t RegisterCallBack(std::unique_ptr<CallManagerCallback> callback);
    int32_t UnRegisterCallBack();
    int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras);
    int32_t AnswerCall();
    int32_t RejectCall();
    int32_t HangUpCall();
    int32_t GetCallState();
    int32_t HoldCall();
    int32_t UnHoldCall();
    int32_t SwitchCall();
    int32_t CombineConference();
    int32_t SeparateConference();
    int32_t StartDtmf(char str);
    int32_t StopDtmf();
    int32_t IsRinging(bool &enabled);
    bool HasCall();
    int32_t IsNewCallAllowed(bool &enabled);
    int32_t IsInEmergencyCall(bool &enabled);
    int32_t SetMuted(bool isMute);
    int32_t MuteRinger();
    int32_t SetAudioDevice(AudioDevice deviceType, const std::string &bluetoothAddress);
    std::vector<CallAttributeInfo> GetCurrentCallList(int32_t slotId);
};
} // namespace Telephony
} // namespace OHOS

#endif