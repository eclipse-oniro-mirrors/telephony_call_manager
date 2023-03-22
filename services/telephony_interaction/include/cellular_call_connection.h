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

#ifndef CELLULAR_CALL_CONNECTION_H
#define CELLULAR_CALL_CONNECTION_H

#include <mutex>

#include "call_status_callback.h"
#include "cellular_call_interface.h"
#include "i_call_status_callback.h"
#include "if_system_ability_manager.h"
#include "refbase.h"
#include "rwlock.h"
#include "singleton.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace Telephony {
class CellularCallConnection : public std::enable_shared_from_this<CellularCallConnection> {
    DECLARE_DELAYED_SINGLETON(CellularCallConnection)
public:
    void Init(int32_t systemAbilityId);
    void UnInit();

    /**
     * Dial
     *
     * @brief Make a phone call
     * @param callInfo[in], Call information.
     * @return Returns callId when the value is greater than zero, others on failure.
     */
    int Dial(const CellularCallInfo &callInfo);

    /**
     * HangUp
     *
     * @brief Hang up the phone
     * @param callInfo[in], Call information.
     * @param CallSupplementType
     * @return Returns 0 on success, others on failure.
     */
    int HangUp(const CellularCallInfo &callInfo, CallSupplementType type);

    /**
     * Reject
     *
     * @brief Reject a phone call
     * @param callInfo[in], Call information.
     * @return Returns 0 on success, others on failure.
     */
    int Reject(const CellularCallInfo &callInfo);

    /**
     * Answer
     *
     * @brief Answer a phone call
     * @param callInfo[in], Call information.
     * @return Returns 0 on success, others on failure.
     */
    int Answer(const CellularCallInfo &callInfo);

    /**
     * HoldCall
     *
     * @brief Park a phone call
     * @param callInfo[in], Call information.
     * @return Returns 0 on success, others on failure.
     */
    int HoldCall(const CellularCallInfo &callInfo);

    /**
     * UnHoldCall
     *
     * @brief Activate a phone call
     * @param callInfo[in], Call information.
     * @return Returns 0 on success, others on failure.
     */
    int UnHoldCall(const CellularCallInfo &callInfo);

    /**
     * SwitchCall
     *
     * @brief Switch the phone
     * @param callInfo[in], Call information.
     * @return Returns 0 on success, others on failure.
     */
    int SwitchCall(const CellularCallInfo &callInfo);

    /**
     * IsEmergencyPhoneNumber
     *
     * @brief Is it an emergency call
     * @param number[in], Phone number to be formatted
     * @param slotId[in], The slot id
     * @param enabled[out] true is emergency phonenumber, other is not
     * @return Returns 0 on success, others on failure.
     */
    int IsEmergencyPhoneNumber(const std::string &phoneNum, int32_t slotId, bool &enabled);

    /**
     * CombineConference
     *
     * @brief Merge calls to form a conference
     * @param callInfo[in], Call information.
     * @return Returns 0 on success, others on failure.
     */
    int CombineConference(const CellularCallInfo &callInfo);

    /**
     * SeparateConference
     *
     * @brief Separates a specified call from a conference call
     * @param callInfo[in], Call information.
     * @return Returns 0 on success, others on failure.
     */
    int SeparateConference(const CellularCallInfo &callInfo);

    /**
     * StartDtmf
     *
     * @brief Enable and send DTMF
     * @param cDTMFCode[in], Characters sent
     * @param callInfo[in], Call information.
     * @return Returns 0 on success, others on failure.
     */
    int StartDtmf(char cDTMFCode, const CellularCallInfo &callInfo);

    /**
     * StopDtmf
     *
     * @brief Stop the DTMF
     * @param callInfo[in], Call information.
     * @return Returns 0 on success, others on failure.
     */
    int StopDtmf(const CellularCallInfo &callInfo);

