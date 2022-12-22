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

#ifndef CALL_MANAGER_SERVICE_H
#define CALL_MANAGER_SERVICE_H

#include <memory>

#include "iremote_stub.h"
#include "iservice_registry.h"
#include "singleton.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "call_state_report_proxy.h"
#include "call_manager_service_stub.h"
#include "call_control_manager.h"
#include "bluetooth_call_manager.h"

namespace OHOS {
namespace Telephony {
class CallManagerService : public SystemAbility,
                           public CallManagerServiceStub,
                           public std::enable_shared_from_this<CallManagerService> {
    DECLARE_DELAYED_SINGLETON(CallManagerService)
    DECLARE_SYSTEM_ABILITY(CallManagerService)
public:
    bool Init();
    void UnInit();

    /**
     * service OnStart
     */
    void OnStart() override;

    /**
     * service OnStop
     */
    void OnStop() override;

    /**
     * service dump
     * @param fd
     * @param args
     * @return
     */
    int32_t Dump(std::int32_t fd, const std::vector<std::u16string> &args) override;

    /**
     * Get bindTime_
     * @return string
     */
    std::string GetBindTime();

    /**
     * Get spendTime_
     * @return string
     */
    std::string GetStartServiceSpent();

    /**
     * Get serviceRunningState_
     * @return serviceRunningState_
     */
    int32_t GetServiceRunningState();

    /**
     * RegisterCallBack
     *
     * @brief Register callback
     * @param callback[in], callback function pointer
     * @param bundleName[in], bundle name
     * @return Returns 0 on success, others on failure.
     */
    int32_t RegisterCallBack(const sptr<ICallAbilityCallback> &callback) override;

    /**
     * UnRegisterCallBack
     *
     * @brief unregister callback
     * @param bundleName[in], bundle name
     * @return Returns 0 on success, others on failure.
     */
    int32_t UnRegisterCallBack() override;

    /**
     * DialCall
     *
     * @brief Make a phone call
     * @param number[in], dial param.
     * @param extras[in], extras date.
     * @return Returns callId when the value is greater than zero, others on failure.
     */
    int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras) override;

    /**
     * AnswerCall
     *
     * @brief Answer a phone call
     * @param callId[in], call id
     * @param videoState[in], 0: audio, 1: video
     * @return Returns 0 on success, others on failure.
     */
    int32_t AnswerCall(int32_t callId, int32_t videoState) override;

    /**
     * RejectCall
     *
     * @brief Reject a phone call
     * @param callId[in], call id
     * @param rejectWithMessage[in], Whether to enter the reason for rejection,true:yes false:no
     * @param textMessage[in], The reason you reject the call
     * @return Returns 0 on success, others on failure.
     */
    int32_t RejectCall(int32_t callId, bool rejectWithMessage, std::u16string textMessage) override;

    /**
     * HangUpCall
     *
     * @brief Hang up the phone
     * @param callId[in], call id
     * @return Returns 0 on success, others on failure.
     */
    int32_t HangUpCall(int32_t callId) override;

    /**
     * GetCallState
     *
     * @brief Obtain the call status of the device
     * @return Returns call state.
     */
    int32_t GetCallState() override;

    /**
     * HoldCall
     *
     * @brief Park a phone call
     * @param callId[in], call id
     * @return Returns 0 on success, others on failure.
     */
    int32_t HoldCall(int32_t callId) override;

    /**
     * UnHoldCall
     *
     * @brief Activate a phone call
     * @param callId[in], call id
     * @return Returns 0 on success, others on failure.
     */
    int32_t UnHoldCall(int32_t callId) override;

    /**
     * SwitchCall
     *
     * @brief Switch the phone
     * @param callId[in], call id
     * @return Returns 0 on success, others on failure.
     */
    int32_t SwitchCall(int32_t callId) override;

    /**
     * HasCall
     *
     * @brief Is there Call
     * @return Returns true on has call, others on there is no call.
     */
    bool HasCall() override;

    /**
     * IsNewCallAllowed
     *
     * @brief Can I initiate a call
     * @param enabled[out], true on can, others on there is not can
     * @return Returns interface processing results.
     */
    int32_t IsNewCallAllowed(bool &enabled) override;

    /**
     * SetMuted
     *
     * @brief Mute the Microphone
     * @param isMute[in], mute state
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetMuted(bool isMute) override;

    /**
     * MuteRinger
     *
     * @brief Call mute
     * @return Returns 0 on success, others on failure.
     */
    int32_t MuteRinger() override;

    /**
     * SetAudioDevice
     *
     * @brief Setting the Audio Channel
     * @param deviceType[in], audio device type
     * @param bluetoothAddress[in], bluetooth device Address
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetAudioDevice(AudioDevice deviceType, const std::string &bluetoothAddress) override;

    /**
     * IsRinging
     *
     * @brief Whether the ringing
     * @param enabled[out], true on ringing, false on there is no ringing
     * @return Returns interface processing results.
     */
    int32_t IsRinging(bool &enabled) override;

    /**
     * IsInEmergencyCall
     *
     * @brief Is there an emergency call
     * @param enabled[out], true on emergency call, false on no emergency call
     * @return Returns interface processing results.
     */
    int32_t IsInEmergencyCall(bool &enabled) override;

