/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef BLUETOOTH_CALL_PROXY_H
#define BLUETOOTH_CALL_PROXY_H

#include <cstdio>
#include <string>

#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "pac_map.h"

#include "bluetooth_call_ipc_interface_code.h"
#include "call_manager_inner_type.h"
#include "i_bluetooth_call.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
class BluetoothCallProxy : public IRemoteProxy<IBluetoothCall> {
public:
    /**
     * BluetoothCallProxy
     *
     * @param impl
     */
    explicit BluetoothCallProxy(const sptr<IRemoteObject> &impl);
    virtual ~BluetoothCallProxy() = default;

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
     * @brief Hangup a specified call from a conference call
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
     * @param deviceType[in], The device type
     * @param name[in], The device name
     * @return Returns 0 on success, others on failure.
     */
    int32_t AddAudioDeviceList(const std::string &address, int32_t deviceType, const std::string &name) override;

    /**
     * RemoveAudioDeviceList
     *
     * @brief Remove an audio device
     * @param address[in], The device address
     * @param deviceType[in], The device type
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

private:
    int32_t SendRequest(BluetoothCallInterfaceCode code);
    int32_t SendRequest(BluetoothCallInterfaceCode code, MessageParcel &dataParcel, MessageParcel &replyParcel);

private:
    static inline BrokerDelegator<BluetoothCallProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS

#endif // BLUETOOTH_CALL_PROXY_H