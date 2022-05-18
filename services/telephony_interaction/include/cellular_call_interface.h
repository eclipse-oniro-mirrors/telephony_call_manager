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

#ifndef CELLULAR_CALL_INTERFACE_H
#define CELLULAR_CALL_INTERFACE_H
#include "telephony_types.h"

#include "i_call_status_callback.h"

namespace OHOS {
namespace Telephony {
class CellularCallInterface : public IRemoteBroker {
public:
    // operation type
    enum class OperationType : uint32_t {
        /****************** basic ******************/
        DIAL = 1,
        HANG_UP,
        REJECT,
        ANSWER,
        HOLD_CALL,
        UN_HOLD_CALL,
        SWITCH_CALL,
        EMERGENCY_CALL,
        SET_EMERGENCY_CALL_LIST,
        COMBINE_CONFERENCE,
        SEPARATE_CONFERENCE,
        INVITE_TO_CONFERENCE,
        KICK_OUT_CONFERENCE,
        HANG_UP_ALL_CONNECTION,
        UPDATE_CALL_MEDIA_MODE,
        REGISTER_CALLBACK,
        UNREGISTER_CALLBACK,

        /****************** dtmf、rtt ******************/
        START_DTMF = 100,
        STOP_DTMF,
        SEND_DTMF,
        START_RTT,
        STOP_RTT,

        /****************** supplement ******************/
        SET_CALL_TRANSFER = 200,
        GET_CALL_TRANSFER,
        SET_CALL_WAITING,
        GET_CALL_WAITING,
        SET_CALL_RESTRICTION,
        GET_CALL_RESTRICTION,

        /****************** config ******************/
        SET_DOMAIN_PREFERENCE_MODE = 300,
        GET_DOMAIN_PREFERENCE_MODE,
        SET_LTE_IMS_SWITCH_STATUS,
        GET_LTE_IMS_SWITCH_STATUS,
        SET_IMS_CONFIG_STRING,
        SET_IMS_CONFIG_INT,
        GET_IMS_CONFIG,
        SET_IMS_FEATURE,
        GET_IMS_FEATURE,
        SET_IMS_SWITCH_ENHANCE_MODE,
        GET_IMS_SWITCH_ENHANCE_MODE,
        SET_MUTE,
        GET_MUTE,

        /****************** video settings ******************/
        CTRL_CAMERA = 400,
        SET_PREVIEW_WINDOW,
        SET_DISPLAY_WINDOW,
        SET_CAMERA_ZOOM,
        SET_PAUSE_IMAGE,
        SET_DEVICE_DIRECTION,
    };