    /**
     * StartDtmf
     *
     * @brief Enable and send DTMF
     * @param callId[in], call id
     * @param str[in], Characters sent
     * @return Returns 0 on success, others on failure.
     */
    int32_t StartDtmf(int32_t callId, char str) override;

    /**
     * StopDtmf
     *
     * @brief Stop the DTMF
     * @param callId[in], call id
     * @return Returns 0 on success, others on failure.
     */
    int32_t StopDtmf(int32_t callId) override;

    /**
     * GetCallWaiting
     *
     * @brief Gets whether the call waiting service of the current account is enabled
     * @param slotId[in], The slot id
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetCallWaiting(int32_t slotId) override;

    /**
     * SetCallWaiting
     *
     * @brief Set the call waiting function for the current account
     * @param slotId[in], The slot id
     * @param activate[in], Activation of switch
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetCallWaiting(int32_t slotId, bool activate) override;

    /**
     * GetCallRestriction
     *
     * @brief Gets the call restriction information of the specified account
     * @param slotId[in], The slot id
     * @param type[in], Call Restriction type
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetCallRestriction(int32_t slotId, CallRestrictionType type) override;

    /**
     * SetCallRestriction
     *
     * @brief Set the call restriction function for the current account
     * @param slotId[in], The slot id
     * @param info[in], Call restriction information
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetCallRestriction(int32_t slotId, CallRestrictionInfo &info) override;

    /**
     * GetCallTransferInfo
     *
     * @brief Gets the call transfer information of the current account
     * @param slotId[in], The slot id
     * @param type[in], Call Transfer Type
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type) override;

    /**
     * SetCallTransferInfo
     *
     * @brief Set the call transfer function for the current account
     * @param slotId[in], The slot id
     * @param info[in], Call Transfer Information
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetCallTransferInfo(int32_t slotId, CallTransferInfo &info) override;

    /**
     * IsSupportCallTransferTime
     *
     * @brief confirm whether IMS support call transfer due to time.
     * @param slotId[in], The slot id
     * @param result[out], The result of support or not
     * @return Returns true on success, others on failure.
     */
    int32_t IsSupportCallTransferTime(int32_t slotId, bool &result) override;

    /**
     * CombineConference
     *
     * @brief Merge calls to form a conference
     * @param callId[in], call id
     * @return Returns 0 on success, others on failure.
     */
    int32_t CombineConference(int32_t mainCallId) override;

    /**
     * SeparateConference
     *
     * @brief Separates a specified call from a conference call
     * @param callId[in], call id
     * @return Returns 0 on success, others on failure.
     */
    int32_t SeparateConference(int32_t callId) override;

    /**
     * ControlCamera
     *
     * @brief Open or close camera
     * @param cameraId[in], The camera id
     * @return Returns 0 on success, others on failure.
     */
    int32_t ControlCamera(std::u16string cameraId) override;

    /**
     * SetPreviewWindow
     *
     * @brief Set the location and size of the preview window for videos captured by the local camera.
     * @param window[in], Window information
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetPreviewWindow(VideoWindow &window) override;

    /**
     * SetDisplayWindow
     *
     * @brief Sets the location and size of the remote video window.
     * @param window[in], Window information
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetDisplayWindow(VideoWindow &window) override;

    /**
     * SetCameraZoom
     *
     * @brief Sets the local camera zoom scale
     * @param zoomRatio[in], Camera scale
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetCameraZoom(float zoomRatio) override;

    /**
     * SetPausePicture
     *
     * @brief APP sets the screen of the remote video freeze immediately.
     * If the APP does not call this interface when making a video call,
     * the last frame before the remote video freeze is displayed by default
     * @param path[in], Local Picture address
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetPausePicture(std::u16string path) override;

    /**
     * SetDeviceDirection
     *
     * @brief Set the rotation Angle of the local device. The default value is 0
     * @param rotation[in], Rotation Angle
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetDeviceDirection(int32_t rotation) override;

    /**
     * IsEmergencyPhoneNumber
     *
     * @brief Is it an emergency call
     * @param number[in], Phone number to be formatted
     * @param slotId[in], The slot id
     * @param errorCode[out] Failed to provision the error code
     * @return Returns 0 on true, others on false.
     */
    bool IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, int32_t &errorCode) override;

    /**
     * FormatPhoneNumber
     *
     * @brief Formatting a phone number
     * @param number[in], Phone number to be formatted
     * @param countryCode[in], Country code of the phone number
     * @param formatNumber[out] Formatting a phone number
     * @return Returns 0 on success, others on failure.
     */
    int32_t FormatPhoneNumber(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) override;

    /**
     * FormatPhoneNumberToE164
     *
     * @brief Formatting a phone number
     * @param number[in]. Phone number to be formatted
     * @param countryCode[in], Country code of the phone number
     * @param formatNumber[out] Formatting a phone number
     * @return Returns 0 on success, others on failure.
     */
    int32_t FormatPhoneNumberToE164(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) override;

