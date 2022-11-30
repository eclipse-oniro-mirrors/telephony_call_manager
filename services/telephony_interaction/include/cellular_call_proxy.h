/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef CELLULAR_CALL_PROXY_H
#define CELLULAR_CALL_PROXY_H

#include "cellular_call_interface.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Telephony {
class CellularCallProxy : public IRemoteProxy<CellularCallInterface> {
public:
    /**
     * CellularCallProxy
     *
     * @param impl
     */
    explicit CellularCallProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<CellularCallInterface>(impl) {}

    /**
     * ~CellularCallProxy
     *
     */
    ~CellularCallProxy() = default;

    /**
     * Call management dial interface
     *
     * @param CellularCallInfo, dial param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t Dial(const CellularCallInfo &callInfo) override;

    /**
     * HangUp.
     *
     * @param CellularCallInfo, HangUp param.
     * @param CallSupplementType
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t HangUp(const CellularCallInfo &callInfo, CallSupplementType type) override;

    /**
     * Reject.
     *
     * @param CellularCallInfo, Reject param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t Reject(const CellularCallInfo &callInfo) override;

    /**
     * Answer.
     *
     * @param CellularCallInfo, Answer param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t Answer(const CellularCallInfo &callInfo) override;

    /**
     * HoldCall.
     *
     * @param CellularCallInfo, Hold param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t HoldCall(const CellularCallInfo &callInfo) override;

    /**
     * UnHoldCall.
     *
     * @param CellularCallInfo, UnHold param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t UnHoldCall(const CellularCallInfo &callInfo) override;

    /**
     * SwitchCall.
     *
     * @param CellularCallInfo, SwitchCall param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SwitchCall(const CellularCallInfo &callInfo) override;

    /**
     * IsEmergencyPhoneNumber.
     *
     * @param phone number.
     * @param slotId.
     * @param error Code.
     * @return Is Urgent Call.
     */
    int32_t IsEmergencyPhoneNumber(int32_t slotId, const std::string &phoneNum, int32_t &errorCode) override;

    /**
     * Merge into multiple calls
     *
     * @param CellularCallInfo.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t CombineConference(const CellularCallInfo &callInfo) override;

    /**
     * SeparateConference.
     *
     * @param std::string splitString
     * @param CellularCallInfo.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SeparateConference(const CellularCallInfo &callInfo) override;

    /**
     * InviteToConference interface
     *
     * @param numberList
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t InviteToConference(int32_t slotId, const std::vector<std::string> &numberList) override;

    /**
     * KickOutFromConference interface
     *
     * @param numberList
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t KickOutFromConference(int32_t slotId, const std::vector<std::string> &numberList) override;

    /**
     * Hang Up All Connection.
     *
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t HangUpAllConnection() override;

    /**
     * IMS Update Call Media Mode
     *
     * @param CellularCallInfo
     * @param ImsCallMode
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t UpdateImsCallMode(const CellularCallInfo &callInfo, ImsCallMode mode) override;

    /**
     * StartDtmf.
     *
     * @param Dtmf Code.
     * @param CellularCallInfo.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t StartDtmf(char cDtmfCode, const CellularCallInfo &callInfo) override;

    /**
     * StopDtmf.
     *
     * @param CellularCallInfo.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t StopDtmf(const CellularCallInfo &callInfo) override;

    /**
     * SendDtmf.
     *
     * @param Dtmf Code.
     * @param CellularCallInfo.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SendDtmf(char cDtmfCode, const CellularCallInfo &callInfo) override;

    /**
     * Start Rtt
     *
     * @param msg
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t StartRtt(int32_t slotId, const std::string &msg) override;

    /**
     * Stop Rtt
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t StopRtt(int32_t slotId) override;

    /**
     * Set Call Transfer
     *
     * @param CallTransferInfo
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetCallTransferInfo(int32_t slotId, const CallTransferInfo &cfInfo) override;

    /**
     * Inquire Call Transfer
     *
     * @param CallTransferType
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type) override;

    /**
     * Set Call Waiting
     *
     * @param activate
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetCallWaiting(int32_t slotId, bool activate) override;

    /**
     * Inquire Call Waiting
     *
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetCallWaiting(int32_t slotId) override;

    /**
     * Set Call Restriction
     *
     * @param CallRestrictionInfo
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetCallRestriction(int32_t slotId, const CallRestrictionInfo &crInfo) override;

    /**
     * Inquire Call Restriction
     *
     * @param CallRestrictionType
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetCallRestriction(int32_t slotId, CallRestrictionType facType) override;

    /**
     * RegisterCallManagerCallBack
     *
     * @param sptr<ICallStatusCallback>
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback) override;

    /**
     * UnRegister CallManager CallBack
     *
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t UnRegisterCallManagerCallBack() override;

    /**
     * Set Domain Preference Mode
     *
     * @param mode
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetDomainPreferenceMode(int32_t slotId, int32_t mode) override;

    /**
     * Get Domain Preference Mode
     *
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetDomainPreferenceMode(int32_t slotId) override;

    /**
     * Set Ims Switch Status
     *
     * @param active
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsSwitchStatus(int32_t slotId, bool active) override;

    /**
     * Get Ims Switch Status
     *
     * @param slot Id
     * @param enabled
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetImsSwitchStatus(int32_t slotId, bool &enabled) override;

    /**
     * Set Ims Config
     *
     * @param ImsConfigItem
     * @param value
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, const std::string &value) override;

    /**
     * Set Ims Config
     *
     * @param ImsConfigItem
     * @param value
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, int32_t value) override;

    /**
     * Get Ims Config
     *
     * @param ImsConfigItem
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetImsConfig(int32_t slotId, ImsConfigItem item) override;

    /**
     * Set Ims Feature Value
     *
     * @param FeatureType
     * @param value
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value) override;

    /**
     * Get Ims Feature Value
     *
     * @param FeatureType
     * @param slot Id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetImsFeatureValue(int32_t slotId, FeatureType type) override;

    /**
     * CtrlCamera
     *
     * @param cameraId
     * @param callingUid
     * @param callingPid
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t CtrlCamera(const std::u16string &cameraId, int32_t callingUid, int32_t callingPid) override;

    /**
     * SetPreviewWindow
     *
     * @param x
     * @param y
     * @param z
     * @param width
     * @param height
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetPreviewWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height) override;

    /**
     * SetDisplayWindow
     *
     * @param x
     * @param y
     * @param z
     * @param width
     * @param height
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetDisplayWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height) override;

    /**
     * SetCameraZoom
     *
     * @param zoomRatio
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetCameraZoom(float zoomRatio) override;

    /**
     * SetPauseImage
     *
     * @param path
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetPauseImage(const std::u16string &path) override;

    /**
     * SetDeviceDirection
     *
     * @param rotation
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetDeviceDirection(int32_t rotation) override;

    /**
     * SetMute
     *
     * @param mute
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetMute(int32_t mute, int32_t slotId) override;

    /**
     * GetMute
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetMute(int32_t slotId) override;

    int32_t SetEmergencyCallList(int32_t slotId, std::vector<EmergencyCall>  &eccVec) override;

private:
    /**
     * SetCommonParamForMessageParcel, set common param for MessageParcel
     *
     * @param slotId
     * @param in
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetCommonParamForMessageParcel(int32_t slotId, MessageParcel &in);

private:
    static inline BrokerDelegator<CellularCallProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CELLULAR_CALL_PROXY_H
