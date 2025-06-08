/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef NEARLINK_CALL_CLIENT_H
#define NEARLINK_CALL_CLIENT_H

#include "singleton.h"
#include "pac_map.h"

#include "call_manager_callback.h"

namespace OHOS {
namespace Telephony {
class NearlinkCallClient : public DelayedRefSingleton<NearlinkCallClient> {
    DECLARE_DELAYED_REF_SINGLETON(NearlinkCallClient)
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
     * @brief Get current call list
     *
     * @param slotId[in], The slot id
     * @return Returns call info list.
     */
    std::vector<CallAttributeInfo> GetCurrentCallList(int32_t slotId);

    /**
     * @brief Add nearlink audio device
     *
     * @param address[in], audio device address
     * @param name[in], audio device name
     * @return Returns 0 on success, others on failure.
     */
    int32_t AddAudioDevice(const std::string &address, const std::string &name);

    /**
     * @brief Remove nearlink audio device
     *
     * @param address[in], audio device address
     * @return Returns 0 on success, others on failure.
     */
    int32_t RemoveAudioDevice(const std::string &address);

    /**
     * @brief Reset all nearlink audio device when nearlink service removed
     *
     * @return Returns 0 on success, others on failure.
     */
    int32_t ResetNearlinkDeviceList();
};
}
}
#endif
