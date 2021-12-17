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

#include "call_request_process.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "cs_call.h"
#include "ims_call.h"
#include "ott_call.h"
#include "common_type.h"
#include "call_control_manager.h"
#include "cellular_call_ipc_interface_proxy.h"

namespace OHOS {
namespace Telephony {
CallRequestProcess::CallRequestProcess() {}

CallRequestProcess::~CallRequestProcess() {}

void CallRequestProcess::DialRequest()
{
    DialParaInfo info;
    DelayedSingleton<CallControlManager>::GetInstance()->GetDialParaInfo(info);
    if (!info.isDialing) {
        TELEPHONY_LOGE("the device is not dialing!");
        return;
    }
    TELEPHONY_LOGI("dialType:%{public}d", info.dialType);
    switch (info.dialType) {
        case DialType::DIAL_CARRIER_TYPE:
            CarrierDialProcess(info);
            break;
        case DialType::DIAL_VOICE_MAIL_TYPE:
            VoiceMailDialProcess(info);
            break;
        case DialType::DIAL_OTT_TYPE:
            break;
        default:
            TELEPHONY_LOGE("invalid dialType:%{public}d", info.dialType);
            break;
    }
}

void CallRequestProcess::AnswerRequest(int32_t callId, int32_t videoState)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    int32_t ret = call->AnswerCall(videoState);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("AnswerCall failed!");
        return;
    }
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyIncomingCallAnswered(call);
}

void CallRequestProcess::RejectRequest(int32_t callId, bool isSendSms, std::string &content)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }

    int32_t ret = call->RejectCall(isSendSms, content);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RejectCall failed!");
        return;
    }
    DelayedSingleton<CallControlManager>::GetInstance()->NotifyIncomingCallRejected(call, isSendSms, content);
    TELEPHONY_LOGI("start to send reject message...");
}

void CallRequestProcess::HangUpRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    TelCallState state = call->GetTelCallState();
    if ((state == CALL_STATUS_ACTIVE) &&
        (CallObjectManager::IsCallExist(call->GetCallType(), TelCallState::CALL_STATUS_HOLDING))) {
        TELEPHONY_LOGI("release the active call and recover the held call");
        call->SetPolicyFlag(PolicyFlag::POLICY_FLAG_HANG_UP_ACTIVE);
    }
    call->HangUpCall();
}

void CallRequestProcess::HoldRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    call->HoldCall();
}

void CallRequestProcess::UnHoldRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    call->UnHoldCall();
}

void CallRequestProcess::SwitchRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    call->SwitchCall();
}

void CallRequestProcess::CombineConferenceRequest(int32_t mainCallId)
{
    sptr<CallBase> call = GetOneCallObject(mainCallId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, mainCallId:%{public}d", mainCallId);
        return;
    }
    int32_t ret = call->CombineConference();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CombineConference failed");
    }
}

void CallRequestProcess::SeparateConferenceRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
    int32_t ret = call->SeparateConference();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SeparateConference failed");
    }
}

void CallRequestProcess::UpgradeCallRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
#ifdef ABILITY_VIDEO_SUPPORT
    DelayedSingleton<VideoControlManager>::GetInstance()->CallDestroyed(call);
#endif
    call->SetVideoStateType(VideoStateType::TYPE_VOICE);
}

void CallRequestProcess::DowngradeCallRequest(int32_t callId)
{
    sptr<CallBase> call = GetOneCallObject(callId);
    if (call == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return;
    }
#ifdef ABILITY_VIDEO_SUPPORT
    DelayedSingleton<VideoControlManager>::GetInstance()->NewCallCreated(call);
#endif
    call->SetVideoStateType(VideoStateType::TYPE_VIDEO);
}

void CallRequestProcess::CarrierDialProcess(DialParaInfo &info)
{
    CellularCallInfo callInfo;
    PackCellularCallInfo(info, callInfo);
    // Obtain gateway information
    int32_t ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->Dial(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Dial failed!");
        return;
    }
}

void CallRequestProcess::VoiceMailDialProcess(DialParaInfo &info)
{
    CellularCallInfo callInfo;
    PackCellularCallInfo(info, callInfo);
    int32_t ret = DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->Dial(callInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Dial VoiceMail failed!");
        return;
    }
}

void CallRequestProcess::PackCellularCallInfo(DialParaInfo &info, CellularCallInfo &callInfo)
{
    callInfo.callId = info.callId;
    callInfo.accountId = info.accountId;
    callInfo.callType = info.callType;
    callInfo.videoState = info.videoState;
    callInfo.index = info.index;
    callInfo.slotId = info.accountId;
    (void)memset_s(callInfo.phoneNum, kMaxNumberLen, 0, kMaxNumberLen);
    if (memcpy_s(callInfo.phoneNum, kMaxNumberLen, info.number.c_str(), info.number.length()) != 0) {
        TELEPHONY_LOGE("memcpy_s failed!");
        return;
    }
}
} // namespace Telephony
} // namespace OHOS
