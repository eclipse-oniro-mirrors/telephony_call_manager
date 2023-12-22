/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef VOIP_CALL_MANAGER_PROXY_H
#define VOIP_CALL_MANAGER_PROXY_H

#include <cfloat>
#include <cstdio>
#include <string>
#include <vector>

#include "i_voip_call_manager_service.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "pac_map.h"
#include "pixel_map.h"

namespace OHOS {
namespace Telephony {
class VoipCallManagerProxy : public IRemoteProxy<IVoipCallManagerService> {
public:
    /**
     * VoipCallManagerProxy
     *
     * @param impl
     */
    explicit VoipCallManagerProxy(const sptr<IRemoteObject> &impl);
    virtual ~VoipCallManagerProxy() = default;

    /**
     * @brief thrid-party applications report a incoming call.
     *
     * @param extras[in], extras date.
     * @return Returns 0 on success, others on failure.
     */
    int32_t ReportIncomingCall(
        AppExecFwk::PacMap &extras, std::shared_ptr<OHOS::Media::PixelMap> &userProfile, ErrorReason &reason) override;
    /**
     * @brief thrid-party applications report incoming call failed causes.
     *
     * @param extras[in], extras date.
     * @return Returns 0 on success, others on failure.
     */
    int32_t ReportIncomingCallError(AppExecFwk::PacMap &extras) override;
    /**
     * @brief thrid-party applications report call state change.
     *
     * @param callId[in], The call id
     * @param VoipCallState[in], The voip call state
     * @return Returns 0 on success, others on failure.
     */
    int32_t ReportCallStateChange(std::string &callId, const VoipCallState &state) override;
    /**
     * RegisterCallBack
     *
     * @brief Register callback
     * @param callback[in], callback function pointer
     * @return Returns 0 on success, others on failure.
     */
    int32_t RegisterCallBack(const sptr<IVoipCallManagerCallback> &callback) override;

    /**
     * UnRegisterCallBack
     *
     * @brief unregister callback
     * @return Returns 0 on success, others on failure.
     */
    int32_t UnRegisterCallBack() override;
    /**
     * @brief pushSA report a incoming call.
     *
     * @param callId[in], The call id
     * @param bundleName[in], The bundleName
     * @return Returns 0 on success, others on failure.
     */
    int32_t ReportVoipIncomingCall(std::string &callId, std::string &bundleName) override;

    /**
     * @brief pushSA report call extensionId.
     *
     * @param callId[in], The call id
     * @param bundleName[in], The bundleName
     * @param extensionId[in], The extensionId
     * @return Returns 0 on success, others on failure.
     */
    int32_t ReportVoipCallExtensionId(std::string &callId, std::string &bundleName, std::string &extensionId) override;
    /**
     * @brief callmanager report call event change.
     *
     * @param callId[in], The call id
     * @param bundleName[in], The bundleName
     * @param event[in], The VoipCallEvent
     * @return Returns 0 on success, others on failure.
     */
    int32_t ReportVoipCallEventChange(const VoipCallEvents &events) override;

    /**
     * @brief Register CallManager CallBack ptr
     *
     * @param callback callback ptr
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback) override;
    /**
     * @brief UnRegister CallManager CallBack ptr
     *
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t UnRegisterCallManagerCallBack() override;

private:
    static inline BrokerDelegator<VoipCallManagerProxy> delegator_;
};

} // namespace Telephony
} // namespace OHOS

#endif