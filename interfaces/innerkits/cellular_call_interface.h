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

#ifndef CELLULAR_CALL_INTERFACE_H
#define CELLULAR_CALL_INTERFACE_H
#include "telephony_types.h"

#include "i_call_status_callback.h"

namespace OHOS {
namespace Telephony {
class CellularCallInterface : public IRemoteBroker {
public:
    /**
     * @brief dial a call
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t Dial(const CellularCallInfo &callInfo) = 0;

    /**
     * @brief hang up a call
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @param type[in] Indicates the +CHLD related supplementary services.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t HangUp(const CellularCallInfo &callInfo, CallSupplementType type) = 0;

    /**
     * @brief answer a call
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t Answer(const CellularCallInfo &callInfo) = 0;

    /**
     * @brief reject a call
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t Reject(const CellularCallInfo &callInfo) = 0;

    /**
     * @brief hold a call
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t HoldCall(const CellularCallInfo &callInfo) = 0;

    /**
     * @brief unhold a call
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t UnHoldCall(const CellularCallInfo &callInfo) = 0;

    /**
     * @brief switch the call
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SwitchCall(const CellularCallInfo &callInfo) = 0;

    /**
     * @brief Checks whether the called number is an emergency number
     *
     * @param slotId[in] the slot id
     * @param phoneNum[in] the phone number
     * @param enabled[out] if the phone number is ecc, true is yes, false is no
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t IsEmergencyPhoneNumber(int32_t slotId, const std::string &phoneNum, bool &enabled) = 0;

    /**
     * @brief Merge into multiple calls
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t CombineConference(const CellularCallInfo &callInfo) = 0;

    /**
     * @brief Separate the Conference call
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SeparateConference(const CellularCallInfo &callInfo) = 0;

    /**
     * @brief Invite someone to conference
     *
     * @param numberList[in] the number list to invite to conference
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t InviteToConference(int32_t slotId, const std::vector<std::string> &numberList) = 0;

    /**
     * @brief Kick out someone from conference
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t KickOutFromConference(const CellularCallInfo &callInfo) = 0;

    /**
     * @brief Hang Up All Connection
     *
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t HangUpAllConnection() = 0;

    /**
     * @brief Hang Up All Connection
     *
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t HangUpAllConnection(int32_t slotId) = 0;

    /**
     * @brief set whether the device can make calls
     *
     * @param slotId[in] the slot id
     * @param callType[in] indicate the call type is cs or ims. 0 is cs, 1 is ims
     * @param isReadyToCall[in] indicate whether the device can make calls
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetReadyToCall(int32_t slotId, int32_t callType, bool isReadyToCall) = 0;

    /**
     * @brief IMS Update Call Media Mode
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @param mode[in] indicate the call mode just like audio only, receive only .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t UpdateImsCallMode(const CellularCallInfo &callInfo, ImsCallMode mode) = 0;

    /**
     * @brief start to paly a dtmf tone
     *
     * @param cDtmfCode[in] A character indicate the DTMF digit for which to play the tone. This
     * value must be one of {0~9}, {*} or {#}.
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StartDtmf(char cDtmfCode, const CellularCallInfo &callInfo) = 0;

    /**
     * @brief stop the playing dtmf tone
     *
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StopDtmf(const CellularCallInfo &callInfo) = 0;

    /**
     * @brief play a dtmf tone
     *
     * @param cDtmfCode[in] A character indicate the DTMF digit for which to play the tone. This
     * value must be one of {0~9}, {*} or {#}.
     * @param callInfo[in] the call detail info which contains phone number, call type, slot id .etc
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SendDtmf(char cDtmfCode, const CellularCallInfo &callInfo) = 0;

    /**
     * @brief Start a RTT session
     *
     * @param msg the RTT message
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StartRtt(int32_t slotId, const std::string &msg) = 0;

    /**
     * @brief Terminate the current RTT session
     *
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StopRtt(int32_t slotId) = 0;

    /**
     * @brief set call transfer for the slot id
     *
     * @param ctInfo[in] contains the call transfer type, enable/disable, the transfered number, the start/end time
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetCallTransferInfo(int32_t slotId, const CallTransferInfo &ctInfo) = 0;

    /**
     * @brief confirm whether IMS can set call transfer time
     *
     * @param slotId[in] the slot id
     * @param result[out] whether IMS can set call transfer time. true mean yes, false mean no
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t CanSetCallTransferTime(int32_t slotId, bool &result) = 0;

    /**
     * @brief Get Call Transfer information
     *
     * @param type[in] indicate the call transfer type, just like CFU, CFB, CFNRy, CFNRc
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type) = 0;

    /**
     * @brief Set Call Waiting
     *
     * @param activate[in] true mean activate the call waiting, false mean inactivate
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetCallWaiting(int32_t slotId, bool activate) = 0;

    /**
     * @brief Gets whether the call waiting service of the current account is enabled
     *
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetCallWaiting(int32_t slotId) = 0;

    /**
     * @brief Set the call restriction function for the current account
     *
     * @param cRInfo[in] contains the password, call restriction type and call restriction mode
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetCallRestriction(int32_t slotId, const CallRestrictionInfo &cRInfo) = 0;

    /**
     * @brief Set the call restriction password of the specified account
     *
     * @param slotId[in] the slot id
     * @param fac[in] indicate the call restriction type, just like all incoming, all outgoing .etc
     * @param oldPassword[in] indicate the call restriction old password
     * @param newPassword[in] indicate the call restriction new password
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetCallRestrictionPassword(
        int32_t slotId, CallRestrictionType fac, const char *oldPassword, const char *newPassword) = 0;

    /**
     * @brief Gets the call restriction information of the specified account
     *
     * @param facType[in] indicate the call restriction type, just like all incoming, all outgoing .etc
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetCallRestriction(int32_t slotId, CallRestrictionType facType) = 0;

    /**
     * @brief Register CallMnager CallBack ptr
     *
     * @param callback callback ptr
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback) = 0;

    /**
     * @brief UnRegister CallMnager CallBack ptr
     *
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t UnRegisterCallManagerCallBack() = 0;

    /**
     * @brief Set Domain Preference Mode
     *
     * @param mode[in] indicate the domain preference, 1: CS only, 2: CS prefer, 3: PS prefer, 4: PS only
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetDomainPreferenceMode(int32_t slotId, int32_t mode) = 0;

    /**
     * @brief Get Domain Preference Mode
     *
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetDomainPreferenceMode(int32_t slotId) = 0;

    /**
     * @brief Set Ims Switch Status
     *
     * @param active[in] indicate the ims switch status, true is on, false is off
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsSwitchStatus(int32_t slotId, bool active) = 0;

    /**
     * @brief Get Ims Switch Status
     *
     * @param slotId[in] the slot id
     * @param enabled[out] indicate the ims switch status, true is on, false is off
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetImsSwitchStatus(int32_t slotId, bool &enabled) = 0;

    /**
     * @brief Set VoNR Switch Status
     *
     * @param state[in] Indicates the VoNR state, 0: off, 1: on
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetVoNRState(int32_t slotId, int32_t state) = 0;

    /**
     * @brief Get VoNR Switch Status
     *
     * @param slotId[in] the slot id
     * @param state[out] Indicates the VoNR state, 0: off, 1: on
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetVoNRState(int32_t slotId, int32_t &state) = 0;

    /**
     * @brief Set Ims Config
     *
     * @param item[in] Identify specific item, like ITEM_VIDEO_QUALITY, ITEM_IMS_SWITCH_STATUS
     * @param value[in] The specific value corresponding to the item
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, const std::string &value) = 0;

    /**
     * @brief Set Ims Config
     *
     * @param item[in] Identify specific item, like ITEM_VIDEO_QUALITY, ITEM_IMS_SWITCH_STATUS
     * @param value[in] The specific value corresponding to the item
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, int32_t value) = 0;

    /**
     * @brief Get Ims Config
     *
     * @param item[in] Identify specific item, like ITEM_VIDEO_QUALITY, ITEM_IMS_SWITCH_STATUS
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetImsConfig(int32_t slotId, ImsConfigItem item) = 0;

    /**
     * @brief Set Ims Feature Value
     *
     * @param type[in] the ims feature item, like VoLTE, ViLTE, SS over UT
     * @param value[in] The specific value corresponding to the item
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value) = 0;

    /**
     * @brief Get Ims Feature Value
     *
     * @param type[in] the ims feature item, like VoLTE, ViLTE, SS over UT
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetImsFeatureValue(int32_t slotId, FeatureType type) = 0;

    /**
     * @brief set camara to be enabled for video call
     *
     * @param cameraId[in] The id of the camera
     * @param callingUid[in] the UID of call
     * @param callingPid[in] the PID if call
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t CtrlCamera(const std::u16string &cameraId, int32_t callingUid, int32_t callingPid) = 0;

    /**
     * @brief set a window which used to display a preview of camera capturing
     *
     * @param x[in] X coordinate of window
     * @param y[in] Y coordinate of window
     * @param z[in] Z coordinate of window
     * @param width[in] the width of window
     * @param height[in] the height of window
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetPreviewWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height) = 0;

    /**
     * @brief set a window which used to display the viedo which is received from remote
     *
     * @param x[in] X coordinate of window
     * @param y[in] Y coordinate of window
     * @param z[in] Z coordinate of window
     * @param width[in] the width of window
     * @param height[in] the height of window
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetDisplayWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height) = 0;

    /**
     * @brief set camera zoom ratio
     *
     * @param zoomRatio[in] the camera zoom ratio
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetCameraZoom(float zoomRatio) = 0;

    /**
     * @brief set a image which will be displayed when the video signal is paused
     *
     * @param path[in] the dispalyed image path
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetPauseImage(const std::u16string &path) = 0;

    /**
     * @brief set the device orientation
     *
     * @param rotation[in] The device orientation, in degrees
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetDeviceDirection(int32_t rotation) = 0;

    /**
     * @brief Set the mute state of the call
     *
     * @param mute[in] 1 means the call could be muted
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetMute(int32_t slotId, int32_t mute) = 0;

    /**
     * @brief Get the mute state of the call
     *
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetMute(int32_t slotId) = 0;

    /**
     * @brief Set emergency phone number
     *
     * @param eccVecr[in] ecc number info list
     * @param slotId[in] The slot id
     * @return Returns TELEPHONY_SUCCESS on true, others on false.
     */
    virtual int32_t SetEmergencyCallList(int32_t slotId, std::vector<EmergencyCall> &eccVec) = 0;

    /**
     * @brief Close Unfinished ussd function for the current account
     *
     * @param slotId[in] the slot id
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t CloseUnFinishedUssd(int32_t slotId) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.CellularCallInterface");
};
} // namespace Telephony
} // namespace OHOS
#endif // CELLULAR_CALL_INTERFACE_H
