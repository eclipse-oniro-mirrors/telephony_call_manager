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

#ifndef I_VOIP_CALL_MANAGER_SERVICE_H
#define I_VOIP_CALL_MANAGER_SERVICE_H
#include <cstdio>
#include <string>
#include <vector>

#include "i_call_status_callback.h"
#include "i_voip_call_manager_callback.h"
#include "iremote_broker.h"
#include "pac_map.h"
#include "voip_call_manager_info.h"

namespace OHOS {
namespace Telephony {

class IVoipCallManagerService : public IRemoteBroker {
public:
    enum {
        INTERFACE_REPORT_INCOMING_CALL = 0,
        INTERFACE_REPORT_INCOMING_CALL_ERROR,
        INTERFACE_REPORT_CALL_STATE_CHANGE,
        INTERFACE_REGISTER_CALLBACK,
        INTERFACE_UN_REGISTER_CALLBACK,
        INTERFACE_REPORT_VOIP_INCOMING_CALL,
        INTERFACE_REPORT_VOIP_CALL_EXTENSIONID,
        INTERFACE_REGISTER_CALL_MANAGER_CALLBACK,
        INTERFACE_UNREGISTER_CALL_MANAGER_CALLBACK,
        INTERFACE_ANSWER_VOIP_CALL,
        INTERFACE_HANGUP_VOIP_CALL,
        INTERFACE_REJECT_VOIP_CALL,
        INTERFACE_UNLOAD_VOIP_SA,
        INTERFACE_SEND_CALL_UI_EVENT,
        INTERFACE_REPORT_CALL_AUDIO_EVENT_CHANGE,
        INTERFACE_REPORT_OUTGOING_CALL
    };
    virtual ~IVoipCallManagerService() = default;
    virtual int32_t ReportIncomingCall(
        AppExecFwk::PacMap &extras, std::vector<uint8_t> &userProfile, ErrorReason &reason) = 0;
    virtual int32_t ReportIncomingCallError(AppExecFwk::PacMap &extras) = 0;
    virtual int32_t ReportCallStateChange(std::string callId, const VoipCallState &state) = 0;
    virtual int32_t RegisterCallBack(const sptr<IVoipCallManagerCallback> &callback) = 0;
    virtual int32_t UnRegisterCallBack() = 0;
    virtual int32_t ReportVoipIncomingCall(
        std::string callId, std::string bundleName, std::string processMode, int32_t uid) = 0;
    virtual int32_t ReportVoipCallExtensionId(
        std::string callId, std::string bundleName, std::string extensionId, int32_t uid) = 0;

    virtual int32_t RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback) = 0;
    virtual int32_t UnRegisterCallManagerCallBack() = 0;
    virtual int32_t Answer(const VoipCallEventInfo &events, int32_t videoState) = 0;
    virtual int32_t HangUp(const VoipCallEventInfo &events) = 0;
    virtual int32_t Reject(const VoipCallEventInfo &events) = 0;
    virtual int32_t UnloadVoipSa() = 0;
    virtual int32_t SendCallUiEvent(std::string voipCallId, const CallAudioEvent &callAudioEvent) = 0;
    virtual int32_t ReportCallAudioEventChange(std::string voipCallId, const CallAudioEvent &callAudioEvent) = 0;
    virtual int32_t ReportOutgoingCall(
        AppExecFwk::PacMap &extras, std::vector<uint8_t> &userProfile, ErrorReason &reason) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.IVoipCallManagerService");
};
} // namespace Telephony
} // namespace OHOS

#endif