    /**
     * SendDtmf
     *
     * @brief Sending DTMF
     * @param cDTMFCode[in], Characters sent
     * @param phoneNum[in], Phone number corresponding to the call
     * @return Returns 0 on success, others on failure.
     */
    int SendDtmf(char cDTMFCode, const std::string &phoneNum);

    /**
     * SendDtmfString
     *
     * @brief Send a string of DTMFS
     * @param dtmfCodeStr[in], Characters sent
     * @param phoneNum[in], Phone number corresponding to the call
     * @param phoneNetType[in].
     * @param switchOn[in].
     * @param switchOff[in].
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int SendDtmfString(const std::string &dtmfCodeStr, const std::string &phoneNum, PhoneNetType phoneNetType,
        int32_t switchOn, int32_t switchOff);

    /**
     * SetCallTransferInfo
     *
     * @brief Set the call transfer function for the current account
     * @param info[in], Call Transfer Information
     * @param slotId[in], The slot id
     * @return Returns 0 on success, others on failure.
     */
    int SetCallTransferInfo(const CallTransferInfo &info, int32_t slotId);

    /**
     * CanSetCallTransferTime
     *
     * @brief confirm whether IMS can set call transfer time.
     * @param slotId[in], The slot id
     * @param result[out], The result of can set or not
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int CanSetCallTransferTime(int32_t slotId, bool &result);

    /**
     * GetCallTransferInfo
     *
     * @brief Gets the call transfer information of the current account
     * @param type[in], Call Transfer Type
     * @param slotId[in], The slot id
     * @return Returns 0 on success, others on failure.
     */
    int GetCallTransferInfo(CallTransferType type, int32_t slotId);

    /**
     * SetCallWaiting
     *
     * @brief Set the call waiting function for the current account
     * @param activate[in], Activation of switch
     * @param slotId[in], The slot id
     * @return Returns 0 on success, others on failure.
     */
    int SetCallWaiting(bool activate, int32_t slotId);

    /**
     * GetCallWaiting
     *
     * @brief Gets whether the call waiting service of the current account is enabled
     * @param slotId[in], The slot id
     * @return Returns 0 on success, others on failure.
     */
    int GetCallWaiting(int32_t slotId);

    /**
     * SetCallRestriction
     *
     * @brief Set the call restriction function for the current account
     * @param info[in], Call restriction information
     * @param slotId[in], The slot id
     * @return Returns 0 on success, others on failure.
     */
    int SetCallRestriction(const CallRestrictionInfo &info, int32_t slotId);

    /**
     * GetCallRestriction
     *
     * @brief Gets the call restriction information of the specified account
     * @param facType[in], Call Restriction type
     * @param slotId[in], The slot id
     * @return Returns 0 on success, others on failure.
     */
    int GetCallRestriction(CallRestrictionType facType, int32_t slotId);

    /**
     * SetCallPreferenceMode
     *
     * @brief Setting the Call Type
     * @param slotId[in], The slot id
     * @param mode[in], Preference Mode
     * @return Returns 0 on success, others on failure.
     */
    int SetCallPreferenceMode(int32_t slotId, int32_t mode);

    /**
     * StartRtt
     *
     * @brief Enable and send RTT information
     * @param callInfo[in], Call information.
     * @param msg[in], RTT information
     * @return Returns 0 on success, others on failure.
     */
    int StartRtt(const CellularCallInfo &callInfo, std::u16string &msg);

    /**
     * StopRtt
     *
     * @brief Close the RTT
     * @param callInfo[in], Call information.
     * @return Returns 0 on success, others on failure.
     */
    int StopRtt(const CellularCallInfo &callInfo);

    /**
     * RegisterCallBack
     *
     * @brief Register callback
     * @param callback[in], callback function pointer
     * @return Returns 0 on success, others on failure.
     */
    int RegisterCallBack(const sptr<ICallStatusCallback> &callback);

    /**
     * ControlCamera
     *
     * @brief Open or close camera
     * @param cameraId[in], The camera id
     * @param callingUid[in]
     * @param callingPid[in]
     * @return Returns 0 on success, others on failure.
     */
    int32_t ControlCamera(std::u16string cameraId, int32_t callingUid, int32_t);

