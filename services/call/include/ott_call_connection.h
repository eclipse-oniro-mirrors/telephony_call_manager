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

#ifndef OTT_CALL_CONNECTION_H
#define OTT_CALL_CONNECTION_H

#include "pac_map.h"

#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
class OTTCallConnection {
public:
    OTTCallConnection() = default;
    ~OTTCallConnection() = default;

    /**
     * Answer
     *
     * @brief Answer a phone call
     * @param requestInfo[in], Call request information.
     * @return Returns 0 on success, others on failure.
     */
    int32_t Answer(const OttCallRequestInfo &requestInfo);

    /**
     * HangUp
     *
     * @brief Hang up the phone
     * @param requestInfo[in], Call request information.
     * @return Returns 0 on success, others on failure.
     */
    int32_t HangUp(const OttCallRequestInfo &requestInfo);

    /**
     * Reject
     *
     * @brief Reject a phone call
     * @param requestInfo[in], Call request information.
     * @return Returns 0 on success, others on failure.
     */
    int32_t Reject(const OttCallRequestInfo &requestInfo);

    /**
     * HoldCall
     *
     * @brief Park a phone call
     * @param requestInfo[in], Call request information.
     * @return Returns 0 on success, others on failure.
     */
    int32_t HoldCall(const OttCallRequestInfo &requestInfo);

    /**
     * UnHoldCall
     *
     * @brief Activate a phone call
     * @param requestInfo[in], Call request information.
     * @return Returns 0 on success, others on failure.
     */
    int32_t UnHoldCall(const OttCallRequestInfo &requestInfo);

    /**
     * SwitchCall
     *
     * @brief Switch the phone
     * @return Returns 0 on success, others on failure.
     */
    int32_t SwitchCall(const OttCallRequestInfo &requestInfo);

    /**
     * CombineConference
     *
     * @brief Merge calls to form a conference
     * @param requestInfo[in], Call request information.
     * @return Returns 0 on success, others on failure.
     */
    int32_t CombineConference(const OttCallRequestInfo &requestInfo);

    /**
     * SeparateConference
     *
     * @brief Separates a specified call from a conference call
     * @param requestInfo[in], Call request information.
     * @return Returns 0 on success, others on failure.
     */
    int32_t SeparateConference(const OttCallRequestInfo &requestInfo);

    /**
     * KickOutFromConference
     *
     * @brief Hangup a specified call from a conference call
     * @param requestInfo[in], Call request information.
     * @return Returns 0 on success, others on failure.
     */
    int32_t KickOutFromConference(const OttCallRequestInfo &requestInfo);

    /**
     * InviteToConference interface
     *
     * @param numberList
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t InviteToConference(const OttCallRequestInfo &requestInfo, const std::vector<std::string> &numberList);

    /**
     * UpdateImsCallMode
     *
     * @brief send update call media request
     * @param requestInfo[in], Call request information.
     * @param mode[in], Calling patterns
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateImsCallMode(const OttCallRequestInfo &requestInfo, ImsCallMode mode);

private:
    void PackCellularCallInfo(const OttCallRequestInfo &requestInfo, AppExecFwk::PacMap &info);
};
} // namespace Telephony
} // namespace OHOS
#endif // OTT_CALL_CONNECTION_H
