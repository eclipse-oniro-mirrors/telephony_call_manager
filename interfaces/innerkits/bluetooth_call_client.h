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
class BluetoothCallClient : public DelayedRefSingleton<BluetoothCallClient> {
    DECLARE_DELAYED_REF_SINGLETON(BluetoothCallClient)
public:
    void Init();
    void UnInit();

    /**
     * @brief Register callback
     *
     * @param callback[in], callback function pointer
     * @return Returns 0 on success, others on failure.
     */
    int32_t RegisterCallBack(std::unique_ptr<CallManagerCallback> callback);

    /**
     * @brief unregister callback
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t UnRegisterCallBack();

    /**
     * @brief Make a phone call
     *
     * @param number[in], dial param.
     * @param extras[in], extras date.
     * @return Returns callId when the value is greater than zero, others on failure.
     */
    int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras);

    /**
     * @brief Answer a phone call
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t AnswerCall();

    /**
     * @brief Reject a phone call
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t RejectCall();

    /**
     * @brief Hang up the phone
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t HangUpCall();

    /**
     * @brief Obtain the call status of the device
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetCallState();

    /**
     * @brief Park a phone call
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t HoldCall();

    /**
     * @brief Activate a phone call
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t UnHoldCall();

    /**
     * @brief Switch the phone
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t SwitchCall();

    /**
     * @brief Merge calls to form a conference
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t CombineConference();

    /**
     * @brief Separates a specified call from a conference call
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t SeparateConference();

    /**
     * @brief Kick out a specified call from a conference call
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t KickOutFromConference();

    /**
     * @brief Enable and send DTMF
     *
     * @param str[in], Characters sent
     * @return Returns 0 on success, others on failure.
     */
    int32_t StartDtmf(char str);

    /**
     * @brief Stop the DTMF
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t StopDtmf();

    /**
     * @brief Whether the ringing
     *
     * @param enabled[out], true on ringing, false on there is no ringing
     * @return Returns interface processing results.
     */
    int32_t IsRinging(bool &enabled);

    /**
     * @brief Is there Call
     *
     * @return Returns true on has call, others on there is no call.
     */
    bool HasCall();

    /**
     * @brief Checks whether a new call is allowed.
     *
     * @param enabled[out], whether allow new calls
     * @return Returns interface processing results.
     */
    int32_t IsNewCallAllowed(bool &enabled);

    /**
     * @brief Is there an emergency call
     *
     * @param enabled[out], true on emergency call, false on no emergency call
     * @return Returns interface processing results.
     */
    int32_t IsInEmergencyCall(bool &enabled);

    /**
     * @brief Mute the Microphone
     *
     * @param isMute[in], mute state
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetMuted(bool isMute);

    /**
     * @brief Call mute
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t MuteRinger();

    /**
     * @brief Setting the Audio Channel
     *
     * @param deviceType[in], audioDeviceType
     * @param bluetoothAddress[in], audio device address
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetAudioDevice(AudioDeviceType deviceType, const std::string &bluetoothAddress);

    /**
     * @brief Get current call list
     *
     * @param slotId[in], The slot id
     * @return Returns call info list.
     */
    std::vector<CallAttributeInfo> GetCurrentCallList(int32_t slotId);
private:
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS

#endif