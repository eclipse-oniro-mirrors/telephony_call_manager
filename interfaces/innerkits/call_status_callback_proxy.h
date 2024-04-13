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

#ifndef CALL_STATUS_CALLBACK_PROXY_H
#define CALL_STATUS_CALLBACK_PROXY_H

#include "call_status_callback_ipc_interface_code.h"
#include "iremote_proxy.h"
#include "i_call_status_callback.h"

namespace OHOS {
namespace Telephony {
class CallStatusCallbackProxy : public IRemoteProxy<ICallStatusCallback> {
public:
    /**
     * @brief Construct a new CallStatusCallbackProxy object
     *
     * @param impl
     */
    explicit CallStatusCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~CallStatusCallbackProxy() = default;

    /**
     * @brief update the call details info
     *
     * @param info[in] call info, contains phone number, call type, call state, call mode, voice domain, account id .etc
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateCallReportInfo(const CallReportInfo &info) override;

    /**
     * @brief update the call details info list
     *
     * @param info[in] calls info, contains slot id and a CallReportInfo vector
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateCallsReportInfo(const CallsReportInfo &info) override;

    /**
     * @brief update the call disconnect reason
     *
     * @param details[in], contains DisconnectedReason and the corresponding message
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateDisconnectedCause(const DisconnectedDetails &details) override;

    /**
     * @brief update the event result
     *
     * @param info[in], contains RequestResultEventId and CellularCallEventType
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateEventResultInfo(const CellularCallEventInfo &info) override;

    /**
     * @brief update the rbtplay info
     *
     * @param info[in], 0: NETWORK_ALERTING, 1: LOCAL_ALERTING
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateRBTPlayInfo(const RBTPlayInfo info) override;

    /**
     * @brief update the response of get call waiting
     *
     * @param callWaitResponse[in], contains result, status and calssCw
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateGetWaitingResult(const CallWaitResponse &callWaitResponse) override;

    /**
     * @brief update the response of set call waiting
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateSetWaitingResult(const int32_t result) override;

    /**
     * @brief update the response of Get Restriction
     *
     * @param callRestrictionResult[in], contains result, status and calssCw
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateGetRestrictionResult(const CallRestrictionResponse &callRestrictionResult) override;

    /**
     * @brief update the response of Set Restriction
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateSetRestrictionResult(const int32_t result) override;

    /**
     * @brief update the response of Set Restriction Password
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateSetRestrictionPasswordResult(const int32_t result) override;

    /**
     * @brief update the response of Get Transfer
     *
     * @param callTransferResponse[in], contains result, status, calssx, reason, number, time .etc
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateGetTransferResult(const CallTransferResponse &callTransferResponse) override;

    /**
     * @brief update the response of Set Transfer
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateSetTransferResult(const int32_t result) override;

    /**
     * @brief update the response of Get CallClip
     *
     * @param clipResponse[in], contains result, action, clipStat
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateGetCallClipResult(const ClipResponse &clipResponse) override;

    /**
     * @brief update the response of Get CallClir
     *
     * @param clirResponse[in], contains result, action, clirStat
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateGetCallClirResult(const ClirResponse &clirResponse) override;

    /**
     * @brief update the result of Update Set CallClir
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateSetCallClirResult(const int32_t result) override;

    /**
     * @brief update the result of Start Rtt
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t StartRttResult(const int32_t result) override;

    /**
     * @brief update the result of stop rtt
     *
     * @param result[in], the Stop Rtt Result, 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t StopRttResult(const int32_t result) override;

    /**
     * @brief update the response of get ims config
     *
     * @param response[in], contains result and value
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetImsConfigResult(const GetImsConfigResponse &response) override;

    /**
     * @brief update the result of SetImsConfig
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetImsConfigResult(const int32_t result) override;

    /**
     * @brief update the response of GetImsFeatureValue
     *
     * @param response[in], contains result and value
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetImsFeatureValueResult(const GetImsFeatureValueResponse &response) override;

    /**
     * @brief update the result of SetImsFeatureValue
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t SetImsFeatureValueResult(const int32_t result) override;

    /**
     * @brief update the result of ReceiveUpdateCallMediaMode
     *
     * @param response[in], indicates the Call Media mode response information
     * @return Returns 0 on success, others on failure.
     */
    int32_t ReceiveUpdateCallMediaModeRequest(const CallModeReportInfo &response) override;

    /**
     * @brief update the result of ReceiveUpdateCallMediaMode
     *
     * @param response[in], indicates the Call Media mode response information
     * @return Returns 0 on success, others on failure.
     */
    int32_t ReceiveUpdateCallMediaModeResponse(const CallModeReportInfo &response) override;

    /**
     * @brief update the result of invite to conference
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t InviteToConferenceResult(const int32_t result) override;

    /**
     * @brief update the result of start dtmf
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t StartDtmfResult(const int32_t result) override;

    /**
     * @brief update the result of stop dtmf
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t StopDtmfResult(const int32_t result) override;

    /**
     * @brief update the result of send ussd
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t SendUssdResult(const int32_t result) override;

    /**
     * @brief update the result of GetImsCallData
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t GetImsCallDataResult(const int32_t result) override;

    /**
     * @brief update the result of send mmi code
     *
     * @param info[in], contains result and message
     * @return Returns 0 on success, others on failure.
     */
    int32_t SendMmiCodeResult(const MmiCodeInfo &info) override;

    /**
     * @brief update the result of Close UnFinished Ussd
     *
     * @param result[in], 0 means the result is success, others is failure
     * @return Returns 0 on success, others on failure.
     */
    int32_t CloseUnFinishedUssdResult(const int32_t result) override;

    int32_t ReportPostDialChar(const std::string &c) override;

    int32_t ReportPostDialDelay(const std::string &str) override;

    /**
     * @brief handle call session event changed
     *
     * @param eventOptions[in], call session event info
     * @return Returns 0 on success, others on failure.
     */
    int32_t HandleCallSessionEventChanged(const CallSessionReportInfo &eventOptions) override;

    /**
     * @brief handle peer dimensions changed
     *
     * @param dimensionsDetail[in], peer dimensions info
     * @return Returns 0 on success, others on failure.
     */
    int32_t HandlePeerDimensionsChanged(const PeerDimensionsReportInfo &dimensionsDetail) override;

    /**
     * @brief handle call data usage changed
     *
     * @param result[in], call data usage
     * @return Returns 0 on success, others on failure.
     */
    int32_t HandleCallDataUsageChanged(const int64_t result) override;

    /**
     * @brief handle camera capabilities changed
     *
     * @param cameraCapabilities[in], camera capabilities info
     * @return Returns 0 on success, others on failure.
     */
    int32_t HandleCameraCapabilitiesChanged(const CameraCapabilitiesReportInfo &cameraCapabilities) override;

    /**
     * @brief update the voip call event
     *
     * @param info[in], contains voipCallEvent
     * @return Returns 0 on success, others on failure.
     */
    int32_t UpdateVoipEventInfo(const VoipCallEventInfo &info) override;

private:
    static inline BrokerDelegator<CallStatusCallbackProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS

#endif
