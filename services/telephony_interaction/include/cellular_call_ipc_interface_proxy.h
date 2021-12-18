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

#ifndef CELLULAR_CALL_IPC_INTERFACE_PROXY_H
#define CELLULAR_CALL_IPC_INTERFACE_PROXY_H

#include <mutex>

#include "if_system_ability_manager.h"
#include "refbase.h"
#include "rwlock.h"
#include "singleton.h"

#include "call_status_callback.h"
#include "cellular_call_interface.h"
#include "i_call_status_callback.h"
#include "timer.h"

namespace OHOS {
namespace Telephony {
class CellularCallIpcInterfaceProxy : public Timer,
                                      public std::enable_shared_from_this<CellularCallIpcInterfaceProxy> {
    DECLARE_DELAYED_SINGLETON(CellularCallIpcInterfaceProxy)
public:
    void Init(int32_t systemAbilityId);
    void unInit();
    static void task();
    /**
     * Call management diale interface
     *
     * @param CallInfo, dial param.
     * @return Returns 0 on success, others on failure.
     */
    int Dial(const CellularCallInfo &callInfo);

    /**
     * HangUp.
     *
     * @param CallInfo, HangUp param.
     * @return Returns 0 on success, others on failure.
     */
    int HangUp(const CellularCallInfo &callInfo);

    /**
     * Reject.
     *
     * @param CallInfo, Reject param.
     * @return Returns 0 on success, others on failure.
     */
    int Reject(const CellularCallInfo &callInfo);

    /**
     * Answer.
     *
     * @param CallInfo, Answer param.
     * @return Returns 0 on success, others on failure.
     */
    int Answer(const CellularCallInfo &callInfo);

    /**
     * Hold.
     *
     * @param CellularCallInfo, Hold param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int HoldCall(const CellularCallInfo &callInfo);

    /**
     * UnHold.
     *
     * @param CellularCallInfo, UnHold param.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int UnHoldCall(const CellularCallInfo &callInfo);

    /**
     * Switch.
     *
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int SwitchCall();

    /**
     * IsEmergencyPhoneNumber
     *
     * @brief Is it an emergency call
     * @param string &phoneNum
     * @param slotId
     * @return Returns 0 on success, others on failure.
     */
    int IsEmergencyPhoneNumber(const std::string &phoneNum, int32_t slotId, int32_t &errorCode);

    /**
     * Merge into multiple calls
     *
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int CombineConference(const CellularCallInfo &callInfo);

    /**
     * SeparateConference.
     *
     * @param std::string splitString
     * @param index
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int SeparateConference(const CellularCallInfo &callInfo);

    /**
     * CallSupplement.
     *
     * @param CallSupplementType type
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int CallSupplement(CallSupplementType type);

    /**
     * Start Dtmf.
     *
     * @param DTMF Code.
     * @param CellularCallInfo callInfo.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int StartDtmf(char cDTMFCode, const CellularCallInfo &callInfo);

    /**
     * Stop Dtmf.
     *
     * @param CellularCallInfo callInfo.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int StopDtmf(const CellularCallInfo &callInfo);

    /**
     * Send Dtmf.
     *
     * @param DTMF Code.
     * @param phoneNum.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int SendDtmf(char cDTMFCode, const std::string &phoneNum);

    /**
     * Send DTMF String.
     *
     * @param DTMF Code string.
     * @param phoneNum.
     * @param switchOn.
     * @param switchOff.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int SendDtmfString(const std::string &dtmfCodeStr, const std::string &phoneNum, PhoneNetType phoneNetType,
        int32_t switchOn, int32_t switchOff);

    /**
     * Set Call Transfer Info
     *
     * @param CallForwardInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int SetCallTransferInfo(const CallTransferInfo &info, int32_t slotId);

    /**
     * Get Call Transfer Info
     *
     * @param CallForwardInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int GetCallTransferInfo(CallTransferType type, int32_t slotId);

    /**
     * Set Call Waiting
     *
     * @param activate
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int SetCallWaiting(bool activate, int32_t slotId);

    /**
     * Inquire Call Waiting
     *
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int GetCallWaiting(int32_t slotId);

    /**
     * Set Call Restriction
     *
     * @param CallBarringInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int SetCallRestriction(const CallRestrictionInfo &info, int32_t slotId);

    /**
     * Get Call Restriction
     *
     * @param CallBarringInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int GetCallRestriction(CallRestrictionType facType, int32_t slotId);

    /**
     * SetCallPreferenceMode
     *
     * @param mode
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int SetCallPreferenceMode(int32_t slotId, int32_t mode);

    /**
     * RegisterCallBack
     *
     * @return Returns 0 on success, others on failure.
     */
    int RegisterCallBack(const sptr<ICallStatusCallback> &callback);

    /**
     * open or close Camera
     *
     * @param cameraId
     * @param callingPackage
     * @param callingUid
     * @param callingPid
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t ControlCamera(
        std::u16string cameraId, std::u16string callingPackage, int32_t callingUid, int32_t callingPid);

    /**
     * Set Camera Preview Window
     *
     * @param window
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetPreviewWindow(VideoWindow &window);

    /**
     * set remote display video window
     *
     * @param window
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetDisplayWindow(VideoWindow &window);

    /**
     * Set Camera Zoom
     *
     * @param zoomRatio
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetCameraZoom(float zoomRatio);

    /**
     * Set Pause Picture
     *
     * @param path
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetPausePicture(std::u16string path);

    /**
     * Set Device Direction
     *
     * @param rotation
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetDeviceDirection(int32_t rotation);

    /**
     * Set Lte Ims Switch Status
     *
     * @param active
     * @return Returns kTelephonyNoErr on success, others on failure.
     */
    int32_t SetLteImsSwitchStatus(int32_t slotId, bool active);

    /**
     * Get Lte Ims Switch Status
     *
     * @return Returns kTelephonyNoErr on success, others on failure.
     */
    int32_t GetLteImsSwitchStatus(int32_t slotId);

private:
    int32_t ConnectService();
    int32_t RegisterCallBackFun();
    void DisconnectService();
    int32_t ReConnectService();
    void OnDeath();
    void Clean();
    void NotifyDeath();

private:
    int32_t systemAbilityId_;
    sptr<ICallStatusCallback> cellularCallCallbackPtr_;
    sptr<CellularCallInterface> cellularCallInterfacePtr_;
    sptr<IRemoteObject::DeathRecipient> cellularCallRecipient_;
    bool connectState_;
    Utils::RWLock rwClientLock_;
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS

#endif
