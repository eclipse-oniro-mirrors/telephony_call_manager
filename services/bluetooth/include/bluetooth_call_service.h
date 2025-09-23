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

#ifndef BLUETOOTH_CALL_SERVICE_H
#define BLUETOOTH_CALL_SERVICE_H

#include <memory>

#include "bluetooth_call_policy.h"
#include "bluetooth_call_stub.h"
#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
class BluetoothCallService : public BluetoothCallStub, public BluetoothCallPolicy {
public:
    BluetoothCallService();
    ~BluetoothCallService();
    /**
     * AnswerCall
     *
     * @brief Answer a phone call
     * @return Returns 0 on success, others on failure.
     */
    int32_t AnswerCall() override;

    /**
     * RejectCall
     *
     * @brief Reject a phone call
     * @return Returns 0 on success, others on failure.
     */
    int32_t RejectCall() override;

    /**
     * HangUpCall
     *
     * @brief Hang up the phone
     * @return Returns 0 on success, others on failure.
     */
    int32_t HangUpCall() override;

    /**
     * GetCallState
     *
     * @brief Obtain the call status of the device
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetCallState() override;

    /**
     * HoldCall
     *
     * @brief Park a phone call
     * @return Returns 0 on success, others on failure.
     */
    int32_t HoldCall() override;

    /**
     * UnHoldCall
     *
     * @brief Activate a phone call
     * @return Returns 0 on success, others on failure.
     */
    int32_t UnHoldCall() override;

    /**
     * SwitchCall
     *
     * @brief Switch the phone
     * @return Returns 0 on success, others on failure.
     */
    int32_t SwitchCall() override;

    /**
     * StartDtmf
     *
     * @brief Enable and send DTMF
     * @param str[in], Characters sent
     * @return Returns 0 on success, others on failure.
     */
    int32_t StartDtmf(char str) override;

    /**
     * StopDtmf
     *
     * @brief Stop the DTMF
     * @return Returns 0 on success, others on failure.
     */
    int32_t StopDtmf() override;

    /**
     * CombineConference
     *
     * @brief Merge calls to form a conference
     * @return Returns 0 on success, others on failure.
     */
    int32_t CombineConference() override;

    /**
     * SeparateConference
     *
     * @brief Separates a specified call from a conference call
     * @return Returns 0 on success, others on failure.
     */
    int32_t SeparateConference() override;

    /**
     * KickOutFromConference
     *
     * @brief Kick out a specified call from a conference call
     * @return Returns 0 on success, others on failure.
     */
    int32_t KickOutFromConference() override;

    /**
     * GetCurrentCallList
     *
     * @brief Get current call list
     * @param slotId[in], The slot id
     * @return Returns call info list.
     */
    std::vector<CallAttributeInfo> GetCurrentCallList(int32_t slotId) override;

    /**
     * AddAudioDeviceList
     *
     * @brief Add an audio device
     * @param address[in], The device address
     * @param deviceType[in], The device type, only support nearing and bt hearing aid
     * @param name[in], The device name
     * @return Returns 0 on success, others on failure.
     */
    int32_t AddAudioDeviceList(const std::string &address, int32_t deviceType, const std::string &name) override;

    /**
     * RemoveAudioDeviceList
     *
     * @brief Remove an audio device
     * @param address[in], The device address
     * @param deviceType[in], The device type, only support nearing and bt hearing aid
     * @return Returns 0 on success, others on failure.
     */
    int32_t RemoveAudioDeviceList(const std::string &address, int32_t deviceType) override;

    /**
     * ResetNearlinkDeviceList
     *
     * @brief Reset all nearlink devices
     * @return Returns 0 on success, others on failure.
     */
    int32_t ResetNearlinkDeviceList() override;

    /**
     * ResetBtHearingAidDeviceList
     *
     * @brief Reset all bt hearing aid devices
     * @return Returns 0 on success, others on failure.
     */
    int32_t ResetBtHearingAidDeviceList() override;

private:
    std::vector<int32_t> getCarrierCallInfoNum(int32_t &callState, std::string &number);
    void sendEventToVoip(CallAbilityEventId eventId);
private:
    void GetVoipCallState(int32_t &numActive, int32_t &callState, std::string &number);
    std::shared_ptr<CallControlManager> callControlManagerPtr_;
    bool sendDtmfState_;
    int32_t sendDtmfCallId_;
    ffrt::ffrt lock_;
};
} // namespace Telephony
} // namespace OHOS

#endif // BLUETOOTH_CALL_SERVICE_H