    /**
     * SetPreviewWindow
     *
     * @brief Set the location and size of the preview window for videos captured by the local camera.
     * @param window[in], Window information
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetPreviewWindow(VideoWindow &window);

    /**
     * SetDisplayWindow
     *
     * @brief Sets the location and size of the remote video window.
     * @param window[in], Window information
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetDisplayWindow(VideoWindow &window);

    /**
     * SetCameraZoom
     *
     * @brief Sets the local camera zoom scale
     * @param zoomRatio[in], Camera scale
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetCameraZoom(float zoomRatio);

    /**
     * SetPausePicture
     *
     * @brief APP sets the screen of the remote video freeze immediately.
     * If the APP does not call this interface when making a video call,
     * the last frame before the remote video freeze is displayed by default
     * @param path[in], Local Picture address
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetPausePicture(std::u16string path);

    /**
     * SetDeviceDirection
     *
     * @brief Set the rotation Angle of the local device. The default value is 0
     * @param rotation[in], Rotation Angle
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetDeviceDirection(int32_t rotation);

    /**
     * SetImsSwitchStatus
     *
     * @brief Setting Ims Switch
     * @param slotId[in], The slot id
     * @param active[in],  On or off
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetImsSwitchStatus(int32_t slotId, bool active);

    /**
     * GetImsSwitchStatus
     *
     * @brief Getting Ims Switch
     * @param slotId[in], The slot id
     * @param enabled[out], The result of enable or not
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetImsSwitchStatus(int32_t slotId, bool &enabled);

    /**
     * SendUpdateCallMediaModeRequest
     *
     * @brief send update call media request
     * @param callInfo[in], Call information.
     * @param mode[in], Calling patterns
     * @return Returns 0 on success, others on failure.
     */
    int32_t SendUpdateCallMediaModeRequest(const CellularCallInfo &callInfo, ImsCallMode mode);

    /**
     * Set Ims Config
     *
     * @param ImsConfigItem
     * @param value
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsConfig(ImsConfigItem item, const std::string &value, int32_t slotId);

    /**
     * Set Ims Config
     *
     * @param ImsConfigItem
     * @param value
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsConfig(ImsConfigItem item, int32_t value, int32_t slotId);

    /**
     * Get Ims Config
     *
     * @param ImsConfigItem
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetImsConfig(ImsConfigItem item, int32_t slotId);

    /**
     * Set Ims Feature Value
     *
     * @param FeatureType
     * @param value
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsFeatureValue(FeatureType type, int32_t value, int32_t slotId);

    /**
     * Get Ims Feature Value
     *
     * @param FeatureType
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetImsFeatureValue(FeatureType type, int32_t slotId);

    /**
     * InviteToConference interface
     *
     * @param numberList
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t InviteToConference(const std::vector<std::string> &numberList, int32_t slotId);

    /**
     * SetMute
     *
     * @param mute
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetMute(int32_t mute, int32_t slotId);

    /**
     * Is Connect cellular call service Object
     *
     * @return result for Connect cellular call service
     */
    bool IsConnect() const;

private:
    int32_t ConnectService();
    int32_t RegisterCallBackFun();
    void DisconnectService();
    int32_t ReConnectService();
    void OnDeath();
    void Clean();
    void NotifyDeath();

private:
    class SystemAbilityListener : public SystemAbilityStatusChangeStub {
    public:
        SystemAbilityListener() = default;
        ~SystemAbilityListener() = default;
        void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
        void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    };

private:
    int32_t systemAbilityId_;
    sptr<ICallStatusCallback> cellularCallCallbackPtr_;
    sptr<CellularCallInterface> cellularCallInterfacePtr_;
    sptr<ISystemAbilityStatusChange> statusChangeListener_ = nullptr;
    bool connectState_;
    Utils::RWLock rwClientLock_;
};
} // namespace Telephony
} // namespace OHOS

#endif
