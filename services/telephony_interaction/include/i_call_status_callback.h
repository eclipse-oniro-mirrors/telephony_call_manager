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

#ifndef I_CALL_STATUS_CALLBACK_H
#define I_CALL_STATUS_CALLBACK_H

#include "iremote_broker.h"
#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
class ICallStatusCallback : public IRemoteBroker {
public:
    virtual ~ICallStatusCallback() = default;

    virtual int32_t UpdateCallReportInfo(const CallReportInfo &info) = 0;
    virtual int32_t UpdateCallsReportInfo(const CallsReportInfo &info) = 0;
    virtual int32_t UpdateDisconnectedCause(const DisconnectedDetails &details) = 0;
    virtual int32_t UpdateEventResultInfo(const CellularCallEventInfo &info) = 0;
    virtual int32_t UpdateRBTPlayInfo(const RBTPlayInfo info) = 0;
    virtual int32_t UpdateGetWaitingResult(const CallWaitResponse &callWaitResponse) = 0;
    virtual int32_t UpdateSetWaitingResult(const int32_t result) = 0;
    virtual int32_t UpdateGetRestrictionResult(const CallRestrictionResponse &callRestrictionResult) = 0;
    virtual int32_t UpdateSetRestrictionResult(int32_t result) = 0;
    virtual int32_t UpdateGetTransferResult(const CallTransferResponse &callTransferResponse) = 0;
    virtual int32_t UpdateSetTransferResult(const int32_t result) = 0;
    virtual int32_t UpdateGetCallClipResult(const ClipResponse &clipResponse) = 0;
    virtual int32_t UpdateGetCallClirResult(const ClirResponse &clirResponse) = 0;
    virtual int32_t UpdateSetCallClirResult(const int32_t result) = 0;
    virtual int32_t StartRttResult(const int32_t result) = 0;
    virtual int32_t StopRttResult(const int32_t result) = 0;
    virtual int32_t GetImsConfigResult(const GetImsConfigResponse &response) = 0;
    virtual int32_t SetImsConfigResult(const int32_t result) = 0;
    virtual int32_t GetImsFeatureValueResult(const GetImsFeatureValueResponse &response) = 0;
    virtual int32_t SetImsFeatureValueResult(const int32_t result) = 0;
    virtual int32_t ReceiveUpdateCallMediaModeResponse(const CallMediaModeResponse &response) = 0;
    virtual int32_t InviteToConferenceResult(const int32_t result) = 0;
    virtual int32_t StartDtmfResult(const int32_t result) = 0;
    virtual int32_t StopDtmfResult(const int32_t result) = 0;
    virtual int32_t SendUssdResult(const int32_t result) = 0;
    virtual int32_t GetImsCallDataResult(const int32_t result) = 0;
    virtual int32_t SendMmiCodeResult(const MmiCodeInfo &info) = 0;

    enum CallManagerCallStatusCode {
        UPDATE_CALL_INFO = 0,
        UPDATE_CALLS_INFO,
        UPDATE_DISCONNECTED_CAUSE,
        UPDATE_EVENT_RESULT_INFO,
        UPDATE_RBT_PLAY_INFO,
        UPDATE_GET_WAITING,
        UPDATE_SET_WAITING,
        UPDATE_GET_RESTRICTION,
        UPDATE_SET_RESTRICTION,
        UPDATE_GET_TRANSFER,
        UPDATE_SET_TRANSFER,
        UPDATE_GET_CALL_CLIP,
        UPDATE_GET_CALL_CLIR,
        UPDATE_SET_CALL_CLIR,
        GET_IMS_SWITCH_STATUS,
        SET_IMS_SWITCH_STATUS,
        UPDATE_STARTRTT_STATUS,
        UPDATE_STOPRTT_STATUS,
        GET_IMS_CONFIG,
        SET_IMS_CONFIG,
        GET_IMS_FEATURE_VALUE,
        SET_IMS_FEATURE_VALUE,
        GET_LTE_ENHANCE_MODE,
        SET_LTE_ENHANCE_MODE,
        RECEIVE_UPDATE_MEDIA_MODE_RESPONSE,
        INVITE_TO_CONFERENCE,
        START_DTMF,
        STOP_DTMF,
        SEND_USSD,
        GET_IMS_CALL_DATA,
        MMI_CODE_INFO_RESPONSE,
    };

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.ICallStatusCallback");
};
} // namespace Telephony
} // namespace OHOS

#endif