    /**
     * GetMainCallId
     *
     * @brief Obtain the ID of the primary call in a conference
     * @param callId[in], Id of a call in a conference
     * @return Returns main call id, -1 on not call id.
     */
    int32_t GetMainCallId(int32_t callId, int32_t &mainCallId) override;

    /**
     * GetSubCallIdList
     *
     * @brief Obtain the list of neutron call ids
     * @param callId[in], Id of a call in a conference
     * @param callIdList[out], the list of neutron call ids
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList) override;

    /**
     * GetCallIdListForConference
     *
     * @brief Obtain the callId list of all calls in a conference
     * @param callId[in], Id of a call in a conference
     * @param callIdList[out], the callId list of all calls in a conference
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList) override;

    /**
     * SetCallPreferenceMode
     *
     * @brief Setting the Call Type
     * @param slotId[in], The slot id
     * @param mode[in], Preference Mode
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetCallPreferenceMode(int32_t slotId, int32_t mode) override;

    /**
     * GetImsConfig
     *
     * @brief Obtain the IMS service configuration
     * @param slotId[in], The slot id
     * @param item[in]
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetImsConfig(int32_t slotId, ImsConfigItem item) override;

    /**
     * SetImsConfig
     *
     * @brief Example Set the IMS service configuration
     * @param slotId[in], The slot id
     * @param item[in]
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value) override;

    /**
     * GetImsFeatureValue
     *
     * @brief Gets the value of the IMS function item of the specified network type
     * @param slotId[in], The slot id
     * @param info[in], FeatureType
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetImsFeatureValue(int32_t slotId, FeatureType type) override;

    /**
     * SetImsFeatureValue
     *
     * @brief Set the value of the IMS function item of the specified network type
     * @param slotId[in], The slot id
     * @param info[in], FeatureType
     * @param value[in]
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value) override;

    /**
     * UpdateImsCallMode
     *
     * @brief Setting the Call Mode
     * @param callId[in], The call id
     * @param mode[in], Calling patterns
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateImsCallMode(int32_t callId, ImsCallMode mode) override;

    /**
     * EnableImsSwitch
     *
     * @brief Start VoLte
     * @param slotId[in], The slot id
     * @return Returns 0 on success, others on failure.
     */
    int32_t EnableImsSwitch(int32_t slotId) override;

    /**
     * DisableImsSwitch
     *
     * @brief Stop VoLte
     * @param slotId[in], The slot id
     * @return Returns 0 on success, others on failure.
     */
    int32_t DisableImsSwitch(int32_t slotId) override;

    /**
     * IsImsSwitchEnabled
     *
     * @brief Whether to enable VoLte
     * @param slotId[in], The slot id
     * @param enabled[out], The result of enable or not
     * @return Returns 0 on success, others on failure.
     */
    int32_t IsImsSwitchEnabled(int32_t slotId, bool &enabled) override;

    /**
     * StartRtt
     *
     * @brief Enable and send RTT information
     * @param callId[in], The call id
     * @param msg[in], RTT information
     * @return Returns 0 on success, others on failure.
     */
    int32_t StartRtt(int32_t callId, std::u16string &msg) override;

    /**
     * StopRtt
     *
     * @brief Close the RTT
     * @param callId[in], The call id
     * @return Returns 0 on success, others on failure.
     */
    int32_t StopRtt(int32_t callId) override;

    /**
     * JoinConference
     *
     * @brief Bring someone into a meeting
     * @param callId[in], The call id
     * @param numberList[in], List of calls to join the conference
     * @return Returns 0 on success, others on failure.
     */
    int32_t JoinConference(int32_t callId, std::vector<std::u16string> &numberList) override;

    /**
     * ReportOttCallDetailsInfo
     *
     * @brief report ott call details info
     * @param ottVec[in], ott call status detail info list
     * @return Returns 0 on success, others on failure.
     */
    int32_t ReportOttCallDetailsInfo(std::vector<OttCallDetailsInfo> &ottVec) override;

    /**
     * ReportOttCallEventInfo
     *
     * @brief report ott call details info
     * @param ottVec[in], ott call status detail info list
     * @return Returns 0 on success, others on failure.
     */
    int32_t ReportOttCallEventInfo(OttCallEventInfo &eventInfo) override;

    /**
     * GetProxyObjectPtr
     *
     * @brief get callManager proxy object ptr
     * @param proxyType[in], proxy type
     * @return Returns nullptr on failure, others on success.
     */
    sptr<IRemoteObject> GetProxyObjectPtr(CallManagerProxyType proxyType) override;

private:
    std::string GetBundleName();

private:
    enum ServiceRunningState {
        STATE_STOPPED = 0,
        STATE_RUNNING,
    };

    ServiceRunningState state_ {ServiceRunningState::STATE_STOPPED};

    std::shared_ptr<CallControlManager> callControlManagerPtr_;
    std::map<uint32_t, sptr<IRemoteObject>> proxyObjectPtrMap_;
    std::mutex lock_;
    const int32_t startTime_ = 1900;
    const int32_t extraMonth_ = 1;
    int64_t bindTime_ = 0L;
    int32_t spendTime_ = 0;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_MANAGER_SERVICE_H