    /**
     * Call management dial interface
     *
     * @param CellularCallInfo, dial param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t Dial(const CellularCallInfo &callInfo) = 0;

    /**
     * HangUp.
     *
     * @param CellularCallInfo, HangUp param.
     * @param CallSupplementType
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t HangUp(const CellularCallInfo &callInfo, CallSupplementType type) = 0;

    /**
     * Answer.
     *
     * @param CellularCallInfo, Answer param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t Answer(const CellularCallInfo &callInfo) = 0;

    /**
     * Reject.
     *
     * @param CellularCallInfo, Reject param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t Reject(const CellularCallInfo &callInfo) = 0;

    /**
     * HoldCall.
     *
     * @param CellularCallInfo, Hold param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t HoldCall(const CellularCallInfo &callInfo) = 0;

    /**
     * UnHoldCall.
     *
     * @param CellularCallInfo, UnHold param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t UnHoldCall(const CellularCallInfo &callInfo) = 0;

    /**
     * SwitchCall.
     *
     * @param CellularCallInfo, SwitchCall param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SwitchCall(const CellularCallInfo &callInfo) = 0;

    /**
     * IsEmergencyPhoneNumber.
     *
     * @param slotId.
     * @param phone number.
     * @param error Code.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t IsEmergencyPhoneNumber(int32_t slotId, const std::string &phoneNum, int32_t &errorCode) = 0;

    /**
     * Merge into multiple calls
     *
     * @param CellularCallInfo, SwitchCall param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t CombineConference(const CellularCallInfo &callInfo) = 0;

    /**
     * Separate Conference.
     *
     * @param CellularCallInfo, SwitchCall param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SeparateConference(const CellularCallInfo &callInfo) = 0;

    /**
     * InviteToConference interface
     *
     * @param numberList
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t InviteToConference(int32_t slotId, const std::vector<std::string> &numberList) = 0;

    /**
     * KickOutFromConference interface
     *
     * @param numberList
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t KickOutFromConference(int32_t slotId, const std::vector<std::string> &numberList) = 0;

    /**
     * Hang Up All Connection.
     *
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t HangUpAllConnection() = 0;

    /**
     * IMS Update Call Media Mode
     *
     * @param CellularCallInfo
     * @param ImsCallMode
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t UpdateImsCallMode(const CellularCallInfo &callInfo, ImsCallMode mode) = 0;

    /**
     * Start Dtmf.
     *
     * @param Dtmf Code.
     * @param CellularCallInfo.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StartDtmf(char cDtmfCode, const CellularCallInfo &callInfo) = 0;

    /**
     * Stop Dtmf.
     *
     * @param CellularCallInfo.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StopDtmf(const CellularCallInfo &callInfo) = 0;

    /**
     * Send Dtmf.
     *
     * @param Dtmf Code.
     * @param CellularCallInfo.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SendDtmf(char cDtmfCode, const CellularCallInfo &callInfo) = 0;

    /**
     * Start Rtt interface
     *
     * @param msg
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StartRtt(int32_t slotId, const std::string &msg) = 0;

    /**
     * Stop Rtt interface
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StopRtt(int32_t slotId) = 0;

    /**
     * Set Call Transfer
     *
     * @param CallTransferInfo
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetCallTransferInfo(int32_t slotId, const CallTransferInfo &ctInfo) = 0;

    /**
     * Get Call Transfer
     *
     * @param CallTransferType
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type) = 0;

    /**
     * Set Call Waiting
     *
     * @param activate
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetCallWaiting(int32_t slotId, bool activate) = 0;

    /**
     * Get Call Waiting
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetCallWaiting(int32_t slotId) = 0;

    /**
     * Set Call Restriction
     *
     * @param CallRestrictionInfo
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetCallRestriction(int32_t slotId, const CallRestrictionInfo &cRInfo) = 0;

    /**
     * Get Call Restriction
     *
     * @param CallRestrictionType
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetCallRestriction(int32_t slotId, CallRestrictionType facType) = 0;

    /**
     * Register CallBack
     *
     * @param sptr<ICallStatusCallback>
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback) = 0;

    /**
     * UnRegister CallBack
     *
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t UnRegisterCallManagerCallBack() = 0;

    /**
     * Set Domain Preference Mode
     *
     * @param mode
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetDomainPreferenceMode(int32_t slotId, int32_t mode) = 0;

    /**
     * Get Domain Preference Mode
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetDomainPreferenceMode(int32_t slotId) = 0;

    /**
     * Set Lte Ims Switch Status
     *
     * @param active
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetLteImsSwitchStatus(int32_t slotId, bool active) = 0;

    /**
     * Get Lte Ims Switch Status
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetLteImsSwitchStatus(int32_t slotId) = 0;

    /**
     * Set Ims Config
     *
     * @param ImsConfigItem
     * @param value
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, const std::string &value) = 0;

    /**
     * Set Ims Config
     *
     * @param ImsConfigItem
     * @param value
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, int32_t value) = 0;

    /**
     * Get Ims Config
     *
     * @param ImsConfigItem
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetImsConfig(int32_t slotId, ImsConfigItem item) = 0;

    /**
     * Set Ims Feature Value
     *
     * @param FeatureType
     * @param value
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value) = 0;

    /**
     * Get Ims Feature Value
     *
     * @param FeatureType
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetImsFeatureValue(int32_t slotId, FeatureType type) = 0;

    /**
     * Set Ims Switch Enhance Mode
     *
     * @param value
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsSwitchEnhanceMode(int32_t slotId, bool value) = 0;

    /**
     * Get Ims Switch Enhance Mode
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetImsSwitchEnhanceMode(int32_t slotId) = 0;

    /**
     * CtrlCamera
     *
     * @param cameraId
     * @param callingUid
     * @param callingPid
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t CtrlCamera(const std::u16string &cameraId, int32_t callingUid, int32_t callingPid) = 0;

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
    virtual int32_t SetPreviewWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height) = 0;

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
    virtual int32_t SetDisplayWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height) = 0;

    /**
     * SetCameraZoom
     *
     * @param zoomRatio
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetCameraZoom(float zoomRatio) = 0;

    /**
     * SetPauseImage
     *
     * @param path
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetPauseImage(const std::u16string &path) = 0;

    /**
     * SetDeviceDirection
     *
     * @param rotation
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetDeviceDirection(int32_t rotation) = 0;

    /**
     * SetMute
     *
     * @param mute
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetMute(int32_t slotId, int32_t mute) = 0;

    /**
     * GetMute
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetMute(int32_t slotId) = 0;

    /**
     * SetEmergencyCallList
     *
     * @brief Is it an emergency call
     * @param eccVecr[in], Phone number to be formatted
     * @param slotId[in], The slot id
     * @return Returns 0 on ture, others on false.
     */
    virtual int32_t SetEmergencyCallList(int32_t slotId, std::vector<EmergencyCall>  &eccVec) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.CellularCallInterface");
};
} // namespace Telephony
} // namespace OHOS
#endif // CELLULAR_CALL_INTERFACE_H
